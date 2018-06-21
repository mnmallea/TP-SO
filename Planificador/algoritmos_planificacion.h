/*
 * algoritmos_planificacion.h
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */

#ifndef ALGORITMOS_PLANIFICACION_H_
#define ALGORITMOS_PLANIFICACION_H_

#include <commons/collections/list.h>
#include <stdbool.h>

#include "config_planificador.h"

typedef enum {
	FIFO, SJFsD, SJFcD, HRRN
} tipo_algoritmo_planif;

typedef struct {

	//info del socket
	int id;
	int socket;

	//info para calcular HRRN, SJF
	int dur_ult_raf; //Se suma uno cada vez que si soy esi corriendo finalizo una rafaga
	double estim_anter;
	double estim_actual;

	int viene_esperando; //Se suma uno cada vez que un esi que no es el finaliza una linea
	double response_ratio;
} t_esi;

extern config configuracion;

t_esi *obtener_proximo_segun_fifo(t_list *lista_esis);
t_esi *obtener_proximo_segun_sjf(t_list *lista_esis);
t_esi *obtener_proximo_segun_hrrn(t_list *lista_esis);

void obtener_proximas_rafagas(void* esi);
void obtener_rr(void* esi);

bool menor_estimacion(void* esi1, void *esi2);
bool mayor_response_ratio(void* esi1, void* esi2);

void matar_nodo_esi(void* esi);
t_esi* remover_esi_de_lista(t_list*, int);

#endif /* ALGORITMOS_PLANIFICACION_H_ */
