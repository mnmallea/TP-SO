#include "my_socket.h"

void configure_logger() {
  logger = log_create("mysocket.log", "socket",true , LOG_LEVEL_INFO);
}

int crear_socket_cliente(char *ip, char *puerto){
    int my_socket;
    struct addrinfo hints, *serverInfo;
    memset(&hints,0,sizeof (hints));
    hints.ai_family=AF_UNSPEC;
    hints.ai_flags=AI_PASSIVE;
    hints.ai_socktype=SOCK_STREAM;
    
    if (getaddrinfo(ip,puerto, &hints, &serverInfo) < 0){
    	salir_con_error(0,"Error en getaddrinfo");
    }
    my_socket=socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

    if(my_socket <0)
        salir_con_error(my_socket,"Error al crear el socket");


    if(connect(my_socket, serverInfo->ai_addr, serverInfo->ai_addrlen)<0)
        salir_con_error(my_socket,"Error al conectar el socket");

    freeaddrinfo(serverInfo);

    log_info(logger, "Socket conectado");
    return my_socket;
}

//----------------------------------------------------------------------------------------------------------

int crear_socket_escucha(char *puerto_escucha, int max_comm) {

	struct addrinfo hints, *serverInfo, *p;
	int sockfd;
	int yes = 1;
	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(NULL, puerto_escucha, &hints, &serverInfo) != 0)
		salir_con_error(0, "Error en el getaddrinfo");

	for (p = serverInfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			log_error(logger, strerror(errno));
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			salir_con_error(sockfd, strerror(errno));
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			log_error(logger, strerror(errno));
			continue;
		}

		break;
		salir_con_error(sockfd,"No se pudo crear el socket escucha");
	}

	freeaddrinfo(serverInfo);

	if (listen(sockfd, max_comm) == -1)
		salir_con_error(sockfd, "no se pudo escuchar");

	return sockfd;
}

//---------------------------------------------------------------------
#define id_verificador 1

void recibir_mensaje(int my_socket){
    int id;
    int res_recv= recv(my_socket,&id,sizeof(id),MSG_WAITALL);
    if(id != id_verificador){ //chequeo por contenido
        salir_con_error(my_socket,"No coincide lo recibido con lo esperado");
    }
    else if (sizeof(id)!=res_recv){ //chequeo por tamaño
        salir_con_error(my_socket,"No coincide lo recibido con lo esperado");
    }

log_info(logger, "Mensaje recibido");
}

int recibir_info(int my_socket,void** element){
    int res_recv= recv(my_socket,&element,sizeof(element),MSG_WAITALL);
    if(element != id_verificador){ //chequeo por contenido
        return -1;
    }
    else if (sizeof(element)!=res_recv){ //chequeo por tamaño
        return -1;
    }
    return 1;

log_info(logger, "Mensaje recibido");
}

void recibir_confirmacion (int my_socket){
    int  resultado=1;
    if (recv(my_socket,&resultado,sizeof(int),0)<=0){
        salir_con_error(my_socket,"no se pudo recibir confirmación");
    }
    log_info(logger, "confirmación recibida");
    //close(my_socket);
}

void mandar_mensaje(int my_socket){
    int id=id_verificador;
    int res_send = send(my_socket, &id, sizeof(id), 0);
    if(res_send != sizeof(id)){
        salir_con_error(my_socket,"No se pudo mandar mensaje");
    }
    log_info(logger, "Mensaje enviado");
}

void mandar_info(int my_socket, void* element){
    int id=id_verificador;
    int res_send = send(my_socket, &element, sizeof(element), 0);
    if(res_send != sizeof(element)){
        salir_con_error(my_socket,"No se pudo mandar mensaje");
    }
    //log_info(logger, "Mensaje enviado");
}



void mandar_confirmacion(int my_socket) {
    int resultado=1;
	if (send(my_socket, &resultado, sizeof(resultado), 0)<=0) {
        salir_con_error(my_socket,"no se pudo mandar confirmación");
	}
    log_info(logger, "confirmación enviada");
    //close(my_socket);
}

void safe_send(int my_socket, void* msg, int msg_len) {
	int res_send = send(my_socket, msg, msg_len, 0);
	if (res_send != msg_len) {
		salir_con_error(my_socket, "No se pudo mandar mensaje");
	}
	log_info(logger, "Mensaje enviado");
}


/*
 * Recibe un mensaje de tamaño msg_len a traves del socket my_socket
 * Devuelve un puntero a un buffer que contiene el mensaje recibido
*/
void* safe_recv(int my_socket, int msg_len) {
	void *buffer = malloc(msg_len);
	int res = recv(my_socket, buffer, msg_len, MSG_WAITALL);
	if (res <= 0) {
		free(buffer);
		salir_con_error(my_socket, strerror(errno));
	}
	log_info(logger, "Mensaje de %d bytes recibido", res);
	return buffer;
}

void salir_con_error(int my_socket, char* error_msg){
  log_error(logger, error_msg);
  if(my_socket <= 0) {
	  close(my_socket);
  }
  exit_gracefully(1);
}


void exit_gracefully(int return_nr) {
  log_destroy(logger);
  exit(return_nr);
}
