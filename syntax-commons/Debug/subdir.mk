################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../conexiones.c \
../deserializador.c \
../my_socket.c \
../protocol.c \
../serializador.c 

OBJS += \
./conexiones.o \
./deserializador.o \
./my_socket.o \
./protocol.o \
./serializador.o 

C_DEPS += \
./conexiones.d \
./deserializador.d \
./my_socket.d \
./protocol.d \
./serializador.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


