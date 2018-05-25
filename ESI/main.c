#include "main.h"

int main(int argc, char* argv[]) {
	FILE * fp;
	char * line = NULL;

	//size_t len = 0;
	//ssize_t read;

	size_t len = 0;
	ssize_t read;
	int semEsi = 0;

	logger = log_create("ESI.log", "ESI", true, LOG_LEVEL);

	if (argc != CANT_ARGUMENTOS_MAIN) {
		log_error(logger, "Cantidad incorrecta de parametros");
		exit_gracefully(1);
		return -1;
	}

	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		log_error(logger, "El archivo está vacio.");
		exit(EXIT_FAILURE);
	}

	configuracion = configurar(argv[2]);

	int socketCord = crear_socket_cliente(configuracion.portCord,
			configuracion.portCord);
	int socketPlan = crear_socket_cliente(configuracion.ipPlan,
			configuracion.portPlan);
	mandar_confirmacion(socketCord);
	mandar_confirmacion(socketPlan);

	while ((read = getline(&line, &len, fp)) != -1) {
		recibir_confirmacion(socketPlan);
		t_esi_operacion parsed = parse(line);

		if (parsed.valido) {
			switch (parsed.keyword) {
			case GET:
				parsed.argumentos.GET.clave;
				//mandar_info(socketCord,t_parsed); enviar al coordinador t_esi_operaciones
				break;
			case SET:
				parsed.argumentos.SET.clave;
				parsed.argumentos.SET.valor;
				//mandar_info(socketCord,t_operacion); enviar al coordinador t_esi_operaciones
				break;
			case STORE:
				parsed.argumentos.STORE.clave;
				//mandar_info(socketCord,t_parsed); enviar al coordinador t_esi_operaciones
				break;
			default:
				log_error(logger, "No pude interpretar <%s>\n", line);
				//enviar respuesta al planificador, error de linea(?)
				exit_gracefully(1);
			}

			destruir_operacion(parsed);
		} else {
			log_error(logger, "La linea <%s> no es valida\n", line);
			//enviar respuesta al planificador, error de linea(?)
			exit_gracefully(1);
		}
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
	/*
	 destruir_operacion(parsed);
	 //hace falta el wait?
	 //o con un recv(socketCord) para el ok o nook ya queda esperando?
	 if(recv

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
