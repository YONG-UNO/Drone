# STM32 电机控制项目 - 目录结构说明
## 项目简介
STM32F4 平台电机控制项目，支持 RS05/DM4310 电机 CAN 通信，采用分层架构设计，实现硬件与业务逻辑解耦。

## 目录结构
上层调用下层，下层不调用上层

└─User
├─Algorithm      // 算法层：纯算法逻辑，无硬件/业务依赖，可移植
│  ├─Controller  // 复合控制算法（如串级PID、云台控制策略）
│  └─PID         // 基础PID算法实现
├─Application    // 应用层：业务逻辑实现，依赖算法层和BSP层
│  ├─connectivity// 业务通信模块（如与上位机的VPC数据交互）
│  │  └─VPC
│  ├─device      // 设备业务驱动（硬件设备的业务层封装）
│  │  ├─bmi088   // BMI088传感器业务驱动
│  │  ├─ist8310  // IST8310传感器业务驱动
│  │  ├─RS05     // RS05电机业务驱动（含MIT协议封装）
│  │  └─ssd1306  // SSD1306显示屏业务驱动
│  └─task        // 任务模块（FreeRTOS任务，业务功能入口）
│      ├─autoAiming // 自动瞄准任务
│      ├─display     // 显示任务
│      ├─gimbal      // 云台控制任务
│      └─shoot       // 发射控制任务
├─BSP            // 板级支持包：硬件驱动层，直接操作外设寄存器
│  ├─CAN         // CAN总线驱动（初始化、收发、过滤器配置）
│  ├─IIC         // IIC总线驱动（传感器通信底层）
│  ├─SPI         // SPI总线驱动
│  │  └─spi_bmi088 // BMI088专用SPI驱动
│  └─UART        // UART总线驱动（如遥控器DBUS通信）
└─Math           // 数学工具层：通用数学函数，全局复用

## 各模块说明
1.  **Core**：CubeMX 自动生成，包含芯片核心初始化代码，禁止手动修改，避免重新生成项目时被覆盖。
2.  **Drivers**：ST 官方 HAL 库和 CMSIS 核心库，提供硬件底层驱动支持，无需修改。
3.  **BSP**：板级支持包，封装硬件相关驱动（CAN、IIC、USART 等），对外提供统一接口，实现硬件与应用层解耦。
4.  **Application**：应用层，包含所有业务逻辑和控制算法，无需关注硬件底层实现，便于后续移植和修改。
5.  **Middlewares**：第三方中间件，如 FreeRTOS 操作系统、SSD1306 OLED 驱动等，可根据项目需求添加/删除。

## 注意事项
1.  新增硬件驱动时，优先添加到 `BSP` 层，遵循「底层封装，上层调用」原则。
2.  新增业务逻辑时，添加到 `Application` 层对应模块，避免修改 `Core` 和 `Drivers` 层代码。

## 书写规范
### void ist8310ResetStatus(PinState pinState)
### 宏定义 
```angular2html
// 全部大写,中间用下划线连接
#define BMI088_GYRO_SUSPEND_MODE 0x80
#define BMI088_GYRO_DEEP_SUSPEND_MODE 0x20
```
## cmake使用建议
### 路径
```
# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
(${CMAKE_SOURCE_DIR})User/CAN
(${CMAKE_SOURCE_DIR})User/uart
)
```

### 源文件
```angular2html
头文件只需要指定路径即可,不需要单独添加
# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
        User/CAN/bsp_can.c
        User/uart/dbus.c
)
```

## git使用
```angular2html
1.git选择添加版本管理
2.管理远程:添加仓库地址  建议使用SSH协议提交(git@github.com:YONG-UNO/XXX.git),避免HTTPS端口限制
3.新建忽略文件:cmake-build-debug/.gitignore

.idea/         # 忽略CLion配置目录
*.exe          # 忽略所有exe文件
pid_data.csv   # 忽略特定CSV文件

4.取消追踪:
git rm -r --cached .idea/  # 移除目录跟踪
git rm --cached main.exe   # 移除文件跟踪
```

## clion使用

### 使用shift + ctrl + R:进行替换和查找
### clion自动生成注释
### Doxygen 风格注释：
```angular2html
自动生成方式：输入 “/**” 或 “/!” 后按回车键，CLion 会根据函数的参数、返回值等信息自动生成注释框架。例如对于函数 “int add (int a, int b);”，
会生成 “ @param a First integer parameter.* @param b Second integer parameter.* @return The result of adding 'a' and 'b'.” 这样的注释。
``` 

### clionWarning类型
#### 1.Expected command \endlink
##### 一般是因为注释中写了link,但是没有endlink,解决方法:在link后添加\endlink
#### 2.无法解析符号'https'
##### 解决方法: 使用markdown格式:[链接名称] (链接)   (经测试无法通过(尴尬.jpg))

### 快速查找文件
```angular2html
在文件树中直接输入想找的文件名称
```

## Ozone使用
### 显示变量波形
#### 1.先添加到wathc 2.右键选择Graph 3.在view菜单里面选择Timeline 4.只选择Data 5.Fit Width | Height

## TODO使用
### 如何查看:
#### TODO窗口查看
### 使用方法
#### // TODO：标记需要完成的任务
#### // FIXME：标记需要修复的问题（CLion 也会识别为待办事项）

## linux环境下配置clion环境
```angular2html
分别需要安装cubemx和cubemxCLT,在clion中创建stm32project即可
sudo sh ./st-stm32cubeclt_1.11.0_2026_20221221_1309_amd64.deb_bundle.sh
安装CLT解压包指令
```