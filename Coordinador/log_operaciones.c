/*
 * log_operaciones.c
 *
 *  Created on: 25 may. 2018
 *      Author: utnso
 */
#include "log_operaciones.h"

void crear_log_operaciones() {
	log_operaciones = log_create("log_operaciones.log", "Coordinador", false,
			LOG_LEVEL_INFO);
	log_info(log_operaciones, "%-15s%s", "ESI", "Operación");
}
void destruir_log_operaciones() {
	log_info(log_operaciones, "\n\n");
	log_destroy(log_operaciones);
}

void logear_get(int n_esi, char* clave) {
	char *nombre_esi = string_from_format("ESI %d", n_esi);
	log_info(log_operaciones, "%-15sGET %s", nombre_esi, clave);
	free(nombre_esi);
}

void logear_set(int n_esi, char* clave, char* valor) {
	char *nombre_esi = string_from_format("ESI %d", n_esi);
	log_info(log_operaciones, "%-15sSET %s %s", nombre_esi, clave, valor);
	free(nombre_esi);
}

void logear_store(int n_esi, char* clave) {
	char *nombre_esi = string_from_format("ESI %d", n_esi);
	log_info(log_operaciones, "%-15sSTORE %s", nombre_esi, clave);
	free(nombre_esi);
}
