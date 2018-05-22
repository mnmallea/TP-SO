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

config configurar(char *ruta);
void limpiar_configuracion(void);

#endif /* CONFIG_H_ */
