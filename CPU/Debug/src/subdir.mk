################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/buffer.c \
../src/ciclo_instruccion.c \
../src/config.c \
../src/cpu.c \
../src/instrucciones.c \
../src/paquete_a_enviar.c \
../src/registros_cpu.c \
../src/servidor_cpu.c 

C_DEPS += \
./src/buffer.d \
./src/ciclo_instruccion.d \
./src/config.d \
./src/cpu.d \
./src/instrucciones.d \
./src/paquete_a_enviar.d \
./src/registros_cpu.d \
./src/servidor_cpu.d 

OBJS += \
./src/buffer.o \
./src/ciclo_instruccion.o \
./src/config.o \
./src/cpu.o \
./src/instrucciones.o \
./src/paquete_a_enviar.o \
./src/registros_cpu.o \
./src/servidor_cpu.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2023-1c-Simple-y-Funciona/utils/src" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/buffer.d ./src/buffer.o ./src/ciclo_instruccion.d ./src/ciclo_instruccion.o ./src/config.d ./src/config.o ./src/cpu.d ./src/cpu.o ./src/instrucciones.d ./src/instrucciones.o ./src/paquete_a_enviar.d ./src/paquete_a_enviar.o ./src/registros_cpu.d ./src/registros_cpu.o ./src/servidor_cpu.d ./src/servidor_cpu.o

.PHONY: clean-src

