#ifndef MAIN_H_
#define MAIN_H_

#include <commons/log.h>

#include "config_instancia.h"

#define LOG_LEVEL LOG_LEVEL_DEBUG

config configuracion;
t_log *logger;
char *obtenerNombre(char *path);
pthread_t thread_dumper;
pthread_mutex_t mutex_operacion;

#endif /* MAIN_H_ */
