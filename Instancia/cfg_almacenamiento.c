/*
 * cfg_almacenamiento2.c
 *
 *  Created on: 31 may. 2018
 *      Author: utnso
 */
#include "cfg_almacenamiento.h"

void configurarAlmacenamiento(int socketCoordinador) {
//	uint32_t size_nombre = strlen(configuracion.nombre_instancia) + 1;
//	int tamanio_a_enviar = sizeof(size_nombre) + size_nombre;
//	void* buffer = calloc(1, tamanio_a_enviar);
//	memcpy(buffer, &size_nombre, sizeof(size_nombre));
//	memcpy(buffer + sizeof(size_nombre), configuracion.nombre_instancia,
//			size_nombre);
//	safe_send(socketCoordinador, buffer, tamanio_a_enviar);
//	free(buffer);
	t_paquete* paquete = paquete_crear();
	paquete_agregar(paquete, configuracion.nombre_instancia,
			strlen(configuracion.nombre_instancia) + 1);
	paquete_enviar_safe(paquete, socketCoordinador);
	paquete_destruir(paquete);

	recibirPaquete(socketCoordinador, &cfgAlmacenamiento.tamanioEntrada,
			sizeof(cfgAlmacenamiento.tamanioEntrada));
	recibirPaquete(socketCoordinador, &cfgAlmacenamiento.totalEntradas,
			sizeof(cfgAlmacenamiento.totalEntradas));
	//mostrar que se asigno una cantidad de  x entradas de x tamanio para el ato

}
unsigned int obtenerEntradasTotales() {
	return cfgAlmacenamiento.totalEntradas;
}

unsigned int obtenerTamanioEntrada() {
	return cfgAlmacenamiento.tamanioEntrada;
}

