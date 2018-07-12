/*
 * instancia.h
 *
 *  Created on: 5 jun. 2018
 *      Author: utnso
 */

#ifndef INSTANCIA_H_
#define INSTANCIA_H_

#include <commons/collections/list.h>
#include <commons/log.h>
#include <stdbool.h>

#include "../syntax-commons/protocol.h"
#include "typedefs.h"

extern t_list *lista_instancias_disponibles;
extern t_list *lista_instancias_inactivas;
extern config configuracion;
extern t_log* logger;

void agregar_clave_almacenada(t_instancia*, char*);
bool tiene_clave_almacenada(t_instancia*, char*);
t_instancia* crear_instancia(int sockfd, char* nombre, int cant_entradas);
void liberar_instancia(t_instancia* instancia);
bool esta_activa_instancia(char* nombre);
bool esta_inactiva_instancia(char* nombre);
void instancia_desactivar(char*);
t_instancia* instancia_con_clave(char* clave);
void remover_clave_almacenada(t_instancia* instancia, char* clave);
int espacio_utilizado_por(char* clave);
t_instancia* sacar_instancia_de_lista(char* nombre, t_list* lista);
t_instancia* instancia_relevantar(char* nombre, int socket);
void instancia_agregar_a_inactivas(t_instancia* instancia);
void instancia_agregar_a_activas(t_instancia* instancia);
void realizar_compactacion();
t_instancia* instancia_con_clave(char* clave);
t_status_clave instancia_solicitar_valor_de_clave(t_instancia* instancia, char* clave, char** valor);
t_instancia* instancia_sacar_de_activas(char* nombre_instancia);

#endif /* INSTANCIA_H_ */
