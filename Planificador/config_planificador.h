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
#include "typedefs.h"


extern t_log *logger;

config configurar(char *ruta);

void limpiar_configuracion();
tipo_algoritmo_planif get_algoritmo_planificacion(char* nombre_algoritmo);

#endif /* CONFIG_PLANIFICADOR_H_ */
