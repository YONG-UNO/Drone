#include "RS05.h"

// 仅依赖上层工具头文件,不包含任何CAN底层头文件(解耦核心)
#include "conversion.h"
#include "limit.h"

// 引入通用CAN发送接口声明(仅依赖接口,不依赖底层实现)
#include "bsp_can.h"
#include "can.h"

// RS05电机指令数组(上层业务数据, 和底层CAN解耦)
uint8_t RS05_Enable_Array[8]  = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC};	    // 电机使能命令
uint8_t RS05_Disable_Array[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD};       // 电机失能命令
uint8_t RS05_Debug_Array[8]   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};       // 电机调试命令


/**
 * @brief  RS05电机MTT模式指令打包函数
 * @details 负责将MTT模式的5个浮点控制参数，经过限幅、方向矫正、格式转换后，
 *          按照MIT协议的字节分配规则，填充到8字节发送缓冲区中（仅打包，不发送）
 * @param  target_pos      目标位置（物理量，单位：rad，范围：-12.57 ~ 12.57）
 * @param  target_vel      目标速度（物理量，单位：rad/s，范围：-50.0 ~ 50.0）
 * @param  kp              位置环比例系数（范围：0.0 ~ 500.0）
 * @param  kd              速度环微分系数（范围：0.0 ~ 5.0）
 * @param  target_torque   目标力矩（物理量，单位：N·m，范围：-5.5 ~ 5.5）
 * @param  mtt_tx_data     输出参数：8字节MTT指令发送缓冲区（用于存放打包后的协议数据）
 * @note   1. 该函数仅处理上层业务逻辑和协议打包，不涉及任何CAN底层发送操作
 * @note   2. 会对输入参数进行限幅，防止超出电机物理允许范围
 * @note   3. 扭矩方向会进行反向矫正，适配RS05电机的转向定义
 */
void RS05_MIT_Pack(float target_pos, float target_vel, float kp, float kd, float target_torque, uint8_t mtt_tx_data[8]) {

    if (mtt_tx_data == NULL) return;  // 立即终止当前函数的剩余代码执行，继续执行调用处后面的代码

    // 参数限幅(上层业务逻辑,和CAN底层无关)
    target_pos   = setOutLimit(target_pos, P_MAX, P_MIN);
    target_vel   = setOutLimit(target_vel, V_MAX, V_MIN);
    kp           = setOutLimit(kp, KP_MAX, KP_MIN);
    kd           = setOutLimit(kd, KD_MAX, KD_MIN);
    target_torque= setOutLimit(target_torque, T_MAX, T_MIN);

    // 扭矩方向矫正(上层业务逻辑,适配电机转向)
    target_torque = -target_torque;

    // 浮点数转协议整数（上层协议逻辑，复用工具函数）
    uint16_t pos_16bit    = (uint16_t)float_to_uint(target_pos, P_MIN, P_MAX, 16);
    uint16_t vel_12bit    = (uint16_t)float_to_uint(target_vel, V_MIN, V_MAX, 12);
    uint16_t kp_12bit     = (uint16_t)float_to_uint(kp, KP_MIN, KP_MAX, 12);
    uint16_t kd_12bit     = (uint16_t)float_to_uint(kd, KD_MIN, KD_MAX, 12);
    uint16_t torque_12bit = (uint16_t)float_to_uint(target_torque, T_MIN, T_MAX, 12);

    // MTT协议字节拆分（上层协议逻辑，和CAN底层无关）
    mtt_tx_data[0] = (uint8_t)(pos_16bit >> 8);
    mtt_tx_data[1] = (uint8_t)(pos_16bit & 0x00FF);
    mtt_tx_data[2] = (uint8_t)(vel_12bit >> 4);
    mtt_tx_data[3] = (uint8_t)(((vel_12bit & 0x000F) << 4) | ((kp_12bit & 0x0F00) >> 8));
    mtt_tx_data[4] = (uint8_t)(kp_12bit & 0x00FF);
    mtt_tx_data[5] = (uint8_t)(kd_12bit >> 4);
    mtt_tx_data[6] = (uint8_t)(((kd_12bit & 0x000F) << 4) | ((torque_12bit & 0x0F00) >> 8));
    mtt_tx_data[7] = (uint8_t)(torque_12bit & 0x00FF);
}

/**
 * @brief  RS05电机MIT（MTT）模式控制入口函数
 * @details 该函数是RS05电机MIT模式控制的上层统一入口，内部完成「MTT指令打包」和「通用CAN发送」两个核心步骤，
 *          无需上层关心协议细节和CAN底层操作，直接返回发送结果，方便业务逻辑判断。
 * @param  target_pos      目标位置（物理量，单位：rad，范围：-12.57 ~ 12.57）
 * @param  target_vel      目标速度（物理量，单位：rad/s，范围：-50.0 ~ 50.0）
 * @param  kp              位置环比例系数（范围：0.0 ~ 500.0）
 * @param  kd              速度环微分系数（范围：0.0 ~ 5.0）
 * @param  target_torque   目标力矩（物理量，单位：N·m，范围：-5.5 ~ 5.5）
 * @retval bool            发送结果：true表示CAN指令发送成功，false表示发送失败（空指针/邮箱满/硬件错误）
 * @note   1. 函数内部会先调用RS05_MIT_Pack完成指令打包，再调用BSP_CAN_Send_Msg完成底层发送
 * @note   2. 依赖宏定义RS05_CAN_HANDLE（CAN句柄）和RS05_CAN_ID（电机CAN ID），需提前配置正确
 * @note   3. 发送失败不影响上层业务逻辑，仅返回结果，建议上层根据返回值做重发或报错处理
 * @note   4. 该函数是解耦后的上层业务入口，与CAN底层实现无直接耦合，可灵活适配不同CAN硬件
 */
bool RS05_MIT_Control(float target_pos, float target_vel, float kp, float kd, float target_torque){
    uint8_t mtt_tx_data[8] = {0};

    // 打包mit数据
    RS05_MIT_Pack(target_pos, target_vel, kp, kd, target_torque, mtt_tx_data);

    // 调用通用CAN发送接口
    // 传入:CAN句柄(&hcan?), RS05 CAN ID, 打包后的数据
    return BSP_CAN_Send_Msg(RS05_CAN_HANDLE, RS05_CAN_ID,  mtt_tx_data);
}

/**
 * @brief  RS05电机反馈数据解析函数
 * @details 负责将CAN接收的8字节原始数据，按照MIT协议的字节定义，解析为电机的位置、速度、力矩数据，
 *          同时完成整数到浮点物理量的转换，存入反馈数据结构体中
 * @param  motor_measure_RS05  输出参数：RS05电机反馈数据结构体（用于存放解析后的完整反馈信息）
 * @param  StdId               CAN消息ID（未实际使用，保留以兼容统一解析接口格式）
 * @param  rx_data             输入参数：CAN接收的8字节原始数据缓冲区
 * @note   1. 该函数仅处理上层协议解析，不涉及任何CAN底层接收操作
 * @note   2. 解析规则严格匹配RS05电机的MIT协议反馈格式
 * @note   3. 最终会将整数映射值转换为直观的浮点物理量，方便上层业务使用
 */
void get_motor_measure_RS05(motor_measure_RS05_t *motor_measure_RS05,uint32_t StdId, const uint8_t rx_data[]) {
  motor_measure_RS05->p_int = (rx_data[1] << 8) | rx_data[2];          // 位置映射整数（协议：0~65535 → 实际：-12.57~12.57rad）
  motor_measure_RS05->v_int = (rx_data[3] << 4) | (rx_data[4] >> 4);   // 速度映射整数（协议：0~4096 → 实际：-50~50rad/s）
  motor_measure_RS05->t_int = ((rx_data[4] & 0xF) << 8) | rx_data[5];  // 力矩映射整数（协议：0~4096 → 实际：-5.5~5.5N·m）
  motor_measure_RS05->position = uint_to_float(motor_measure_RS05 -> p_int, -P_MAX, P_MAX, 16);    // 实际位置
  motor_measure_RS05->velocity = uint_to_float(motor_measure_RS05 -> v_int, -V_MAX, V_MAX, 12);    // 实际速度
  motor_measure_RS05->torque   = uint_to_float(motor_measure_RS05 -> t_int, -T_MAX, T_MAX, 12);    // 实际力矩
}


/**
 * @brief  RS05电机使能函数
 * @details 发送RS05电机标准使能指令，使电机进入可控制状态，是电机响应MIT指令的前置条件
 * @retval bool  发送结果：true表示使能指令发送成功，false表示发送失败
 * @note   1. 电机使能后需要短暂延时（50ms左右），确保电机完成内部初始化流程
 * @note   2. 若发送失败，可多次调用该函数进行重发，提升使能成功率
 */
bool RS05_Enable(void)
{
    // 调用通用CAN发送接口，发送使能指令
    return BSP_CAN_Send_Msg(RS05_CAN_HANDLE, RS05_CAN_ID, RS05_Enable_Array);
}

/**
 * @brief  RS05电机失能函数
 * @details 发送RS05电机标准失能指令，使电机进入休眠状态，停止响应任何控制指令
 * @retval bool  发送结果：true表示失能指令发送成功，false表示发送失败
 * @note   1. 电机失能后会立即停止运转，适用于设备停机、故障保护等场景
 * @note   2. 失能指令优先级高于MIT控制指令，发送后电机将不再响应后续MIT指令
 */
bool RS05_Disable(void)
{
    // 调用通用CAN发送接口，发送失能指令
    return BSP_CAN_Send_Msg(RS05_CAN_HANDLE, RS05_CAN_ID, RS05_Disable_Array);
}

/**
 * @brief  RS05电机全0调试指令发送函数
 * @details 发送RS05电机全0填充的调试指令，用于电机通信测试、状态复位或空载调试，
 *          指令缓冲区8个字节均为0x00，无实际控制意义，仅用于验证CAN通信链路是否通畅
 * @retval bool  发送结果：true表示全0调试指令发送成功，false表示发送失败（空指针/邮箱满/硬件错误）
 * @note   1. 该指令仅用于调试排错，无实际控制效果，电机不会响应任何动作指令
 * @note   2. 可用于验证CAN总线、电机通信地址、底层发送接口是否正常工作
 * @note   3. 发送成功仅代表指令已送入CAN发送邮箱，不代表电机已接收或解析成功
 * @note   4. 依赖宏定义RS05_CAN_HANDLE（CAN句柄）、RS05_CAN_ID（电机ID）和RS05_Debug_Array（全0指令数组）
 */
bool RS05_Debug(void) {
    return BSP_CAN_Send_Msg(RS05_CAN_HANDLE, RS05_CAN_ID, RS05_Debug_Array);
}

// 使用
// #include "RS05.h"
// #include "CAN.h"
//
// int main(void)
// {
//     // 1. 现有初始化逻辑（保留不变，完成CAN、时钟等初始化）
//     HAL_Init();
//     SystemClock_Config();
//     MX_CAN1_Init();
//     MX_CAN2_Init();
//     CAN_Filter_Init();
//
//     // 2. RS05电机先使能（必须步骤，电机只有使能后才能响应MIT指令）
//     RS05_Enable();
//     HAL_Delay(50);  // 短暂延时，确保电机完成使能流程，避免指令丢失
//
//     while (1)
//     {
//         // 3. 调用RS05_MIT_Control，实现极慢转动
//         // 参数配置：目标位置0rad，目标速度0.01rad/s（极慢），KP=100（位置保持），KD=1（抑制抖动），力矩0.5N·m（微弱驱动力）
//         RS05_MIT_Control(0.0f, 0.01f, 100.0f, 1.0f, 0.5f);
//
//         // 4. 延时20ms，保证指令发送频率稳定，避免电机过载
//         HAL_Delay(20);
//     }
// }