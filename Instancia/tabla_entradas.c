#include "tabla_entradas.h"

void crearTablaEntradas() {
	tabla = list_create();
	entradasLibres = obtenerEntradasTotales();
	log_trace(logger,"se crea la tabla de entrada, con (%d) entradas",entradasLibres);

}

bool agregarEnTabla(int nuevaEntrada, claveEntrada* claveE) {
	log_trace(logger,"se adapta a estructura de tabla  la claveEntrada recibida");
	tablaE * nuevaE = adaptoClave(claveE);
	nuevaE->numero = nuevaEntrada;
	nuevaE->operaciones= nroOperacion;
	if (nuevaEntrada >= 0) {
		list_add(tabla, (void *) nuevaE);
		list_sort(tabla, ordenAscendente);
		log_trace(logger,"entradas libres antes de agregar en tabla (%d)",entradasLibres);
		entradasLibres -= claveE->tamanio / obtenerTamanioEntrada() + 1;
		log_trace(logger,"entradas libres luego de agregar en tabla (%d)",entradasLibres);
		return true;
	}
	return false;
}

int entradaSiguienteEnTabla(claveEntrada* claveE) {
	log_trace(logger, "Se procede a buscar la siguiente entrada disponible en tabla");
	if(tablaEstaVacia(tabla)){
			log_trace(logger, "La tabla de entradas esta vacia, se debe guardar en la posicion 0");
			return 0;
	}
	for(int i=0; i< (cfgAlmacenamiento.totalEntradas); i++){
		log_trace(logger, "chequeando que la entrada (%d) este vacia",i);
		tablaE* entrada1 =(tablaE*) list_get(tabla,i);
		if(entrada1->clave == NULL || entrada1 == NULL){
			log_trace(logger, "la entrada (%d) esta vacia",i);
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
	if (otraEntrada->numero < unaEntrada->numero) {
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

bool ordenDescendentePorTamanio(void * primero, void * segundo){
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
			log_trace(logger, "se procede a eliminar la entrada (%d) de la tabla claveEntrada", i);
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
	claveE->valor =  string_duplicate(valor);
	claveE->clave = string_substring_until(clave, 40);
	claveE->tamanio = (unsigned int) (string_length(valor) + 1);
	return claveE;
}

bool hayEntradasDisponibles(claveEntrada* cv) {
	return (cv->tamanio / obtenerTamanioEntrada() + 1) <= entradasLibres;
}

void reemplazarCVEnTabla(claveEntrada* cv){
	tablaE* entrada=buscarEntrada(cv->clave);
	entrada->operaciones=nroOperacion;
	entrada->tamanio=cv->tamanio;
	setEnAlmacenamiento(entrada->numero,cv->valor,cv->tamanio);
	liberarEntrada(entrada);
}

tablaE* buscarEntrada(char* claveAPedir) {
	log_trace(logger, "Se procede a recorrer la tabla de entradas para encontrar la clave");
	tablaE* entrada;
	for (int i = 0; i < list_size(tabla); i++) {
		entrada = (tablaE*) list_get(tabla, i);
		if (strcmp(claveAPedir, entrada->clave)==0) {
			log_trace(logger, "Se encontro un una entrada asociada a la clave %s", claveAPedir);
			return entrada;
		}
	}
	log_trace(logger, "No se encontro la entrada asociada a la clave %s", claveAPedir);
	return NULL;
}

void* obtenerClave(void* cv){
	tablaE* unaEntrada= cv;
	return (void*) unaEntrada->clave;
}

int entradas_que_ocupa(tablaE* unaEntrada){
  int div = unaEntrada->tamanio / obtenerTamanioEntrada();
if(unaEntrada->tamanio % obtenerTamanioEntrada()){
	div+=1;
	}

return div;
}

