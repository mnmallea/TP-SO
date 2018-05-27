/*
 * typedefs.h
 *
 *  Created on: 27 may. 2018
 *      Author: utnso
 */

#ifndef TYPEDEFS_H_
#define TYPEDEFS_H_

#define PORT_MAX_STRING_LENGTH 5

typedef struct{

	//info del socket
	int id;
	int socket;

	//info para calcular HRRN, SJF
	int dur_ult_raf;
	int estim_anter; //HAY QUE CAMBIAR ESTO A DOUBLE
	double estim_actual;

	int viene_esperando;
	double response_ratio;
	char *clave_bloq;
} t_esi;

typedef enum {FIFO, SJFsD, SJFcD, HRRN } tipo_algoritmo_planif ;

typedef struct{
	char puerto[PORT_MAX_STRING_LENGTH + 1]; //el maximo es 65535 mas el '\0'
	int algoritmo;
	int estimacion_inicial;
	char *ipCoord;
	char *portCoord;
	char *claves_bloqueadas;
	int alfa;
} config;


#endif /* TYPEDEFS_H_ */
