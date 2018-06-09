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
	safe_send(socketCord, esi_id, sizeof(int));


	//todo descomentar esto despues:
	recibir_confirmacion(socketPlan); //signal para ejecutar

	while ((read = getline(&line, &len, fp)) != -1) {

		t_esi_operacion parsed = parse(line);

		if (parsed.valido) {

PROCESAR:switch (parsed.keyword) {
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
					exit_gracefully(1);

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
			key = recibir_cod_operacion(socketCord);
			switch(key){
			case BLOQUEO_ESI:
				log_info(logger, "ESI bloqueado por clave %s", parsed.argumentos.SET.clave);
				recibir_confirmacion(socketPlan);
				goto PROCESAR;

			}
			log_trace(logger, "Recibi mensaje de coordinador: %s", to_string_protocolo(key));
			enviar_cod_operacion(socketPlan, key);

			//Frees

			destruir_operacion(parsed);

		}
		else {
			log_error(logger, "La linea <%s> no es valida\n", line);
			enviar_cod_operacion(socketPlan, LINEA_INVALIDA);
			exit_gracefully(1);
		}

		recibir_confirmacion(socketPlan);

	}

	log_info(logger, "No quedan mas lineas en el archivo");

	enviar_cod_operacion(socketPlan, FINALIZO_ESI);

	fclose(fp);
	if (line)
		free(line);

	free(esi_id);

	log_destroy(logger);
	limpiar_configuracion();
	exit(1);
}
