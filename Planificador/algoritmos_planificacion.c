/*
 * algoritmos-planificacion.c
 *
 *  Created on: 3 may. 2018
 *      Author: utnso
 */

#include "algoritmos_planificacion.h"
#include <stdbool.h>


bool menor_estimacion(t_esi *esi1, t_esi *esi2){
	return esi1->estim_actual < esi2->estim_actual;
}

bool mayor_response_ratio(t_esi *esi1, t_esi *esi2){
	return esi1->response_ratio > esi2->response_ratio;
}

void matar_nodo_esi(t_esi *esi){
	free(esi);
}


t_esi *obtener_proximo_segun_fifo(t_list *lista_esis){

	t_esi *esi_elegido = list_remove(lista_esis, 0);
	return esi_elegido;

}


t_esi *obtener_proximo_segun_sjf(t_list *lista_esis){

	/*para cada esi de la lista debo calcularle la estimacion nueva,
	 * guardarla en la variable estim_nueva y despues obtener el que menor tenga
	 * al que elijo le cambio estim_anterior = estim_nueva
	 * devuelvo ese esi
	 */


	t_list *lista_duplic = list_duplicate(lista_esis);
	t_list *lista_nueva = list_create();
	int cant_elementos = list_size(lista_duplic);
	int alfa = 0.5;

	for(int i=0; i< cant_elementos; i++){
		t_esi *esi_actual = list_get(lista_duplic, i);
		esi_actual->estim_actual = alfa * esi_actual->estim_anter + (1-alfa) * esi_actual->dur_ult_raf;
		list_add(lista_nueva, esi_actual);
	}

	list_sort(lista_nueva, (void*)menor_estimacion);
	t_esi *esi_elegido = list_remove(lista_nueva, 0);
	esi_elegido->estim_anter = esi_elegido->estim_actual;
	lista_esis=lista_nueva;

	list_destroy_and_destroy_elements(lista_nueva, (void*)free);
	list_destroy_and_destroy_elements(lista_duplic, (void*)free);

	return esi_elegido;

}


t_esi *obtener_proximo_segun_hrrn(t_list *lista_esis){

	/*para cada esi de la lista debo calcularle el response ratio,
	* guardarla en la variable response_ratio y despues obtener el que mayor tenga
	* devuelvo ese esi
	*/


		t_list *lista_duplic = list_duplicate(lista_esis);
		t_list *lista_nueva = list_create();
		int cant_elementos = list_size(lista_duplic);
		int alfa = 0.5;

		for(int i=0; i< cant_elementos; i++){
			t_esi *esi_actual = list_get(lista_duplic, i);

			double estim_actual= alfa * esi_actual->estim_anter + (1-alfa) * esi_actual->dur_ult_raf;
			esi_actual->response_ratio = (esi_actual->viene_esperando + estim_actual)/ estim_actual;
			esi_actual->estim_actual = estim_actual; //esto sirve para algo??
			list_add(lista_nueva, esi_actual);
		}

		list_sort(lista_nueva, (void*)mayor_response_ratio);
		t_esi *esi_elegido = list_remove(lista_nueva, 0);
		esi_elegido->estim_anter = esi_elegido->estim_actual;
		lista_esis=lista_nueva;

		list_destroy_and_destroy_elements(lista_nueva, (void*)free);
		list_destroy_and_destroy_elements(lista_duplic, (void*)free);

		return esi_elegido;
}
