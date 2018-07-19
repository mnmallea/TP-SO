/*
 * planificacion.c
 *
 *  Created on: 17 may. 2018
 *      Author: utnso
 */

#include "planificacion.h"

#include <commons/log.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../syntax-commons/conexiones.h"
#include "../syntax-commons/my_socket.h"
#include "algoritmos_planificacion.h"
#include "funciones_auxiliares.h"
#include "sincronizacion.h"

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

void* planificar(void* _) {
	log_trace(logger, "Trabajo muy duro, como un esclavo!");
	while (1) {

		pthread_mutex_lock(&mutex_pausa);
		if (planificacion_pausada) { //dejar esta pausa, sirve para cuando se inicia una prueba con el planificador vacio
			pthread_mutex_unlock(&mutex_pausa);
			sem_wait(&pausa_planificacion);
		} else {
			pthread_mutex_unlock(&mutex_pausa);
		}

		if (hay_que_planificar()) {
			log_trace(logger, "Planificando...");
			esi_corriendo = obtener_nuevo_esi_a_correr(); //movi el mutex adentro, porque si se lockeaba por el contador arrastraba el mutex con el tambien

			log_debug(logger, "Proximo esi a correr: %d \n", esi_corriendo->id);
		} else {
			log_debug(logger, "Continua corriendo: %d \n", esi_corriendo->id);
		}
		correr(esi_corriendo);
	}
	return NULL;

}

bool hay_que_planificar() {
	return esi_corriendo == NULL || algoritmo_debe_planificar();
}

void correr(t_esi* esi) {

	mandar_confirmacion(esi->socket);

	sem_wait(&respondio_esi_corriendo);

	log_debug(logger, "Signal para correr");

	//Aumenta cuanto vienen esperando los que estan en listos
	pthread_mutex_lock(&mutex_lista_esis_listos);
	list_iterate(lista_esis_listos, aumentar_viene_esperando);
	pthread_mutex_unlock(&mutex_lista_esis_listos);

	log_debug(logger, "Aumento el viene esperando de la lista de listos");

	//Aumenta la rafaga del esi que esta corriendo
	pthread_mutex_lock(&mutex_esi_corriendo);
	aumentar_viene_corriendo(esi_corriendo);
	pthread_mutex_unlock(&mutex_esi_corriendo);

	log_debug(logger, "Aumento rafaga actual del esi corriendo(%d) a: %d", esi_corriendo->id, esi_corriendo->rafaga_actual);

	switch (respuesta_esi_corriendo) {
	case EXITO:
		ya_termino_linea();

		log_debug(logger,
				"Se procede a validar si habian pedido matar/bloquear al esi por consola");
		if (validar_si_hubo_bloqueo_o_asesinato_por_consola()) {
			log_debug(logger,
					"Se encontro pedido de consola, se procede a ejecutar bloqueo/asesinato de esi");
			ejecutar_bloqueo_o_asesinato();
			nullear_esis_por_consola();
		}

		break;
	case CLAVE_SIZE:
		if (validar_si_hubo_bloqueo_o_asesinato_por_consola()) {
			log_error(logger,
					"Se ignora el pedido realizado por consola(bloqueo/asesinato) porque hubo una falla en el esi");
			nullear_esis_por_consola();
		}
		linea_size();
		log_error(logger,
				"El error fue: el ESI intento leer una linea de mas de 40 caracteres");
		break;
	case INTERPRETAR:
		if (validar_si_hubo_bloqueo_o_asesinato_por_consola()) {
			log_error(logger,
					"Se ignora el pedido realizado por consola(bloqueo/asesinato) porque hubo una falla en el esi");
			nullear_esis_por_consola();
		}
		interpretar();
		log_error(logger,
				"El error fue: no se pudo interpretar una linea del ESI");
		break;
	case ERROR_CONEXION: //porque valida en todos los casos si hubo asesinato por consola?
		if (validar_si_hubo_bloqueo_o_asesinato_por_consola()) {
			log_error(logger,
					"Se ignora el pedido realizado por consola(bloqueo/asesinato) porque hubo una falla en el esi");
			nullear_esis_por_consola();
		}
		finalizar_esi_corriendo(esi_corriendo);
		log_error(logger, "El error fue: error de conexion en el ESI");
		break; //antes no break
	case ABORTA:
		if (validar_si_hubo_bloqueo_o_asesinato_por_consola()) {
			log_error(logger,
					"Se ignora el pedido realizado por consola(bloqueo/asesinato) porque hubo una falla en el esi");
			nullear_esis_por_consola();
		}
		fallo_linea();
		log_error(logger,
				"El error fue: no se pudo ejecutar una linea del ESI");
		break;

	case INSTANCIA_CAIDA_EXCEPTION:
		if (validar_si_hubo_bloqueo_o_asesinato_por_consola()) {
			log_error(logger,
					"Se ignora el pedido realizado por consola(bloqueo/asesinato) porque hubo una falla en el esi");
			nullear_esis_por_consola();
		}

		finalizar_esi_corriendo(esi_corriendo);
		log_error(logger,
				"El error fue: falla leyendo una linea del ESI debido a problemas de instancia");
		break;

	case FINALIZO_ESI:
		if (validar_si_hubo_bloqueo_o_asesinato_por_consola()) {
			log_debug(logger,
					"Se ignora el pedido realizado por consola(bloqueo/asesinato) porque el esi ya ha finalizado su script");
			nullear_esis_por_consola();
		}
		finalizar_esi_corriendo(esi_corriendo);
		break;
	case BLOQUEO_ESI:
		if (validar_si_hubo_bloqueo_o_asesinato_por_consola()) {
			log_error(logger,
					"Se ignora el pedido realizado por consola(bloqueo/asesinato) porque ya el esi fue bloqueado por su script");
			nullear_esis_por_consola();
		}
		bloquear_esi(clave_bloqueadora, esi_corriendo);

		free(clave_bloqueadora);
		break;

	default:
		break;
	}

	//log_debug(logger, "Salio de correr");

}

/*
 * @thread_safe
 */
t_esi *obtener_nuevo_esi_a_correr() {
	t_esi* prox_esi;
	//log_debug(logger, "Esperando el contador de esis");
	CONTADOR: sem_wait(&contador_esis);
	//log_debug(logger, "Pase el contador de esi a correr");
	pthread_mutex_lock(&mutex_pausa);
	if (planificacion_pausada) {
		pthread_mutex_unlock(&mutex_pausa);
		sem_wait(&pausa_planificacion);
		sem_post(&contador_esis);
		goto CONTADOR;

	} else {
		pthread_mutex_unlock(&mutex_pausa);
		//log_debug(logger, "deslockeo mutex pausa");
	}
	pthread_mutex_lock(&mutex_esi_corriendo);

	switch(configuracion.algoritmo){

	case FIFO:
		prox_esi = obtener_proximo_segun_fifo(lista_esis_listos);
		break;
	case SJFsD:
		prox_esi = obtener_proximo_segun_sjf(lista_esis_listos);
		break;
	case SJFcD:
		prox_esi = obtener_proximo_segun_sjfcd(lista_esis_listos);

		if(esi_corriendo != NULL && remaining_time(prox_esi) > remaining_time(esi_corriendo)){
			log_debug(logger, "El remaining time del esi elegido(%d) es: %f > el del corriendo(%d): %f",prox_esi->id,remaining_time(prox_esi), esi_corriendo->id,remaining_time(esi_corriendo) );
			//el remaining time del actual es menor que todas las estimaciones
			//devuelvo al esi_corriendo sin modificarle ninguna rafaga ni nada
			prox_esi = esi_corriendo;
			sem_post(&contador_esis);

		}else{
			//o no habia esi corriendo o el esi corriendo debe ser desalojado
			if(esi_corriendo != NULL){
				log_debug(logger, "El remaining time del esi elegido(%d) es: %f < el del corriendo(%d): %f",prox_esi->id,remaining_time(prox_esi), esi_corriendo->id,remaining_time(esi_corriendo) );
				//se desaloja el esi que estaba corriendo
				nuevo_esi(esi_corriendo);
			}

			//prox_esi->estim_anter = prox_esi->estim_actual;
			pthread_mutex_lock(&mutex_lista_esis_listos);
			remover_esi_de_lista(lista_esis_listos,prox_esi->id);
			pthread_mutex_unlock(&mutex_lista_esis_listos);
		}

		break;
	case HRRN:
		prox_esi = obtener_proximo_segun_hrrn(lista_esis_listos);
		break;
	default:
		break;
	}

	pthread_mutex_unlock(&mutex_esi_corriendo);
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

void finalizar_esi_corriendo(t_esi* esi_a_finalizar) {

	log_debug(logger, "Cerrando conexion del esi %d...", esi_a_finalizar->id);
	int morite_hdp = -1;
	send(esi_a_finalizar->socket, &morite_hdp, sizeof(morite_hdp),
	MSG_NOSIGNAL);
	cerrarConexion(&esi_a_finalizar->socket);

	log_debug(logger, "Se procede a finalizar el ESI : %d \n",
			esi_a_finalizar->id);

	liberar_recursos(esi_a_finalizar);
	pthread_mutex_lock(&mutex_lista_esis_finalizados);
	list_add(lista_esis_finalizados, esi_a_finalizar);
	pthread_mutex_unlock(&mutex_lista_esis_finalizados);
	log_debug(logger, "El esi ID %d se agrego a finalizado\n",
			esi_a_finalizar->id);

	pthread_mutex_lock(&mutex_esi_corriendo);
	esi_corriendo = NULL;
	//log_debug(logger, "Nullea el corriendo");
	pthread_mutex_unlock(&mutex_esi_corriendo);

}

void finalizar_esi_sync(t_esi* esi_a_finalizar) {

	log_debug(logger, "Cerrando conexion del esi %d...", esi_a_finalizar->id);
	int morite_hdp = -1;
	send(esi_a_finalizar->socket, &morite_hdp, sizeof(morite_hdp),
	MSG_NOSIGNAL);
	cerrarConexion(&esi_a_finalizar->socket);

	log_debug(logger, "Se procede a finalizar el ESI : %d \n",
			esi_a_finalizar->id);

	liberar_recursos(esi_a_finalizar);
	pthread_mutex_lock(&mutex_lista_esis_finalizados);
	list_add(lista_esis_finalizados, esi_a_finalizar);
	pthread_mutex_unlock(&mutex_lista_esis_finalizados);
	log_debug(logger, "El esi ID %d se agrego a finalizado\n",
			esi_a_finalizar->id);

	pthread_mutex_lock(&mutex_esi_corriendo);
	if (esi_corriendo != NULL && esi_corriendo->id == esi_a_finalizar->id) {
		esi_corriendo = NULL;
		//log_debug(logger, "Nullea el corriendo");
	}
	pthread_mutex_unlock(&mutex_esi_corriendo);
}

void bloquear_esi(char* clave, t_esi* esi_a_bloquear) {

	agregar_a_dic_bloqueados(clave, esi_a_bloquear);
	log_debug(logger, "Se bloqueo el esi: %d para la clave: %s \n",
			esi_a_bloquear->id, clave);

	pthread_mutex_lock(&mutex_esi_corriendo);
	if (esi_corriendo != NULL && esi_a_bloquear->id == esi_corriendo->id) {
		esi_corriendo->estim_anter = esi_corriendo->estim_actual;
		esi_corriendo->rafaga_anterior = esi_corriendo->rafaga_actual;
		esi_corriendo->rafaga_actual = 0;

		log_debug(logger, "Se bloquea el esi corriendo(%d), rafaga anterior: (%d)", esi_corriendo->id, esi_corriendo->rafaga_anterior);
		esi_corriendo = NULL;
	}
	pthread_mutex_unlock(&mutex_esi_corriendo);
}

void se_desbloqueo_un_recurso(char* clave) {

	pthread_mutex_lock(&mutex_dic_clave_x_esi);

	//valido si es una clave a la que le habian hecho un get/set
	log_debug(logger,
			"Se procede a validar la clave %s haya sido tomada por un esi",
			clave);
	if (dictionary_has_key(dic_clave_x_esi, clave)) {

		t_esi* esi_tenia_clave = dictionary_get(dic_clave_x_esi, clave);
		log_debug(logger, "Se le desbloquea la clave: %s al esi: %d", clave,
				esi_tenia_clave->id);
		dictionary_remove(dic_clave_x_esi, clave);

		pthread_mutex_unlock(&mutex_dic_clave_x_esi);

	} else {
		pthread_mutex_unlock(&mutex_dic_clave_x_esi);
		log_debug(logger, "La clave no estaba tomada por nadie");
	}

	log_debug(logger,
			"Se procede a validar que la clave %s tenga esis encolados", clave);
	pthread_mutex_lock(&mutex_dic_esis_bloqueados);
	//valido si la clave tenia esis encolados
	if (dictionary_has_key(dic_esis_bloqueados, clave)) {
		t_list *lista_esis_bloq_x_esta_clave = dictionary_remove(
				dic_esis_bloqueados, clave);
		t_esi* esi_a_desbloquear = list_remove(lista_esis_bloq_x_esta_clave, 0);

		log_debug(logger, "Habia ESIS esperando el desbloqueo de esta clave, "
				"se desbloqueo el esi: %d", esi_a_desbloquear->id);

		//valido seguir teniendo esis esperando esta clave
		if (list_size(lista_esis_bloq_x_esta_clave) != 0) {
			dictionary_put(dic_esis_bloqueados, clave,
					lista_esis_bloq_x_esta_clave);
		}

		pthread_mutex_unlock(&mutex_dic_esis_bloqueados);

		nuevo_esi(esi_a_desbloquear);

	} else {
		pthread_mutex_unlock(&mutex_dic_esis_bloqueados);
		log_debug(logger,
				"La clave desbloqueada (%s) no tenia esis encolados esperandola",
				clave);
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
	finalizar_esi_corriendo(esi_corriendo);
}

void interpretar() {
//si leyo mal la linea
	log_error(logger, "No se pudo interpretar una linea en el esi %d\n",
			esi_corriendo->id);
	finalizar_esi_corriendo(esi_corriendo);
}

void fallo_linea() {
//si leyo mal la linea
	log_error(logger, "Hubo una falla cuando el esi %d leyo una nueva linea \n",
			esi_corriendo->id);
	finalizar_esi_corriendo(esi_corriendo);
}

void nueva_solicitud(int socket, char* clave, int id_pedido) {

	t_protocolo cod_op;

	//valido que la clave no se encuntre ya tomada por otro esi

	pthread_mutex_lock(&mutex_esi_corriendo); //aca se queda el dl
	log_debug(logger, "paso mutex nueva solicitud");

	if (esi_corriendo == NULL || esi_corriendo->id != id_pedido) {
		pthread_mutex_unlock(&mutex_esi_corriendo);
		cod_op = MURIO_ESI_CORRIENDO;
	} else {
		log_debug(logger,
				"El esi: %d solicito la clave: %s, se procede a validar si ya estaba tomada",
				esi_corriendo->id, clave);

		if (!puede_tomar_la_clave(clave, esi_corriendo)) {
			pthread_mutex_unlock(&mutex_esi_corriendo);
			log_debug(logger,
					"El esi: %d solicito una clave ya tomada, se procede a bloquearlo",
					esi_corriendo->id);
			cod_op = BLOQUEO_ESI;
			clave_bloqueadora = strdup(clave);

		} else {
			pthread_mutex_unlock(&mutex_esi_corriendo); //
			log_debug(logger,
					"Se procede a bloquear la clave: %s para el esi: %d \n",
					clave, esi_corriendo->id);
			nueva_clave_tomada_x_esi(clave, esi_corriendo);
			cod_op = EXITO;

		}

//		pthread_mutex_lock(&mutex_esi_corriendo);
//		if(id_pedido != esi_corriendo->id){
//			pthread_mutex_unlock(&mutex_esi_corriendo);
//			cod_op = MURIO_ESI_CORRIENDO;
//		}
	}
	enviar_cod_operacion(socket, cod_op);
}

void liberar_recursos(t_esi* esi_a_liberar) {
	int cont = 0;
	t_list *lista_claves_a_desbloquear = list_create();
	void obtener_claves_tomadas_por_esi_a_liberar(char* clave, void* esi) {

		if (((t_esi*) esi)->id == esi_a_liberar->id) {
			list_add(lista_claves_a_desbloquear, strdup(clave));
			cont++;
		}
	}

	//se obtienen todas las claves tomadas por el esi a liberar
	pthread_mutex_lock(&mutex_dic_clave_x_esi);
	dictionary_iterator(dic_clave_x_esi,
			obtener_claves_tomadas_por_esi_a_liberar);
	pthread_mutex_unlock(&mutex_dic_clave_x_esi);

	//se itera la lista, liberando cada clave
	if (cont > 0) {
		log_debug(logger, "Se procede a liberar las claves del esi: %d",
				esi_a_liberar->id);
		list_iterate(lista_claves_a_desbloquear, liberar_clave);
	}
	list_destroy_and_destroy_elements(lista_claves_a_desbloquear, free);
	//log_debug(logger, "Paso el cont");

}

void liberar_clave(void* clave) {
	se_desbloqueo_un_recurso((char*) clave);

}

bool validar_si_hubo_bloqueo_o_asesinato_por_consola() {

	bool valido1;
	bool valido2;

	pthread_mutex_lock(&mutex_esi_a_bloquear_por_consola);
	valido1 = esi_a_bloquear_por_consola != NULL;
	pthread_mutex_unlock(&mutex_esi_a_bloquear_por_consola);

	pthread_mutex_lock(&mutex_esi_a_matar_por_consola);
	valido2 = esi_a_matar_por_consola != NULL;
	pthread_mutex_unlock(&mutex_esi_a_matar_por_consola);

	return (valido1 || valido2);
}

void nullear_esis_por_consola() {

	pthread_mutex_lock(&mutex_esi_a_matar_por_consola);
	esi_a_matar_por_consola = NULL;
	pthread_mutex_unlock(&mutex_esi_a_matar_por_consola);

	pthread_mutex_lock(&mutex_esi_a_bloquear_por_consola);
	esi_a_bloquear_por_consola = NULL;
	pthread_mutex_unlock(&mutex_esi_a_bloquear_por_consola);
}

void ejecutar_bloqueo_o_asesinato() {
	pthread_mutex_lock(&mutex_esi_a_bloquear_por_consola);
	if (esi_a_bloquear_por_consola != NULL) {
		log_debug(logger, "Se habia pedido bloquear al esi para la clave: %s",
				clave_a_bloquear);
		bloquear_esi(clave_a_bloquear, esi_a_bloquear_por_consola);
		pthread_mutex_unlock(&mutex_esi_a_bloquear_por_consola);
	} else {
		pthread_mutex_unlock(&mutex_esi_a_bloquear_por_consola);

		pthread_mutex_lock(&mutex_esi_a_matar_por_consola);
		if (esi_a_matar_por_consola != NULL) {
			log_debug(logger, "Se habia pedido matar al esi");

			//cerrarConexion(&esi_corriendo->socket);
			finalizar_esi_sync(esi_a_matar_por_consola); //era corriendo
		}
		pthread_mutex_unlock(&mutex_esi_a_matar_por_consola);

	}
	free(clave_a_bloquear);

}

t_list * listaDL;
t_list *idsDL;
int *idCandidatoDL;
char *candidatoRetiene;
char *candidatoEspera;
t_esi *esi_q_retiene;
int rye;
int primer;

void deadlock() {
	idsDL = list_create();
	log_debug(logger, "Crea lista DL");
	dictionary_iterator(dic_clave_x_esi, itera_por_linea);
	list_destroy(idsDL);
}

void itera_por_linea(char *claveIncialTomada, void *esiInicial) {
	listaDL = list_create();
	rye = 0;
	//primero
	idCandidatoDL = &((t_esi*) esiInicial)->id;
	if (!list_any_satisfy(idsDL, esta) && *idCandidatoDL != -1) {
		log_debug(logger, "Entra por el primer ESI (%d)", *idCandidatoDL);
		candidatoRetiene = claveIncialTomada;
		log_debug(logger, "Toma clave clave %s", candidatoRetiene);
		dictionary_iterator(dic_esis_bloqueados, buscarClaveQEspera);

		if (!rye) {
			log_debug(logger, "No cumple RYE");
			goto DEST;
		}
		log_debug(logger, "Espera clave %s", candidatoEspera);
		rye = 0;
		//sgtes

		esi_q_retiene = dictionary_get(dic_clave_x_esi, candidatoEspera);
		idCandidatoDL = &esi_q_retiene->id;
		while (((t_esi*) esiInicial)->id != *idCandidatoDL) {
			candidatoRetiene = candidatoEspera;
			log_debug(logger,
					"Encontro el ESI(%d) que retiene la clave esperada",
					*idCandidatoDL);
			if (!list_any_satisfy(idsDL, esta) && *idCandidatoDL != -1) {
				log_debug(logger,
						"No es esta retenida por el sistema, ni pertenece a otro deadlock");
				log_debug(logger, "Toma clave clave %s", candidatoRetiene);
				dictionary_iterator(dic_esis_bloqueados, buscarClaveQEspera);

				if (!rye) {
					log_debug(logger, "No cumple RYE");
					goto DEST;
				}
				log_debug(logger, "Espera clave %s", candidatoEspera);
				rye = 0;
			} else {
				log_debug(logger,
						"Esta retenida por el sistema/El esi ya pertenece a un dl\n");
				goto DEST;
			}
			esi_q_retiene = dictionary_get(dic_clave_x_esi, candidatoEspera);
			idCandidatoDL = &esi_q_retiene->id;
		}

		list_add_all(idsDL, listaDL);
		//print
		//printf header de la tabla
		idCandidatoDL = list_get(listaDL, 0);
		printf("El ESI (ID:%d) espera al ", *idCandidatoDL);
		list_remove(listaDL, 0);
		list_iterate(listaDL, mostrarDL);
		printf("primer ESI (ID:%d)\n", *idCandidatoDL);
	} else {
		log_debug(logger,
				"Esta retenida por el sistema/El esi ya pertenece a un dl\n");
	}

	DEST: list_destroy(listaDL);
}

void buscarClaveQEspera(char* claveQEspera, void* esisbloq) {
	if (list_find(esisbloq, esta) != NULL && list_find(idsDL, esta) == NULL) {
		candidatoEspera = claveQEspera;
		list_add(listaDL, idCandidatoDL);
		log_debug(logger, "Encuentra la clave que espera el esi");
		rye = 1;
	}
}

bool esta(void *esi) {
	return ((t_esi*) esi)->id == *idCandidatoDL;
}

void mostrarDL(void* candidato) {
	printf("ESI (ID:%d) que espera al ", *((int*) candidato));
}
