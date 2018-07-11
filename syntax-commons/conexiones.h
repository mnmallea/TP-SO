/*
 * conexiones.h
 *
 *  Created on: 2 may. 2018
 *      Author: utnso
 */

#ifndef DEBUG_CONEXIONES_H_
#define DEBUG_CONEXIONES_H_

#include <commons/log.h>
#include <sys/socket.h>
#include "protocol.h"
#include "my_socket.h"
#include "serializador.h"
#include "deserializador.h"

extern t_log* logger;
extern void limpiar_configuracion(void);

/*
 *Sirve para conectarse al coordinador y realizar un handshake.
 * En el remitente debe ir quien es el que lo llama.
 * Te devuelve el socket por el que se conecto
 * Si no puede, cierra el proceso
 */
int conectarse_a_coordinador(char* ip, char* puerto, t_identidad remitente);

/*
 * Devuelve ERROR_CONEXION si fallo
 */
t_protocolo recibir_cod_operacion(int sockfd);

/*
 * Devuelve valor negativo si fallo
 */

int enviar_cod_operacion(int sockfd, t_protocolo cod_op);
int recibir_operacion_unaria(int sockfd, char** clave);
int recibir_set(int sockfd, char** clave, char** valor);

/*
 * Devuelve valor negativo si fallo
 */
int enviar_set(int sockfd, char* clave, char* valor);

/*
 * Devuelve valor negativo si fallo
 */
int enviar_get(int sockfd, char *clave);

/*
 * Devuelve valor negativo si fallo
 */
int enviar_store(int sockfd, char *clave);

/*
 * Devuelve valor negativo si fallo
 */
int enviar_operacion_unaria(int sockfd, t_protocolo cod_op, char *clave);

#endif /* DEBUG_CONEXIONES_H_ */
