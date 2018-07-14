/*
 * cfg_almacenamiento2.c
 *
 *  Created on: 31 may. 2018
 *      Author: utnso
 */
#include "cfg_almacenamiento.h"

void configurarAlmacenamiento(int socketCoordinador) {

	t_paquete* paquete = paquete_crear();
	paquete_agregar(paquete, configuracion.nombre_instancia,
			strlen(configuracion.nombre_instancia) + 1);
	paquete_enviar_safe(paquete, socketCoordinador);
	paquete_destruir(paquete);

	recibirPaquete(socketCoordinador, &cfgAlmacenamiento.tamanioEntrada,
			sizeof(cfgAlmacenamiento.tamanioEntrada));
	recibirPaquete(socketCoordinador, &cfgAlmacenamiento.totalEntradas,
			sizeof(cfgAlmacenamiento.totalEntradas));
	log_trace(logger, "se asignaron (%d) cantidad de entradas de (%d) tamanio para el almacenamiento",cfgAlmacenamiento.totalEntradas,cfgAlmacenamiento.tamanioEntrada);

}
unsigned int obtenerEntradasTotales() {
	return cfgAlmacenamiento.totalEntradas;
}

unsigned int obtenerTamanioEntrada() {
	return cfgAlmacenamiento.tamanioEntrada;
}

