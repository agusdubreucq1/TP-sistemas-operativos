/*
 * socketConsola.h
 *
 *  Created on: Apr 10, 2023
 *      Author: utnso
 */

#ifndef SOCKETCONSOLA_H_
#define SOCKETCONSOLA_H_

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<commons/log.h>

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

// ------------------------------------------------------------------------------------------
// -- Logger del proceso --
// ------------------------------------------------------------------------------------------

extern t_log* consola_logger;



int crear_conexion(char* ip, char* puerto);
//void enviar_mensaje(char* mensaje, int socket_cliente);
void handshake(int);
t_paquete* crear_paquete(void);
void crear_buffer(t_paquete* paquete);
void* serializar_paquete(t_paquete* paquete, int bytes);
//t_paquete* crear_super_paquete(void);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente, t_log *logger);
void liberar_conexion(int socket_cliente);
void eliminar_paquete(t_paquete* paquete);

#endif /* SOCKETCONSOLA_H_ */
