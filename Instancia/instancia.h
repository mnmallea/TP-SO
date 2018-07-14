/*
 * instancia.h
 *
 *  Created on: 7 jun. 2018
 *      Author: utnso
 */

#ifndef INSTANCIA_H_
#define INSTANCIA_H_
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include "tabla_entradas.h"
#include "almacenamiento.h"
#include <sys/mman.h>
#include <commons/collections/dictionary.h>
#include "../syntax-commons/conexiones.h"
#include "algoritmo.h"

typedef struct {
	char*			puntoMontaje;
	t_dictionary*	fd;
} dumperEstructura;
dumperEstructura* dumper;

extern t_log *logger;
extern config configuracion;
void* dumpearADisco(void* sinUso);
int crearDumperCV(char*clave);
void iniciarDumper(char* puntoMontaje);
void almacenarEnDumper(char*data,char*Clave,unsigned int tamanio);

t_list* obtenerAReemplazarSegunAlgoritmo(claveEntrada* cv, t_list* posiblesAReemplazar);
//------------------------------------------------------------------
int		SET(int socketCoordinador,t_list* posiblesAReemplazar);
int hacer_set(char* clave,char* valor,t_list* posiblesAReemplazar);
int		STORE(char* clave);




#endif /* INSTANCIA_H_ */
