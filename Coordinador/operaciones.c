/*
 * operaciones.c
 *
 *  Created on: 7 jun. 2018
 *      Author: utnso
 */

#include <commons/log.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "../syntax-commons/conexiones.h"
#include "../syntax-commons/protocol.h"
#include "algoritmos_distribucion.h"
#include "instancia.h"
#include "log_operaciones.h"
#include "planificador.h"
#include "sincronizacion.h"
#include "typedefs.h"

void realizar_get(t_esi* esi, char* clave) {
	logear_get(esi->id, clave);
	solicitar_clave(clave);
	t_protocolo cod_op = recibir_cod_operacion(socket_planificador);
	switch (cod_op) {
	case ERROR: //en realidad es que la clave estaba ocupada
		return;
		break;
	case EXITO:
		log_info(logger, "Get realizado exitosamente");
		send(esi->socket, &cod_op, sizeof(t_protocolo), 0);
		break;
	default:
		if (cod_op < 0) {
			log_error(logger, "Error de conexion con el planificador");
			//todo self.morirse()
			exit(EXIT_FAILURE);
		}
		log_error(logger, "Respuesta del planificador desconocida");
	}
}

void realizar_set(t_esi* esi, char* clave, char* valor) {
	t_instancia* instancia_elegida;

	logear_set(esi->id, clave, valor);
	esi_tiene_clave(clave);
	t_protocolo cod_op = recibir_cod_operacion(socket_planificador);

	switch (cod_op) {
	case ERROR: //en realidad es que la clave estaba ocupada
		//todo aca habria que avisarle al esi que se bloqueo???
		return;
		break;
	case EXITO:
		instancia_elegida = obtener_instancia_siguiente(clave);
		log_debug(logger, "Instancia elegida: %s", instancia_elegida->nombre);
		if (enviar_set(instancia_elegida->socket, clave, valor) < 0) {
			log_error(logger, "Error al enviar set a instancia %s",
					instancia_elegida->nombre);
			instancia_desactivar(instancia_elegida);
			informar_instancia_caida(instancia_elegida);
			return;
		}
		t_protocolo respuesta_instancia = recibir_cod_operacion(
				instancia_elegida->socket);
		switch (respuesta_instancia) {
		case EXITO:
			log_info(logger, "Set realizado exitosamente en Instancia %s",
					instancia_elegida->nombre);
			send(esi->socket, &respuesta_instancia, sizeof(t_protocolo), 0);
			break;
		case ERROR:
			log_error(logger, "Error al realizar set en Instancia %s",
					instancia_elegida->nombre);
			send(esi->socket, &respuesta_instancia, sizeof(t_protocolo), 0);
			break;
		default:
			log_error(logger, "Error al recibir retorno de instancia %s",
					instancia_elegida->nombre);
			instancia_desactivar(instancia_elegida);
			informar_instancia_caida(instancia_elegida);
		}

		break;
	default:
		if (cod_op < 0) {
			log_error(logger, "Error de conexion con el planificador");
			//todo self.morirse()
			exit(EXIT_FAILURE);
		}
		log_error(logger, "Respuesta del planificador desconocida");
	}
}

void realizar_store(t_esi* esi, char* clave) {
	t_instancia* instancia_elegida;

	logear_store(esi->id, clave);
	esi_tiene_clave(clave);
	t_protocolo cod_op = recibir_cod_operacion(socket_planificador);

	switch (cod_op) {
	case EXITO:
		instancia_elegida = obtener_instancia_siguiente(clave);
		log_debug(logger, "Instancia elegida: %s", instancia_elegida->nombre);
		if (enviar_store(instancia_elegida->socket, clave) < 0) {
			log_error(logger, "Error al enviar set a instancia %s",
					instancia_elegida->nombre);
			instancia_desactivar(instancia_elegida);
			informar_instancia_caida(instancia_elegida);
			return;
		}
		t_protocolo respuesta_instancia = recibir_cod_operacion(
				instancia_elegida->socket);
		switch (respuesta_instancia) {
		case EXITO:
			log_info(logger, "Store realizado exitosamente en Instancia %s",
					instancia_elegida->nombre);
			send(esi->socket, &respuesta_instancia, sizeof(t_protocolo), 0);
			break;
		case ERROR:
			log_error(logger, "Error al realizar store en Instancia %s",
					instancia_elegida->nombre);
			send(esi->socket, &respuesta_instancia, sizeof(t_protocolo), 0);
			break;
		default:
			log_error(logger, "Error al recibir retorno de instancia %s",
					instancia_elegida->nombre);
			instancia_desactivar(instancia_elegida);
			informar_instancia_caida(instancia_elegida);
		}
		informar_liberacion_clave(clave);
		break;
	case ERROR:		//aca la clave esta bloqueada
		return;
		break;
	default:
		if (cod_op < 0) {
			log_error(logger, "Error de conexion con el planificador");
			//todo self.morirse()
			exit(EXIT_FAILURE);
		}
		log_error(logger, "Respuesta del planificador desconocida");
	}
}
