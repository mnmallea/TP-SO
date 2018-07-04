/*
 * typedefs.h
 *
 *  Created on: 2 jul. 2018
 *      Author: utnso
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#include <stdbool.h>

#include "../syntax-commons/protocol.h"


#define PORT_MAX_STRING_LENGTH 5

typedef enum {
	FIFO, SJFsD, SJFcD, HRRN
} tipo_algoritmo_planif;

typedef struct {

	//info del socket
	int id;
	int socket;

	//info para calcular HRRN, SJF
	int dur_ult_raf; //Se suma uno cada vez que si soy esi corriendo finalizo una rafaga
	double estim_anter;
	double estim_actual;

	int viene_esperando; //Se suma uno cada vez que un esi que no es el finaliza una linea
	double response_ratio;
} t_esi;

typedef struct{
	char puerto[PORT_MAX_STRING_LENGTH + 1]; //el maximo es 65535 mas el '\0'
	int algoritmo;
	int estimacion_inicial;
	char *ipCoord;
	char *portCoord;
	char **claves_bloqueadas;
	double alfa;
} config;

typedef struct{
	bool hay_valor;
	char* valor;
	t_status_clave estado_instancia;
	bool hay_instancia;
	char* instancia;
	bool hay_simulacion;
	char* instancia_simulacion;
} respuesta_status_clave_t;

#endif /* TYPEDEFS_H_ */
