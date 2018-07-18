/*
 * error.h
 *
 *  Created on: 16 abr. 2018
 *      Author: utnso
 */

#ifndef ERROR_H_
#define ERROR_H_

#include <commons/collections/list.h>
#include <commons/log.h>

#include "typedefs.h"

extern t_log *logger;

extern int socket_planificador;
extern t_log *logger;
extern config configuracion;
extern t_list *lista_instancias_disponibles;
extern t_list *lista_instancias_inactivas;

void exit_error_with_msg(char* msg);
void morir_liberando_recursos(int retorno);

#endif /* ERROR_H_ */
