#include socket.h

void configure_logger() {
  logger = log_create("socket.log", "socket", true, LOG_LEVEL_INFO);
}

int crear_socket_cliente(char *ip, char *puerto){
    int socket;
    struct addrinfo hints, *serverInfo;
    hints.ai_family=AF_UNSPECT;
    hints.ai_flags=AI_PASSIVE;
    hints.ai_socktype=SOCK_STREAM;
    memset(&hints,0,sizeof (hints));

    
    if (getaddrinfo(ip, puerto, &hints, &serverInfo)) != 0){
        salir_con_error(NULL,"Error en getaddrinfo");
    }
    
    if(socket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol)<0){
        salir_con_error(socket,"Error al crear el socket");
    }
    if(connect(socket, serverInfo->ai_addr, serverInfo->ai_addrlen)<0){
        salir_con_error(socket,"Error al conectar el socket");
    }
    freeaddrinfo(serverInfo);
    log_info(logger, "Socket conectado");
    return socket;
}

//----------------------------------------------------------------------------------------------------------

int crear_socket_escucha(char *puerto_escucha,int max_comm){
    int socket;
    
     if(socket = socket(AF_UNSPECT, SOCK_STREAM, 0)<0){
        salir_con_error(socket,"error al crear el socket");
    }

    struct addrinfo hints, *serverInfo;
    hints.ai_family=AF_UNSPECT;
    hints.ai_flags=AI_PASSIVE;
    hints.ai_socktype=SOCK_STREAM;
    memset(&hints,0,sizeof (hints));
    
    int esta_activo = 1; //SETSOCKOPT permite reusar 
    //el socket, es decir, libera la parte del socket que quedo dando vueltas en el Kernel
    //que retrasa al puerto y asi no esperar a que se libere por si solo. Elimina el mensaje "Address already in use"
    if(setsockopt(socket,SOL_SOCKET,SO_REUSEADDR,&si,sizeof(esta_activo)) == -1){
        salir_con_error(socket,"error en el setsockpot");
    }
    //Obtiene la informacion de la direccion (IP, puerto, etc)
    //y lo guarda
    if (getaddrinfo(ip, puerto_escucha, &hints, &serverInfo)) != 0){
        salir_con_error(socket,"Error en getaddrinfo");
    }
    //Asocio al socket con una direccion en particular,si falla graba el error
    if (bind(socket, (struct sockaddr*) &serverInfo, sizeof(struct sockaddr)) == -1) {
        salir_con_error(socket,"error en el bind");
	}

    if (listen(serverInfo, max_com) == -1) {
		salir_con_error(socket,"no se pudo escuchar");
	}
	return socket ;
    }

//---------------------------------------------------------------------
#define id_verificador 1111

void esperar_mensaje(int socket){
    int id;
    int res_recv= recv(socket,&id,sizeof(id),MSG_WAITALL);
    if(id != id_verificador){ //chequeo por contenido
        salir_con_error(socket,"No coincide lo recibido con lo esperado");
    }
    else if (sizeof(id)!=res_recv){ //chequeo por tamaño
        salir_con_error(socket,"No coincide lo recibido con lo esperado");
    }

log_info(logger, "Mensaje recibido");
free(id);
}

void mandar_mensaje(int socket){
    int id=id_verificador;
    int res_send = send(socket, &id, sizeof(id), 0);
    if(res_send != sizeof(id)){
        salir_con_error(socket,"No se pudo mandar mensaje");
    }
log_info(logger, "Mensaje enviado");
free(id);    
}

void recibir_confirmacion (int socket){
    int  resultado=1;
    if (recv(socket,&resultado,sizeof(int),0)<=0){
        salir_con_error(socket,"no se pudo recibir confirmación");
    }
    log_info(logger, "confirmación recibida");
    close(socket);
}

void mandar_confirmacion(int socket) {
    int resultado=1;
	if (send(socket, &resultado, sizeof(resultado), 0)<=0) {
        salir_con_error(socket,"no se pudo mandar confirmación");
	}
    log_info(logger, "confirmación enviada");
    close(socket);
}



void salir_con_eror(int socket, char* error_msg){
        log_error(logger, error_msg);
        close(socket);
  exit_gracefully(1);
}


void exit_gracefully(int return_nr) {
  log_destroy(logger);
  exit(return_nr);
}
