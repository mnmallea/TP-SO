#include "almacenamiento.h"

void almacenamiento_logger() {
	logger = log_create("almacenamiento.log", "almacenamiento", true,
			LOG_LEVEL_INFO);
}

void inicializarAlmacenamiento(unsigned int entradas,
		unsigned int tamanioEntrada) {
	log_info(logger, "inicio el almacenamiento");
	ato = malloc(sizeof(almacenamiento));
	estadoAto= crearBitArray(entradas);
	ato->cantEntradas = entradas;
	ato->tamanioEntrada = tamanioEntrada;
	ato->dato = calloc(ato->cantEntradas, ato->tamanioEntrada);
	log_info(logger, "inicio el almacenamiento");
}

void setEnAlmacenamiento(int proximaEntrada, void* valor, unsigned int tamanio) {
	if (tamanio <= ato->tamanioEntrada) {
		memcpy(ato->dato + (proximaEntrada * ato->tamanioEntrada), valor,
				tamanio);
		char* valorEnString = convertirString(valor, tamanio);

		free(valorEnString);

	} else {
		unsigned int resto = tamanio - ato->tamanioEntrada;
		void* parteAinsertar = malloc(ato->tamanioEntrada);
		void* parteRestante = malloc(resto);

		memcpy(parteAinsertar, valor, ato->tamanioEntrada);
		memcpy(parteRestante, valor + ato->tamanioEntrada, resto);

		memcpy(ato->dato + (proximaEntrada * ato->tamanioEntrada),
				parteAinsertar, ato->tamanioEntrada);
		char* valorEnString = convertirString(parteAinsertar,
				ato->tamanioEntrada);
		free(valorEnString);

		setEnAlmacenamiento(++proximaEntrada, parteRestante, resto);
		free(parteAinsertar);
		free(parteRestante);

	}

}


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

void eliminarAlmacenamiento() {
	free(ato->dato);
	free(ato);
	log_info(logger, "memoria liberada");
}

void* buscarEnALmacenamiento(int posicion, unsigned int tamanio){
	void*carga =malloc(tamanio);
	memcpy(carga,ato->dato + (posicion * ato->tamanioEntrada),tamanio);
	return carga;
}



t_bitarray * crearBitArray(uint32_t cantBloques){
int tamanioBitarray=cantBloques/8;
if(cantBloques % 8 != 0){
tamanioBitarray++;
}
char* bits=malloc(tamanioBitarray);
t_bitarray * bitarray = bitarray_create_with_mode(bits,tamanioBitarray,MSB_FIRST);
int cont=0;
for(; cont < tamanioBitarray*8; cont++){
bitarray_clean_bit(bitarray, cont);
}
return bitarray;
}
