cmake_minimum_required(VERSION 3.15.3)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/arm-none-eabi-gcc.cmake)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(irremote-sources LANGUAGES C CXX ASM)

include(${CMAKE_SOURCE_DIR}/cmake/common.cmake)

set(root ${CMAKE_SOURCE_DIR})

# Add STM32 HAL and CMSIS
add_subdirectory(${root}/lib/stm32cubemx ${CMAKE_CURRENT_BINARY_DIR}/hal)

add_subdirectory(${root}/lib/cpputil ${CMAKE_CURRENT_BINARY_DIR}/cpputil)

# Arch interface library
add_library(g431_arch INTERFACE)

target_compile_definitions(g431_arch INTERFACE
    STM32FG431xx 
    STM32G4 
    STM32G431
    USE_HAL_DRIVER 
    USE_FULL_LL_DRIVER
)

target_compile_options(g431_arch INTERFACE
          $<$<CONFIG:Debug>:-O0 -g3>
          $<$<CONFIG:Release>:-Ofast>
          $<$<CONFIG:RelWithDebInfo>:-Ofast -g3>
          -fdata-sections
          -ffunction-sections
          -fno-common
          -fno-unwind-tables
          -mcpu=cortex-m4 
          -mfpu=fpv4-sp-d16 
          -mfloat-abi=hard
          # -mthumb
          -nostartfiles
          -nostdlib
          -Wdouble-promotion
          -Werror=return-type
          -Wall
          $<$<COMPILE_LANGUAGE:CXX>:
              -std=c++23
              -ffold-simple-inlines
              -fno-rtti
              -fno-threadsafe-statics
              -fno-exceptions
              -Wno-register
              -Wno-volatile
          >
)

target_link_options(g431_arch INTERFACE
    -Wl,--gc-sections
    -nostdlib
      -mcpu=cortex-m4 
      -mfpu=fpv4-sp-d16 
      -mfloat-abi=hard
    # ${ARCH_FLAGS}
)

target_link_libraries(STM32_Drivers PRIVATE g431_arch)


# Main
add_executable(main.elf)

target_sources(main.elf PRIVATE
    ${root}/src/main.cc
    ${root}/src/console.cc
    ${root}/src/startup_stm32g431xx.s
    # ${root}/src/system_stm32g4xx.c
    ${root}/lib/mdrivlib/drivers/hal_handlers.cc
    ${root}/src/libc_stub.c
    ${root}/src/libcpp_stub.cc

    ${root}/lib/mdrivlib/drivers/pin.cc
    ${root}/lib/mdrivlib/target/stm32g4xx/drivers/interrupt_handler.cc
)

target_compile_definitions(main.elf PRIVATE
)


target_include_directories(main.elf PRIVATE
    ${root}/src
    ${root}/lib/mdrivlib
    ${root}/lib/mdrivlib/target/stm32g4xx
    ${root}/lib/CMSIS/Device/ST/STM32G4xx/Include
    ${root}/lib/STM32G4xx_HAL_Driver/Inc
)


target_link_libraries(main.elf PRIVATE g431_arch stm32cubemx STM32_Drivers cpputil::cpputil)

target_link_script(main ${CMAKE_SOURCE_DIR}/linker/STM32G431XX_FLASH.ld)

add_bin_hex_command(main)

