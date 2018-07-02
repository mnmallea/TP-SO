/*
 * sincronizacion.h
 *
 *  Created on: 29 may. 2018
 *      Author: utnso
 */

#ifndef SINCRONIZACION_H_
#define SINCRONIZACION_H_

#include <commons/log.h>
#include <pthread.h>
#include <semaphore.h>

extern t_log* logger;

pthread_mutex_t mutex_flag_pausa_despausa; //PAUSAR Y DESPAUSAR CONSOLA
pthread_mutex_t mutex_var_comp_id; //Variable compartida id
pthread_mutex_t mutex_lista_esis_listos;
pthread_mutex_t mutex_lista_esis_finalizados;
pthread_mutex_t mutex_dic_clave_x_esi;
pthread_mutex_t mutex_pausa;

sem_t pausa_planificacion;
sem_t coordinador_respondio_paq;
sem_t contador_esis;
sem_t respondio_esi_corriendo;

void inicializar_semaforos();

#endif /* SINCRONIZACION_H_ */
