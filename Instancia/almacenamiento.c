#include "almacenamiento.h"

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
			almac_espacio_disponible());
	log_debug(logger, "La primera entrada libre es %d", almac_primera_posicion_libre_con_tamanio(2));
}

void setEnAlmacenamiento(int proximaEntrada, void* valor, unsigned int tamanio) {

	log_trace(logger,
			"ALMACENAMIENTO: se procede a guardar el valor %s en la entrada %d",
			(char*) valor, proximaEntrada);

	if (tamanio <= ato->tamanioEntrada) {
		log_trace(logger, "El valor a guardar es atomico");
		memcpy(ato->dato + (proximaEntrada * ato->tamanioEntrada), valor,
				tamanio);

	} else {
		log_trace(logger,
				"El valor a guardar no es atomico, ocupara varias entradas");
		unsigned int resto = tamanio - ato->tamanioEntrada;
		void* parteAinsertar = malloc(ato->tamanioEntrada);
		void* parteRestante = malloc(resto);

		memcpy(parteAinsertar, valor, ato->tamanioEntrada);
		memcpy(parteRestante, valor + ato->tamanioEntrada, resto);

		memcpy(ato->dato + (proximaEntrada * ato->tamanioEntrada),
				parteAinsertar, ato->tamanioEntrada);

		setEnAlmacenamiento(++proximaEntrada, parteRestante, resto);
		free(parteAinsertar);
		free(parteRestante);

	}

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

int almac_cant_entradas_libres_desde(int index) {
	int i;
	int contador = 0;
	for (i = index; i < ato->cantEntradas; i++) {
		if (bitarray_test_bit(bitarray_almac, i)) {
			return contador;
		}
		contador ++;
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
	for (i = 0; i <= index_final; i++) {
		bitarray_set_bit(bitarray_almac, i);
	}
	return 0;
}

/*
 * Devuelve el espacio restante en el almacenamiento
 */
int almac_espacio_disponible() {
	int i;
	int contador_espacio = 0;
	for (i = 0; i < ato->cantEntradas; i++) {
		if (!bitarray_test_bit(bitarray_almac, i))
			contador_espacio++;
	}
	return contador_espacio;
}
