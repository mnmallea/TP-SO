#ifndef MY_SOCKET_H_
#define MY_SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <commons/log.h>
#include <errno.h>


t_log * logger;

//#include <readline/readline.h> // Para usar readline
//#include <commons/collections/list.h>

void configure_logger();
int crear_socket_cliente(char *ip, char *puerto);
int crear_socket_escucha(char *puerto_escucha, int max_comm);
void esperar_mensaje(int my_socket); 
int recibir_mensaje(int my_socket);
void mandar_mensaje(int my_socket,int id);
void recibir_confirmacion (int my_socket); //wait
/*
 * Devuelve valor negativo si fallo
 */
int mandar_confirmacion(int my_socket);   //signal
void mandar_error(int my_socket);
void salir_con_error(int my_socket, char* error_msg);
void exit_gracefully(int return_nr);


void* safe_recv(int my_socket, int msg_len);
void safe_send(int my_socket, void* msg, int msg_len);

#endif
