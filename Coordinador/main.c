/*
 * main.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#include "../syntax-commons/my_socket.h"
#include "config_coordinador.h"
#include "algoritmos_distribucion.h"
#include "servidor.h"

#define LOG_LEVEL LOG_LEVEL_TRACE
#define BACKLOG 5

int *c;

t_log *logger;
config configuracion;

int main(int argc, char **argv){//aca recibiriamos la ruta del archivo de configuracion como parametro
	logger = log_create("coordinador.log","Coordinador", true, LOG_LEVEL);
	configuracion = configurar(argv[1]);
	log_trace(logger, "Coordinador correctamente configurado");

	int local_socket = crear_socket_escucha(configuracion.puerto, BACKLOG);

	log_info(logger, "Escuchando en puerto: %s", configuracion.puerto);

	esperar_nuevas_conexiones(local_socket);
//	int client_socket = accept(local_socket,NULL,NULL);
//	log_info(logger, "Conexion aceptada");
//		recibir_confirmacion(client_socket);
//		mandar_confirmacion(client_socket);
//
//	int client_socket2 = accept(local_socket,NULL,NULL);
//	log_info(logger, "Conexion aceptada");
//		recibir_confirmacion(client_socket2);
//		mandar_confirmacion(client_socket2);

	log_destroy(logger);
	exit(0);
}

