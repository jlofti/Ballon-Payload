cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(Balloon C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(Balloon
	balloon.cpp
	)

add_library(LoRa_pico_lib src/LoRa-RP2040.cpp include/LoRa-RP2040.h)
add_library(LoRa_print include/Print.h src/Print.cpp)
add_library(LoRa include/LoRa.h src/LoRa.cpp)
add_library(GPS include/GPS.h src/GPS.cpp)
add_library(Pressure include/Pressure.h src/Pressure.cpp)
add_library(Thermistor include/Thermistor.h src/Thermistor.cpp)
add_library(CommandHandler include/CommandHandler.h src/CommandHandler.cpp)


target_link_libraries(LoRa_pico_lib pico_stdlib hardware_spi hardware_gpio)
target_link_libraries(LoRa LoRa_pico_lib hardware_gpio)
target_link_libraries(GPS hardware_i2c pico_stdlib)
target_link_libraries(Pressure hardware_i2c pico_stdlib)
target_link_libraries(Thermistor hardware_adc pico_stdlib hardware_gpio)
target_link_libraries(CommandHandler pico_sync )

target_link_libraries(Balloon 
	pico_multicore
	pico_stdlib
	hardware_i2c
	hardware_uart
	hardware_irq
	hardware_spi
	hardware_rtc
	hardware_watchdog
	LoRa_pico_lib
	LoRa_print
	GPS
	Pressure
	Thermistor
	LoRa	
	pico_sync
	CommandHandler
)

pico_enable_stdio_usb(Balloon 1)
pico_enable_stdio_uart(Balloon 0)

pico_add_extra_outputs(Balloon)