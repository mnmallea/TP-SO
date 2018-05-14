/*
 * config_planificador.c
 *
 *  Created on: 12 may. 2018
 *      Author: utnso
 */

#include "config_planificador.h"


config configurar(char *ruta){
	log_info(logger,"Configurando coordinador");
	config configuracion;

	t_config *config_dictionary = config_create(ruta);

	if(config_dictionary == NULL){
		log_error(logger, "No existe la ruta del archivo de configuracion");
		exit(EXIT_FAILURE);
	}

	if(		config_has_property(config_dictionary, "LOCALPORT") &&
			config_has_property(config_dictionary, "ALGORITMO") &&
			config_has_property(config_dictionary, "ESTADO_INICIAL") &&
			config_has_property(config_dictionary, "IP_COORD") &&
			config_has_property(config_dictionary, "PORT_COORD") &&
			config_has_property(config_dictionary, "CLAVES_BLOQUEADAS")){
		strncpy(configuracion.puerto, config_get_string_value(config_dictionary, "LOCALPORT"),PORT_MAX_STRING_LENGTH);//hay que copiarselo porque al final borras el diccionario
		configuracion.puerto[5] = '\0';
		configuracion.algoritmo = config_get_int_value(config_dictionary, "ALGORITMO");
		configuracion.estimacion_inicial = config_get_int_value(config_dictionary, "ESTADO_INICIAL");
		configuracion.ipCoord = config_get_int_value(config_dictionary, "IP_COORD");
		configuracion.portCoord = config_get_int_value(config_dictionary, "PORT_COORD");
		configuracion.claves_bloqueadas = config_get_int_value(config_dictionary, "CLAVES_BLOQUEADAS");
	}else{
		config_destroy(config_dictionary);
		log_error(logger, "El archivo de configuracion es incorrecto\n");
		exit(EXIT_FAILURE);
	}

	config_destroy(config_dictionary);
	return configuracion;
}
