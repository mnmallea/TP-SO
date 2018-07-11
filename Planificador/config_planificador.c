/*
 * config_planificador.c
 *
 *  Created on: 12 may. 2018
 *      Author: utnso
 */

#include "config_planificador.h"


config configurar(char *ruta){
	log_debug(logger,"Configurando coordinador");
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
			config_has_property(config_dictionary, "CLAVES_BLOQUEADAS") &&
			config_has_property(config_dictionary, "ALFA")){
		strncpy(configuracion.puerto, config_get_string_value(config_dictionary, "LOCALPORT"),PORT_MAX_STRING_LENGTH);//hay que copiarselo porque al final borras el diccionario
		configuracion.puerto[5] = '\0';
		configuracion.algoritmo = get_algoritmo_planificacion(config_get_string_value(config_dictionary, "ALGORITMO"));
		configuracion.estimacion_inicial = config_get_int_value(config_dictionary, "ESTADO_INICIAL");
		configuracion.ipCoord = string_duplicate(config_get_string_value(config_dictionary, "IP_COORD"));
		configuracion.portCoord = string_duplicate(config_get_string_value(config_dictionary, "PORT_COORD"));
		configuracion.claves_bloqueadas = config_get_array_value(config_dictionary, "CLAVES_BLOQUEADAS");
		configuracion.alfa = ((double)config_get_int_value(config_dictionary, "ALFA"))/100;
		log_trace(logger, "Alfa de la configuracion: %f", configuracion.alfa);
	}
	else{
		config_destroy(config_dictionary);
		log_error(logger, "El archivo de configuracion es incorrecto\n");
		exit(EXIT_FAILURE);
	}
	log_debug(logger, "Planificador correctamente configurado");
	config_destroy(config_dictionary);
	return configuracion;
}

void limpiar_configuracion(){
//TODO: QUE ES ESTO???
}

tipo_algoritmo_planif get_algoritmo_planificacion(char* nombre_algoritmo){
	if(string_equals_ignore_case(nombre_algoritmo, "FIFO"))
		return FIFO;
	if(string_equals_ignore_case(nombre_algoritmo, "SJF-CD"))
		return SJFcD;
	if(string_equals_ignore_case(nombre_algoritmo, "SJF-SD"))
		return SJFsD;
	if(string_equals_ignore_case(nombre_algoritmo, "HRRN"))
		return HRRN;
	log_error(logger, "Algoritmo invalido");
	exit(EXIT_FAILURE);

	return -1;
}
