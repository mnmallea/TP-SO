/*
 * algoritmos_distribucion.c
 *
 *  Created on: 3 may. 2018
 *      Author: utnso
 */
#include "algoritmos_distribucion.h"

int instancia_a_utilizar = 0;
int largo_lista;

t_instancia* obtener_instancia_segun_EL(t_list* instancias){
	t_instancia* inst_elegida = list_get(instancias, instancia_a_utilizar);
	largo_lista = list_size(instancias);
	instancia_a_utilizar++;

	if(instancia_a_utilizar == largo_lista) //recien utilice la ultima posicion
		instancia_a_utilizar = 0;

	return inst_elegida;
}

/*
 *
 * faltan declarar
instancia obtener_instancia_segun_LSU(t_list* instancias){}
instancia obtener_instancia_segun_KE(t_list* instancias){}

*/
