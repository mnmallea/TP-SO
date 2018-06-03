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
#include "../syntax-commons/protocol.h"

#define LOG_LEVEL LOG_LEVEL_DEBUG

config configuracion;
t_log *logger;

int main(int argc, char** argv) {
	logger = log_create("instancia.log", "Instancia", true, LOG_LEVEL);
	configuracion = configurar(argv[1]);

	int sockfd = conectarse_a_coordinador(configuracion.ip_coordinador,
			configuracion.puerto_coordinador, INSTANCIA);

	char* clave;
//	char* valor;
	switch (recibir_cod_operacion(sockfd)) {
	case GET:
	case STORE:
		recibir_operacion_unaria(sockfd, &clave);
		break;
	case SET:
//		recibir_set(sockfd, &clave, &valor);
		break;
	}

	printf("%s\n", clave);
	free(clave);
//	free(valor);

//	sockfd = crear_socket_cliente(configuracion.ip_coordinador, configuracion.puerto_coordinador);
//	mandar_mensaje(sockfd);
//	recibir_confirmacion(sockfd);

	limpiar_configuracion();
	log_destroy(logger);
	exit(0);
}
