#ifndef DESERIALIZADOR_H_
#define DESERIALIZADOR_H_
#include <my_socket.h>
#include <stdlib.h>



t_log * logger;

void configure_logger();

int recibirpaquete(int my_socket, void* rcp, size_t tamanio);

int recibirPaqueteVariable(int my_socket, void** rcp);

#endif
