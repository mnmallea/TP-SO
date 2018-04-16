/*
 * config.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include "config_coordinador.h"

config configurar(char *ruta){
	log_info(logger,"Configurando coordinador");
	config configuracion;

	t_config *config_dictionary;
	config_dictionary = config_create(ruta);

	if(config_dictionary == NULL)
		exit_error_with_msg("No se pudo crear la configuración (no se encuentra el archivo)");

	if(		config_has_property(config_dictionary, "PORT") &&
			config_has_property(config_dictionary, "ALGORITMO") &&
			config_has_property(config_dictionary, "CANT_ENTRADAS") &&
			config_has_property(config_dictionary, "ENTRADA_SIZE") &&
			config_has_property(config_dictionary, "RETARDO")){
		configuracion.puerto = config_get_int_value(config_dictionary, "PORT");
		configuracion.algoritmo = config_get_int_value(config_dictionary, "ALGORITMO");
		configuracion.cant_entradas = config_get_int_value(config_dictionary, "CANT_ENTRADAS");
		configuracion.entrada_size = config_get_int_value(config_dictionary, "ENTRADA_SIZE");
		configuracion.retardo = config_get_int_value(config_dictionary, "RETARDO");
	}else{
		config_destroy(config_dictionary);
		exit_error_with_msg("El archivo de configuracion esta incorrecto");
	}

	config_destroy(config_dictionary);
	return configuracion;
}
