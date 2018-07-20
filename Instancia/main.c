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
#include "algoritmos.h"
#include "almacenamiento.h"
#include "cfg_almacenamiento.h"
#include "instancia.h"
#include "tabla_entradas.h"

void sigalrm_handler();
void configurar_timer_dumper();
void responder_solicitud_clave(int);
void imprimir_almacenamiento();

int main(int argc, char** argv) {
	nroOperacion = 0;
	posicion = 0;
	char * nombreInst;
	char * nombreLog=malloc(sizeof(char)*25);

	nombreInst=obtenerNombre(argv[1]);
	nombreLog=strcat(strcpy(nombreLog,obtenerNombre(argv[1])),".log");
	logger = log_create(nombreLog, nombreInst, true, LOG_LEVEL);

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
	while (escucha) {
		int resultado;
		switch (recibir_cod_operacion(socketCoordinador)) {
		case OP_SET:
			log_trace(logger, "Se procede a realizar el codigo de op_set");
			operacion_set(socketCoordinador);
			nroOperacion++;
			imprimir_almacenamiento();
			break;
		case OP_STORE:
			;
			char* clave;
			resultado = recibir_operacion_unaria(socketCoordinador, &clave);
			log_trace(logger, "Se recibio la clave (%s) del coordinador",
					clave);
			if (resultado < 0) {
				log_error(logger, "Error al recibir operacion ");
				close(socketCoordinador);
				exit(EXIT_FAILURE);
			}
			log_trace(logger, "Store %s", clave);
			resultado = STORE(clave);
			if (resultado >= 0) {
				log_info(logger, "STORE %s arrojo resultado EXITO", clave);
				enviar_cod_operacion(socketCoordinador, EXITO);
				mandar_mensaje(socketCoordinador, almac_entradas_disponibles());
			} else {
				log_error(logger, "STORE %s recibio resultado ERROR", clave);
				enviar_cod_operacion(socketCoordinador, ERROR);
			}
			nroOperacion++;
			break;
		case RELEVANTAR_INSTANCIA:
			log_info(logger, "La instancia se esta relevantando.....");
			int cantidadClaves;
			recibirPaquete(socketCoordinador, &cantidadClaves, sizeof(int));
			log_trace(logger, "cantidad de claves a relevantar (%d)",
					cantidadClaves);
			for (int i = 0; i < cantidadClaves; i++) {
				char* clave_recibida = NULL;
				try_recibirPaqueteVariable(socketCoordinador,
						(void**) &clave_recibida);
				log_debug(logger, "Recibi clave %s", clave_recibida);
				FILE* arch;
				char* path = string_from_format("%s/%s.txt",
						configuracion.punto_montaje, clave_recibida);
				if ((arch = fopen(path, "r")) == NULL) {
					log_warning(logger,
							" la instancia no encuentra el archivo de la clave %s",
							clave_recibida);
					free(path);
					free(clave_recibida);
					continue;
				}
				char* valor = NULL;
				size_t lenght = 0;
				getline(&valor, &lenght, arch);
				log_debug(logger, "Valor encontrado: \"%s\"", valor);
				SET: ;
				int intento_set = 0;
				t_resultado_set resultado_set = hacer_set(clave_recibida,
						valor);

				if (resultado_set == REQUIERE_COMPACTACION && intento_set <= 0){
					compactar();
					intento_set ++;
					goto SET;
				}
				free(path);
				free(valor);
				fclose(arch);
				free(clave_recibida);
				imprimir_almacenamiento();
			}
			break;
		case INSTANCIA_COMPACTAR:
			log_info(logger, "Estoy compactando ...");
			compactar();
			enviar_cod_operacion(socketCoordinador, EXITO);
			break;
		case SOLICITUD_VALOR:
			//recibo una clave voy a la tabla de entradas me fijo la posicion y voy al almacenamiento y la saco y la devuelvo
			responder_solicitud_clave(socketCoordinador);
			break;
		default:
			log_error(logger,
					"No se pudo interpretar el mensaje del coordinador o este se ha desconectado");
			eliminarAlmacenamiento();
			//TODO	destruirTE();
			list_destroy_and_destroy_elements(tabla, entrada_destroyer);
			close(socketCoordinador);
			escucha = 0;

		}
	}

	limpiar_configuracion();
	log_destroy(logger);
	free(nombreInst);
	free(nombreLog);
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

void responder_solicitud_clave(int sockfd) {
	char* clave;
	try_recibirPaqueteVariable(sockfd, (void**) &clave);
	log_info(logger, "Recuperando valor de la clave %s", clave);
	char* valor = obtener_valor_de_clave(clave);
	free(clave);

	if (valor == NULL) {
		log_debug(logger, "Se informa al coordinador VALOR_NO_ENCONTRADO");
		enviar_cod_operacion(sockfd, VALOR_NO_ENCONTRADO);
		return;
	}
	log_debug(logger, "Se informa al coordinador VALOR_ENCONTRADO");
	enviar_operacion_unaria(sockfd, VALOR_ENCONTRADO, valor);
	free(valor);
	return;
}

void imprimir_almacenamiento() {
	char* alm = ato->dato;
	int i, j;
	for (i = 0; i < obtenerEntradasTotales(); i++) {
		for (j = 0; j < obtenerTamanioEntrada(); j++) {
			putchar(alm[i * obtenerTamanioEntrada() + j]);
		}
		printf("\n");
	}
}

char *obtenerNombre(char *path){
	char *t=malloc(sizeof(char)*20);
	char *q,*p=path;
	int cont = 0;
		while(*p!='i'&&*p!='\0'){
			p = (p+1);
		}
		q=p;
		while(*q!='c'&&*q!='\0'){
			q = (q+1);
			cont++;
		}
		if(*q=='c'){
			cont=cont-1;
		}

		strncpy(t,p,cont);
	return t;

}
