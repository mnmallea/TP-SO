/*
 * config.h
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#ifndef CONFIG_COORDINADOR_H_
#define CONFIG_COORDINADOR_H_

#include "algoritmos_distribucion.h"

extern t_log *logger;

typedef struct{
	int puerto;
	tipo_algoritmo_dist algoritmo;
	int cant_entradas;
	int entrada_size;
	int retardo;
} config;

config configurar(char *ruta);

#endif /* CONFIG_H_ */
