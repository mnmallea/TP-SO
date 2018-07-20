/*
 * main.h
 *
 *  Created on: 12 may. 2018
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <commons/log.h>

#include "../syntax-commons/protocol.h"
#include "../syntax-commons/serializador.h"
#include "config_esi.h"


#define CANT_ARGUMENTOS_MAIN 3
#define LOG_LEVEL LOG_LEVEL_TRACE



void estadoOperacion(int socketCord,int socketPlan,int esi_id);

t_log* logger;

t_paquete *pkg_esi;

typedef struct
{
int keyword;
char *clave;
char *valor;
}t_operacion;

t_protocolo key;

char *obtenerNombre(char *path);

config configuracion; //no le pongan otro nombre, porque despues limpiar_configuracion() se va a encargar de borrarla


#endif /* MAIN_H_ */
