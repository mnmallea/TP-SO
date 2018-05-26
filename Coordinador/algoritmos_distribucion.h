/*
 * algoritmos_distribucion.h
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */

#ifndef ALGORITMOS_DISTRIBUCION_H_
#define ALGORITMOS_DISTRIBUCION_H_
#include <stdlib.h>
#include <commons/collections/list.h>
#include "sincronizacion.h"
#include "config_coordinador.h"


#define CANT_LETRAS_ALFABETO 26

extern config configuracion;
extern t_list* lista_instancias_disponibles;


t_instancia* obtener_instancia_siguiente(char* clave);
t_instancia* obtener_instancia_segun_EL(char* clave);
t_instancia* obtener_instancia_segun_LSU(char* clave);
t_instancia* obtener_instancia_segun_KE(char* clave);
bool tieneMasEspacioLibre(void*, void*);
int get_nro_letra(char letra);

#endif /* ALGORITMOS_DISTRIBUCION_H_ */
