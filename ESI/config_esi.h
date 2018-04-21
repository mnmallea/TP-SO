/*
 * config.h
 *
 *  Created on: 16 abr. 2018
 *      Author: utnso
 */

#ifndef CONFIG_ESI_H_
#define CONFIG_ESI_H_


typedef struct{
	char* ipCord;
	int portCord;
	char* ipPlan;
	int portPlan;
} config;

config configurar(char *ruta);

#endif /* CONFIG_H_ */
