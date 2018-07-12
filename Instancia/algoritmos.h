/*
 * algoritmos.h
 *
 *  Created on: 4 jul. 2018
 *      Author: utnso
 */

#ifndef ALGORITMOS_H_
#define ALGORITMOS_H_


#include "tabla_entradas.h"
#include "cfg_almacenamiento.h"
t_list* algoritmoCircular(claveEntrada* cv,t_list* posiblesAReemplazar);
t_list* algoritmoLRU(claveEntrada* cv);
t_list* algoritmoBSU(claveEntrada* cv);
#endif /* ALGORITMOS_H_ */
