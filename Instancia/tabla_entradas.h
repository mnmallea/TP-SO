#ifndef TABLA_ENTRADAS_H_
#define TABLA_ENTRADAS_H_
#include <commons/collections/list.h>
#include <stdlib.h>
#include <string.h>
#include "almacenamiento.h"
#include "cfg_almacenamiento.h"


int entradasLibres;
t_list * tabla;
extern t_log* logger;

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

void 			mostrarTabla();
void			crearTablaEntradas();
void  			liberarCv(claveEntrada* cv);
bool    		tablaEstaVacia(t_list* tabla);
void 			liberarEntrada(tablaE* entrada);
tablaE* 		buscarEntrada(char* claveAPedir);
bool 			ordenAscendente(void * a, void *b);
tablaE* 		adaptoClave(claveEntrada * claveE);
bool    		quitarDeTabla(claveEntrada * claveE);
bool 			hayEntradasDisponibles(claveEntrada* cv);
claveEntrada* 	crearClaveEntrada(char* clave, char* valor);
int				entradaSiguienteEnTabla(claveEntrada* claveE);
void    		removerDeLista(int unaVariable, tablaE* entrada);
bool    		agregarEnTabla(int nuevaEntrada, claveEntrada* claveE);




/*  *@NAME: crearTablaEntradas()
	* @DESC: a partir de tabla creo una lista que corresponde
	* a mi tabla de entradas y mediante la conexion del coordinador
	* con la instancia asigno la cantidad de entradas
	* que va a tener
	* @PARAMS:-
	*/

/*  *@NAME: agregarEnTabla
	* @DESC: Esta funcion booleana sirve para instancia.c,
	* donde se la consultará para insertar el valor o no
	* A partir de una variable puntero tipo claveEntrada, esta funcion
	* la transforma en una estructura de tipo entrada, la cual inserta en la lista
	* y la ordena de manera ascendente de acuerdo al numero de entrada
	* @PARAMS:	nuevaEntrada
	* 			claveE
	*/

/*  *@NAME: ordenAscendente
	* @DESC: comprueba cual de los dos parametros es mayor
	* @PARAMS: a
	* 		   b
	*/

/*  *@NAME: tablaEstaVacia
	* @DESC: comprueba que la tabla no este vacia,
	* tenga al menos un malloc hecho
	* @PARAMS: tabla
	*/


/*  *@NAME:entradaSiguienteEnTabla
	* @DESC:busca la entrada en la que entre la estructura, mas proxima
	* @PARAMS: claveE
	*/


/*  *@NAME: adaptoClave
	* @DESC: cambia la estructura de tipo claveEntrada a la
	* que corresponderia a la estructura de la tabla
	* @PARAMS: claveE
	*/

/*  *@NAME: quitarDeTabla
	* @DESC: va de la mano con removerDeLista, eliminan una entrada
	* y agregan el tamaño que ocupaba la misma a entradas restantes
	* @PARAMS: claveE
	*/


/*  *@NAME: mostrarTabla()
	* @DESC: muestra toda la tabla de inicio a fin indicando
	* registro, posicion de almacenamiento, clave y tamanio
	* @PARAMS: -
	*/

/*  *@NAME: crearClaveEntrada
	* @DESC: a partir de una clave y valor crea una clavEntrada
	*
	* @PARAMS: clave
	* 		   valor
	*/



/*  *@NAME: crearClaveEntrade
	* @DESC:a partir de clave valor devuelve una estructura claveEntrada
	* es decir guarda dichos valores en esa estructura
	* @PARAMS: clave
	* 		   valor
	*/



#endif /* TABLA_ENTRADAS_H_ */
