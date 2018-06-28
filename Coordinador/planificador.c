/*
 * planificador.c
 *
 *  Created on: 7 jun. 2018
 *      Author: utnso
 */

#include "planificador.h"

#include <stdlib.h>
#include <string.h>

#include "../syntax-commons/my_socket.h"
#include "../syntax-commons/serializador.h"
#include "algoritmos_distribucion.h"
#include "instancia.h"

void consulta_de_clave(char* clave, t_protocolo tipo_consulta) {
	t_paquete* paquete = paquete_crear();
	paquete_agregar(paquete, clave, strlen(clave) + 1);
	if (paquete_enviar_con_codigo(paquete, tipo_consulta, socket_planificador)
			< 0) {
		log_error(logger, "Error de comunicacion con el planificador");
		paquete_destruir(paquete);
		//hay que liberar varias cosas
		exit(EXIT_FAILURE);
	}
	paquete_destruir(paquete);
}

void solicitar_clave(char* clave) {
	consulta_de_clave(clave, SOLICITUD_CLAVE);
}

void informar_liberacion_clave(char* clave) {
	consulta_de_clave(clave, DESBLOQUEO_CLAVE);
}

void esi_tiene_clave(char* clave) {
	consulta_de_clave(clave, ESI_TIENE_CLAVE);
}

void informar_instancia_caida(t_instancia* instancia) {
	t_protocolo msg = INSTANCIA_CAIDA_EXCEPTION;
	safe_send(socket_planificador, &msg, sizeof(msg));
}

void agregar_status_a_paquete(t_paquete* paquete, t_status_clave status) {
	paquete_agregar_sin_tamanio(paquete, &status, sizeof(status));
}

void informar_status_clave(char* clave) {
	t_paquete* paquete = paquete_crear();

	t_instancia* instancia = instancia_con_clave(clave);

	if (instancia != NULL) {
		char* valor;
		t_status_clave estado_instancia = instancia_solicitar_valor_de_clave(
				instancia, clave, &valor);

		switch (estado_instancia) {
		case INSTANCIA_CAIDA:
		case INSTANCIA_NO_TIENE_CLAVE:
			agregar_status_a_paquete(paquete, NO_HAY_VALOR);
			break;
		case INSTANCIA_OK:
			agregar_status_a_paquete(paquete, HAY_VALOR);
			paquete_agregar(paquete, valor, strlen(valor) + 1);
			break;
		default:
			;
			// esto nunca deberia pasar
		}

		agregar_status_a_paquete(paquete, estado_instancia);
		paquete_agregar(paquete, instancia->nombre,
				strlen(instancia->nombre) + 1);
		agregar_status_a_paquete(paquete, NO_HAY_SIMULACION);
	} else {
		agregar_status_a_paquete(paquete, NO_HAY_VALOR);
		agregar_status_a_paquete(paquete, INSTANCIA_NO_ASIGNADA);
		instancia = simular_algoritmo(clave);
		if (instancia != NULL) {
			agregar_status_a_paquete(paquete, HAY_SIMULACION);
			paquete_agregar(paquete, instancia->nombre,
					strlen(instancia->nombre) + 1);
		}else{
			agregar_status_a_paquete(paquete, NO_HAY_SIMULACION);
		}

	}
	log_info(logger, "Enviando paquete de status clave al Planificador");
	paquete_enviar_safe(paquete, socket_planificador);
	log_info(logger, "Paquete enviado");
	paquete_destruir(paquete);

}
