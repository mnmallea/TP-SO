/*
 * main.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#include "main.h"


int main(int argc, char **argv){//aca recibiriamos la ruta del archivo de configuracion como parametro

	lista_esis_listos = list_create();
	esi_corriendo=(t_esi *)malloc(sizeof(t_esi));
	lista_esis_finalizados = list_create();
	dic_esis_bloqueados = dictionary_create();

	/*Config*/
	logger = log_create("planificador.log","Planificador",true,LOG_LEVEL);
	logger -> is_active_console=0;
	configuracion = configurar(argv[1]);

	/*Creacion de hilos*/
	pthread_t selector_planificador;
	pthread_t consola_planificador;

	const char *message0 = "Inicializacion el selector";
	if(pthread_create(&selector_planificador, NULL, listener, (void*) message0)) {
		log_error(logger, "Cantidad incorrecta de parametros");
			exit(EXIT_FAILURE);
	}
	const char *message1 = "Inicializacion de la consola";
	if(pthread_create(&consola_planificador, NULL, menu, (void*) message1)) {
		log_error(logger, "Error creando el hilo de la consola\n");
		 exit(EXIT_FAILURE);
	}

	/*Join threads*/
	if(pthread_join(selector_planificador, NULL)) {
		log_error(logger, "Error al joinear el hilo del selector\n");
			exit(EXIT_FAILURE);
	}
	if(pthread_join(consola_planificador, NULL)) {
		log_error(logger, "Error al joinear el hilo de la consola\n");
		 exit(EXIT_FAILURE);
	}
	return 0;
}


/*dejo estas funciones aca, despues vemos en donde las metemos y si van en un hilo aparte*/

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
}

void nuevo_esi(t_esi* esi){
	list_add(lista_esis_listos, esi);

	if(configuracion.algoritmo == SJFcD){
			obtener_nuevo_esi_a_correr();
		}

}

void finalizar_esi(t_esi* esi){
	list_add(lista_esis_finalizados, esi);
}

void bloquear_esi(t_esi* esi, char* clave){
	/* obtengo la lista de esis bloqueados para esta clave,
	 * agrego el esi a la lista de bloqueados
	 * actualizo en el diccionario la lista de bloqueados(con el nuevo esi) para esta clave
	 */

	t_list *lista_esis_bloq_esta_clave = dictionary_get(dic_esis_bloqueados,clave);
	list_add(lista_esis_bloq_esta_clave, esi);
	//habria que ver si el put pisa lo que esta,
	//o se deberia destruir la lista asociada a esa clave y recrearla despues
	dictionary_put(dic_esis_bloqueados,clave,lista_esis_bloq_esta_clave);


}

void pasar_esi_bloqueado_a_listo(t_esi* esi){
	list_add(lista_esis_listos, esi);

	if(configuracion.algoritmo == SJFcD){
		obtener_nuevo_esi_a_correr();
	}
}

void se_desbloqueo_un_recurso(char* clave){
	/*se debe buscar en el diccionario de claves y lista esis bloqueados
	 * por la clave y obtener la lista esis bloqueados para esa clave(dictionary_remove)
	 * cada uno de los elementos se los debe pasar a pasar_esi_bloqueado_a_listo
	 */

	t_list *lista_esis_bloq_esta_clave =  dictionary_get(dic_esis_bloqueados,clave);

	int cant_esis_bloq = list_size(lista_esis_bloq_esta_clave);

	for(int i=0; i< cant_esis_bloq; i++){
		pasar_esi_bloqueado_a_listo(list_get(lista_esis_bloq_esta_clave, i));
	}

	list_destroy_and_destroy_elements(lista_esis_bloq_esta_clave, (void*)free);

}




