/*
 * sincronizacion.h
 *
 *  Created on: 22 may. 2018
 *      Author: utnso
 */

#ifndef SINCRONIZACION_H_
#define SINCRONIZACION_H_

#include <pthread.h>
#include <semaphore.h>

sem_t semaforo_binario;
pthread_mutex_t mutex_instancias_disponibles;
pthread_mutex_t mutex_instancias_inactivas;
pthread_mutex_t mutex_operacion;
sem_t contador_instancias_disponibles;

sem_t planificador_respondio;
sem_t semaforo_compactacion;

void inicializar_semaforos();
void n_waits(sem_t* semaforo, int n);
void destruir_semaforos();

#endif /* SINCRONIZACION_H_ */
