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

void obtener_nuevo_esi_a_correr() {

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

	}
	else{ //Existe la clave, agrego el esi a la lista de bloq

		t_list *lista_esis_bloq_esta_clave = dictionary_remove(dic_esis_bloqueados,clave);

		list_add(lista_esis_bloq_esta_clave, esi_corriendo);

		dictionary_put(dic_esis_bloqueados,clave,lista_esis_bloq_esta_clave);

	}

	obtener_nuevo_esi_a_correr();



}

void se_desbloqueo_un_recurso(char* clave){

	t_list *lista_esis_bloq_esta_clave =  dictionary_get(dic_esis_bloqueados,clave);
	t_esi* esi_desbloq = list_remove(lista_esis_bloq_esta_clave, 0);
	nuevo_esi(esi_desbloq);
	dictionary_put(dic_esis_bloqueados,clave,lista_esis_bloq_esta_clave);

	nuevo_esi(esi_desbloq);

}




