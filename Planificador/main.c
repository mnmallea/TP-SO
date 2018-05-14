/*
 * main.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#include "main.h"


int main(int argc, char **argv){//aca recibiriamos la ruta del archivo de configuracion como parametro

	lista_esis_listos = list_create();
	lista_esis_corriendo = list_create();
	lista_esis_bloq_consola = list_create();
	lista_esis_bloq_rec = list_create();
	lista_esis_finalizados = list_create();

	/*Config*/
	logger = log_create("planificador.log","Planificador",true,LOG_LEVEL);
	configuracion = configurar(argv[1]);
	log_trace(logger, "Planificador correctamente configurado");


	/*Creacion de hilos*/
	pthread_t selector_planificador;
	pthread_t consola_planificador;

	const char *message0 = "Inicializacion el selector";
	if(pthread_create(&selector_planificador, NULL, listening, (void*) message0)) {
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





