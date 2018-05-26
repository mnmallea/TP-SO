/*
 * sincronizacion.h
 *
 *  Created on: 26 may. 2018
 *      Author: utnso
 */

#ifndef SINCRONIZACION_H_
#define SINCRONIZACION_H_

#include <pthread.h>
#include <semaphore.h>

pthread_mutex_t mutex_flag_pausa_despausa;

void inicializar_semaforos();

#endif /* SINCRONIZACION_H_ */
