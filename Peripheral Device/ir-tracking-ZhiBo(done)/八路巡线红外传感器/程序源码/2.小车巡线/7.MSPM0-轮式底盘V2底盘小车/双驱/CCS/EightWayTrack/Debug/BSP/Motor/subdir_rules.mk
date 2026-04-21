################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
BSP/Motor/%.o: ../BSP/Motor/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: Arm Compiler'
	"C:/ti/ccstheia151/ccs/tools/compiler/ti-cgt-armllvm_4.0.0.LTS/bin/tiarmclang.exe" -c @"syscfg/device.opt"  -march=thumbv6m -mcpu=cortex-m0plus -mfloat-abi=soft -mlittle-endian -mthumb -O2 -I"C:/ti/mspm0_sdk_2_02_00_05/EightWayTrack" -I"C:/ti/mspm0_sdk_2_02_00_05/EightWayTrack/Debug" -I"C:/ti/mspm0_sdk_2_02_00_05/source/third_party/CMSIS/Core/Include" -I"C:/ti/mspm0_sdk_2_02_00_05/source" -I"C:/ti/mspm0_sdk_2_02_00_05/EightWayTrack/BSP" -I"C:/ti/mspm0_sdk_2_02_00_05/EightWayTrack/BSP/Motor" -I"C:/ti/mspm0_sdk_2_02_00_05/EightWayTrack/BSP/Eight_Tracking" -gdwarf-3 -MMD -MP -MF"BSP/Motor/$(basename $(<F)).d_raw" -MT"$(@)" -I"C:/ti/mspm0_sdk_2_02_00_05/EightWayTrack/Debug/syscfg"  $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


