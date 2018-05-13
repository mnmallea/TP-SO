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


typedef struct{

	//info del socket
	char *ip;
	char *puerto;

	//info para calcular HRRN, SJF
	int dur_ult_raf;
	int estim_anter;
	int viene_esperando;
} t_esi;



void *menu(void *ptr);



#endif /* MAIN_H_ */
