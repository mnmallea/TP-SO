/*
 * instancia.c
 *
 *  Created on: 5 jun. 2018
 *      Author: utnso
 */

#include "instancia.h"

#include <commons/string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sincronizacion.h"

/*
 * Aclaracion, esto te copia la clave que le pases
 * Por lo que si no la vas a usar mas despues le tenes que hacer vos el free
 */
void agregar_clave_almacenada(t_instancia* instancia, char* clave) {
	char* copia_clave = string_duplicate(clave);
	list_add(instancia->claves_almacenadas, copia_clave);
}
/*
 * Remueve la clave almacenada de la instancia
 * serviria luego de que haga el store
 */
void remover_clave_almacenada(t_instancia* instancia, char* clave) {
	bool esLaClave(void* unaClave) {
		return !strcmp(unaClave, clave);
	}
	char* clave_buscada = list_remove_by_condition(instancia->claves_almacenadas, esLaClave);
	free(clave_buscada);
}
/*
 * Deberia decirte si la instancia almacena una clave
 */
bool tiene_clave_almacenada(t_instancia* instancia, char* clave) {
	bool esLaClave(void* otraClave) {
		return string_equals_ignore_case(clave, (char*) otraClave);
	}
	return list_any_satisfy(instancia->claves_almacenadas, esLaClave);
}

/*
 * Liberar el nombre luego de usar esto porque se lo copia
 */
t_instancia* crear_instancia(int sockfd, char* nombre, int cant_entradas) {
	t_instancia* new_instancia = malloc(sizeof(*new_instancia));
	new_instancia->claves_almacenadas = list_create();
	new_instancia->nombre = string_duplicate(nombre);
	new_instancia->socket = sockfd;
	new_instancia->cant_entradas_vacias = cant_entradas;
	return new_instancia;
}

/*
 * Libera la memoria ocupada por la estructura de la instancia
 */
void liberar_instancia(t_instancia* instancia) {
	if (instancia != NULL) {
		list_destroy(instancia->claves_almacenadas);
		free(instancia->nombre);
		free(instancia);
	}
}

bool esta_instancia_en_lista(char* nombre, t_list* lista) {
	bool esLaInstanciaQueBusco(void* instancia) {
		return string_equals_ignore_case(((t_instancia*) instancia)->nombre,
				nombre);
	}
	return list_any_satisfy(lista, esLaInstanciaQueBusco);
}

bool esta_activa_instancia(char* nombre) {
	return esta_instancia_en_lista(nombre, lista_instancias_disponibles);
}

bool esta_inactiva_instancia(char* nombre) {
	return esta_instancia_en_lista(nombre, lista_instancias_inactivas);
}

/*
 * Thread safe
 */
void instancia_desactivar(t_instancia* instancia) {
	pthread_mutex_lock(&mutex_instancias_disponibles);
	bool esLaInstanciaQueBusco(void* una_instancia) {
		return instancia == una_instancia;
	}
	list_remove_by_condition(lista_instancias_disponibles,
			esLaInstanciaQueBusco);
	sem_wait(&contador_instancias_disponibles);
	pthread_mutex_unlock(&mutex_instancias_disponibles);

	close(instancia->socket);
	instancia->socket = -1; //porlas
	//todo no se si hay que poner o no mutex aca
	list_add(lista_instancias_inactivas, instancia);
}

/*
 * Thread safe
 */
//head. filter (tieneClave unaClave) $ lista_instancias_disponibles
t_instancia* instancia_con_clave(char* clave) {
	bool instanciaTieneLaClave(void * unaInstancia) {
		return tiene_clave_almacenada(unaInstancia, clave);
	}
	pthread_mutex_lock(&mutex_instancias_disponibles);
	t_list* instancias_con_clave = list_filter(lista_instancias_disponibles,
			instanciaTieneLaClave);
	pthread_mutex_unlock(&mutex_instancias_disponibles);
	t_instancia* instancia = list_get(instancias_con_clave, 0);
	list_destroy(instancias_con_clave);
	return instancia;
}
