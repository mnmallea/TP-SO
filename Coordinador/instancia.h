/*
 * instancia.h
 *
 *  Created on: 5 jun. 2018
 *      Author: utnso
 */

#ifndef INSTANCIA_H_
#define INSTANCIA_H_
#include <stdlib.h>
#include <stdbool.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include "typedefs.h"

void agregar_clave_almacenada(t_instancia*, char*);
bool tiene_clave_almacenada(t_instancia*, char*);
t_instancia* crear_instancia(int sockfd, char* nombre, int cant_entradas);
void liberar_instancia(t_instancia* instancia);

#endif /* INSTANCIA_H_ */
