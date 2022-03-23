################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Tetris-v2.c \
../src/commands.c \
../src/control.c \
../src/diskio.c \
../src/drmario.c \
../src/ff.c \
../src/fifo.c \
../src/lcd.c \
../src/main.c \
../src/midi.c \
../src/music.c \
../src/spritemap.c \
../src/syscalls.c \
../src/system_stm32f0xx.c \
../src/tetris_theme.c \
../src/tty.c 

OBJS += \
./src/Tetris-v2.o \
./src/commands.o \
./src/control.o \
./src/diskio.o \
./src/drmario.o \
./src/ff.o \
./src/fifo.o \
./src/lcd.o \
./src/main.o \
./src/midi.o \
./src/music.o \
./src/spritemap.o \
./src/syscalls.o \
./src/system_stm32f0xx.o \
./src/tetris_theme.o \
./src/tty.o 

C_DEPS += \
./src/Tetris-v2.d \
./src/commands.d \
./src/control.d \
./src/diskio.d \
./src/drmario.d \
./src/ff.d \
./src/fifo.d \
./src/lcd.d \
./src/main.d \
./src/midi.d \
./src/music.d \
./src/spritemap.d \
./src/syscalls.d \
./src/system_stm32f0xx.d \
./src/tetris_theme.d \
./src/tty.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F091RCTx -DDEBUG -DSTM32F091 -DUSE_STDPERIPH_DRIVER -I"/Users/Kirito/Documents/workspace/miniproj-v3/StdPeriph_Driver/inc" -I"/Users/Kirito/Documents/workspace/miniproj-v3/inc" -I"/Users/Kirito/Documents/workspace/miniproj-v3/CMSIS/device" -I"/Users/Kirito/Documents/workspace/miniproj-v3/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


