################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../almacenamiento.c \
../cfg_almacenamiento.c \
../config_instancia.c \
../main.c \
../tabla_entradas.c 

OBJS += \
./almacenamiento.o \
./cfg_almacenamiento.o \
./config_instancia.o \
./main.o \
./tabla_entradas.o 

C_DEPS += \
./almacenamiento.d \
./cfg_almacenamiento.d \
./config_instancia.d \
./main.d \
./tabla_entradas.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


