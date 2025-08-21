cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Drone)

# Include toolchain file
include("cmake/gcc-arm-none-eabi.cmake")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        User/connectivity/iic/bsp_iic.c
        User/connectivity/iic/bsp_iic.h
        User/device/ist8310/ist8310.c
        User/device/ist8310/ist8310.h
        User/Math/conversion.c
        User/Math/conversion.h
        User/task/gimbal/gimbal_control.c
        User/task/gimbal/gimbal_control.h
        User/Math/limit.c
        User/Math/limit.h
        User/task/shoot/shoot_control.c
        User/task/shoot/shoot_control.h
        User/task/autoAiming/autoAiming.c
        User/task/autoAiming/autoAiming.h
        User/connectivity/VPC/vpc.c
        User/connectivity/VPC/vpc.h
        User/task/display/display.c
        User/task/display/display.h
        User/device/ssd1306/ssd1306.c
        User/device/ssd1306/ssd1306.h

)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
        User/algorithm/PID.c
        User/connectivity/CAN/bsp_can.c
        User/connectivity/uart/dbus.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    (${CMAKE_SOURCE_DIR}) User/algorithm
    (${CMAKE_SOURCE_DIR}) User/connectivity/CAN
    (${CMAKE_SOURCE_DIR}) User/connectivity/uart
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
