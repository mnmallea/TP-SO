/*
 * main.h
 *
 *  Created on: 17 abr. 2018
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

#define LOG_LEVEL LOG_LEVEL_TRACE
#define BACKLOG 5

#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include "config_planificador.h"
#include "selector.h"
#include "consola.h"

config configuracion;

t_list *lista_esis_listos;
t_esi *esi_corriendo;
t_list *lista_esis_finalizados;
t_dictionary *dic_esis_bloqueados;


void obtener_nuevo_esi_a_correr();
void bloquear_esi(t_esi* esi, char* clave);
void finalizar_esi(t_esi* esi);
void pasar_esi_bloqueado_a_listo(t_esi* esi);
void se_desbloqueo_un_recurso(char* clave);
void nuevo_esi(t_esi* esi);


//#include "typescommons.h"

#endif /* MAIN_H_ */
