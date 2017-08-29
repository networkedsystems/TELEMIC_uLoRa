################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/LoRaMac-api-v3.c \
../src/LoRaMac.c \
../src/LoRaMacCrypto.c \
../src/adc-board.c \
../src/aes.c \
../src/board.c \
../src/cmac.c \
../src/delay.c \
../src/fifo.c \
../src/gpio-board.c \
../src/main_telemic.c \
../src/rtc-board.c \
../src/spi-board.c \
../src/stm32l0xx_hal_msp.c \
../src/stm32l0xx_it.c \
../src/sx1272-board.c \
../src/sx1272.c \
../src/sysIrqHandlers.c \
../src/timer.c \
../src/utilities.c 

OBJS += \
./src/LoRaMac-api-v3.o \
./src/LoRaMac.o \
./src/LoRaMacCrypto.o \
./src/adc-board.o \
./src/aes.o \
./src/board.o \
./src/cmac.o \
./src/delay.o \
./src/fifo.o \
./src/gpio-board.o \
./src/main_telemic.o \
./src/rtc-board.o \
./src/spi-board.o \
./src/stm32l0xx_hal_msp.o \
./src/stm32l0xx_it.o \
./src/sx1272-board.o \
./src/sx1272.o \
./src/sysIrqHandlers.o \
./src/timer.o \
./src/utilities.o 

C_DEPS += \
./src/LoRaMac-api-v3.d \
./src/LoRaMac.d \
./src/LoRaMacCrypto.d \
./src/adc-board.d \
./src/aes.d \
./src/board.d \
./src/cmac.d \
./src/delay.d \
./src/fifo.d \
./src/gpio-board.d \
./src/main_telemic.d \
./src/rtc-board.d \
./src/spi-board.d \
./src/stm32l0xx_hal_msp.d \
./src/stm32l0xx_it.d \
./src/sx1272-board.d \
./src/sx1272.d \
./src/sysIrqHandlers.d \
./src/timer.d \
./src/utilities.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall  -g3 -DDEBUG -DTRACE -DSTM32L051xx -DUSE_BAND_868 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32l0xx" -I"../system/include/cmsis/device" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


