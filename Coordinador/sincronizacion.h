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
sem_t contador_instancias_disponibles;

void inicializar_semaforos();

#endif /* SINCRONIZACION_H_ */
