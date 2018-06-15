/*
 * consola.h
 *
 *  Created on: 12 may. 2018
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "planificacion.h"
#include "sincronizacion.h"
#include <commons/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_LEVEL LOG_LEVEL_TRACE
#define BACKLOG 5

//extern int flag;
extern t_log *logger;

void *menu(void *ptr);
void listar(char* rec);
void bloquear(char* clave, int id);
void desbloquear(char* clave);
void pausar_despausar_consola();
void mostrar_esi_en_pantalla(void* esi);
void matar_por_consola(int id);
void deadlock();

#endif /* CONSOLA_H_ */
