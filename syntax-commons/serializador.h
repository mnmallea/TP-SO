#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_

#include <stdlib.h>
#include <stddef.h>
#include <commons/log.h>
#include <string.h>

typedef struct {
	void* carga;
	size_t tamanioActual;
} paquete;

t_log * logger;

paquete* crearPaquete();

void configure_logger_ser();

void agregar(paquete* pqt, void* contenido, size_t tamanioContenido);

void agregarTamanioVariable(paquete* pqt, void* contenido,
		size_t tamanioContenido);

void* construirPaquete(paquete* p);

#endif
