/*
 * funciones_auxiliares.c
 *
 *  Created on: 2 jul. 2018
 *      Author: utnso
 */

#include "funciones_auxiliares.h"

void agregar_a_dic_bloqueados(char* clave, t_esi *esi) {

	//Me fijo si ya hay algun esi bloqueado para esa clave

	pthread_mutex_lock(&mutex_dic_esis_bloqueados);

	if (!dictionary_has_key(dic_esis_bloqueados, clave)) {
		t_list *primer_esi = list_create();
		list_add(primer_esi, esi);
		dictionary_put(dic_esis_bloqueados, clave, primer_esi);

	} else { //Existe la clave, agrego el esi a la lista de bloq
		t_list *lista_esis_bloq_esta_clave = dictionary_remove(
				dic_esis_bloqueados, clave);
		list_add(lista_esis_bloq_esta_clave, esi);
		dictionary_put(dic_esis_bloqueados, clave, lista_esis_bloq_esta_clave);

	}

	pthread_mutex_unlock(&mutex_dic_esis_bloqueados);

}

bool esi_tiene_clave(char* clave, t_esi* esi_a_validar) {

	pthread_mutex_lock(&mutex_dic_clave_x_esi);

	//valido si el esi se encuentra tomando alguna clave
	if (dictionary_has_key(dic_clave_x_esi, clave)) {
		t_esi* esi_que_la_tomo = dictionary_get(dic_clave_x_esi, clave);
		pthread_mutex_unlock(&mutex_dic_clave_x_esi);

		return (esi_que_la_tomo->id == esi_a_validar->id);
	} else {
		pthread_mutex_unlock(&mutex_dic_clave_x_esi);
		return false;
	}

}

bool puede_tomar_la_clave(char* clave, t_esi* esi_solicitante) {

	pthread_mutex_lock(&mutex_dic_clave_x_esi);

	//valido si la clave pedida ya estaba tomada por otro esi
	if (dictionary_has_key(dic_clave_x_esi, clave)) {

		t_esi* esi_que_la_tomo = dictionary_get(dic_clave_x_esi, clave);

		pthread_mutex_unlock(&mutex_dic_clave_x_esi);

		if(esi_que_la_tomo->id == esi_solicitante->id){
			//la esta volviendo a pedir
			return true;
		}else{
			//la tiene pedida otro
			return false;
		}

	}else{
		//no la tiene nadie, la puede tomar
		return true;
	}


}

void nueva_clave_tomada_x_esi(char* clave, t_esi* esi_solicitante) {

	pthread_mutex_lock(&mutex_dic_clave_x_esi);

	dictionary_put(dic_clave_x_esi, clave, esi_solicitante);

	pthread_mutex_unlock(&mutex_dic_clave_x_esi);

}

void aumentar_viene_esperando(void* esi) {
	((t_esi*) esi)->viene_esperando = ((t_esi*) esi)->viene_esperando + 1;
}

void aumentar_viene_corriendo(void* esi) {
	((t_esi*) esi)->dur_ult_raf = ((t_esi*) esi)->dur_ult_raf + 1;
}
