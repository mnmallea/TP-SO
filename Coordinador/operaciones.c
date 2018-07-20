/*
 * operaciones.c
 *
 *  Created on: 7 jun. 2018
 *      Author: utnso
 */

#include <commons/log.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "../syntax-commons/conexiones.h"
#include "../syntax-commons/protocol.h"
#include "algoritmos_distribucion.h"
#include "error.h"
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
		enviar_cod_operacion(esi->socket, cod_op);
		return;
		break;
	case EXITO:
		log_info(logger, "[ESI %d] Get realizado exitosamente", esi->id);
		enviar_cod_operacion(esi->socket, cod_op);
		break;
	case MURIO_ESI_CORRIENDO:
		log_info(logger,
				"El ESI %d actual ha muerto mientras intentaba realizar una operacion",
				esi->id);
		enviar_cod_operacion(esi->socket, ABORTA);
		break;
	default:
		if (cod_op < 0) {
			exit_error_with_msg("Error de conexion con el planificador al realizar GET");
		}
		enviar_cod_operacion(esi->socket, ABORTA);
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
		enviar_cod_operacion(esi->socket, ABORTA);
		break;
	case CLAVE_NO_BLOQUEADA_EXCEPTION: //en realidad es que la clave estaba ocupada
	case CLAVE_NO_IDENTIFICADA_EXCEPTION:
		log_trace(logger, "[ESI %d] Enviando Aborta al ESI", esi->id);
		enviar_cod_operacion(esi->socket, ABORTA);
		return;
		break;
	case EXITO:
		;// Caso en que la clave la tenga una instancia previamente caida
		t_instancia* instancia_caida_con_clave = instancia_inactiva_con_clave(
				clave);
		if (instancia_caida_con_clave != NULL) {
			log_error(logger,
					"La instancia %s que tenia la clave %s no está disponible",
					instancia_caida_con_clave->nombre, clave);

			remover_clave_almacenada(instancia_caida_con_clave, clave);
			enviar_cod_operacion(esi->socket, INSTANCIA_CAIDA_EXCEPTION);
			return;
		}

		// Caso en que la clave la tenga una instancia que esta disponible (hasta el momento)
		ELEGIR_INSTANCIA: instancia_elegida = instancia_disponible_con_clave(
				clave);
		bool instancia_tenia_clave;
		if (instancia_elegida != NULL) {
			instancia_tenia_clave = true;
			log_info(logger, "La instancia %s ya tenía la clave %s",
					instancia_elegida->nombre, clave);
		} else {
			instancia_elegida = obtener_instancia_siguiente(clave);
			instancia_tenia_clave = false;
		}
		log_debug(logger, "Instancia elegida: %s", instancia_elegida->nombre);

		/*
		 * Comienza la comunicación con la instancia elegida
		 */
		pthread_mutex_lock(&instancia_elegida->mutex_comunicacion);
		if (enviar_set(instancia_elegida->socket, clave, valor) < 0) {
			log_error(logger, "Error al enviar set a instancia %s",
					instancia_elegida->nombre);
			instancia_desactivar_y_post(instancia_elegida);
			if (instancia_tenia_clave) {
				enviar_cod_operacion(esi->socket, INSTANCIA_CAIDA_EXCEPTION);
				return;
			} else {
				log_info(logger,
						"La instancia %s se cayó, se elegirá otra instancia ya que esta aún no tenía la clave",
						instancia_elegida->nombre);
				goto ELEGIR_INSTANCIA;
			}

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

			pthread_mutex_unlock(&instancia_elegida->mutex_comunicacion);
			realizar_compactacion();
			pthread_mutex_lock(&instancia_elegida->mutex_comunicacion);

			int resultado_compactacion = set_tras_compactacion(
					instancia_elegida, clave, valor);
			if (resultado_compactacion) {
				enviar_cod_operacion(esi->socket, ABORTA);
				instancia_desactivar_y_post(instancia_elegida);
				return;
			}
			enviar_cod_operacion(esi->socket, EXITO);
			break;
		default:
			log_error(logger,
					"[ESI %d] Error al recibir retorno de instancia %s",
					esi->id, instancia_elegida->nombre);
			instancia_desactivar_y_post(instancia_elegida);
			if (instancia_tenia_clave) {
				enviar_cod_operacion(esi->socket, INSTANCIA_CAIDA_EXCEPTION);
				return; //no borrar esto
			} else {
				log_info(logger,
						"La instancia %s se cayó, se elegirá otra instancia ya que esta aún no tenía la clave",
						instancia_elegida->nombre);
				goto ELEGIR_INSTANCIA;
			}
		}
		pthread_mutex_unlock(&instancia_elegida->mutex_comunicacion);

		break;
	default:
		if (cod_op < 0) {
			exit_error_with_msg("Error de conexion con el planificador");
		}
		enviar_cod_operacion(esi->socket, ABORTA);
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
				"SET %s %s exitoso tras la compactacion en instancia %s", clave,
				valor, instancia->nombre);
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
		enviar_cod_operacion(esi->socket, ABORTA);
		break;
	case EXITO:
		informar_liberacion_clave(clave); //Se le informa al planificador que la clave debe ser desbloqueada

		/*
		 * Se busca a la instancia que tiene la clave en el sistema
		 */

		t_instancia* instancia_muerta = instancia_inactiva_con_clave(clave);
		if (instancia_muerta != NULL) {
			log_error(logger,
					"[ESI %d] La instancia %s tenía la clave \"%s\", pero ya no esta disponible en el sistema",
					esi->id, instancia_muerta->nombre, clave);
			remover_clave_almacenada(instancia_muerta, clave);
			enviar_cod_operacion(esi->socket, INSTANCIA_CAIDA_EXCEPTION);
			return;
		}

		instancia_elegida = instancia_disponible_con_clave(clave);
		if (instancia_elegida == NULL) {
			log_error(logger,
					"[ESI %d] La clave \"%s\" no está presente en ninguna instancia del sistema",
					esi->id, clave);
			enviar_cod_operacion(esi->socket, ABORTA);
			return;
		}
		log_debug(logger, "Instancia elegida: %s,ya que tiene la clave \"%s\"",
				instancia_elegida->nombre, clave);
		/*
		 * Comienza comunicacion con la instancia
		 */

		pthread_mutex_lock(&instancia_elegida->mutex_comunicacion);
		if (enviar_store(instancia_elegida->socket, clave) < 0) {
			log_error(logger, "Error al enviar set a instancia %s",
					instancia_elegida->nombre);
			instancia_desactivar_y_post(instancia_elegida);
			remover_clave_almacenada(instancia_elegida, clave);
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
			instancia_desactivar_y_post(instancia_elegida);
			enviar_cod_operacion(esi->socket, INSTANCIA_CAIDA_EXCEPTION);
			return; //importante
		}

		pthread_mutex_unlock(&instancia_elegida->mutex_comunicacion);
		break;
	case CLAVE_NO_BLOQUEADA_EXCEPTION:		//aca la clave esta bloqueada
	case CLAVE_NO_IDENTIFICADA_EXCEPTION:
		enviar_cod_operacion(esi->socket, ABORTA);
		return;
		break;
	default:
		if (cod_op < 0) {
			exit_error_with_msg("Error de conexion con el planificador");
		}
		enviar_cod_operacion(esi->socket, ABORTA);
		log_error(logger, "Respuesta del planificador desconocida");
	}
}
