/*
 * config_planificador.h
 *
 *  Created on: 12 may. 2018
 *      Author: utnso
 */

#ifndef CONFIG_PLANIFICADOR_H_
#define CONFIG_PLANIFICADOR_H_

#include <stdlib.h>
#include <string.h>
#include <commons/config.h>
#include <commons/log.h>
#include <commons/string.h>
#include "error.h"
#include "algoritmos_planificacion.h"


#define PORT_MAX_STRING_LENGTH 5

typedef struct{
	char puerto[PORT_MAX_STRING_LENGTH + 1]; //el maximo es 65535 mas el '\0'
	int algoritmo;
	int estimacion_inicial;
	char *ipCoord;
	char *portCoord;
	char *claves_bloqueadas;
	int alfa;
} config;

extern t_log *logger;

config configurar(char *ruta);
void limpiar_configuracion();

#endif /* CONFIG_PLANIFICADOR_H_ */
