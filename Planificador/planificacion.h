/*
 * planificacion.h
 *
 *  Created on: 17 may. 2018
 *      Author: utnso
 */

#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include <stdlib.h>
#include <pthread.h>
#include <commons/log.h>
#include "config_planificador.h"
#include "algoritmos_planificacion.h"
#include "sincronizacion.h"
#include "selector.h"
#include "main.h"


int flag;

/* Nombre: obtener_nuevo_esi_a_correr();
 * Desc: obtiene el proximo esi a correr a partir del algoritmo elegido
 * Le dice al ESI elegido que lea la proxima linea
 */
void obtener_nuevo_esi_a_correr();

/* Nombre: bloquear_esi(char* clave)
 * Desc: Luego de que el coordinador me indico que un recurso se encuentra bloqueado
 * Agarro al esi que esta ejecutando y lo agrego al diccionario de bloqueados
 */
void bloquear_esi(char* clave);

/* Nombre: bloquear_esi_por_consola(char* clave, int id_esi)
 * Desc: por consola me bloquean un esi para un recurso
 * busco al esi con ese id dentro de la lista de listos
 * lo meto en el diccionario de esis bloqueados por clave
 */
void bloquear_esi_por_consola(char* clave, int id_esi);


/* Nombre: finalizar_esi()
 * Desc: El esi corriendo me avisa que termino
 * Lo saco de corriendo y lo paso a la lista de finalizados
 * Se replanifica
 */
void finalizar_esi();

/* Nombre: se_desbloqueo_un_recurso(char* clave)
 * Desc: Cuando un esi me indica que realizo una operacion de STORE
 * Se liberan todos los recursos que estaban esperando esa clave y se los pasa a la lista de listos
 */
void se_desbloqueo_un_recurso(char* clave);

/* Nombre: nuevo_esi(t_esi* esi)
 * Desc: Agrego un nuevo esi a la lista de listos
 */
void nuevo_esi(t_esi* esi);


t_esi buscar_esi_por_id(int id_esi);

bool esi_con_este_id(void* esi);

#endif /* PLANIFICACION_H_ */
