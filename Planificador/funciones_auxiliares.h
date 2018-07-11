/*
 * funciones_auxiliares.h
 *
 *  Created on: 2 jul. 2018
 *      Author: utnso
 */

#ifndef FUNCIONES_AUXILIARES_H_
#define FUNCIONES_AUXILIARES_H_

#include <stdbool.h>
#include <pthread.h>

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>

#include "sincronizacion.h"
#include "typedefs.h"

extern t_dictionary *dic_esis_bloqueados;
extern t_dictionary *dic_clave_x_esi;

bool puede_tomar_la_clave(char* clave, t_esi* esi_solicitante);
bool esi_tiene_clave(char* clave, t_esi* esi_a_validar);

void agregar_a_dic_bloqueados(char* clave, t_esi *esi);
void nueva_clave_tomada_x_esi(char* clave, t_esi* esi_solicitante);
void aumentar_viene_esperando(void* esi);
void aumentar_viene_corriendo(void* esi);



#endif /* FUNCIONES_AUXILIARES_H_ */
