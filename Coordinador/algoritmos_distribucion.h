/*
 * algoritmos_distribucion.h
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#ifndef ALGORITMOS_DISTRIBUCION_H_
#define ALGORITMOS_DISTRIBUCION_H_
#include <commons/collections/list.h>

typedef enum {LSU, EL, KE} tipo_algoritmo_dist;

void obtener_instancia_segun_EL(t_list* instancias);
void obtener_instancia_segun_LSU(t_list* instancias);
void obtener_instancia_segun_KE(t_list* instancias);


#endif /* ALGORITMOS_DISTRIBUCION_H_ */
