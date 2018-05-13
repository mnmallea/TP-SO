
#ifndef SELECTOR_H_
#define SELECTOR_H_

#include <commons/collections/list.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define LOG_LEVEL LOG_LEVEL_TRACE
#define BACKLOG 5


fd_set master;
fd_set read_fds;
int fdmax;
int listener;
int newfd;
socklen_t addrlen;
int i,j;
struct sockaddr_storage remoteaddr;



int selector();


#endif /* SELECTOR_H_ */

