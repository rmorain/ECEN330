################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/mySimon/buttonHandler.c \
../src/mySimon/buttonHandler_runTest.c \
../src/mySimon/flashSequence.c \
../src/mySimon/flashSequence_runTest.c \
../src/mySimon/globals.c \
../src/mySimon/simonControl.c \
../src/mySimon/simonDisplay.c \
../src/mySimon/simonDisplay_runTest.c \
../src/mySimon/simonMain.c \
../src/mySimon/verifySequence.c \
../src/mySimon/verifySequence_runTest.c 

OBJS += \
./src/mySimon/buttonHandler.o \
./src/mySimon/buttonHandler_runTest.o \
./src/mySimon/flashSequence.o \
./src/mySimon/flashSequence_runTest.o \
./src/mySimon/globals.o \
./src/mySimon/simonControl.o \
./src/mySimon/simonDisplay.o \
./src/mySimon/simonDisplay_runTest.o \
./src/mySimon/simonMain.o \
./src/mySimon/verifySequence.o \
./src/mySimon/verifySequence_runTest.o 

C_DEPS += \
./src/mySimon/buttonHandler.d \
./src/mySimon/buttonHandler_runTest.d \
./src/mySimon/flashSequence.d \
./src/mySimon/flashSequence_runTest.d \
./src/mySimon/globals.d \
./src/mySimon/simonControl.d \
./src/mySimon/simonDisplay.d \
./src/mySimon/simonDisplay_runTest.d \
./src/mySimon/simonMain.d \
./src/mySimon/verifySequence.d \
./src/mySimon/verifySequence_runTest.d 


# Each subdirectory must supply rules for building sources it contributes
src/mySimon/%.o: ../src/mySimon/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM g++ compiler'
	arm-xilinx-eabi-g++ -Wall -O3 -g3 -I"C:\Users\cdmoo\Desktop\BYU_FALL_2016\ECEN_330\XilinxWorkspace\Labs\Consolidated_330_SW" -c -fmessage-length=0 -MT"$@" -I../../Consolidated_330_SW_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


