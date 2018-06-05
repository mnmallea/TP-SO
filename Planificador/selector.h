
#ifndef SELECTOR_H_
#define SELECTOR_H_

#include <commons/collections/list.h>
#include <commons/log.h>
#include "../syntax-commons/my_socket.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "config_planificador.h"
#include "algoritmos_planificacion.h"
#include "../syntax-commons/protocol.h"
#include "../syntax-commons/conexiones.h"

#define LOG_LEVEL LOG_LEVEL_TRACE
#define BACKLOG 5


fd_set master;
fd_set read_fds;
int fdmax;
int socketServer;
int newfd;
socklen_t addrlen;
int i,j;
int socketAEliminar;
struct sockaddr_storage remoteaddr;

extern t_list *lista_esis_listos;
extern t_esi esi;
extern config configuracion;


void *listener(void *ptr);
t_esi *crear_nodo_esi(int socket);
int socketProceso(t_esi *n_esi);


#endif /* SELECTOR_H_ */

