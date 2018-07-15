#ifndef ALMACENAMIENTO_H_
#define ALMACENAMIENTO_H_

#include <commons/string.h>
#include <stdlib.h>
#include <commons/bitarray.h>
#include <commons/log.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

t_log * logger;

typedef struct {
	unsigned int tamanioEntrada;
	unsigned int cantEntradas;
	void* dato;
} almacenamiento;

almacenamiento* ato;

t_bitarray* bitarray_almac;

void almacenamiento_logger();
void mostrarAlmacenamiento();
void eliminarAlmacenamiento();
t_bitarray * crearBitArray(int cantBloques);
char* convertirString(const void *valor, size_t tamanio);
void* buscarEnALmacenamiento(int posicion, unsigned int tamanio);
void inicializarAlmacenamiento(unsigned int entradas,unsigned int tamanioEntrada);
void setEnAlmacenamiento(int proximaEntrada, void* valor, unsigned int tamanio);

int almac_primera_posicion_libre_desde(int index_inicio);
int almac_entradas_disponibles();
int almac_cant_entradas_libres_desde(int index);
int almac_primera_posicion_libre_con_tamanio(int size);
int almac_ocupar_entradas(int index_inicio, int cantidad_entradas);
int almac_liberar_entradas(int index_inicio, int cantidad_entradas);


#endif /* ALMACENAMIENTO_H_ */
