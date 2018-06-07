#ifndef TABLA_ENTRADAS_H_
#define TABLA_ENTRADAS_H_
#include <commons/collections/list.h>
#include <stdlib.h>
#include <string.h>
#include "almacenamiento.h"
#include "cfg_almacenamiento.h"


int entradasLibres;
t_list * tabla;

typedef struct{
	unsigned int	numero;
	unsigned int	tamanio;
	char * clave;
} tablaE;

typedef struct {
	char*	clave;
	char*	valor;
	unsigned int tamanio;
} claveEntrada;

/*  *@NAME: crearTablaEntradas()
	* @DESC: a partir de tabla creo una lista que corresponde
	* a mi tabla de entradas y mediante la conexion del coordinador
	* con la instancia asigno la cantidad de entradas
	* que va a tener
	* @PARAMS:-
	*/
void	crearTablaEntradas();
/*  *@NAME: agregarEnTabla
	* @DESC: Esta funcion booleana sirve para instancia.c,
	* donde se la consultará para insertar el valor o no
	* A partir de una variable puntero tipo claveEntrada, esta funcion
	* la transforma en una estructura de tipo entrada, la cual inserta en la lista
	* y la ordena de manera ascendente de acuerdo al numero de entrada
	* @PARAMS:	nuevaEntrada
	* 			claveE
	*/
bool    agregarEnTabla(int nuevaEntrada, claveEntrada* claveE);
/*  *@NAME: ordenAscendente
	* @DESC: comprueba cual de los dos parametros es mayor
	* @PARAMS: a
	* 		   b
	*/
bool 	ordenAscendente(void * a, void *b);
/*  *@NAME: tablaEstaVacia
	* @DESC: comprueba que la tabla no este vacia,
	* tenga al menos un malloc hecho
	* @PARAMS: tabla
	*/
bool tablaEstaVacia(t_list* tabla);

/*  *@NAME:entradaSiguienteEnTabla
	* @DESC:busca la entrada en la que entre la estructura, mas proxima
	* @PARAMS: claveE
	*/
int	entradaSiguienteEnTabla(claveEntrada* claveE);

/*  *@NAME: adaptoClave
	* @DESC: cambia la estructura de tipo claveEntrada a la
	* que corresponderia a la estructura de la tabla
	* @PARAMS: claveE
	*/
tablaE * adaptoClave(claveEntrada * claveE);

/*  *@NAME: quitarDeTabla
	* @DESC: va de la mano con removerDeLista, eliminan una entrada
	* y agregan el tamaño que ocupaba la misma a entradas restantes
	* @PARAMS: claveE
	*/
bool quitarDeTabla(claveEntrada * claveE);
void removerDeLista(int unaVariable, tablaE* entrada);

/*  *@NAME: mostrarTabla()
	* @DESC: muestra toda la tabla de inicio a fin indicando
	* registro, posicion de almacenamiento, clave y tamanio
	* @PARAMS: -
	*/
void mostrarTabla();
/*  *@NAME: liberarEntrada
	* @DESC: libera una entrada de la tabla
	* @PARAMS: entrada
	*/
void liberarEntrada(tablaE* entrada);
/*  *@NAME: crearClaveEntrade
	* @DESC:a partir de clave valor devuelve una estructura claveEntrada
	* es decir guarda dichos valores en esa estructura
	* @PARAMS: clave
	* 		   valor
	*/
claveEntrada* crearClaveEntrada(char* clave, char* valor);


#endif /* TABLA_ENTRADAS_H_ */
