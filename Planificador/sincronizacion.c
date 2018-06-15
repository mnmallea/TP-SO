/*
 * sincronizacion.c
 *
 *  Created on: 29 may. 2018
 *      Author: utnso
 */



#include "sincronizacion.h"

void inicializar_semaforos(){
	pthread_mutex_init(&mutex_flag_pausa_despausa, NULL);
	pthread_mutex_init(&mutex_var_comp_id, NULL);
	sem_init(&sem_binario_planif, 0, 1);
	sem_init(&contador_esis, 0,1);
}
