################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ticTacToe/minimax.c \
../src/ticTacToe/minimaxTest.c \
../src/ticTacToe/ticTacToeControl.c \
../src/ticTacToe/ticTacToeControlMain.c \
../src/ticTacToe/ticTacToeDisplay.c 

OBJS += \
./src/ticTacToe/minimax.o \
./src/ticTacToe/minimaxTest.o \
./src/ticTacToe/ticTacToeControl.o \
./src/ticTacToe/ticTacToeControlMain.o \
./src/ticTacToe/ticTacToeDisplay.o 

C_DEPS += \
./src/ticTacToe/minimax.d \
./src/ticTacToe/minimaxTest.d \
./src/ticTacToe/ticTacToeControl.d \
./src/ticTacToe/ticTacToeControlMain.d \
./src/ticTacToe/ticTacToeDisplay.d 


# Each subdirectory must supply rules for building sources it contributes
src/ticTacToe/%.o: ../src/ticTacToe/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM g++ compiler'
	arm-xilinx-eabi-g++ -Wall -O3 -g3 -I"C:\Users\cdmoo\Desktop\BYU_FALL_2016\ECEN_330\XilinxWorkspace\Labs\Consolidated_330_SW" -c -fmessage-length=0 -MT"$@" -I../../Consolidated_330_SW_bsp/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


