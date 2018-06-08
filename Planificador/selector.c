#include "selector.h"

void *listener(void *ptr) {

	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	t_protocolo buf;
	char* clave;
	int nbytes;
	t_esi *n_esi;
	int id = 1;
	int handshake_msg = PLANIFICADOR;

	socketServer = crear_socket_escucha(configuracion.puerto, BACKLOG);
	int socketCord = conectarse_a_coordinador(configuracion.ipCoord,
			configuracion.portCoord, handshake_msg);

	log_info(logger, "Escuchando en puerto: %s", configuracion.puerto);

	FD_SET(socketServer, &master);
	FD_SET(socketCord, &master);

	if (socketCord > socketServer)
		fdmax = socketCord;
	else
		fdmax = socketServer;

	for (;;) {
		read_fds = master;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			log_error(logger, "No se pudo seleccionar conexiones\n");
		}

		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == socketServer) {

					addrlen = sizeof remoteaddr;
					newfd = accept(socketServer,
							(struct sockaddr *) &remoteaddr, &addrlen);
					if (newfd == -1) {
						log_error(logger, "No se pudo aceptar la conexion\n");
					} else {
						FD_SET(newfd, &master);
						if (newfd > fdmax) {
							fdmax = newfd;
						}

						log_trace(logger, "Nueva conexion por el socket %d\n",
								newfd);
						n_esi = crear_nodo_esi(newfd);
						n_esi->id = id;
						nuevo_esi(n_esi);
						if (id == 1) {
							sem_post(&sem_binario_planif);
							primera_vez = true;
						}

						mandar_mensaje(newfd, id);
						id++;
						log_info(logger,
								"Cantidad de elementos en la lista: %d",
								list_size(lista_esis_listos));
					}
				}

				else {

					if ((nbytes = recv(i, &buf, sizeof buf, 0)) <= 0) {

						if (nbytes == 0) {
							log_error(logger,
									"La conexion del socket %d finalizo inesperadamente\n",
									i);
							socketAEliminar = i;
							list_remove_and_destroy_by_condition(
									lista_esis_listos, (void*) socketProceso,
									(void*) free);
						} else {
							log_error(logger,
									"El mensaje recivido por socket %d tiene errores\n",
									i);
						}
						close(i);
						FD_CLR(i, &master);

					} else {
						if (i == socketCord) {
							//mandar_confirmacion(socketCord);
							//queda comentado porque era para pruebas


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

								if(la_tiene){
									cod_op = EXITO;
								}else{
									cod_op = CLAVE_NO_BLOQUEADA_EXCEPTION;
								}

								enviar_cod_operacion(i, cod_op);
								}
								break;
							case SOLICITUD_CLAVE:
								nueva_solicitud(i, clave);

								break;
							default:
								break;
							}

						} else {
							switch (buf) { //mensajes de esis
							case EXITO:
								ya_termino_linea();
								break;
							case ABORTA:
								fallo_linea();
								break;
							case FINALIZO_ESI:
								finalizar_esi();
								break;
							case BLOQUEO_ESI:
								//supongo que ya me encargue de guardarlo como bloqueado
								break;
							case INSTANCIA_CAIDA_EXCEPTION:
								fallo_linea();
								break;
							default:
								break;
							}
							//sem_post(&sem_binario_planif);
						}

					}

				}

			}

		}

	}
//	void list_destroy_and_destroy_elements(lista_esis_listos,n_esi); ver e tipo del elementos
//list_remove_and_destroy_by_condition(lista_esis_listos,(void*)socketProceso,(void*)free);
	return NULL;

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
