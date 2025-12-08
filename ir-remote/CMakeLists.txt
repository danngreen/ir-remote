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

# Set the project name
set(CMAKE_PROJECT_NAME ir-remote)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C CXX ASM)

add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

add_subdirectory(${CMAKE_SOURCE_DIR}/lib/cpputil
                 ${CMAKE_CURRENT_BINARY_DIR}/cpputil)

target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE Core/Src/hal_init.cc)

target_include_directories(
  ${CMAKE_PROJECT_NAME} PRIVATE # Add user defined include paths
)

target_compile_definitions(
  ${CMAKE_PROJECT_NAME} PRIVATE # Add user defined symbols
)

target_link_libraries(${CMAKE_PROJECT_NAME} stm32cubemx cpputil::cpputil)
