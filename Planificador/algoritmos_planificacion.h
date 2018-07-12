/*
 * algoritmos_planificacion.h
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */

#ifndef ALGORITMOS_PLANIFICACION_H_
#define ALGORITMOS_PLANIFICACION_H_

#include <stdbool.h>
#include <unistd.h>
#include <stdbool.h>

#include <commons/collections/list.h>
#include <commons/log.h>

#include "config_planificador.h"
#include "sincronizacion.h"
#include "typedefs.h"

extern config configuracion;

t_esi *obtener_proximo_segun_fifo(t_list *lista_esis);
t_esi *obtener_proximo_segun_sjf(t_list *lista_esis);
t_esi *obtener_proximo_segun_hrrn(t_list *lista_esis);
t_esi* remover_esi_de_lista(t_list*, int);

void obtener_proximas_rafagas(void* esi);
void obtener_rr(void* esi);

bool menor_estimacion(void* esi1, void *esi2);
bool mayor_response_ratio(void* esi1, void* esi2);

#endif /* ALGORITMOS_PLANIFICACION_H_ */
