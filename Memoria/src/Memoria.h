#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "logger.h"
#include "config.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <pthread.h>


	t_log* memoria_logger;

// ------------------------------------------------------------------------------------------
// -- Variables del archivo de configuración --
// ------------------------------------------------------------------------------------------


	t_config* memoria_config;

	//int puerto_escucha, tam_memoria, tam_segmento_0, cant_segmentos, retardo_memoria, retardo_compactacion;
	char *algoritmo_asignacion, *puerto_escucha, *tam_memoria, *tam_segmento_0, *cant_segmentos;
	char *retardo_memoria, *retardo_compactacion;


//void leer_config(t_config*, t_log*);

	pthread_t atender_conexiones;

uint32_t handshake;
int server_memoria;
void* abrirSocket();


#endif
