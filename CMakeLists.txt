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
# 嵌入式项目中，头文件（.h/.hpp）无需添加到 add_executable 中
#（CMake 仅需要源文件用于编译，头文件通过 target_include_directories 关联即可），删除所有头文件，仅保留源文件（.c/.cpp/.s）
add_executable(${CMAKE_PROJECT_NAME}
        User/connectivity/iic/bsp_iic.c
        User/device/ist8310/ist8310.c
        User/Math/conversion.c
        User/task/gimbal/gimbal_control.c
        User/Math/limit.c
        User/task/shoot/shoot_control.c
        User/task/autoAiming/autoAiming.c
        User/connectivity/VPC/vpc.c
        User/task/display/display.c
        User/device/ssd1306/ssd1306.c
        User/connectivity/SPI/spi_bmi088/spi_bmi088.c
        User/device/bmi088/bmi088.c
        User/task/gimbal/gimbal_control_dm4310.c
        User/device/RS05/RS05.cpp
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
)

# Add include paths
# 步骤 1：CMake 中用 target_include_directories 配置「头文件搜索路径」
# 这是「给编译器指路」的关键步骤，你在 CMakeLists.txt 中已经配置了 target_include_directories，修复语法后，它的作用就是告诉编译器：「去这些目录下找我需要的头文件」。
# 步骤 2：代码中用 #include 指令引入头文件（你已在代码中实现）
# 这是「源文件主动查找头文件」的步骤，在需要使用头文件声明的源文件中，通过 #include 引入对应的头文件，编译器就会根据步骤 1 配置的搜索路径，找到并解析该头文件。
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
    ${CMAKE_SOURCE_DIR}/User/algorithm
    ${CMAKE_SOURCE_DIR}/User/connectivity/CAN
    ${CMAKE_SOURCE_DIR}/User/connectivity/uart
    ${CMAKE_SOURCE_DIR}/User/connectivity/iic
    ${CMAKE_SOURCE_DIR}/User/connectivity/SPI/spi_bmi088
    ${CMAKE_SOURCE_DIR}/User/device/RS05
    ${CMAKE_SOURCE_DIR}/User/Math
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
