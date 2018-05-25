/*
 * consola.h
 *
 *  Created on: 12 may. 2018
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "algoritmos_planificacion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *menu(void *ptr);
void listar(char* rec);
void bloquear(char* clave,  int id);
void desbloquear(char* clave);

#endif /* CONSOLA_H_ */
