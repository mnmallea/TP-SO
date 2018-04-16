/*
 * config.h
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#ifndef CONFIG_COORDINADOR_H_
#define CONFIG_COORDINADOR_H_

#include "algoritmos_distribucion.h"

typedef struct{
	char* puerto;
	tipo_algoritmo_dist algoritmo;
	int cant_entradas;
	int entrada_size;
	int retardo;
} config;

extern t_log *logger;

config* configuar(char *ruta);
void eliminar_configuracion(config* configuracion);

#endif /* CONFIG_H_ */
