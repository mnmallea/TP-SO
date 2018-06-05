/*
 * main.h
 *
 *  Created on: 3 may. 2018
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_
#include <stdlib.h>
#include <commons/collections/list.h>
#include "../syntax-commons/my_socket.h"
#include "algoritmos_distribucion.h"
#include "config_coordinador.h"
#include "servidor.h"
#include "sincronizacion.h"
#include "log_operaciones.h"

#define LOG_LEVEL LOG_LEVEL_TRACE
#define BACKLOG 5

t_log *logger;
config configuracion;
t_list *lista_instancias_disponibles;
t_list *lista_esis_disponibles;
pthread_t thread_listener;



#endif /* MAIN_H_ */
