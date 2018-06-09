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
	log_trace(logger, "Mensaje recibido del planificador: %s",
			to_string_protocolo(cod_op));
	switch (cod_op) {
	case BLOQUEO_ESI: //en realidad es que la clave estaba ocupada
		log_info(logger, "Clave ocupada %s", clave);
		send(esi->socket, &cod_op, sizeof(t_protocolo), 0);
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
	log_trace(logger, "Mensaje recibido del planificador: %s",
			to_string_protocolo(cod_op));
	switch (cod_op) {
	case CLAVE_NO_BLOQUEADA_EXCEPTION: //en realidad es que la clave estaba ocupada
	case CLAVE_NO_IDENTIFICADA_EXCEPTION:
		log_trace(logger, "Enviando Aborta al ESI");
		enviar_cod_operacion(esi->socket, ABORTA);
		return;
		break;
	case EXITO:
		instancia_elegida = obtener_instancia_siguiente(clave);
		log_debug(logger, "Instancia elegida: %s", instancia_elegida->nombre);
		if (enviar_set(instancia_elegida->socket, clave, valor) < 0) {
			log_error(logger, "Error al enviar set a instancia %s",
					instancia_elegida->nombre);
			instancia_desactivar(instancia_elegida);

			enviar_cod_operacion(esi->socket, INSTANCIA_CAIDA_EXCEPTION);
//			informar_instancia_caida(instancia_elegida);
			return;
		}
		t_protocolo respuesta_instancia = recibir_cod_operacion(
				instancia_elegida->socket);
//		t_protocolo respuesta_instancia = EXITO;
		switch (respuesta_instancia) {
		case EXITO:
			log_info(logger, "Set realizado exitosamente en Instancia %s",
					instancia_elegida->nombre);
			agregar_clave_almacenada(instancia_elegida, clave);
			enviar_cod_operacion(esi->socket, EXITO);
			break;
		case ERROR:
			log_error(logger, "Error al realizar set en Instancia %s",
					instancia_elegida->nombre);
			enviar_cod_operacion(esi->socket, ABORTA);
			break;
		default:
			log_error(logger, "Error al recibir retorno de instancia %s",
					instancia_elegida->nombre);
			enviar_cod_operacion(esi->socket, INSTANCIA_CAIDA_EXCEPTION);
			instancia_desactivar(instancia_elegida);
//			informar_instancia_caida(instancia_elegida);
		}

		break;
	default:
		if (cod_op < 0) {
			log_error(logger, "Error de conexion con el planificador");
			//todo self.morirse()
			exit(EXIT_FAILURE);
		}
		log_error(logger, "Respuesta del planificador desconocida");
		//todo no se que haria en este caso
	}
}

void realizar_store(t_esi* esi, char* clave) {
	t_instancia* instancia_elegida;

	logear_store(esi->id, clave);
	esi_tiene_clave(clave);
	t_protocolo cod_op = recibir_cod_operacion(socket_planificador);
	log_trace(logger, "Mensaje recibido del planificador: %s",
			to_string_protocolo(cod_op));
	switch (cod_op) {
	case EXITO:
		instancia_elegida = instancia_con_clave(clave);
		if (instancia_elegida == NULL) {
			log_error(logger,
					"La instancia que tiene la clave %s no esta disponible en el sistema",
					clave);
			enviar_cod_operacion(esi->socket, INSTANCIA_CAIDA_EXCEPTION);
		}
		log_debug(logger, "Instancia elegida: %s", instancia_elegida->nombre);
		if (enviar_store(instancia_elegida->socket, clave) < 0) {
			log_error(logger, "Error al enviar set a instancia %s",
					instancia_elegida->nombre);
			instancia_desactivar(instancia_elegida);
			enviar_cod_operacion(esi->socket, INSTANCIA_CAIDA_EXCEPTION);
//			informar_instancia_caida(instancia_elegida);
			return;
		}
		t_protocolo respuesta_instancia = recibir_cod_operacion(
				instancia_elegida->socket);
//		t_protocolo respuesta_instancia = EXITO;
		switch (respuesta_instancia) {
		case EXITO:
			log_info(logger, "Store realizado exitosamente en Instancia %s",
					instancia_elegida->nombre);
			enviar_cod_operacion(esi->socket, EXITO);
			break;
		case ERROR:
			log_error(logger, "Error al realizar store en Instancia %s",
					instancia_elegida->nombre);
			enviar_cod_operacion(esi->socket, ABORTA);
			break;
		default:
			log_error(logger, "Error al recibir retorno de instancia %s",
					instancia_elegida->nombre);
			instancia_desactivar(instancia_elegida);
			enviar_cod_operacion(esi->socket, INSTANCIA_CAIDA_EXCEPTION);
//			informar_instancia_caida(instancia_elegida);
		}
		informar_liberacion_clave(clave);
		break;
	case CLAVE_NO_BLOQUEADA_EXCEPTION:		//aca la clave esta bloqueada
	case CLAVE_NO_IDENTIFICADA_EXCEPTION:
		enviar_cod_operacion(esi->socket, ABORTA);
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
