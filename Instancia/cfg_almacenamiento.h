/*cfg_almacenamiento se utiliza solo cuando
 * hago el handshake con el coordinador, para recibir
 * la cantidad de entradas y el tamaño de c/u
 */
#ifndef CFG_ALMACENAMIENTO_H_
#define CFG_ALMACENAMIENTO_H_

#include <commons/string.h>
#include <stdlib.h>
#include "../syntax-commons/deserializador.h"
#include "../syntax-commons/serializador.h"
#include "../syntax-commons/conexiones.h"
#include "config_instancia.h"
#include "almacenamiento.h"
#include <commons/log.h>

typedef struct {
	unsigned int	tamanioEntrada;
	unsigned int	totalEntradas;
} almacenamiento_cfg;
almacenamiento_cfg cfgAlmacenamiento;
extern 	t_log *logger;
extern config configuracion;

/*  *@NAME: configurarAlmacenamiento
	* @DESC: crea un socket por el cual se conecta con el coordinador, le manda su nombre "instancia1"
	* y de esa manera el coordinador se da cuenta que es la instancia y le manda la cantidad de entradas
	* y el tamanio por una estructura, luego se inicializa el almacenamiento
	* @PARAMS:
	* 			ip
	* 			puerto
	*/
void configurarAlmacenamiento(int socketCoordinador);
/*  *@NAME: obtenerEntradasTotales()
	* @DESC: da la cantidad de entradas que me paso el coordinador
	* @PARAMS:-
	*/
unsigned int obtenerEntradasTotales();
/*  *@NAME: obtenerTamanioEntrada()
	* @DESC: da el tamaño de las entradas que me paso el coordinador
	* @PARAMS:-
	*/
unsigned int obtenerTamanioEntrada();


#endif /* CFG_ALMACENAMIENTO_H_ */
