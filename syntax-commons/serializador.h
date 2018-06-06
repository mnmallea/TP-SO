#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_

#include <stdlib.h>
#include <stddef.h>
#include <commons/log.h>
#include <string.h>
#include "../syntax-commons/my_socket.h"

typedef struct paquete {
	int tamanioActual;
	void* carga;
} t_paquete;

extern void limpiar_configuracion();
extern t_log * logger;

t_paquete* paquete_crear();
void paquete_destruir(t_paquete* unPaquete);
void paquete_agregar(t_paquete* pqt, void* contenido, uint32_t tamanioContenido);
int paquete_enviar(t_paquete* paquete, int socket);
void paquete_enviar_safe(t_paquete* paquete, int socket);


void agregarTamanioVariable(t_paquete* pqt, void* contenido,
		size_t tamanioContenido);

void* construirPaquete(t_paquete* p);

void enviarPaquete(int socket, void* paqueteSerializado, size_t tamanioPaquete);

#endif
