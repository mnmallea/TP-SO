/*
 * consola.c
 *
 *  Created on: 12 may. 2018
 *      Author: utnso
 */

#include "consola.h"

respuesta_status_clave_t* respuesta_status_clave;

void limpiar_respuesta_status_clave();

void *menu(void *ptr) {

	int opcion_seleccionada;
	char *clave = (char*) malloc(40);
	int id;
	respuesta_status_clave = NULL;

	do {
		system("clear");

		printf("Ingrese la opcion que desea ejecutar: "
				"\n 1: para pausar/continuar la ejecucion del planificador "
				"\n 2 <clave> <ID>: para bloquear un proceso"
				"\n 3 <clave>: para desbloquear un proceso bloqueado "
				"\n 4 <recurso>: listar procesos esperando al recurso"
				"\n 5 <ID>: para matar un proceso"
				"\n 6 <clave>: para conocer el status de la clave"
				"\n 7: Vista de ESIs en deadlock "
				"\n 0: Para salir de la consola \n"
				"Opcion n°:");

		scanf("%d", &opcion_seleccionada);

		printf("\n");

		switch (opcion_seleccionada) {
		case 1:
			pausar_despausar_consola();
			break;

		case 2:
			printf("Ingreso bloquear un proceso, ingrese <clave>:");

			scanf("%s", clave);

			printf("Ingreso bloquear un proceso, ingrese <ID>:");

			scanf("%d", &id);

			bloquear(clave, id);
			break;
		case 3:
			printf("Ingreso desbloquear un proceso, ingrese <clave>:");

			scanf("%s", clave);
			se_desbloqueo_un_recurso(clave);

			break;
		case 4:
			printf(
					"Ingreso listar procesos esperando un recurso, ingrese <recurso>:");

			scanf("%s", clave);
			listar(clave);
			break;
		case 5:

			printf("Ingrese el <ID> del ESI a matar:");

			scanf("%d", &id);
			matar_por_consola(id);
			break;
		case 6:
			printf("Ingrese <clave> a verificar el estado:");

			scanf("%s", clave);

			envia_status_clave(clave);
			listar(clave);
			break;
		case 7:
			printf("Deadlocks activos en el sistema:\n");
			deadlock();
			break;
		case 0:
			break;
		default:
			break;

		}

		getchar();
		printf("\nPresione [Enter] para continuar..");
		getchar();

	} while (opcion_seleccionada != 0);

	printf("Ha salido de la consola");
	free(clave);
	free(clave_a_bloquear);

	return NULL;
}

void pausar_despausar_consola() {

	pthread_mutex_lock(&mutex_pausa);
	if (planificacion_pausada) {
		printf("Continuando con la planificacion\n");
		log_debug(logger,
				"CONSOLA: pausar/despausar. Actualmente consola pausada. Se continua planificacion");
		planificacion_pausada = false;
		sem_post(&pausa_planificacion);
	} else {
		printf("Planificador pausado\n");
		log_debug(logger,
				"CONSOLA: pausar/despausar. Actualmente consola planificando. Se pausa la planificacion");
		planificacion_pausada = true;
	}
	pthread_mutex_unlock(&mutex_pausa);
}

void listar(char* rec) {

	pthread_mutex_lock(&mutex_dic_esis_bloqueados);

	if (!dictionary_has_key(dic_esis_bloqueados, rec)) {

		pthread_mutex_unlock(&mutex_dic_esis_bloqueados);
		printf("La clave solicitada(%s) no tiene ningun ESI bloqueado", rec);
		log_debug(logger,
				"CONSOLA: Se eligio listar los esis bloqueados para la clave: %s",
				rec);

	} else {

		printf("Se procede a listar los ESIS encolados para la clave: %s\n",
				rec);
		log_debug(logger,
				"CONSOLA: Se listaran los esis encolados para la clave: %s",
				rec);

		t_list* esis_bloq = dictionary_get(dic_esis_bloqueados, rec);
		pthread_mutex_unlock(&mutex_dic_esis_bloqueados);
		list_iterate(esis_bloq, mostrar_esi_en_pantalla);

	}

}

void mostrar_esi_en_pantalla(void* esi) {

	printf("Esi bloqueado para este recurso: %d \n", ((t_esi*) esi)->id);
}

void bloquear(char* clave, int id) {

	printf("Se procede a bloquear al esi: %d para la clave: %s\n", id, clave);
	log_debug(logger, "CONSOLA: Se eligio bloquear al esi: %d para la clave %s",
			id, clave);

	pthread_mutex_lock(&mutex_esi_corriendo);
	if (esi_corriendo != NULL && esi_corriendo->id == id) {

		pthread_mutex_lock(&mutex_esi_a_matar_por_consola);
		if (esi_a_matar_por_consola != NULL) {
			//ya habian pedido matarlo
			pthread_mutex_unlock(&mutex_esi_a_matar_por_consola);
			printf(
					"No se puede bloquear al esi: %d por consola, ya se solicito matarlo",
					id);
		} else {
			pthread_mutex_unlock(&mutex_esi_a_matar_por_consola);
			pthread_mutex_lock(&mutex_esi_a_bloquear_por_consola);
			esi_a_bloquear_por_consola = esi_corriendo;
			pthread_mutex_unlock(&mutex_esi_a_bloquear_por_consola);

			pthread_mutex_lock(&mutex_clave_a_bloquear);
			clave_a_bloquear = strdup(clave);
			pthread_mutex_unlock(&mutex_clave_a_bloquear);
		}

		pthread_mutex_unlock(&mutex_esi_corriendo);
	} else {
		pthread_mutex_unlock(&mutex_esi_corriendo);
		if (es_un_esi_listo(id)) {

			t_esi* esi_a_bloquear = obtener_de_listos(id);
			eliminar_de_listos(esi_a_bloquear);
			bloquear_esi(clave, esi_a_bloquear);
		} else if (es_un_esi_bloqueado(id)) {
			printf(
					"El esi solicitado para el bloqueo(%d) ya se encontraba bloqueado",
					id);
		} else if (es_un_esi_finalizado(id)) {
			printf(
					"El esi solicitado para el bloqueo(%d) ya se encontraba finalizado",
					id);
		} else {
			printf(
					"El esi solicitado para el bloqueo(%d) no existe en el sistema",
					id);
		}
	}
}

void matar_por_consola(int id) {

	printf("Se procede a matar al esi: %d", id);
	log_debug(logger, "CONSOLA: Se eligio matar al esi: %d", id);

	pthread_mutex_lock(&mutex_esi_corriendo);
	if (esi_corriendo != NULL && esi_corriendo->id == id) {

		pthread_mutex_lock(&mutex_esi_a_bloquear_por_consola);
		if (esi_a_bloquear_por_consola != NULL) {
			//ya habian pedido bloquearlo
			pthread_mutex_unlock(&mutex_esi_a_bloquear_por_consola);
			printf(
					"No puede solicitar matar al esi: %d por consola, ya se solicito bloquearlo",
					id);
		} else {
			pthread_mutex_unlock(&mutex_esi_a_bloquear_por_consola);
			pthread_mutex_lock(&mutex_esi_a_matar_por_consola);
			esi_a_matar_por_consola = esi_corriendo;
			pthread_mutex_unlock(&mutex_esi_a_matar_por_consola);
		}
		pthread_mutex_unlock(&mutex_esi_corriendo);
	} else {
		pthread_mutex_unlock(&mutex_esi_corriendo);
		if (es_un_esi_listo(id)) {
			t_esi* esi_a_matar = obtener_de_listos(id);
			finalizar_esi_sync(esi_a_matar);
			eliminar_de_listos(esi_a_matar);
		} else if (es_un_esi_finalizado(id)) {
			printf(
					"El esi solicitado para matar(%d) ya se encontraba finalizado",
					id);
		} else if (es_un_esi_bloqueado(id)) {
			t_esi* esi_a_matar = obtener_de_bloqueados(id);
			finalizar_esi_sync(esi_a_matar);
			eliminar_de_bloqueados(esi_a_matar);

		} else {
			printf("El esi solicitado para matar(%d) no existe en el sistema",
					id);
		}
	}
}

void envia_status_clave(char* clave) {

	printf("Se procede a mostrar el status de la clave: %s\n", clave);
	log_debug(logger, "CONSOLA: Se eligio status clave para: %s", clave);

	t_paquete* paquete = paquete_crear();
	paquete_agregar(paquete, clave, strlen(clave) + 1);
	if (paquete_enviar_con_codigo(paquete, SOLICITUD_STATUS_CLAVE, socketCord)
			< 0) {
		log_error(logger,
				"Error enviandole el paquete de status clave al coordindor");
		paquete_destruir(paquete);
		exit(EXIT_FAILURE);
	}
	paquete_destruir(paquete);
	sem_wait(&coordinador_respondio_paq);

	show_respuesta_status_clave(respuesta_status_clave);
	limpiar_respuesta_status_clave();
}
void show_respuesta_status_clave(respuesta_status_clave_t* res) {
	if (res->hay_valor)
		printf("Valor de la clave: %s\n", res->valor);
	else
		printf("No hay valor para la clave solicitada\n");

	if (res->hay_instancia)
		printf("Instancia: %s \t Estado: %s\n", res->instancia,
				to_string_status_clave(res->estado_instancia));
	else
		printf("%s\n", to_string_status_clave(res->estado_instancia));

	if (res->hay_simulacion)
		printf("La clave sería asignada a la instancia: %s\n",
				res->instancia_simulacion);
	else
		printf("No se ha realizado la simulacion\n");

}

void limpiar_respuesta_status_clave() {
	if (respuesta_status_clave == NULL)
		return;
	free(respuesta_status_clave->instancia);
	free(respuesta_status_clave->instancia_simulacion);
	free(respuesta_status_clave->valor);
	free(respuesta_status_clave);
	respuesta_status_clave = NULL;
}
