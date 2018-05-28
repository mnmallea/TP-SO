/*
 * config.c
 *
 *  Created on: 16 abr. 2018
 *      Author: utnso
 */
#include "config_esi.h"


config configurar(char *ruta){
	log_debug(logger,"Configurando ESI");
	config configuracion;

	t_config *config_dictionary = config_create(ruta);

	if(config_dictionary == NULL){
		log_error(logger, "No existe la ruta del archivo de configuracion");
		exit(1);
	}

	if(		config_has_property(config_dictionary,"IP_COORD") &&
			config_has_property(config_dictionary,"PUERTO_COORD") &&
			config_has_property(config_dictionary,"IP_PLANIF") &&
			config_has_property(config_dictionary,"PUERTO_PLANIF")){
		configuracion.ipCoord = string_duplicate(config_get_string_value(config_dictionary, "IP_COORD"));
		configuracion.portCoord = string_duplicate(config_get_string_value(config_dictionary, "PUERTO_COORD"));
		configuracion.ipPlan = string_duplicate(config_get_string_value(config_dictionary, "IP_PLANIF"));
		configuracion.portPlan = string_duplicate(config_get_string_value(config_dictionary, "PUERTO_PLANIF"));
	}
	else{
		config_destroy(config_dictionary);
		log_error(logger, "No se pudo realizar la configuracion");
		exit(1);

	}

	config_destroy(config_dictionary);

	return configuracion;
}

void limpiar_configuracion(){
	free(configuracion.ipCoord);
	free(configuracion.ipPlan);
	free(configuracion.portCoord);
	free(configuracion.portPlan);
}
