/*
 * main.h
 *
 *  Created on: 17 abr. 2018
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/log.h>

#include "../syntax-commons/protocol.h"

#include "config_planificador.h"
#include "selector.h"
#include "consola.h"
#include "planificacion.h"

#define LOG_LEVEL LOG_LEVEL_TRACE
#define BACKLOG 5

config configuracion;
t_log *logger;


void configurar_claves_inicialmente_bloqueadas();


#endif /* MAIN_H_ */
