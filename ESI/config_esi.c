/*
 * config.c
 *
 *  Created on: 16 abr. 2018
 *      Author: utnso
 */
#include "config_esi.h"



config configurar(char *ruta){


	config configuracion;

	t_config *config_dictionary = config_create(ruta);

	if(config_dictionary == NULL){
		log_error(logger, "No existe la ruta del archivo de configuracion");
		exit(1);
	}

	if(		config_has_property(config_dictionary,"IP_CORD") &&
			config_has_property(config_dictionary,"PUERTO_CORD") &&
			config_has_property(config_dictionary,"IP_PLANIF") &&
			config_has_property(config_dictionary,"PUERTO_PLANIF")){
		log_debug(logger,"IP del coordinador: %s", config_get_string_value(config_dictionary, "IP_CORD"));
	    configuracion.ipCord = string_duplicate(config_get_string_value(config_dictionary, "IP_CORD"));
		configuracion.portCord = string_duplicate(config_get_string_value(config_dictionary, "PUERTO_CORD"));
		configuracion.ipPlan = string_duplicate(config_get_string_value(config_dictionary, "IP_PLANIF"));
		configuracion.portPlan = string_duplicate(config_get_string_value(config_dictionary, "PUERTO_PLANIF"));
	}else{
		config_destroy(config_dictionary);
		log_error(logger, "No se pudo realizar la configuracion");
		exit(1);

	}

	config_destroy(config_dictionary);

	return configuracion;
}

void limpiar_configuracion(){
	free(configuracion.ipCord);
	free(configuracion.ipPlan);
	free(configuracion.portCord);
	free(configuracion.portPlan);
}
