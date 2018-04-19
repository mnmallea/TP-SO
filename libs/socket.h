#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>

//Conecto 
int crear_socket_cliente(char *ip, char *puerto);

//Devuelve socket.
int crear_socket_escucha(char* puerto_escucha, int max_comm);

#endif