/*
 * sincronizacion.c
 *
 *  Created on: 22 may. 2018
 *      Author: utnso
 */

#include "sincronizacion.h"

void inicializar_semaforos(){
	pthread_mutex_init(&mutex_instancias_disponibles, NULL);
	pthread_mutex_init(&mutex_esi_disponibles, NULL);
	sem_init(&contador_instancias_disponibles, 0, 0);
	sem_init(&semaforo_binario, 0, 1);
}
