/*
 * config.h
 *
 *  Created on: 16 abr. 2018
 *      Author: utnso
 */

#ifndef CONFIG_ESI_H_
#define CONFIG_ESI_H_

#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>

typedef struct{
	char* ipCoord;
	char* portCoord;
	char* ipPlan;
	char* portPlan;
} config;

extern config configuracion;
extern t_log *logger;


config configurar(char *ruta);
void limpiar_configuracion(void);

#endif /* CONFIG_H_ */
