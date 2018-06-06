################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../algoritmos_planificacion.c \
../config_planificador.c \
../consola.c \
../main.c \
../planificacion.c \
../selector.c 

OBJS += \
./algoritmos_planificacion.o \
./config_planificador.o \
./consola.o \
./main.o \
./planificacion.o \
./selector.o 

C_DEPS += \
./algoritmos_planificacion.d \
./config_planificador.d \
./consola.d \
./main.d \
./planificacion.d \
./selector.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


