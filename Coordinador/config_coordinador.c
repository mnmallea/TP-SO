/*
 * config.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */
#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include "config_coordinador.h"
#include "error.h"

void configurar(char *ruta) {
	log_info(logger, "Configurando coordinador");

	t_config *config_dictionary = config_create(ruta);

	if (config_dictionary == NULL) {
		log_error(logger,
				"No se pudo crear la configuración (no se encuentra el archivo)");
		log_destroy(logger);
		exit(EXIT_FAILURE);
	}

	if (config_has_property(config_dictionary, "PORT")
			&& config_has_property(config_dictionary, "ALGORITMO")
			&& config_has_property(config_dictionary, "CANT_ENTRADAS")
			&& config_has_property(config_dictionary, "ENTRADA_SIZE")
			&& config_has_property(config_dictionary, "RETARDO")) {
		strncpy(configuracion.puerto,
				config_get_string_value(config_dictionary, "PORT"),
				PORT_MAX_STRING_LENGTH); //hay que copiarselo porque al final borras el diccionario
		configuracion.puerto[5] = '\0';
		configuracion.algoritmo = get_algoritmo_distribucion(
				config_get_string_value(config_dictionary, "ALGORITMO"));
		configuracion.cant_entradas = config_get_int_value(config_dictionary,
				"CANT_ENTRADAS");
		configuracion.entrada_size = config_get_int_value(config_dictionary,
				"ENTRADA_SIZE");
		configuracion.retardo = config_get_int_value(config_dictionary,
				"RETARDO");
	} else {
		config_destroy(config_dictionary);
		log_error(logger,
				"El archivo de configuracion esta incorrecto");
		log_destroy(logger);
		exit(EXIT_FAILURE);
	}

	config_destroy(config_dictionary);
}

void limpiar_configuracion(void) {
	/*
	 * Polimorfismo(?
	 */
}

t_algoritmo get_algoritmo_distribucion(char* nombre_algoritmo) {
	if (string_equals_ignore_case(nombre_algoritmo, "LSU"))
		return LSU;
	if (string_equals_ignore_case(nombre_algoritmo, "KE"))
		return KE;
	if (string_equals_ignore_case(nombre_algoritmo, "EL"))
		return EL;
	exit_error_with_msg("Nombre del algoritmo desconocido");
	return -1;
}
