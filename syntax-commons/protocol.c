#include "protocol.h"
#define MAX_STRING_LENGTH 40

char nombres_protocolo[][MAX_STRING_LENGTH + 1] = {
	"EXITO",
	"ERROR",
	"SOLICITUD_CLAVE",
	"BLOQUEO_CLAVE",
	"DESBLOQUEO_CLAVE",
	"BLOQUEO_ESI",
	"CORRER_ESI",
	"ESI_TIENE_CLAVE",
	"FINALIZO_ESI",
	"OP_GET",
	"OP_SET",
	"OP_STORE",
	"MATAR_INSTANCIA",
	"INSTANCIA_CAIDA_EXCEPTION",
	"CLAVE_INACCESIBLE_EXCEPTION",
	"CLAVE_NO_BLOQUEADA_EXCEPTION",
	"CLAVE_NO_IDENTIFICADA_EXCEPTION",
	"LINEA_INVALIDA",
	"CLAVE_SIZE",
	"INTERPRETAR",
	"ABORTA",
	"RELEVANTAR_INSTANCIA",
	"SOLICITUD_STATUS_CLAVE",
	"RESPUESTA_STATUS_CLAVE",
	"SOLICITUD_VALOR",
	"VALOR_ENCONTRADO",
	"VALOR_NO_ENCONTRADO",
	"INSTANCIA_COMPACTAR",
	"ERROR_CONEXION",
	"MURIO_ESI_CORRIENDO",
	"SOLICITUD_COMPACTACION"
};

char desc_status_clave[][MAX_STRING_LENGTH + 1] = {
	"Hay valor para la clave",
	"No hay valor para la clave",
	"La instancia esta disponible",
	"La instancia se encuentra caida",
	"No hay instancia asignada",
	"La instancia no posee la clave",
	"Se realizo la simulacion",
	"No se realizo la simulacion"
};

char* to_string_status_clave(t_status_clave codigo) {
	return desc_status_clave[codigo];
}

char* to_string_protocolo(t_protocolo codigo) {
	return nombres_protocolo[codigo];
}
