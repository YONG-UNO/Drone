cmake_minimum_required(VERSION 3.22)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING ON)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# 编译器
set(CMAKE_C_COMPILER    arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER  arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER  arm-none-eabi-gcc)
set(CMAKE_LINKER        arm-none-eabi-ld)
set(CMAKE_AR            arm-none-eabi-ar)

# 公共选项（C/C++/ASM 都用）
add_compile_options(
    -g -Og   # 编译优化
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
    -Wall
)

# C++ 专用选项
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>)

# 链接选项
add_link_options(
    -mcpu=cortex-m4
    -mthumb
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
    --specs=nano.specs
    --specs=nosys.specs
    -Wl,-Map=${PROJECT_NAME}.map
    -Wl,--gc-sections
)

# 语言标准
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 项目名
project(Drone C CXX ASM)

# 自动找启动文件 + 链接脚本
file(GLOB_RECURSE STARTUP_FILES "${CMAKE_SOURCE_DIR}/startup_*.s")
file(GLOB_RECURSE LD_FILES "${CMAKE_SOURCE_DIR}/*.ld")

if(LD_FILES)
    list(GET LD_FILES 0 SELECTED_LD)
    add_link_options(-T ${SELECTED_LD})
    message("✅ 使用链接脚本: ${SELECTED_LD}")
else()
    message(FATAL_ERROR "❌ 没有找到 .ld 链接脚本文件！请从 CubeMX 复制到项目根目录")
endif()

# 可执行文件
add_executable(${PROJECT_NAME}
        # 你的 C 文件
        User/Modules/Algorithm/PID/pid.c
        User/Application/VPC/vpc.cpp
        User/Modules/bmi088/bmi088.c
        User/Modules/ist8310/ist8310.c
        User/Modules/ssd1306/ssd1306.c
        User/Application/display/display.c
        User/Application/gimbal/gimbal_control.c
        User/Application/shoot/shoot_control.c
        User/BSP/CAN/bsp_can.c
        User/BSP/IIC/bsp_iic.c
        User/BSP/SPI/spi_bmi088/bsp_spi_bmi088.c
        User/BSP/UART/dbus.c
        User/Modules/Math/conversion.c
        User/Modules/Math/limit.c

        # C++
        User/Modules/RS05/RS05.cpp
        User/Application/gc/GC_task.cpp
                User/Application/gimbal/gimbal_control_RS05.cpp


    # 启动文件（必须）
    ${STARTUP_FILES}
)

# 包含 HAL 库
add_subdirectory(cmake/stm32cubemx)

# 头文件路径
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/User/Application/display
        ${CMAKE_SOURCE_DIR}/User/Application/gimbal
        ${CMAKE_SOURCE_DIR}/User/Application/shoot
        ${CMAKE_SOURCE_DIR}/User/Application/VPC
        ${CMAKE_SOURCE_DIR}/User/Modules/Algorithm/PID
        ${CMAKE_SOURCE_DIR}/User/Modules/Algorithm/GC
        ${CMAKE_SOURCE_DIR}/User/Application/gc
        ${CMAKE_SOURCE_DIR}/User/Modules/bmi088
        ${CMAKE_SOURCE_DIR}/User/Modules/ist8310
        ${CMAKE_SOURCE_DIR}/User/Modules/Math
        ${CMAKE_SOURCE_DIR}/User/Modules/RS05
        ${CMAKE_SOURCE_DIR}/User/Modules/ssd1306
        ${CMAKE_SOURCE_DIR}/User/BSP/CAN
        ${CMAKE_SOURCE_DIR}/User/BSP/IIC
        ${CMAKE_SOURCE_DIR}/User/BSP/SPI/spi_bmi088
        ${CMAKE_SOURCE_DIR}/User/BSP/UART
)

# 链接驱动库
target_link_libraries(${PROJECT_NAME} stm32cubemx)

set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX .elf)
