#include "selector.h"

void atender_error(int socketCord, int nbytes) {
	if (i == socketCord) {
		if (nbytes == 0) {
			log_error(logger,
					"La conexion con el Coordinador finalizo inesperadamente\n");
			socketAEliminar = i; //para q sirve?
			exit(EXIT_FAILURE);
		} else {
			log_error(logger,
					"El mensaje recivido por el Coordinador tiene errores\n");
		}
	} else {
		if (nbytes == 0) {
			log_error(logger,
					"La conexion con del socket %d finalizo inesperadamente\n",
					i);
			socketAEliminar = i; //para q sirve?
		} else {
			log_error(logger,
					"El mensaje recivido por el socket %d tiene errores\n", i);
		}
		//list_remove_and_destroy_by_condition(lista_esis_listos, (void*) socketProceso,
		//								(void*) free);
	}
	close(i);
	FD_CLR(i, &master);
}

void atender_nueva_conexion(int id) {
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
		log_info(logger, "Cantidad de elementos en la lista: %d",
				list_size(lista_esis_listos));
	}
}

void listener(void) {

	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	t_protocolo buf;
	char* clave;
	int nbytes;

	int id = 1;
	int handshake_msg = PLANIFICADOR;

	socketServer = crear_socket_escucha(configuracion.puerto, BACKLOG);
	log_debug(logger, "socketServer = %d", socketServer);
	int socketCord = conectarse_a_coordinador(configuracion.ipCoord,
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
					atender_nueva_conexion(id);
					continue;
				}

				if ((nbytes = recv(i, &buf, sizeof buf, MSG_NOSIGNAL)) <= 0) {
					atender_error(socketCord, nbytes);
					continue;
				}

				if (i == socketCord) {
					recibir_operacion_unaria(i, &clave);

					switch (buf) { //mensajes de coord

					case DESBLOQUEO_CLAVE:
						se_desbloqueo_un_recurso(clave);
						break;
					case ESI_TIENE_CLAVE:
						//dejar estos corchetes sin cuestionar (nay)
					{
						bool la_tiene = esi_tiene_clave(clave);
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
					case SOLICITUD_STATUS_CLAVE:
						//manda paquete con rtas
						break;
					default:
						log_warning(logger,
								"Mensaje del coordinador desconocido");
						break;
					}
					free(clave);
					continue;
				}
				if (esi_corriendo !=NULL && i == esi_corriendo->socket) {
					log_debug(logger, "Mensaje recibido de un ESI: %s",
							to_string_protocolo(buf));
					respuesta_esi_corriendo = buf;
					sem_post(&respondio_esi_corriendo);
					continue;
				}

				log_warning(logger, "Quien te conoce papa");

			}

		}

	}

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

/*void atenderESI(socket,buf)
 {

 switch(buf)
 {

 }

 }*/
