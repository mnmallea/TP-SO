
#ifndef SELECTOR_H_
#define SELECTOR_H_

#include <arpa/inet.h>
#include <commons/collections/list.h>
#include <stdbool.h>
#include <sys/select.h>
#include "funciones_auxiliares_consola.h"

#include "typedefs.h"

#define LOG_LEVEL LOG_LEVEL_TRACE
#define BACKLOG 5

extern t_list *lista_esis_listos;
extern t_esi* esi_corriendo;
extern t_esi esi;
extern config configuracion;
extern respuesta_status_clave_t respuesta_status_clave;

fd_set master;
fd_set read_fds;

bool la_tiene;
char** buffer;

int socketCord;
int fdmax;
int socketServer;
int newfd;
int i,j;
int socketAEliminar;

socklen_t addrlen;

struct sockaddr_storage remoteaddr;

void listener(void);
t_esi *crear_nodo_esi(int socket);
int socketProceso(t_esi *n_esi);
respuesta_status_clave_t recibir_status_clave();
void atender_nueva_conexion();
void atender_error(int nbytes);
int encontrarIdDelSocket(int i);


#endif /* SELECTOR_H_ */

