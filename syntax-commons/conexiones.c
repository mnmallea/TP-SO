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

/*
 * Devuelve valor negativo si fallo
 */
int enviar_operacion_unaria(int sockfd, t_protocolo cod_op, char *clave) {
	if (send(sockfd, &cod_op, sizeof(cod_op), 0) < 0) {
		log_error(logger, "Error al enviar operacion");
		return -1;
	}
	t_paquete* paquete = paquete_crear();
	paquete_agregar(paquete, clave, strlen(clave) + 1);
	paquete_enviar(paquete, sockfd);
	paquete_destruir(paquete);
	return 0;
}

/*
 * Devuelve valor negativo si fallo
 */
int enviar_get(int sockfd,char *clave){
	return enviar_operacion_unaria(sockfd, OP_GET, clave);
}

/*
 * Devuelve valor negativo si fallo
 */
int enviar_store(int sockfd,char *clave){
	return enviar_operacion_unaria(sockfd, OP_STORE, clave);
}

/*
 * Devuelve valor negativo si fallo
 */
int enviar_set(int sockfd, char* clave, char* valor) {
	t_protocolo cod_op = OP_SET;
	if (send(sockfd, &cod_op, sizeof(cod_op), 0) < 0) {
		log_error(logger, "Error al enviar codigo operacion");
		return -1;
	}
	t_paquete* paquete = paquete_crear();
	paquete_agregar(paquete, clave, strlen(clave) + 1);
	paquete_agregar(paquete, valor, strlen(valor) + 1);
	paquete_enviar(paquete, sockfd);
	paquete_destruir(paquete);
	return 0;
}

int recibir_set(int sockfd, char** clave, char** valor) {
	int res;
	if ((res = try_recibirPaqueteVariable(sockfd, (void**)clave)) <= 0) {
		log_error(logger, "Error al recibir clave");
		return res;
	}
	if ((res = try_recibirPaqueteVariable(sockfd, (void**)valor)) <= 0) {
		log_error(logger, "Error al recibir clave");
		return res;
	}
	return res;
}

t_protocolo recibir_cod_operacion(int sockfd) {
	t_protocolo cod_op;
	if (recv(sockfd, &cod_op, sizeof(cod_op), MSG_WAITALL) <= 0) {
		log_error(logger, "Error al recibir codigo de operacion");
		return -1;
	}
	return cod_op;
}

int recibir_operacion_unaria(int sockfd, char** clave) {
	return try_recibirPaqueteVariable(sockfd, (void**) clave);
}
