#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_

#include <stdlib.h>
#include <stddef.h>
#include <commons/log.h>
#include <string.h>
#include "../syntax-commons/my_socket.h"
#include "../syntax-commons/protocol.h"

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
/*
 * Envia un paquete antecedido del codigo
 * Devuelve -1 si hay error
 */
int paquete_enviar_con_codigo(t_paquete* paquete, t_protocolo codigo, int sockfd);

void enviarPaquete(int socket, void* paqueteSerializado, size_t tamanioPaquete);

#endif
