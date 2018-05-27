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
 * -NUEVO PROCESO LLEGA (o uno se desbloquea)
 */

flag = 0;

void obtener_nuevo_esi_a_correr() {

	pthread_mutex_lock(&mutex_flag_pausa_despausa);
	if(flag==0){
		if(configuracion.algoritmo == FIFO){
			esi_corriendo = obtener_proximo_segun_fifo(lista_esis_listos);
		}else if(configuracion.algoritmo == SJFsD){
			esi_corriendo = obtener_proximo_segun_sjf(lista_esis_listos);
		}else if(configuracion.algoritmo == SJFcD){
			esi_corriendo = obtener_proximo_segun_sjf(lista_esis_listos);
		}else{
			esi_corriendo = obtener_proximo_segun_hrrn(lista_esis_listos);
		}

	}

	pthread_mutex_unlock(&mutex_flag_pausa_despausa);
	log_debug(logger, "Proximo esi a correr: %d \n", esi_corriendo->id);


	/*ACA SE DE LE DEBE MANDAR AL ESI_CORRIENDO POR SOCKETS QUE MANDE A EJECUTAR
	 * LA SIGUIENTE LINEA
	 */



}

void nuevo_esi(t_esi* esi){
	list_add(lista_esis_listos, esi);

	log_debug(logger, "Llego/se desbloqueo un nuevo esi: %d \n", esi>id);
	if(configuracion.algoritmo == SJFcD){
			obtener_nuevo_esi_a_correr();
		}


}

void finalizar_esi(){
	list_add(lista_esis_finalizados, esi_corriendo);
	log_debug(logger, "Termino un esi: %d \n", esi->id);
	obtener_nuevo_esi_a_correr();
}

void bloquear_esi(char* clave){

	//No existe la clave, agrego esta nueva linea
	if(!dictionary_has_key(dic_esis_bloqueados, clave)){
		dictionary_put(dic_esis_bloqueados, clave, esi_corriendo);

	}else{ //Existe la clave, agrego el esi a la lista de bloq

		t_list *lista_esis_bloq_esta_clave = dictionary_remove(dic_esis_bloqueados,clave);
		list_add(lista_esis_bloq_esta_clave, esi_corriendo);
		dictionary_put(dic_esis_bloqueados,clave,lista_esis_bloq_esta_clave);

	}

	log_debug(logger, "Se bloqueo el esi: %d para la clave: %s \n", esi_corriendo->id, clave);
	obtener_nuevo_esi_a_correr();
}

int id;
void bloquear_esi_por_consola(char* clave, int id_esi){

	id = id_esi;

	t_esi *esi_afectado = buscar_esi_por_id(id_esi);
	//No existe la clave, agrego esta nueva linea
		if(!dictionary_has_key(dic_esis_bloqueados, clave)){
			dictionary_put(dic_esis_bloqueados, clave, esi_afectado);

		}else{ //Existe la clave, agrego el esi a la lista de bloq

			t_list *lista_esis_bloq_esta_clave = dictionary_remove(dic_esis_bloqueados,clave);
			list_add(lista_esis_bloq_esta_clave, esi_afectado);
			dictionary_put(dic_esis_bloqueados,clave,lista_esis_bloq_esta_clave);

		}
	log_debug(logger, "Se bloqueo por consola el esi: %d para la clave: %s \n", esi_corriendo->id, clave);

}


t_esi buscar_esi_por_id(int id_esi){

	//me fijo si es el esi que esta corriendo
	if(esi_corriendo->id == id){
		return esi_corriendo; //COMO HAGO QUE ESPERE HASTA QUE TERMINE DE CORRER?

	}else{
		//si no es el q esta corriendo lo busco en la lista de esis listos
		t_esi esi= list_find(lista_esis_listos, esi_con_este_id);
		return esi;
	}


}

bool esi_con_este_id(void* esi){
	return ((t_esi*)esi)->id == id;
}

void se_desbloqueo_un_recurso(char* clave){

	t_list *lista_esis_bloq_esta_clave =  dictionary_get(dic_esis_bloqueados,clave);
	t_esi* esi_desbloq = list_remove(lista_esis_bloq_esta_clave, 0);
	nuevo_esi(esi_desbloq);
	dictionary_put(dic_esis_bloqueados,clave,lista_esis_bloq_esta_clave);

	nuevo_esi(esi_desbloq);

}



