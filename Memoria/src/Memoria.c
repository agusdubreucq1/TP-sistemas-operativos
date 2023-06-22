#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

	sem_init(&sem_conexiones, 0, 0);
	sem_init(&sem_kernel, 0, 0);
	sem_init(&sem_cpu, 0, 0);

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
	sem_wait(&sem_kernel);
	socket_kernel = abrir_socket();
	sem_post(&sem_conexiones);

	while(1){
		int cod_op = recibir_operacion(socket_kernel);
		switch (cod_op) {
		case MENSAJE:
			char* recibi = recibir_instruccion(socket_kernel, memoria_logger);
			ejecutar_instruccion(recibi);
			break;
		case PAQUETE:
			int size;
			void* buffer;
			int* tam_recibido= malloc(sizeof(int));
			tam_recibido = 0;
			buffer = recibir_buffer(&size, socket_kernel);

			tabla_recibida = deserializar_segmentos(buffer, tam_recibido);
			log_info(memoria_logger, "Recibi Tabla de Segmentos - PID: %d", tabla_recibida->pid);

			*tam_recibido+=2*sizeof(int);
			send(socket_kernel, tam_recibido, sizeof(int), 0);
			break;
		}
	}
	return "";
}

void* atenderCPU(){
	sem_wait(&sem_cpu);
	socket_cpu = abrir_socket();
	sem_post(&sem_kernel);

	while(1){
		int cod_op = recibir_operacion(socket_cpu);
		switch (cod_op) {
		case MENSAJE:
			char* motivo = recibir_instruccion(socket_cpu, memoria_logger);
			ejecutar_instruccion(motivo);
			break;
		}
	}
	return "";
}

void* atenderFileSystem(){

	socket_filesystem = abrir_socket();
	sem_post(&sem_cpu);

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


void ejecutar_instruccion(char* motivo){
	char** parametros = string_split(motivo, " ");
	codigo_instruccion cod_instruccion = obtener_codigo_instruccion(parametros[0]);

	switch(cod_instruccion) {
	case INICIAR:
		parametros = string_split(motivo, " ");
		t_tabla_segmentos* tabla = crear_tabla(atoi(parametros[1]));
		enviar_segmentos(tabla, socket_kernel);
		break;
	case CREATE_SEGMENT:
		parametros = string_split(motivo, " ");
		char* mensaje = elegir_hueco(atoi(parametros[2]));

		//char* mensaje = "OUT";

		if(!(strcmp(mensaje, "OUT"))){
			log_error(memoria_logger, "Out of memory - Cerrando PID: %s", parametros[3]);
			log_info(memoria_logger, "Eliminación de Proceso PID: %s", parametros[3]);
			t_tabla_segmentos* tabla_a_borrar = buscar_tabla_proceso(atoi(parametros[3]));
			borrar_tabla(tabla_a_borrar);
			list_remove_element(tablas_segmentos, tabla_a_borrar);
			enviar_mensaje("OUT", socket_kernel);
		} else if(!(strcmp(mensaje, "COMPACT"))){
			enviar_mensaje("COMPACT", socket_kernel);
		} else {
			void* base_elegida = (void*) (memoria_fisica + atoi(mensaje));
			void* limite_elegido = (void*) (base_elegida + atoi(parametros[2]));

			//t_segmento* segmento_nuevo = malloc(sizeof(t_segmento));
			//segmento_nuevo = crear_segmento(base_elegida, limite_elegido);
			ocupar_bitmap(base_elegida - memoria_fisica, atoi(parametros[2]));
			log_info(memoria_logger, "PID: %s - Crear Segmento: %s - Base: %p - TAMAÑO: %s", parametros[3], parametros[1], base_elegida, parametros[2]);
			t_tabla_segmentos* tabla_buscada = buscar_tabla_proceso(atoi(parametros[3]));
			t_segmento* segmento_nuevo = list_get(tabla_buscada->segmentos, atoi(parametros[1]));
			segmento_nuevo->direccion_base = base_elegida;
			segmento_nuevo->limite = limite_elegido;
			//list_add_in_index(tabla_buscada->segmentos, atoi(parametros[1]), segmento_nuevo);
			//imprimir_bitmap(bitmap);
			//imprimir_segmentos(tabla_buscada);

			char motivo[30] = "SEGMENT ";
			char numero[20];
			sprintf(numero, "%p", base_elegida);
			strcat(motivo, numero);

			enviar_mensaje(motivo, socket_kernel);
		}
		break;
	case DELETE_SEGMENT:
		parametros = string_split(motivo, " ");
		t_tabla_segmentos* tabla_del_segmento = buscar_tabla_proceso(atoi(parametros[2]));
		t_segmento* segmento_a_borrar = list_get(tabla_del_segmento->segmentos, atoi(parametros[1]));
		borrar_segmento(segmento_a_borrar->direccion_base, segmento_a_borrar->limite);
		int size = segmento_a_borrar->limite - segmento_a_borrar->direccion_base;
		log_info(memoria_logger, "PID: %s - Eliminar Segmento: %s - Base: %p - TAMAÑO: %u", parametros[2], parametros[1], segmento_a_borrar->direccion_base, size);
		segmento_a_borrar->direccion_base = NULL;
		segmento_a_borrar->limite = NULL;

		//imprimir_segmentos(tabla_del_segmento);
		enviar_segmentos(tabla_del_segmento, socket_kernel);
		//imprimir_bitmap(bitmap);
		//list_remove_element(self, element)(tabla_del_segmento->segmentos);
		break;
	case FINALIZAR:
		parametros = string_split(motivo, " ");
		t_tabla_segmentos* tabla_a_finalizar = buscar_tabla_proceso(atoi(parametros[1]));
		borrar_tabla(tabla_a_finalizar);
		list_remove_element(tablas_segmentos, tabla_a_finalizar);
		log_info(memoria_logger, "Eliminación de Proceso PID: %s", parametros[1]);
		break;
	case MOV_IN:
		parametros = string_split(motivo, " ");
		log_info(memoria_logger, "ENTRE POR MOV_IN");

		break;
	case MOV_OUT:

		parametros = string_split(motivo, " ");
<<<<<<< HEAD
		log_info(memoria_logger,"%s", parametros[0]); //MOV_OUT
		log_info(memoria_logger,"%s", parametros[1]); //dir_fisica
		log_info(memoria_logger,"%s", parametros[2]); //HOLA (si es AX)
		log_info(memoria_logger,"%s", parametros[3]); //SIZE DEL REGISTRO

		//Se graba en la direccion recibida (parametros[1]) el valor recibido, con el peso de ese registro
		memcpy(parametros[1],parametros[2],atoi(parametros[3]));

=======
		//imprimir_bitmap();
		void *direccion_fisica = (void *)strtoul(parametros[1], NULL, 16);
		char *valor = (char *)parametros[2];
		int tamanio = atoi(parametros[3]);
		printf("\nString: %s Direccion: %lu Direccion 2: %p", parametros[1], strtoul(parametros[1], NULL, 16), (void *)strtoul(parametros[1], NULL, 16));
		memcpy(direccion_fisica, valor, tamanio);
		//imprimir_memoria_segun_base_y_tam((void *)(uintptr_t)parametros[1], atoi(parametros[3]));
		imprimir_memoria();
		//Se graba en la direccion recibida (parametros[1]) + desplazamiento (parametros[2]), el valor de parametros[3] (el registro AX), la cantidad que pesa el parametro[3]
>>>>>>> sigoConMemoria

		break;
	default:
		break;
	}
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
	sem_wait(&sem_conexiones);

	memoria_fisica = reservar_espacio_memoria();
	memoria_libre = atoi(tam_memoria);
	log_info(memoria_logger, "Espacio reservado: %s Bytes -> Direccion: %p", tam_memoria, memoria_fisica);
	tablas_segmentos = list_create();
	inicializar_bitmap();
	segmento_cero = crear_segmento(memoria_fisica, memoria_fisica + atoi(tam_segmento_0));
}

void cerrar_conexiones(){
	printf("\ncerrando conexiones\n");

	close(server_memoria);
	//close(socket_Kernel);
	printf("cerre conexiones");
	exit(1);
}

