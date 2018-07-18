/*
 * algoritmos.h
 *
 *  Created on: 14 jul. 2018
 *      Author: utnso
 */

#ifndef ALGORITMOS_H_
#define ALGORITMOS_H_

#include <commons/collections/list.h>
#include <commons/log.h>
#include <stdbool.h>

#include "config_instancia.h"
#include "instancia.h"
#include "tabla_entradas.h"

extern config configuracion;

int posicion;

t_resultado_set ReemplazarSegunAlgoritmo(claveEntrada* cv);
tablaE* obtener_siguiente_entrada_segun_algoritmo();

tablaE* obtener_siguiente_entrada_circular();
int obtener_indice_de_entrada(t_list* lista_de_tablaE, int posicion);

bool atomica_masGrande(void* unaEntrada);
tablaE* primera_entrada_mas_grande();
tablaE* primera_entrada_masGrande_desde(int posicion_desde);
tablaE* obtener_siguiente_entrada_bsu();
unsigned int tamanio_entrada_mas_grande();
tablaE* primera_entrada_atomica_desde(int posicion_desde);

unsigned int operacion_mas_vieja();
bool mas_vieja(void* una_entrada);
tablaE* primera_entrada_mas_vieja();
tablaE* primera_entrada_mas_vieja_desde(int posicion_desde);
tablaE* obtener_siguiente_entrada_lru();
unsigned int operacion_mas_vieja();

bool esAtomica(void* cv);
int cantidad_entradas_atomicas();
tablaE* primera_entrada_atomica();
void liberar_entrada(tablaE*);

extern t_log *logger;

#endif /* ALGORITMOS_H_ */
