cmake_minimum_required(VERSION 3.22)

enable_language(C ASM)

# Interface library for includes and symbols
add_library(stm32cubemx INTERFACE)

target_include_directories(stm32cubemx INTERFACE 
    ${CMAKE_SOURCE_DIR}/src/conf
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Inc
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Inc/Legacy
    ${CMAKE_SOURCE_DIR}/lib/CMSIS/Device/ST/STM32G4xx/Include
    ${CMAKE_SOURCE_DIR}/lib/CMSIS/Include
)

target_compile_definitions(stm32cubemx INTERFACE 
    USE_HAL_DRIVER 
    STM32G431xx 
    $<$<CONFIG:Debug>:DEBUG>
)

# Create STM32_Drivers static library
add_library(STM32_Drivers OBJECT)
target_sources(STM32_Drivers PRIVATE
    ${CMAKE_SOURCE_DIR}/src/system_stm32g4xx.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_adc.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_sai.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_sai_ex.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc_ex.c
    # ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash.c
    # ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ex.c
    # ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ramfunc.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_gpio.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_exti.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma_ex.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr_ex.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_cortex.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim_ex.c
    ${CMAKE_SOURCE_DIR}/lib/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_usart.c
)

target_link_libraries(STM32_Drivers PUBLIC stm32cubemx)

