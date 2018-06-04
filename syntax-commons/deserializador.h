#ifndef DESERIALIZADOR_H_
#define DESERIALIZADOR_H_

#include "../syntax-commons/my_socket.h"
#include <stdlib.h>



 extern t_log * logger;

unsigned int recibirPaquete(int my_socket, void* buffer, size_t tamanio);
unsigned int recibirPaqueteVariable(int my_socket, void** buffer);

#endif
