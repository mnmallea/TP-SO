/*
 * servidor.c
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */

#include "servidor.h"

#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../syntax-commons/conexiones.h"
#include "../syntax-commons/deserializador.h"
#include "../syntax-commons/my_socket.h"
#include "../syntax-commons/protocol.h"
#include "instancia.h"
#include "log_operaciones.h"
#include "operaciones.h"
#include "planificador.h"
#include "sincronizacion.h"

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
	while (1) {
		t_protocolo cod_operacion = recibir_cod_operacion(socket);
		log_info(logger, "Codigo operacion %s recibido desde el planificador",
				to_string_protocolo(cod_operacion));
		switch (cod_operacion) {
		case ERROR_CONEXION:
			//todo aca nos morimos de una manera piola
			log_error(logger, "Error en la conexion con el planificador");
			exit(EXIT_FAILURE);
			break; //el break mas necesario de la historia
		case SOLICITUD_STATUS_CLAVE:
			;
			char* clave = NULL;
			int respuesta = try_recibirPaqueteVariable(socket_planificador,
					(void**) &clave);
			if (respuesta <= 0) {
				free(clave);
				log_error(logger, "Error en la conexion con el planificador");
				exit(EXIT_FAILURE);
			}
			log_info(logger, "Recuperando el status de la clave %s", clave);
			informar_status_clave(clave);
			free(clave);
			break;
		default:
			respuesta_planificador = cod_operacion;
			sem_post(&planificador_respondio);
		}
	}
}

void atender_instancia(int sockfd) {
	t_instancia* instancia;
	char* nombre;
	if (try_recibirPaqueteVariable(sockfd, (void**) &nombre) <= 0) {
		log_error(logger, "Error al configurar instancia");
		close(sockfd);
		return;
	}
	if (send(sockfd, &configuracion.entrada_size,
			sizeof(configuracion.entrada_size), MSG_NOSIGNAL) < 0) {
		log_error(logger, "Error al configurar instancia");
		close(sockfd);
		return;
	}
	if (send(sockfd, &configuracion.cant_entradas,
			sizeof(configuracion.cant_entradas), MSG_NOSIGNAL) < 0) {
		log_error(logger, "Error al configurar instancia");
		close(sockfd);
		return;
	}

	if (esta_activa_instancia(nombre)) {
		log_info(logger, "La instancia %s ha estado previamente en el sistema",
				nombre);
		instancia_desactivar(nombre);
		instancia = instancia_relevantar(nombre, sockfd);
		if (instancia == NULL)
			return;
	} else {
		if (esta_inactiva_instancia(nombre)) {
			if ((instancia = instancia_relevantar(nombre, sockfd)) == NULL)
				return;
		} else {
			instancia = crear_instancia(sockfd, nombre,
					configuracion.cant_entradas);
			log_info(logger, "La instancia %s ha sido agregada por primera vez",
					instancia->nombre);
			instancia_agregar_a_activas(instancia);
		}
	}
	free(nombre);

	while (1) {
		sem_wait(&instancia->semaforo_instancia);
		if (enviar_cod_operacion(instancia->socket, INSTANCIA_COMPACTAR) < 0) {
			log_error(logger, "La instancia % se cayo al enviarla a compactar",
					instancia->nombre);
			instancia_desactivar(instancia->nombre);
			sem_post(&semaforo_compactacion);
			return;
		}
		t_protocolo cod_op = recibir_cod_operacion(instancia->socket);
		switch (cod_op) {
		case EXITO:
			log_info(logger,
					"Se ha realizado compactacion correctamente en instancia %s",
					instancia->nombre);
			break;
		case ERROR:
			log_error(logger,
					"Un error ha ocurrrido al realizar compactacion correctamente en instancia %s",
					instancia->nombre);
			break;
		case ERROR_CONEXION:
			log_error(logger,
					"La instancia %s se cayo durante el proceso de compactacion",
					instancia->nombre);
			sem_post(&semaforo_compactacion);
			instancia_desactivar(instancia->nombre);
			return;
		default:
			log_warning(logger, "Mensaje no esperado: %s", cod_op);
		}
		sem_post(&semaforo_compactacion);
	}

}

void atender_esi(int socket) {

	t_esi *esi = malloc(sizeof(t_esi));
	esi->socket = socket;
	if (recv(socket, &esi->id, sizeof(int), MSG_WAITALL) <= 0) {
		log_error(logger, "Error al recibir id de esi");
		free(esi);
		close(socket);
		return;
	}
	log_info(logger, "El esi %d se ha conectado correctamente al sistema",
			esi->id);

	bool seguir = true;
	while (seguir) {
		char* clave = NULL;
		char* valor = NULL;

		t_protocolo cod_op = recibir_cod_operacion(socket);
		pthread_mutex_lock(&mutex_operacion);
		log_trace(logger, "Codigo de operacion %s recibido del ESI %d",
				to_string_protocolo(cod_op), esi->id);

		switch (cod_op) {
		case OP_GET:
			recibir_operacion_unaria(socket, &clave);
			log_trace(logger, "[ESI %d]Recibi GET %s", esi->id, clave);
			retardarse(configuracion.retardo);
			realizar_get(esi, clave);
			break;
		case OP_STORE:
			recibir_operacion_unaria(socket, &clave);
			log_trace(logger, "[ESI %d]Recibi STORE %s", esi->id, clave);
			retardarse(configuracion.retardo);
			realizar_store(esi, clave);
			break;
		case OP_SET:
			recibir_set(socket, &clave, &valor);
			log_trace(logger, "[ESI %d]Recibi SET %s %s", esi->id, clave,
					valor);
			retardarse(configuracion.retardo);
			realizar_set(esi, clave, valor);
			break;
		case FINALIZO_ESI:
			log_info(logger, "Se termino de atender el ESI id: %d", esi->id);
			seguir = false;
			break;
		default:
			log_error(logger,
					"El esi %d ha muerto, le cortaron la garganta de aqui a aca",
					esi->id);
			seguir = false;
			break;
		}
		pthread_mutex_unlock(&mutex_operacion);

		free(clave);
		free(valor);
	}
	close(socket);
	free(esi);
}

void retardarse(long int milisegundos) {
	log_info(logger, "Retardandose %d milisegundos ...", milisegundos);
	struct timespec requested, remaining;
	requested.tv_sec = milisegundos / 1000;
	long int resto = milisegundos % 1000;
	if (resto != 0)
		requested.tv_nsec = resto * 1000000;
	else
		requested.tv_nsec = 0;
	nanosleep(&requested, &remaining);
}
