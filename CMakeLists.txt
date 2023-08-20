cmake_minimum_required(VERSION 3.16)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PICO_PATH_SDK "lib/pico-sdk")
include(lib/pico-sdk/pico_sdk_init.cmake)
project(pico_chip8 C CXX ASM)
pico_sdk_init()

add_executable(${PROJECT_NAME} 
	src/main.c
	src/roms.c
	src/device.c
	src/display.c
	src/button.c

	src/chip-8/chip8.c
	src/chip-8/chip8_opcodes.c
	src/chip-8/schip8_opcodes.c
	src/chip-8/xochip_opcodes.c
)

pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_set_program_name(${PROJECT_NAME} "pico-chip8")
pico_set_program_version(${PROJECT_NAME} "0.1")

add_subdirectory("lib/pico-st7735")
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/lib/pico-st7735/lib-st7735/include
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/lib/pico-sdk
)
target_link_libraries(${PROJECT_NAME} pico_stdlib hardware_spi st7735)
pico_add_extra_outputs(${PROJECT_NAME})
