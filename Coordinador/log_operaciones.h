/*
 * log_operaciones.h
 *
 *  Created on: 25 may. 2018
 *      Author: utnso
 */
#ifndef LOG_OPERACIONES_H_
#define LOG_OPERACIONES_H_
#include <commons/log.h>
#include <commons/string.h>
#include <stdlib.h>


t_log* log_operaciones;

void crear_log_operaciones();
void destruir_log_operaciones();
void logear_get(int n_esi, char* clave);
void logear_set(int n_esi, char* clave, char* valor);
void logear_store(int n_esi, char* clave);

#endif /* LOG_OPERACIONES_H_ */
