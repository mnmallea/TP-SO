#include "serializador.h"


void configure_logger_ser() {
  logger = log_create("serializador.log", "serializador",true, LOG_LEVEL_INFO);
}

paquete* crearPaquete(){
	paquete* nuevoPaquete = malloc(sizeof(paquete));
	nuevoPaquete->tamanioActual=0;
	nuevoPaquete->carga=malloc(0);
return nuevoPaquete;
};

void destruirPaquete(paquete* unPaquete){
	free(unPaquete->carga);
	free(unPaquete);
}

void eviarPaquete(int my_socket, void* element){
    int res_send = send(my_socket, &element, sizeof(element),MSG_NOSIGNAL);
    if(res_send != sizeof(element)){
        salir_con_error(my_socket,"No se pudo mandar mensaje");
    }
    //log_info(logger, "Mensaje enviado");
}

void agregar(paquete *pqt,void* contenido, size_t tamanioContenido){
	//pqt->carga = realloc(pqt->carga,pqt->tamanioActual + tamanioContenido );
	//memcpy(pqt->carga + pqt->tamanioActual,contenido,tamanioContenido);
	pqt->carga=contenido;
	pqt->tamanioActual += tamanioContenido;
	log_error(logger,"paquete de tamanio fijo agregado");
};

void agregarTamanioVariable(paquete* pqt, void* contenido, size_t tamanioContenido){
	agregar(pqt, &tamanioContenido, sizeof(tamanioContenido));
	agregar(pqt,contenido, tamanioContenido);
	log_error(logger," paquete tamanio variable agregado");
};

void* construirPaquete(paquete* pqt) {
	void* paqueteProcesado = malloc(pqt->tamanioActual);
	if(paqueteProcesado == NULL){
		 log_error(logger,"ERROR EN EL MALLOC");
	}
	memcpy(paqueteProcesado, pqt->carga, pqt->tamanioActual);

	return paqueteProcesado;
}

