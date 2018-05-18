/*
 * config.h
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#ifndef CONFIG_COORDINADOR_H_
#define CONFIG_COORDINADOR_H_

#define PORT_MAX_STRING_LENGTH 5


#include "main.h"
#include "algoritmos_distribucion.h"



typedef struct{
	char puerto[PORT_MAX_STRING_LENGTH + 1]; //el maximo es 65535 mas el '\0'
	tipo_algoritmo_dist algoritmo;
	int cant_entradas;
	int entrada_size;
	int retardo;
} config;

extern t_log *logger;

config configurar(char *ruta);
void limpiar_configuracion(void);

#endif /* CONFIG_H_ */
