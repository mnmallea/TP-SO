/*
 * planificador.h
 *
 *  Created on: 5 jun. 2018
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_
#include "typedefs.h"

void solicitar_clave(int n_esi, char* clave);
void tiene_esi_clave(int n_esi, char* clave);
void informar_liberacion_clave(char* clave);

#endif /* PLANIFICADOR_H_ */
