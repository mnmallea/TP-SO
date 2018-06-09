/*
 * instancia.c
 *
 *  Created on: 7 jun. 2018
 *      Author: utnso
 */

#include "instancia.h"

#include <bits/mman-linux.h>
#include <commons/log.h>
#include <commons/string.h>
#include <errno.h>

int SET(int socketCoordinador) {
	log_info(logger, "inicializando OP_SET");
	char* clave;
	char* valor;
	int estado = 1;
	recibir_set(socketCoordinador, &clave, &valor);
	claveEntrada* cv = crearClaveEntrada(clave, valor);
	if (!hayEntradasDisponibles(cv)) {
		//implementar algoritmo
	}
	int proximaEntrada = entradaSiguienteEnTabla(cv);
	setEnAlmacenamiento(proximaEntrada, cv->valor, cv->tamanio);
	agregarEnTabla(proximaEntrada, cv);
	liberarCv(cv);
	return estado;
}

int STORE(char* clave) {
	log_info(logger, "inicializo OP_STORE");
	int estado = 1;
	tablaE* cv = buscarEntrada(clave);
	void* carga = buscarEnALmacenamiento(cv->numero, cv->tamanio);
	log_trace(logger, "estoy storeando un %s", carga);
	almacenarEnDumper(carga, clave, cv->tamanio);
	free(carga);
	free(cv);
	return estado;
}
//------------------------------------------------------------------------
//https://stackoverflow.com/questions/7430248/creating-a-new-directory-in-c

void iniciarDumper(char* puntoMontaje) {
	log_info(logger, "inicializo DUMPER");
	dumper = malloc(sizeof(dumperEstructura));

	struct stat directorio = { 0 };

	if (stat(puntoMontaje, &directorio) < 0) {
		mkdir(puntoMontaje, S_IRWXU);
	}
	dumper->puntoMontaje = puntoMontaje;
	dumper->fd = dictionary_create();
}

//https://www.linuxquestions.org/questions/programming-9/mmap-tutorial-c-c-511265/
void almacenarEnDumper(char* data, char* clave, unsigned int tamanio) {
	int fd;
	if (dictionary_has_key(dumper->fd, clave)) {
		fd = (int) dictionary_get(dumper->fd, clave);
	} else {
		fd = crearDumperCV(clave);
	}
	if (fd == -1) {
		log_info(logger, "error al almacenar clave en dumper");
		//TERMINAR ALMACENARENDUMPER
	}
	ftruncate(fd, tamanio);
	void* memMap = mmap(NULL, tamanio, PROT_READ | PROT_WRITE | PROT_EXEC,
	MAP_SHARED, fd, 0);
	memcpy(memMap, data, tamanio);
	munmap(memMap, tamanio);
}

int crearDumperCV(char*clave) {
	char* nombreArchivo = string_from_format("%s/%s.txt", dumper->puntoMontaje,
			clave);
	log_trace(logger, "Nombre del archivo a guardar: %s", nombreArchivo);
	int fd = open(nombreArchivo, O_CREAT | O_RDWR, S_IRWXU);
	log_trace(logger, "File descriptor: %d", fd);
	if (fd == -1) {
		log_error(logger, "%s", strerror(errno));
		log_info(logger, "error al crear el archivo txt ");
		// debemos cerrarlo ? close(fd);
		return -1;
	}
	dictionary_put(dumper->fd, clave, (void*) fd);
	free(nombreArchivo);
	return fd;
}

