#include "my_socket.h"

void configure_logger() {
  logger = log_create("mysocket.log", "socket",true , LOG_LEVEL_INFO);
}

int crear_socket_cliente(char *ip, char *puerto){
    int my_socket,retorno;
    struct addrinfo hints, *serverInfo;
    hints.ai_family=AF_UNSPEC;
    hints.ai_flags=AI_PASSIVE;
    hints.ai_socktype=SOCK_STREAM;
    memset(&hints,0,sizeof (hints));

    
    if (getaddrinfo(ip, puerto, &hints, &serverInfo) < 0){
        salir_con_error(0,"Error en getaddrinfo");
    }
    my_socket=socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
    if(my_socket <0){
        salir_con_error(my_socket,"Error al crear el socket");
    }
    retorno=connect(my_socket, serverInfo->ai_addr, serverInfo->ai_addrlen);
    if(retorno<0){
        salir_con_error(my_socket,"Error al conectar el socket");
    }
    freeaddrinfo(serverInfo);
    log_info(logger, "Socket conectado");
    return my_socket;
}

//----------------------------------------------------------------------------------------------------------

int crear_socket_escucha(char *puerto_escucha,int max_comm){

    struct addrinfo hints, *serverInfo;
    memset(&hints,0,sizeof (hints));
    hints.ai_family=AF_UNSPEC;
    hints.ai_flags=AI_PASSIVE;
    hints.ai_socktype=SOCK_STREAM;

    if(getaddrinfo(NULL,puerto_escucha,&hints,&serverInfo)!=0){
    	salir_con_error(0,"Error en el getaddrinfo");
    }
    
    int my_socket = socket(AF_UNSPEC, SOCK_STREAM, 0);
    if(my_socket  <0){
        salir_con_error(my_socket,"error al crear el socket");
    }
    int esta_activo = 1; //SETSOCKOPT permite reusar 
    //el socket, es decir, libera la parte del socket que quedo dando vueltas en el Kernel
    //que retrasa al puerto y asi no esperar a que se libere por si solo. Elimina el mensaje "Address already in use"
    if(setsockopt(my_socket,SOL_SOCKET,SO_REUSEADDR,&esta_activo,sizeof(esta_activo)) == -1){
        salir_con_error(my_socket,"error en el setsockpot");
    }
    //Obtiene la informacion de la direccion (IP, puerto, etc)
    //y lo guarda
   /* if (getaddrinfo(ip, puerto_escucha, &hints, &serverInfo) != 0){
        salir_con_error(socket,"Error en getaddrinfo");
    }*/
    //Asocio al socket con una direccion en particular,si falla graba el error
    if (bind(my_socket, (struct sockaddr*) &serverInfo, sizeof(struct sockaddr)) == -1) {
        salir_con_error(my_socket,"error en el bind");
	}

    if (listen(my_socket, max_comm) == -1) {
		salir_con_error(my_socket,"no se pudo escuchar");
	}
	return my_socket ;
    }

//---------------------------------------------------------------------
#define id_verificador 1111

void esperar_mensaje(int my_socket){
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

void mandar_mensaje(int my_socket){
    int id=id_verificador;
    int res_send = send(my_socket, &id, sizeof(id), 0);
    if(res_send != sizeof(id)){
        salir_con_error(my_socket,"No se pudo mandar mensaje");
    }
    log_info(logger, "Mensaje enviado");
}

void recibir_confirmacion (int my_socket){
    int  resultado=1;
    if (recv(my_socket,&resultado,sizeof(int),0)<=0){
        salir_con_error(my_socket,"no se pudo recibir confirmación");
    }
    log_info(logger, "confirmación recibida");
    close(my_socket);
}

void mandar_confirmacion(int my_socket) {
    int resultado=1;
	if (send(my_socket, &resultado, sizeof(resultado), 0)<=0) {
        salir_con_error(my_socket,"no se pudo mandar confirmación");
	}
    log_info(logger, "confirmación enviada");
    close(my_socket);
}



void salir_con_error(int my_socket, char* error_msg){
  log_error(logger, error_msg);
  if(my_socket) {
	  close(my_socket);
  }
  exit_gracefully(1);
}


void exit_gracefully(int return_nr) {
  log_destroy(logger);
  exit(return_nr);
}
