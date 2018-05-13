/*
 * main.h
 *
 *  Created on: 17 abr. 2018
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

#define LOG_LEVEL LOG_LEVEL_TRACE
#define BACKLOG 5

#include <stdlib.h>
#include <pthread.h>
#include "../syntax-commons/my_socket.h"
#include <commons/log.h>
#include "algoritmos_planificacion.h"
#include "config_planificador.h"
#include "selector.h"
#include "consola.h"

t_list lista_esis_listos;
t_list lista_esis_corriendo;
t_list lista_esis_bloq_consola;
t_list lista_esis_bloq_rec;
t_list lista_esis_finalizados;

typedef struct{

	//info del socket
	int socket;

	//info para calcular HRRN, SJF
	int dur_ult_raf;
	int estim_anter;
	int viene_esperando;
	char *clave_bloq;
} t_esi;



void *menu(void *ptr);



#endif /* MAIN_H_ */
