/*
 * typedefs.h
 *
 *  Created on: 22 may. 2018
 *      Author: utnso
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#define PORT_MAX_STRING_LENGTH 5
#include <commons/collections/list.h>

typedef enum {
	LSU, EL, KE
} t_algoritmo;

typedef struct {
	//info del socket
	int socket;
	unsigned id;
	//para lsu
	int cant_entradas_vacias;

	t_list* claves_almacenadas;
} t_instancia;


typedef struct{
	char puerto[PORT_MAX_STRING_LENGTH + 1]; //el maximo es 65535 mas el '\0'
	t_algoritmo algoritmo;
	int cant_entradas;
	int entrada_size;
	int retardo;
} config;

typedef struct //para recibir al esi
{
int keyword;
char *clave;
char *valor;
}t_operacion;

typedef struct //para guardar al esi
{
int *id;
int socket;
t_operacion *valores;
}t_esi;



#endif /* TYPEDEFS_H_ */
