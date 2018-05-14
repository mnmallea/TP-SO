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
t_list lista_esis_corriendo;
t_list lista_esis_bloq_consola;
t_list lista_esis_bloq_rec;
t_list lista_esis_finalizados;

//#include "typescommons.h"

#endif /* MAIN_H_ */
