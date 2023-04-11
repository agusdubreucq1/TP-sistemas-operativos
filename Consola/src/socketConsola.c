/*
 * socketConsola.c
 *
 *  Created on: Apr 10, 2023
 *      Author: utnso
 */


/*
 * socketConsola.c
 *
 *  Created on: Apr 8, 2023
 *      Author: utnso
 */

#include "socketConsola.h"

int crear_conexion(char *ip, char* puerto)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	int resultado_conexion = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);

	while(resultado_conexion == -1){
		log_info(consola_logger, "Hubo un fallo al conectarse a la direccion %s:%s, reintentando...", ip, puerto);
		sleep(1);
		resultado_conexion = connect(socket_cliente, server_info -> ai_addr, server_info -> ai_addrlen );
	}

	freeaddrinfo(server_info);

	return socket_cliente;
}

t_paquete* crear_paquete(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	crear_buffer(paquete);
	return paquete;
}

void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente, t_log *logger)
{
	int bytes = paquete->buffer->size + 2*sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	log_info(consola_logger,"Datos enviados, esperando respuesta de Kernel...");

	uint32_t respuesta;
	recv(socket_cliente, &respuesta, sizeof(uint32_t), MSG_WAITALL);

	if(respuesta == bytes){
		log_info(consola_logger,"Datos enviados correctamente");
	}
	else
	{
		log_error(consola_logger,"Ha ocurrido un error en la comunicacion entre Kernel y Consola");
	}

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

void handshake(int conexion){
	uint32_t handshake = 1;
	uint32_t result;

	send(conexion, &handshake, sizeof(uint32_t), 0);
	recv(conexion, &result, sizeof(uint32_t), MSG_WAITALL);

	if(result == 0)
		log_info(consola_logger,"Conexion establecida");
	else
		log_error(consola_logger,"Error en la comunicacion");
}
