/*
 * main.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#include "main.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include "../syntax-commons/my_socket.h"

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


	// menu();
	return 0;
}




