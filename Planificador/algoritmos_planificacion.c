/*
 * algoritmos-planificacion.c
 *
 *  Created on: 3 may. 2018
 *      Author: utnso
 */

#include "algoritmos_planificacion.h"

t_esi* remover_esi_de_lista(t_list* lista, int id) {
	bool esElEsi(void* esi) {
		return id == ((t_esi*) esi)->id;
	}
	return list_remove_by_condition(lista, esElEsi);
}

double remaining_time(t_esi* esi) {
	return (esi->estim_actual - esi->rafaga_actual);
}

bool menor_estimacion(void* esi1, void *esi2) {
	return ((t_esi*) esi1)->estim_actual <= ((t_esi*) esi2)->estim_actual;

}

bool mayor_response_ratio(void* esi1, void* esi2) {
	return ((t_esi*) esi1)->response_ratio >= ((t_esi*) esi1)->response_ratio;
}

bool menor_remaining_time(void* esi1, void *esi2) {
	return ( remaining_time( ((t_esi*) esi1) ) <= remaining_time( ((t_esi*) esi2)) );

}

t_esi *obtener_proximo_segun_fifo(t_list *lista_esis) {

	//log_trace(logger, "Se procede a buscar al proximo esi a ejecutar segun FIFO");
	pthread_mutex_lock(&mutex_lista_esis_listos);
	t_esi *esi_elegido = list_remove(lista_esis, 0);
	//log_trace(logger, "El esi elegido(en orden de llegada) es: %d", esi_elegido->id);
	pthread_mutex_unlock(&mutex_lista_esis_listos);
	return esi_elegido;

}

void obtener_proximas_rafagas(void* _esi) {
	double alfa = configuracion.alfa;
	t_esi* esi = _esi;
	log_trace(logger, "El alfa es %f", alfa);
	esi->estim_actual = alfa * esi->estim_anter
			+ (1 - alfa) * esi->rafaga_anterior;

	log_info(logger, "La ESTIMACIÓN de la proxima rafaga para el ESI %d es %f, la estimacion anterior era %f, rafaga anterior: %d",
			esi->id, esi->estim_actual, esi->estim_anter, esi->rafaga_anterior);

}

t_esi *obtener_proximo_segun_sjf(t_list *lista_esis) {

	/*para cada esi de la lista debo calcularle la estimacion nueva,
	 * guardarla en la variable estim_nueva y despues obtener el que menor tenga
	 * al que elijo le cambio estim_anterior = estim_nueva
	 * devuelvo ese esi
	 */

	//log_trace(logger, "Se procede a buscar al proximo esi a ejecutar segun SJF");
	//log_trace(logger, "antes de tomar mutex");
	pthread_mutex_lock(&mutex_lista_esis_listos);
	//log_trace(logger, "tomo mutex sjf");
	t_list *lista_nueva = list_duplicate(lista_esis); //aca rompe
	log_trace(logger, "Creo lista");
	list_iterate(lista_nueva, obtener_proximas_rafagas);
	log_trace(logger, "Itera lista");
	list_sort(lista_nueva, menor_estimacion);
	log_trace(logger, "ordena lista");

	t_esi *esi_elegido = list_get(lista_nueva, 0);
	remover_esi_de_lista(lista_esis, esi_elegido->id);

	log_trace(logger, "El esi elegido(con la menor proxima rafaga) es: %d",
			esi_elegido->id);

	pthread_mutex_unlock(&mutex_lista_esis_listos);

	esi_elegido->estim_anter = esi_elegido->estim_actual;
	list_destroy(lista_nueva);

	return esi_elegido;

}

void obtener_rr(void* _esi) {
	double alfa = configuracion.alfa;

	t_esi* esi = _esi;
		log_trace(logger, "El alfa es %f", alfa);
		esi->estim_actual = alfa * esi->estim_anter
				+ (1 - alfa) * esi->rafaga_anterior;

	esi->response_ratio = (esi->viene_esperando + esi->estim_actual) / esi->estim_actual;

	log_info(logger, "La ESTIMACIÓN de la proxima rafaga para el ESI %d es %f, la estimacion anterior era %f, rafaga anterior: %d",
				esi->id, esi->estim_actual, esi->estim_anter, esi->rafaga_anterior);

	log_info(logger, "El response ratio para el ESI %d es %f, la estimacion es %f, esperando: %d",
				((t_esi*) esi)->id, ((t_esi*) esi)->response_ratio, ((t_esi*) esi)->estim_actual, ((t_esi*) esi)->viene_esperando);


}

t_esi *obtener_proximo_segun_hrrn(t_list *lista_esis) {

	/*para cada esi de la lista debo calcularle la estimacion nueva,
	 * guardarla en la variable estim_nueva y despues obtener el que menor tenga
	 * al que elijo le cambio estim_anterior = estim_nueva
	 * devuelvo ese esi
	 */

	log_trace(logger,
			"Se procede a buscar al proximo esi a ejecutar segun HRRN");
	pthread_mutex_lock(&mutex_lista_esis_listos);
	t_list *lista_nueva = list_duplicate(lista_esis);
	list_iterate(lista_nueva, obtener_rr);
	list_sort(lista_nueva, mayor_response_ratio);
	t_esi *esi_elegido = list_get(lista_nueva, 0);
	log_trace(logger, "El esi elegido(con el mayor response ratio) es: %d",
			esi_elegido->id);
	remover_esi_de_lista(lista_esis, esi_elegido->id);

	pthread_mutex_unlock(&mutex_lista_esis_listos);

	esi_elegido->estim_anter = esi_elegido->estim_actual;
	esi_elegido->viene_esperando = 0; //reseteo el viene esperando
	list_destroy(lista_nueva);

	return esi_elegido;

}



t_esi *obtener_proximo_segun_sjfcd(t_list *lista_esis) {

	/*para cada esi de la lista debo calcularle la estimacion nueva,
	 * guardarla en la variable estim_nueva y despues obtener el que menor tenga
	 * al que elijo le cambio estim_anterior = estim_nueva
	 * devuelvo ese esi
	 */

	//log_trace(logger, "Se procede a buscar al proximo esi a ejecutar segun SJF");
	//log_trace(logger, "antes de tomar mutex");
	pthread_mutex_lock(&mutex_lista_esis_listos);
	t_list *lista_nueva = list_duplicate(lista_esis);
	//log_trace(logger, "Creo lista");
	list_iterate(lista_nueva, obtener_proximas_rafagas);
	//log_trace(logger, "Itera lista");
	list_sort(lista_nueva, menor_remaining_time);
	//log_trace(logger, "ordena lista");

	t_esi *esi_elegido = list_get(lista_nueva, 0);
	pthread_mutex_unlock(&mutex_lista_esis_listos);

	list_destroy(lista_nueva);

	return esi_elegido;

}

