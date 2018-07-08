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
pthread_mutex_t mutex_dic_esis_bloqueados;
pthread_mutex_t mutex_esi_corriendo;
pthread_mutex_t mutex_esi_a_matar_por_consola;
pthread_mutex_t mutex_esi_a_bloquear_por_consola;
pthread_mutex_t mutex_clave_a_bloquear;


sem_t pausa_planificacion;
sem_t coordinador_respondio_paq;
sem_t contador_esis;
sem_t respondio_esi_corriendo;
sem_t operacion_coordinador;

void inicializar_semaforos();

#endif /* SINCRONIZACION_H_ */
