/*
 * main.c
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */
#include <stdio.h>
#include <commons/log.h>
#include "config_instancia.h"
#include "../syntax-commons/my_socket.h"
#include "../syntax-commons/conexiones.h"
#include "cfg_almacenamiento.h"
#include "almacenamiento.h"
#include "tabla_entradas.h"
#include "../syntax-commons/protocol.h"
#include "instancia.h"
#include "tabla_entradas.h"
#define LOG_LEVEL LOG_LEVEL_TRACE

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
	iniciarDumper(configuracion.punto_montaje);
	int escucha = 1;
	while (escucha) { //despues se va a transformar a while(instanciaEsteActiva())
		int resultado;
		switch (recibir_cod_operacion(socketCoordinador)) {
		case OP_SET:
			resultado = SET(socketCoordinador);
			if (resultado >= 0) {
				enviar_cod_operacion(socketCoordinador, EXITO);
				//notificarCoordExito
			} else {
				//notificarCoordFalla
				enviar_cod_operacion(socketCoordinador, ERROR);
			}
			break;
		case OP_STORE:
			char* clave;
			int rs = recibir_operacion_unaria(socketCoordinador, &clave);
			if (rs < 0) {
				log_error(logger, "Error al recibir operacion ");
				close(socketCoordinador);
				exit(EXIT_FAILURE);
			}
			log_trace(logger, "Store %s", clave);
			resultado = STORE(clave);
			if (resultado >= 0) {
				enviar_cod_operacion(socketCoordinador, EXITO);
			} else {
				enviar_cod_operacion(socketCoordinador, ERROR);
			}
			break;

		case MATAR_INSTANCIA:

			escucha = 0;

			break;
		default: 
			log_info(logger, "no se pudo interpretar el mensaje");
			//todo aca te tendrias que morir de manera copada
			exit(EXIT_FAILURE);
		

		}
	}

	limpiar_configuracion();
	log_destroy(logger);
	exit(0);
}
