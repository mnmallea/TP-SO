#include "tabla_entradas.h"

void crearTablaEntradas() {
	tabla = list_create();
	entradasLibres = obtenerEntradasTotales();
}


bool agregarEnTabla (int nuevaEntrada, claveEntrada* claveE){
	tablaE * nuevaE = adaptoClave(claveE);
	nuevaE -> numero = nuevaEntrada;
	if(nuevaEntrada >= 0){
		list_add(tabla,(void *)nuevaE);
		list_sort(tabla,ordenAscendente);
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

int entradaSiguienteEnTabla(claveEntrada* claveE){
	tablaE* entrada1;
	tablaE* entrada2;
	// me fijo la cantidad de entradas usadas
	int cantEntradasUsadas = list_size(tabla);
	// me fijo cuantas entradas necesitria la clave que llega por parametro
	int cantEntradasNecesarias = claveE->tamanio/obtenerTamanioEntrada()+1;
	//defino una variable para chequear el espacio entre entradas de la tabla
	int espacioEntreEntradas = 0;
	// si la tabla no esta vacia y las entradas libres me alcanzan para cubrir las que necesito empiezo a ejecutar
	if(entradasLibres >=cantEntradasNecesarias && !tablaEstaVacia(tabla)){
			//if(cantEntradasUsadas == 0){
			//	return 0;
		//	} else{
				entrada1 = (tablaE*) list_get(tabla,0);
				if (entrada1->numero > 0 && cantEntradasNecesarias <= (entrada1->numero)){
						return 0;
					}


			int a = 0;
			//va a ir hasta el anteultimo de la lista
			while (cantEntradasUsadas > a+1){
				//tengo una entrada y su siguiente
				entrada1 = (tablaE*) list_get(tabla,a);
				entrada2 = (tablaE*) list_get(tabla,a+1);
				// chequeo el espacio entre la primera y la segunda entrada
				espacioEntreEntradas = (entrada2->numero) - (entrada1->numero + (entrada1->tamanio/obtenerTamanioEntrada()+1));
				//si la cantidad de entradas necesarias para meter la que llego por parametro es menor al espacio entre entradas
				if(cantEntradasNecesarias <= espacioEntreEntradas){
					//retorno la posicion donde podria insertar esa clave
					return a + (entrada1->tamanio/obtenerTamanioEntrada()+1);
				}
			a++;
			}
			entrada2 = (tablaE*) list_get(tabla,list_size(tabla)-1);
			if(obtenerTamanioEntrada()- entrada2->numero>= cantEntradasNecesarias){
				return (entrada2->tamanio/obtenerTamanioEntrada()+ 1) + entrada2->numero;
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
	free(entrada);
}

claveEntrada* crearClaveEntrada(char* clave, char* valor) {
	claveEntrada* claveE = malloc(sizeof(claveEntrada));
	claveE->valor = valor;
	claveE->clave = string_substring_until(clave, 40);
	claveE->tamanio = (unsigned int) (string_length(valor) + 1);
	return claveE;
}
