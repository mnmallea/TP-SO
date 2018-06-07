/*
 * main.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */
#include "main.h"

int main(int argc, char **argv) { //aca recibiriamos la ruta del archivo de configuracion como parametro
	crear_log_operaciones();
	logger = log_create("coordinador.log", "Coordinador", true, LOG_LEVEL);
	configurar(argv[1]);
	log_trace(logger, "Coordinador correctamente configurado");
	lista_instancias_disponibles = list_create();
	lista_instancias_inactivas = list_create();
	lista_esis_disponibles = list_create();
	cant_instancias = 0;
	inicializar_semaforos();
	int local_socket = crear_socket_escucha(configuracion.puerto, BACKLOG);

	log_info(logger, "Escuchando en puerto: %s", configuracion.puerto);

	if (pthread_create(&thread_listener, NULL,
			(void*) esperar_nuevas_conexiones, &local_socket)) {
		log_error(logger, "Error creando el hilo del servidor escucha\n");
		exit(EXIT_FAILURE);
	}


//	while (1) {
//		sem_wait(&contador_instancias_disponibles);
//		t_instancia* elegida = obtener_instancia_siguiente("");
//		sem_post(&contador_instancias_disponibles);//porque en realidad no la sacaste de la lista a la instancia
//		log_debug(logger, "Instancia elegida: %s", elegida->nombre);
//
//		//instancia.haceTuMagia>>>>>>> 9e71288354523bf2f56d8810ef0f85ee3959260c()
//
//	}

	if (pthread_join(thread_listener, NULL)) {
		log_error(logger, "Error al joinear thread del servidor escucha");
		exit(EXIT_FAILURE);
	}

	destruir_log_operaciones();
	log_destroy(logger);
	exit(EXIT_SUCCESS);
}

