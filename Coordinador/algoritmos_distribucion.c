/*
 * algoritmos_distribucion.c
 *
 *  Created on: 3 may. 2018
 *      Author: utnso
 */

#include "algoritmos_distribucion.h"
#include "main.h"

int primera_vez = 0;
int instancia_a_utilizar;
int largo_lista;

t_instancia *obtener_instancia_segun_EL(t_list* instancias){

	if(primera_vez == 0){
		instancia_a_utilizar = 0;
	}

	t_instancia *inst_elegida = list_get(instancias, instancia_a_utilizar);
	largo_lista = list_size(instancias);

	if(instancia_a_utilizar == (largo_lista -1 )){ //recien utilice la ultima posicion
		instancia_a_utilizar = 0;
	}else{
		instancia_a_utilizar++;
	}

	return inst_elegida;

}


/*
 *
 * faltan definir
instancia obtener_instancia_segun_LSU(t_list* instancias){}
instancia obtener_instancia_segun_KE(t_list* instancias){}

*/
