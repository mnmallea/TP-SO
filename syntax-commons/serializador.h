#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_

#include <stdlib.h>
#include <stddef.h>
#include <commons/log.h>
#include <string.h>
#include "../syntax-commons/my_socket.h"

typedef struct {
	void* carga;
	size_t tamanioActual;
} paquete;

extern t_log * logger;

paquete* crearPaquete();

void agregar(paquete* pqt, void* contenido, size_t tamanioContenido);

void agregarTamanioVariable(paquete* pqt, void* contenido,
		size_t tamanioContenido);

void* construirPaquete(paquete* p);

void destruirPaquete(paquete* unPaquete);

void enviarPaquete(int socket, void* paqueteSerializado, size_t tamanioPaquete);

#endif
