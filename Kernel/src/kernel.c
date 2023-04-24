#include "kernel.h"


int main(void){

	kernel_logger = iniciar_logger("../../logs/logKernel.log", "Kernel");

	if (kernel_logger == NULL){
		exit(1);
	}

	kernel_config = iniciar_config("../../config/Kernel.config", "Kernel");

	if (kernel_config == NULL){
		exit(2);
	}

	leer_configs(kernel_config, kernel_logger);
	log_info(kernel_logger, "¡Kernel iniciado correctamente!");

	server_kernel = iniciar_servidor(IP_SERVER, puerto_escucha, kernel_logger);
	log_info(kernel_logger, "Servidor listo para recibir al cliente");

	pthread_create(&conexionFileSystem, NULL, conectarFileSystem, NULL);
	pthread_detach(conexionFileSystem);
	pthread_create(&conexionCPU, NULL, conectarCPU, NULL);
	pthread_detach(conexionCPU);
	pthread_create(&conexionMemoria, NULL, conectarMemoria, NULL);
	pthread_detach(conexionMemoria);
	pthread_create(&atender_consolas, NULL, recibirProcesos, NULL);
	pthread_join(atender_consolas, NULL);

	return EXIT_SUCCESS;
}

void* conectarFileSystem(){
	socket_fileSystem = crear_conexion(ip_filesystem, puerto_filesystem, kernel_logger, "File System");
	handshake(socket_fileSystem, 1, kernel_logger, "File System");

	enviar_mensaje("Soy el Kernel", socket_fileSystem);
	return "";
}

void* conectarCPU(){
	socket_cpu = crear_conexion(ip_cpu, puerto_cpu, kernel_logger, "CPU");
	handshake(socket_cpu, 1, kernel_logger, "CPU");

	enviar_mensaje("Soy el Kernel", socket_cpu);
	return "";
}

void* conectarMemoria(){
	socket_memoria = crear_conexion(ip_memoria, puerto_memoria, kernel_logger, "Memoria");
	handshake(socket_memoria, 1, kernel_logger, "Memoria");

	enviar_mensaje("Soy el Kernel", socket_memoria);
	return "";
}

void* recibirProcesos() {
	while (1) {
		int cliente_consola = esperar_cliente(server_kernel, kernel_logger);

		uint32_t resultOk = 0;
		uint32_t resultError = -1;

		recv(cliente_consola, &respuesta, sizeof(uint32_t), MSG_WAITALL);
		if(respuesta == 1)
		   send(cliente_consola, &resultOk, sizeof(uint32_t), 0);
		else
		   send(cliente_consola, &resultError, sizeof(uint32_t), 0);

		t_list* lista;
		int cod_op = recibir_operacion(cliente_consola);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_consola, kernel_logger);
			break;
		case PAQUETE:
			uint32_t tamanio;
			lista = recibir_paquete(cliente_consola, &tamanio);
			log_info(kernel_logger, "Paquete recibido con exito");
			//list_iterate(lista, (void*)iterator);
			t_pcb* pcb = crear_pcb(0, lista, estimacion_inicial, tamanio);
			print_pcb(pcb);
			break;
		}


	}
	return "";
}


void iterator(char* value) {
	log_info(kernel_logger,"%s", value);
}
