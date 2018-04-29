/*
 * main.c
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */
#include <stdlib.h>
#include <commons/log.h>
#include "config_instancia.h"

config configuracion;
t_log *logger;

int main(int argc, char** argv){
	logger = log_create(argv[1], "Instancia.log", true, LOG_LEVEL_DEBUG);
	configuracion = configurar(argv[1]);

	limpiar_configuracion();
	log_destroy(logger);
	exit(0);
}
