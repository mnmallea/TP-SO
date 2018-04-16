/*
 * main.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */
//#define RUTA_CONFIG "../config_coordinador"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <commons/log.h>
#include "algoritmos_distribucion.h"
#include "config_coordinador.h"

#define LOG_LEVEL LOG_LEVEL_TRACE

t_log *logger;
config *configuracion;

int main(int argc, char **argv){//aca recibiriamos la ruta del archivo de configuracion como parametro
	logger = log_create("coordinador.log","Coordinador", true, LOG_LEVEL);
	configuracion = configurar(argv[1]);
	printf("Puerto: %s\n", configuracion->puerto);
	printf("Si, esto todavia no hace nada.. \n");

	eliminar_configuracion(configuracion);
	log_destroy(logger);
	exit(0);
}

