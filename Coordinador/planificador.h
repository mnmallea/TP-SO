/*
 * planificador.h
 *
 *  Created on: 5 jun. 2018
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <commons/log.h>

#include "../syntax-commons/protocol.h"
#include "typedefs.h"

extern t_log* logger;
extern int socket_planificador;

t_protocolo respuesta_planificador;

void solicitar_clave(char* clave, t_esi* esi);
void esi_tiene_clave(char* clave, t_esi* esi);
void informar_liberacion_clave(char* clave);
void informar_instancia_caida(t_instancia* instancia);
void informar_status_clave(char* clave);

#endif /* PLANIFICADOR_H_ */
