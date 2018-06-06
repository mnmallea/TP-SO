################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../algoritmos_distribucion.c \
../config_coordinador.c \
../error.c \
../instancia.c \
../log_operaciones.c \
../main.c \
../servidor.c \
../sincronizacion.c 

OBJS += \
./algoritmos_distribucion.o \
./config_coordinador.o \
./error.o \
./instancia.o \
./log_operaciones.o \
./main.o \
./servidor.o \
./sincronizacion.o 

C_DEPS += \
./algoritmos_distribucion.d \
./config_coordinador.d \
./error.d \
./instancia.d \
./log_operaciones.d \
./main.d \
./servidor.d \
./sincronizacion.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


