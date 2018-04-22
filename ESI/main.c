#include "../syntax-commons/my_socket.h"
#include "config_esi.h"

#define ipCord "127.0.0.1"
#define portCord "8000"
#define ipPlanif "127.0.0.2"
#define portPlanif "8001"


#define LOG_LEVEL LOG_LEVEL_TRACE

t_log* logger;

config reg;

#include "config_esi.h"

int *c;


/*#include <parser.h>*/


int main(int argc, char* argv[]){
	logger = log_create("ESI.log","ESI",true,LOG_LEVEL);
	//char* j;
	//reg = configurar("/home/utnso/workspace/TP/tp-2018-1c-SyntaxError/ESI/config_esi");

	if(argc!=2){
			log_info(logger, "Cantidad incorrecta de parametros");
			exit_gracefully(1);
		return -1;
		}

	int socketCoord=crear_socket_cliente(ipCord,portCord);
	int socketPlan=crear_socket_cliente(ipPlanif,portPlanif);

	mandar_confirmacion(socketCoord);
	mandar_confirmacion(socketPlan);

	recibir_confirmacion(socketCoord);
	recibir_confirmacion(socketPlan);


/*
	if((j=malloc(sizeof(char)*50))==NULL){
		printf("No pudo alocar en memoria.\n");
		return -1;
	}
	FILE * f;
	f=fopen(argv[1],"r");
	fscanf(f,"%s",j);
	//parser(f)
	printf("La cadena es: %s\n",j);

	free(j);
*/
	return 1;
}
