/*
 * error.c
 *
 *  Created on: 16 abr. 2018
 *      Author: utnso
 */
#include "error.h"

void exit_error_with_msg(char* msg){
	log_error(logger,msg);
	log_destroy(logger);
	eliminar_configuracion(configuracion);
	exit(1);
}

