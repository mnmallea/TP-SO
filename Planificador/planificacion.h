/*
 * planificacion.h
 *
 *  Created on: 17 may. 2018
 *      Author: utnso
 */

#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <commons/log.h>
#include "config_planificador.h"
#include "algoritmos_planificacion.h"
#include "sincronizacion.h"
//#include "selector.h"
//#include "consola.h"
#include "main.h"
#include "../syntax-commons/conexiones.h"


int flag;
bool primera_vez;
bool hay_nuevo_esi;
bool hay_esi_bloqueado;
bool hay_esi_finalizado;
t_esi* esi_a_matar;

/* Nombre: obtener_nuevo_esi_a_correr();
 * Desc: obtiene el proximo esi a correr a partir del algoritmo elegido
 * Le dice al ESI elegido que lea la proxima linea
 */
t_esi *obtener_nuevo_esi_a_correr();

/* Nombre: bloquear_esi(char* clave)
 * Desc: Luego de que el coordinador me indico que un recurso se encuentra bloqueado
 * Agarro al esi que esta ejecutando y lo agrego al diccionario de bloqueados
 */
void bloquear_esi(char* clave);

void agregar_a_dic_bloqueados(char* clave, t_esi *esi);

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

void desbloquear_por_consola(char* clave);

/* Nombre: nuevo_esi(t_esi* esi)
 * Desc: Agrego un nuevo esi a la lista de listos
 */
void nuevo_esi(t_esi* esi);

t_esi *buscar_esi_por_id(int id_esi);

bool esi_con_este_id(void* esi);

/* Nombre: esi_tiene_clave(t_esi* esi, char* clave)
 * Desc: Indica si el esi corriendo tiene tomada la clave mencionada
 */
bool esi_tiene_clave(char* clave);

/* Nombre: esta_tomada_x_otro_la_clave(char* clave)
 * Desc: Indica si la clave mencionada ya fue o no tomada
 */
bool esta_tomada_x_otro_la_clave(char* clave);

/* Nombre: nueva_clave_tomada_x_esi(char* clave)
 * Desc: Cuando el coordinador me dice que un esi pidio una clave llama a esta funcion
 * Que me agrega esa clave y ese esi al diccionario de claves x esi
 */
void nueva_clave_tomada_x_esi(char* clave);

/* Nombre: liberar_recursos(t_esi* esi)
 * Desc: Se liberan las claves tomadas por ese esi
 * Se la saca del dic_clave_x_esi y se desbloquean los recursos
 */
void liberar_recursos(t_esi* esi);

/* Nombre: liberar_claves(char* clave, void* esi)
 * Desc: Le saca las claves tomadas al esi
 */
void liberar_claves_de_esi(t_esi* esi_a_matar, char* clave, t_esi* esi);

/* Nombre: desbloquear_claves_tomadas(char* clave, void* esi)
 * Desc: Desbloquea las claves tomadas por ese esi
 */
void desbloquear_claves_tomadas(char* clave, void* esi);


void correr(t_esi* esi);
void ya_termino_linea();
void fallo_linea();
void interpretar();
void linea_size();
void aumentar_viene_esperando(void* esi);
void aumentar_viene_corriendo(void* esi);


void nueva_solicitud(int socket, char* clave);

void* planificar(void *);

void matar_esi_corriendo();

void clave_esta_tomada_x_esi_a_liberar(char* clave, void* esi);
void liberar_clave(void* clave);
t_list *obtener_todos_los_esis_bloqueados();

void agregar_a_lista_bloq(void* esi);
void obtener_esis_bloq(char* c, void* data);

//t_esi *obtener_procesos_en_deadlock();


#endif /* PLANIFICACION_H_ */
