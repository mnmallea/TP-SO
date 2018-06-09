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

/*
 * si alguien agrega algo agreguelo tambien a la funcion de to_string_protocolo
 * Gracias!!
 */
typedef enum protocolo {
	EXITO,
	ERROR,
	SOLICITUD_CLAVE,
	BLOQUEO_CLAVE,
	DESBLOQUEO_CLAVE,
	BLOQUEO_ESI,
	CORRER_ESI,
	ESI_TIENE_CLAVE,
	FINALIZO_ESI,
	OP_GET,
	OP_SET,
	OP_STORE,
	MATAR_INSTANCIA,
	INSTANCIA_CAIDA_EXCEPTION,
	CLAVE_INACCESIBLE_EXCEPTION,
	CLAVE_NO_BLOQUEADA_EXCEPTION,
	CLAVE_NO_IDENTIFICADA_EXCEPTION,
	ABORTA
} t_protocolo;

char* to_string_protocolo(t_protocolo codigo);

#endif /* PROTOCOL_H_ */
