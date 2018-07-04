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
	LINEA_INVALIDA,
	CLAVE_SIZE,
	INTERPRETAR,
	ABORTA,
	RELEVANTAR_INSTANCIA,
	SOLICITUD_STATUS_CLAVE,
	RESPUESTA_STATUS_CLAVE,
	SOLICITUD_VALOR,
	VALOR_ENCONTRADO,
	VALOR_NO_ENCONTRADO,
	INSTANCIA_COMPACTAR,
	ERROR_CONEXION
} t_protocolo;

typedef enum status_clave{
	HAY_VALOR,
	NO_HAY_VALOR,
	INSTANCIA_OK,
	INSTANCIA_CAIDA,
	INSTANCIA_NO_ASIGNADA,
	INSTANCIA_NO_TIENE_CLAVE,
	HAY_SIMULACION,
	NO_HAY_SIMULACION

} t_status_clave;

char* to_string_protocolo(t_protocolo codigo);
char* to_string_status_clave(t_status_clave codigo);

#endif /* PROTOCOL_H_ */
