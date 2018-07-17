#include "almacenamiento.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cfg_almacenamiento.h"

void almacenamiento_logger() {
	logger = log_create("almacenamiento.log", "almacenamiento", true,
			LOG_LEVEL_INFO);
}

void inicializarAlmacenamiento(unsigned int entradas,
		unsigned int tamanioEntrada) {
	log_info(logger, "inicio el almacenamiento");
	ato = malloc(sizeof(almacenamiento));
	ato->cantEntradas = entradas;
	ato->tamanioEntrada = tamanioEntrada;
	ato->dato = calloc(ato->cantEntradas, ato->tamanioEntrada);
	log_info(logger, "inicio el almacenamiento");
	bitarray_almac = crearBitArray(ato->cantEntradas);
	log_debug(logger, "Se ha creado un bitarray de tamaño %d",
			bitarray_get_max_bit(bitarray_almac));
	log_debug(logger, "El espacio restante (total) del almacenamiento es %d",
			almac_entradas_disponibles());
}

/*
 * Copia el valor que le das en el almacenamiento
 * Actualiza el bitarray
 * NO actualiza la tabla de entradas
 */
void setEnAlmacenamiento(int indice, void* valor, unsigned int tamanio) {

	log_trace(logger,
			"ALMACENAMIENTO: se procede a guardar el valor %s en la entrada %d",
			(char*) valor, indice);

	int posicion_en_memoria = indice * obtenerTamanioEntrada();

	memcpy(ato->dato + posicion_en_memoria, valor, tamanio);

	almac_ocupar_entradas(indice, entradas_que_ocupa_por_tamanio(tamanio));

}

/*
 * Devuelve la entrada segun la posicion en el almacenamiento
 * Si no la encuentra devuelve NULL
 */
tablaE* encontrar_entrada_en_posicion(int indice_almacenamiento, t_list* lista) {
	bool estaEnLaPosicion(void* unaEntrada) {
		return ((tablaE*) unaEntrada)->indice == indice_almacenamiento;
	}
	tablaE* entrada = list_find(lista, estaEnLaPosicion);
	return entrada;
}

/*
 char* convertirString(const void *valor, size_t tamanio) {
 char *valorStr = calloc(tamanio + 1, sizeof(tamanio));
 memcpy(valorStr, valor, tamanio);

 return valorStr;
 }
 void mostrarAlmacenamiento(){
 int a=0;
 while(a < ato->cantEntradas){
 void* valor= malloc(ato->tamanioEntrada);
 memcpy(valor,ato->dato + (a *ato->tamanioEntrada),ato->tamanioEntrada);;
 char* valorStr= convertirString(valor,ato->tamanioEntrada);
 free(valor);
 printf("imprimiendo la entrada numero %d : %s",a,valorStr);
 free(valorStr);
 a++;
 }
 }
 */

void eliminarAlmacenamiento() {
	free(ato->dato);
	free(ato);
	log_info(logger, "memoria liberada");
}

void* buscarEnALmacenamiento(int posicion, unsigned int tamanio) {
	void*carga = malloc(tamanio);
	memcpy(carga, ato->dato + (posicion * ato->tamanioEntrada), tamanio);
	return carga;
}

t_bitarray * crearBitArray(int cantBloques) {
	int tamanioBitarray = cantBloques / 8;
	if (cantBloques % 8 != 0) {
		tamanioBitarray++;
	}
	char* bits = malloc(tamanioBitarray);
	t_bitarray * bitarray = bitarray_create_with_mode(bits, tamanioBitarray,
			MSB_FIRST);
	int cont = 0;
	for (; cont < tamanioBitarray * 8; cont++) {
		bitarray_clean_bit(bitarray, cont);
	}
	return bitarray;
}


/*
 * Devuelve el indice de la primera posicion libre del almacenamiento
 * Si no hay posiciones libres devuelve -1
 */
int almac_primera_posicion_libre() {
	return almac_primera_posicion_libre_desde(0);
}

/*
 * Devuelve cual es la primera posicion libre desde el index que le das
 * Si no quedan posiciones libre devuelve -1
 */
int almac_primera_posicion_libre_desde(int index_inicio) {
	int i;
	for (i = index_inicio; i < ato->cantEntradas; i++) {
		if (!bitarray_test_bit(bitarray_almac, i)) {
			return i;
		}
	}
	return -1;
}

/*
 * Devuelve cual es la primera posicion ocupada desde el index que le das
 * Si no quedan posiciones ocupadas devuelve -1
 */
int almac_primera_posicion_ocupada_desde(int index_inicio) {
	int i;
	for (i = index_inicio; i < ato->cantEntradas; i++) {
		if (bitarray_test_bit(bitarray_almac, i)) {
			return i;
		}
	}
	return -1;
}


int almac_cant_entradas_libres_desde(int index) {
	int i;
	int contador = 0;
	for (i = index; i < ato->cantEntradas; i++) {
		if (bitarray_test_bit(bitarray_almac, i)) {
			return contador;
		}
		contador++;
	}
	return contador;
}

int almac_primera_posicion_libre_con_tamanio(int size) {
	for (int i = 0; i < ato->cantEntradas; i++) {
		if (almac_cant_entradas_libres_desde(i) >= size) {
			return i;
		}
	}

	return -1;
}

/*
 * Setea como ocupadas las entradas en el bitarray
 * Devuelve -1 si falla
 */
int almac_ocupar_entradas(int index_inicio, int cantidad_entradas) {
	int index_final = index_inicio + cantidad_entradas - 1;
	if (index_final > ato->cantEntradas) {
		log_warning(logger,
				"Se está intentando ocupar mas entradas que las que tiene el almacenamiento");
		return -1;
	}
	int i;
	for (i = index_inicio; i <= index_final; i++) {
		bitarray_set_bit(bitarray_almac, i);
	}
	return 0;
}

/*
 * Devuelve el espacio restante en el almacenamiento
 */
int almac_entradas_disponibles() {
	int i;
	int contador_espacio = 0;
	for (i = 0; i < ato->cantEntradas; i++) {
		if (!bitarray_test_bit(bitarray_almac, i))
			contador_espacio++;
	}
	return contador_espacio;
}

/*
 * Setea como libres las entradas en el bitarray
 * Devuelve -1 si falla
 */
int almac_liberar_entradas(int index_inicio, int cantidad_entradas) {
	int index_final = index_inicio + cantidad_entradas - 1;
	if (index_final > ato->cantEntradas) {
		log_warning(logger,
				"Se está intentando liberar mas entradas que las que tiene el almacenamiento");
		return -1;
	}
	int i;
	for (i = index_inicio; i <= index_final; i++) {
		bitarray_clean_bit(bitarray_almac, i);
	}
	return 0;
}

char* obtener_valor_de_entrada(tablaE* entrada) {
	char* valor = malloc(entrada->tamanio + 1);
	void* ptr_almac = ato->dato + obtenerTamanioEntrada() * entrada->indice;
	memcpy(valor, ptr_almac, entrada->tamanio);
	valor[entrada->tamanio] = '\0';
	log_debug(logger, "El valor de la clave %s es: %s", entrada->clave, valor);
	return valor;
}

char* obtener_valor_de_clave(char* clave) {
	tablaE* entrada = buscarEntrada(clave);
	if (entrada == NULL) {
		log_warning(logger, "La clave %s no está en el almacentamiento", clave);
		return NULL;
	}
	char* valor = obtener_valor_de_entrada(entrada);
	return valor;
}

void compactar(){
	int primer0 = almac_primera_posicion_libre();
	int primer1 = almac_primera_posicion_ocupada_desde(primer0);
	tablaE* entrada;
	while(primer0 >=0 && primer1 >= 0){
		entrada = obtener_entrada_en_posicion(primer1);
		mover_entrada(entrada, primer0);
		primer0 = almac_primera_posicion_libre();
		primer1 = almac_primera_posicion_ocupada_desde(primer0);
	}
}

