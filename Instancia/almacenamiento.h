#ifndef ALMACENAMIENTO_H_
#define ALMACENAMIENTO_H_

#include <commons/string.h>
#include <stdlib.h>
#include <commons/bitarray.h>
#include <commons/log.h>
#include <stdio.h>


t_log * logger;

typedef struct{
	unsigned int tamanioEntrada;
	unsigned int cantEntradas;
	void** dato;
}almacenamiento;

almacenamiento* ato;

t_bitarray estadoAto;

void almacenamiento_logger();
/*  *@NAME: mostrarALmacenamiento
	* @DESC: creo el almacenamiento inicializando su contenido data
	* 		 en 0
	* @PARAMS:
	* 			entradas
	* 			tamanioEntradaa
	*/
void inicializarAlmacenamiento(unsigned int entradas,unsigned int tamanioEntrada);
/*  *@NAME: set
	* @DESC: ingresa el valor en el almacenamiento, chequeando
	* si entra en una entrada o necesita mas de manera recursiva
	* @PARAMS:
	* 		proximaEntrada
	* 		valor
	* 		tamanio
	*/
void set(int proximaEntrada,void* valor,unsigned int tamanio);

//void store(int entrada,unsigned int tamanioEntrada);//falta hacer

/*  *@NAME: convertirString
	* @DESC: convierte un valor en string para ser mostrado por
	* 		 pantalla
	* @PARAMS:
	* 		valor
	* 		tamanio
	*/
static char* convertirString(void* valor, size_t tamanio);
/*  *@NAME: mostrarALmacenamiento
	* @DESC: muestra todo el almacenamiento
	* @PARAMS:-
	*/
void mostrarAlmacenamiento();
/*  *@NAME: eliminarALmacenamiento
	* @DESC: libera todo el almacenamiento
	* @PARAMS:-
	*/
void eliminarAlmacenamiento();

#endif /* ALMACENAMIENTO_H_ */
