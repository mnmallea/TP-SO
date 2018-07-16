/*
 * algoritmos.h
 *
 *  Created on: 14 jul. 2018
 *      Author: utnso
 */

#ifndef ALGORITMOS_H_
#define ALGORITMOS_H_

#include "cfg_almacenamiento.h"
#include "tabla_entradas.h"
#include <commons/collections/list.h>
#include "instancia.h"


int posicion;

void algoritmoCircular(claveEntrada* cv);
void algoritmoLRU(claveEntrada* cv);
void algoritmoBSU(claveEntrada* cv);

bool esAtomica(void* cv);
bool listaNoContigua(t_list* unaLista);

tablaE* obtener_siguiente_entrada_circular();

extern t_log *logger;


#endif /* ALGORITMOS_H_ */
