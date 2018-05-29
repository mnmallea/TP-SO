/*
 * sincronizacion.h
 *
 *  Created on: 29 may. 2018
 *      Author: utnso
 */

#ifndef SINCRONIZACION_H_
#define SINCRONIZACION_H_

#include <pthread.h>
#include <semaphore.h>

pthread_mutex_t mutex_flag_pausa_despausa; //PAUSAR Y DESPAUSAR CONSOLA
pthread_mutex_t mutex_lista_esis_listos; //PRODUCTOR CONSUMIDOR QUE AGREGa/SACA ESIS

void inicializar_semaforos();

#endif /* SINCRONIZACION_H_ */
