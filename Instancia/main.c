/*
 * main.c
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */
#include <stdlib.h>
#include <commons/log.h>
#include "config_instancia.h"
#include "../syntax-commons/my_socket.h"
#include "../syntax-commons/conexiones.h"
#include "cfg_almacenamiento.h"
#include "almacenamiento.h"
#include "tabla_entradas.h"
#define LOG_LEVEL LOG_LEVEL_DEBUG

config configuracion;
t_log *logger;

int main(int argc, char** argv) {
	logger = log_create("instancia.log", "Instancia", true, LOG_LEVEL);
	configuracion = configurar(argv[1]);
	int socketCoordinador = conectarse_a_coordinador(
			configuracion.ip_coordinador, configuracion.puerto_coordinador,
			INSTANCIA);
	configurarAlmacenamiento(socketCoordinador);
	log_trace(logger,
			"Se va a inicializar el almacenamiento con: cant_entradas = %d y tamaño = %d",
			cfgAlmacenamiento.totalEntradas, cfgAlmacenamiento.tamanioEntrada);
	inicializarAlmacenamiento(cfgAlmacenamiento.totalEntradas,
			cfgAlmacenamiento.tamanioEntrada);
	crearTablaEntradas();
	while (1) {
		//aca toda la bola de que lleguen cosas las reciba haga set get store
	}
	limpiar_configuracion();
	log_destroy(logger);
	exit(0);
}
