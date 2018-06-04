#include "deserializador.h"


unsigned int recibirPaquete(int my_socket, void* buffer, size_t tamanio){
	unsigned int res_recv = recv(my_socket, buffer, tamanio, MSG_WAITALL);

	if(res_recv < 0) {
		salir_con_error(my_socket,"Error al recibir la información");
	}

	log_info(logger, "Mensaje recibido");
	return res_recv;
}

unsigned int recibirPaqueteVariable(int my_socket, void** buffer) {
	unsigned int res_recv;
	size_t tamanio;

	res_recv = recibirPaquete(my_socket, &tamanio, sizeof(tamanio));

	if(res_recv < 0) {
		salir_con_error(my_socket,"Error al recibir el tamanio de la información");
	}

	if((*buffer=malloc(tamanio)) == NULL) {
		salir_con_error(my_socket,"Error al realizar el malloc");
	}
	res_recv = recibirPaquete(my_socket, *buffer, tamanio);
	return res_recv;
}
