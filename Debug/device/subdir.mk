################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../device/system_LPC55S36.c 

C_DEPS += \
./device/system_LPC55S36.d 

OBJS += \
./device/system_LPC55S36.o 


# Each subdirectory must supply rules for building sources it contributes
device/%.o: ../device/%.c device/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_LPC55S36JBD100 -DCPU_LPC55S36JBD100_cm33 -DMCUXPRESSO_SDK -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DSDK_DEBUGCONSOLE=0 -I"/Users/tedd/dev/mcuxpresso/lpc55s36/lpcxpresso55s36_i2c_polling_b2b_slave/source" -I"/Users/tedd/dev/mcuxpresso/lpc55s36/lpcxpresso55s36_i2c_polling_b2b_slave/utilities" -I"/Users/tedd/dev/mcuxpresso/lpc55s36/lpcxpresso55s36_i2c_polling_b2b_slave/drivers" -I"/Users/tedd/dev/mcuxpresso/lpc55s36/lpcxpresso55s36_i2c_polling_b2b_slave/device" -I"/Users/tedd/dev/mcuxpresso/lpc55s36/lpcxpresso55s36_i2c_polling_b2b_slave/component/uart" -I"/Users/tedd/dev/mcuxpresso/lpc55s36/lpcxpresso55s36_i2c_polling_b2b_slave/component/lists" -I"/Users/tedd/dev/mcuxpresso/lpc55s36/lpcxpresso55s36_i2c_polling_b2b_slave/CMSIS" -I"/Users/tedd/dev/mcuxpresso/lpc55s36/lpcxpresso55s36_i2c_polling_b2b_slave/board" -I"/Users/tedd/dev/mcuxpresso/lpc55s36/lpcxpresso55s36_i2c_polling_b2b_slave/lpcxpresso55s36/driver_examples/i2c/polling_b2b/slave" -O0 -fno-common -g3 -gdwarf-4 -mcpu=cortex-m33 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33 -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-device

clean-device:
	-$(RM) ./device/system_LPC55S36.d ./device/system_LPC55S36.o

.PHONY: clean-device

