#ifndef TABLAENTRADAS_H_
#define TABLAENTRADAS_H_

#include <commons/collections/list.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
	unsigned int	numero;
	unsigned int	tamanio;
	char * clave;
} tablaE;

int entradasLibres;
t_list * tabla;

typedef struct {
	char*	clave;
	char*	valor;
	unsigned int tamanio;
} claveEntrada;

void	crearTablaEntradas();
bool    agregarEnTabla(int nuevaEntrada, claveEntrada* claveE);
bool 	ordenAscendente(void * a, void *b);
int		nombreDeEntradaSiguienteEnTabla(claveEntrada* claveE);
tablaE * adaptoClave(claveEntrada * claveE);
bool quitarDeTabla(claveEntrada * claveE);
void removerDeLista(int unaVariable, tablaE* entrada);
void mostrarTabla();
void liberarEntrada(tablaE* entrada);
void	destruirTabla();

#endif /* TABLAENTRADAS_H_ */
