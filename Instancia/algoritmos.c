/*
 * algoritmos.c
 *
 *  Created on: 14 jul. 2018
 *      Author: utnso
 */
#include "algoritmos.h"

void algoritmoCircular(claveEntrada* cv){
	 unsigned int tamanio=0;
	 t_list* atomicasNecesarias= list_create();
	 t_list* tablaDup = list_duplicate(tabla);
	 list_sort(tablaDup,ordenAscendente);
	 log_trace(logger,"la posicion inicial de algoritmo circular es(%d)",posicion);
		 for(int i =0; i< obtenerEntradasTotales();i++){
			log_trace(logger,"iteracion numero(%d) de (%d)",i, obtenerEntradasTotales());
			tablaE* unaEntrada= list_get(tablaDup,posicion+i);
			log_trace(logger,"obtengo la posicion (%d) de la tabla", posicion + i);
			if(esAtomica(unaEntrada) && tamanio<(cv->tamanio)){
				tamanio += unaEntrada->tamanio;
				posicion= posicion+i;
				log_trace(logger,"el nuevo puntero que me dice donde esta la ultima clave atomica es (%d)",posicion);
				list_add(atomicasNecesarias,unaEntrada->clave);
			}
			//la posicion solo puede dar negativa si estoy al final de la lista y vuelvo al inicio y no encuentro ninguna atomica
			if(unaEntrada->indice ==(obtenerEntradasTotales()-1)){
				posicion=-i-1;
			}
		 }

	if (posicion <0){
		tablaE* unaEntrada= list_get(atomicasNecesarias,(list_size(atomicasNecesarias)-1));
		posicion= unaEntrada->indice;
		//liberarEntrada(unaEntrada);
	}
	if(cv->tamanio> list_size(atomicasNecesarias)){
		log_trace(logger," el tamaño de las atomicas (%d) no me alcanza para cubrir el de mi entrada con tamaño (%d)",tamanio,cv->tamanio);
		list_destroy(atomicasNecesarias);
		list_destroy(tablaDup);
		posicion++;
		return ;
	}
	else{
	for(int i=0; i<list_size(atomicasNecesarias);i++){
		char* claveAPedir = list_get(atomicasNecesarias,0);
		tablaE* unaEntrada= buscarEntrada(claveAPedir);
		liberarEntrada(unaEntrada);
		//free(claveAPedir);
	}
	/// todo compactar solo si es necesario
	// todo avisar coordinador que compacto
	hacer_set(cv->clave,cv->valor);
	}
	list_destroy(atomicasNecesarias);
	list_destroy(tablaDup);
	posicion++;
}

void algoritmoLRU(claveEntrada* cv){
	log_trace(logger, "Se procede a reemplazar mediante algoritmo LRU");
	t_list* posiblesAReemplazar= list_duplicate(tabla);
	list_sort(posiblesAReemplazar, ordenAscendentePorOperacion);
	log_trace(logger,"Se procede a chequear que la cantidad de entradas a reemplazar alcance para el tamaño del cv");
	if (list_size(posiblesAReemplazar) >= cv->tamanio) {
		log_trace(logger,"Alcanzaban: La cantidad de posiblesAReemplazar es %d, el tamaño del cv es: %d",list_size(posiblesAReemplazar), cv->tamanio);
		t_list* vanASerReemplazadas = list_take_and_remove(posiblesAReemplazar,cv->tamanio / obtenerTamanioEntrada());
		if(listaNoContigua(vanASerReemplazadas)){
					// todo: compactar
		}
		log_trace(logger,"Se ṕrocede a obtener el listado final de claves a ser reemplazadas");
		 t_list* clavesAReemplazar= list_map(vanASerReemplazadas,obtenerClave);
		 log_trace(logger,"se procede a eliminar las claves a ser reemplazadas");
		 for(int i=0;i<list_size(clavesAReemplazar);i++){
			char* clave= list_get(clavesAReemplazar,i);
			log_trace(logger,"se obtuvo la clave (%s)",clave);
			tablaE* unaEntrada=buscarEntrada(clave);
			liberarEntrada(unaEntrada);
			free(clave);
			log_trace(logger,"se elimino la clave de la tabla de entradas");
		 	}
		hacer_set(cv->clave,cv->valor);
		list_destroy(posiblesAReemplazar);
		list_destroy(vanASerReemplazadas);
		list_destroy(clavesAReemplazar);
		return;
	}
	list_destroy(posiblesAReemplazar);
	log_error(logger, "No se pudo encontrar clave a reemplazar");
}




void algoritmoBSU(claveEntrada* cv){
	t_list* entradasAtomicas= list_filter(tabla,esAtomica);
	list_sort(entradasAtomicas,ordenDescendentePorTamanio);
	if(list_size(entradasAtomicas)>=(cv->tamanio)){
		log_trace(logger,"la cantidad de entradas atomicas son suficientes para mi nueva clave");
		t_list* vanASerReemplazadas = list_take_and_remove(entradasAtomicas,(cv->tamanio / obtenerTamanioEntrada()+1));
		if(listaNoContigua(vanASerReemplazadas)){
			// todo: compactar
		}
		t_list* clavesAReemplazar= list_map(vanASerReemplazadas,obtenerClave);
		log_trace(logger,"se procede a eliminar las claves a ser reemplazadas");
		for(int i=0;i<list_size(clavesAReemplazar);i++){
			char* clave= list_get(clavesAReemplazar,i);
			log_trace(logger,"se obtuvo la clave (%s)",clave);
			tablaE* unaEntrada=buscarEntrada(clave);
			liberarEntrada(unaEntrada);
			log_trace(logger,"se elimino la clave de la tabla de entradas");
			//free(clave);
		}
		hacer_set(cv->clave,cv->valor);
		list_destroy(clavesAReemplazar);
		list_destroy(entradasAtomicas);
		list_destroy(vanASerReemplazadas);


	}
	else{
	log_trace(logger," la cantidad de entradas atomicas no me alcanzaron para reemplazar mi clave");
	}
}


bool esAtomica(void* unaEntrada){
	tablaE* entrada= unaEntrada;
	return entrada->tamanio <= obtenerTamanioEntrada();
}

bool listaNoContigua(t_list* unaLista){
	for(int i=0; i<(list_size(unaLista)-1);i++){
			tablaE* unaEntrada= list_get(unaLista,i);
			tablaE* sigEntrada= list_get(unaLista,i+1);
			if((unaEntrada->indice + entradas_que_ocupa(unaEntrada))!=sigEntrada->indice-1){
				return true;
			}
	}
return false;
}


