/*
 * config_instancia.h
 *
 *  Created on: 29 abr. 2018
 *      Author: utnso
 */

#ifndef CONFIG_INSTANCIA_H_
#define CONFIG_INSTANCIA_H_


#include <stdlib.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>

typedef enum{CIRC, LRU, BSU} tipo_algoritmo;

typedef struct{
	char* ip_coordinador;
	char* puerto_coordinador;
	tipo_algoritmo algoritmo;
	char* punto_montaje;
	char* nombre_instancia;
	int intervalo_dump;
} config;

extern t_log *logger;
extern config configuracion;

config configurar(char* ruta);
tipo_algoritmo get_enum_algoritmo(char* nombre_algoritmo);
void limpiar_configuracion(void);

#endif /* CONFIG_INSTANCIA_H_ */
