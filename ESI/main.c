#include "main.h"

int main(int argc, char* argv[]) {
	FILE * fp;
	char * line = NULL;

	size_t len = 0;
	ssize_t read;
	t_operacion *n_esi_operacion;

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
	int handshake_msg = ESI;

	int socketCord = conectarse_a_coordinador(configuracion.ipPlan,
			configuracion.portCoord, handshake_msg);
	int socketPlan = crear_socket_cliente(configuracion.ipPlan,
			configuracion.portPlan);

	int *esi_id = safe_recv(socketPlan, sizeof(int)); //hay q hacer free
	log_debug(logger, "el id del esi es: %d", *esi_id);
	//int esi_id=recibir_mensaje(socketPlan);
	safe_send(socketCord, esi_id, sizeof(int));

	if ((n_esi_operacion = malloc(sizeof(t_operacion))) == NULL) {
		log_error(logger, "No se puede alocar memoria");
		//enviar respuesta al planificador, error de linea(?)
		exit_gracefully(1);
	}

	//todo descomentar esto despues:
//	recibir_confirmacion(socketPlan); //signal para ejecutar

	while ((read = getline(&line, &len, fp)) != -1) {

		size_t s_valor = 0;
		size_t s_clave = 0;
		size_t s_carga = 0;
		t_esi_operacion parsed = parse(line);

		if (parsed.valido) {
//			pkg_esi = crearPaquete();
//			log_debug(logger, "Crea pkg");

			switch (parsed.keyword) {
			case GET:
				log_debug(logger, "Get %s", parsed.argumentos.SET.clave);
				enviar_get(socketCord, parsed.argumentos.GET.clave);
				break;

			case SET:
				log_debug(logger, "Set %s %s", parsed.argumentos.SET.clave, parsed.argumentos.SET.valor);
				if (strlen(parsed.argumentos.SET.valor) < 40) {

					enviar_set(socketCord, parsed.argumentos.SET.clave,
							parsed.argumentos.SET.valor);

				} else {
					log_error(logger,
							"El tamaño del valor <%s> es superior al permitido\n",
							line);
					exit_gracefully(1);
					//error valor mayor a 40
				}
				break;

			case STORE:
				log_debug(logger, "Store %s", parsed.argumentos.STORE.clave);
				enviar_store(socketCord, parsed.argumentos.STORE.clave);
				break;

			default:
				log_error(logger, "No pude interpretar <%s>\n", line);
				//enviar respuesta al planificador, error de linea(?)
				exit_gracefully(1);
			}

			//Respuesta al planificador
			//key = recibir_mensaje(socketCord);
			//mandar_mensaje(socketPlan,key);

//			//Frees
//			free(paqueteProcesado);
//			destruirPaquete(pkg_esi);
			destruir_operacion(parsed);

		} else {
			log_error(logger, "La linea <%s> no es valida\n", line);
			//enviar respuesta al planificador, error de linea(?)
			exit_gracefully(1);
		}
//		recibir_confirmacion(socketPlan); todo descomentar esto tambien
	}

	log_info(logger, "No quedan mas lineas en el archivo");
	//enviar respuesta al planificador , no habia mas lineas(?)

	fclose(fp);
	if (line)
		free(line);

	free(esi_id);

	log_destroy(logger);
	limpiar_configuracion();
	exit(1);
}

