/*
 * main.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#include "main.h"
#include "config_planificador.h"
#include "algoritmos_planificacion.h"
#include <commons/collections/list.h>
int *c;


#define ipCord "127.0.0.1"
#define portCord "8080"
#define port "8001"
#define LOG_LEVEL LOG_LEVEL_TRACE
#define BACKLOG 5


t_log *logger;
config configuracion;


t_list* lista_esis_listos;
t_list* lista_esis_corriendo;
t_list* lista_esis_bloq_consola;
t_list* lista_esis_bloq_rec;
t_list* lista_esis_finalizados;

void *menu(void *ptr){

	char *message;
	message = (char *) ptr;
	printf("%s \n", message);

	int opcion_seleccionada;
	char *clave = (char*)malloc(40);
	int recurso;
	int id;

		printf("Ingrese la opcion que desea ejecutar: "
					"\n 1: para pausar/continuar la ejecucion del planificador "
					"\n 2 <clave> <ID>: para bloquear un proceso"
					"\n 3 <clave>: para desbloquear un proceso bloqueado "
					"\n 4 <recurso>: listar procesos esperando al recurso"
					"\n 5 <ID>: para matar un proceso"
					"\n 6 <clave>: para conocer el status de la clave"
					"\n 7: para solucionar conflictos de deadlock "
					"\n 0: para salir de la consola \n"
					);

		scanf("%d", &opcion_seleccionada);

		while( opcion_seleccionada != 0 ){

			switch(opcion_seleccionada){
			case 1:
				printf("Ingreso pausar/despausar la consola");
				break;
			case 2:
				printf("Ingreso bloquear un proceso, ingrese <clave>");

				//ACA ME TIRA UN WARNING de que clave es char **
				scanf("%s", &clave);

				printf("Ingreso bloquear un proceso, ingrese <ID>");

				scanf("%d", &id);
				break;
			case 3:
				printf("Ingreso desbloquear un proceso, ingrese <clave>");

				scanf("%s", &clave);
				break;
			case 4:
				printf("Ingreso listar procesos esperando un recurso, ingrese <recurso>");

				scanf("%d", &recurso);
				break;
			case 5:

				printf("Ingreso matar un proceso, ingrese <ID>");

				scanf("%d", &id);
				break;
			case 6:
				printf("Ingreso matar un proceso, ingrese <clave>");

				scanf("%s", &clave);
				break;
			case 7:
				printf("Ingreso solucionar problemas de deadlock");
				break;
			case 0:
				break;
			default:
				break;
			}

			scanf("%d", &opcion_seleccionada);

		}

	printf("Ha salido de la consola");
	free(clave);
	return NULL;

}

int main(int argc, char **argv){//aca recibiriamos la ruta del archivo de configuracion como parametro

	logger = log_create("planificador.log","Planificador",true,LOG_LEVEL);
	configuracion = configurar(argv[1]);
	log_trace(logger, "Planificador correctamente configurado");

	lista_esis_listos = list_create();
	lista_esis_corriendo = list_create();
	lista_esis_bloq_consola = list_create();
	lista_esis_bloq_rec = list_create();
	lista_esis_finalizados = list_create();
	log_trace(logger, "Lista de Esis creadas correctamente");

	int local_socket=crear_socket_escucha(port,BACKLOG);
		log_info(logger,"Escuchando en puerto: %s", port);

		log_trace(logger,"Intentando conectarse al Coordinador. IP: %s  Puerto: %s", ipCord, portCord);
	    int socketCoord=crear_socket_cliente(ipCord,portCord);
			mandar_confirmacion(socketCoord);
			recibir_confirmacion(socketCoord);

		int client_socket=accept(local_socket,NULL,NULL);
		log_info(logger, "Conexion aceptada");
			recibir_confirmacion(client_socket);
			mandar_confirmacion(client_socket);



/*
	 pthread_t consola_planificador;
	 const char *message1 = "Inicializacion de la consola";

	if(pthread_create(&consola_planificador, NULL, menu, (void*) message1)) {

		fprintf(stderr, "Error creando el hilo de la consola\n");
		return 1;
	}

	if(pthread_join(consola_planificador, NULL)) {

		fprintf(stderr, "Error al joinear el hilo de la consola\n");
		return 1;
	}
*/



	return 0;
}


void obtener_proximo_segun_fifo(t_list* lista_esis_actual, t_list* lista_esis_nueva){

	t_esi* esi_elegido = malloc(sizeof(t_esi));
	esi_elegido = list_remove(lista_esis_actual, 0);
	list_add(lista_esis_nueva, esi_elegido);

}


