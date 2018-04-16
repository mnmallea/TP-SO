/*
 * main.c
 *
 *  Created on: 15 abr. 2018
 *      Author: utnso
 */
//#define RUTA_CONFIG "../config_coordinador"

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include "algoritmos_distribucion.h"
#include "config_coordinador.h"


int main(int argc, char **argv){//aca recibiriamos la ruta del archivo de configuracion como parametro
	config configuracion = configurar(argv[1]);
	printf("Si, esto todavia no hace nada.. \n");

	exit(0);
}

