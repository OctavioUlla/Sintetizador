################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Config.c \
../src/Stack.c \
../src/cr_startup_lpc175x_6x.c \
../src/crp.c \
../src/tp_final.c 

C_DEPS += \
./src/Config.d \
./src/Stack.d \
./src/cr_startup_lpc175x_6x.d \
./src/crp.d \
./src/tp_final.d 

OBJS += \
./src/Config.o \
./src/Stack.o \
./src/cr_startup_lpc175x_6x.o \
./src/crp.o \
./src/tp_final.o 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DDEBUG -D__CODE_RED -DCORE_M3 -D__USE_CMSIS=CMSISv2p00_LPC17xx -D__LPC17XX__ -D__REDLIB__ -I"/home/ferminverdolini/Universidad/Octavo semestre/Electronica digital 3/Trabajo Final/Sintetizador/tp_final/inc" -I"/home/ferminverdolini/Universidad/Octavo semestre/Electronica digital 3/Trabajo Final/Sintetizador/CMSISv2p00_LPC17xx/Drivers/inc" -I"/home/ferminverdolini/Universidad/Octavo semestre/Electronica digital 3/Trabajo Final/Sintetizador/CMSISv2p00_LPC17xx/inc" -O0 -fno-common -g3 -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m3 -mthumb -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src

clean-src:
	-$(RM) ./src/Config.d ./src/Config.o ./src/Stack.d ./src/Stack.o ./src/cr_startup_lpc175x_6x.d ./src/cr_startup_lpc175x_6x.o ./src/crp.d ./src/crp.o ./src/tp_final.d ./src/tp_final.o

.PHONY: clean-src

