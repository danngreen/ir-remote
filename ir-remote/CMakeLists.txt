cmake_minimum_required(VERSION 3.22)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_PROJECT_NAME ir-remote)

include("cmake/gcc-arm-none-eabi.cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(irremote)
message("Build type: " ${CMAKE_BUILD_TYPE})

enable_language(C CXX ASM)

add_executable(irremote)

# CubeMX/HAL
add_subdirectory(cmake/stm32cubemx)

# App
target_sources(irremote PRIVATE
    Core/Src/main.cc
    Core/Src/stm32g4xx_it.c
    Core/Src/stm32g4xx_hal_msp.c
    Core/Src/sysmem.c
    Core/Src/syscalls.c
    startup_stm32g431xx.s
    Core/Src/hal_init.cc)

set_target_properties(irremote PROPERTIES ADDITIONAL_CLEAN_FILES irremote.map)


# mdrivlib
add_library(mdrivlib STATIC
  # lib/mdrivlib/drivers/i2c.cc
  # lib/mdrivlib/drivers/pin.cc
  lib/mdrivlib/target/stm32f7xx/drivers/interrupt_handler.cc
  # lib/mdrivlib/target/stm32f7xx/drivers/sai_tdm.cc
)

target_include_directories(mdrivlib PUBLIC
  lib/cpputil
  lib/mdrivlib/
  lib/mdrivlib/target/stm32f7xx/
  lib/CMSIS/Include
  lib/CMSIS/Core/Include
  lib/CMSIS/Device/ST/STM32G4xx/Include
)

target_compile_definitions(mdrivlib PUBLIC
  STM32G4
  STM32G431
  STM32G431xx
)

# cpputil
add_subdirectory(${CMAKE_SOURCE_DIR}/lib/cpputil ${CMAKE_CURRENT_BINARY_DIR}/cpputil)


target_link_libraries(irremote STM32_Drivers)
target_link_libraries(irremote stm32cubemx cpputil::cpputil mdrivlib)
