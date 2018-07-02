/*
 * consola.c
 *
 *  Created on: 12 may. 2018
 *      Author: utnso
 */

#include "consola.h"
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../syntax-commons/protocol.h"
#include "../syntax-commons/serializador.h"
#include "algoritmos_planificacion.h"
#include "main.h"
#include "planificacion.h"

void *menu(void *ptr) {

	int opcion_seleccionada;
	char *clave = (char*) malloc(40);
	int id;

	do {

		printf("Ingrese la opcion que desea ejecutar: "
				"\n 1: para pausar/continuar la ejecucion del planificador "
				"\n 2 <clave> <ID>: para bloquear un proceso"
				"\n 3 <clave>: para desbloquear un proceso bloqueado "
				"\n 4 <recurso>: listar procesos esperando al recurso"
				"\n 5 <ID>: para matar un proceso"
				"\n 6 <clave>: para conocer el status de la clave"
				"\n 7: para solucionar conflictos de deadlock "
				"\n 0: para salir de la consola \n"
				"\nOpcion n°:"
				);

		scanf("%d", &opcion_seleccionada);

		printf("\n");

		switch (opcion_seleccionada) {
		case 1:
			pausar_despausar_consola();
			break;

		case 2:
			printf("Ingreso bloquear un proceso, ingrese <clave>");

			scanf("%s", clave);

			printf("Ingreso bloquear un proceso, ingrese <ID>");

			scanf("%d", &id);

			bloquear(clave, id);
			break;
		case 3:
			printf("Ingreso desbloquear un proceso, ingrese <clave>");

			scanf("%s", clave);
			se_desbloqueo_un_recurso(clave);

			break;
		case 4:
			printf(
					"Ingreso listar procesos esperando un recurso, ingrese <recurso>");

			scanf("%s", clave);
			listar(clave);
			break;
		case 5:

			printf("Ingreso matar un proceso, ingrese <ID>");

			scanf("%d", &id);
			matar_por_consola(id);
			break;
		case 6:
			printf("Ingreso status de una clave, ingrese <clave>");

			scanf("%s", clave);

			envia_status_clave(clave);
			break;
		case 7:
			printf("Ingreso solucionar problemas de deadlock");
			//deadlock();
			break;
		case 0:
			break;
		default:
			break;

		}

		getchar();
		printf("\nPresione Enter para continuar..\n");
		getchar();

		system("clear");



	} while (opcion_seleccionada != 0);

	printf("Ha salido de la consola");
	free(clave);

	return NULL;
}

void pausar_despausar_consola() {
	pthread_mutex_lock(&mutex_pausa);
	if (planificacion_pausada) {
		printf("Continuando con la planificacion\n");
		planificacion_pausada = false;
		sem_post(&pausa_planificacion);
	} else {
		printf("Planificador pausado\n");
		planificacion_pausada = true;
	}
	pthread_mutex_unlock(&mutex_pausa);
}

void listar(char* rec) {

	/*supongo q voy al diccionario y hago dictionary_get(clave)
	 * despues muestro por pantalla la lista
	 */

	if (!dictionary_has_key(dic_esis_bloqueados, rec)) {
		printf("No existe esa clave\n");
	} else {
		t_list* esis_bloq = dictionary_get(dic_esis_bloqueados, rec);
		list_iterate(esis_bloq, mostrar_esi_en_pantalla);
	}
}

void mostrar_esi_en_pantalla(void* esi) {

	printf("Esi bloqueado para este recurso: %d \n", ((t_esi*) esi)->id);
}

void bloquear(char* clave, int id) {

	bloquear_esi_por_consola(clave, id);
}

void matar_por_consola(int id) {

	t_esi* esi_a_matar = buscar_esi_por_id(id);
	if (esi_a_matar == NULL) {
		log_debug(logger, "El esi elegido no existe en el sistema");
	} else {
		log_debug(logger,
				"Se encontro el esi  %d en el sistema, se procede a matarlo",
				esi_a_matar->id);

		if (esi_a_matar->id != esi_corriendo->id) {
			log_debug(logger, "Se mato el esi  %d", esi_a_matar->id);
			liberar_recursos(esi_a_matar);
			matar_nodo_esi(esi_a_matar);

		} else {
			//es el esi corriendo
		}


	}

}

void envia_status_clave(char* clave) {

	t_paquete* paquete = paquete_crear();
	paquete_agregar(paquete, clave, strlen(clave) + 1);
	if (paquete_enviar_con_codigo(paquete, SOLICITUD_STATUS_CLAVE, socketCord)
			< 0) {
		log_error(logger, "Error enviandole el paquete al coordindor");
		paquete_destruir(paquete);
		exit(EXIT_FAILURE);
	}
	paquete_destruir(paquete);

	while (1) {
		sem_wait(&coordinador_respondio_paq);

		//todo:  acceder a la variable compartida
		//ponerle mutex a la variable
		//hacer printf de las cosas que me mando
	}

}

/*void deadlock(){
 t_list* esis_deadlock = obtener_procesos_en_deadlock();
 list_iterate(esis_deadlock, mostrar_esi_en_pantalla);
 list_destroy(esis_deadlock);
 }*/
