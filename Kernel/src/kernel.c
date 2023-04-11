#include "kernel.h"
#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>

int main(void){

	kernel_logger = iniciar_logger("./../logs/logKernel.log", "Kernel");
	kernel_config = iniciar_config("./../config/Kernel.config");

	leer_configs(kernel_config, kernel_logger);

	log_info(kernel_logger, "¡Kernel iniciado correctamente!");

	server_kernel = iniciar_servidor();
	log_info(kernel_logger, "Servidor listo para recibir al cliente");

	pthread_create(&atender_consolas, NULL, recibirProcesos, NULL);
	pthread_join(atender_consolas, NULL);

	return EXIT_SUCCESS;
}

void* recibirProcesos() {
	while (1) {
		int cliente_consola = esperar_cliente(server_kernel);

		uint32_t resultOk = 0;
		uint32_t resultError = -1;

		recv(cliente_consola, &handshake, sizeof(uint32_t), MSG_WAITALL);
		if(handshake == 1)
		   send(cliente_consola, &resultOk, sizeof(uint32_t), 0);
		else
		   send(cliente_consola, &resultError, sizeof(uint32_t), 0);

		t_list* lista;
		int cod_op = recibir_operacion(cliente_consola);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_consola);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_consola);
			log_info(kernel_logger, "Me llegaron los siguientes valores:");
			list_iterate(lista, (void*)iterator);
			break;
		}
	}
	return "";
}


void iterator(char* value) {
	log_info(kernel_logger,"%s", value);
}
