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
	pthread_mutex_unlock(&mutex_dic_esis_bloqueados);


	bool resultado = contenido_en_lista(lista_bloqueados, id_a_buscar);

	list_destroy(lista_bloqueados); //tiene q ser un destroy and destroy elements
	return resultado;

}

void eliminar_de_listos(t_esi* esi_a_eliminar){

	bool esElEsi(void* esi){
			return esi_a_eliminar->id == ((t_esi*)esi)->id;
	}
	log_debug(logger,"x Lockear");
	pthread_mutex_lock(&mutex_lista_esis_listos);
	log_debug(logger,"Lockeo");
	list_remove_by_condition(lista_esis_listos, esElEsi);
	pthread_mutex_unlock(&mutex_lista_esis_listos);
	log_debug(logger,"ESI eliminado de listos");
	sem_wait(&contador_esis);
}

void eliminar_de_bloqueados(t_esi* esi_a_eliminar){ //debuggear esta funcion
	log_debug(logger,"ESI ID:%d, entra a eliminar bloqueados",esi_a_eliminar->id);
	bool esElEsi(void* esi){
				return esi_a_eliminar->id == ((t_esi*)esi)->id;
			}

	void borrar_esi_a_matar(char* c, void* lista_esis_bloq){
		if(contenido_en_lista(lista_esis_bloq, esi_a_eliminar->id)){
			list_remove_by_condition(lista_esis_bloq, esElEsi);
			log_debug(logger,"Se borra el esi ID:%d, de la lista de bloqueados",esi_a_eliminar->id);
				if(list_size(lista_esis_bloq)==0){
					dictionary_remove_and_destroy(dic_esis_bloqueados,c,free);
					log_debug(logger,"Se borra la clave del diccionario ya que no hay ningun esi bloqueado por la misma");
				}
		}
	}

	pthread_mutex_lock(&mutex_dic_esis_bloqueados);
	dictionary_iterator(dic_esis_bloqueados, borrar_esi_a_matar);
	pthread_mutex_unlock(&mutex_dic_esis_bloqueados);


}

t_esi* obtener_de_listos(int id_a_devolver){
	t_esi* esi_a_devolver;

	bool tiene_el_id_buscado(void* esi){
				return ((t_esi*)esi)->id == id_a_devolver;
	}

		pthread_mutex_lock(&mutex_lista_esis_listos);
		esi_a_devolver=list_find(lista_esis_listos, tiene_el_id_buscado);
		pthread_mutex_unlock(&mutex_lista_esis_listos);
		return esi_a_devolver;

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



