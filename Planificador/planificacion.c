/*
 * planificacion.c
 *
 *  Created on: 17 may. 2018
 *      Author: utnso
 */

#include "planificacion.h"

/* SE DEBE REPLANIFICAR CUANDO:
 *
 * SI USO HRRN/SJF SIN DESALOJO:
 * -SE BLOQUEA UN PROCESO
 * -FINALIZA UN PROCESO
 *
 * SI USO SJF
 * -SE BLOQUEA UN PROCESO
 * -FINALIZA UN PROCESO
 * -NUEVO PROCESO LLEGA (o uno se desbloquea)
 */

int flag = 0;
bool primera_vez = false;
bool hay_nuevo_esi = false;
bool hay_esi_bloqueado = false;
bool hay_esi_finalizado = false;
t_esi* esi_a_matar;

void* planificar(void* nada) {

	while (1) {
		sem_wait(&sem_binario_planif);

		pthread_mutex_lock(&mutex_flag_pausa_despausa);
		if (flag == 0) { //esta despausada la planificacion
			pthread_mutex_unlock(&mutex_flag_pausa_despausa);

			t_esi* proximo_esi;
			if (configuracion.algoritmo == SJFcD) { //planifico por desalojo
				if (primera_vez || hay_nuevo_esi || hay_esi_bloqueado
						|| hay_esi_finalizado) { //ocurrio un evento de replanificacion (en alg. con desalojo)
					/*DESALOJO AL ESI: (lo paso de nuevo a listos,
					 dejo el puntero con esi corriendo como esta, total despues se pisa)*/
					if (!primera_vez)
						nuevo_esi(esi_corriendo);
					sem_wait(&contador_esis);
					proximo_esi = obtener_nuevo_esi_a_correr();
					primera_vez = hay_nuevo_esi = hay_esi_bloqueado =
							hay_esi_finalizado = false;
				} else {
					proximo_esi = esi_corriendo; //no hubo evento de replanif
				}
			} else {

				if (primera_vez || hay_esi_bloqueado || hay_esi_finalizado) { //ocurrio un evento de replanificacion (en alg. sin desalojo)
					sem_wait(&contador_esis);
					proximo_esi = obtener_nuevo_esi_a_correr();
					primera_vez = hay_esi_bloqueado = hay_esi_finalizado =
							false;
				} else {
					proximo_esi = esi_corriendo; //no hubo evento de replanif
				}

			}

			log_debug(logger, "Proximo esi a correr: %d \n", proximo_esi->id);

			esi_corriendo = proximo_esi;
			correr(esi_corriendo);

		}

	}
	return NULL;
}

t_esi *obtener_nuevo_esi_a_correr() {
	t_esi* prox_esi;

	if (configuracion.algoritmo == FIFO) {
		prox_esi = obtener_proximo_segun_fifo(lista_esis_listos);
	} else if (configuracion.algoritmo == SJFsD) {
		prox_esi = obtener_proximo_segun_sjf(lista_esis_listos);
	} else if (configuracion.algoritmo == SJFcD) {
		prox_esi = obtener_proximo_segun_sjf(lista_esis_listos);
	} else {
		prox_esi = obtener_proximo_segun_hrrn(lista_esis_listos);
	}

	return prox_esi;

}

//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE LLEGO UN NUEVO ESI
void nuevo_esi(t_esi* esi) {
	list_add(lista_esis_listos, esi);

	log_debug(logger, "Llego/se desalojo/se desbloqueo un nuevo esi: %d \n",
			esi->id);
	sem_post(&contador_esis);
	hay_nuevo_esi = true;

}

//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE EL ESI CORRIENDO FINALIZO
void finalizar_esi() {

	list_add(lista_esis_finalizados, esi_corriendo);

	log_debug(logger, "Termino un esi: %d \n", esi_corriendo->id);

	hay_esi_finalizado = true;
	list_iterate(lista_esis_finalizados, aumentar_viene_esperando);
	sem_post(&sem_binario_planif);
}

void bloquear_esi(char* clave) {

	//No existe la clave, agrego esta nueva linea
	if (!dictionary_has_key(dic_esis_bloqueados, clave)) {
		dictionary_put(dic_esis_bloqueados, clave, esi_corriendo);

	} else { //Existe la clave, agrego el esi a la lista de bloq

		t_list *lista_esis_bloq_esta_clave = dictionary_remove(
				dic_esis_bloqueados, clave);
		list_add(lista_esis_bloq_esta_clave, esi_corriendo);
		dictionary_put(dic_esis_bloqueados, clave, lista_esis_bloq_esta_clave);

	}

	log_debug(logger, "Se bloqueo el esi: %d para la clave: %s \n",
			esi_corriendo->id, clave);
	hay_esi_bloqueado = true;
}

int id;

void bloquear_esi_por_consola(char* clave, int id_esi) {

	id = id_esi;

	t_esi *esi_afectado = buscar_esi_por_id(id_esi);

	//No existe la clave, agrego esta nueva linea
	if (!dictionary_has_key(dic_esis_bloqueados, clave)) {
		dictionary_put(dic_esis_bloqueados, clave, esi_afectado);

	} else { //Existe la clave, agrego el esi a la lista de bloq

		t_list *lista_esis_bloq_esta_clave = dictionary_remove(
				dic_esis_bloqueados, clave);
		list_add(lista_esis_bloq_esta_clave, esi_afectado);
		dictionary_put(dic_esis_bloqueados, clave, lista_esis_bloq_esta_clave);

	}
	log_debug(logger, "Se bloqueo por consola el esi: %d para la clave: %s \n",
			esi_corriendo->id, clave);

}

void desbloquear_por_consola(char* clave) {
	se_desbloqueo_un_recurso(clave);
}

t_esi *buscar_esi_por_id(int id_esi) {

	t_esi *esi_a_devolver;
	//me fijo si es el esi que esta corriendo
	if (esi_corriendo->id == id) {
		esi_a_devolver = esi_corriendo;

	} else {
		//si no es el q esta corriendo lo busco en la lista de esis listos
		esi_a_devolver = list_find(lista_esis_listos, esi_con_este_id);

	}

	return esi_a_devolver;

}

bool esi_con_este_id(void* esi) {
	return ((t_esi*) esi)->id == id;
}

//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE EL COORDINADOR LE INDICA QUE SE DESBLOQUIO UN RECURSO
void se_desbloqueo_un_recurso(char* clave) {

	t_list *lista_esis_bloq_esta_clave = dictionary_get(dic_esis_bloqueados,
			clave);
	t_esi* esi_desbloq = list_remove(lista_esis_bloq_esta_clave, 0);
	dictionary_put(dic_esis_bloqueados, clave, lista_esis_bloq_esta_clave);
	dictionary_remove(dic_clave_x_esi, clave);

	nuevo_esi(esi_desbloq);

}

//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE EL COORDINADOR LE PREGUNTA SI UN ESI TIENE UNA CLAVE
bool esi_tiene_clave(char* clave) {

	if (dictionary_has_key(dic_clave_x_esi, clave)) {
		t_esi* esi_que_la_tomo = dictionary_get(dic_clave_x_esi, clave);
		return (esi_que_la_tomo->id == esi_corriendo->id);
	} else {
		return false;
	}

}

bool esta_tomada_x_otro_la_clave(char* clave) {

	if (dictionary_has_key(dic_clave_x_esi, clave)) { //ya esta tomada?
		t_esi* esi_que_la_tomo = dictionary_get(dic_clave_x_esi, clave);

		//el esi que la tiene es el actual?
		// si es el actual -> la tiene el, esta volviendo a hacer get
		// si no es el actual la tiene otro

		return !(esi_que_la_tomo->id == esi_corriendo->id);
	}

	return false; //no esta tomada

}

//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE EL COORDINADOR ME INDICA QUE SE TOMO UNA NUEVA CLAVE
void nueva_clave_tomada_x_esi(char* clave) {

	//No existe la clave, agrego esta nueva linea
	if (!dictionary_has_key(dic_clave_x_esi, clave)) {
		dictionary_put(dic_clave_x_esi, clave, esi_corriendo); //aca estoy guardando el puntero al esi_corriendo verdad?

	} else { //Existe la clave, saco al que estaba y pongo al nuevo

		dictionary_put(dic_clave_x_esi, clave, esi_corriendo);
		//supongo que el put pisa lo que estaba

	}

}

void correr(t_esi* esi) {

	mandar_confirmacion(esi->socket);

}

void ya_termino_linea() {

	//si leyo bien la linea
	list_iterate(lista_esis_listos, aumentar_viene_esperando);
	aumentar_viene_corriendo(esi_corriendo);
	log_debug(logger, "El esi %d se termino de leer una nueva linea \n",
			esi_corriendo->id);

}

void fallo_linea() {
	//si leyo mal la linea
	log_debug(logger, "Hubo una falla cuando el esi %d leyo una nueva linea \n",
			esi_corriendo->id);
	matar_nodo_esi(esi_corriendo);
	list_iterate(lista_esis_finalizados, aumentar_viene_esperando);
}

void aumentar_viene_esperando(void* esi) {
	((t_esi*) esi)->viene_esperando = ((t_esi*) esi)->viene_esperando + 1;
}

void aumentar_viene_corriendo(void* esi) {
	((t_esi*) esi)->dur_ult_raf = ((t_esi*) esi)->dur_ult_raf + 1;
}

void liberar_recursos(t_esi* esi) {

	esi_a_matar = esi;
	dictionary_iterator(dic_clave_x_esi, liberar_claves);
	dictionary_iterator(dic_esis_bloqueados, desbloquear_claves_tomadas);
}

void liberar_claves(char* clave, void* esi) {

	if (((t_esi*) esi)->id == esi_a_matar->id) {
		dictionary_remove(dic_clave_x_esi, clave);
	}

}

void desbloquear_claves_tomadas(char* clave, void* esi) {

	if (((t_esi*) esi)->id == esi_a_matar->id) {
		se_desbloqueo_un_recurso(clave);
	}

}

void nueva_solicitud(int socket, char* clave) {

	t_protocolo cod_op;
	if (esta_tomada_x_otro_la_clave(clave)) {
		bloquear_esi(clave);
		cod_op = ERROR;

	} else {
		nueva_clave_tomada_x_esi(clave);
		cod_op = EXITO;

	}

	enviar_cod_operacion(socket, cod_op);
}

