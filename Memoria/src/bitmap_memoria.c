/*
 * bitmap.c
 *
 *  Created on: Jun 16, 2023
 *      Author: utnso
 */

#include "bitmap_memoria.h"
#include <commons/collections/list.h>

void inicializar_bitmap(){

    char* bloque = (char*) malloc(atoi(tam_memoria)/8);

	bitmap = bitarray_create_with_mode(bloque , atoi(tam_memoria)/8, MSB_FIRST);
	liberar_bitmap(0, atoi(tam_memoria));
	//imprimir_bitmap(bitmap);
	free(bloque);
}

void imprimir_bitmap(){

	for (int i = 0; i < bitmap->size*8; i++) {
    	//bitarray_clean_bit(bitmap, i);
        printf("%d", bitarray_test_bit(bitmap, i));
    }
}

int validar_huecos_libres(int inicio, int cant) {
	int huecos_validos = 1;
	for (int i=inicio; i<inicio+cant; i++) {
		if (bitarray_test_bit(bitmap, i) == 1) {
			huecos_validos = 0;
		}
	}
	return huecos_validos;
}

void ocupar_bitmap(int inicio, int cant) {
	if (validar_huecos_libres(inicio, cant)) {
		for (int i=inicio; i<inicio+cant; i++) {
				bitarray_set_bit(bitmap, i);
			}
	}
	else {
		printf("\nLos huecos donde se desea escribir no estan libres");
	}

}

void liberar_bitmap(int inicio, int cant) {
	for (int i=inicio; i<inicio+cant; i++) {
		bitarray_clean_bit(bitmap, i);
		}
}

int first_fit_bitmap(int tamanio) {

	int cont_huecos_libres = 0;

	for (int i = 0; i < bitmap->size*8; i++) {
		if (cont_huecos_libres == tamanio) {
			return i-tamanio;
			//Devuelve la primera posicion tal que bitmap[posicion] esta libre y tiene *tamanio* huecos libres contiguos
		}
		else {
			if (bitarray_test_bit(bitmap, i) == 0) {
				cont_huecos_libres++;
			}
			else {
				cont_huecos_libres = 0;
			}
		}
	}
	return -1; //No hay disponibilidad contigua del bitmap
}

int best_fit_bitmap(int tamanio) {

    int posicion_best_fit = -1;
    int best_fit_size = bitmap->size*8 + 1;
    int fit_size_actual = 0;
    int posicion_actual = -1;

    for (int i = 0; i < bitmap->size*8; i++) {
            if (bitarray_test_bit(bitmap, i) == 0) {
                if (posicion_actual == -1) {
                	posicion_actual = i;
                }
                fit_size_actual++;
            } else {
                if (fit_size_actual >= tamanio && fit_size_actual < best_fit_size) {
                	posicion_best_fit = posicion_actual;
                	best_fit_size = fit_size_actual;
                }
                posicion_actual = -1;
                fit_size_actual = 0;
            }
        }

        if (fit_size_actual >= tamanio && fit_size_actual < best_fit_size) {
        	posicion_best_fit = posicion_actual;
        }

    return posicion_best_fit;
}

int worst_fit_bitmap(int tamanio) {

    int posicion_worst_fit = -1;
    int worst_fit_size = -1;
    int fit_size_actual = 0;
    int posicion_actual = -1;

    for (int i = 0; i < bitmap->size*8; i++) {
        if (bitarray_test_bit(bitmap, i) == 0) {
            if (posicion_actual == -1) {
            	posicion_actual = i;
            }
            fit_size_actual++;
        } else {
            if (fit_size_actual >= tamanio && fit_size_actual > worst_fit_size) {
            	posicion_worst_fit = posicion_actual;
            	worst_fit_size = fit_size_actual;
            }
            posicion_actual = -1;
            fit_size_actual = 0;
        }
    }

    if (fit_size_actual >= tamanio && fit_size_actual > worst_fit_size) {
    	posicion_worst_fit = posicion_actual;
    }

    return posicion_worst_fit;
}

int devolver_posicion_bitmap_segun_direccion(void *direccion_memoria) {
	printf("\nEntre a devolver_posicion_bitmap_segun_direccion!");
	for (int i=0; i<bitmap->size*8; i++) {
		if ((memoria_fisica+i) == direccion_memoria) {
			printf("\n%p == %p\nEl hueco libre es: %d", memoria_fisica, direccion_memoria, i);
			return i;
		}
	}

	return -1;
}