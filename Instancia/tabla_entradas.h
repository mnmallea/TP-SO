#ifndef TABLA_ENTRADAS_H_
#define TABLA_ENTRADAS_H_

#include <commons/collections/list.h>
#include <commons/log.h>
#include <stdbool.h>



int entradasLibres;
t_list * tabla;
extern t_log* logger;
unsigned int nroOperacion;

typedef struct{
	unsigned int	indice;
	unsigned int	tamanio;
	char * clave;
	unsigned int operaciones;
} tablaE;



typedef struct {
	char*	clave;
	char*	valor;
	unsigned int tamanio;
} claveEntrada;

void* 			obtenerClave(void*);
void			crearTablaEntradas();
void  			liberarCv(claveEntrada* cv);
bool    		tablaEstaVacia(t_list* tabla);
void 			liberarEntrada(tablaE* entrada);
tablaE* 		buscarEntrada(char* claveAPedir);
bool 			ordenAscendente(void * a, void *b);
tablaE* 		adaptoClave(claveEntrada * claveE);
bool    		quitarDeTabla(claveEntrada * claveE);
void 			reemplazarCVEnTabla(claveEntrada* cv);
int				entradas_que_ocupa(tablaE* unaEntrada);
bool 			hayEntradasDisponibles(claveEntrada* cv);
claveEntrada* 	crearClaveEntrada(char* clave, char* valor);
int				entradaSiguienteEnTabla(claveEntrada* claveE);
void    		removerDeLista(int unaVariable, tablaE* entrada);
void    		agregarEnTabla(int nuevaEntrada, claveEntrada* claveE);
bool 			ordenAscendentePorOperacion(void * primero, void * segundo);
bool 			ordenDescendentePorTamanio(void * primero, void * segundo);
int entradas_que_ocupa_por_tamanio(int size);
tablaE* remover_de_tabla(int indice_almacenamiento) ;





#endif /* TABLA_ENTRADAS_H_ */
