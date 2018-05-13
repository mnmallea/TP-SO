/*
 * consola.c
 *
 *  Created on: 12 may. 2018
 *      Author: utnso
 */

#include "consola.h"

void *menu(void *ptr){

	char *message;
	message = (char *) ptr;
	printf("%s \n", message);

	int opcion_seleccionada;
	char *clave = (char*)malloc(40);
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
				printf("Ingreso bloquear un proceso, ingrese <clave>");

				//ACA ME TIRA UN WARNING de que clave es char **
				scanf("%s", &clave);

				printf("Ingreso bloquear un proceso, ingrese <ID>");

				scanf("%d", &id);
				break;
			case 3:
				printf("Ingreso desbloquear un proceso, ingrese <clave>");

				scanf("%s", &clave);
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

				scanf("%s", &clave);
				break;
			case 7:
				printf("Ingreso solucionar problemas de deadlock");
				break;
			case 0:
				break;
			default:
				break;
			}

			scanf("%d", &opcion_seleccionada);

		}

	printf("Ha salido de la consola");
	free(clave);

	return NULL;
}
