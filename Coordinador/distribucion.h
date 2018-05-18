/*
 * distribucion.h
 *
 *  Created on: 17 may. 2018
 *      Author: utnso
 */

#ifndef DISTRIBUCION_H_
#define DISTRIBUCION_H_

#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include "config_coordinador.h"
#include "algoritmos_distribucion.h"
#include "main.h"

/*
 * Nombre: nueva_instancia(t_instancia* inst)
 * Descripcion: agrega la instancia enviada a la lista de instancias disponibles
 */
void nueva_instancia(t_instancia* inst);

/*
 * Nombre: obtener_instancia_para_almacenar();
 * Descripcion: obtiene la instancia en donde se debera guardar el valor
 * a partir del algoritmo definido en la configuracion
 */
t_instancia *obtener_instancia_para_almacenar();

void instancia_desconectada (t_instancia* inst);
void instancia_reconectada (t_instancia* inst);



#endif /* DISTRIBUCION_H_ */
