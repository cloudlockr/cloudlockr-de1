################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/UART.c \
../source/WIFI.c \
../source/aesHwAcc.c \
../source/bluetoothService.c \
../source/cloudlockrMain.c \
../source/hexService.c \
../source/hpsService.c \
../source/processingService.c \
../source/tests.c \
../source/verificationService.c 

OBJS += \
./source/UART.o \
./source/WIFI.o \
./source/aesHwAcc.o \
./source/bluetoothService.o \
./source/cloudlockrMain.o \
./source/hexService.o \
./source/hpsService.o \
./source/processingService.o \
./source/tests.o \
./source/verificationService.o 

C_DEPS += \
./source/UART.d \
./source/WIFI.d \
./source/aesHwAcc.d \
./source/bluetoothService.d \
./source/cloudlockrMain.d \
./source/hexService.d \
./source/hpsService.d \
./source/processingService.d \
./source/tests.d \
./source/verificationService.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM C Compiler 5'
	armcc -I"C:\Users\danie\Documents\school\cpen391\AES\CPEN391FW\include" --c99 -O0 -g --md --depend_format=unix_escaped -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


