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
#include <semaphore.h>
#include <commons/collections/list.h>
#include "../syntax-commons/protocol.h"
#include "../syntax-commons/my_socket.h"
#include "algoritmos_distribucion.h"
#include "config_coordinador.h"
#include "sincronizacion.h"
#include "../syntax-commons/serializador.h"
#include "../syntax-commons/deserializador.h"

extern t_log* logger;
extern t_list *lista_instancias_disponibles;
extern config configuracion;
extern t_list *lista_esis_disponibles;
pthread_t thread_ESI;
pthread_t thread_Planificador;

unsigned cant_instancias;

/**
 * @NAME: esperar_nuevas_conexiones
 * @DESC: Sirve para que el coordinador atienda a los que se conectan tirando hilos
 */
void esperar_nuevas_conexiones(int* sockfd);
void atender_nueva_conexion(int* sockfd_ptr);

void atender_instancia(int sockfd);
void atender_esi(int socket);
void atender_planif(int socket);

#endif /* SERVIDOR_H_ */
