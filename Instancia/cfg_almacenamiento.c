/*
 * cfg_almacenamiento2.c
 *
 *  Created on: 31 may. 2018
 *      Author: utnso
 */
#include "cfg_almacenamiento.h"

void configurarAlmacenamiento(char* ip, char* puerto){
	int socketCoordinador=conectarse_a_coordinador(ip,puerto,INSTANCIA);
	paquete* pqt = crearPaquete();
	agregarTamanioVariable(pqt,configuracion.nombre_instancia,string_length(configuracion.nombre_instancia) + 1);
	pqt = construirPaquete(pqt);
	enviarPaquete(socketCoordinador, pqt,pqt->tamanioActual);
	destruirPaquete(pqt);
	recibirPaquete(socketCoordinador,&cfgAlmacenamiento,sizeof(cfgAlmacenamiento));

	//mostrar que se asigno una cantidad de  x entradas de x tamanio para el ato
	//faltaria a partir de esto.... inicializar el almacenamiento

}
