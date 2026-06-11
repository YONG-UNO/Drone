/**
*   @file dev_daemon.h
*   @brief 守护进程(看门狗)软件实现头文件
*   @author HuaCheng Ma
*   @date 2026/1/16
*   @version 0.1
*   @note 该功能需要FreeRTOS才可使用
*/

#ifndef DEV_DAEMON_H
#define DEV_DAEMON_H

#include <stdbool.h>
#include <stdint.h>


typedef struct _Daemon_Instance_s
{
    uint32_t timeout_ms;      //看门狗超时时间，单位ms
    uint32_t last_feed_time;  //上次喂狗时间，单位ms
    bool is_timeout;        //看门狗是否超时
    bool is_started;       //看门狗是否启动
    void (*daemon_callback)(struct _Daemon_Instance_s*); //看门狗超时回调函数
    void (*daemon_reload_callback)(struct _Daemon_Instance_s*); //看门狗重置回调
} Daemon_Instance_s;

typedef struct {
    uint32_t timeout_ms; //看门狗超时时间，单位ms
    void (*daemon_callback)(Daemon_Instance_s*);
    void (*daemon_reload_callback)(Daemon_Instance_s*);
} Daemon_InitConfig_s;



/**
 * @brief 注册一个看门狗实例
 * @param config 看门狗初始化配置指针
 * @return 看门狗实例指针
 */
Daemon_Instance_s* Daemon_Register(const Daemon_InitConfig_s* config);

/**
 * @brief 看门狗开始
 * @param instance 看门狗实例指针
 */
void Daemon_Start(Daemon_Instance_s* instance);

/**
 * @brief 喂狗
 * @param instance 看门狗实例指针
 */
void Daemon_Feed(Daemon_Instance_s* instance);

/**
 * @brief 重置
 * @param instance 看门狗实例指针
 */
void Daemon_Reload(Daemon_Instance_s* instance);

/**
 * @brief 看门狗步进函数，需要定时调用
 * @param instance 看门狗实例指针
 * @return 如果看门狗超时返回false(代表发生异常) ，否则返回true
 */
bool Daemon_Step(Daemon_Instance_s* instance);


#endif //DEV_DAEMON_H