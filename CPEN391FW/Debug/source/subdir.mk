################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/Bluetooth.c \
../source/C391main.c \
../source/GraphicsTest.c \
../source/HPS.c \
../source/UART.c \
../source/WIFI.c 

OBJS += \
./source/Bluetooth.o \
./source/C391main.o \
./source/GraphicsTest.o \
./source/HPS.o \
./source/UART.o \
./source/WIFI.o 

C_DEPS += \
./source/Bluetooth.d \
./source/C391main.d \
./source/GraphicsTest.d \
./source/HPS.d \
./source/UART.d \
./source/WIFI.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C Compiler 5'
	armcc --c99 -O0 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


