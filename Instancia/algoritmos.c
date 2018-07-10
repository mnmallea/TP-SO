/*
 * algoritmos.c
 *
 *  Created on: 4 jul. 2018
 *      Author: utnso
 */

//todo no sabemos si el algoritmo lru y bsu deberian devolver una sola clave o las suficientes como para que ingrese la clave nueva

#include "algoritmos.h"

t_list* algoritmoCircular(claveEntrada* cv,t_list* posiblesAReemplazar){
	if(!(list_is_empty(posiblesAReemplazar))){
		//filtro tabla por claves atomicas
		posiblesAReemplazar=realloc(posiblesAReemplazar,list_size(list_filter(tabla,esAtomica)));
		posiblesAReemplazar = list_filter(tabla,esAtomica);
	}
	//chequo que la cantidad de entradas a replazar alcance para el tamaño de mi cv
	if(list_size(posiblesAReemplazar)>= cv->tamanio){
		//ordeno ascendente
		list_sort(posiblesAReemplazar,ordenAscendente);
		//acorto la lista de posiblesARemplazar para que solo tenga las necesarias para mi nueva cv
		t_list* vanASerReemplazadas=list_take_and_remove(posiblesAReemplazar,cv->tamanio/obtenerTamanioEntrada());
		//mapeo para retornar solo las claves no la entrada que despues se encarge la entrada de eliminar dicha clave
		vanASerReemplazadas= list_map(vanASerReemplazadas,(void*)obtenerClave);
		return vanASerReemplazadas;
	}
	// que pasa si el tamaño de la lista de posibles a remplazar no alcanza, pero si busco en la tabla cuales son atomicas
	//y lo concateno con la otra lista de posibles a remplazar me alzanza el tamañoi
	return NULL;
}


t_list* algoritmoLRU(claveEntrada* cv){
	t_list* posiblesAReemplazar= malloc(sizeof(tabla));
	list_sort(posiblesAReemplazar,ordenAscendentePorOperacion);
	if(list_size(posiblesAReemplazar)>= cv->tamanio){
		t_list* vanASerReemplazadas=list_take_and_remove(posiblesAReemplazar,cv->tamanio/obtenerTamanioEntrada());
		vanASerReemplazadas= list_map(vanASerReemplazadas,(void*)obtenerClave);
		free(posiblesAReemplazar);
		return vanASerReemplazadas;
	}
	free(posiblesAReemplazar);
	return NULL;
}

t_list* algoritmoBSU(claveEntrada* cv){
	t_list* posiblesAReemplazar= malloc(sizeof(tabla));
	posiblesAReemplazar = list_filter(tabla,esAtomica);
	list_sort(posiblesAReemplazar,ordenDescendentePorTamanio);
	if(list_size(posiblesAReemplazar)>= cv->tamanio){
			t_list* vanASerReemplazadas=list_take_and_remove(posiblesAReemplazar,cv->tamanio/obtenerTamanioEntrada());
			vanASerReemplazadas= list_map(vanASerReemplazadas,(void*)obtenerClave);
			free(posiblesAReemplazar);
			return vanASerReemplazadas;
	}
	free(posiblesAReemplazar);
	return NULL;


}




