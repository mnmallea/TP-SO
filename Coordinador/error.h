/*
 * error.h
 *
 *  Created on: 16 abr. 2018
 *      Author: utnso
 */

#ifndef ERROR_H_
#define ERROR_H_

#include <commons/log.h>
#include <stdlib.h>
#include "config_coordinador.h"

extern t_log *logger;
extern config *configuracion;

void exit_error_with_msg(char* msg);

#endif /* ERROR_H_ */
