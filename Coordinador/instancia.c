/*
 * instancia.c
 *
 *  Created on: 5 jun. 2018
 *      Author: utnso
 */
#include "instancia.h"

/*
 * Aclaracion, vos te tenes que encargar de que la clave tenga el lugar en memoria.
 * No hacer el free despues porque la vas a borrar y cagas todo
 */
void agregar_clave_almacenada(t_instancia* instancia, char* clave){
	list_add(instancia->claves_almacenadas, clave);
}

/*
 * Deberia decirte si la instancia almacena una clave
 */
bool tiene_clave_almacenada(t_instancia* instancia, char* clave){
	bool esLaClave(void* otraClave){
		return string_equals_ignore_case(clave,(char*)otraClave);
	}
	return list_any_satisfy(instancia->claves_almacenadas, esLaClave);
}

/*
 * Liberar el nombre luego de usar esto porque se lo copia
 */
t_instancia* crear_instancia(int sockfd, char* nombre, int cant_entradas){
	t_instancia* new_instancia = malloc(sizeof(new_instancia));
	new_instancia->claves_almacenadas = list_create();
	new_instancia->nombre = string_duplicate(nombre);
	new_instancia->socket = sockfd;
	new_instancia->cant_entradas_vacias = cant_entradas;
	return new_instancia;
}

/*
 * Libera la memoria ocupada por la estructura de la instancia
 */
void liberar_instancia(t_instancia* instancia){
	if(instancia != NULL){
		list_destroy(instancia->claves_almacenadas);
		free(instancia->nombre);
		free(instancia);
	}
}
