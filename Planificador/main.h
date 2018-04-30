/*
 * main.h
 *
 *  Created on: 17 abr. 2018
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <pthread.h>
#include "../syntax-commons/my_socket.h"
#include <commons/log.h>
#include <commons/collections/list.h>



typedef struct{

	/*informacion del socket*/
	int dur_ult_raf;
	int estim_anter;
	int viene_esperando;
} t_esi;


void *menu(void *ptr);


void obtener_proximo_segun_fifo(t_list* lista_esis_actual, t_list* lista_esis_nueva);
void obtener_proximo_segun_sjfsd(t_list* lista_esis_actual, t_list* lista_esis_nueva);
void obtener_proximo_segun_sjfcd(t_list* lista_esis_actual, t_list* lista_esis_nueva);
void obtener_proximo_segun_hrrn(t_list* lista_esis_actual, t_list* lista_esis_nueva);


#endif /* MAIN_H_ */
