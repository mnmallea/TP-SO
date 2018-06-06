#include "serializador.h"

paquete* crearPaquete() {
	paquete* nuevoPaquete = malloc(sizeof(paquete));
	nuevoPaquete->tamanioActual = 0;
	nuevoPaquete->carga = malloc(0);
	return nuevoPaquete;
}

void destruirPaquete(paquete* unPaquete) {
	if (unPaquete != NULL) {
		free(unPaquete->carga);
		free(unPaquete);
	}
}

void agregar(paquete* pqt, void* contenido, size_t tamanioContenido) {
	pqt->carga = realloc(pqt->carga, pqt->tamanioActual + tamanioContenido);
	memcpy(pqt->carga + pqt->tamanioActual, contenido, tamanioContenido);
	pqt->tamanioActual += tamanioContenido;
	log_error(logger, "paquete de tamanio fijo agregado");
}

void agregarTamanioVariable(paquete* pqt, void* contenido,
		size_t tamanioContenido) {
	agregar(pqt, &tamanioContenido, sizeof(tamanioContenido));
	agregar(pqt, contenido, tamanioContenido);
	log_error(logger, " paquete tamanio variable agregado");
}

void* construirPaquete(paquete* pqt) {
	void* paqueteProcesado = malloc(pqt->tamanioActual);
	if (paqueteProcesado == NULL) {
		log_error(logger, "ERROR EN EL MALLOC");
	}
	memcpy(paqueteProcesado, pqt->carga, pqt->tamanioActual);

	return paqueteProcesado;
}

void enviarPaquete(int socket, void* paqueteSerializado, size_t tamanioPaquete) {
	int cantEnviada = send(socket, paqueteSerializado, tamanioPaquete, 0);

	if (tamanioPaquete > cantEnviada) {
		enviarPaquete(socket, paqueteSerializado + cantEnviada,
				tamanioPaquete - cantEnviada);
	}
}

