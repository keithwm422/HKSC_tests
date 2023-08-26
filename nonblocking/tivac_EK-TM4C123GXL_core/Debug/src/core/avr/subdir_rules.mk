################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/core/avr/dtostrf.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/avr/dtostrf.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/avr/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu11 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


