#include "instancia.h"

int SET(int socketCoordinador, t_list* posiblesAReemplazar) {
	log_info(logger, "inicializando OP_SET");
	char* clave;
	char* valor;
	recibir_set(socketCoordinador, &clave, &valor);
	claveEntrada* cv= crearClaveEntrada(clave, valor);
	nroOperacion ++;
	if(buscarEntrada(cv->clave)!=NULL){
		reemplazarCVEnTabla(cv);
		liberarCv(cv);
		return 0;
	}
	if(hayEntradasDisponibles(cv)){
	//	if(buscarEntrada(cv->clave)==NULL){
			int proximaEntrada = entradaSiguienteEnTabla(cv);
			agregarEnTabla(proximaEntrada, cv);
			setEnAlmacenamiento(proximaEntrada, cv->valor, cv->tamanio);
			liberarCv(cv);
			return 0;
	/*	}else{
			reemplazarCVEnTabla(cv);
			liberarCv(cv);
			return 0;
		} */
	}else{
	t_list* vanAReemplazarse = algoritmoCircular(cv,posiblesAReemplazar);
	for(int i=0; i<list_size(vanAReemplazarse);i++){
		tablaE* aReemp= buscarEntrada(list_get(vanAReemplazarse,i));
		removerDeLista(aReemp->numero, aReemp);
		}
	int proximaEntrada = entradaSiguienteEnTabla(cv);
	agregarEnTabla(proximaEntrada, cv);
	setEnAlmacenamiento(proximaEntrada, cv->valor, cv->tamanio);
	liberarCv(cv);
	}
		liberarCv(cv);
		return 0;
			}



int STORE(char* clave) {
	log_info(logger, "inicializo OP_STORE");
	tablaE* cv = buscarEntrada(clave);
	if(cv == NULL){
		return -1;
	}
	void* carga = buscarEnALmacenamiento(cv->numero, cv->tamanio);
	if(carga == NULL){
		return -1;
	}
	nroOperacion ++;

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

int crearDumperCV(char* clave) {
	char* nombreArchivo = string_from_format("%s/%s.txt", dumper->puntoMontaje,clave);
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


// funcion dumper cada 100 segundos se prende recorre la tabla de entradas, la storea y vacia la entrada y el almacenamiento
void* dumpearADisco(void* sinuso){
	while(1){
	sleep(10);
	for(int i=0;i<obtenerEntradasTotales();i++){
		tablaE* entrada=list_get(tabla,i);
		STORE(entrada->clave);
		free(entrada);
	}
}
}



