/*
 * planificacion.h
 *
 *  Created on: 17 may. 2018
 *      Author: utnso
 */

#ifndef PLANIFICACION_H_
#define PLANIFICACION_H_

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <stdbool.h>

#include "../syntax-commons/protocol.h"
#include "typedefs.h"

extern bool planificacion_pausada;
extern t_esi* esi_a_matar_por_consola;
extern t_esi* esi_a_bloquear_por_consola;
extern char* clave_a_bloquear;

int flag;
char* clave_bloqueadora;

bool hay_nuevo_esi;
bool hay_esi_bloqueado;
bool hay_esi_finalizado;

t_esi *esi_corriendo;

t_list *lista_esis_listos;
t_list *lista_esis_finalizados;

t_dictionary *dic_esis_bloqueados;
t_dictionary *dic_clave_x_esi;

t_protocolo respuesta_esi_corriendo;

bool algoritmo_debe_planificar() ;
bool hay_que_planificar();
bool validar_si_hubo_bloqueo_o_asesinato_por_consola();

t_esi *obtener_nuevo_esi_a_correr() ;




void* planificar(void* _);
void nuevo_esi(t_esi* esi);
void finalizar_esi(t_esi* esi_a_finalizar);
void bloquear_esi(char* clave, t_esi* esi_a_bloquear);
void se_desbloqueo_un_recurso(char* clave);
void correr(t_esi* esi);
void ya_termino_linea();
void linea_size();
void interpretar();
void fallo_linea();
void nueva_solicitud(int socket, char* clave, int id_pedido);
void liberar_recursos(t_esi* esi_a_liberar);
void liberar_clave(void* clave);
void ejecutar_bloqueo_o_asesinato();
void nullear_esis_por_consola();


void deadlock();
void itera_por_linea(char *claveIncialTomada, void *esiInicial);
void buscarClaveQEspera(char* claveQEspera, void* esisbloq);
bool esta(void *esi);
void mergearL(void *id);
void mostrarDL(void* candidato);

#endif /* PLANIFICACION_H_ */
