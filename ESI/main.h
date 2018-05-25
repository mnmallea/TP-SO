/*
 * main.h
 *
 *  Created on: 12 may. 2018
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <commons/log.h>
#include "../syntax-commons/my_socket.h"
#include "config_esi.h"
//#include "../syntax-commons/protocol.h"
//#include "../syntax-commons/conexiones.h"
#include <parsi/parser.h>


#define CANT_ARGUMENTOS_MAIN 3
#define LOG_LEVEL LOG_LEVEL_TRACE

t_log* logger;

typedef struct //no usarse
{
int keyword;
char* clave[40];
size_t sizeClave;
char* valor[40];
int sizeValor;
}t_operacion;

config configuracion; //no le pongan otro nombre, porque despues limpiar_configuracion() se va a encargar de borrarla
t_operacion *n_operacion;

#endif /* MAIN_H_ */
