/*
 * planificador.c
 *
 *  Created on: 7 jun. 2018
 *      Author: utnso
 */

#include "planificador.h"

void consulta_de_clave(char* clave, t_protocolo tipo_consulta) {
	t_paquete* paquete = paquete_crear();
	paquete_agregar(paquete, clave, strlen(clave) + 1);
	if (paquete_enviar_con_codigo(paquete, tipo_consulta, socket_planificador)
			< 0) {
		log_error(logger, "Error de comunicacion con el planificador");
		//hay que liberar varias cosas
		exit(EXIT_FAILURE);
	}
}

void solicitar_clave(char* clave) {
	consulta_de_clave(clave, SOLICITUD_CLAVE);
}

void informar_liberacion_clave(char* clave){
	consulta_de_clave(clave, DESBLOQUEO_CLAVE);
}

void esi_tiene_clave(char* clave){
	consulta_de_clave(clave, ESI_TIENE_CLAVE);
}
