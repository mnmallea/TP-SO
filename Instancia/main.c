/*
 * main.c
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */

#include "main.h"

#include <commons/collections/list.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../syntax-commons/conexiones.h"
#include "../syntax-commons/deserializador.h"
#include "../syntax-commons/my_socket.h"
#include "../syntax-commons/protocol.h"
#include "almacenamiento.h"
#include "cfg_almacenamiento.h"
#include "instancia.h"
#include "tabla_entradas.h"

int main(int argc, char** argv) {
	nroOperacion = 0;
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
	pthread_t dumper;
	//tengo que hacer pthread join ?
	if (pthread_create(&dumper, NULL, dumpearADisco, NULL)) {
		log_error(logger, "Error creando el hilo del dumper\n");
		exit(EXIT_FAILURE);
	}
	t_list* posiblesAReemplazar = NULL;
	while (escucha) {
		int resultado;
		switch (recibir_cod_operacion(socketCoordinador)) {
		case OP_SET:
			resultado = SET(socketCoordinador, posiblesAReemplazar);
			if (resultado >= 0) {
				enviar_cod_operacion(socketCoordinador, EXITO);
				mandar_mensaje(socketCoordinador,
						obtenerEntradasTotales() - entradasLibres);
			} else {

				enviar_cod_operacion(socketCoordinador, ERROR);
			}
			break;
		case OP_STORE:
			;
			char* clave;
			resultado = recibir_operacion_unaria(socketCoordinador, &clave);
			if (resultado < 0) {
				log_error(logger, "Error al recibir operacion ");
				close(socketCoordinador);
				exit(EXIT_FAILURE);
			}
			log_trace(logger, "Store %s", clave);
			resultado = STORE(clave);
			if (resultado >= 0) {
				enviar_cod_operacion(socketCoordinador, EXITO);
				mandar_mensaje(socketCoordinador,
						obtenerEntradasTotales() - entradasLibres);
			} else {
				enviar_cod_operacion(socketCoordinador, ERROR);
			}
			break;
		case RELEVANTAR_INSTANCIA:
			log_info(logger, "La instancia se esta relevantando.....");
			int cantidadClaves;
			recibirPaquete(socketCoordinador, &cantidadClaves, sizeof(int));
			for (int i = 0; i < cantidadClaves; i++) {
				void* clave;
				try_recibirPaqueteVariable(socketCoordinador, &clave);
				FILE* arch;
				if ((arch = fopen(clave, "r")) == NULL) {
					log_info(logger,
							" la instancia no encuentra el archivo de la clave %d",
							clave);
				}
				char* valor;
				size_t lenght = 0;
				getline(&valor, &lenght, arch);
				hacer_set(clave, valor, posiblesAReemplazar);
				free(valor);
				fclose(arch);
				free(clave);
			}
			free(clave);
			break;
		case INSTANCIA_COMPACTAR:
			log_info(logger, "Estoy compactando ...");
			enviar_cod_operacion(socketCoordinador, EXITO);
			//si falla deberia contestarle ERROR
			break;
		case SOLICITUD_CLAVE:
			//recibo una clave voy a la tabla de entradas me fijo la posicion y voy al almacenamiento y la saco y la devuelvo
			break;
		default:
			log_info(logger, "no se pudo interpretar el mensaje");
			eliminarAlmacenamiento();
			//TODO	destruirTE();
			free(posiblesAReemplazar);
			close(socketCoordinador);
			escucha = 0;

		}
	}

	limpiar_configuracion();
	log_destroy(logger);
	exit(0);
}
