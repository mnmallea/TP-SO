/*
 * config_instancia.c
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */
#include "config_instancia.h"

config configurar(char* ruta) {
	config configuracion;
	t_config *config_dictionary = config_create(ruta);

	if (config_dictionary == NULL) {
		log_error(logger, "No existe la ruta del archivo de configuracion");
		log_destroy(logger);
		exit(1);
	}
	if(		config_has_property(config_dictionary,"IP_COORD") &&
			config_has_property(config_dictionary,"PUERTO_COORD") &&
			config_has_property(config_dictionary,"ALGORITMO") &&
			config_has_property(config_dictionary,"PUNTO_MONTAJE") &&
			config_has_property(config_dictionary,"NOMBRE_INSTANCIA") &&
			config_has_property(config_dictionary,"INTERVALO_DUMP")){

	    configuracion.ip_coordinador = string_duplicate(config_get_string_value(config_dictionary, "IP_COORD"));
		configuracion.puerto_coordinador = string_duplicate(config_get_string_value(config_dictionary, "PUERTO_COORD"));
		configuracion.punto_montaje = string_duplicate(config_get_string_value(config_dictionary, "PUNTO_MONTAJE"));
		configuracion.nombre_instancia = string_duplicate(config_get_string_value(config_dictionary, "NOMBRE_INSTANCIA"));
		configuracion.intervalo_dump = config_get_int_value(config_dictionary, "INTERVALO_DUMP");
		configuracion.algoritmo = get_enum_algoritmo(config_get_string_value(config_dictionary, "ALGORITMO"));

		if (configuracion.algoritmo < 0) {
			config_destroy(config_dictionary);
			log_error(logger, "No se pudo realizar la configuracion, codigo de algoritmo invalido");
			log_destroy(logger);
			exit(1);
		}
	}else{
		config_destroy(config_dictionary);
		log_error(logger, "No se pudo realizar la configuracion");
		log_destroy(logger);
		exit(1);

	}

	config_destroy(config_dictionary);
	return configuracion;
}

void limpiar_configuracion(void){
	free(configuracion.ip_coordinador);
	free(configuracion.nombre_instancia);
	free(configuracion.puerto_coordinador);
	free(configuracion.punto_montaje);
}

tipo_algoritmo get_enum_algoritmo(char* nombre_algoritmo){
	if(string_equals_ignore_case("CIRC", nombre_algoritmo))
		return CIRC;
	if(string_equals_ignore_case("LRU", nombre_algoritmo))
		return LRU;
	if(string_equals_ignore_case("BSU", nombre_algoritmo))
		return BSU;
	return -1;
}
