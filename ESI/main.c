#include "main.h"

int main(int argc, char* argv[]){
	FILE * fp;
	char * line = NULL;

	size_t len = 0;
	ssize_t read;
	t_operacion oper;

	logger = log_create("ESI.log","ESI",true,LOG_LEVEL);

	if(argc != CANT_ARGUMENTOS_MAIN){
			log_error(logger, "Cantidad incorrecta de parametros");
			exit_gracefully(1);
		return -1;
	}

	fp = fopen(argv[1], "r");
		    if (fp == NULL){
		    	log_error(logger, "El archivo está vacio.");
		        exit(EXIT_FAILURE);
		    }

	configuracion = configurar(argv[2]);

	int socketCord= crear_socket_cliente(configuracion.portCord,configuracion.portCord);
	int socketPlan= crear_socket_cliente(configuracion.ipPlan,configuracion.portPlan);

	mandar_confirmacion(socketPlan);
	int id=recibir_mensaje(socketPlan);
	mandar_mensaje(socketCord,id);
	recibir_confirmacion(socketPlan);

	while ((read = getline(&line, &len, fp)) != -1)
	{

		t_esi_operacion parsed = parse(line);
		t_operacion *n_esi_operacion;

		if((n_esi_operacion=malloc(sizeof(t_operacion)))==NULL){
			log_error(logger, "No se puede alocar memoria");
				            	//enviar respuesta al planificador, error de linea(?)
			exit_gracefully(1);
		}

		if(parsed.valido)
	    {
			pkg_esi=crearPaquete();
	    	switch(parsed.keyword){
	        	case GET:
	        		n_esi_operacion->keyword=0;
	        		strcpy(n_esi_operacion->clave,parsed.argumentos.GET.clave);
	        		break;
	            case SET:
	            	if(strlen(parsed.argumentos.SET.valor)<40){
	            	n_esi_operacion->keyword=1;
	            	strcpy(n_esi_operacion->clave,parsed.argumentos.SET.clave);
	            	strcpy(n_esi_operacion->valor,parsed.argumentos.SET.valor);

	            	}
	            	else{
	            		log_error(logger, "El tamaño del valor <%s> es superior al permitido\n", line);
	            		exit_gracefully(1);
	            		//error valor mayor a 40
	            	}

	            	break;
	            case STORE:
	            	n_esi_operacion->keyword=2;
	            	strcpy(n_esi_operacion->clave,parsed.argumentos.STORE.clave);

	            	break;
	            default:
	            	log_error(logger, "No pude interpretar <%s>\n", line);
	            	//enviar respuesta al planificador, error de linea(?)
	            	exit_gracefully(1);
	    	}

	    	agregar(pkg_esi,n_esi_operacion,sizeof(n_operacion));
       		eviarPaquete(socketCord,pkg_esi);


	    	if(recibir_mensaje(socketCord)==1)
	    		mandar_confirmacion(socketPlan);
	    	else
	    		mandar_error(socketPlan);

	    	destruirPaquete(pkg_esi);
	    	destruir_operacion(parsed);
	    }
	    else
	    {
	    	log_error(logger, "La linea <%s> no es valida\n", line);
	    	//enviar respuesta al planificador, error de linea(?)
	    	exit_gracefully(1);
	    }
	    recibir_confirmacion(socketPlan);
	}
  
/*
  destruir_operacion(parsed);
	        	//hace falta el wait?
	        	//o con un recv(socketCord) para el ok o nook ya queda esperando?
  */

log_info(logger, "No quedan mas lineas en el archivo");
	//enviar respuesta al planificador , no habia mas lineas(?)

	fclose(fp);
	if (line)
		free(line);

  log_destroy(logger);
	limpiar_configuracion();
	exit(1);
}
