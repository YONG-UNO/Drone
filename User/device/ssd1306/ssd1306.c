//
// Created by DingYong on 25-8-20.
//

#include "ssd1306.h"

#include <math.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#include "i2c.h"

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
void OLED_draw_point(uint8_t x, uint8_t y, pen_typedef operate) {
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

/**
 * @brief 画一条直线,从起点到终点
 * @param x1 起点横坐标
 * @param y1 起点纵坐标
 * @param x2 终点横坐标
 * @param y2 终点纵坐标
 * @param operate PEN_WRITE,PEN_CLEAR,PEN_INVERSION
 */
void OLED_draw_line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, pen_typedef operate) {
    uint8_t column = 0, row = 0;
    uint8_t x_start = 0, x_end = 0, y_start = 0, y_end = 0;

    if (y1 == y2) {
        (x1 <= x2) ? (x_start = x1) : (x_start = x2);
        (x1 <= x2) ? (x_end   = x2) : (x_end   = x1);

        for (column = x_start; column <= x_end; column++) {
            OLED_draw_point(column, y1, operate);
        }
    }else if (x1 == x2) {
        (y1 <= y2) ? (y_start = y1) : (y_start = y2);
        (y1 <= y2) ? (y_end   = y2) : (y_end   = y1);

        for (row = y_start; row <= y_end; row++) {
            OLED_draw_point(x1, row, operate);
        }
    }else {
        float k = (float)(y2 - y1) / (float)(x2 - x1);
        float b = (float)y1 - k * (float)x1;

        (x1 <= x2) ? (x_start = x1) : (x_start = x2);
        (x1 <= x2) ? (x_end   = x2) : (x_end   = x1);

        for (column = x_start; column <= x_end; column++) {
            OLED_draw_point(column, (uint8_t)roundf(k * column + b), operate);
        }
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
