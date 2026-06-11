//
// Created by DingYong on 2026/5/13.
//

#include "video_transfer.h"
#include <stddef.h>
#include <stdlib.h>
#include <usart.h>

int16_t value_x[8] = {0};
int16_t value_y[8] = {0};
int16_t value_z[8] = {0};

// 全局 接收缓冲区
uint8_t vt_rx_buf[VT_FRAME_LEN];
// 全局 解析结果
vt_t vt_data;
// 全局 图传切换
bool vt_switch;

static int16_t filter_mouse_value(int16_t mouse_value, int16_t *value);
static uint16_t crc16_init = 0xffff;
static const uint16_t crc16_table[256] =
{
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

/**
 * @brief Get the crc16 checksum
 *
 * @param p_msg Data to check
 * @param lenData length
 * @param crc16 Crc16 initialized checksum
 * @return crc16 Crc16 checksum
 */
static uint16_t get_crc16_check_sum(uint8_t *p_msg, uint16_t len, uint16_t crc16)
{
    uint8_t data;

    if(p_msg == NULL)
    {
        return 0xffff;
    }

    while(len--)
    {
        data = *p_msg++;
        (crc16) = ((uint16_t)(crc16) >> 8) ^ crc16_table[((uint16_t)(crc16) ^ (uint16_t)(data)) & 0x00ff];
    }

    return crc16;
}

/**
 * @brief crc16 verify function
 *
 * @param p_msg Data to verify
 * @param len Stream length=data+checksum
 * @return bool Crc16 check result
 */
bool verify_crc16_check_sum(uint8_t *p_msg, uint16_t len)
{
    uint16_t w_expected = 0;

    if((p_msg == NULL) || (len <= 2))
    {
        return false;
    }
    w_expected = get_crc16_check_sum(p_msg, len - 2, crc16_init);

    return ((w_expected & 0xff) == p_msg[len - 2] && ((w_expected >> 8) & 0xff) == p_msg[len - 1]);
}

void vt_init(void) {
    // 开启DMA 接收21 bytes
    HAL_UART_Receive_DMA(VT_UART_HANDEL, vt_rx_buf, VT_FRAME_LEN);
}

void vt_decode(uint8_t *buf, vt_t *out) {
    // 类型转换
    vt_raw_t *raw = (vt_raw_t *)buf;

    //
    out->ch0 = (int16_t)raw->ch0 - 1024;
    out->ch1 = (int16_t)raw->ch1 - 1024;
    out->ch2 = (int16_t)raw->ch2 - 1024;
    out->ch3 = (int16_t)raw->ch3 - 1024;

    out->mouse_x = filter_mouse_value(raw->mouse_x, value_x);
    out->mouse_y = filter_mouse_value(raw->mouse_y, value_y);
    out->mouse_z = filter_mouse_value(raw->mouse_z, value_z);

    out->mouse_left = raw->mouse_left;
    out->mouse_right = raw->mouse_right;
    out->mouse_middle = raw->mouse_middle;

    out->key = raw->key;
    out->ascii_key = 0;
    if (raw->key & (1 << 0))   out->ascii_key = 'W';
    if (raw->key & (1 << 1))   out->ascii_key = 'S';
    if (raw->key & (1 << 2))   out->ascii_key = 'A';
    if (raw->key & (1 << 3))   out->ascii_key = 'D';
    if (raw->key & (1 << 4))   out->ascii_key = 's';
    if (raw->key & (1 << 5))   out->ascii_key = 'c';
    if (raw->key & (1 << 6))   out->ascii_key = 'Q';
    if (raw->key & (1 << 7))   out->ascii_key = 'E';
    if (raw->key & (1 << 8))   out->ascii_key = 'R';
    if (raw->key & (1 << 9))   out->ascii_key = 'F';
    if (raw->key & (1 << 10))  out->ascii_key = 'G';
    if (raw->key & (1 << 11))  out->ascii_key = 'Z';
    if (raw->key & (1 << 12))  out->ascii_key = 'X';
    if (raw->key & (1 << 13))  out->ascii_key = 'C';
    if (raw->key & (1 << 14))  out->ascii_key = 'V';
    if (raw->key & (1 << 15))  out->ascii_key = 'B';

}


static int16_t filter_mouse_value(int16_t mouse_value, int16_t *value)
{
    if (value == NULL) return mouse_value;

    // 8 点滑动窗口
    for (int i = 6; i >= 0; --i)
    {
        value[i + 1] = value[i];
    }
    value[0] = mouse_value;

    // 丢帧/噪声判断（和 VTM 完全一样）
    if (mouse_value == 0 || mouse_value == 255)
    {
        uint8_t cnt = 0;
        for (uint8_t i = 0; i < 8; i++)
        {
            if (value[i] == mouse_value) cnt++;
        }

        if (cnt >= 4)
        {
            return mouse_value;
        }
        else
        {
            for (uint8_t i = 1; i < 8; i++)
            {
                if (value[i] != 0 && value[i] != 255)
                    return value[i];
            }
            return value[1];
        }
    }

    return mouse_value;
}