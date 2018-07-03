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
#include <semaphore.h>
#include <sys/select.h>

#include <commons/log.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>

#include "../syntax-commons/conexiones.h"
#include "../syntax-commons/my_socket.h"
#include "../syntax-commons/protocol.h"

#include "config_planificador.h"
#include "algoritmos_planificacion.h"
#include "typedefs.h"
#include "sincronizacion.h"
#include "funciones_auxiliares.h"


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

extern bool planificacion_pausada;

bool algoritmo_debe_planificar() ;
bool hay_que_planificar();

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
void nueva_solicitud(int socket, char* clave);
void liberar_recursos(t_esi* esi_a_liberar);
void liberar_clave(void* clave);

void deadlock();
void itera_por_linea(char *claveIncialTomada, void *esiInicial);
void buscarClaveQEspera(char* claveQEspera, void* esisbloq);
bool esta(void *esi);
void mergearL(void *id);
void mostrarDL(void* candidato);

#endif /* PLANIFICACION_H_ */
