/*
 * protocol.h
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

typedef enum identidad {
	ESI, INSTANCIA, PLANIFICADOR, COORDINADOR
} t_identidad;

typedef enum operacion{
	GET, SET, STORE
} t_cod_operacion;

#endif /* PROTOCOL_H_ */
