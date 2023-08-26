################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/i2c_nb_ds_v1" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mfloat-abi=hard -mabi=aapcs -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu++11 -fno-rtti -fno-threadsafe-statics -fno-exceptions $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

i2c_nb_ds_v1.cpp: ../i2c_nb_ds_v1.ino
	@echo 'Building file: "$<"'
	@echo 'Invoking: Resource Custom Build Step'
	
	@echo 'Finished building: "$<"'
	@echo ' '

%.o: ./%.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/i2c_nb_ds_v1" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mfloat-abi=hard -mabi=aapcs -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu++11 -fno-rtti -fno-threadsafe-statics -fno-exceptions $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


