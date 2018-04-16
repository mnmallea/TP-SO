/*
 * config.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */
#include <stdlib.h>
#include <commons/config.h>
#include "config_coordinador.h"

config configurar(char *ruta){
	config configuracion;

	t_config *config_dictionary;
	config_dictionary = config_create(ruta);

	configuracion.puerto = config_get_int_value(config_dictionary, "PORT");
	configuracion.algoritmo = config_get_int_value(config_dictionary, "ALGORITMO");
	configuracion.cant_entradas = config_get_int_value(config_dictionary, "CANT_ENTRADAS");
	configuracion.entrada_size = config_get_int_value(config_dictionary, "ENTRADA_SIZE");
	configuracion.retardo = config_get_int_value(config_dictionary, "RETARDO");

	config_destroy(config_dictionary);
	return configuracion;
}
