//
// Created by DingYong on 25-8-10.
//


#ifndef IST8310_H
#define IST8310_H

#include <stdint.h>

#define MAG_SEN 0.3f                    // MAG_SEN:磁场测量灵敏度,在ist8310手册4.4提到灵敏度为0.3uT/LSB(LSB为最低有效位,即1uT = 1LBS)

#define ist8310_WAI 0x00                // WhoAmI寄存器地址(DeviceID):0x00
#define ist8310_WAI_DefaultValue 0x10   // 设备ID(DeviceID)默认值

#define IST8310_OK               0x11   // 设备寻址正常返回值
#define IST8310_NO_SENSOR        0x02   // 设备寻址失败返回值

uint8_t ist8310Init(void);
void ist8310ReadRegister(float magneticField[3]);

#endif //IST8310_H
