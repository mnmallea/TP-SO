/*
 * planificador.h
 *
 *  Created on: 5 jun. 2018
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_
#include <string.h>
#include "typedefs.h"
#include "../syntax-commons/serializador.h"

extern int socket_planificador;

void solicitar_clave(char* clave);
void esi_tiene_clave(char* clave);
void informar_liberacion_clave(char* clave);

#endif /* PLANIFICADOR_H_ */
