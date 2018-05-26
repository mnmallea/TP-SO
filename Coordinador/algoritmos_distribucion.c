/*
 * algoritmos_distribucion.c
 *
 *  Created on: 3 may. 2018
 *      Author: utnso
 */
#include "algoritmos_distribucion.h"

int instancia_a_utilizar = 0;
int largo_lista;

/*
 * Obtiene instancia segun el algoritmo Equitative Load
 * Thread safe (mutex implementado sobre la lista de instancias disponibles)
 */
t_instancia* obtener_instancia_segun_EL(char* clave){
	pthread_mutex_lock(&mutex_instancias_disponibles);
	t_instancia* inst_elegida = list_get(lista_instancias_disponibles, instancia_a_utilizar);
	largo_lista = list_size(lista_instancias_disponibles);
	pthread_mutex_unlock(&mutex_instancias_disponibles);
	instancia_a_utilizar++;

	if(instancia_a_utilizar == largo_lista) //recien utilice la ultima posicion
		instancia_a_utilizar = 0;

	return inst_elegida;
}

/*
 * Obtiene instancia segun el algoritmo Least Space Used
 * Thread safe (mutex implementado sobre la lista de instancias disponibles)
 */
t_instancia* obtener_instancia_segun_LSU(char* clave){
	t_instancia* instancia_elegida;
	pthread_mutex_lock(&mutex_instancias_disponibles);
	list_sort(lista_instancias_disponibles,tieneMasEspacioLibre);
	instancia_elegida = list_get(lista_instancias_disponibles, 0);
	pthread_mutex_unlock(&mutex_instancias_disponibles);
	return instancia_elegida;
}

bool tieneMasEspacioLibre(void* una_instancia, void* otra_instancia){
	return ((t_instancia*)una_instancia)->cant_entradas_vacias > ((t_instancia*)otra_instancia)->cant_entradas_vacias;
}

/*
 * Obtiene instancia segun el algoritmo indicado en la configuracion
 * Thread safe (mutex implementado sobre la lista de instancias disponibles)
 */
t_instancia* obtener_instancia_siguiente(char* clave){
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
	return NULL;
}

/*
 * Obtiene instancia segun el algoritmo Key Explicit
 * Thread safe (mutex implementado sobre la lista de instancias disponibles)
 */
t_instancia* obtener_instancia_segun_KE(char* clave){
	t_instancia* instancia_elegida;
	int nro_letra = get_nro_letra(clave[0]);

	pthread_mutex_lock(&mutex_instancias_disponibles);
	int cantidad_instancias = list_size(lista_instancias_disponibles);
	int cant_letras_por_instancia = CANT_LETRAS_ALFABETO / cantidad_instancias;
	if(CANT_LETRAS_ALFABETO % cantidad_instancias)
		cant_letras_por_instancia++;
	int nro_instancia_a_elegir = (nro_letra - 1)/ cant_letras_por_instancia;
	instancia_elegida = list_get(lista_instancias_disponibles, nro_instancia_a_elegir);
	pthread_mutex_unlock(&mutex_instancias_disponibles);

	return instancia_elegida;
}

int get_nro_letra(char letra){
	return letra - 96;
}

