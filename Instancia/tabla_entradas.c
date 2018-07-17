#include "tabla_entradas.h"

#include <commons/string.h>
#include <stdlib.h>
#include <string.h>

#include "almacenamiento.h"
#include "cfg_almacenamiento.h"

tablaE* remover_de_tabla(int indice_almacenamiento) {
	bool esLaMisma(void* _entrada) {
		return indice_almacenamiento == ((tablaE*) _entrada)->indice;
	}

	return list_remove_by_condition(tabla, esLaMisma);
}

/*
 * Te devuelve la entrada que esta en un indice del almacenamiento
 * Pero no la saca de la tabla
 * Devuelve NULL si no la encuentra
 */
tablaE* obtener_entrada_en_posicion(int indice_almacenamiento) {
	bool esLaMisma(void* _entrada) {
		return indice_almacenamiento == ((tablaE*) _entrada)->indice;
	}

	return list_find(tabla, esLaMisma);
}

void crearTablaEntradas() {
	tabla = list_create();
	entradasLibres = obtenerEntradasTotales();
	log_trace(logger, "se crea la tabla de entrada, con (%d) entradas",
			entradasLibres);

}

void agregarEnTabla(int nuevaEntrada, claveEntrada* claveE) {
	log_trace(logger,
			"se adapta a estructura de tabla  la claveEntrada recibida");
	tablaE * nuevaE = adaptoClave(claveE);
	nuevaE->indice = nuevaEntrada;
	nuevaE->operaciones = nroOperacion;

	list_add(tabla, nuevaE);
	list_sort(tabla, ordenAscendente);
}

int entradaSiguienteEnTabla(claveEntrada* claveE) {
	log_trace(logger,
			"Se procede a buscar la siguiente entrada disponible en tabla");
	if (tablaEstaVacia(tabla)) {
		log_trace(logger,
				"La tabla de entradas esta vacia, se debe guardar en la posicion 0");
		return 0;
	}
	for (int i = 0; i < (cfgAlmacenamiento.totalEntradas); i++) {
		log_trace(logger, "chequeando que la entrada (%d) este vacia", i);
		tablaE* entrada1 = (tablaE*) list_get(tabla, i);
		if (entrada1->clave == NULL || entrada1 == NULL) {
			log_trace(logger, "la entrada (%d) esta vacia", i);
			liberarEntrada(entrada1);
			return i;
		}
		liberarEntrada(entrada1);
	}
	return -1;
}

bool ordenAscendente(void * primero, void * segundo) {
	tablaE* unaEntrada = (tablaE*) primero;
	tablaE* otraEntrada = (tablaE*) segundo;
	if (otraEntrada->indice < unaEntrada->indice) {
		return false;
	}
	return true;
}

bool ordenAscendentePorOperacion(void * primero, void * segundo) {
	tablaE* unaEntrada = (tablaE*) primero;
	tablaE* otraEntrada = (tablaE*) segundo;
	if (otraEntrada->operaciones < unaEntrada->operaciones) {
		return false;
	}
	return true;
}

bool ordenDescendentePorTamanio(void * primero, void * segundo) {
	tablaE* unaEntrada = (tablaE*) primero;
	tablaE* otraEntrada = (tablaE*) segundo;
	if (otraEntrada->tamanio >= unaEntrada->tamanio) {
		return false;
	}
	return true;
}

bool tablaEstaVacia(t_list* tabla) {
	return list_is_empty(tabla);
}

tablaE * adaptoClave(claveEntrada * claveE) {
	tablaE * entrada = malloc(sizeof(tablaE));
//Si strdup no encuentra espacio para el nuevo string, entonces devuelve un puntero null
	entrada->clave = strdup(claveE->clave);
	entrada->tamanio = claveE->tamanio;
	return entrada;
}

bool quitarDeTabla(claveEntrada * claveE) {
	int i = 0;
	while (i < list_size(tabla)) {
		tablaE* entrada = (tablaE*) list_get(tabla, i);
		if (!strcmp(claveE->clave, entrada->clave)) {
			log_trace(logger,
					"se procede a eliminar la entrada (%d) de la tabla claveEntrada",
					i);
			removerDeLista(i, entrada);
			return true;
		}
		i++;
	}
	return false;
}
void removerDeLista(int unaVariable, tablaE* entrada) {
	list_remove(tabla, unaVariable);
	entradasLibres += (entrada->tamanio / obtenerTamanioEntrada()) + 1;
}

void liberarEntrada(tablaE* entrada) {
	if (entrada != NULL)
		free(entrada->clave);
	free(entrada);

}
void liberarCv(claveEntrada* cv) {
	free(cv->clave);
	free(cv->valor);
	free(cv);
}

claveEntrada* crearClaveEntrada(char* clave, char* valor) {
	claveEntrada* claveE = malloc(sizeof(claveEntrada));
	claveE->valor = string_duplicate(valor);
	claveE->clave = string_substring_until(clave, 40);
	claveE->tamanio = strlen(valor);
	return claveE;
}

bool hayEntradasDisponibles(claveEntrada* cv) {
	int entradas_que_ocupa = entradas_que_ocupa_por_tamanio(cv->tamanio);
	return entradas_que_ocupa <= almac_entradas_disponibles();
}

void reemplazarCVEnTabla(claveEntrada* cv) {
	tablaE* entrada = buscarEntrada(cv->clave);
	int entradas_ocupadas = entradas_que_ocupa(entrada);
	int entradas_que_ocuparia = entradas_que_ocupa_por_tamanio(cv->tamanio);
	if (entradas_que_ocuparia > entradas_ocupadas) {
		log_error(logger, "Esto no deberia pasar");
		return;
	}

	almac_liberar_entradas(entrada->indice, entradas_ocupadas);

	entrada->operaciones = nroOperacion;
	entrada->tamanio = cv->tamanio;

	setEnAlmacenamiento(entrada->indice, cv->valor, cv->tamanio);
}

tablaE* buscarEntrada(char* claveAPedir) {
	log_trace(logger,
			"Se procede a recorrer la tabla de entradas para encontrar la clave");
	tablaE* entrada;
	for (int i = 0; i < list_size(tabla); i++) {
		entrada = (tablaE*) list_get(tabla, i);
		if (strcmp(claveAPedir, entrada->clave) == 0) {
			log_trace(logger,
					"Se encontro un una entrada asociada a la clave %s",
					claveAPedir);
			return entrada;
		}
	}
	log_trace(logger, "No se encontro la entrada asociada a la clave %s",
			claveAPedir);
	return NULL;
}

void* obtenerClave(void* cv) {
	tablaE* unaEntrada = cv;
	return (void*) unaEntrada->clave;
}

int entradas_que_ocupa(tablaE* unaEntrada) {
	return entradas_que_ocupa_por_tamanio(unaEntrada->tamanio);
}

int entradas_que_ocupa_por_tamanio(int size) {
	int cant_entradas = size / obtenerTamanioEntrada();
	if (size % obtenerTamanioEntrada()) {
		cant_entradas++;
	}

	return cant_entradas;
}

void mover_entrada(tablaE* entrada, int nueva_posicion){
	int cant_entradas = entradas_que_ocupa(entrada);
	almac_liberar_entradas(entrada->indice, cant_entradas);
	char* valor = obtener_valor_de_entrada(entrada);
	entrada->indice = nueva_posicion;
	setEnAlmacenamiento(nueva_posicion, valor, entrada->tamanio);
	free(valor);
}
