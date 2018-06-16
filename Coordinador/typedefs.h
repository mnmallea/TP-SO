/*
 * typedefs.h
 *
 *  Created on: 22 may. 2018
 *      Author: utnso
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <semaphore.h>

#define PORT_MAX_STRING_LENGTH 5
#include <commons/collections/list.h>

typedef enum {
	LSU, EL, KE
} t_algoritmo;

typedef struct {
	//info del socket
	int socket;
	char* nombre;
	//para lsu
	int cant_entradas_vacias;
	sem_t semaforo_instancia;
	t_list* claves_almacenadas;
} t_instancia;

typedef struct {
	char puerto[PORT_MAX_STRING_LENGTH + 1]; //el maximo es 65535 mas el '\0'
	t_algoritmo algoritmo;
	int cant_entradas;
	int entrada_size;
	int retardo;
} config;

typedef struct //para guardar al esi
{
	int id;
	int socket;
} t_esi;

#endif /* TYPEDEFS_H_ */
