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
unsigned int nroOperacion;

typedef struct{
	unsigned int	numero;
	unsigned int	tamanio;
	char * clave;
	unsigned int operaciones;
} tablaE;



typedef struct {
	char*	clave;
	char*	valor;
	unsigned int tamanio;
} claveEntrada;

void 			mostrarTabla();
void			crearTablaEntradas();
void  			liberarCv(claveEntrada* cv);
bool 			esAtomica(void* unaEntrada);
bool    		tablaEstaVacia(t_list* tabla);
char* 			obtenerClave(claveEntrada* cv);
void 			liberarEntrada(tablaE* entrada);
tablaE* 		buscarEntrada(char* claveAPedir);
bool 			ordenAscendente(void * a, void *b);
tablaE* 		adaptoClave(claveEntrada * claveE);
bool    		quitarDeTabla(claveEntrada * claveE);
void 			reemplazarCVEnTabla(claveEntrada* cv);
bool 			hayEntradasDisponibles(claveEntrada* cv);
claveEntrada* 	crearClaveEntrada(char* clave, char* valor);
int				entradaSiguienteEnTabla(claveEntrada* claveE);
void    		removerDeLista(int unaVariable, tablaE* entrada);
bool    		agregarEnTabla(int nuevaEntrada, claveEntrada* claveE);
bool 			ordenAscendentePorOperacion(void * primero, void * segundo);
bool 			ordenDescendentePorTamanio(void * primero, void * segundo);




#endif /* TABLA_ENTRADAS_H_ */
