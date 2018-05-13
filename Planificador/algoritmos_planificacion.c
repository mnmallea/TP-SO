/*
 * algoritmos-planificacion.c
 *
 *  Created on: 3 may. 2018
 *      Author: utnso
 */

#include "algoritmos_planificacion.h"

t_esi obtener_proximo_segun_fifo(t_list* lista_esis_actual, t_list* lista_esis_nueva){

	t_esi* esi_elegido = malloc(sizeof(t_esi));
	esi_elegido = list_remove(lista_esis_actual, 0);
	list_add(lista_esis_nueva, esi_elegido);
	return *esi_elegido;

}
