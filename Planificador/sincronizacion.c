/*
 * sincronizacion.c
 *
 *  Created on: 29 may. 2018
 *      Author: utnso
 */

#include "sincronizacion.h"

#include <errno.h>
#include <sched.h>
#include <stdio.h>

void inicializar_semaforos() {
	log_trace(logger, "Inicializando semaforos...");
	pthread_mutex_init(&mutex_flag_pausa_despausa, NULL);
	pthread_mutex_init(&mutex_var_comp_id, NULL);
	pthread_mutex_init(&mutex_lista_esis_listos, NULL);
	pthread_mutex_init(&mutex_pausa, NULL);

	sem_init(&pausa_planificacion, 0, 0);
	sem_init(&coordinador_respondio_paq, 0, 1);
	sem_init(&contador_esis, 0, 0);
	sem_init(&respondio_esi_corriendo, 0, 0);
}
