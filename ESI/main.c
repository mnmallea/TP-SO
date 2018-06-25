#include "main.h"

#include <parsi/parser.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "../syntax-commons/conexiones.h"
#include "../syntax-commons/my_socket.h"

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

	int socketCord = conectarse_a_coordinador(configuracion.ipCoord,
			configuracion.portCoord, handshake_msg);
	int socketPlan = crear_socket_cliente(configuracion.ipPlan,
			configuracion.portPlan);

	int *esi_id = safe_recv(socketPlan, sizeof(int));
	log_debug(logger, "el id del esi es: %d", *esi_id);
	safe_send(socketCord, esi_id, sizeof(int));

	if ((n_esi_operacion = malloc(sizeof(t_operacion))) == NULL) {
		log_error(logger, "No se puede alocar memoria");
		exit_gracefully(1);
	}

	recibir_confirmacion(socketPlan); //signal para ejecutar
	log_trace(logger, "Recibi señal para ejecutar");

	while ((read = getline(&line, &len, fp)) != -1) {

		t_esi_operacion parsed = parse(line);

		if (parsed.valido) {

			PROCESAR: switch (parsed.keyword) {
			case GET:
				log_debug(logger, "Get %s", parsed.argumentos.SET.clave);
				enviar_get(socketCord, parsed.argumentos.GET.clave);
				break;

			case SET:
				log_debug(logger, "Set %s %s", parsed.argumentos.SET.clave,
						parsed.argumentos.SET.valor);
				if (strlen(parsed.argumentos.SET.valor) < 40) {

					enviar_set(socketCord, parsed.argumentos.SET.clave,
							parsed.argumentos.SET.valor);

				} else {
					log_error(logger,
							"El tamaño del valor <%s> es superior al permitido\n",
							line);
					enviar_cod_operacion(socketPlan, LINEA_SIZE);
					destruir_operacion(parsed);
					goto FREE;

				}
				break;

			case STORE:
				log_debug(logger, "Store %s", parsed.argumentos.STORE.clave);
				enviar_store(socketCord, parsed.argumentos.STORE.clave);
				break;

			default:
				log_error(logger, "No pude interpretar <%s>\n", line);
				enviar_cod_operacion(socketPlan, INTERPRETAR);
				destruir_operacion(parsed);
				goto FREE;
			}

			//Respuesta al planificador
			key = recibir_cod_operacion(socketCord);
			log_trace(logger, "Recibi mensaje de coordinador: %s",
					to_string_protocolo(key));

			if (enviar_cod_operacion(socketPlan, key) < 0) {
				log_error(logger, "Error de conexion con planificador");
				exit_gracefully(EXIT_FAILURE);
			}
			log_trace(logger, "Le envie al planificador: %s", to_string_protocolo(key));

			if (key == BLOQUEO_ESI) {
				log_info(logger, "ESI bloqueado por clave %s",
						parsed.argumentos.SET.clave);
				recibir_confirmacion(socketPlan);
				log_trace(logger, "Recibi señal para ejecutar");
				goto PROCESAR;

			}

			destruir_operacion(parsed);

		} else {
			log_error(logger, "La linea <%s> no es valida\n", line);
			if (enviar_cod_operacion(socketPlan, ABORTA) < 0) {
				log_error(logger, "Error de conexion con planificador");
				exit_gracefully(EXIT_FAILURE);
			}
			destruir_operacion(parsed);
			goto FREE;

		}

		recibir_confirmacion(socketPlan);
		log_trace(logger, "Recibi señal para ejecutar");
	}

	log_info(logger, "No quedan mas lineas en el archivo");

	enviar_cod_operacion(socketPlan, FINALIZO_ESI);
	enviar_cod_operacion(socketCord, FINALIZO_ESI);

	FREE: fclose(fp);
	if (line)
		free(line);

	free(esi_id);

	log_destroy(logger);
	limpiar_configuracion();
	exit(1);
}
