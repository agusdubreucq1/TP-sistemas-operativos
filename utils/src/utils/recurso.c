/*
 * recurso.c
 *
 *  Created on: May 25, 2023
 *      Author: utnso
 */

#include "recurso.h"

t_recurso* crear_recurso(char nombre[20], uint32_t cantidad){
	t_recurso* recurso = malloc(sizeof(t_recurso));
	strcpy(recurso->nombre,nombre);
	recurso->cantidad = cantidad;
	recurso->listaBloqueados = list_create();

	return recurso;
}

void imprimir_recurso(t_recurso* recurso){
	printf("Recurso: %s", recurso->nombre);
	printf("Cantidad: %d", recurso->cantidad);
	//printf("Procesos Bloqueados: \n");
}
