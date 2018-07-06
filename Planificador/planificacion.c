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

int flag = 1;
bool hay_nuevo_esi = false;
bool hay_esi_bloqueado = false;
bool hay_esi_finalizado = false;

bool algoritmo_debe_planificar() {
	switch (configuracion.algoritmo) {
	case FIFO:
	case SJFsD:
	case HRRN:
		return false;
	case SJFcD:
		return hay_nuevo_esi;
	default:
		log_error(logger, "Algoritmo desconocido");
		exit(EXIT_FAILURE);
	}
}

bool hay_que_planificar() {
	pthread_mutex_lock(&mutex_esi_corriendo);
	return esi_corriendo == NULL || algoritmo_debe_planificar();

	pthread_mutex_unlock(&mutex_esi_corriendo);
}

void* planificar(void* _) {
	esi_corriendo = NULL;
	lista_esis_listos = list_create();
	lista_esis_finalizados = list_create();
	dic_esis_bloqueados = dictionary_create();
	dic_clave_x_esi = dictionary_create();

	while (1) {
		pthread_mutex_lock(&mutex_pausa);
		if (planificacion_pausada) {
			pthread_mutex_unlock(&mutex_pausa);
			sem_wait(&pausa_planificacion);
		} else {
			pthread_mutex_unlock(&mutex_pausa);
		}

		log_trace(logger, "Estoy planificandoooo!!!!");

		if (hay_que_planificar()) {
			log_debug(logger, "Pase hay que planificar");
			//pthread_mutex_lock(&mutex_esi_corriendo);
			log_debug(logger, "Antes de nuevo esi");
			esi_corriendo = obtener_nuevo_esi_a_correr();
			//pthread_mutex_unlock(&mutex_esi_corriendo);

			log_debug(logger, "Proximo esi a correr: %d \n", esi_corriendo->id);
		} else {
			log_debug(logger, "Se continua corriendo: %d \n",
					esi_corriendo->id);
		}
		log_debug(logger, "Para correr");
		correr(esi_corriendo);
	}
	return NULL;

}

/*
 * @thread_safe
 */
t_esi *obtener_nuevo_esi_a_correr() {
	t_esi* prox_esi;

	sem_wait(&contador_esis);
	log_debug(logger, "Pase el contador de esi a correr");

	if (configuracion.algoritmo == FIFO) {
		prox_esi = obtener_proximo_segun_fifo(lista_esis_listos);
	} else if (configuracion.algoritmo == SJFsD) {
		prox_esi = obtener_proximo_segun_sjf(lista_esis_listos);
	} else if (configuracion.algoritmo == SJFcD) {
		prox_esi = obtener_proximo_segun_sjf(lista_esis_listos);
	} else {
		prox_esi = obtener_proximo_segun_hrrn(lista_esis_listos);
	}
	hay_nuevo_esi = false;


	return prox_esi;

}

//thread safe
void nuevo_esi(t_esi* esi) {
	pthread_mutex_lock(&mutex_lista_esis_listos);
	list_add(lista_esis_listos, esi);
	pthread_mutex_unlock(&mutex_lista_esis_listos);
	hay_nuevo_esi = true;

	sem_post(&contador_esis);
	log_debug(logger, "Llego/se desalojo/se desbloqueo un nuevo esi: %d \n",
			esi->id);
}

void finalizar_esi(t_esi* esi_a_finalizar) {

	log_debug(logger, "Se procede a finalizar el ESI : %d \n", esi_a_finalizar->id);

	liberar_recursos(esi_a_finalizar);
	pthread_mutex_lock(&mutex_lista_esis_finalizados);
	list_add(lista_esis_finalizados, esi_corriendo);
	pthread_mutex_unlock(&mutex_lista_esis_finalizados);

	log_debug(logger, "Se procede a cerrar el socket del ESI : %d \n", esi_a_finalizar->id);
	close(esi_a_finalizar->socket);

	pthread_mutex_lock(&mutex_esi_corriendo);
	if(esi_a_finalizar-> id == esi_corriendo->id){
		esi_corriendo = NULL;
	}
	pthread_mutex_unlock(&mutex_esi_corriendo);

}


void bloquear_esi(char* clave, t_esi* esi_a_bloquear) {

	agregar_a_dic_bloqueados(clave, esi_a_bloquear);
	log_debug(logger, "Se bloqueo el esi: %d para la clave: %s \n",
			esi_a_bloquear->id, clave);

	pthread_mutex_lock(&mutex_esi_corriendo);
	if(esi_a_bloquear-> id == esi_corriendo->id){
		esi_corriendo = NULL;
	}
	pthread_mutex_unlock(&mutex_esi_corriendo);
}

void se_desbloqueo_un_recurso(char* clave) {

	pthread_mutex_lock(&mutex_dic_clave_x_esi);

	//valido que la clave este efectivamente tomada por algun esi
	if (dictionary_has_key(dic_clave_x_esi, clave)) {

		t_esi* esi_tenia_clave = dictionary_get(dic_clave_x_esi, clave);
		log_debug(logger, "Se le desbloquea la clave: %s al esi: %d",clave,esi_tenia_clave->id);
		dictionary_remove(dic_clave_x_esi, clave);

		pthread_mutex_unlock(&mutex_dic_clave_x_esi);

		pthread_mutex_lock(&mutex_dic_esis_bloqueados);
		//valido si la clave tenia esis encolados
		if (dictionary_has_key(dic_esis_bloqueados, clave)) {
				t_list *lista_esis_bloq_esta_clave = dictionary_remove(
						dic_esis_bloqueados, clave);
				t_esi* esi_a_desbloquear = list_remove(lista_esis_bloq_esta_clave, 0);

				log_debug(logger, "Habia esis esperando el desbloqueo de esta clave, "
						"se desbloqueo el esi: %d", esi_a_desbloquear->id);

				//valido seguir teniendo esis esperando esta clave
				if (list_size(lista_esis_bloq_esta_clave) != 0) {
					dictionary_put(dic_esis_bloqueados, clave,
							lista_esis_bloq_esta_clave);
				}

				list_destroy(lista_esis_bloq_esta_clave);

				pthread_mutex_unlock(&mutex_dic_esis_bloqueados);

				nuevo_esi(esi_a_desbloquear);

			} else {
				pthread_mutex_unlock(&mutex_dic_esis_bloqueados);
				log_debug(logger, "La clave desbloqueada no tenia esis encolados esperandola");
			}



	}else{
		pthread_mutex_unlock(&mutex_dic_clave_x_esi);
		log_error(logger, "Se esta intentando desbloquear un recurso que no estaba tomado");
	}

}

void correr(t_esi* esi) {

	mandar_confirmacion(esi->socket);

	sem_wait(&respondio_esi_corriendo);

	//Aumenta cuanto vienen esperando los que estan en listos
	pthread_mutex_lock(&mutex_lista_esis_listos);
	list_iterate(lista_esis_listos, aumentar_viene_esperando);
	pthread_mutex_unlock(&mutex_lista_esis_listos);

	//Aumenta la rafaga del esi que esta corriendo
	pthread_mutex_lock(&mutex_esi_corriendo);
	aumentar_viene_corriendo(esi_corriendo);
	pthread_mutex_unlock(&mutex_esi_corriendo);

	switch (respuesta_esi_corriendo) {
	case EXITO:
		ya_termino_linea();

		log_debug(logger, "Se procede a validar si habian pedido matar/bloquear al esi por consola");
		if(validar_si_hubo_bloqueo_o_asesinato_por_consola()){
			log_debug(logger, "Se encontro pedido de consola, se procede a ejecutar bloqueo/asesinato de esi");
			ejecutar_bloqueo_o_asesinato();
			nullear_esis_por_consola();
		}

		break;
	case CLAVE_SIZE:
		if(validar_si_hubo_bloqueo_o_asesinato_por_consola()){
			log_error(logger, "Se ignora el pedido realizado por consola(bloqueo/asesinato) porque hubo una falla en el esi");
			nullear_esis_por_consola();
		}
		linea_size();
		log_error(logger, "El error fue: el ESI intento leer una linea de mas de 40 caracteres");
		break;
	case INTERPRETAR:
		if(validar_si_hubo_bloqueo_o_asesinato_por_consola()){
			log_error(logger, "Se ignora el pedido realizado por consola(bloqueo/asesinato) porque hubo una falla en el esi");
			nullear_esis_por_consola();
		}
		interpretar();
		log_error(logger, "El error fue: no se pudo interpretar una linea del ESI");
		break;
	case ERROR_CONEXION:
		if(validar_si_hubo_bloqueo_o_asesinato_por_consola()){
			log_error(logger, "Se ignora el pedido realizado por consola(bloqueo/asesinato) porque hubo una falla en el esi");
			nullear_esis_por_consola();
		}

		log_error(logger, "El error fue: error de conexion en el ESI");
		/* no break */
	case ABORTA:
		if(validar_si_hubo_bloqueo_o_asesinato_por_consola()){
			log_error(logger, "Se ignora el pedido realizado por consola(bloqueo/asesinato) porque hubo una falla en el esi");
			nullear_esis_por_consola();
		}
		fallo_linea();
		log_error(logger, "El error fue: no se pudo ejecutar una linea del ESI");
		break;

	case INSTANCIA_CAIDA_EXCEPTION:
		if(validar_si_hubo_bloqueo_o_asesinato_por_consola()){
			log_error(logger, "Se ignora el pedido realizado por consola(bloqueo/asesinato) porque hubo una falla en el esi");
			nullear_esis_por_consola();
		}

		fallo_linea();
		log_error(logger, "El error fue: falla leyendo una linea del ESI debido a problemas de instancia");
		break;

	case FINALIZO_ESI:

		if(validar_si_hubo_bloqueo_o_asesinato_por_consola()){
			log_debug(logger, "Se ignora el pedido realizado por consola(bloqueo/asesinato) porque el esi ya ha finalizado su script");
			nullear_esis_por_consola();
		}
		pthread_mutex_lock(&mutex_esi_corriendo);
		finalizar_esi(esi_corriendo);
		pthread_mutex_lock(&mutex_esi_corriendo);
		break;
	case BLOQUEO_ESI:

		if(validar_si_hubo_bloqueo_o_asesinato_por_consola()){
			log_error(logger, "Se ignora el pedido realizado por consola(bloqueo/asesinato) porque ya el esi fue bloqueado por su script");
			nullear_esis_por_consola();
		}

		pthread_mutex_lock(&mutex_esi_corriendo);
		bloquear_esi(clave_bloqueadora,esi_corriendo);
		pthread_mutex_unlock(&mutex_esi_corriendo);
		free(clave_bloqueadora);
		break;

	default:
		break;
	}
}

void ya_termino_linea() {

//si leyo bien la linea
	log_trace(logger, "El esi %d se termino de leer una nueva linea \n",
			esi_corriendo->id);

}

void linea_size() {
//si leyo mal la linea
	log_error(logger, "El ESI %d leyo una clave con mas de 40 caracteres\n",
			esi_corriendo->id);
	finalizar_esi(esi_corriendo);
}

void interpretar() {
//si leyo mal la linea
	log_error(logger, "No se pudo interpretar una linea en el esi %d\n",
			esi_corriendo->id);
	finalizar_esi(esi_corriendo);
}

void fallo_linea() {
//si leyo mal la linea
	log_error(logger, "Hubo una falla cuando el esi %d leyo una nueva linea \n",
			esi_corriendo->id);
	finalizar_esi(esi_corriendo);
}


void nueva_solicitud(int socket, char* clave) {

	t_protocolo cod_op;

	//valido que la clave no se encuntre ya tomada por otro esi

	log_debug(logger, "El esi: %d solicito la clave: %s, se procede a validar si ya estaba tomada", esi_corriendo->id, clave);

	pthread_mutex_lock(&mutex_esi_corriendo);
	if (puede_tomar_la_clave(clave, esi_corriendo)) {
		pthread_mutex_unlock(&mutex_esi_corriendo);
		log_debug(logger, "El esi: %d solicito una clave ya tomada, se procede a bloquearlo", esi_corriendo->id);
		cod_op = BLOQUEO_ESI;
		clave_bloqueadora = strdup(clave);

	} else {

		log_debug(logger, "Se procede a bloquear la clave: %s para el esi: %d \n", clave, esi_corriendo->id);
		nueva_clave_tomada_x_esi(clave, esi_corriendo);
		pthread_mutex_unlock(&mutex_esi_corriendo);
		cod_op = EXITO;

	}

	enviar_cod_operacion(socket, cod_op);
}

void liberar_recursos(t_esi* esi_a_liberar) {

	t_list *lista_claves_a_desbloquear = list_create();

	void obtener_claves_tomadas_por_esi_a_liberar(char* clave, void* esi) {

		if (((t_esi*) esi)->id == esi_a_liberar->id) {
			list_add(lista_claves_a_desbloquear, clave);
		}
	}

	log_debug(logger, "Se procede a liberar las claves del esi: %d", esi_a_liberar);

	pthread_mutex_lock(&mutex_dic_clave_x_esi);
	//se obtienen todas las claves tomadas por el esi a liberar
	dictionary_iterator(dic_clave_x_esi, obtener_claves_tomadas_por_esi_a_liberar);
	pthread_mutex_unlock(&mutex_dic_clave_x_esi);

	//se itera la lista, liberando cada clave
	list_iterate(lista_claves_a_desbloquear, liberar_clave);
	list_destroy(lista_claves_a_desbloquear);

}

void liberar_clave(void* clave) {
	se_desbloqueo_un_recurso((char*) clave);

}

bool validar_si_hubo_bloqueo_o_asesinato_por_consola(){

	bool valido1; bool valido2;

	pthread_mutex_lock(&mutex_esi_a_bloquear_por_consola);
	valido1 = esi_a_bloquear_por_consola != NULL;
	pthread_mutex_unlock(&mutex_esi_a_bloquear_por_consola);


	pthread_mutex_lock(&mutex_esi_a_matar_por_consola);
	valido2 = esi_a_matar_por_consola != NULL;
	pthread_mutex_unlock(&mutex_esi_a_matar_por_consola);

	return (valido1 || valido2);
}

void nullear_esis_por_consola(){

	pthread_mutex_lock(&mutex_esi_a_matar_por_consola);
	esi_a_matar_por_consola = NULL;
	pthread_mutex_unlock(&mutex_esi_a_matar_por_consola);

	pthread_mutex_lock(&mutex_esi_a_bloquear_por_consola);
	esi_a_bloquear_por_consola = NULL;
	pthread_mutex_unlock(&mutex_esi_a_bloquear_por_consola);
}


void ejecutar_bloqueo_o_asesinato(){
			pthread_mutex_lock(&mutex_esi_a_bloquear_por_consola);
			if(esi_a_bloquear_por_consola != NULL){
				log_debug(logger, "Se habia pedido bloquear al esi para la clave: %s", clave_a_bloquear);
				bloquear_esi(clave_a_bloquear,esi_a_bloquear_por_consola);
				pthread_mutex_unlock(&mutex_esi_a_bloquear_por_consola);
			}else {
				pthread_mutex_unlock(&mutex_esi_a_bloquear_por_consola);

				pthread_mutex_lock(&mutex_esi_a_matar_por_consola);
				if(esi_a_matar_por_consola != NULL){
					log_debug(logger, "Se habia pedido matar al esi");
					finalizar_esi(esi_a_matar_por_consola);
				}

				pthread_mutex_unlock(&mutex_esi_a_matar_por_consola);
			}

}



typedef struct {
	int idEsi;
	char *retiene;
	char *espera;
} t_dl;

t_list * listaDL;
t_list *idsDL, *idsDLL;
t_dl *candidato;
t_esi *esiDL;
int rye = 0;

void deadlock(){
 idsDL = list_create();
 dictionary_iterator(dic_clave_x_esi,itera_por_linea);
 list_destroy_and_destroy_elements(idsDL,(void*) free);
 }

void itera_por_linea(char *claveIncialTomada, void *esiInicial){
	rye=1;
	//primerEsi
	candidato = malloc(sizeof(t_dl));
	candidato->idEsi = ((t_esi*) esiInicial)->id;

	if(!list_any_satisfy(idsDL,esta)&&((t_esi*) esiInicial)->id!=-1){
		listaDL = list_create();
		idsDLL = list_create();
		candidato->retiene = claveIncialTomada;
		dictionary_iterator(dic_esis_bloqueados, buscarClaveQEspera);
			if(!rye)
				goto DEST;
		//sgtes
		do{
			char *retieneProx=candidato->espera;
			candidato = malloc(sizeof(t_dl));
			esiDL=dictionary_get(dic_clave_x_esi,retieneProx);
			candidato->idEsi = ((t_esi*) esiDL)->id;
			candidato->retiene = retieneProx;
			dictionary_iterator(dic_esis_bloqueados, buscarClaveQEspera);
			if(!rye)
				goto DEST;

		}while(strcmp((*claveIncialTomada),candidato->espera)!=0);

		list_iterate(idsDLL,mergearL);

		//print
		//printf header de la tabla
		list_iterate(listaDL,mostrarDL);

		DEST:list_destroy_and_destroy_elements(listaDL,(void*) free);
			 list_destroy_and_destroy_elements(idsDLL,(void*) free);
	}
}

void buscarClaveQEspera(char* claveQEspera, void* esisbloq){
	if (list_find(esisbloq,esta)!=NULL){
			candidato->espera = claveQEspera;
			list_add(listaDL,candidato);
			list_add(idsDLL,&(candidato->idEsi));
		}
	else{
		rye=0;
	}
}

bool esta(void *esi){
	return ((t_esi*) esi)->id == candidato->idEsi;
}

void mergearL(void *id){
			list_add(idsDL,id);
}

void mostrarDL(void* candidato){
			printf("%d  |  %s   | %s",((t_dl*) candidato)->idEsi,((t_dl*) candidato)->retiene,((t_dl*) candidato)->espera);
}
