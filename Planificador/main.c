/*
 * main.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#include "main.h"

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <stdbool.h>

#include "algoritmos_planificacion.h"

int main(int argc, char **argv) {

	lista_esis_listos = list_create();
	esi_corriendo = (t_esi *) malloc(sizeof(t_esi));
	lista_esis_finalizados = list_create();
	dic_esis_bloqueados = dictionary_create();
	dic_clave_x_esi = dictionary_create();
	lista_deadlock = list_create();


	/*Config*/
	logger = log_create("planificador.log", "Planificador", true, LOG_LEVEL);
	logger->is_active_console = 0; //para ver la consola tail -200f planificador.log en otra ventana y se ve en tiempo real
	configuracion = configurar(argv[1]);
	/*Creacion de hilos*/
	pthread_t selector_planificador;
	pthread_t consola_planificador;

	const char *message0 = "Inicializacion del planificador";
	if (pthread_create(&selector_planificador, NULL, listener,
			(void*) message0)) {
		log_error(logger, "Cantidad incorrecta de parametros");
		exit(EXIT_FAILURE);
	}

	const char *message1 = "Inicializacion de la consola";
	if (pthread_create(&consola_planificador, NULL, menu, (void*) message1)) {
		log_error(logger, "Error creando el hilo de la consola\n");
		exit(EXIT_FAILURE);
	}

	/*Join threads*/
	if (pthread_join(selector_planificador, NULL)) {
		log_error(logger, "Error al joinear el hilo del selector\n");
		exit(EXIT_FAILURE);
	}

	if (pthread_join(consola_planificador, NULL)) {
		log_error(logger, "Error al joinear el hilo de la consola\n");
		exit(EXIT_FAILURE);
	}

	return 0;
}


void configurar_claves_inicialmente_bloqueadas(){

	t_esi *esi_trucho = malloc(sizeof(esi));
	esi_trucho->socket = -1;
	esi_trucho->estim_anter = 0;
	esi_trucho->dur_ult_raf = 0;
	esi_trucho->viene_esperando = 0;
	esi_trucho->id = -1;

	int i = 0;
	while(configuracion.claves_bloqueadas[i] != NULL){

		dictionary_put(dic_clave_x_esi, configuracion.claves_bloqueadas[i], esi_trucho);
		i++;
	}
	
	free(esi_trucho);

}
