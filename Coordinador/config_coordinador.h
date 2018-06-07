/*
 * config.h
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#ifndef CONFIG_COORDINADOR_H_
#define CONFIG_COORDINADOR_H_

#include "typedefs.h"
#include <commons/log.h>
#include "algoritmos_distribucion.h"


extern t_log *logger;
extern config configuracion;

void configurar(char *ruta);
void limpiar_configuracion(void);
t_algoritmo get_algoritmo_distribucion(char*);

#endif /* CONFIG_H_ */
