//
// Created by DingYong on 25-8-16.
//

#ifndef AUTOAIMING_H
#define AUTOAIMING_H
#include <stdbool.h>
#include <stdint.h>

#include "User/connectivity/VPC/vpc.h"

extern bool has_new_cmd;
extern A0_Frame a0_frame;

void autoAiming(void const * argument);

#endif //AUTOAIMING_H
