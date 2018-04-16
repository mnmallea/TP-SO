/*
 * config.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include "config_coordinador.h"
#include "error.h"

config* configurar(char *ruta){
	log_info(logger,"Configurando coordinador");
	config* configuracion = malloc(sizeof(*configuracion));

	t_config *config_dictionary = config_create(ruta);

	if(config_dictionary == NULL){
		eliminar_configuracion(configuracion);
		exit_error_with_msg("No se pudo crear la configuración (no se encuentra el archivo)");
	}

	if(		config_has_property(config_dictionary, "PORT") &&
			config_has_property(config_dictionary, "ALGORITMO") &&
			config_has_property(config_dictionary, "CANT_ENTRADAS") &&
			config_has_property(config_dictionary, "ENTRADA_SIZE") &&
			config_has_property(config_dictionary, "RETARDO")){
		configuracion->puerto = string_duplicate(config_get_string_value(config_dictionary, "PORT"));//hay que copiarselo porque al final borras el diccionario
		configuracion->algoritmo = config_get_int_value(config_dictionary, "ALGORITMO");
		configuracion->cant_entradas = config_get_int_value(config_dictionary, "CANT_ENTRADAS");
		configuracion->entrada_size = config_get_int_value(config_dictionary, "ENTRADA_SIZE");
		configuracion->retardo = config_get_int_value(config_dictionary, "RETARDO");
	}else{
		config_destroy(config_dictionary);
		eliminar_configuracion(configuracion);
		exit_error_with_msg("El archivo de configuracion esta incorrecto");
	}

	config_destroy(config_dictionary);
	return configuracion;
}

void eliminar_configuracion(config* configuracion){
	if(configuracion != NULL){//Conditional jump or move depends on uninitialised value(s)
		free(configuracion->puerto);
		free(configuracion);
	}
}


