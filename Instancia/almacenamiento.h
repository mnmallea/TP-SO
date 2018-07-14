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

t_bitarray* estadoAto;

void almacenamiento_logger();
void mostrarAlmacenamiento();
void eliminarAlmacenamiento();
t_bitarray * crearBitArray(uint32_t cantBloques);
char* convertirString(const void *valor, size_t tamanio);
void* buscarEnALmacenamiento(int posicion, unsigned int tamanio);
void inicializarAlmacenamiento(unsigned int entradas,unsigned int tamanioEntrada);
void setEnAlmacenamiento(int proximaEntrada, void* valor, unsigned int tamanio);


#endif /* ALMACENAMIENTO_H_ */
