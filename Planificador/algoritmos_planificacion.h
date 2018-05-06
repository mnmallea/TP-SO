/*
 * algoritmos_planificacion.h
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */
#include <commons/collections/list.h>
#include "main.h"

#ifndef ALGORITMOS_PLANIFICACION_H_
#define ALGORITMOS_PLANIFICACION_H_

typedef enum {FIFO, SJFsD, SJFcD, HRRN } tipo_algoritmo_planif;
/* FIFO, SHORT JOB FIRST SIN DESALOJO, SHORT JOB FIRST CON DESALOJO, HRRN*/

t_esi obtener_proximo_segun_fifo(t_list* lista_esis_actual, t_list* lista_esis_nueva);
t_esi obtener_proximo_segun_sjfsd(t_list* lista_esis_actual, t_list* lista_esis_nueva);
t_esi obtener_proximo_segun_sjfcd(t_list* lista_esis_actual, t_list* lista_esis_nueva);
t_esi obtener_proximo_segun_hrrn(t_list* lista_esis_actual, t_list* lista_esis_nueva);

#endif /* ALGORITMOS_PLANIFICACION_H_ */
