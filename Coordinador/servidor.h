/*
 * servidor.h
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */

#ifndef SERVIDOR_H_
#define SERVIDOR_H_

#include <errno.h>
#include <commons/log.h>
#include <pthread.h>
#include "../syntax-commons/protocol.h"
#include "../syntax-commons/my_socket.h"

extern t_log* logger;


void esperar_nuevas_conexiones(int sockfd);
void atender_nueva_conexion(void* sockfd_ptr);


#endif /* SERVIDOR_H_ */
