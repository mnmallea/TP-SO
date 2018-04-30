/*
 * config_planificador.h
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */

#ifndef CONFIG_PLANIFICADOR_H_
#define CONFIG_PLANIFICADOR_H_

#include "algoritmos_planificacion.h"

typedef struct{
	tipo_algoritmo_planif algoritmo;

} config;

extern t_log *logger;

config configurar(char *ruta);

#endif /* CONFIG_PLANIFICADOR_H_ */
