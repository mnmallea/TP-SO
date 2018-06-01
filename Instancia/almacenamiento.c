#include "almacenamiento.h"

void almacenamiento_logger() {
  logger = log_create("almacenamiento.log", "almacenamiento",true , LOG_LEVEL_INFO);
}

void inicializarAlmacenamiento(unsigned int entradas,unsigned int tamanioEntrada){
log_info(logger,"inicio el almacenamiento");
 ato = malloc(sizeof(almacenamiento));
// estadoAto= bitarray_create_with_mode(char *bitarray,entradas*tamanioEntrada,LSB_FIRST);
ato->cantEntradas=entradas;
ato->tamanioEntrada=tamanioEntrada;
ato->dato=calloc(ato->cantEntradas,ato->tamanioEntrada);
log_info(logger,"inicio el almacenamiento");
}

void set(int proximaEntrada,void* valor,unsigned int tamanio){
if(tamanio <= ato->tamanioEntrada){
	memcpy(ato->dato + (proximaEntrada * ato->tamanioEntrada),valor,tamanio);

	//este paso lo hago para mostrar en pantalla el valor que inserte
	char* valorEnString= convertirString(valor,tamanio);
	free(valorEnString);

}
else{
	unsigned int resto= tamanio - ato->tamanioEntrada;
	//ver que onda, el malloc de parte a insertar lo hacia con ato->tamanioENtrada
	void* parteAinsertar= malloc (ato->tamanioEntrada);
	void* parteRestante= malloc(resto);

	memcpy(parteAinsertar,valor,ato->tamanioEntrada);
	memcpy(parteRestante,valor + ato->tamanioEntrada,resto);

	memcpy(ato->dato + (proximaEntrada*ato->tamanioEntrada),parteAinsertar,ato->tamanioEntrada);
	//este paso lo hago para mostrar en pantalla el valor que inserte
	char* valorEnString= convertirString(parteAinsertar,ato->tamanioEntrada);
	free(valorEnString);

	set(++proximaEntrada,parteRestante,resto);
	free(parteAinsertar);
	free(parteRestante);

}

}
void store(int entrada,unsigned int tamanioEntrada){
}


static char* convertirString(void* valor, size_t tamanio) {
	// calloc en vez de malloc y despues inicializar en 0
	char* valorStr = calloc(tamanio + 1,sizeof(tamanio));
	memcpy(valorStr, valor, tamanio);

	return valorStr;
}
void mostrarAlmacenamiento(){
	int a=0;
	while(a < ato->tamanioEntrada){
		char* valor= convertirString(ato->tamanioEntrada + (a * (ato->tamanioEntrada)),ato->tamanioEntrada);
		log_info(logger,"Entrada %d: %s");
		free(valor);
		a++;
	}
}

void eliminarAlmacenamiento(){
	int a=0;
	while(a < ato->tamanioEntrada){
		free(ato->dato[a]);
	}
	free(ato->dato);
	free(ato);
	log_info(logger, "memoria liberada");
	}

