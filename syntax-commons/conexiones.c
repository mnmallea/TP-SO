/*
 * conexiones.c
 *
 *  Created on: 2 may. 2018
 *      Author: utnso
 */

#include "conexiones.h"

void conectarse_a_coordinador(char* ip, char* puerto, t_identidad remitente) {
	log_info(logger, "Conectandose al Coordinador, IP: %s\tPuerto: %s", ip, puerto);
	int socket_coord = crear_socket_cliente(ip, puerto);

//	t_identidad handshake_msg = remitente;

	safe_send(socket_coord, &remitente, sizeof(remitente));

	t_identidad *respuesta = safe_recv(socket_coord, sizeof(*respuesta));

	if (*respuesta == COORDINADOR) {
		log_info(logger, "Se ha conectado al coordinador correctamente!!");
	} else {
		log_error(logger, "No se pudo conectar al coordinador");
		close(socket_coord);
		log_destroy(logger);
		limpiar_configuracion();
		exit(1);
	}
}
