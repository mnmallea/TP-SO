#ifndef DESERIALIZADOR_H_
#define DESERIALIZADOR_H_

#include "../syntax-commons/my_socket.h"
#include <stdlib.h>



t_log * logger;

void configure_logger_deser();

int recibirpaquete(int my_socket, void* buffer, size_t tamanio);

int recibirPaqueteVariable(int my_socket, void** buffer);

#endif
