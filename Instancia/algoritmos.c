/*
 * algoritmos.c
 *
 *  Created on: 14 jul. 2018
 *      Author: utnso
 */

#include "algoritmos.h"

#include <stdlib.h>

#include "almacenamiento.h"
#include "cfg_almacenamiento.h"

/*
 * Saca la entrada de la tabla de entradas
 * Y te limpia el bitarray
 */

t_resultado_set ReemplazarSegunAlgoritmo(claveEntrada* cv) {
	int entradas_necesarias = entradas_que_ocupa_por_tamanio(cv->tamanio);
	int reemplazos_necesarios = entradas_necesarias - almac_entradas_disponibles();
	log_debug(logger,
			"Se necesitan %d entradas para reemplazar, hay %d entradas atómicas",
			reemplazos_necesarios, cantidad_entradas_atomicas());
	if (reemplazos_necesarios > cantidad_entradas_atomicas()) {
		log_error(logger,
				"No hay suficientes entradas atomicas para reemplazar");
		//aca hay que informar error al coordinador
		return SET_ERROR;
	}

	int i;
	for (i = 0; i < reemplazos_necesarios; i++) {
		tablaE* entrada = obtener_siguiente_entrada_segun_algoritmo();
		if (entrada == NULL) {
			log_error(logger, "Esto no deberia pasar");
			return SET_ERROR;
		}
		log_debug(logger,
				"Se reemplazara la entrada Nro: %d, que contenia la clave %s",
				entrada->indice, entrada->clave);
		liberar_entrada(entrada);
	}

	int posicion_a_insertar = almac_primera_posicion_libre_con_tamanio(
			entradas_necesarias);

	if (posicion_a_insertar < 0) {
		log_warning(logger, "Hay fragmentación externa");
		//Aca hay que solicitar compactacion al coordinador
		return REQUIERE_COMPACTACION;
	}
	agregarEnTabla(posicion_a_insertar, cv);
	setEnAlmacenamiento(posicion_a_insertar, cv->valor, cv->tamanio);
	log_info(logger, "Reemplazo ejecutado exitosamente!!!!");
	return SET_EXITOSO;
}

tablaE* obtener_siguiente_entrada_segun_algoritmo() {
	if (configuracion.algoritmo == CIRC) {
		return obtener_siguiente_entrada_circular();
	} else if (configuracion.algoritmo == LRU) {
		return obtener_siguiente_entrada_lru();
	} else if (configuracion.algoritmo == BSU) {
		return obtener_siguiente_entrada_bsu();
	} else {
		log_error(logger, "No se encontro algoritmo de reemplazo");
		return NULL;
	}
}

void liberar_entrada(tablaE* entrada) {
	remover_de_tabla(entrada->indice);
	log_debug(logger,
			"Se procede a liberar la entrada Nro: %d, que contiene la clave %s",
			entrada->indice, entrada->clave);
	int cant_entradas = entradas_que_ocupa(entrada);
	almac_liberar_entradas(entrada->indice, cant_entradas);
	liberarEntrada(entrada);
}

int cantidad_entradas_atomicas() {
	return list_count_satisfying(tabla, esAtomica);
}

int obtener_indice_de_entrada(t_list* lista_de_tablaE, int posicion) {
	int i;
	tablaE* elemento;
	for (i = 0; i < list_size(lista_de_tablaE); i++) {
		elemento = list_get(lista_de_tablaE, i);
		if (elemento->indice == posicion) {
			return i;
		}
	}
	log_error(logger, "No se pudo encontrar el elemento en la lista");
	return -1;
}

tablaE* primera_entrada_atomica() {
	return list_find(tabla, esAtomica);
}

/*
 * Devuelve la primera entrada atomica desde cierta posicion
 * Hace el "circulo"
 * Validar antes que haya entradas atomicas, porque sino devuelve null
 */
tablaE* primera_entrada_atomica_desde(int posicion_desde) {
	list_sort(tabla, ordenAscendente);
	tablaE* entrada;
	int i;

	for (i = 0; i < list_size(tabla); i++) {
		entrada = list_get(tabla, i);
		if (esAtomica(entrada) && entrada->indice >= posicion_desde) {
			return entrada;
		}
	}

	return primera_entrada_atomica();
}

unsigned int tamanio_entrada_mas_grande() {
	t_list* atomicas = list_filter(tabla, esAtomica);
	list_sort(atomicas, ordenDescendentePorTamanio);
	tablaE* primeraEntrada = list_get(atomicas, 0);
	list_destroy(atomicas);
	return primeraEntrada->tamanio;
}

/*
 * Devuelve la siguiente entrada que seria reemplazada por el algoritmo circular
 * Avanza el puntero a la ultima entrada que devolvio
 * IMPORTANTE: validar antes que haya entradas atomicas disponibles
 */
tablaE* obtener_siguiente_entrada_circular() {
	tablaE* entrada = primera_entrada_atomica_desde(posicion);
	posicion = entrada->indice + 1;
	if (posicion >= obtenerEntradasTotales())
		posicion = 0;
	return entrada;
}

tablaE* obtener_siguiente_entrada_bsu() {
	tablaE* entrada = primera_entrada_masGrande_desde(posicion);
	posicion = entrada->indice + 1;
	if (posicion >= obtenerEntradasTotales())
		posicion = 0;
	return entrada;
}

tablaE* primera_entrada_masGrande_desde(int posicion_desde) {
	int tamanio_mas_grande = tamanio_entrada_mas_grande();
	list_sort(tabla, ordenAscendente);
	tablaE* entrada;
	int i;

	for (i = 0; i < list_size(tabla); i++) {
		entrada = list_get(tabla, i);
		if (esAtomica(entrada) && entrada->indice >= posicion_desde
				&& entrada->tamanio == tamanio_mas_grande) {
			return entrada;
		}
	}

	return primera_entrada_mas_grande();
}

tablaE* primera_entrada_mas_grande() {
	return list_find(tabla, atomica_masGrande);
}

bool atomica_masGrande(void* unaEntrada) {
	tablaE* entrada = unaEntrada;
	int tamanio_mas_grande = tamanio_entrada_mas_grande();
	return entrada->tamanio == tamanio_mas_grande;

}

bool esAtomica(void* unaEntrada) {
	tablaE* entrada = unaEntrada;
	return entrada->tamanio <= obtenerTamanioEntrada();
}

tablaE* obtener_siguiente_entrada_lru() {
	tablaE* entrada = primera_entrada_mas_vieja_desde(posicion);
	posicion = entrada->indice + 1;
	if (posicion >= obtenerEntradasTotales())
		posicion = 0;
	return entrada;
}

tablaE* primera_entrada_mas_vieja_desde(int posicion_desde) {
	unsigned int nro_operacion = operacion_mas_vieja();
	list_sort(tabla, ordenAscendente);
	tablaE* entrada;
	int i;
	for (i = 0; i < list_size(tabla); i++) {
		entrada = list_get(tabla, i);
		if (esAtomica(entrada) && entrada->indice >= posicion_desde
				&& entrada->operaciones == nro_operacion) {
			return entrada;
		}
	}
	return primera_entrada_mas_vieja();

}

tablaE* primera_entrada_mas_vieja() {
	return list_find(tabla, mas_vieja);
}

bool mas_vieja(void* una_entrada) {
	tablaE* entrada = una_entrada;
	unsigned int nro_operacion = operacion_mas_vieja();
	return nro_operacion == entrada->operaciones;
}

unsigned int operacion_mas_vieja() {
	t_list* atomicas = list_filter(tabla, esAtomica);
	list_sort(atomicas, ordenAscendentePorOperacion);
	tablaE* entrada = list_get(atomicas, 0);
	list_destroy(atomicas);
	return entrada->operaciones;

}

