################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.c 

OBJS += \
./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.o 

C_DEPS += \
./Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/cmsis_os.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/%.o: ../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DUSE_HAL_DRIVER -DSTM32F407xx -I"F:/STM/rotary_device/Inc" -I"F:/STM/rotary_device/Middlewares/Third_Party/LwIP/src/include" -I"F:/STM/rotary_device/Middlewares/Third_Party/LwIP/system" -I"F:/STM/rotary_device/Drivers/STM32F4xx_HAL_Driver/Inc" -I"F:/STM/rotary_device/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"F:/STM/rotary_device/Middlewares/Third_Party/FreeRTOS/Source/include" -I"F:/STM/rotary_device/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"F:/STM/rotary_device/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"F:/STM/rotary_device/Middlewares/Third_Party/LwIP/src/include/netif/ppp" -I"F:/STM/rotary_device/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"F:/STM/rotary_device/Middlewares/Third_Party/LwIP/src/include/lwip" -I"F:/STM/rotary_device/Middlewares/Third_Party/LwIP/src/include/lwip/apps" -I"F:/STM/rotary_device/Middlewares/Third_Party/LwIP/src/include/lwip/priv" -I"F:/STM/rotary_device/Middlewares/Third_Party/LwIP/src/include/lwip/prot" -I"F:/STM/rotary_device/Middlewares/Third_Party/LwIP/src/include/netif" -I"F:/STM/rotary_device/Middlewares/Third_Party/LwIP/src/include/posix" -I"F:/STM/rotary_device/Middlewares/Third_Party/LwIP/src/include/posix/sys" -I"F:/STM/rotary_device/Middlewares/Third_Party/LwIP/system/arch" -I"F:/STM/rotary_device/Middlewares/Third_Party/LwIP/src/apps/snmp" -I"F:/STM/rotary_device/Drivers/CMSIS/Include"  -Og -g3 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


