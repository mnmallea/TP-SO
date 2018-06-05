/*
 * servidor.c
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */

#include "servidor.h"

void esperar_nuevas_conexiones(int *sockfd) {
	int new_sockfd;
	socklen_t addr_size;
	struct sockaddr_storage their_addr;
	pthread_t thread;
	while (1) {
		addr_size = sizeof(their_addr);

		sem_wait(&semaforo_binario);
		new_sockfd = accept(*sockfd, (struct sockaddr *) &their_addr,
				&addr_size);

		if (new_sockfd < 0) {
			log_error(logger, strerror(errno));
			continue;
		}
		log_info(logger, "Nueva conexion recibida");
		log_debug(logger, "Socket creado: %d", new_sockfd);
		if (pthread_create(&thread, NULL, (void*) atender_nueva_conexion,
				&new_sockfd)) {
			log_error(logger, "Error creando thread");
		}
		pthread_detach(thread);
	}
}

void atender_nueva_conexion(int* sockfd_ptr) {
	int socket = *sockfd_ptr;
	sem_post(&semaforo_binario);

	log_debug(logger, "socketFD = %d", socket);
	t_identidad identidad;
	if(recv(socket, &identidad, sizeof(identidad), MSG_WAITALL) <= 0){
		log_error(logger, "Error al atender nueva conexion: %s", strerror(errno));
		close(socket);
		return;
	}
	int handshake_msg = COORDINADOR; //hace falta? todos los que se conectan al coordinador ya saben de antemano a quien se conectan
	if(send(socket, &handshake_msg, sizeof(handshake_msg),0) < 0){
		log_error(logger, "Error al atender nueva conexion: %s", strerror(errno));
		close(socket);
		return;
	}
	switch (identidad) {
	case ESI:
		log_info(logger, "Se ha conectado un ESI");
		atender_esi(socket);
		/*
		if (pthread_create(&thread_ESI, NULL,
					(void*) atender_esi, &socket)) {
				log_error(logger, "Error creando el hilo del servidor escucha\n");
				exit(EXIT_FAILURE);
			}

		if (pthread_join(thread_ESI, NULL)) {
			log_error(logger, "Error al joinear thread del servidor escucha");
			exit(EXIT_FAILURE);
		}
		*/

		log_trace(logger, "Se termino de atender un ESI, sockfd = %d", socket);
		break;
	case INSTANCIA:
		log_info(logger, "Se ha conectado una Instancia");
		atender_instancia(socket);
		log_trace(logger, "Se termino de atender una Instancia, sockfd = %d",
				socket);
		break;
	case PLANIFICADOR:
		log_info(logger, "Se ha conectado el Planificador");
		atender_planif(socket);
		log_trace(logger, "Se termino de atender al Planificador, sockfd = %d",
				socket);
		/*
		if (pthread_create(&thread_Planificador, NULL,
					(void*) atender_planif, &socket)) {
				log_error(logger, "Error creando el hilo del servidor escucha\n");
				exit(EXIT_FAILURE);
			}

		if (pthread_join(thread_Planificador, NULL)) {
			log_error(logger, "Error al joinear thread del servidor escucha");
			exit(EXIT_FAILURE);
		}
		*/

		break;
	default:
		log_error(logger, "Conexion desconocida");
	}
	close(socket); //hay que ver si aca se cerraria el socket o se cerraria antes
	//tambien me preocupa saber si el socket al pasarselo como puntero esta siendo compartido
}

void atender_instancia(int sockfd) {
	t_instancia* instancia = calloc(1, sizeof(instancia));
	instancia->socket = sockfd;
	instancia->cant_entradas_vacias = configuracion.cant_entradas;
	instancia->claves_almacenadas = list_create();

	pthread_mutex_lock(&mutex_instancias_disponibles);
	instancia->id = cant_instancias;
	cant_instancias++;
	list_add(lista_instancias_disponibles, instancia);
	log_debug(logger, "Instancia Nº:%d agregada a la lista", instancia->id);
	pthread_mutex_unlock(&mutex_instancias_disponibles);
	sem_post(&contador_instancias_disponibles);
}

void atender_esi(int socket) {


	t_esi *n_esi = malloc(sizeof(t_esi));
	n_esi->socket = socket;
	n_esi->id = safe_recv(socket, sizeof(int));

	//n_esi->valores=malloc(0); //hay que acordarse de hacer free a los valores cuando termine de atender al ESI
	pthread_mutex_lock(&mutex_esi_disponibles);
	list_add(lista_esis_disponibles, n_esi);
	log_debug(logger, "Esi id:%d agregada a la lista", *(n_esi->id));
	pthread_mutex_unlock(&mutex_esi_disponibles);

	sem_post(&planif_binario);
	recibir_confirmacion(socket);

	//t_operacion *carga=NULL; tipo de la carga
	//recibirPaqueteVariable(socket,(void**)carga);
	//log_debug(logger, "Clave del pkg recivido: %s ", *(carga->clave));



}

void atender_planif(int socket){

	while(1){
	sem_wait(&planif_binario);
	mandar_confirmacion(socket);
	recibir_confirmacion(socket);
	}
};
