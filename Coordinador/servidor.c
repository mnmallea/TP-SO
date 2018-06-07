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
	if (recv(socket, &identidad, sizeof(identidad), MSG_WAITALL) <= 0) {
		log_error(logger, "Error al atender nueva conexion: %s",
				strerror(errno));
		close(socket);
		return;
	}
	int handshake_msg = COORDINADOR; //hace falta? todos los que se conectan al coordinador ya saben de antemano a quien se conectan
	if (send(socket, &handshake_msg, sizeof(handshake_msg), 0) < 0) {
		log_error(logger, "Error al atender nueva conexion: %s",
				strerror(errno));
		close(socket);
		return;
	}
	switch (identidad) {
	case ESI:
		log_info(logger, "Se ha conectado un ESI");
		atender_esi(socket);
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
		atender_planificador(socket);
		log_trace(logger, "Se termino de atender al Planificador, sockfd = %d",
				socket);
		break;
	default:
		log_error(logger, "Conexion desconocida");
		close(socket);
	}
}

void atender_planificador(int socket) {
	socket_planificador = socket;
}

void atender_instancia(int sockfd) {
	char* nombre;
	if (try_recibirPaqueteVariable(sockfd, (void**) &nombre) <= 0) {
		log_error(logger, "Error al configurar instancia");
		close(sockfd);
		return;
	}
	if (send(sockfd, &configuracion.entrada_size,
			sizeof(configuracion.entrada_size), 0) < 0) {
		log_error(logger, "Error al configurar instancia");
		close(sockfd);
		return;
	}
	if (send(sockfd, &configuracion.cant_entradas,
			sizeof(configuracion.cant_entradas), 0) < 0) {
		log_error(logger, "Error al configurar instancia");
		close(sockfd);
		return;
	}
	if (esta_activa_instancia(nombre)) {
		log_error(logger, "La instancia %s ya se encuentra activa", nombre);
		close(sockfd);
		return;
	}
	t_instancia* instancia = crear_instancia(sockfd, nombre,
			configuracion.cant_entradas);
	pthread_mutex_lock(&mutex_instancias_disponibles);
	cant_instancias++;
	list_add(lista_instancias_disponibles, instancia);
	log_debug(logger, "Instancia %s agregada a la lista", instancia->nombre);
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
//	recibir_confirmacion(socket);
	char* clave = NULL;
	char* valor = NULL;
	while (1) {
		t_protocolo cod_op = recibir_cod_operacion(socket);
		log_trace(logger, "codigo de operacion recibido");

		switch (cod_op) {
		case OP_GET:
		case OP_STORE:
			recibir_operacion_unaria(socket, &clave);
			log_trace(logger, "Recibi Get/store %s", clave);
			break;
		case OP_SET:
			recibir_set(socket, &clave, &valor);
			log_trace(logger, "Recibi SET %s %s", clave, valor);
			break;
		default:
			log_error(logger, "el esi ha muerto");
			close(socket);
			return;
		}
	}
	free(clave);
	free(valor);
}

void atender_planif(int socket) {

	while (1) {
		sem_wait(&planif_binario);
		mandar_confirmacion(socket);
		recibir_confirmacion(socket);
	}
}
