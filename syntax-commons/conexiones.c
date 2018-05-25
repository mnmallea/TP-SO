/*
 * conexiones.c
 *
 *  Created on: 2 may. 2018
 *      Author: utnso
 */

#include "conexiones.h"

/*
 *Sirve para conectarse al coordinador y realizar un handshake.
 * En el remitente debe ir quien es el que lo llama.
 * Te devuelve el socket por el que se conecto
 * Si no puede, cierra el proceso
 */
int conectarse_a_coordinador(char* ip, char* puerto, t_identidad remitente) {
	log_info(logger, "Conectandose al Coordinador, IP: %s\tPuerto: %s", ip,
			puerto);
	int socket_coord = crear_socket_cliente(ip, puerto);

	safe_send(socket_coord, &remitente, sizeof(remitente));

	t_identidad *respuesta = safe_recv(socket_coord, sizeof(*respuesta));

	if (*respuesta == COORDINADOR) {
		log_info(logger, "Se ha conectado al coordinador correctamente!!");
	} else {
		free(respuesta);
		log_error(logger, "No se pudo conectar al coordinador");
		close(socket_coord);
		log_destroy(logger);
		limpiar_configuracion();
		exit(1);
	}
	free(respuesta);
	return socket_coord;
}

void enviar_operacion_unaria(int sockfd, t_cod_operacion cod_op, char *clave) {
	int32_t tamanio_clave = strlen(clave) + 1;
	if (send(sockfd, &cod_op, sizeof(cod_op), 0) < 0) {
		log_error(logger, "Error al enviar operacion");
	}
	if (send(sockfd, &tamanio_clave, sizeof(tamanio_clave), 0) < 0) {
		log_error(logger, "Error al enviar operacion");
	}
	if (send(sockfd, clave, tamanio_clave, 0) < 0) {
		log_error(logger, "Error al enviar operacion");
	}
}

void enviar_set(int sockfd, char* clave, char* valor) {
	int32_t tamanio_clave = strlen(clave) + 1;
	int32_t tamanio_valor = strlen(valor) + 1;
	t_cod_operacion cod_op = SET;
	if (send(sockfd, &cod_op, sizeof(cod_op), 0) < 0) {
		log_error(logger, "Error al enviar cod operacion");
	}
	if (send(sockfd, &tamanio_clave, sizeof(tamanio_clave), 0) < 0) {
		log_error(logger, "Error al enviar clave");
	}
	if (send(sockfd, clave, tamanio_clave, 0) < 0) {
		log_error(logger, "Error al enviar clave");
	}
	if (send(sockfd, &tamanio_valor, sizeof(tamanio_valor), 0) < 0) {
		log_error(logger, "Error al enviar valor");
	}
	if (send(sockfd, valor, tamanio_valor, 0) < 0) {
		log_error(logger, "Error al enviar valor");
	}
}

void recibir_set(int sockfd, char** clave, char** valor){
	recibirPaqueteVariable(sockfd, (void**)clave);
	recibirPaqueteVariable(sockfd, (void**)valor);
}

t_cod_operacion recibir_cod_operacion(int sockfd) {
	t_cod_operacion cod_op;
	if (recv(sockfd, &cod_op, sizeof(cod_op), MSG_WAITALL) < 0) {
		log_error(logger, "Error al recibir codigo de operacion");
		return -1;
	}
	return cod_op;
}

void recibir_operacion_unaria(int sockfd, char** clave) {
	/*
	int32_t tamanio;
	if (recv(sockfd, &tamanio, sizeof(tamanio), MSG_WAITALL) <= 0) {
		log_error(logger, "Error al recibir operacion unaria");
	}
	char* buffer = malloc(tamanio);
	if (recv(sockfd, buffer, tamanio, MSG_WAITALL) <= 0) {
		log_error(logger, "Error al recibir operacion unaria");
	}
	*clave = buffer;
	*/
	recibirPaqueteVariable(sockfd, (void**)clave);
}
