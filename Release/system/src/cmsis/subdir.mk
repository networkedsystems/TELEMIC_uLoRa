################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/cmsis/system_stm32l0xx.c 

S_UPPER_SRCS += \
../system/src/cmsis/startup_stm32l051xx.S 

OBJS += \
./system/src/cmsis/startup_stm32l051xx.o \
./system/src/cmsis/system_stm32l0xx.o 

C_DEPS += \
./system/src/cmsis/system_stm32l0xx.d 

S_UPPER_DEPS += \
./system/src/cmsis/startup_stm32l051xx.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/cmsis/%.o: ../system/src/cmsis/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall  -g3 -x assembler-with-cpp -DDEBUG -DTRACE -DSTM32L051xx -DUSE_BAND_868 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32l0xx" -I"../system/include/cmsis/device" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

system/src/cmsis/%.o: ../system/src/cmsis/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -fno-move-loop-invariants -Wall  -g3 -DDEBUG -DTRACE -DSTM32L051xx -DUSE_BAND_868 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32l0xx" -I"../system/include/cmsis/device" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


