/*
 * planificador.h
 *
 *  Created on: 5 jun. 2018
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <commons/log.h>

#include "typedefs.h"

extern t_log* logger;
extern int socket_planificador;

void solicitar_clave(char* clave);
void esi_tiene_clave(char* clave);
void informar_liberacion_clave(char* clave);
void informar_instancia_caida(t_instancia* instancia);

#endif /* PLANIFICADOR_H_ */
