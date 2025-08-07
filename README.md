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