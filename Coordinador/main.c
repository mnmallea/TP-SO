/*
 * main.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#include "../syntax-commons/my_socket.h"
#include "config_coordinador.h"
#include "algoritmos_distribucion.h"
#include "main.h"
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

	lista_instancias_disponibles = list_create();
	log_info(logger, "Lista instancias creada correctamente");

	log_destroy(logger);
	exit(0);
}

