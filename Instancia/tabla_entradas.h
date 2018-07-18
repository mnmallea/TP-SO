#ifndef TABLA_ENTRADAS_H_
#define TABLA_ENTRADAS_H_

#include <commons/collections/list.h>
#include <commons/log.h>
#include <stdbool.h>



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

void entrada_destroyer(void* entrada);

void* 			obtenerClave(void*);
void			crearTablaEntradas();
void  			liberarCv(claveEntrada* cv);
bool    		tablaEstaVacia(t_list* tabla);
void 			liberarEntrada(tablaE* entrada);
tablaE* 		buscarEntrada(char* claveAPedir);
bool 			ordenAscendente(void * a, void *b);
tablaE* 		adaptoClave(claveEntrada * claveE);
bool    		quitarDeTabla(claveEntrada * claveE);
int 			reemplazarCVEnTabla(claveEntrada* cv);
int				entradas_que_ocupa(tablaE* unaEntrada);
bool 			hayEntradasDisponibles(claveEntrada* cv);
claveEntrada* 	crearClaveEntrada(char* clave, char* valor);
int				entradaSiguienteEnTabla(claveEntrada* claveE);

void    		agregarEnTabla(int nuevaEntrada, claveEntrada* claveE);
bool 			ordenAscendentePorOperacion(void * primero, void * segundo);
bool 			ordenDescendentePorTamanio(void * primero, void * segundo);
int entradas_que_ocupa_por_tamanio(int size);
tablaE* remover_de_tabla(int indice_almacenamiento) ;
void mover_entrada(tablaE* entrada, int nueva_posicion);
tablaE* obtener_entrada_en_posicion(int indice_almacenamiento);





#endif /* TABLA_ENTRADAS_H_ */
