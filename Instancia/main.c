/*
 * main.c
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */

#include "main.h"

#include <commons/collections/list.h>
#include <commons/string.h>
#include <signal.h>
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

void sigalrm_handler();
void configurar_timer_dumper();

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
	configurar_timer_dumper();
	t_list* posiblesAReemplazar = NULL;
	while (escucha) {
		int resultado;
		switch (recibir_cod_operacion(socketCoordinador)) {
		case OP_SET:
			resultado = SET(socketCoordinador, posiblesAReemplazar);
			if (resultado >= 0) {
				enviar_cod_operacion(socketCoordinador, EXITO);
				mandar_mensaje(socketCoordinador, entradasLibres);
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
				mandar_mensaje(socketCoordinador, entradasLibres);
			} else {
				enviar_cod_operacion(socketCoordinador, ERROR);
			}
			break;
		case RELEVANTAR_INSTANCIA:
			log_info(logger, "La instancia se esta relevantando.....");
			int cantidadClaves;
			recibirPaquete(socketCoordinador, &cantidadClaves, sizeof(int));
			for (int i = 0; i < cantidadClaves; i++) {
				char* clave_recibida = NULL;
				try_recibirPaqueteVariable(socketCoordinador,
						(void**) &clave_recibida);
				log_debug(logger, "Recibi clave %s", clave_recibida);
				FILE* arch;
				char* path = string_from_format("%s/%s.txt",
						configuracion.punto_montaje, clave_recibida);
				if ((arch = fopen(path, "r")) == NULL) {
					log_info(logger,
							" la instancia no encuentra el archivo de la clave %s",
							clave_recibida);
					free(path);
					free(clave_recibida);
					continue;
				}
				char* valor = NULL;
				size_t lenght = 0;
				getline(&valor, &lenght, arch);
				hacer_set(clave_recibida, valor, posiblesAReemplazar);
				free(path);
				free(valor);
				fclose(arch);
				free(clave_recibida);
			}
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

void configurar_timer_dumper() {
	signal(SIGALRM, sigalrm_handler);
	alarm(configuracion.intervalo_dump);
}
void sigalrm_handler() {
	log_info(logger, "Se procede a realizar el DUMP...");
	dumpearADisco(NULL);
	alarm(configuracion.intervalo_dump);
}
