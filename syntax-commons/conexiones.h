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

int conectarse_a_coordinador(char* ip, char* puerto, t_identidad remitente);

t_protocolo recibir_cod_operacion(int sockfd);
int recibir_operacion_unaria(int sockfd, char** clave);
int recibir_set(int sockfd, char** clave, char** valor);
int enviar_set(int sockfd, char* clave, char* valor);
int enviar_get(int sockfd,char *clave);
int enviar_store(int sockfd,char *clave);
int enviar_operacion_unaria(int sockfd, t_protocolo cod_op, char *clave);

#endif /* DEBUG_CONEXIONES_H_ */
