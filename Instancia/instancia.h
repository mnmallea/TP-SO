/*
 * instancia.h
 *
 *  Created on: 7 jun. 2018
 *      Author: utnso
 */

#ifndef INSTANCIA_H_
#define INSTANCIA_H_

#include <commons/collections/dictionary.h>
#include <commons/log.h>

#include "config_instancia.h"
#include "tabla_entradas.h"

typedef enum {
	SET_EXITOSO, REQUIERE_COMPACTACION, SET_ERROR
} t_resultado_set;

typedef struct {
	char* puntoMontaje;
	t_dictionary* fd;
} dumperEstructura;
dumperEstructura* dumper;

extern t_log *logger;
extern config configuracion;
void* dumpearADisco(void* sinUso);
int crearDumperCV(char*clave);
void iniciarDumper(char* puntoMontaje);
void almacenarEnDumper(char*data, char*Clave, unsigned int tamanio);

//------------------------------------------------------------------
void operacion_set(int socketCoordinador);
t_resultado_set hacer_set(char* clave, char* valor);
int STORE(char* clave);
void bajar_a_disco(tablaE* entrada);

#endif /* INSTANCIA_H_ */
