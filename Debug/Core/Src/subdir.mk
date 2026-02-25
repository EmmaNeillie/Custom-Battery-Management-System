################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc.c \
../Core/Src/cell.c \
../Core/Src/diagnostics.c \
../Core/Src/fdcan.c \
../Core/Src/gpio.c \
../Core/Src/i2c.c \
../Core/Src/lptim.c \
../Core/Src/main.c \
../Core/Src/scheduler.c \
../Core/Src/soc.c \
../Core/Src/stm32g4xx_hal_msp.c \
../Core/Src/stm32g4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_monitor.c \
../Core/Src/system_stm32g4xx.c \
../Core/Src/utilities.c \
../Core/Src/wwdg.c 

OBJS += \
./Core/Src/adc.o \
./Core/Src/cell.o \
./Core/Src/diagnostics.o \
./Core/Src/fdcan.o \
./Core/Src/gpio.o \
./Core/Src/i2c.o \
./Core/Src/lptim.o \
./Core/Src/main.o \
./Core/Src/scheduler.o \
./Core/Src/soc.o \
./Core/Src/stm32g4xx_hal_msp.o \
./Core/Src/stm32g4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_monitor.o \
./Core/Src/system_stm32g4xx.o \
./Core/Src/utilities.o \
./Core/Src/wwdg.o 

C_DEPS += \
./Core/Src/adc.d \
./Core/Src/cell.d \
./Core/Src/diagnostics.d \
./Core/Src/fdcan.d \
./Core/Src/gpio.d \
./Core/Src/i2c.d \
./Core/Src/lptim.d \
./Core/Src/main.d \
./Core/Src/scheduler.d \
./Core/Src/soc.d \
./Core/Src/stm32g4xx_hal_msp.d \
./Core/Src/stm32g4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_monitor.d \
./Core/Src/system_stm32g4xx.d \
./Core/Src/utilities.d \
./Core/Src/wwdg.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G473xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/cell.cyclo ./Core/Src/cell.d ./Core/Src/cell.o ./Core/Src/cell.su ./Core/Src/diagnostics.cyclo ./Core/Src/diagnostics.d ./Core/Src/diagnostics.o ./Core/Src/diagnostics.su ./Core/Src/fdcan.cyclo ./Core/Src/fdcan.d ./Core/Src/fdcan.o ./Core/Src/fdcan.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/i2c.cyclo ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/lptim.cyclo ./Core/Src/lptim.d ./Core/Src/lptim.o ./Core/Src/lptim.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/scheduler.cyclo ./Core/Src/scheduler.d ./Core/Src/scheduler.o ./Core/Src/scheduler.su ./Core/Src/soc.cyclo ./Core/Src/soc.d ./Core/Src/soc.o ./Core/Src/soc.su ./Core/Src/stm32g4xx_hal_msp.cyclo ./Core/Src/stm32g4xx_hal_msp.d ./Core/Src/stm32g4xx_hal_msp.o ./Core/Src/stm32g4xx_hal_msp.su ./Core/Src/stm32g4xx_it.cyclo ./Core/Src/stm32g4xx_it.d ./Core/Src/stm32g4xx_it.o ./Core/Src/stm32g4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_monitor.cyclo ./Core/Src/system_monitor.d ./Core/Src/system_monitor.o ./Core/Src/system_monitor.su ./Core/Src/system_stm32g4xx.cyclo ./Core/Src/system_stm32g4xx.d ./Core/Src/system_stm32g4xx.o ./Core/Src/system_stm32g4xx.su ./Core/Src/utilities.cyclo ./Core/Src/utilities.d ./Core/Src/utilities.o ./Core/Src/utilities.su ./Core/Src/wwdg.cyclo ./Core/Src/wwdg.d ./Core/Src/wwdg.o ./Core/Src/wwdg.su

.PHONY: clean-Core-2f-Src

