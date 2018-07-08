/*
 * main.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#include "main.h"

int main(int argc, char **argv) {

	/*Config*/
	logger = log_create("planificador.log", "Planificador", false, LOG_LEVEL);
	//para ver la consola tail -200f planificador.log en otra ventana y se ve en tiempo real
	configuracion = configurar(argv[1]);
	inicializar_semaforos();
	planificacion_pausada = true;

	dic_clave_x_esi = dictionary_create();
	esi_corriendo=NULL; //al principio debe empezar nulo

	configurar_claves_inicialmente_bloqueadas();

	log_trace(logger, "Se ha terminado la inicializacion");
	/*Creacion de hilos*/
	pthread_t consola_planificador;
	pthread_t planificador;

	if (pthread_create(&consola_planificador, NULL, menu, NULL)) {
		log_error(logger, "Error creando el hilo de la consola\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_create(&planificador, NULL, planificar, NULL)) {
		log_error(logger, "Error creando el hilo del planificador\n");
		exit(EXIT_FAILURE);
	}

	listener();

	/*Join threads*/
	if (pthread_join(consola_planificador, NULL)) {
		log_error(logger, "Error al joinear el hilo de la consola\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_join(planificador, NULL)) {
		log_error(logger, "Error al joinear el hilo del planificador\n");
		exit(EXIT_FAILURE);
	}
	return 0;
}

void configurar_claves_inicialmente_bloqueadas() {

	t_esi *esi_trucho = malloc(sizeof(esi));
	esi_trucho->socket = -1;
	esi_trucho->estim_anter = -666;
	esi_trucho->dur_ult_raf = 0;
	esi_trucho->viene_esperando = 0;
	esi_trucho->id = -1;

	int i = 0;
	while (configuracion.claves_bloqueadas[i] != NULL) {

		dictionary_put(dic_clave_x_esi, configuracion.claves_bloqueadas[i],
				esi_trucho);
		i++;
	}

}
