/*
 * algoritmos-planificacion.c
 *
 *  Created on: 3 may. 2018
 *      Author: utnso
 */

#include "algoritmos_planificacion.h"

t_esi* remover_esi_de_lista(t_list* lista, int id){
	bool esElEsi(void* esi){
		return id == ((t_esi*)esi)->id;
	}
	return list_remove_by_condition(lista, esElEsi);
}

bool menor_estimacion(void* esi1, void *esi2){
	return ((t_esi*)esi1)->estim_actual <= ((t_esi*)esi2)->estim_actual ;

}

bool mayor_response_ratio(void* esi1, void* esi2){
	return ((t_esi*)esi1)->response_ratio >= ((t_esi*)esi1)->response_ratio;
}


t_esi *obtener_proximo_segun_fifo(t_list *lista_esis){

	pthread_mutex_lock(&mutex_lista_esis_listos);
	t_esi *esi_elegido = list_remove(lista_esis, 0);
	pthread_mutex_unlock(&mutex_lista_esis_listos);
	return esi_elegido;

}

void obtener_proximas_rafagas(void* esi){
	double alfa = configuracion.alfa;
	log_trace(logger, "El alfa es %f", alfa);
	((t_esi*)esi)->estim_actual = alfa * ((t_esi*)esi)->estim_anter + (1-alfa) * ((t_esi*)esi)->dur_ult_raf;
	log_trace(logger, "esi %d, proxima rafaga estimada %f", ((t_esi*)esi)->id, ((t_esi*)esi)->estim_actual);

}

t_esi *obtener_proximo_segun_sjf(t_list *lista_esis){

	/*para cada esi de la lista debo calcularle la estimacion nueva,
	 * guardarla en la variable estim_nueva y despues obtener el que menor tenga
	 * al que elijo le cambio estim_anterior = estim_nueva
	 * devuelvo ese esi
	 */

	pthread_mutex_lock(&mutex_lista_esis_listos);
	t_list *lista_nueva = list_duplicate(lista_esis);
	list_iterate(lista_nueva, obtener_proximas_rafagas);
	list_sort(lista_nueva, menor_estimacion);

	t_esi *esi_elegido = list_get(lista_nueva, 0);
	remover_esi_de_lista(lista_esis, esi_elegido->id);

	pthread_mutex_unlock(&mutex_lista_esis_listos);

	esi_elegido->estim_anter = esi_elegido->estim_actual;
	list_destroy(lista_nueva);

	return esi_elegido;

}


void obtener_rr(void* esi){
	double alfa = configuracion.alfa;

	double estim_actual= alfa * ((t_esi*)esi)->estim_anter + (1-alfa) *( (t_esi*)esi)->dur_ult_raf;
	((t_esi*)esi)->response_ratio = (((t_esi*)esi)->viene_esperando + estim_actual)/ estim_actual;

}

t_esi *obtener_proximo_segun_hrrn(t_list *lista_esis){

	/*para cada esi de la lista debo calcularle la estimacion nueva,
	 * guardarla en la variable estim_nueva y despues obtener el que menor tenga
	 * al que elijo le cambio estim_anterior = estim_nueva
	 * devuelvo ese esi
	 */

	pthread_mutex_lock(&mutex_lista_esis_listos);
	t_list *lista_nueva = list_duplicate(lista_esis);
	list_iterate(lista_nueva, obtener_rr);
	list_sort(lista_nueva, mayor_response_ratio);
	t_esi *esi_elegido = list_get(lista_nueva, 0);
	remover_esi_de_lista(lista_esis, esi_elegido->id);

	pthread_mutex_unlock(&mutex_lista_esis_listos);

	esi_elegido->estim_anter = esi_elegido->estim_actual;
	list_destroy(lista_nueva);

	return esi_elegido;

}

