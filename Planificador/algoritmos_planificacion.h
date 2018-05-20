/*
 * algoritmos_planificacion.h
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */


#ifndef ALGORITMOS_PLANIFICACION_H_
#define ALGORITMOS_PLANIFICACION_H_

#include <stdlib.h>
#include <commons/collections/list.h>

typedef enum {FIFO, SJFsD, SJFcD, HRRN } tipo_algoritmo_planif;
/* FIFO, SHORT JOB FIRST SIN DESALOJO, SHORT JOB FIRST CON DESALOJO, HRRN*/

typedef struct{

	//info del socket
	int socket;

	//info para calcular HRRN, SJF
	int dur_ult_raf;
	int estim_anter;
	int viene_esperando;
	char *clave_bloq;
} t_esi;

t_esi obtener_proximo_segun_fifo(t_list* lista_esis_actual, t_list* lista_esis_nueva);
t_esi obtener_proximo_segun_sjfsd(t_list* lista_esis_actual, t_list* lista_esis_nueva);
t_esi obtener_proximo_segun_sjfcd(t_list* lista_esis_actual, t_list* lista_esis_nueva);
t_esi obtener_proximo_segun_hrrn(t_list* lista_esis_actual, t_list* lista_esis_nueva);

#endif /* ALGORITMOS_PLANIFICACION_H_ */
