/*
 * funciones_auxiliares_consola.h
 *
 *  Created on: 2 jul. 2018
 *      Author: utnso
 */

#ifndef FUNCIONES_AUXILIARES_CONSOLA_H_
#define FUNCIONES_AUXILIARES_CONSOLA_H_

#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>

#include "sincronizacion.h"
#include "typedefs.h"

extern t_list *lista_esis_listos;
extern t_list *lista_esis_finalizados;
extern t_dictionary *dic_esis_bloqueados;


bool contenido_en_lista(t_list* lista, int id_a_buscar);
bool es_un_esi_listo(int id_a_buscar);
bool es_un_esi_finalizado(int id_a_buscar);
bool es_un_esi_bloqueado(int id_a_buscar);
bool tiene_el_id_buscado(void* esi);

void eliminar_de_listos(t_esi* esi_a_eliminar);
void eliminar_de_bloqueados(t_esi* esi_a_eliminar);
void agregar_a_lista_bloq(void* esi);
void obtener_esis_bloq(char* c, void* data);
void borrar_esi_a_matar(char* c, void* lista_esis_bloq);
void buscar_esi(char* c, void* lista_esis_bloq);

t_esi* obtener_de_listos(int id_a_devolver);
t_esi* obtener_de_bloqueados(int id_a_devolver);


#endif /* FUNCIONES_AUXILIARES_CONSOLA_H_ */
