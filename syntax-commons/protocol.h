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

typedef enum protocolo {
	EXITO,
	ERROR,
	SOLICITUD_CLAVE,
	BLOQUEO_CLAVE,
	DESBLOQUEO_CLAVE,
	BLOQUEO_ESI,
	CORRER_ESI,
	FINALIZO_ESI,
	OP_GET,
	OP_SET,
	OP_STORE
} t_protocolo;

#endif /* PROTOCOL_H_ */
