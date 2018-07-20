/*
 * error.c
 *
 *  Created on: 16 abr. 2018
 *      Author: utnso
 */

#include "error.h"

#include <stdlib.h>
#include <unistd.h>

#include "config_coordinador.h"
#include "instancia.h"
#include "log_operaciones.h"

void exit_error_with_msg(char* msg) {
	log_error(logger, msg);
	morir_liberando_recursos(EXIT_FAILURE);
}

void morir_liberando_recursos(int retorno) {
	log_debug(logger, "Finalizando ..");
	pthread_mutex_lock(&mutex_instancias_disponibles);
	list_destroy_and_destroy_elements(lista_instancias_disponibles,
			instancia_destroyer);
	pthread_mutex_unlock(&mutex_instancias_disponibles);

	pthread_mutex_lock(&mutex_instancias_inactivas);
	list_destroy_and_destroy_elements(lista_instancias_inactivas,
			instancia_destroyer);
	pthread_mutex_unlock(&mutex_instancias_inactivas);
	close(socket_planificador);
	limpiar_configuracion();
	log_info(logger, "Recursos liberados");
	log_destroy(logger);
	destruir_log_operaciones();
	exit(retorno);
}
