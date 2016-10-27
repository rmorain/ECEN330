################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/minimaxMain.c 

LD_SRCS += \
../src/lscript.ld 

OBJS += \
./src/minimaxMain.o 

C_DEPS += \
./src/minimaxMain.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM g++ compiler'
	arm-xilinx-eabi-g++ -Wall -O0 -g3 -I"C:\Users\cdmoo\Desktop\BYU_FALL_2016\ECEN_330\XilinxWorkspace\Labs\Consolidated_330_SW" -c -fmessage-length=0 -MT"$@" -I../../Consolidated_330_SW_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


