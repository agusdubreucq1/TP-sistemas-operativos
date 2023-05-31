#include <stdio.h>
#include <stdlib.h>

#include "Memoria.h"


int main(void) {

	//signal(SIGINT, cerrar_conexiones);

	memoria_logger = iniciar_logger("../../logs/logMemoria.log", "Memoria");

	if (memoria_logger == NULL){
		exit(1);
	}

	memoria_config = iniciar_config("../../config/Memoria.config", "Memoria");

	if (memoria_config == NULL){
		exit(2);
	}

	leer_configs(memoria_config, memoria_logger);
	log_info(memoria_logger, "¡Memoria iniciado correctamente!");

	server_memoria = iniciar_servidor(IP_SERVER, puerto_escucha, memoria_logger);
	log_info(memoria_logger, "Servidor listo para recibir al cliente");

	sem_init(&semaforo_conexiones, 0, 0);
	//sem_wait(&semaforo_conexiones);
	//sem_wait(&semaforo_conexiones);

	pthread_create(&hilo_estructuras, NULL, (void *) crear_estructuras, NULL);
	pthread_detach(hilo_estructuras);

	pthread_create(&hilo_conexion_FileSystem, NULL, atenderFileSystem, NULL);
	pthread_detach(hilo_conexion_FileSystem);

	pthread_create(&hilo_conexion_CPU, NULL, atenderCPU, NULL);
	pthread_detach(hilo_conexion_CPU);

	pthread_create(&hilo_conexion_Kernel, NULL, atenderKernel, NULL);
	pthread_join(hilo_conexion_Kernel, NULL);


	return EXIT_SUCCESS;
}

void* atenderKernel(){

	int socket_kernel = abrir_socket();
	sem_post(&semaforo_conexiones);

	while(1){
		int cod_op = recibir_operacion(socket_kernel);
		switch (cod_op) {
		case MENSAJE:
			recibir_instruccion(socket_kernel, memoria_logger);
			break;
		}
	}
	return "";
}


void* atenderCPU(){

	int socket_cpu = abrir_socket();
	sem_post(&semaforo_conexiones);

	while(1){
		int cod_op = recibir_operacion(socket_cpu);
		switch (cod_op) {
		case MENSAJE:
			recibir_instruccion(socket_cpu, memoria_logger);
			break;
		}
	}
	return "";
}

void* atenderFileSystem(){

	int socket_filesystem = abrir_socket();
	sem_post(&semaforo_conexiones);

	while(1){
		int cod_op = recibir_operacion(socket_filesystem);
		switch (cod_op) {
		case MENSAJE:
			recibir_instruccion(socket_filesystem, memoria_logger);
			break;
		}
	}
	return "";
}

int abrir_socket(){
	int socket = esperar_cliente(server_memoria, memoria_logger);

	uint32_t resultOk = 0;
	uint32_t resultError = -1;

	recv(socket, &respuesta, sizeof(uint32_t), MSG_WAITALL);
	if(respuesta == 1)
	   send(socket, &resultOk, sizeof(uint32_t), 0);
	else
	   send(socket, &resultError, sizeof(uint32_t), 0);

	return socket;
}


void crear_estructuras(){

	sem_wait(&semaforo_conexiones);
	sem_wait(&semaforo_conexiones);
	sem_wait(&semaforo_conexiones);

	//ACA ARMAMOS LAS ESTRUCTURAS

}

void cerrar_conexiones(){
	printf("\ncerrando conexiones\n");

	close(server_memoria);
	//close(socket_Kernel);
	printf("cerre conexiones");
	exit(1);
}

