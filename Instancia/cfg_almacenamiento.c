/*
 * cfg_almacenamiento2.c
 *
 *  Created on: 31 may. 2018
 *      Author: utnso
 */
#include "cfg_almacenamiento.h"

void configurarAlmacenamiento(int socketCoordinador){
	paquete* pqt = crearPaquete();
	agregarTamanioVariable(pqt,configuracion.nombre_instancia,string_length(configuracion.nombre_instancia) + 1);
	pqt = construirPaquete(pqt);
	enviarPaquete(socketCoordinador, pqt,pqt->tamanioActual);
	destruirPaquete(pqt);
	recibirPaquete(socketCoordinador,&cfgAlmacenamiento.tamanioEntrada,sizeof(cfgAlmacenamiento.tamanioEntrada));
	recibirPaquete(socketCoordinador,&cfgAlmacenamiento.totalEntradas,sizeof(cfgAlmacenamiento.totalEntradas));
	inicializarAlmacenamiento(cfgAlmacenamiento.totalEntradas,cfgAlmacenamiento.tamanioEntrada);
	//mostrar que se asigno una cantidad de  x entradas de x tamanio para el ato

}
unsigned int obtenerEntradasTotales(){
	return cfgAlmacenamiento.totalEntradas;
}

unsigned int obtenerTamanioEntrada(){
	return cfgAlmacenamiento.tamanioEntrada;
}


