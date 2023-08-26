################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/core/HardwareSerial.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/HardwareSerial.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu++11 -fno-rtti -fno-threadsafe-statics -fno-exceptions $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/IPAddress.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/IPAddress.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu++11 -fno-rtti -fno-threadsafe-statics -fno-exceptions $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/MACAddress.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/MACAddress.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu++11 -fno-rtti -fno-threadsafe-statics -fno-exceptions $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/Print.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/Print.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu++11 -fno-rtti -fno-threadsafe-statics -fno-exceptions $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/Stream.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/Stream.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu++11 -fno-rtti -fno-threadsafe-statics -fno-exceptions $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/Tone.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/Tone.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu++11 -fno-rtti -fno-threadsafe-statics -fno-exceptions $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/WInterrupts.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/WInterrupts.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu11 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/WMath.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/WMath.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu++11 -fno-rtti -fno-threadsafe-statics -fno-exceptions $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/WString.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/WString.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu++11 -fno-rtti -fno-threadsafe-statics -fno-exceptions $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/itoa.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/itoa.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu11 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/main.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/main.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu++11 -fno-rtti -fno-threadsafe-statics -fno-exceptions $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/new.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/new.cpp $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu++11 -fno-rtti -fno-threadsafe-statics -fno-exceptions $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/random.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/random.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu11 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/startup_gcc.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/startup_gcc.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu11 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/wiring.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/wiring.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu11 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/wiring_analog.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/wiring_analog.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu11 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/wiring_digital.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/wiring_digital.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu11 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/wiring_pulse.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/wiring_pulse.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu11 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

src/core/wiring_shift.o: /home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac/wiring_shift.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/bin/arm-none-eabi-gcc-8.3.1" -c -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -Dprintf=iprintf -DF_CPU=80000000L -DENERGIA_EK_TM4C123GXL -DENERGIA_ARCH_TIVAC -DENERGIA=23 -DARDUINO=10610 -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/driverlib" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system/inc" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/system" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/cores/tivac" -I"/home/mcbride.342/.energia15/packages/energia/hardware/tivac/1.0.4/variants/EK-TM4C123GXL" -I"/home/mcbride.342/workspace_v11/tivac_EK-TM4C123GXL_core" -I"/home/mcbride.342/.energia15/packages/energia/tools/arm-none-eabi-gcc/8.3.1-20190703/arm-none-eabi/include" -Os -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -w -Wall --param max-inline-insns-single=500 -mabi=aapcs -MMD -MP -MF"src/core/$(basename $(<F)).d_raw" -MT"$(@)" -std=gnu11 $(GEN_OPTS__FLAG) -o"$@" "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


