#include "tablaEntradas.h"
#include <stdlib.h>
#include "almacenamiento.h"
#include "cfg_almacenamiento.h"

claveEntrada* crearClaveEntrada(char* clave, char* valor) {
	claveEntrada* claveE = malloc(sizeof(claveEntrada));
//Copio Valor
	claveE->valor = valor;
//Separo cantidad de caracteres menor a 40
	claveE->clave = string_substring_until(clave, 40);
//
	claveE->tamanio = (unsigned int) (string_length(valor) + 1);
	return claveE;
}

void crearTablaEntradas() {
//Creo tabla
	tabla = list_create();
//Asigno cuantas entradas libres va a haber para crear la tabla
	entradasLibres = obtenerEntradasTotales();
}

//Esta funcion booleana servira para instancia.c, donde se la consultará para insertar el valor o no
bool agregarEnTabla (int nuevaEntrada, claveEntrada* claveE){
	tablaE * nuevaE = adaptoClave(claveE);
	nuevaE -> numero = nuevaEntrada;
	if(nuevaEntrada >= 0){
		list_add(tabla,(void *)nuevaE);
		list_sort(tabla,ordenAscendente);
//claveE->tamanio/tamanioEntradas()+1 ----> esto devuelve la cantidad de entradas que necesita
		entradasLibres -= claveE->tamanio/obtenerTamanioEntrada()+1;
		return true;
	}
	return false;
}

bool ordenAscendente(void * primero, void * segundo){
	tablaE* unaEntrada = (tablaE*) primero;
	tablaE* otraEntrada = (tablaE*) segundo;
	if (otraEntrada->numero < unaEntrada->numero){
		return false;
	}
	return true;
}

bool tablaEstaVacia(t_list* tabla){
	if (tabla == NULL){
		return true;
	}
	return false;
}

int nombreDeEntradaSiguienteEnTabla(claveEntrada* claveE){
	tablaE* entrada1;
	tablaE* entrada2;
	int cantEntradasUsadas = list_size(tabla);
	int cantEntradasNecesarias = claveE->tamanio/obtenerTamanioEntrada()+1;
	int espacioEntreEntradas = 0;
	if(entradasLibres >=claveE->tamanio/obtenerTamanioEntrada()+1){
		if(tablaEstaVacia(tabla)){
			if(cantEntradasUsadas == 0){
				return 0;
			} else{
				entrada1 = (tablaE*) list_get(tabla,0);
				if (entrada1->numero > 0 && cantEntradasNecesarias <= (entrada1->numero)){
						return 0;
					}
				}
			int a = 0;
			while (cantEntradasUsadas > a+1){
				entrada1 = (tablaE*) list_get(tabla,a);
				entrada2 = (tablaE*) list_get(tabla,a+1);
				espacioEntreEntradas = (entrada2->numero) - (entrada1->numero + (entrada1->tamanio/obtenerTamanioEntrada()+1));
				if(cantEntradasNecesarias <= espacioEntreEntradas){
					return a + (entrada1->tamanio/obtenerTamanioEntrada()+1);
				}
			a++;
			}
			entrada2 = (tablaE*) list_get(tabla,list_size(tabla)-1);
			if(obtenerTamanioEntrada()- entrada2->numero>= cantEntradasNecesarias){
				return (entrada2->tamanio/obtenerTamanioEntrada()+ 1) + entrada2->numero;
				}
			}
		}
//arreglar tiene que decir que hubo error y exit_gracefully o algo asi
return -1;
	}


tablaE * adaptoClave(claveEntrada * claveE){
	tablaE * entrada = malloc(sizeof(tablaE));
//Si strdup no encuentra espacio para el nuevo string, entonces devuelve un puntero null
	entrada->clave = strdup(claveE->clave);
	entrada->tamanio = claveE->tamanio;
	return entrada;
}

bool quitarDeTabla(claveEntrada * claveE){
	int i=0;
	while (i < list_size(tabla)){
		tablaE* entrada = (tablaE*) list_get(tabla,i);
		if (!strcmp(claveE->clave,entrada->clave)){
			removerDeLista(i,entrada);
			return true;
		}
	i++;
	}
	return false;
}

void removerDeLista(int unaVariable, tablaE* entrada ){
	list_remove(tabla,unaVariable);
	entradasLibres += (entrada->tamanio/obtenerTamanioEntrada())+1;
}

void mostrarTabla(){
	int i=0;
	while (list_size(tabla) > i){
		tablaE* entrada = (tablaE*) list_get(tabla,i);
		printf("----Registro n° %d----\n",i);
		printf("----Posicion Almacenamiento: %d", entrada->numero);
		printf("----La Clave es: %s", entrada->clave);
		printf("----El Tamanio es: %d \n", entrada->tamanio);
		i++;
		liberarEntrada(entrada);
	}
}

void liberarEntrada(tablaE* entrada){
	free(entrada->clave);
	free(entrada->numero);
	free(entrada->tamanio);
	free(entrada);
}
