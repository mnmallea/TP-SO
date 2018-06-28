/*
 * algoritmos_distribucion.c
 *
 *  Created on: 3 may. 2018
 *      Author: utnso
 */

#include "algoritmos_distribucion.h"

#include <commons/string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

#include "instancia.h"
#include "typedefs.h"

int instancia_a_utilizar = 0;
int largo_lista;

/*
 * Obtiene instancia segun el algoritmo Equitative Load
 */
t_instancia* obtener_instancia_segun_EL(char* clave) {
	t_instancia* inst_elegida = list_get(lista_instancias_disponibles,
			instancia_a_utilizar);
	largo_lista = list_size(lista_instancias_disponibles);
	instancia_a_utilizar++;

	if (instancia_a_utilizar == largo_lista) //recien utilice la ultima posicion
		instancia_a_utilizar = 0;

	return inst_elegida;
}

/*
 * Obtiene instancia segun el algoritmo Least Space Used
 */
t_instancia* obtener_instancia_segun_LSU(char* clave) {

	t_instancia* instancia_elegida = instancia_con_menos_espacio_usado(
			lista_instancias_disponibles);
	sacar_instancia_de_lista(instancia_elegida->nombre,
			lista_instancias_disponibles);
	list_add(lista_instancias_disponibles, instancia_elegida);

	return instancia_elegida;
}

bool tieneMasEspacioLibre(void* una_instancia, void* otra_instancia) {
	return ((t_instancia*) una_instancia)->cant_entradas_vacias
			> ((t_instancia*) otra_instancia)->cant_entradas_vacias;
}

/*
 * Obtiene instancia segun el algoritmo indicado en la configuracion
 * Thread safe (mutex implementado sobre la lista de instancias disponibles)
 */
t_instancia* obtener_instancia_siguiente(char* clave) {
	t_instancia * instancia;
	if ((instancia = instancia_con_clave(clave)) != NULL) {
		return instancia;
	}

	sem_wait(&contador_instancias_disponibles);
	pthread_mutex_lock(&mutex_instancias_disponibles);
	switch (configuracion.algoritmo) {
	case EL:
		return obtener_instancia_segun_EL(clave);
		break;
	case LSU:
		return obtener_instancia_segun_LSU(clave);
		break;
	case KE:
		return obtener_instancia_segun_KE(clave);
		break;
	}
	pthread_mutex_unlock(&mutex_instancias_disponibles);
	sem_post(&contador_instancias_disponibles);
	return NULL;
}

/*
 * Obtiene instancia segun el algoritmo Key Explicit
 */
t_instancia* obtener_instancia_segun_KE(char* clave) {
	t_instancia* instancia_elegida;
	string_to_lower(clave);
	int nro_letra = get_nro_letra(clave[0]);

	int cantidad_instancias = list_size(lista_instancias_disponibles);
	int cant_letras_por_instancia = CANT_LETRAS_ALFABETO / cantidad_instancias;
	if (CANT_LETRAS_ALFABETO % cantidad_instancias)
		cant_letras_por_instancia++;
	int nro_instancia_a_elegir = (nro_letra - 1) / cant_letras_por_instancia;
	instancia_elegida = list_get(lista_instancias_disponibles,
			nro_instancia_a_elegir);

	return instancia_elegida;
}

int get_nro_letra(char letra) {
	return letra - 96;
}

t_instancia* instancia_con_menos_espacio_usado(t_list* una_lista_instancias) {
	t_list* lista_temporal = list_duplicate(una_lista_instancias);
	list_sort(lista_temporal, tieneMasEspacioLibre);
	t_instancia* inst_elegida = list_get(lista_temporal, 0);
	list_destroy(lista_temporal);
	return inst_elegida;
}

t_instancia* simular_algoritmo(char* clave) {
	t_instancia* instancia_elegida;

	pthread_mutex_lock(&mutex_instancias_disponibles);

	if (list_is_empty(lista_instancias_disponibles)) {
		pthread_mutex_unlock(&mutex_instancias_disponibles);
		log_warning(logger, "No hay instancias para elegir");
		return NULL;
	}
	switch (configuracion.algoritmo) {
	case EL:
		instancia_elegida = list_get(lista_instancias_disponibles,
				instancia_a_utilizar);
		break;
	case LSU:
		instancia_elegida = instancia_con_menos_espacio_usado(
				lista_instancias_disponibles);
		break;
	case KE:
		instancia_elegida = obtener_instancia_segun_KE(clave);
		break;
	}

	pthread_mutex_unlock(&mutex_instancias_disponibles);
	return instancia_elegida;
}
