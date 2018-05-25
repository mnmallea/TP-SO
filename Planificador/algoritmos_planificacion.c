/*
 * algoritmos-planificacion.c
 *
 *  Created on: 3 may. 2018
 *      Author: utnso
 */

#include "algoritmos_planificacion.h"
#include <stdbool.h>


bool menor_estimacion(void* esi1, void *esi2){
	return ((t_esi*)esi1)->estim_actual <= ((t_esi*)esi2)->estim_actual ;

}

bool mayor_response_ratio(void* esi1, void* esi2){
	return ((t_esi*)esi1)->response_ratio >= ((t_esi*)esi1)->response_ratio;
}

void matar_nodo_esi(void* esi){
	free(((t_esi*)esi));
}


t_esi *obtener_proximo_segun_fifo(t_list *lista_esis){

	t_esi *esi_elegido = list_remove(lista_esis, 0);
	return esi_elegido;

}

void obtener_proximas_rafagas(void* esi){
	int alfa = 0.5; //ACA HAY QUE USAR EL DEL ARCHIVO DE CONFIGURACION
	((t_esi*)esi)->estim_actual = alfa * ((t_esi*)esi)->estim_anter + (1-alfa) * ((t_esi*)esi)->dur_ult_raf;

}

t_esi *obtener_proximo_segun_sjf(t_list *lista_esis){

	/*para cada esi de la lista debo calcularle la estimacion nueva,
	 * guardarla en la variable estim_nueva y despues obtener el que menor tenga
	 * al que elijo le cambio estim_anterior = estim_nueva
	 * devuelvo ese esi
	 */


	t_list *lista_nueva = list_duplicate(lista_esis);
	list_iterate(lista_nueva, obtener_proximas_rafagas);
	list_sort(lista_nueva, menor_estimacion);

	t_esi *esi_elegido = list_remove(lista_nueva, 0);
	esi_elegido->estim_anter = esi_elegido->estim_actual;
	list_destroy_and_destroy_elements(lista_nueva, matar_nodo_esi);

	return esi_elegido;

}


void obtener_rr(void* esi){
	int alfa = 0.5;//aca hay q usar el configg

	double estim_actual= alfa * ((t_esi*)esi)->estim_anter + (1-alfa) *( (t_esi*)esi)->dur_ult_raf;
	((t_esi*)esi)->response_ratio = (((t_esi*)esi)->viene_esperando + estim_actual)/ estim_actual;

}

t_esi *obtener_proximo_segun_hrrn(t_list *lista_esis){

	/*para cada esi de la lista debo calcularle la estimacion nueva,
	 * guardarla en la variable estim_nueva y despues obtener el que menor tenga
	 * al que elijo le cambio estim_anterior = estim_nueva
	 * devuelvo ese esi
	 */


	t_list *lista_nueva = list_duplicate(lista_esis);
	list_iterate(lista_nueva, obtener_rr);
	list_sort(lista_nueva, mayor_response_ratio);
	t_esi *esi_elegido = list_remove(lista_nueva, 0);
	esi_elegido->estim_anter = esi_elegido->estim_actual;
	list_destroy_and_destroy_elements(lista_nueva, matar_nodo_esi);

	return esi_elegido;

}

