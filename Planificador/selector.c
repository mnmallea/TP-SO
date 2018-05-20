#include "selector.h"

void *listener(void *ptr){

	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	//char remoteIP[INET_ADDRSTRLEN];
	char buf[50];
	int nbytes;
	t_esi *n_esi;

	socketServer=crear_socket_escucha(configuracion.portCoord,BACKLOG);
	log_info(logger,"Escuchando en puerto: %s", configuracion.puerto);

	FD_SET(socketServer, &master);

	fdmax = socketServer;
	for(;;) {
		read_fds = master;
		if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			log_error(logger, "No se pudo seleccionar conexiones\n");
		}

		for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == socketServer) {

					addrlen = sizeof remoteaddr;
					newfd = accept(socketServer, (struct sockaddr *)&remoteaddr, &addrlen);
					if (newfd == -1) {
						log_error(logger, "No se pudo aceptar la conexion\n");
					}
					else {
						FD_SET(newfd, &master);
						if (newfd > fdmax) {
							fdmax = newfd;
						}
						log_trace(logger, "Nueva conexion por el socket %d\n",newfd);
						n_esi=crear_nodo_esi(newfd);
						list_add(lista_esis_listos,n_esi);
						log_info(logger,"Cantidad de elementos en la lista: %d", list_size(lista_esis_listos));
					}
				}
				else {

					if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {

						if (nbytes == 0) {
							log_error(logger, "La conexion del socket %d finalizo inesperadamente\n", i);
							socketAEliminar=i;
							list_remove_and_destroy_by_condition(lista_esis_listos,(void*)socketProceso,(void*)free);
						}
						else {
							log_error(logger, "El mensaje recivido por socket %d tiene errores\n", i);
						}
							close(i);
							FD_CLR(i, &master);
					}
				}



			}

		}
	}
//	void list_destroy_and_destroy_elements(lista_esis_listos,n_esi); ver e tipo del elementos
//list_remove_and_destroy_by_condition(lista_esis_listos,(void*)socketProceso,(void*)free);
	return NULL;

}


t_esi *crear_nodo_esi(int socket){
		t_esi *p=malloc(sizeof(esi));
			   p -> socket=newfd;
			   p -> estim_anter=configuracion.estimacion_inicial;
			   p -> clave_bloq=configuracion.claves_bloqueadas;
			   p -> dur_ult_raf=0;
			   p -> viene_esperando=0;

		return p;
}

int socketProceso(t_esi* n_esi){
	return (n_esi->socket==socketAEliminar);
}
