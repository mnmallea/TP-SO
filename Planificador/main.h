/*
 * main.h
 *
 *  Created on: 17 abr. 2018
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

#define LOG_LEVEL LOG_LEVEL_TRACE
#define BACKLOG 5

#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include "config_planificador.h"
#include "selector.h"
#include "consola.h"

config configuracion;

t_list *lista_esis_listos;
t_esi *esi_corriendo;
t_list *lista_esis_finalizados;

/*Nombre: dic_esis_bloqueados
 * Estructura: char* clave bloqueada, t_list esis_bloqueados
 *
 * Se agrega una linea cuando un esi solicita una clave que esta bloqueada (GET del mismo)
 * Se elimina una linea cuando un esi libera un recurso (STORE del mismo)
 */
t_dictionary *dic_esis_bloqueados;

/*Nombre: dic_esi_recurso_bloq
 * Estructura: t_esi esi, char* clave retenida
 *
 * Se agrega una linea cuando un esi solicita un recurso (GET del mismo)
 * Se elimina una linea cuando un esi libera un recurso (STORE del mismo)
 */
t_dictionary *dic_esi_recurso_bloq;



//#include "typescommons.h"

#endif /* MAIN_H_ */
