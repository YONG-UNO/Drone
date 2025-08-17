//
// Created by DingYong on 25-8-16.
//

#include "autoAiming.h"

#include "cmsis_os.h"
#include "usbd_cdc_if.h"
#include "User/connectivity/VPC/vpc.h"

// 函数任务: 处理接收指令并发送反馈
void autoAiming(void const * argument) {
    (void)argument;      // 显示告知编译器此参数未使用

    for (;;) {
        // 检查是否有新的上位机指令
        // if (has_new_cmd) {
            // 处理指令(示例:是否追踪)
            if (a0_frame.find_bool == 1) {
                // 执行追踪逻辑
            }

            // 向上位机发送反馈
            /* char start;                // 0: 帧头 's'
             * char type;                 // 1: 消息类型 0xB0
             * float yaw;                 // 2-5: 偏航角 (4字节)
             * float pitch;               // 6-9: 俯仰角 (4字节)
             * uint8_t enemy_team_color;  // 10: 敌方颜色 (0红 / 1蓝)
             * uint8_t mode;              // 11: 模式(0自瞄 / 1符)
             * uint8_t rune_flag;         // 12: 符模式(0不可激活/1小符/2大符)
             * char reserve[18];          // 13-30: 预留空位(18字节)
             * char end;                  // 31: 帧尾 'e'
            */
            send_b0_frame(
                1.23f,
                -0.45f,
                0,
                0,
                0
            );

            has_new_cmd = false;     // 清除标志位
        // }

        osDelay(10);  // 10ms周期(避免频繁通讯)
    }
}
