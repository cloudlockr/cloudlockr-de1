################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/UART.c \
../source/aesHwacc.c \
../source/altera_avalon_spi.c \
../source/bluetoothService.c \
../source/cloudlockrMain.c \
../source/hexService.c \
../source/hpsService.c \
../source/mpu9250.c \
../source/processingService.c \
../source/tests.c \
../source/verificationService.c \
../source/wifiService.c 

OBJS += \
./source/UART.o \
./source/aesHwacc.o \
./source/altera_avalon_spi.o \
./source/bluetoothService.o \
./source/cloudlockrMain.o \
./source/hexService.o \
./source/hpsService.o \
./source/mpu9250.o \
./source/processingService.o \
./source/tests.o \
./source/verificationService.o \
./source/wifiService.o 

C_DEPS += \
./source/UART.d \
./source/aesHwacc.d \
./source/altera_avalon_spi.d \
./source/bluetoothService.d \
./source/cloudlockrMain.d \
./source/hexService.d \
./source/hpsService.d \
./source/mpu9250.d \
./source/processingService.d \
./source/tests.d \
./source/verificationService.d \
./source/wifiService.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C Compiler 5'
	armcc -I"C:\Users\danie\Documents\school\cpen391\AES\CPEN391FW\include" --c99 -O0 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


