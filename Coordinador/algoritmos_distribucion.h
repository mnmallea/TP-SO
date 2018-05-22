/*
 * algoritmos_distribucion.h
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#ifndef ALGORITMOS_DISTRIBUCION_H_
#define ALGORITMOS_DISTRIBUCION_H_
#include <commons/collections/list.h>

typedef enum {
	LSU, EL, KE
} t_algoritmo;

typedef struct {
	//info del socket
	int socket;
	unsigned id;
	//para lsu
	int cant_entradas_vacias;

	//para ke
	char primer_letra;
	char ult_letra;

} t_instancia;

t_instancia* obtener_instancia_segun_EL(t_list* instancias);
t_instancia* obtener_instancia_segun_LSU(t_list* instancias);
t_instancia* obtener_instancia_segun_KE(t_list* instancias);

#endif /* ALGORITMOS_DISTRIBUCION_H_ */
