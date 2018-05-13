

#include "selector.h"

int selector(){


	lista_esis_listos = *list_create();
	lista_esis_corriendo = *list_create();
	lista_esis_bloq_consola = *list_create();
	lista_esis_bloq_rec = *list_create();
	lista_esis_finalizados = *list_create();

	listener=crear_socket_escucha(configuracion.portCoord,BACKLOG);
			log_info(logger,"Escuchando en puerto: %s", configuracion.portCoord);


			// add the listener to the master set
			FD_SET(listener, &master);
			// keep track of the biggest file descriptor
			fdmax = listener; // so far, it's this one
			// main loop
			for(;;) {
			read_fds = master; // copy it
			if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
			}
			// run through the existing connections looking for data to read
			for(i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) { // we got one!!
			if (i == listener) {
			// handle new connections
			addrlen = sizeof remoteaddr;
			newfd = accept(listener,
			(struct sockaddr *)&remoteaddr,
			&addrlen);
			if (newfd == -1) {
			perror("accept");
			} else {
			FD_SET(newfd, &master); // add to master set
			if (newfd > fdmax) { // keep track of the max
			fdmax = newfd;
			}
			printf("selectserver: new connection from %s on "
			"socket %d\n",
			inet_ntop(remoteaddr.ss_family,
			get_in_addr((struct sockaddr*)&remoteaddr),
			remoteIP, INET6_ADDRSTRLEN),
			newfd);
			}
			} else {
			// handle data from a client
			if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
			Beej's Guide to Network Programming 40
			// got error or connection closed by client
			if (nbytes == 0) {
			// connection closed
			printf("selectserver: socket %d hung up\n", i);
			} else {
			perror("recv");
			}
			close(i); // bye!
			FD_CLR(i, &master); // remove from master set
			} else {
			// we got some data from a client
			for(j = 0; j <= fdmax; j++) {
			// send to everyone!
			if (FD_ISSET(j, &master)) {
			// except the listener and ourselves
			if (j != listener && j != i) {
			if (send(j, buf, nbytes, 0) == -1) {
			perror("send");
			}
			}
			}
			}
			}
			} // END handle data from client
			} // END got new incoming connection
			} // END looping through file descriptors
			} // END for(;;)--and you thought it would never end!

			int client_socket=accept(listener,NULL,NULL);
							log_info(logger, "Conexion aceptada");
								recibir_confirmacion(client_socket);
								mandar_confirmacion(client_socket);

return -1;

}