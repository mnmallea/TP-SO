/*
 * planificacion.c
 *
 *  Created on: 17 may. 2018
 *      Author: utnso
 */

#include "planificacion.h"

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <semaphore.h>
#include <sys/select.h>

#include "../syntax-commons/my_socket.h"
#include "../syntax-commons/protocol.h"
#include "selector.h"

/* SE DEBE REPLANIFICAR CUANDO:
 *
 * SI USO HRRN/SJF SIN DESALOJO:
 * -SE BLOQUEA UN PROCESO
 * -FINALIZA UN PROCESO
 *
 * SI USO SJF
 * -SE BLOQUEA UN PROCESO
 * -FINALIZA UN PROCESO
 * -NUEVO PROCESO LLEGA (o uno se desbloquea)
 */

int flag = 1;
bool hay_nuevo_esi = false;
bool hay_esi_bloqueado = false;
bool hay_esi_finalizado = false;
t_esi* esi_a_matar;

bool algoritmo_debe_planificar() {
	switch (configuracion.algoritmo) {
	case FIFO:
	case SJFsD:
	case HRRN:
		return false;
	case SJFcD:
		return hay_nuevo_esi;
	default:
		log_error(logger, "Algoritmo desconocido");
		exit(EXIT_FAILURE);
	}
}

bool hay_que_planificar() {
	return esi_corriendo == NULL || algoritmo_debe_planificar();
}

void* planificar(void* _) {
	esi_corriendo = NULL;

	while (1) {
		pthread_mutex_lock(&mutex_pausa);
		if (planificacion_pausada) {
			pthread_mutex_unlock(&mutex_pausa);
			sem_wait(&pausa_planificacion);
		} else {
			pthread_mutex_unlock(&mutex_pausa);
		}

		log_trace(logger, "Estoy planificandoooo!!!!");

		if (hay_que_planificar()) {
			esi_corriendo = obtener_nuevo_esi_a_correr();
			log_debug(logger, "Proximo esi a correr: %d \n", esi_corriendo->id);
		} else {
			log_debug(logger, "Se continua corriendo: %d \n",
					esi_corriendo->id);
		}

		correr(esi_corriendo);
	}
	return NULL;

}

/*
 * @thread_safe
 */
t_esi *obtener_nuevo_esi_a_correr() {
	t_esi* prox_esi;

	sem_wait(&contador_esis);
	pthread_mutex_lock(&mutex_lista_esis_listos);
	if (configuracion.algoritmo == FIFO) {
		prox_esi = obtener_proximo_segun_fifo(lista_esis_listos);
	} else if (configuracion.algoritmo == SJFsD) {
		prox_esi = obtener_proximo_segun_sjf(lista_esis_listos);
	} else if (configuracion.algoritmo == SJFcD) {
		prox_esi = obtener_proximo_segun_sjf(lista_esis_listos);
	} else {
		prox_esi = obtener_proximo_segun_hrrn(lista_esis_listos);
	}
	hay_nuevo_esi = false;
	pthread_mutex_unlock(&mutex_lista_esis_listos);

	return prox_esi;

}

//thread safe
//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE LLEGO UN NUEVO ESI
void nuevo_esi(t_esi* esi) {
	pthread_mutex_lock(&mutex_lista_esis_listos);
	list_add(lista_esis_listos, esi);
	hay_nuevo_esi = true;
	pthread_mutex_unlock(&mutex_lista_esis_listos);
	sem_post(&contador_esis);
	log_debug(logger, "Llego/se desalojo/se desbloqueo un nuevo esi: %d \n",
			esi->id);
}

//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE EL ESI CORRIENDO FINALIZO
void finalizar_esi() {

	liberar_recursos(esi_corriendo);
	list_add(lista_esis_finalizados, esi_corriendo);

	log_debug(logger, "Termino el ESI id: %d \n", esi_corriendo->id);

	esi_corriendo = NULL;

}

void bloquear_esi(char* clave) {

	agregar_a_dic_bloqueados(clave, esi_corriendo);
	log_debug(logger, "Se bloqueo el esi corriendo: %d para la clave: %s \n",
			esi_corriendo->id, clave);
	esi_corriendo = NULL;
}

void agregar_a_dic_bloqueados(char* clave, t_esi *esi) {

	//No existe la clave, agrego esta nueva linea
	if (!dictionary_has_key(dic_esis_bloqueados, clave)) {
		t_list *primer_esi = list_create();
		list_add(primer_esi, esi);
		dictionary_put(dic_esis_bloqueados, clave, primer_esi);

	} else { //Existe la clave, agrego el esi a la lista de bloq

		t_list *lista_esis_bloq_esta_clave = dictionary_remove(
				dic_esis_bloqueados, clave);
		list_add(lista_esis_bloq_esta_clave, esi);
		dictionary_put(dic_esis_bloqueados, clave, lista_esis_bloq_esta_clave);

	}

}

//variable para memoria compartida
int id;

void bloquear_esi_por_consola(char* clave, int id_esi) {

	t_esi *esi_afectado = buscar_esi_por_id(id_esi);

	if (esi_afectado != NULL) { //valido que me haya devuelto algo coherente

		if (esi_afectado->id != esi_corriendo->id) {
			t_list* todos_los_esis_bloqueados =
					obtener_todos_los_esis_bloqueados();

			if (list_find(todos_los_esis_bloqueados, esi_con_este_id) != NULL) {
				log_debug(logger, "Ya se encuentra bloqueado ese esi");
			} else {
				agregar_a_dic_bloqueados(clave, esi_afectado);
				log_debug(logger,
						"Se bloqueo por consola el esi: %d para la clave: %s \n",
						esi_afectado->id, clave);
			}

			list_destroy(todos_los_esis_bloqueados);

		} else {
			//es el esi corriendo
		}

	} else {
		log_error(logger, "No existe ningun esi en el sistema con el id: %d",
				id);
	}

}

t_esi *buscar_esi_por_id(int id_esi){

	id = id_esi;
	t_esi *esi_a_devolver;
	//me fijo si es el esi que esta corriendo
	if (esi_corriendo->id == id) {
		esi_a_devolver = esi_corriendo;
	} else {
		//si no es el q esta corriendo lo busco en la lista de esis listos
		esi_a_devolver = list_find(lista_esis_listos, esi_con_este_id);

		if (esi_a_devolver == NULL) {
			//es un esi bloqueado

			t_list* todos_los_esis_bloqueados =
					obtener_todos_los_esis_bloqueados();

			esi_a_devolver = list_find(todos_los_esis_bloqueados,
					esi_con_este_id);
			list_destroy(todos_los_esis_bloqueados);
		}
	}

	return esi_a_devolver;

}

t_list *obtener_todos_los_esis_bloqueados() {
	t_list* lista = list_create();
	void agregar_a_lista_bloq(void* esi) {
		list_add(lista, esi);
	}
	void obtener_esis_bloq(char* c, void* data) {
		list_iterate((t_list*) data, agregar_a_lista_bloq);
	}

	dictionary_iterator(dic_esis_bloqueados, obtener_esis_bloq);

	return lista;
}

bool esi_con_este_id(void* esi) {
	return ((t_esi*) esi)->id == id;
}

//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE EL COORDINADOR LE INDICA QUE SE DESBLOQUIO UN RECURSO
void se_desbloqueo_un_recurso(char* clave) {

	if (dictionary_has_key(dic_clave_x_esi, clave)) {
		dictionary_remove(dic_clave_x_esi, clave);
	}

	if (dictionary_has_key(dic_esis_bloqueados, clave)) { //hay esis encolados
		t_list *lista_esis_bloq_esta_clave = dictionary_remove(
				dic_esis_bloqueados, clave);
		t_esi* esi_desbloq = list_remove(lista_esis_bloq_esta_clave, 0);

		if (list_size(lista_esis_bloq_esta_clave) != 0) { //agrego la lista de bloqueados solo si tiene algun esi
			dictionary_put(dic_esis_bloqueados, clave,
					lista_esis_bloq_esta_clave);
		}

		nuevo_esi(esi_desbloq);
	} else {
		log_debug(logger, "No estaba bloqueada esta clave");
	}

}

//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE EL COORDINADOR LE PREGUNTA SI UN ESI TIENE UNA CLAVE
bool esi_tiene_clave(char* clave) {

	if (dictionary_has_key(dic_clave_x_esi, clave)) {
		t_esi* esi_que_la_tomo = dictionary_get(dic_clave_x_esi, clave);
		return (esi_que_la_tomo->id == esi_corriendo->id);
	} else {
		return false;
	}

}

bool esta_tomada_x_otro_la_clave(char* clave) {

	if (dictionary_has_key(dic_clave_x_esi, clave)) { //ya esta tomada?
		t_esi* esi_que_la_tomo = dictionary_get(dic_clave_x_esi, clave);

		//el esi que la tiene es el actual?
		// si es el actual -> la tiene el, esta volviendo a hacer get
		// si no es el actual la tiene otro

		return !(esi_que_la_tomo->id == esi_corriendo->id);
	}

	return false; //no esta tomada

}

//FUNCION A LLAMAR CUANDO EL SELECT ESCUCHA QUE EL COORDINADOR ME INDICA QUE SE TOMO UNA NUEVA CLAVE
void nueva_clave_tomada_x_esi(char* clave) {

	//No existe la clave, agrego esta nueva linea
	if (!dictionary_has_key(dic_clave_x_esi, clave)) {
		dictionary_put(dic_clave_x_esi, clave, esi_corriendo); //aca estoy guardando el puntero al esi_corriendo verdad?

	} else { //Existe la clave, saco al que estaba y pongo al nuevo

		dictionary_put(dic_clave_x_esi, clave, esi_corriendo);
		//supongo que el put pisa lo que estaba

	}

}

void correr(t_esi* esi) {

	mandar_confirmacion(esi->socket);

	sem_wait(&respondio_esi_corriendo);
	list_iterate(lista_esis_listos, aumentar_viene_esperando);
	aumentar_viene_corriendo(esi_corriendo);

	switch (respuesta_esi_corriendo) { //mensajes de esis
	case EXITO:
		ya_termino_linea();
		break;
	case LINEA_SIZE:
		linea_size();
		break;
	case INTERPRETAR:
		interpretar();
		break;
	case ERROR_CONEXION:
		log_error(logger, "Error de conexion en el ESI %d", esi->id);
		/* no break */
	case ABORTA:
		fallo_linea();
		break;
	case FINALIZO_ESI:
		finalizar_esi();
		break;
	case BLOQUEO_ESI:
		bloquear_esi(clave_bloqueadora);
		free(clave_bloqueadora);
		break;
	case INSTANCIA_CAIDA_EXCEPTION:
		fallo_linea();
		break;
	default:
		break;
	}
}

void ya_termino_linea() {

//si leyo bien la linea
	log_debug(logger, "El esi %d se termino de leer una nueva linea \n",
			esi_corriendo->id);

}

void linea_size() {
//si leyo mal la linea
	log_debug(logger, "El ESI %d leyo una linea con mas de 40 caracteres\n",
			esi_corriendo->id);
	matar_esi_corriendo();
}

void interpretar() {
//si leyo mal la linea
	log_debug(logger, "No se pudo interpretar una linea en el esi %d\n",
			esi_corriendo->id);
	matar_esi_corriendo();
}

void fallo_linea() {
//si leyo mal la linea
	log_debug(logger, "Hubo una falla cuando el esi %d leyo una nueva linea \n",
			esi_corriendo->id);
	matar_esi_corriendo();
}

void matar_esi_corriendo() {
	liberar_recursos(esi_corriendo);
	matar_nodo_esi(esi_corriendo);
	esi_corriendo = NULL;
}

void sacar_al_esi_de_donde_este(t_esi* esi_to_kill){

	bool es_el_esi_a_matar(void* esi){
			return ((t_esi*)esi)->id == esi_to_kill->id;
		}

	t_esi *esi_encontrado = list_find(lista_esis_listos, es_el_esi_a_matar);


	void buscar_esi_a_matar(char* clave, void* lista){
		t_esi *esi_encontrado = list_find(lista, es_el_esi_a_matar);

		if(esi_encontrado != NULL){
			list_remove_by_condition(lista, es_el_esi_a_matar);
		}
	}

	if(esi_encontrado == NULL){
		//no era un esi en la lista de listos, lo busco en bloqueados
		dictionary_iterator(dic_esis_bloqueados, buscar_esi_a_matar);
	}else{
		//era un esi en la lista de listos
		list_remove_by_condition(lista_esis_listos, es_el_esi_a_matar);
	}

}

void aumentar_viene_esperando(void* esi) {
	((t_esi*) esi)->viene_esperando = ((t_esi*) esi)->viene_esperando + 1;
}

void aumentar_viene_corriendo(void* esi) {
	((t_esi*) esi)->dur_ult_raf = ((t_esi*) esi)->dur_ult_raf + 1;
}

void liberar_recursos(t_esi* esi_a_liberar) {

	t_list *lista_claves_a_desbloquear = list_create();

	void clave_esta_tomada_x_esi_a_liberar(char* clave, void* esi) {
		if (((t_esi*) esi)->id == esi_a_liberar->id) {
			list_add(lista_claves_a_desbloquear, clave);
		}
	}


	dictionary_iterator(dic_clave_x_esi, clave_esta_tomada_x_esi_a_liberar);

	list_iterate(lista_claves_a_desbloquear, liberar_clave);
	list_destroy(lista_claves_a_desbloquear);

}

void liberar_clave(void* clave) {
	se_desbloqueo_un_recurso((char*) clave);

}

void nueva_solicitud(int socket, char* clave) {

	t_protocolo cod_op;
	if (esta_tomada_x_otro_la_clave(clave)) {
//		bloquear_esi(clave); NO HAY QUE HACER ESTO ACA
		cod_op = BLOQUEO_ESI;
		clave_bloqueadora = strdup(clave);

	} else {
		nueva_clave_tomada_x_esi(clave);
		cod_op = EXITO;

	}

	enviar_cod_operacion(socket, cod_op);
}



typedef struct{
	int idEsi;
	char *retiene;
	char *espera;
}t_dl;

typedef struct{
	char *clave;
}t_clave_dl;

t_list * listaDL;
t_list *idsDL,*idsDLL;
t_dl *candidato;
t_esi *esiDL;
int rye=0;


void deadlock(){
 idsDL = list_create();
 dictionary_iterator(dic_clave_x_esi,itera_por_linea);
 list_destroy_and_destroy_elements(idsDL,(void*) free);
 }

void itera_por_linea(char *claveIncialTomada, void *esiInicial){
	rye=1;
	//primerEsi
	candidato = malloc(sizeof(t_dl));
	candidato->idEsi = ((t_esi*) esiInicial)->id;

	if(!list_any_satisfy(idsDL,esta)&&((t_esi*) esiInicial)->id!=-1){
		listaDL = list_create();
		idsDLL = list_create();
		candidato->retiene = claveIncialTomada;								//ya sabemos que retiene porque lo toma del diccionario de clave_x_esi
		dictionary_iterator(dic_esis_bloqueados, buscarClaveQEspera);   	//busco la espera el ESI
			if(!rye)														//flag para ver si se cumple retencion y espera
				goto DEST;
		//sgtes
		do{
			char * retieneProx=candidato->espera;
			candidato = malloc(sizeof(t_dl));
			esiDL=dictionary_get(dic_clave_x_esi,retieneProx);				//quien tiene la clave que espera el anterior?
			candidato->idEsi = ((t_esi*) esiDL)->id;						//el nuevo esi ahora es el actual
			candidato->retiene = retieneProx;								//ya sabemos que retiene porque lo toma del diccionario de clave_x_esi
			dictionary_iterator(dic_esis_bloqueados, buscarClaveQEspera);	//busco la clave que espera el ESI
			if(!rye)														//flag para ver si se cumple retencion y espera
				goto DEST;													//sino salta al fin ya que una condicion no se cumple

		}while(strcmp(claveIncialTomada,candidato->espera)!=0);				//se busca la espera circular

		list_iterate(idsDLL,mergearL);										//agrego los ids en dl a la lista global para evitar que los vuelvan a revisar en la proxima iteracion

		//print
		//printf header de la tabla
		list_iterate(listaDL,mostrarDL);

		DEST:list_destroy_and_destroy_elements(listaDL,(void*) free);
			 list_destroy_and_destroy_elements(idsDLL,(void*) free);
	}
}
int *idEsis;
void buscarClaveQEspera(char* claveQEspera, void* esisbloq){
	if (list_find(esisbloq,esta)!=NULL){
			candidato->espera = claveQEspera;
			list_add(listaDL,candidato);
			idEsis=malloc(sizeof(int));
			*(idEsis)=candidato->idEsi;
			list_add(idsDLL,idEsis);
		}
	else{
		rye=0;
	}
}

bool esta(void *esi){
	return ((t_esi*) esi)->id == candidato->idEsi;
}

void mergearL(void *id){
			list_add(idsDL,id);
}

void mostrarDL(void* candidato){
			printf("%d  |  %s   | %s",((t_dl*) candidato)->idEsi,((t_dl*) candidato)->retiene,((t_dl*) candidato)->espera);
}


