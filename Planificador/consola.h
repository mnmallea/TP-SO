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


#define LOG_LEVEL LOG_LEVEL_TRACE
#define BACKLOG 5

//extern int flag;
extern t_log *logger;
extern int socketCord ; //sale del selector

bool planificacion_pausada;

void *menu(void *ptr);
void listar(char* rec);
void bloquear(char* clave, int id);
void desbloquear(char* clave);
void pausar_despausar_consola();
void mostrar_esi_en_pantalla(void* esi);
void matar_por_consola(int id);
void deadlock();
void envia_status_clave(char* clave);

#endif /* CONSOLA_H_ */
