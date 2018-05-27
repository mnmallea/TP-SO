/*
 * sincronizacion.c
 *
 *  Created on: 26 may. 2018
 *      Author: utnso
 */

#include "sincronizacion.h"

void inicializar_semaforos(){
	pthread_mutex_init(&mutex_flag_pausa_despausa, NULL);
	pthread_mutex_init(&mutex_lista_esis_listos, NULL);
}

