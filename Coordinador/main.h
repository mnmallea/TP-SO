/*
 * main.h
 *
 *  Created on: 3 may. 2018
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_
#include <commons/collections/list.h>

typedef struct {

	//info del socket
	char* ip;
	char* puerto;

	//info gral
	int cant_entradas;
	int tam_total;


	//para lsu
	int cant_entradas_vacias;

	//para ke
	char primer_letra;
	char ult_letra;


} instancia;

t_list lista_instancias_disponibles;

#endif /* MAIN_H_ */
