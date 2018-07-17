/*
 * instancia.c
 *
 *  Created on: 5 jun. 2018
 *      Author: utnso
 */

#include "instancia.h"

#include <commons/string.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../syntax-commons/conexiones.h"
#include "../syntax-commons/protocol.h"
#include "../syntax-commons/serializador.h"
#include "sincronizacion.h"

/*
 * Aclaracion, esto te copia la clave que le pases
 * Por lo que si no la vas a usar mas despues le tenes que hacer vos el free
 * Ademas te disminuye el espacio restante de la instancia
 */
void agregar_clave_almacenada(t_instancia* instancia, char* clave) {
	char* copia_clave = string_duplicate(clave);
	list_add(instancia->claves_almacenadas, copia_clave);
	instancia->cant_entradas_vacias -= espacio_utilizado_por(clave);
}
/*
 * Remueve la clave almacenada de la instancia
 * serviria luego de que haga el store
 */
void remover_clave_almacenada(t_instancia* instancia, char* clave) {
	bool esLaClave(void* unaClave) {
		return !strcmp(unaClave, clave);
	}
	char* clave_buscada = list_remove_by_condition(
			instancia->claves_almacenadas, esLaClave);
	free(clave_buscada);
}
/*
 * Deberia decirte si la instancia almacena una clave
 */
bool tiene_clave_almacenada(t_instancia* instancia, char* clave) {
	bool esLaClave(void* otraClave) {
		return string_equals_ignore_case(clave, (char*) otraClave);
	}
	return list_any_satisfy(instancia->claves_almacenadas, esLaClave);
}

/*
 * Liberar el nombre luego de usar esto porque se lo copia
 */
t_instancia* crear_instancia(int sockfd, char* nombre, int cant_entradas) {
	t_instancia* new_instancia = malloc(sizeof(*new_instancia));
	new_instancia->claves_almacenadas = list_create();
	new_instancia->nombre = string_duplicate(nombre);
	new_instancia->socket = sockfd;
	new_instancia->cant_entradas_vacias = cant_entradas;
	new_instancia->thread = pthread_self();
	sem_init(&new_instancia->semaforo_instancia, 0, 0);
	pthread_mutex_init(&new_instancia->mutex_comunicacion, NULL);
	return new_instancia;
}

/*
 * Libera la memoria ocupada por la estructura de la instancia
 */
void liberar_instancia(t_instancia* instancia) {
	if (instancia != NULL) {
		list_destroy(instancia->claves_almacenadas);
		free(instancia->nombre);
		free(instancia);
	}
}

t_instancia* sacar_instancia_de_lista(char* nombre, t_list* lista) {
	bool esLaInstanciaQueBusco(void* instancia) {
		return string_equals_ignore_case(((t_instancia*) instancia)->nombre,
				nombre);
	}
	return list_remove_by_condition(lista, esLaInstanciaQueBusco);
}

bool esta_instancia_en_lista(char* nombre, t_list* lista) {
	bool esLaInstanciaQueBusco(void* instancia) {
		return string_equals_ignore_case(((t_instancia*) instancia)->nombre,
				nombre);
	}
	return list_any_satisfy(lista, esLaInstanciaQueBusco);
}

bool esta_activa_instancia(char* nombre) {
	return esta_instancia_en_lista(nombre, lista_instancias_disponibles);
}

bool esta_inactiva_instancia(char* nombre) {
	return esta_instancia_en_lista(nombre, lista_instancias_inactivas);
}

/*
 * La saca de la lista de activas y la mete en inactivas
 * Thread safe
 */
void instancia_desactivar(char* nombre_instancia) {
	t_instancia* instancia = instancia_sacar_de_activas(nombre_instancia);

	if(instancia == NULL){
		log_warning(logger, "La instancia que se intento desactivar no estaba activa");
		return;
	}

	instancia_agregar_a_inactivas(instancia);

	if (pthread_equal(instancia->thread, pthread_self())) {
		return;
	}
	if (pthread_cancel(instancia->thread)) {
		log_error(logger, "Error al finalizar thread de instancia %s",
				instancia->nombre);
	}

}

/*
 * Saca una instancia de la lista de activas y te devuelve el puntero
 * Thread safe
 * Unsafe en el sentido de que puede devolverte NULL (VALIDAR ESTO)
 */
t_instancia* instancia_sacar_de_activas(char* nombre_instancia) {
	t_instancia * instancia;
	pthread_mutex_lock(&mutex_instancias_disponibles);
	bool esLaInstanciaQueBusco(void* una_instancia) {
		t_instancia* inst = una_instancia;
		return string_equals_ignore_case(inst->nombre, nombre_instancia);
	}
	instancia = list_remove_by_condition(lista_instancias_disponibles,
			esLaInstanciaQueBusco);
	sem_wait(&contador_instancias_disponibles);
	pthread_mutex_unlock(&mutex_instancias_disponibles);
	return instancia;
}

/*
 * Cierra el socket y la agrega a la lista de instancias inactivas
 * thread safe
 */
void instancia_agregar_a_inactivas(t_instancia* instancia) {
	close(instancia->socket);
	sem_destroy(&instancia->semaforo_instancia);
	pthread_mutex_lock(&mutex_instancias_inactivas);
	list_add(lista_instancias_inactivas, instancia);
	pthread_mutex_unlock(&mutex_instancias_inactivas);
}

/*
 * Agrega una instancia a la lista de instancias activas
 * Se encarga de los semaforos
 */
void instancia_agregar_a_activas(t_instancia* instancia) {
	pthread_mutex_lock(&mutex_instancias_disponibles);
	list_add(lista_instancias_disponibles, instancia);
	pthread_mutex_unlock(&mutex_instancias_disponibles);
	sem_post(&contador_instancias_disponibles);
}

/*
 * Thread safe
 */
//head. filter (tieneClave unaClave) $ lista_instancias_disponibles
t_instancia* instancia_con_clave(char* clave) {
	bool instanciaTieneLaClave(void * unaInstancia) {
		return tiene_clave_almacenada(unaInstancia, clave);
	}
	pthread_mutex_lock(&mutex_instancias_disponibles);
	t_list* instancias_con_clave = list_filter(lista_instancias_disponibles,
			instanciaTieneLaClave);
	pthread_mutex_unlock(&mutex_instancias_disponibles);
	t_instancia* instancia = list_get(instancias_con_clave, 0);
	list_destroy(instancias_con_clave);
	return instancia;
}

int espacio_utilizado_por(char* clave) {

	int espacio_utilizado = (strlen(clave) + 1) / configuracion.entrada_size;
	if ((strlen(clave) + 1) % configuracion.entrada_size) {
		espacio_utilizado++;
	}
	return espacio_utilizado;
}

/*
 * Relevanta la instancia y devuelve su estructura
 * Si no puede la desactiva y devuelve NULL
 */
t_instancia* instancia_relevantar(char* nombre, int socket) {
	pthread_mutex_lock(&mutex_instancias_inactivas);
	t_instancia* instancia = sacar_instancia_de_lista(nombre,
			lista_instancias_inactivas);
	pthread_mutex_unlock(&mutex_instancias_inactivas);
	instancia->socket = socket;

	t_paquete* paquete_claves = paquete_crear();
	int i;
	int cantidad_claves = list_size(instancia->claves_almacenadas);
	log_info(logger, "La instancia %s tenia %d claves:", instancia->nombre, cantidad_claves);
	for (i = 0; i < cantidad_claves; i++) {
		char* clave_actual = list_get(instancia->claves_almacenadas, i);
		paquete_agregar(paquete_claves, clave_actual, strlen(clave_actual) + 1);
		printf("%d - %s\n", i, clave_actual);
	}
	if (enviar_cod_operacion(socket, RELEVANTAR_INSTANCIA) < 0) {
		log_error(logger, "Error al relevantar instancia %s",
				instancia->nombre);
		instancia_agregar_a_inactivas(instancia);
		return NULL;
	}
	if (send(socket, &cantidad_claves, sizeof(cantidad_claves), 0) < 0) {
		log_error(logger, "Error al relevantar instancia %s",
				instancia->nombre);
		instancia_agregar_a_inactivas(instancia);
		return NULL;
	}

	if (paquete_enviar(paquete_claves, socket) < 0) {
		log_error(logger, "Error al relevantar instancia %s",
				instancia->nombre);
		instancia_agregar_a_inactivas(instancia);
		return NULL;
	}
	//todo ver si necesitas algun tipo de confirmacion por parte de la instancia

	log_info(logger, "La instancia %s ha sido relevantada", instancia->nombre);
	instancia->thread = pthread_self();
	sem_init(&instancia->semaforo_instancia, 0, 0);

	instancia_agregar_a_activas(instancia);
	return instancia;
}

void levantar_semaforo(void* instancia) {
	sem_post(&((t_instancia*) instancia)->semaforo_instancia);
}

/*
 * Realiza la compactacion en todas las instancias
 */
void realizar_compactacion() { //todo ver como esperar a que todas compacten
	log_info(logger, "Todas las instancias compactaran...");
	pthread_mutex_lock(&mutex_instancias_disponibles);
	int cantidad_instancias = list_size(lista_instancias_disponibles);
	log_debug(logger, "Hay %d instancias que compactaran", cantidad_instancias);
	list_iterate(lista_instancias_disponibles, levantar_semaforo);
	pthread_mutex_unlock(&mutex_instancias_disponibles);
	n_waits(&semaforo_compactacion, cantidad_instancias);
	log_info(logger, "La compactación ha finalizado");
}

t_status_clave instancia_solicitar_valor_de_clave(t_instancia* instancia,
		char* clave, char** valor) {
	pthread_mutex_lock(&instancia->mutex_comunicacion);
	if (enviar_operacion_unaria(instancia->socket, SOLICITUD_VALOR, clave)
			< 0) {
		pthread_mutex_unlock(&instancia->mutex_comunicacion);
		return INSTANCIA_CAIDA;
	}
	t_protocolo respuesta_instancia = recibir_cod_operacion(instancia->socket);

	switch (respuesta_instancia) {
	case ERROR_CONEXION:
		pthread_mutex_unlock(&instancia->mutex_comunicacion);
		return INSTANCIA_CAIDA;
	case VALOR_NO_ENCONTRADO:
		pthread_mutex_unlock(&instancia->mutex_comunicacion);
		return INSTANCIA_NO_TIENE_CLAVE;
	case VALOR_ENCONTRADO:
		;
		int respuesta = try_recibirPaqueteVariable(instancia->socket,
				(void**) valor);
		pthread_mutex_unlock(&instancia->mutex_comunicacion);
		if (respuesta <= 0) {
			free(*valor);
			return INSTANCIA_CAIDA;
		}
		return INSTANCIA_OK;
	default:
		//si llegaste hasta aca es porque algo salio mal
		log_warning(logger, "Mensaje no esperado de la instancia");
		return INSTANCIA_CAIDA;
	}
}
