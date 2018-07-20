#include "instancia.h"

#include <commons/collections/list.h>
#include <commons/string.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../syntax-commons/conexiones.h"
#include "../syntax-commons/my_socket.h"
#include "../syntax-commons/protocol.h"
#include "algoritmos.h"
#include "almacenamiento.h"
#include "main.h"

void operacion_set(int socketCoordinador) {
	log_info(logger, "inicializando OP_SET");
	char* clave;
	char* valor;

	log_trace(logger,
			"Se procede a obtener la clave y el valor del coordinador");
	recibir_set(socketCoordinador, &clave, &valor);

	log_info(logger, "Se procede a realizar set con la clave: %s y el valor %s",
			clave, valor);
	int resultado;
	resultado = hacer_set(clave, valor);
	switch (resultado) {
	case SET_EXITOSO:
		log_debug(logger, "SET arrojo resultado EXITO");
		enviar_cod_operacion(socketCoordinador, EXITO);
		mandar_mensaje(socketCoordinador, almac_entradas_disponibles());
		break;
	case SET_ERROR:
		log_debug(logger, "SET arrojo resultado ERROR");
		enviar_cod_operacion(socketCoordinador, ERROR);
		break;
	case REQUIERE_COMPACTACION:
		log_debug(logger, "SET arrojo resultado REQUIERE_COMPACTACION");
		enviar_cod_operacion(socketCoordinador, SOLICITUD_COMPACTACION);
		break;
	}
	free(clave);
	free(valor);
	log_trace(logger, "El resultado de la operacion set fue %d", resultado);

}

t_resultado_set hacer_set(char* clave, char* valor) {
	log_trace(logger,
			"Se procede a crear la entrada con la clave: %s y el valor %s",
			clave, valor);
	claveEntrada* cv = crearClaveEntrada(clave, valor);
	log_trace(logger, "Numero de operacion %d", nroOperacion);
	log_trace(logger,
			"Se procede a ver si esa clave(%s) ya tenia un valor asociado",
			clave);
	if (buscarEntrada(cv->clave) != NULL) {
		log_info(logger,
				"La clave %s tenia un valor asociado, se procede a reemplazarlo",
				clave);
		int res_reemplazo = reemplazarCVEnTabla(cv);
		liberarCv(cv);
		if (res_reemplazo < 0)
			return SET_ERROR;
		else
			return SET_EXITOSO;
	}
	log_info(logger,
			"La clave %s no tenia un valor asociado, se procede a encontrarle un lugar",
			clave);
	int entradas_que_ocuparia = entradas_que_ocupa_por_tamanio(cv->tamanio);
	log_debug(logger, "La cantidad de entradas disponibles son %d",
			almac_entradas_disponibles());
	log_debug(logger, "El valor ocuparia %d entradas", entradas_que_ocuparia);
	if (hayEntradasDisponibles(cv)) {
		int proximaEntrada = almac_primera_posicion_libre_con_tamanio(
				entradas_que_ocuparia);
		if (proximaEntrada < 0) {
			log_info(logger, "Hay fragmentacion externa, se debe compactar");
			liberarCv(cv);
			return REQUIERE_COMPACTACION;
		}
		log_trace(logger, "Se almacenara en la posicion %d", proximaEntrada);
		agregarEnTabla(proximaEntrada, cv);
		log_trace(logger,
				"Se agrego a la tabla la clave %s, se procede a guardarlo en almacenamiento",
				clave);
		setEnAlmacenamiento(proximaEntrada, cv->valor, cv->tamanio);
		liberarCv(cv);
		return SET_EXITOSO;
	} else {
		log_info(logger,
				"No se encontro lugar disponible, se procede a reemplazar alguna clave existente");
		t_resultado_set resultado_reemplazo = ReemplazarSegunAlgoritmo(cv);
		liberarCv(cv);
		return resultado_reemplazo;
	}
}

int STORE(char* clave) {
	log_info(logger, "inicializo OP_STORE");
	log_trace(logger,
			"Se procede a ver si la clave (%s) esta en tabla de entradas",
			clave);
	tablaE* entrada = buscarEntrada(clave);
	if (entrada == NULL) {
		return -1;
	}
	entrada->operaciones = nroOperacion;
	log_trace(logger, "Se busca el valor de la clave (%s) en el almacenamiento",
			clave);

	return 0;
}

void bajar_a_disco(tablaE* entrada) {
	void* carga = buscarEnALmacenamiento(entrada->indice, entrada->tamanio);
	log_info(logger, "Almacenando valor de la clave \"%s\"",
			entrada->clave);
	almacenarEnDumper(carga, entrada->clave, entrada->tamanio);
	free(carga);
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
		log_trace(logger,
				"se encontro la clave (%s) en el diccionario de file descriptors",
				clave);
		fd = (int) dictionary_get(dumper->fd, clave);
	} else {
		fd = crearDumperCV(clave);
	}
	if (fd == -1) {
		log_error(logger, "Error al almacenar clave \"%s\" en dumper", clave);
		return;
	}
	ftruncate(fd, tamanio);
	log_trace(logger, "Se trunca el archivo a un tamanio (%d)", tamanio);
	void* memMap = mmap(NULL, tamanio, PROT_READ | PROT_WRITE | PROT_EXEC,
	MAP_SHARED, fd, 0);
	memcpy(memMap, data, tamanio);
	munmap(memMap, tamanio);
}

int crearDumperCV(char* clave) {
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

void bajar_a_disco_iterator(void* entrada) {
	bajar_a_disco(entrada);
}

void* dumpearADisco(void* sinuso) {
	while(1){
		sleep(configuracion.intervalo_dump);
		pthread_mutex_lock(&mutex_operacion);
		list_iterate(tabla, bajar_a_disco_iterator);
		pthread_mutex_unlock(&mutex_operacion);
	}
	return NULL;
}

void sacarDelDumper(char* clave) {
	if (!dictionary_has_key(dumper->fd, clave)) {
		log_debug(logger, "La clave %s no estaba en el dumper", clave);
		return;
	}
	int fd = (int) dictionary_remove(dumper->fd, clave);
	log_debug(logger, "Se saco la clave %s del dumper (fd=%d)", clave, fd);
	close(fd);
}
