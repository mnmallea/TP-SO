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

extern t_log* logger;
extern void limpiar_configuracion(void);

/*
 * Sirve para conectarse al coordinador y realizar un handshake.
 * En el remitente debe ir quien es el que lo llama
 * Si falla, finaliza el proceso.
 * Retorna el socket por el que se conecto
 */
int conectarse_a_coordinador(char* ip, char* puerto, t_identidad remitente);

#endif /* DEBUG_CONEXIONES_H_ */
