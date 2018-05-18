/*
 * distribucion.c
 *
 *  Created on: 17 may. 2018
 *      Author: utnso
 */

#include "distribucion.h"


void nueva_instancia(t_instancia* inst){
	list_add(lista_instancias_disponibles, inst);
}

t_instancia *obtener_instancia_para_almacenar(){

	t_instancia *inst;
	if(configuracion.algoritmo == LSU){
		inst = obtener_instancia_segun_EL(lista_instancias_disponibles);
	}else if(configuracion.algoritmo == KE){
		inst = obtener_instancia_segun_KE(lista_instancias_disponibles);
	}else{
		inst = obtener_instancia_segun_LSU(lista_instancias_disponibles);
	}

	return inst;
}
