#include "serializador.h"

t_paquete* paquete_crear() {
	t_paquete* nuevoPaquete = malloc(sizeof(t_paquete));
	nuevoPaquete->tamanioActual = 0;
	nuevoPaquete->carga = NULL;
	return nuevoPaquete;
}

void paquete_destruir(t_paquete* unPaquete) {
	if (unPaquete != NULL) {
		free(unPaquete->carga);
		free(unPaquete);
	}
}

void paquete_agregar_sin_tamanio(t_paquete* paquete, void* contenido, int size){
	paquete->carga = realloc(paquete->carga, paquete->tamanioActual + size);
	memcpy(paquete->carga + paquete->tamanioActual, contenido, size);
	paquete->tamanioActual += size;
}

void paquete_agregar(t_paquete* pqt, void* contenido, uint32_t tamanioContenido) {
	int tamanio_a_agregar = sizeof(tamanioContenido) + tamanioContenido;
	pqt->carga = realloc(pqt->carga, pqt->tamanioActual + tamanio_a_agregar);
	memcpy(pqt->carga + pqt->tamanioActual, &tamanioContenido,
			sizeof(tamanioContenido));
	memcpy(pqt->carga + pqt->tamanioActual + sizeof(tamanioContenido),
			contenido, tamanioContenido);
	pqt->tamanioActual += tamanio_a_agregar;
}

int paquete_enviar(t_paquete* paquete, int socket) {
	int res_send;
	if ((res_send = send(socket, paquete->carga, paquete->tamanioActual, MSG_NOSIGNAL))
			< 0) {
		log_error(logger, "Error al enviar paquete: %s", strerror(errno));
	}
	return res_send;
}

int paquete_enviar_con_codigo(t_paquete* paquete, t_protocolo codigo,
		int sockfd) {
	if (send(sockfd, &codigo, sizeof(codigo), MSG_NOSIGNAL) <= 0) {
		log_error(logger, "Error al enviar codigo paquete");
		return -1;
	}
	if (paquete_enviar(paquete, sockfd) <= 0) {
		log_error(logger, "Error al enviar paquete");
		return -1;
	}
	return 0;
}

void paquete_enviar_safe(t_paquete* paquete, int socket) {
	if (paquete_enviar(paquete, socket) < 0) {
		close(socket);
		limpiar_configuracion();
		exit(EXIT_FAILURE);
	}
}

void enviarPaquete(int socket, void* paqueteSerializado, size_t tamanioPaquete) {
	int cantEnviada = send(socket, paqueteSerializado, tamanioPaquete, MSG_NOSIGNAL);

	if (tamanioPaquete > cantEnviada) {
		enviarPaquete(socket, paqueteSerializado + cantEnviada,
				tamanioPaquete - cantEnviada);
	}
}

