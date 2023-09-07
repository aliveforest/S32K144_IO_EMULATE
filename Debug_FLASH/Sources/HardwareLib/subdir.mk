################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/HardwareLib/GPIO_init.c \
../Sources/HardwareLib/IIC_IO.c \
../Sources/HardwareLib/LPSPI.c \
../Sources/HardwareLib/LPSPI_oled.c \
../Sources/HardwareLib/LPTMR.c \
../Sources/HardwareLib/LPUART.c \
../Sources/HardwareLib/LP_I2C_oled.c \
../Sources/HardwareLib/LP_IT_timer.c \
../Sources/HardwareLib/OLED_FONT.c \
../Sources/HardwareLib/OLED_IO.c \
../Sources/HardwareLib/RGB_LED.c \
../Sources/HardwareLib/SPI_OLED.c \
../Sources/HardwareLib/UART_IO.c \
../Sources/HardwareLib/clocks_and_modes.c \
../Sources/HardwareLib/latency.c 

OBJS += \
./Sources/HardwareLib/GPIO_init.o \
./Sources/HardwareLib/IIC_IO.o \
./Sources/HardwareLib/LPSPI.o \
./Sources/HardwareLib/LPSPI_oled.o \
./Sources/HardwareLib/LPTMR.o \
./Sources/HardwareLib/LPUART.o \
./Sources/HardwareLib/LP_I2C_oled.o \
./Sources/HardwareLib/LP_IT_timer.o \
./Sources/HardwareLib/OLED_FONT.o \
./Sources/HardwareLib/OLED_IO.o \
./Sources/HardwareLib/RGB_LED.o \
./Sources/HardwareLib/SPI_OLED.o \
./Sources/HardwareLib/UART_IO.o \
./Sources/HardwareLib/clocks_and_modes.o \
./Sources/HardwareLib/latency.o 

C_DEPS += \
./Sources/HardwareLib/GPIO_init.d \
./Sources/HardwareLib/IIC_IO.d \
./Sources/HardwareLib/LPSPI.d \
./Sources/HardwareLib/LPSPI_oled.d \
./Sources/HardwareLib/LPTMR.d \
./Sources/HardwareLib/LPUART.d \
./Sources/HardwareLib/LP_I2C_oled.d \
./Sources/HardwareLib/LP_IT_timer.d \
./Sources/HardwareLib/OLED_FONT.d \
./Sources/HardwareLib/OLED_IO.d \
./Sources/HardwareLib/RGB_LED.d \
./Sources/HardwareLib/SPI_OLED.d \
./Sources/HardwareLib/UART_IO.d \
./Sources/HardwareLib/clocks_and_modes.d \
./Sources/HardwareLib/latency.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/HardwareLib/%.o: ../Sources/HardwareLib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Standard S32DS C Compiler'
	arm-none-eabi-gcc "@Sources/HardwareLib/GPIO_init.args" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


