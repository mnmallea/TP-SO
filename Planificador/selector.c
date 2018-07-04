#include "selector.h"

#include <commons/log.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "../syntax-commons/conexiones.h"
#include "../syntax-commons/deserializador.h"
#include "../syntax-commons/my_socket.h"
#include "../syntax-commons/protocol.h"
#include "funciones_auxiliares.h"
#include "planificacion.h"
#include "sincronizacion.h"

int id = 1;

void listener(void) {

	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	t_protocolo buf;
	char* clave;
	int nbytes;

	int handshake_msg = PLANIFICADOR;

	socketServer = crear_socket_escucha(configuracion.puerto, BACKLOG);
	log_debug(logger, "socketServer = %d", socketServer);
	socketCord = conectarse_a_coordinador(configuracion.ipCoord,
			configuracion.portCoord, handshake_msg);
	log_debug(logger, "socketCord = %d", socketCord);

	log_info(logger, "Escuchando en puerto: %s", configuracion.puerto);

	FD_SET(socketServer, &master);
	FD_SET(socketCord, &master);

	if (socketCord > socketServer)
		fdmax = socketCord;
	else
		fdmax = socketServer;

	while (1) {
		read_fds = master;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			log_error(logger, "No se pudo seleccionar conexiones\n");
		}

		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {

				if (i == socketServer) {
					atender_nueva_conexion();
					continue;
				}

				nbytes = recv(i, &buf, sizeof buf, MSG_NOSIGNAL);

				if (esi_corriendo != NULL && i == esi_corriendo->socket) {
					if (nbytes <= 0) {
						close(i);
						FD_CLR(i, &master);
						buf = ERROR_CONEXION;
					}

					if (buf == FINALIZO_ESI) {//me ahorra sincronizar y una posible condicion de carrera hacer esto aca
						close(i);
						FD_CLR(i, &master);
					}
					log_debug(logger, "Mensaje recibido de un ESI: %s",
							to_string_protocolo(buf));
					respuesta_esi_corriendo = buf;
					sem_post(&respondio_esi_corriendo);
					continue;
				}

				if (nbytes <= 0) {
					atender_error(nbytes);
					continue;
				}

				if (i == socketCord) {

					if (buf != SOLICITUD_STATUS_CLAVE) {

						recibir_operacion_unaria(i, &clave);

						switch (buf) { //mensajes de coord

						case DESBLOQUEO_CLAVE:
							se_desbloqueo_un_recurso(clave);
							break;
						case ESI_TIENE_CLAVE:
							//dejar estos corchetes sin cuestionar (nay)
						{
							//si este esi_corriendo jode mucho hablarme(nay) y lo cambio
							bool la_tiene = esi_tiene_clave(clave,
									esi_corriendo);
							t_protocolo cod_op;

							if (la_tiene) {
								cod_op = EXITO;
							} else {
								cod_op = CLAVE_NO_BLOQUEADA_EXCEPTION;
							}

							enviar_cod_operacion(i, cod_op);
						}
							break;
						case SOLICITUD_CLAVE:
							nueva_solicitud(i, clave);
							break;
						default:
							log_warning(logger,
									"Mensaje del coordinador desconocido");
							break;
						}
						free(clave);
						continue;

					} else {

						respuesta_status_clave = recibir_status_clave();
						sem_post(&coordinador_respondio_paq);

					}
				}

				log_warning(logger, "Quien te conoce papa");

			}

		}

	}

}

void atender_nueva_conexion() {
	addrlen = sizeof remoteaddr;
	newfd = accept(socketServer, (struct sockaddr*) &remoteaddr, &addrlen);
	if (newfd == -1) {
		log_error(logger, "No se pudo aceptar la conexion\n");
	} else {
		FD_SET(newfd, &master);
		if (newfd > fdmax) {
			fdmax = newfd;
		}
		log_trace(logger, "Nueva conexion por el socket %d\n", newfd);
		t_esi* n_esi = crear_nodo_esi(newfd);
		n_esi->id = id;
		nuevo_esi(n_esi);
		mandar_mensaje(newfd, id);
		id++;
	}
}

void atender_error(int nbytes) {
	log_trace(logger, "Error en el socket %d\t Socket Coordinador %d", i,
			socketCord);
	if (i == socketCord) {
		if (nbytes == 0) {
			log_error(logger,
					"La conexion con el Coordinador finalizo inesperadamente\n");

			exit(EXIT_FAILURE);
		} else {
			log_error(logger,
					"El mensaje recivido por el Coordinador tiene errores\n");
		}
	} else {
		if (nbytes == 0) {
			log_error(logger,
					"El esi conectado al socket %d finalizo inesperadamente\n",
					i);
		} else {
			log_error(logger,
					"El mensaje recivido por el socket %d tiene errores\n", i);
		}
		//list_remove_and_destroy_by_condition(lista_esis_listos, (void*) i,
		//							(void*) free);
	}
	close(i);
	FD_CLR(i, &master);
}


t_status_clave recibir_enum_status_clave() {
	t_status_clave respuesta;
	if (recv(socketCord, &respuesta, sizeof(respuesta), MSG_NOSIGNAL) <= 0) {
		log_error(logger, "Se perdio la comunicación con el coordinador");

		exit(EXIT_FAILURE);
	}
	return respuesta;
}

respuesta_status_clave_t recibir_status_clave() {
	respuesta_status_clave_t respuesta_status;

	t_status_clave respuesta;
	respuesta = recibir_enum_status_clave();

	respuesta_status.hay_valor = respuesta == HAY_VALOR ? true : false;
	if (respuesta_status.hay_valor) {
		try_recibirPaqueteVariable(socketCord,
				(void**) &respuesta_status.valor);
	}

	respuesta_status.estado_instancia = recibir_enum_status_clave();
	respuesta_status.hay_instancia =
			respuesta_status.estado_instancia != INSTANCIA_NO_ASIGNADA ?
					true : false;
	if (respuesta_status.hay_instancia) {
		try_recibirPaqueteVariable(socketCord,
				(void**) &respuesta_status.instancia);
	}

	respuesta = recibir_enum_status_clave();
	respuesta_status.hay_simulacion =
			respuesta == HAY_SIMULACION ? true : false;
	if (respuesta_status.hay_simulacion) {
		try_recibirPaqueteVariable(socketCord,
				(void**) &respuesta_status.instancia_simulacion);
	}

	return respuesta_status;
}

t_esi *crear_nodo_esi(int socket) {
	t_esi *p = malloc(sizeof(esi));
	p->socket = socket;
	p->estim_anter = configuracion.estimacion_inicial;
	p->dur_ult_raf = configuracion.estimacion_inicial;
	p->viene_esperando = 0;

	return p;
}

int socketProceso(t_esi* n_esi) {
	return (n_esi->socket == socketAEliminar);
}

