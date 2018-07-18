/*
 * sincronizacion.c
 *
 *  Created on: 22 may. 2018
 *      Author: utnso
 */

#include "sincronizacion.h"

void inicializar_semaforos() {
	pthread_mutex_init(&mutex_instancias_disponibles, NULL);
	pthread_mutex_init(&mutex_instancias_inactivas, NULL);
	pthread_mutex_init(&mutex_operacion, NULL);
	sem_init(&contador_instancias_disponibles, 0, 0);
	sem_init(&semaforo_binario, 0, 1);
	sem_init(&planificador_respondio, 0, 0);
	sem_init(&semaforo_compactacion, 0, 0);
}

void n_waits(sem_t* semaforo, int n) {
	int i;
	for (i = 0; i < n; i++) {
		sem_wait(semaforo);
	}
}
