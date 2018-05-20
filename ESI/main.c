#include "main.h"

int main(int argc, char* argv[]){
	FILE * fp;
	char * line = NULL;
	//size_t len = 0;
	//ssize_t read;
	logger = log_create("ESI.log","ESI",true,LOG_LEVEL);

	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
  
	if(argc != CANT_ARGUMENTOS_MAIN){
			log_error(logger, "Cantidad incorrecta de parametros");
			exit_gracefully(1);
		return -1;
	}

	configuracion = configurar(argv[1]);

	//conectarse_a_coordinador(configuracion.ipCord, configuracion.portCord, ESI);
	//log_info(logger, "Conectandose al Planificador, IP: %s\tPuerto: %s", configuracion.ipPlan, configuracion.portPlan);

	int socketPlan= crear_socket_cliente(configuracion.ipPlan,configuracion.portPlan);

	mandar_confirmacion(socketPlan);

	fp = fopen(argv[1], "r");
	    if (fp == NULL){
	    	log_error(logger, "Cantidad incorrecta de parametros");
	        exit(EXIT_FAILURE);
	    }

/*

	    while ((read = getline(&line, &len, fp)) != -1)
	    {
	        t_esi_operacion parsed = parse(line);

	        if(parsed.valido)
	        {
	            switch(parsed.keyword)
	            {
	                case GET:
	                    printf("GET\tclave: <%s>\n", parsed.argumentos.GET.clave);
	                    break;
	                case SET:
	                    printf("SET\tclave: <%s>\tvalor: <%s>\n", parsed.argumentos.SET.clave, parsed.argumentos.SET.valor);
	                    break;
	                case STORE:
	                    printf("STORE\tclave: <%s>\n", parsed.argumentos.STORE.clave);
	                    break;
	                default:
	                    fprintf(stderr, "No pude interpretar <%s>\n", line);
	                    exit(EXIT_FAILURE);
	            }

	            destruir_operacion(parsed);
	        }

	        else
	        {
	            fprintf(stderr, "La linea <%s> no es valida\n", line);
	            exit(EXIT_FAILURE);
	        }

	    recibir_confirmacion(socketPlan);


	    fclose(fp);
	    if (line)
	        free(line);
*/


	log_destroy(logger);
	limpiar_configuracion();
	exit(1);
}
