
#ifndef SELECTOR_H_
#define SELECTOR_H_

#include <commons/collections/list.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define LOG_LEVEL LOG_LEVEL_TRACE
#define BACKLOG 5

t_list lista_esis_listos;
t_list lista_esis_corriendo;
t_list lista_esis_bloq_consola;
t_list lista_esis_bloq_rec;
t_list lista_esis_finalizados;

fd_set master;
fd_set read_fds;
int fdmax;
int listener;
int newfd;
socklen_t addrlen;
int i,j,rv;
struct sockaddr_storage remoteaddr;

FD_ZERO(&master);
FD_ZERO(&read_fds);

int selector();


#endif /* SELECTOR_H_ */

