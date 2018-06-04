#include "selector.h"

void *listener(void *ptr){

	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	t_protocolo buf;
	int nbytes;
	t_esi *n_esi;
	int id=1;

	socketServer=crear_socket_escucha(configuracion.puerto,BACKLOG);
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
				if (i == socketServer)
				{

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
						n_esi->id=id;
						list_add(lista_esis_listos,n_esi);
						mandar_mensaje(newfd,id);
						id++;
						log_info(logger,"Cantidad de elementos en la lista: %d", list_size(lista_esis_listos));
						mandar_confirmacion(newfd);
					}
				}
				else
				{

					if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0)
					{

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


					//funcion atenderESI(i,buf); //necesita el id?
					switch (buf)
					{
						case EXITO :

						break;

						case ERROR :

						break;

						case SOLICITUD_CLAVE :

						break;

						case BLOQUEO_CLAVE :

						break;

						case DESBLOQUEO_CLAVE :

						break;

						case BLOQUEO_ESI :

						break;

						case CORRER_ESI :

						break;

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
			   p -> socket=socket;
			   p -> estim_anter=configuracion.estimacion_inicial;
			   p -> clave_bloq=configuracion.claves_bloqueadas;
			   p -> dur_ult_raf=0;
			   p -> viene_esperando=0;

		return p;
}

int socketProceso(t_esi* n_esi){
	return (n_esi->socket==socketAEliminar);
}

void atenderESI(socket,buf)
{

	switch(buf)
	{

	}

}
