/*
 * main.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#include "main.h"


int main(int argc, char **argv){//aca recibiriamos la ruta del archivo de configuracion como parametro

	lista_esis_listos = list_create();
	lista_esis_corriendo = *list_create();
	lista_esis_bloq_consola = *list_create();
	lista_esis_bloq_rec = *list_create();
	lista_esis_finalizados = *list_create();

	logger = log_create("planificador.log","Planificador",true,LOG_LEVEL);
	configuracion = configurar(argv[1]);
	log_trace(logger, "Planificador correctamente configurado");


/*

	log_trace(logger,"Intentando conectarse al Coordinador. IP: %s  Puerto: %s", configuracion.ipCoord, configuracion.portCoord);
	int socketCoord=crear_socket_cliente(configuracion.ipCoord,configuracion.portCoord);
			mandar_confirmacion(socketCoord);
			recibir_confirmacion(socketCoord);

*/


	/*aca iria el select
	 * el select debe: crear un nuevo esi, instanciandole la ip y el puerto
	 * debe agregarlo a la lista de esis nuevos con la funcion list_add
	 * */

	pthread_t selector_planificador;
	const char *message0 = "Inicializacion el selector";
	if(pthread_create(&selector_planificador, NULL, selector, (void*) message0)) {
		log_error(logger, "Cantidad incorrecta de parametros");
			exit(EXIT_FAILURE);
	}


/*
	pthread_t consola_planificador;
	const char *message1 = "Inicializacion de la consola";

	if(pthread_create(&consola_planificador, NULL, menu, (void*) message1)) {

		log_error(logger, "Error creando el hilo de la consola\n");
		 exit(EXIT_FAILURE);
	}
*/
	if(pthread_join(selector_planificador, NULL)) {
		log_error(logger, "Error al joinear el hilo del selector\n");
			exit(EXIT_FAILURE);
	}
/*
	if(pthread_join(consola_planificador, NULL)) {

		log_error(logger, "Error al joinear el hilo de la consola\n");
		 exit(EXIT_FAILURE);
	}

*/


	return 0;
}





