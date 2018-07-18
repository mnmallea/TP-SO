/*
 * main.h
 *
 *  Created on: 3 may. 2018
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <commons/collections/list.h>
#include <commons/log.h>
#include <pthread.h>

#include "typedefs.h"

#define LOG_LEVEL LOG_LEVEL_TRACE
#define BACKLOG 5

int socket_planificador;
t_log *logger;
config configuracion;
t_list *lista_instancias_disponibles;
t_list *lista_instancias_inactivas;
pthread_t thread_listener;

#endif /* MAIN_H_ */
