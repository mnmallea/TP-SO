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

typedef struct{

	//info del socket
	int socket;

	//info para calcular HRRN, SJF
	int dur_ult_raf;
	int estim_anter; //HAY QUE CAMBIAR ESTO A DOUBLE
	double estim_actual;

	int viene_esperando;
	double response_ratio;
	char *clave_bloq;
} t_esi;

t_esi *obtener_proximo_segun_fifo(t_list *lista_esis);
t_esi *obtener_proximo_segun_sjf(t_list *lista_esis);
t_esi *obtener_proximo_segun_hrrn(t_list *lista_esis);

#endif /* ALGORITMOS_PLANIFICACION_H_ */
