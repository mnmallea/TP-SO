/*
 * main.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#include "main.h"
int *c;


#define ipCord "127.0.0.1"
#define portCord "8000"
#define port "8001"
#define LOG_LEVEL LOG_LEVEL_TRACE

void *menu(void *ptr){

	char *message;
	message = (char *) ptr;
	printf("%s \n", message);

	int opcion_seleccionada;
	int clave;
	int recurso;
	int id;

		printf("Ingrese la opcion que desea ejecutar: "
					"\n 1: para pausar/continuar la ejecucion del planificador "
					"\n 2 <clave> <ID>: para bloquear un proceso"
					"\n 3 <clave>: para desbloquear un proceso bloqueado "
					"\n 4 <recurso>: listar procesos esperando al recurso"
					"\n 5 <ID>: para matar un proceso"
					"\n 6 <clave>: para conocer el status de la clave"
					"\n 7: para solucionar conflictos de deadlock "
					"\n 0: para salir de la consola \n"
					);

		scanf("%d", &opcion_seleccionada);

		while( opcion_seleccionada != 0 ){

			switch(opcion_seleccionada){
			case 1:
				printf("Ingreso pausar/despausar la consola");
				break;
			case 2:
				printf("Ingreso bloquear un proceso, ingrese <clave> ");

				scanf("%d", &clave);

				printf("Ingreso bloquear un proceso, ingrese <ID> ");

				scanf("%d", &id);
				break;
			case 3:
				printf("Ingreso desbloquear un proceso, ingrese <clave>");

				scanf("%d", &clave);
				break;
			case 4:
				printf("Ingreso listar procesos esperando un recurso, ingrese <recurso>");

				scanf("%d", &recurso);
				break;
			case 5:

				printf("Ingreso matar un proceso, ingrese <ID>");

				scanf("%d", &id);
				break;
			case 6:
				printf("Ingreso matar un proceso, ingrese <clave>");

				scanf("%d", &clave);
				break;
			case 7:
				printf("Ingreso solucionar problemas de deadlock");
				break;
			case 0:
				break;
			}

			scanf("%d", &opcion_seleccionada);

		}

	return NULL;

}

int main(void){
	logger = log_create("planificador.log","ESI",true,LOG_LEVEL);
/*
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
*/
	int local_socket=crear_socket_escucha(port,5);


    int socketCoord=crear_socket_cliente(ipCord,portCord);
		mandar_confirmacion(socketCoord);
		recibir_confirmacion(socketCoord);

	int client_socket=accept(local_socket,NULL,NULL);
	log_info(logger, "Conexion aceptada");
		recibir_confirmacion(client_socket);
		mandar_confirmacion(client_socket);




	// menu();
	return 0;
}




