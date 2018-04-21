#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/log.h>


t_log * logger;

//#include <readline/readline.h> // Para usar readline
//#include <commons/collections/list.h>

void configure_logger();
int crear_socket_cliente(char *ip, char *puerto);
int crear_socket_escucha(char *puerto_escucha,int max_comm);
void esperar_mensaje(int my_socket);
void mandar_mensaje(int my_socket);
void recibir_confirmacion (int my_socket);
void mandar_confirmacion(int my_socket);
void salir_con_error(int my_socket, char* error_msg);
void exit_gracefully(int return_nr);
#endif
