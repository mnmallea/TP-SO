/*
 * main.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#include "main.h"




t_log *logger;
config configuracion;


int main(int argc, char **argv){//aca recibiriamos la ruta del archivo de configuracion como parametro

	logger = log_create("planificador.log","Planificador",true,LOG_LEVEL);
	configuracion = configurar(argv[1]);
	log_trace(logger, "Planificador correctamente configurado");


	log_trace(logger, "Lista de Esis creadas correctamente");

	log_trace(logger,"Intentando conectarse al Coordinador. IP: %s  Puerto: %s", configuracion.ipCoord, configuracion.portCoord);
	int socketCoord=crear_socket_cliente(configuracion.ipCoord,configuracion.portCoord);
			mandar_confirmacion(socketCoord);
			recibir_confirmacion(socketCoord);




	/*aca iria el select
	 * el select debe: crear un nuevo esi, instanciandole la ip y el puerto
	 * debe agregarlo a la lista de esis nuevos con la funcion list_add
	 * */

	pthread_t selector_planificador;
	const char *message0 = "Inicializacion el selector";
	if(pthread_create(&selector_planificador, NULL, selector, (void*) message0)) {

					fprintf(stderr, "Error creando el hilo de la consola\n");
					return 1;
				}



	pthread_t consola_planificador;
	const char *message1 = "Inicializacion de la consola";

	if(pthread_create(&consola_planificador, NULL, menu, (void*) message1)) {

		fprintf(stderr, "Error creando el hilo de la consola\n");
		return 1;
	}

	if(pthread_join(consola_planificador, NULL)) {

		fprintf(stderr, "Error al joinear el hilo de la consola\n");
		return 1;
	}




	return 0;
}





