/*
 * config.c
 *
 *  Created on: 16 abr. 2018
 *      Author: utnso
 */
#include <stdlib.h>
#include <commons/config.h>
#include "config_esi.h"



config configurar(char *ruta){


	config configuracion;

	t_config *config_dictionary = config_create(ruta);


	    configuracion.ipCord = config_get_string_value(config_dictionary, "IP_CORD");
		configuracion.portCord = config_get_string_value(config_dictionary, "PUERTO_CORD");
		configuracion.ipPlan = config_get_string_value(config_dictionary, "IP_PLANIF");
		configuracion.portPlan = config_get_string_value(config_dictionary, "PUERTO_PLANIF");

	config_destroy(config_dictionary);

	return configuracion;
}

