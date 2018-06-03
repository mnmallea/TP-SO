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

flag = 0;


void planificar(){

	pthread_mutex_lock(&mutex_flag_pausa_despausa);
	if(flag==0){ //esta despausada la planificacion
	pthread_mutex_unlock(&mutex_flag_pausa_despausa);

		t_esi* proximo_esi;
		if(configuracion.algoritmo == SJFcD){ //planifico por desalojo
			if(hay_nuevo_esi || hay_esi_bloqueado || hay_esi_finalizado){ //ocurrio un evento de replanificacion (en alg. con desalojo)
				proximo_esi = obtener_nuevo_esi_a_correr();
			}else{
				proximo_esi = esi_corriendo; //no hubo evento de replanif
			}
		}else{

			if(primera_vez || hay_esi_bloqueado || hay_esi_finalizado){//ocurrio un evento de replanificacion (en alg. sin desalojo)
				proximo_esi = obtener_nuevo_esi_a_correr();
			}else{
				proximo_esi = esi_corriendo; //no hubo evento de replanif
			}

		}

	log_debug(logger, "Proximo esi a correr: %d \n", proximo_esi->id);

	esi_corriendo = proximo_esi;
	correr(esi_corriendo);

	}


}

t_esi *obtener_nuevo_esi_a_correr() {
		t_esi* prox_esi;

		if(configuracion.algoritmo == FIFO){
			prox_esi = obtener_proximo_segun_fifo(lista_esis_listos);
		}else if(configuracion.algoritmo == SJFsD){
			prox_esi = obtener_proximo_segun_sjf(lista_esis_listos);
		}else if(configuracion.algoritmo == SJFcD){
			prox_esi = obtener_proximo_segun_sjf(lista_esis_listos);
		}else{
			prox_esi = obtener_proximo_segun_hrrn(lista_esis_listos);
		}

		return prox_esi;

}



//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE LLEGO UN NUEVO ESI
void nuevo_esi(t_esi* esi){
	list_add(lista_esis_listos, esi);

	log_debug(logger, "Llego/se desbloqueo un nuevo esi: %d \n", esi->id);
	hay_nuevo_esi = true;

}

//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE EL ESI CORRIENDO FINALIZO
void finalizar_esi(){

	list_add(lista_esis_finalizados, esi_corriendo);

	log_debug(logger, "Termino un esi: %d \n", esi_corriendo->id);

	hay_esi_finalizado = true;
}

//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE EL COORDINADOR ME INDICA UN BLOQUEO
void bloquear_esi(char* clave){

	//No existe la clave, agrego esta nueva linea
	if(!dictionary_has_key(dic_esis_bloqueados, clave)){
		dictionary_put(dic_esis_bloqueados, clave, esi_corriendo);

	}else{ //Existe la clave, agrego el esi a la lista de bloq

		t_list *lista_esis_bloq_esta_clave = dictionary_remove(dic_esis_bloqueados,clave);
		list_add(lista_esis_bloq_esta_clave, esi_corriendo);
		dictionary_put(dic_esis_bloqueados,clave,lista_esis_bloq_esta_clave);

	}

	log_debug(logger, "Se bloqueo el esi: %d para la clave: %s \n", esi_corriendo->id, clave);
	hay_esi_bloqueado = true;
}

int id;

void bloquear_esi_por_consola(char* clave, int id_esi){

	id = id_esi;

	t_esi *esi_afectado = buscar_esi_por_id(id_esi);

	//No existe la clave, agrego esta nueva linea
		if(!dictionary_has_key(dic_esis_bloqueados, clave)){
			dictionary_put(dic_esis_bloqueados, clave, esi_afectado);

		}else{ //Existe la clave, agrego el esi a la lista de bloq

			t_list *lista_esis_bloq_esta_clave = dictionary_remove(dic_esis_bloqueados,clave);
			list_add(lista_esis_bloq_esta_clave, esi_afectado);
			dictionary_put(dic_esis_bloqueados,clave,lista_esis_bloq_esta_clave);

		}
	log_debug(logger, "Se bloqueo por consola el esi: %d para la clave: %s \n", esi_corriendo->id, clave);

}

void desbloquear_por_consola(clave){
	se_desbloqueo_un_recurso(clave);
}


t_esi *buscar_esi_por_id(int id_esi){

	t_esi *esi_a_devolver;
	//me fijo si es el esi que esta corriendo
	if(esi_corriendo->id == id){
		esi_a_devolver = esi_corriendo; //COMO HAGO QUE ESPERE HASTA QUE TERMINE DE CORRER?

	}else{
		//si no es el q esta corriendo lo busco en la lista de esis listos
		esi_a_devolver = list_find(lista_esis_listos, esi_con_este_id);

	}

	return esi_a_devolver;

}


bool esi_con_este_id(void* esi){
	return ((t_esi*)esi)->id == id;
}

//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE EL COORDINADOR LE INDICA QUE SE DESBLOQUIO UN RECURSO
void se_desbloqueo_un_recurso(char* clave){

	t_list *lista_esis_bloq_esta_clave =  dictionary_get(dic_esis_bloqueados,clave);
	t_esi* esi_desbloq = list_remove(lista_esis_bloq_esta_clave, 0);
	nuevo_esi(esi_desbloq);
	dictionary_put(dic_esis_bloqueados,clave,lista_esis_bloq_esta_clave);

	nuevo_esi(esi_desbloq);

}

//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE EL COORDINADOR LE PREGUNTA SI UN ESI TIENE UNA CLAVE
//SEGURAMENTE HAYA QUE REFACTORIZARLO A UN ID
bool esi_tiene_clave(t_esi* esi, char* clave){

	if(!dictionary_has_key(dic_clave_x_esi, clave)){
		t_esi* esi_que_la_tomo = dictionary_get(dic_clave_x_esi,clave);
		return (esi_que_la_tomo->id == esi->id);
	}else{
		return false;
	}

}

//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE EL COORDINADOR ME INDICA QUE SE TOMO UNA NUEVA CLAVE
void nueva_clave_tomada_x_esi(char* clave){

	//No existe la clave, agrego esta nueva linea
		if(!dictionary_has_key(dic_clave_x_esi, clave)){
			dictionary_put(dic_clave_x_esi, clave, esi_corriendo); //aca estoy guardando el puntero al esi_corriendo verdad?

			}else{ //Existe la clave, saco al que estaba y pongo al nuevo

				t_esi esi_que_lo_tenia= dictionary_remove(dic_clave_x_esi,clave);
				dictionary_put(dic_clave_x_esi,clave,esi_corriendo);
				//lo saco porque si, no se si el dictionary_put me pisa lo que ya tengo

			}

}

void correr(t_esi* esi){

	//LE MANDA AL ESI X SOCKET QUE CORRA
}

void ya_termino_linea(){


	list_iterate(lista_esis_listos, aumentar_viene_esperando);
	aumentar_viene_corriendo(esi_corriendo);

	//signal(ṕlanificador)
}

void aumentar_viene_esperando(void* esi){
	((t_esi*)esi)->viene_esperando = ((t_esi*)esi)->viene_esperando + 1;
}

void aumentar_viene_corriendo(void* esi){
	((t_esi*)esi)->dur_ult_raf = ((t_esi*)esi)->dur_ult_raf + 1;
}



