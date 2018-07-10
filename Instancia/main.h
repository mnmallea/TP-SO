#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <commons/log.h>
#include "config_instancia.h"
#include "../syntax-commons/my_socket.h"
#include "../syntax-commons/conexiones.h"
#include "cfg_almacenamiento.h"
#include "almacenamiento.h"
#include "tabla_entradas.h"
#include "../syntax-commons/protocol.h"
#include "instancia.h"
#include "tabla_entradas.h"
#include <pthread.h>
#define LOG_LEVEL LOG_LEVEL_TRACE

config configuracion;
t_log *logger;



#endif /* MAIN_H_ */
