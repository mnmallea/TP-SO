#include "instancia.h"

int operacion_set(int socketCoordinador) {
	log_info(logger, "inicializando OP_SET");
	char* clave;
	char* valor;

	log_trace(logger,
			"Se procede a obtener la clave y el valor del coordinador");
	recibir_set(socketCoordinador, &clave, &valor);

	log_info(logger,
			"Se procede a realizar set con la clave: %s y el valor %s", clave,
			valor);
	int resultado;
	resultado = hacer_set(clave, valor);
	free(clave);
	free(valor);
	log_trace(logger, "El resultado de la operacion set fue %d", resultado);
	return resultado;
}

int hacer_set(char* clave, char* valor) {
	log_trace(logger,
			"Se procede a crear la entrada con la clave: %s y el valor %s",
			clave, valor);
	claveEntrada* cv = crearClaveEntrada(clave, valor);
	log_trace(logger, "Numero de operacion %d", nroOperacion);
	log_trace(logger,
			"Se procede a ver si esa clave(%s) ya tenia un valor asociado",
			clave);
	if (buscarEntrada(cv->clave) != NULL) {
		log_info(logger, "La clave %s tenia un valor asociado, se procede a reemplazarlo", clave);
		reemplazarCVEnTabla(cv);
		liberarCv(cv);
		return 0;
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
			//aca hay que solicitarle al coordinador que mande a compactar
			return -1;
		}
		log_trace(logger, "Se almacenara en la posicion %d", proximaEntrada);
		agregarEnTabla(proximaEntrada, cv);
		log_trace(logger,
				"Se agrego a la tabla la clave %s, se procede a guardarlo en almacenamiento",
				clave);
		setEnAlmacenamiento(proximaEntrada, cv->valor, cv->tamanio);
		liberarCv(cv);
		return 0;
	} else {
		log_info(logger,
				"No se encontro lugar disponible, se procede a reemplazar alguna clave existente");
		obtenerAReemplazarSegunAlgoritmo(cv);
		liberarCv(cv);
		return 0;
	}
}

void obtenerAReemplazarSegunAlgoritmo(claveEntrada* cv) {
	if (configuracion.algoritmo == CIRC) {
		algoritmoCircular(cv);
	} else if (configuracion.algoritmo == LRU) {
		algoritmoLRU(cv);
	} else if (configuracion.algoritmo == BSU) {
		algoritmoBSU(cv);
	} else {
		log_error(logger, "No se encontro algoritmo de reemplazo");
		return;
	}
}

int STORE(char* clave) {
	log_info(logger, "inicializo OP_STORE");
	log_trace(logger,
			"Se procede a ver si la clave (%s) esta en tabla de entradas",
			clave);
	tablaE* cv = buscarEntrada(clave);
	if (cv == NULL) {
		return -1;
	}
	log_trace(logger, "Se busca el valor de la clave (%s) en el almacenamiento",
			clave);
	void* carga = buscarEnALmacenamiento(cv->indice, cv->tamanio);
	if (carga == NULL) {
		log_trace(logger, "no se encontro el valor en el almacenamiento");
		return -1;
	}

	cv->operaciones = nroOperacion;
	log_trace(logger, "se aumenta el numero de operacion(%d)", nroOperacion);
	log_trace(logger, "estoy storeando un %s", carga);
	almacenarEnDumper(carga, clave, cv->tamanio);
	free(carga);
	return 0;
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
		log_info(logger, "error al almacenar clave en dumper");
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

void* dumpearADisco(void* sinuso) {
	for (int i = 0; i < list_size(tabla); i++) {
		tablaE* entrada = list_get(tabla, i);
		STORE(entrada->clave);
	}
	return NULL;
}

// funcion dumper cada 100 segundos se prende recorre la tabla de entradas, la storea y vacia la entrada y el almacenamiento
