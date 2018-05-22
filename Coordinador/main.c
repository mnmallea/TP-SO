/*
 * main.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */
#include "main.h"

int main(int argc, char **argv) { //aca recibiriamos la ruta del archivo de configuracion como parametro
	logger = log_create("coordinador.log", "Coordinador", true, LOG_LEVEL);
	configuracion = configurar(argv[1]);
	log_trace(logger, "Coordinador correctamente configurado");
	lista_instancias_disponibles = list_create();
	pthread_mutex_init(&mutex_instancias_disponibles, NULL);
	sem_init(&contador_instancias_disponibles, 0, 0);
	cant_instancias = 0;

	int local_socket = crear_socket_escucha(configuracion.puerto, BACKLOG);

	log_info(logger, "Escuchando en puerto: %s", configuracion.puerto);

	if (pthread_create(&thread_listener, NULL,
			(void*) esperar_nuevas_conexiones, &local_socket)) {
		log_error(logger, "Error creando el hilo del servidor escucha\n");
		exit(EXIT_FAILURE);
	}


	while (1) {
		sem_wait(&contador_instancias_disponibles);
		pthread_mutex_lock(&mutex_instancias_disponibles);
		t_instancia* elegida = obtener_instancia_segun_EL(lista_instancias_disponibles);
		pthread_mutex_unlock(&mutex_instancias_disponibles);
		sem_post(&contador_instancias_disponibles);//porque en realidad no la sacaste de la lista a la instancia
		log_debug(logger, "Instancia elegida Nº %d", elegida->id);

		//instancia.haceTuMagia()

	}

	if (pthread_join(thread_listener, NULL)) {
		log_error(logger, "Error al joinear thread del servidor escucha");
		exit(EXIT_FAILURE);
	}

	log_destroy(logger);
	exit(0);
}

