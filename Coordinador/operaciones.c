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

int set_tras_compactacion(t_instancia* instancia, char* clave, char* valor);

void realizar_get(t_esi* esi, char* clave) {
	logear_get(esi->id, clave);
	solicitar_clave(clave, esi);

	sem_wait(&planificador_respondio);
	t_protocolo cod_op = respuesta_planificador;
	log_trace(logger, "[ESI %d] Mensaje recibido del planificador: %s", esi->id,
			to_string_protocolo(cod_op));
	switch (cod_op) {
	case BLOQUEO_ESI: //en realidad es que la clave estaba ocupada
		log_info(logger, "[ESI %d] Clave ocupada %s", esi->id, clave);
		send(esi->socket, &cod_op, sizeof(t_protocolo), 0);
		return;
		break;
	case EXITO:
		log_info(logger, "[ESI %d] Get realizado exitosamente", esi->id);
		send(esi->socket, &cod_op, sizeof(t_protocolo), 0);
		break;
	case MURIO_ESI_CORRIENDO:
		log_info(logger,
				"El ESI %d actual ha muerto mientras intentaba realizar una operacion",
				esi->id);
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

void instancia_actualizar_entradas_libres(t_instancia* instancia) {
	int entradas_libres;
	recv(instancia->socket, &entradas_libres, sizeof(entradas_libres),
	MSG_WAITALL);
	instancia->cant_entradas_vacias = entradas_libres;
	log_info(logger, "A la instancia %s le quedan %d entradas libres",
			instancia->nombre, instancia->cant_entradas_vacias);
}

void realizar_set(t_esi* esi, char* clave, char* valor) {
	t_instancia* instancia_elegida;

	logear_set(esi->id, clave, valor);
	esi_tiene_clave(clave, esi);

	sem_wait(&planificador_respondio);
	t_protocolo cod_op = respuesta_planificador;
	log_trace(logger, "[ESI %d] Mensaje recibido del planificador: %s", esi->id,
			to_string_protocolo(cod_op));

	/*
	 * Recibe la respuesta del planificador
	 */
	switch (cod_op) {
	case MURIO_ESI_CORRIENDO:
		log_info(logger,
				"El ESI %d actual ha muerto mientras intentaba realizar una operacion",
				esi->id);
		break;
	case CLAVE_NO_BLOQUEADA_EXCEPTION: //en realidad es que la clave estaba ocupada
	case CLAVE_NO_IDENTIFICADA_EXCEPTION:
		log_trace(logger, "[ESI %d] Enviando Aborta al ESI", esi->id);
		enviar_cod_operacion(esi->socket, ABORTA);
		return;
		break;
	case EXITO:
		instancia_elegida = obtener_instancia_siguiente(clave);
		log_debug(logger, "Instancia elegida: %s", instancia_elegida->nombre);
		if (enviar_set(instancia_elegida->socket, clave, valor) < 0) {
			log_error(logger, "Error al enviar set a instancia %s",
					instancia_elegida->nombre);
			instancia_desactivar(instancia_elegida->nombre);

			enviar_cod_operacion(esi->socket, INSTANCIA_CAIDA_EXCEPTION);

			return;
		}
		t_protocolo respuesta_instancia = recibir_cod_operacion(
				instancia_elegida->socket);

		/*
		 * Recibe la respuesta de la instancia
		 */
		switch (respuesta_instancia) {
		case EXITO:
			log_info(logger,
					"[ESI %d] Set realizado exitosamente en Instancia %s",
					esi->id, instancia_elegida->nombre);
			agregar_clave_almacenada(instancia_elegida, clave);

			instancia_actualizar_entradas_libres(instancia_elegida);
			enviar_cod_operacion(esi->socket, EXITO);
			break;
		case ERROR:
			log_error(logger, "[ESI %d] Error al realizar set en Instancia %s",
					esi->id, instancia_elegida->nombre);
			enviar_cod_operacion(esi->socket, ABORTA);
			remover_clave_almacenada(instancia_elegida, clave);
			break;
		case SOLICITUD_COMPACTACION:
			log_info(logger, "La instancia %s requiere compactación",
					instancia_elegida->nombre);
			realizar_compactacion();
			int resultado_compactacion = set_tras_compactacion(
					instancia_elegida, clave, valor);
			if(resultado_compactacion){
				enviar_cod_operacion(esi->socket, ABORTA);
				instancia_desactivar(instancia_elegida->nombre);
				return;
			}
			enviar_cod_operacion(esi->socket, EXITO);
			break;
		default:
			log_error(logger,
					"[ESI %d] Error al recibir retorno de instancia %s",
					esi->id, instancia_elegida->nombre);
			enviar_cod_operacion(esi->socket, INSTANCIA_CAIDA_EXCEPTION);
			instancia_desactivar(instancia_elegida->nombre);

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

/*
 * Devuelve valor negativo si falla
 */
int set_tras_compactacion(t_instancia* instancia, char* clave, char* valor) {
	if (enviar_set(instancia->socket, clave, valor) < 0) {
		log_error(logger, "Error al enviar set a instancia %s",
				instancia->nombre);
		instancia_desactivar(instancia->nombre);
		log_warning(logger, "La instancia %s se ha caido tras la compactacion",
				instancia->nombre);
		return -1;
	}
	t_protocolo respuesta_instancia = recibir_cod_operacion(instancia->socket);

	/*
	 * Recibe la respuesta de la instancia
	 */
	switch (respuesta_instancia) {
	case EXITO:
		agregar_clave_almacenada(instancia, clave);
		instancia_actualizar_entradas_libres(instancia);
		log_debug(logger,
				"SET %s %s exitoso tras la compactacion en instancia %s",
				instancia->nombre);
		return 0;
		break;
	case SOLICITUD_COMPACTACION:
		log_warning(logger, "La instancia %s ha solicitado compactar otra vez",
				instancia->nombre); // @suppress("No break at end of case")
	default:
		log_error(logger, "Error al realizar el set en la instancia %s",
				instancia->nombre);
		return -1;
	}

}

void realizar_store(t_esi* esi, char* clave) {
	t_instancia* instancia_elegida;

	logear_store(esi->id, clave);
	esi_tiene_clave(clave, esi);

	sem_wait(&planificador_respondio);
	t_protocolo cod_op = respuesta_planificador;
	log_trace(logger, "[ESI %d] Mensaje recibido del planificador: %s", esi->id,
			to_string_protocolo(cod_op));

	/*
	 * Interpreta la respuesta del planificador
	 */
	switch (cod_op) {
	case MURIO_ESI_CORRIENDO:
		log_info(logger,
				"El ESI %d actual ha muerto mientras intentaba realizar una operacion",
				esi->id);
		break;
	case EXITO:
		instancia_elegida = instancia_con_clave(clave);
		if (instancia_elegida == NULL) {
			log_error(logger,
					"[ESI %d] La instancia que tiene la clave %s no esta disponible en el sistema",
					esi->id, clave);
			enviar_cod_operacion(esi->socket, INSTANCIA_CAIDA_EXCEPTION);
		}
		log_debug(logger, "Instancia elegida: %s,ya que tiene la clave %s",
				instancia_elegida->nombre, clave);
		if (enviar_store(instancia_elegida->socket, clave) < 0) {
			log_error(logger, "Error al enviar set a instancia %s",
					instancia_elegida->nombre);
			instancia_desactivar(instancia_elegida->nombre);

			enviar_cod_operacion(esi->socket, INSTANCIA_CAIDA_EXCEPTION);

			return;
		}
		t_protocolo respuesta_instancia = recibir_cod_operacion(
				instancia_elegida->socket);

		/*
		 * Interpreta la respuesta de la instancia
		 */
		switch (respuesta_instancia) {
		case EXITO:
			log_info(logger,
					"[ESI %d] Store realizado exitosamente en Instancia %s",
					esi->id, instancia_elegida->nombre);
			instancia_actualizar_entradas_libres(instancia_elegida);
			enviar_cod_operacion(esi->socket, EXITO);
			break;
		case ERROR:
			log_error(logger,
					"[ESI %d] Error al realizar store en Instancia %s", esi->id,
					instancia_elegida->nombre);
			remover_clave_almacenada(instancia_elegida, clave);
			enviar_cod_operacion(esi->socket, ABORTA);
			break;
		default:
			log_error(logger,
					"[ESI %d] Error al recibir retorno de instancia %s",
					esi->id, instancia_elegida->nombre);
			instancia_desactivar(instancia_elegida->nombre);
			enviar_cod_operacion(esi->socket, INSTANCIA_CAIDA_EXCEPTION);

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
