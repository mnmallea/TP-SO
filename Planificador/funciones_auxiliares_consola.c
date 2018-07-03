/*
 * funciones_auxiliares_consola.c
 *
 *  Created on: 2 jul. 2018
 *      Author: utnso
 */

#include "funciones_auxiliares_consola.h"

bool contenido_en_lista(t_list* lista, int id_a_buscar){

	bool tiene_el_id_buscado(void* esi){
			return ((t_esi*)esi)->id == id_a_buscar;
		}

	return list_any_satisfy(lista, tiene_el_id_buscado);

}

bool es_un_esi_listo(int id_a_buscar){

	pthread_mutex_lock(&mutex_lista_esis_listos);
	bool resultado = contenido_en_lista(lista_esis_listos, id_a_buscar);
	pthread_mutex_unlock(&mutex_lista_esis_listos);

	return resultado;

}

bool es_un_esi_finalizado(int id_a_buscar){

	pthread_mutex_lock(&mutex_lista_esis_finalizados);
		bool resultado = contenido_en_lista(lista_esis_finalizados, id_a_buscar);
		pthread_mutex_unlock(&mutex_lista_esis_finalizados);

		return resultado;
}

bool es_un_esi_bloqueado(int id_a_buscar){

	t_list* lista_bloqueados = list_create();

	void agregar_a_lista_bloq(void* esi) {
			list_add(lista_bloqueados, esi);
	}

	void obtener_esis_bloq(char* c, void* data) {
			list_iterate((t_list*) data, agregar_a_lista_bloq);
	}

	pthread_mutex_lock(&mutex_dic_esis_bloqueados);
	dictionary_iterator(dic_esis_bloqueados, obtener_esis_bloq);
	pthread_mutex_lock(&mutex_dic_esis_bloqueados);


	bool resultado = contenido_en_lista(lista_bloqueados, id_a_buscar);

	list_destroy(lista_bloqueados);
	return resultado;

}

void eliminar_de_listos(t_esi* esi_a_eliminar){

	bool esElEsi(void* esi){
			return esi_a_eliminar->id == ((t_esi*)esi)->id;
		}

	pthread_mutex_lock(&mutex_lista_esis_listos);
	list_remove_by_condition(lista_esis_listos, esElEsi);
	pthread_mutex_unlock(&mutex_lista_esis_listos);
}

void eliminar_de_bloqueados(t_esi* esi_a_eliminar){

	char* clave_donde_se_encuentra = (char*) malloc(40);

	void buscar_esi_a_matar(char* c, void* lista_esis_bloq){
		if(contenido_en_lista(lista_esis_bloq, esi_a_eliminar->id)){
			clave_donde_se_encuentra = c;
		}
	}

	bool esElEsi(void* esi){
			return esi_a_eliminar->id == ((t_esi*)esi)->id;
		}

	pthread_mutex_lock(&mutex_dic_esis_bloqueados);
	dictionary_iterator(dic_esis_bloqueados, buscar_esi_a_matar);

	t_list* lista_afectada = dictionary_remove(dic_esis_bloqueados, clave_donde_se_encuentra);
	list_remove_by_condition(lista_afectada, esElEsi);
	dictionary_put(dic_esis_bloqueados, clave_donde_se_encuentra, lista_afectada);

	pthread_mutex_unlock(&mutex_dic_esis_bloqueados);

	free(clave_donde_se_encuentra);
}

t_esi* obtener_de_listos(int id_a_devolver){

	bool tiene_el_id_buscado(void* esi){
				return ((t_esi*)esi)->id == id_a_devolver;
	}

	return list_find(lista_esis_listos, tiene_el_id_buscado);
}

t_esi* obtener_de_bloqueados(int id_a_devolver){

	t_esi* esi_a_devolver;

	bool tiene_el_id_buscado(void* esi){
					return ((t_esi*)esi)->id == id_a_devolver;
		}

	void buscar_esi(char* c, void* lista_esis_bloq){
		if(contenido_en_lista(lista_esis_bloq, id_a_devolver)){
			esi_a_devolver = list_find(lista_esis_bloq, tiene_el_id_buscado);
		}
	}

	pthread_mutex_lock(&mutex_dic_esis_bloqueados);
	dictionary_iterator(dic_esis_bloqueados, buscar_esi);
	pthread_mutex_unlock(&mutex_dic_esis_bloqueados);

	return esi_a_devolver;
}



