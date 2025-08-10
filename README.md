## 书写规范
### void ist8310ResetStatus(PinState pinState)

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
2.管理远程:添加仓库地址
3.新建忽略文件:cmake-build-debug/.gitignore

.idea/         # 忽略CLion配置目录
*.exe          # 忽略所有exe文件
pid_data.csv   # 忽略特定CSV文件

4.取消追踪:
git rm -r --cached .idea/  # 移除目录跟踪
git rm --cached main.exe   # 移除文件跟踪
```

## clion使用
### 显示变量波形
#### 1.先添加到wathc 2.右键选择Graph 3.在view菜单里面选择Timeline 4.只选择Data 5.Fit Width | Height

### 使用shift + ctrl + R:进行替换和查找
### clion自动生成注释
### Doxygen 风格注释：
#### 自动生成方式：输入 “/**” 或 “/!” 后按回车键，CLion 会根据函数的参数、返回值等信息自动生成注释框架。例如对于函数 “int add (int a, int b);”，会生成 “ @param a First integer parameter.* @param b Second integer parameter.* @return The result of adding 'a' and 'b'.” 这样的注释。

### clionWarning类型
#### 1.Expected command \endlink
##### 一般是因为注释中写了link,但是没有endlink,解决方法:在link后添加\endlink
#### 2.无法解析符号'https'
##### 解决方法: 使用markdown格式:[链接名称] (链接)   (经测试无法通过(尴尬.jpg))

## Ozone使用
### 没写

## TODO使用
### 如何查看:
#### TODO窗口查看
### 使用方法
#### // TODO：标记需要完成的任务
#### // FIXME：标记需要修复的问题（CLion 也会识别为待办事项）
