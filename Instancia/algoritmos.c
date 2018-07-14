/*
 * algoritmos.c
 *
 *  Created on: 4 jul. 2018
 *      Author: utnso
 */

//todo no sabemos si el algoritmo lru y bsu deberian devolver una sola clave o las suficientes como para que ingrese la clave nueva
#include "algoritmos.h"

t_list* algoritmoCircular(claveEntrada* cv, t_list* posiblesAReemplazar) {
	log_trace(logger, "Se procede a reemplazar mediante algoritmo circular");
	log_trace(logger,"Se procede a validar si la lista de posiblesAReemplazar no es vacia");
	if (!(list_is_empty(posiblesAReemplazar))) {

		log_trace(logger,"La lista de posiblesAReemplazar no estaba vacia, se filtran las claves atomicas");
		//ojo aca se van a repetir
		list_add_all(posiblesAReemplazar,list_filter(tabla, esAtomica));
	}
	log_trace(logger,"Se procede a chequear que la cantidad de entradas a reemplazar alcance para el tamaño del cv");
	//chequo que la cantidad de entradas a replazar alcance para el tamaño de mi cv
	if (list_size(posiblesAReemplazar) >= cv->tamanio) {
		//ordeno ascendente
		log_trace(logger,"Alcanzaban: La cantidad de posiblesAReemplazar es %d, el tamaño del cv es: %d",list_size(posiblesAReemplazar), cv->tamanio);
		list_sort(posiblesAReemplazar, ordenAscendente);
		//acorto la lista de posiblesARemplazar para que solo tenga las necesarias para mi nueva cv
		t_list* vanASerReemplazadas = list_take_and_remove(posiblesAReemplazar,cv->tamanio / obtenerTamanioEntrada());
		log_trace(logger,"Se ṕrocede a obtener el listado final de claves a ser reemplazadas");
		//mapeo para retornar solo las claves no la entrada que despues se encarge la entrada de eliminar dicha clave
		vanASerReemplazadas = list_map(vanASerReemplazadas, obtenerClave);
		return vanASerReemplazadas;
	}
	// que pasa si el tamaño de la lista de posibles a remplazar no alcanza, pero si busco en la tabla cuales son atomicas
	//y lo concateno con la otra lista de posibles a remplazar me alzanza el tamañoi
	log_error(logger, "No se pudo encontrar clave a reemplazar");
	return NULL;
}



/* ALG CIRCULAR V2.0

t_list* algoritmoCircular(claveEntrada* cv, t_list* posiblesAReemplazar) {
	//lista posiblesAReemplazar vacia
	if(list_is_empty(posiblesAReemplazar)){
		posiblesAReemplazar= list_filter(tabla,esAtomica);
		 if(list_size(posiblesAReemplazar)> sizeof(cv)){
			 list_sort(posiblesAReemplazar,ordenAscendente);
			  t_list* vanASerReemplazadas = list_take_and_remove(posiblesAReemplazar,cv->tamanio / obtenerTamanioEntrada());
			  t_list* listaMapeada= list_map(vanASerReemplazadas, obtenerClave);
			  list_destroy(vanASerReemplazadas);
			  return listaMapeada;
		 }
		 else{
			 list_destroy(posiblesAReemplazar);
			 return NULL;
		 }
	//lista posiblesAReemplazar con algo
	}else{
	 	 if(list_size(posiblesAReemplazar)> sizeof(cv)){
	 		 list_sort(posiblesAReemplazar,ordenAscendente);
	 		 t_list* vanASerReemplazadas = list_take_and_remove(posiblesAReemplazar,cv->tamanio / obtenerTamanioEntrada());
	 		 t_list* listaMapeada= list_map(vanASerReemplazadas, obtenerClave);
	 		 list_destroy(vanASerReemplazadas);
	 		 return listaMapeada;
	 	 }else{
	 		 void list_add_all(posiblesAReemplazar,filter(tabla,esAtomica);



	 	 }

 }
}



*/




t_list* algoritmoLRU(claveEntrada* cv) {
	log_trace(logger, "Se procede a reemplazar mediante algoritmo LRU");
	t_list* posiblesAReemplazar= list_duplicate(tabla);
	list_sort(posiblesAReemplazar, ordenAscendentePorOperacion);
	log_trace(logger,"Se procede a chequear que la cantidad de entradas a reemplazar alcance para el tamaño del cv");
	if (list_size(posiblesAReemplazar) >= cv->tamanio) {
		log_trace(logger,"Alcanzaban: La cantidad de posiblesAReemplazar es %d, el tamaño del cv es: %d",list_size(posiblesAReemplazar), cv->tamanio);
		t_list* vanASerReemplazadas = list_take_and_remove(posiblesAReemplazar,cv->tamanio / obtenerTamanioEntrada());
		log_trace(logger,"Se ṕrocede a obtener el listado final de claves a ser reemplazadas");
		 t_list* clavesAReemplazar= list_map(vanASerReemplazadas,obtenerClave);
		free(posiblesAReemplazar);
		free(vanASerReemplazadas);
		return clavesAReemplazar;
	}
	free(posiblesAReemplazar);
	log_error(logger, "No se pudo encontrar clave a reemplazar");
	return NULL;
}





t_list* algoritmoBSU(claveEntrada* cv) {
	log_trace(logger, "Se procede a reemplazar mediante algoritmo BSU");
	t_list* posiblesAReemplazar = list_filter(tabla, esAtomica);
	list_sort(posiblesAReemplazar, ordenDescendentePorTamanio);
	log_trace(logger,"Se procede a chequear que la cantidad de entradas a reemplazar alcance para el tamaño del cv");
	if (list_size(posiblesAReemplazar) >= cv->tamanio) {
		log_trace(logger,"Alcanzaban: La cantidad de posiblesAReemplazar es %d, el tamaño del cv es: %d",list_size(posiblesAReemplazar), cv->tamanio);
		log_trace(logger,"Se ṕrocede a obtener el listado final de claves a ser reemplazadas");
		t_list* vanASerReemplazadas = list_take_and_remove(posiblesAReemplazar,cv->tamanio / obtenerTamanioEntrada());
		vanASerReemplazadas = list_map(vanASerReemplazadas,obtenerClave);
		free(posiblesAReemplazar);
		return vanASerReemplazadas;
	}

	free(posiblesAReemplazar);
	log_error(logger, "No se pudo encontrar clave a reemplazar");
	return NULL;

}

