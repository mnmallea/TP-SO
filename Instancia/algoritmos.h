/*
 * algoritmos.h
 *
 *  Created on: 14 jul. 2018
 *      Author: utnso
 */

#ifndef ALGORITMOS_H_
#define ALGORITMOS_H_

#include "cfg_almacenamiento.h"

int posicion;

void algoritmoCircular(claveEntrada* cv);
void algoritmoLRU(claveEntrada* cv);
void algoritmoBSU(claveEntrada* cv);

bool esAtomica(claveEntrada* cv);
bool listaNoContigua(t_list* unaLista);

extern t_log *logger;

#endif /* ALGORITMO_H_ */



#endif /* ALGORITMOS_H_ */
