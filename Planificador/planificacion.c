/*
 * planificacion.c
 *
 *  Created on: 17 may. 2018
 *      Author: utnso
 */

#include "planificacion.h"

/* SE DEBE REPLANIFICAR CUANDO:
 *
 * SI USO HRRN/SJF SIN DESALOJO:
 * -SE BLOQUEA UN PROCESO
 * -FINALIZA UN PROCESO
 *
 * SI USO SJF
 * -SE BLOQUEA UN PROCESO
 * -FINALIZA UN PROCESO
 * -NUEVO PROCESO LLEGA
 */

void obtener_nuevo_esi_a_correr(){

	if(configuracion.algoritmo == FIFO){
		esi_corriendo = obtener_proximo_segun_fifo(lista_esis_listos);
	}else if(configuracion.algoritmo == SJFsD){
		esi_corriendo = obtener_proximo_segun_sjf(lista_esis_listos);
	}else if(configuracion.algoritmo == SJFcD){
		esi_corriendo = obtener_proximo_segun_sjf(lista_esis_listos);
	}else{
		esi_corriendo = obtener_proximo_segun_hrrn(lista_esis_listos);
	}


	/*ACA SE DE LE DEBE MANDAR AL ESI_CORRIENDO POR SOCKETS QUE MANDE A EJECUTAR
	 * LA SIGUIENTE LINEA
	 */

}

void nuevo_esi(t_esi* esi){
	list_add(lista_esis_listos, esi);

	if(configuracion.algoritmo == SJFcD){
			obtener_nuevo_esi_a_correr();
		}

}

void finalizar_esi(){
	list_add(lista_esis_finalizados, esi_corriendo);
	obtener_nuevo_esi_a_correr();
}

void bloquear_esi(char* clave){

	//No existe la clave, agrego esta nueva linea
	if(!dictionary_has_key(dic_esis_bloqueados, clave)){
		dictionary_put(dic_esis_bloqueados, clave, esi_corriendo);

	}else{ //Existe la clave, agrego el esi a la lista de bloq

		t_list *lista_esis_bloq_esta_clave = dictionary_remove(dic_esis_bloqueados,clave);
		//PREGUNTAR: el remove me saca esta linea????

		list_add(lista_esis_bloq_esta_clave, esi_corriendo);

		dictionary_put(dic_esis_bloqueados,clave,lista_esis_bloq_esta_clave);

	}



}

void se_desbloqueo_un_recurso(char* clave){

	t_list *lista_esis_bloq_esta_clave =  dictionary_get(dic_esis_bloqueados,clave);

	int cant_esis_bloq = list_size(lista_esis_bloq_esta_clave);

	for(int i=0; i< cant_esis_bloq; i++){
		t_esi *esi_bloqueado = (list_get(lista_esis_bloq_esta_clave, i));
		list_add(lista_esis_listos, esi_bloqueado);
	}

	list_destroy_and_destroy_elements(lista_esis_bloq_esta_clave, (void*)free);

	//COMO ELIMINO UNA LINEA DEL DICCIONARIO???
	dictionary_remove_and_destroy(dic_esis_bloqueados, clave, (void*)free);

	obtener_nuevo_esi_a_correr();

}

