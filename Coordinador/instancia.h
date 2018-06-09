/*
 * instancia.h
 *
 *  Created on: 5 jun. 2018
 *      Author: utnso
 */

#ifndef INSTANCIA_H_
#define INSTANCIA_H_

#include <commons/collections/list.h>
#include <stdbool.h>

#include "typedefs.h"

extern t_list *lista_instancias_disponibles;
extern t_list *lista_instancias_inactivas;

void agregar_clave_almacenada(t_instancia*, char*);
bool tiene_clave_almacenada(t_instancia*, char*);
t_instancia* crear_instancia(int sockfd, char* nombre, int cant_entradas);
void liberar_instancia(t_instancia* instancia);
bool esta_activa_instancia(char* nombre);
bool esta_inactiva_instancia(char* nombre);
void instancia_desactivar(t_instancia*);
t_instancia* instancia_con_clave(char* clave);
void remover_clave_almacenada(t_instancia* instancia, char* clave);

#endif /* INSTANCIA_H_ */
