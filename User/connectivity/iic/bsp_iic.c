//
// Created by DingYong on 25-8-10.
//

#include "bsp_iic.h"

#include <string.h>
#include <sys/types.h>

#include "i2c.h"
#include "stm32f4xx_hal_gpio.h"


// // 占位接口:用于初始化ist8310相关GPIO(复位引脚RSTN,DTRY等)
// void ist8310GpioInit(void) {
//
// }
//
// // 占位接口:用于配置IIC时钟频率,引脚复用等
// void ist8310ComInit(void) {
//
// }
//
// /**
//  *@brief read a byte of ist8310 by iic
//   * @brief  Read an amount of data in blocking mode from a specific memory address
//   * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
//   *         the configuration information for the specified I2C.
//   *         IIC句柄(如已经初始化的IIC外设,如&hi2c3)
//   *
//   * @param  DevAddress Target device address: The device 7 bits address value
//   *         in datasheet must be shifted to the left before calling the interface
//   *         从设备8位地址(7位设备地址左移一位,最低位预留为读写位)
//   *
//   * @param  MemAddress Internal memory address
//   *         目标寄存器地址(ist8310 的 0x03寄存器)
//   *
//   * @param  MemAddSize Size of internal memory address
//   *         寄存器地址长度(8bit: I2C_MEMADD_SIZE_8BIT; 16bit: I2C_MEMADD_SIZE_16BIT)
//   *
//   * @param  pData Pointer to data buffer
//   *         接收数据的缓冲区(用于存储读取到的寄存器数据)
//   *
//   * @param  Size Amount of data to be sent
//   *         要读取的数据长度(字节数)
//   *
//   * @param  Timeout Timeout duration
//   *         超时时间(毫秒, 超过此时间未完成则返回错误)
//   *
//   * @retval HAL status
//   *
//   *@retrval value of the register
//   */
// uint8_t ist8310ReadRegisterSingle(uint8_t MemAddress ) {
//     uint8_t result = 0;
//     HAL_I2C_Mem_Read(&hi2c3, IST8310_IIC_ADDRESS << 1, MemAddress,I2C_MEMADD_SIZE_8BIT, &result, 1, 10);
//     return result;
// }
//
// /**
//  * @brief 通过iic读取ist8310的多个字节
//  * @param MemAddress 寄存器开始地址
//  * @param buf 读取数据缓冲区
//  * @param len 读取字节总数
//  */
// void ist8310ReadRegisterMultiple(uint8_t MemAddress, uint8_t *buf, uint8_t len){
//     HAL_I2C_Mem_Read(&hi2c3, IST8310_IIC_ADDRESS << 1, MemAddress, I2C_MEMADD_SIZE_8BIT, buf, len, 10);
// }
//
// /**
//   * @brief  Write an amount of data in blocking mode to a specific memory address
//   * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
//   *         the configuration information for the specified I2C.
//   *         IIC句柄(如已经初始化的IIC外设,如&hi2c3)
//   *
//   * @param  DevAddress Target device address: The device 7 bits address value
//   *         in datasheet must be shifted to the left before calling the interface
//   *         从设备8位地址(7位设备地址左移一位,最低位预留为读写位)
//   *
//   * @param  MemAddress Internal memory address
//   *         目标寄存器地址(ist8310 的 0x03寄存器)
//   *
//   * @param  MemAddSize Size of internal memory address
//   *         寄存器地址长度(8bit: I2C_MEMADD_SIZE_8BIT; 16bit: I2C_MEMADD_SIZE_16BIT)
//   *
//   * @param  pData Pointer to data buffer
//   *         指向待写入数据的缓冲区指针(如存放配置值的数组)
//   *
//   * @param  Size Amount of data to be sent
//   *         要发送的数据字节数(如写入一个字节的配置值,则为1)
//   *
//   * @param  Timeout Timeout duration
//   *         超时时间(ms):若操作超过此时间未完成,函数会返回超时错误
//   *
//   * @retval HAL status
//   */
// void ist8310WriteRegisterSingle(uint8_t MemAddress, uint8_t const data) {
//     uint8_t data_bur = data;          // 创建缓冲区(单个字节)
//     HAL_I2C_Mem_Write(&hi2c3, IST8310_IIC_ADDRESS << 1, MemAddress, I2C_MEMADD_SIZE_8BIT, &data_bur, 1, 10);
// }
//
// /**
//  * @brief 通过iic向ist8310写入多个数据
//  * @param MemAddress 寄存器开始地址
//  * @param data 写入数据缓冲区
//  * @param len  写入数据长度
//  */
// void ist8310WriteRegisterMultiple(uint8_t MemAddress, uint8_t *data, uint8_t len) {
//     HAL_I2C_Mem_Write(&hi2c3, IST8310_IIC_ADDRESS << 1, MemAddress, I2C_MEMADD_SIZE_8BIT, data, len, 10);
// }
//
// /**
//   * @brief  设置IST8310磁力计的复位引脚状态
//   * @param  PinState: 引脚状态，可取值GPIO_PIN_SET（高电平）或GPIO_PIN_RESET（低电平）
//   * @note   复位引脚连接到GPIOG的PIN6
//   * @retval None
//   */
// void ist8310_RST_H (void) {
//     HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
// }
//
// void ist8310_RST_L (void) {
//     HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
// }
//
//
// /**
//  * @brief us级延迟(轮询实现)
//  * @param us
//  * @fixme 用中断实现
//  * @todo 重复写
//  */
// void HAL_Delay_us(uint16_t us)
// {
//     uint32_t ticks = 0;
//     uint32_t told = 0, tnow = 0, tcnt = 0;
//     uint32_t reload = 0;
//     reload = SysTick->LOAD;       // 获取SysTick最大重装载值(二十四位递减器: 2^24 - 1 = 16777215) (计数到0后会从从该值开始递减)
//     ticks = us * 168;              // 假设系统时钟为72MHz(1e6 us = 72e6 时钟周期 | 1us = 72时钟周期)
//     told = SysTick->VAL;          // 实时存储当前计数器的数值(递减过程中不断变化)
//     while (1)
//     {
//         tnow = SysTick->VAL;      // 获取当前计数值
//         if (tnow != told){        // 计数器值发生变化时才计数
//
//             /*判断讲解
//              *举例:reload = 100(大概是7位减数器:128) 系统时钟1MHz(1us = 1计数),即ticks = us * 1
//              *目标延迟 50us
//              *
//              *情况1:未溢出
//              *初始时刻:told = 80(记录初始计数)
//              *第一次循环:tnow = 60
//              *判断:tnow(60) < told(80) -> 未溢出
//              *累计计数:tcnt += 80 - 60 = 20
//              *第二次循环:tnow = 30(计数器又减了30)
//              *判断:tnow(30) < told(60) -> 未溢出
//              *累计计数:tcnt += 60 - 30 -> tcnt = 20 + 30 = 50 >= ticks(50) -> break
//              *
//              *情况2:溢出(tnow > told)
//              *初始时刻:told = 20(记录初始计数)
//              *第一次循环:tnow = 90 (计数器从20减到0后,重装载reload = 100,再减到90)
//              *判断:tnow(90) > told(20) ->溢出
//              *累计计数:从told(20)到0的差 + 从reload(100)到tnow(90)的差 = 20 + 10 = 30
//              *第二次循环:tnow = 70(计数器经过一次while从90到70)
//              *判断:tnow(70) < told(90) -> 未溢出
//              *累计计数:tcnt += 70 - 90 -> tcnt = 30 + 20 = 50 >= ticks(50) -> break
//              */
//
//             if (tnow < told)
//             {
//                 tcnt += told - tnow;
//             }
//             else
//             {
//                 tcnt += (reload - tnow) + (told - 0);
//             }
//             told = tnow;
//             if (tcnt >= ticks)
//             {
//                 break;
//             }
//         }
//     }
// }
uint8_t OLED_GRAM[128][8];

void oled_write_byte_dma(uint8_t data, uint8_t cmd) {
    // 静态局部变量,若不使用static,函数结束后,数组
    // 占用的内存会被释放,如果发送的操作有延迟,如DMA,可能还没发送完,数组内部类容就被覆盖了
    static uint8_t cmd_data[2];
    cmd_data[0] = (cmd == OLED_CMD) ? 0x00 : 0x40;
    cmd_data[1] = data;

    // 使用DMA发送,函数立刻返回,数据由DMA控制器传输,不需要等待数据传输完成
    HAL_I2C_Master_Transmit_DMA(&hi2c2, OLED_I2C_ADDRESS, cmd_data, 2);

    // 注意: 需等待前一次DMA传输完成再发起新传输,避免冲突
    while (HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY);
}

/**
 * @brief 该函数主要配置OLED参数，通过调用oled_write_byte_dma传入OLED_CMD，传输控制指令完成配置
 * @brief 流程:先关闭显示->配置硬件参数->最后开启显示
 */
void OLED_init(void) {
    // 1.关闭显示(必须第一步)
    oled_write_byte_dma(0xAE, OLED_CMD);  // DISPLAY_OFF

    // 2.配置内存寻址模式
    oled_write_byte_dma(0x20, OLED_CMD);  // SET_MEMORY_ADDRESSING_MODE
    oled_write_byte_dma(0x10, OLED_CMD);  // 选择页寻址模式(Page Addressing )

    // 3.设置显示方向(段/行映射)
    oled_write_byte_dma(0xA1, OLED_CMD);  // SET_SEGMENT_REMAP (列转行)
    oled_write_byte_dma(0xC8, OLED_CMD);  // SET_COM_SCAN_DIRECTION(行转列)

    // 4.设置多路复用比(在配置COM之前,因为COM引脚配置依赖于多路复用比)
    oled_write_byte_dma(0xA8, OLED_CMD);  // SET_MULTIPLEX_RATIO
    oled_write_byte_dma(0x3F, OLED_CMD);  // 64行驱动(128*64屏幕专用)

    // 5.配置COM引脚硬件
    oled_write_byte_dma(0xDA, OLED_CMD);  // SET_COM_PINS_CONFIG
    oled_write_byte_dma(0x12, OLED_CMD);  // 适配128*64屏幕

    // 6.设置显示偏移
    oled_write_byte_dma(0xD3, OLED_CMD);  // SET_DISPLAY_OFFSET
    oled_write_byte_dma(0x00, OLED_CMD);  // 无偏移

    // 7.设置时钟分频
    oled_write_byte_dma(0xD5, OLED_CMD);  // SET_DISPLAY_CLOCK
    oled_write_byte_dma(0xF0, OLED_CMD);  // 分频比和振荡器频率

    // 8.设置预充电周期
    oled_write_byte_dma(0xD9, OLED_CMD);  // SET_PRE_CHARGE_PERIOD
    oled_write_byte_dma(0x22, OLED_CMD);  // 预充电时间

    // 9.设置VCOMH电压
    oled_write_byte_dma(0xDB, OLED_CMD);  // SET_VCOMH_DESELECT
    oled_write_byte_dma(0x20, OLED_CMD);  // 0.77xVcc

    // 10.使能电荷泵(关键:为OLED提供高压)
    oled_write_byte_dma(0x8D, OLED_CMD);  // SET_CHARGE_PUMP
    oled_write_byte_dma(0x14, OLED_CMD);  // 开启电荷泵

    // 11.恢复显示RAM内容(正常显示模式)
    oled_write_byte_dma(0xA4, OLED_CMD);  // DISPLAY_ALL_ON_RESUME

    // 12.设置正常显示(非反色)
    oled_write_byte_dma(0xA6, OLED_CMD);  // SET_NORMAL_DISPLAY

    // 13.最后一步:开启显示
    oled_write_byte_dma(0xAF, OLED_CMD);  // DISPLAY_ON
}

/**
 * @brief 打开OLED显示
 */
void OLED_display_ON(void) {
    // 1.使能电荷泵
    oled_write_byte_dma(0x8D, OLED_CMD);
    oled_write_byte_dma(0x14, OLED_CMD);

    // 2.开启显示
    oled_write_byte_dma(0xAF, OLED_CMD);
}

/**
 * @brief 关闭OLED显示
 */
void OLED_display_OFF(void) {
    // 1.使能电荷泵
    oled_write_byte_dma(0x8D, OLED_CMD);
    oled_write_byte_dma(0x14, OLED_CMD);

    // 2.关闭显示
    oled_write_byte_dma(0xAE, OLED_CMD);
}


/**
 * @brief 操作GRAM内存(128页 * 8列 = 1024字节数组)
 *
 * @param operate   operate: 操作类型
 *                      PEN_CLEAR: 填充0x00(清屏)
 *                      PEN_WRITE: 填充0xFF(全屏点亮)
 *                      PEN_INVERSION: 按位取反(反色显示)
 */
void OLED_operate_gram(pen_typedef operate) {
    // 计算GRAM总大小(128列 * 8页 = 1024字节)
    const uint16_t GRAM_SIZE = 1024;
    // 将二维数组转为一维指针,简化访问
    uint8_t *gram_ptr = (uint8_t *)OLED_GRAM;

    // 根据操作类型选择处理逻辑
    switch (operate) {
        case PEN_WRITE:
            // 用memset快速填充0xFF,效率高于循环赋值
            memset(gram_ptr, 0xFF, GRAM_SIZE);
            break;
        case PEN_CLEAR:
            // 用memset快速填充0x00
            memset(gram_ptr, 0x00, GRAM_SIZE);
            break;
        case PEN_INVERSION:
            // 对每个字节按位取反,用循环操作
            for (uint16_t i = 0; i < GRAM_SIZE; i++) {
                // 异或操作比减法更高效(x = 0xFF - x 等价于 x = x ^ 0xFF,^:当两者相同时取0,不同时取1)
                gram_ptr[i] ^= 0xFF;
            }
            break;
        default:
            // 处理无效参数(可选: 添加日志或返回值)
            break;
    }
}

/**
 * @brief 将"绘图起点"定位到指定坐标
 *          x:列地址(0~127),对应屏幕水平方向的像素位置
 *          y:页地址(0~7), 对应屏幕垂直方向的页位置(128*64中,每8行为1页,共8页)
 * @param x
 * @param y
 */
void OLED_set_position(uint8_t x, uint8_t y) {
    // 1.设置页地址(0xb0(1011 0000b) + y) page31:page addressing mode
    oled_write_byte_dma((0xb0 + y), OLED_CMD);

    // 2.设置列地址高四位 page30:Set Higher Column Start Address
    //   (00010000 | COLUMN_HIGH) page34:Set the upper start column...10h~1Fh
    //   (x & 0xF0(1111 0000) >> 4) | 10h(0001 0000b)
    oled_write_byte_dma((((x & 0xF0) >> 4) | 0x10), OLED_CMD);

    // 3.设置列地址第四位 page30:Set Lower Column Start Address
    oled_write_byte_dma(x & 0x0F, OLED_CMD);
}

/**
  * @brief          操作屏幕单个像素点(128*64分辨率)
  * @param[in]      x: x轴坐标 [0, X_WIDTH-1]
  * @param[in]      y: y轴坐标 [0, Y_WIDTH-1]
  * @param[in]      operate: 操作类型
  *                   PEN_CLEAR: 像素置0(灭)
  *                   PEN_WRITE: 像素置1(亮)
  *                   PEN_INVERSION: 像素状态反转
  * @retval         none
  */
void OLED_draw_point(int8_t x, int8_t y, pen_typedef operate) {
    // 1.参数范围检查
    if ((x > 127) || (y > 63) || (x < 0) || (y < 0)) {
        return;
    }

    // 2.计算页地址和位偏移
    //   计算像素所在页,如y=10,10/8 = 1 -> 第一页
    uint8_t page = y / 8;
    //   计算像素所在页的行,如y=10,10%8 = 2 -> 第三行
    uint8_t row  = y % 8;

    switch (operate) {
        case PEN_WRITE:
            // |=(或运算): 目标位被强制设为1,其他位保存不变
            // 如:row = 2, 1 << 2 = 0b 0000 0100
            // 如:原字节:0b 1000 0001 运算后:0b 1000 0101(仅bit2强制设为1)
            OLED_GRAM[x][page]  |= 1 << row;
            break;
        case PEN_CLEAR:
            // &=(与运算): 目标位被强制设为0,其他位保持不变
            // 如:row = 2, 1 << 2 = 0b 0000 0100,取反后:0b 1111 1011
            // 如:原字节:0b 1000 0101 运算后:0b 1000 0001(仅bit2强制设为0)
            OLED_GRAM[x][page]  &= ~(1 << row);
            break;
        case PEN_INVERSION:
            // ^=(异或运算): 目标位与1异或会翻转(0->1,1->0),其他位与0异或保持不变
            // 如:row = 2, 1 << 2 = 0b 0000 0100
            // 如:原字节:0b 1000 0101 运算后:0b 1000 0001(仅bit2被强制翻转)
            OLED_GRAM[x][page]  ^= 1 << row;
            break;
        default:
            break;
    }
}

void OLED_refresh_gram(void) {
    uint8_t page, column;
    for (page = 0; page < 8; page++) {
        // 1. 设置当前页地址
        OLED_set_position(0, page);  // 从第0列开始刷新当前页
        // 2. 发送该页的128列数据
        for (column = 0; column < 128; column++) {
            // 发送数据：cmd=0表示数据（根据OLED硬件定义，可能是0x40）
            oled_write_byte_dma(OLED_GRAM[column][page], 0x40);  // 0x40为数据命令标志
        }
    }
}