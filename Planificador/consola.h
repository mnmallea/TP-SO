/*
 * consola.h
 *
 *  Created on: 12 may. 2018
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <commons/log.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>

#include "../syntax-commons/protocol.h"
#include "../syntax-commons/serializador.h"

#include "planificacion.h"
#include "funciones_auxiliares_consola.h"


#define LOG_LEVEL LOG_LEVEL_TRACE
#define BACKLOG 5

//extern int flag;
extern t_log *logger;
extern int socketCord ; //sale del selector
extern char** buffer;

bool planificacion_pausada;

t_esi* esi_a_matar_por_consola;
t_esi* esi_a_bloquear_por_consola;
char* clave_a_bloquear;

void *menu(void *ptr);
void listar(char* rec);
void bloquear(char* clave, int id);
void desbloquear(char* clave);
void pausar_despausar_consola();
void mostrar_esi_en_pantalla(void* esi);
void matar_por_consola(int id);
void deadlock();
void envia_status_clave(char* clave);
void show_respuesta_status_clave(respuesta_status_clave_t*);

#endif /* CONSOLA_H_ */
