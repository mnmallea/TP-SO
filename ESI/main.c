#include "../syntax-commons/my_socket.h"
#include "config_esi.h"
/*
#define ipCord "127.0.0.1"
#define portCord "8000"
#define ipPlanif "127.0.0.2"
#define portPlanif "8001"
*/
#define CANT_ARGUMENTOS_MAIN 2


#define LOG_LEVEL LOG_LEVEL_TRACE

t_log* logger;

config configuracion; //no le pongan otro nombre, porque despues limpiar_configuracion() se va a encargar de borrarla

#include "config_esi.h"

int *c;//para que carajo sirve esto???



/*#include <parser.h>*/


int main(int argc, char* argv[]){
	logger = log_create("ESI.log","ESI",true,LOG_LEVEL);
	//char* j;
	if(argc != CANT_ARGUMENTOS_MAIN){
			log_error(logger, "Cantidad incorrecta de parametros");
			exit_gracefully(1);
		return -1;
	}
	log_debug(logger, "Ruta de configuracion: %s", argv[1]);
	configuracion = configurar(argv[1]);

	log_info(logger, "Conectandose al Coordinador, IP: %s\tPuerto: %s", configuracion.ipCord, configuracion.portCord);
	int socketCoord= crear_socket_cliente(configuracion.ipCord,configuracion.portCord);
	log_info(logger, "Conectandose al Planificador, IP: %s\tPuerto: %s", configuracion.ipPlan, configuracion.portPlan);
	int socketPlan= crear_socket_cliente(configuracion.ipCord,configuracion.portPlan);

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
	limpiar_configuracion();
	exit(1);
}
