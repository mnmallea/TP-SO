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
	char *clave = (char*)malloc(40) ;
	char *recurso = (char*)malloc(40);
	int id;


		do{

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

			switch(opcion_seleccionada){
			case 1:
				pausar_despausar_consola();
				break;

			case 2:
				printf("Ingreso bloquear un proceso, ingrese <clave>");

				//ACA ME TIRA UN WARNING de que clave es char **
				scanf("%s", clave);

				printf("Ingreso bloquear un proceso, ingrese <ID>");

				scanf("%d", &id);

				bloquear(clave, id);
				break;
			case 3:
				printf("Ingreso desbloquear un proceso, ingrese <clave>");

				scanf("%s", clave);
				desbloquear(clave);

				break;
			case 4:
				printf("Ingreso listar procesos esperando un recurso, ingrese <recurso>");

				scanf("%s", recurso);
				listar(recurso);
				break;
			case 5:

				printf("Ingreso matar un proceso, ingrese <ID>");

				scanf("%d", &id);
				break;
			case 6:
				printf("Ingreso matar un proceso, ingrese <clave>");

				scanf("%s", clave);
				break;
			case 7:
				printf("Ingreso solucionar problemas de deadlock");
				break;
			case 0:
				break;
			default:
				break;

			}


		}while(opcion_seleccionada != 0);

	printf("Ha salido de la consola");
	free(clave);

	return NULL;
}


void pausar_despausar_consola(){
	pthread_mutex_lock(&mutex_flag_pausa_despausa);
	if(flag==0){
	printf("Planificador pausado\n");
	flag++;
	}else{
	printf("Continuando con la planificacion\n");
	flag--;
	}
	pthread_mutex_unlock(&mutex_flag_pausa_despausa);

}

void listar(char* rec){

	/*supongo q voy al diccionario y hago dictionary_get(clave)
	 * despues muestro por pantalla la lista
	 */

	if(!dictionary_has_key(dic_esis_bloqueados, rec)){
		printf("No existe esa clave\n");
	}else{
		t_list* esis_bloq = dictionary_get(dic_esis_bloqueados,rec);
		list_iterate(esis_bloq, mostrar_esi_en_pantalla);
	}
}


void mostrar_esi_en_pantalla(void* esi){

	printf("Esi bloqueado para este recurso: %d \n", ((t_esi*)esi)->id);
}

void bloquear(char* clave, int id){

	bloquear_esi_por_consola(clave, id);
}

void desbloquear(char* clave){
	desbloquear_por_consola(clave); //falta definirlo
}
