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
#include "deserializador.h"

extern t_log* logger;
extern void limpiar_configuracion(void);

int conectarse_a_coordinador(char* ip, char* puerto, t_identidad remitente);

void enviar_operacion_unaria(int sockfd, t_cod_operacion cod_op, char *clave);

void recibir_operacion_unaria(int sockfd, char** clave);

t_cod_operacion recibir_cod_operacion(int sockfd);

void recibir_set(int sockfd, char** clave, char** valor);
void enviar_set(int sockfd, char* clave, char* valor);

#endif /* DEBUG_CONEXIONES_H_ */
