/**
  ******************************************************************************
  * @file    key.h
  * @brief   TI MSPM0G3507平台按键驱动头文件 - 支持长短按检测
  * @author  移植并重构自STM32平台
  * @date    2025-07-31
  ******************************************************************************
  * @attention
  *
  * 本驱动支持多按键的长短按检测，包括消抖、连击、长按重复等功能
  * 适用于TI MSPM0G3507平台的GPIO按键输入
  *
  ******************************************************************************
  */

#ifndef KEY_H
#define KEY_H

#include "ti_msp_dl_config.h"
#include <stdbool.h>
#include <stdint.h>

// 按键配置 - 根据实际硬件修改
#define KEY_PORT                Key_PORT                  // 按键 GPIO端口
#define KEY_PIN                 Key_PIN_1_PIN          // 按键 GPIO引脚

// 按键逻辑电平定义
#define KEY_PRESSED_LEVEL       0                       // 按键按下时的电平 (0=低电平有效, 1=高电平有效)
#define KEY_RELEASED_LEVEL      (!KEY_PRESSED_LEVEL)    // 按键释放时的电平

// 按键时间参数配置 (单位: ms)
#define KEY_DEBOUNCE_TIME       20                      // 消抖时间
#define KEY_SHORT_PRESS_TIME    50                      // 短按最小时间
#define KEY_LONG_PRESS_TIME     1000                    // 长按判定时间
#define KEY_REPEAT_TIME         200                     // 长按重复间隔
#define KEY_DOUBLE_CLICK_TIME   300                     // 双击间隔时间

// 按键事件类型
typedef enum {
    KEY_EVENT_NONE = 0,         // 无事件
    KEY_EVENT_PRESS,            // 按键按下
    KEY_EVENT_RELEASE,          // 按键释放
    KEY_EVENT_SHORT_PRESS,      // 短按
    KEY_EVENT_LONG_PRESS,       // 长按
    KEY_EVENT_LONG_PRESS_REPEAT,// 长按重复
    KEY_EVENT_DOUBLE_CLICK      // 双击
} key_event_t;

// 按键状态
typedef enum {
    KEY_STATE_IDLE = 0,         // 空闲状态
    KEY_STATE_DEBOUNCE,         // 消抖状态
    KEY_STATE_PRESSED,          // 按下状态
    KEY_STATE_LONG_PRESS,       // 长按状态
    KEY_STATE_WAIT_DOUBLE       // 等待双击状态
} key_state_t;

// 按键ID定义
typedef enum {
    KEY_ID_1 = 0,               // 按键1
    KEY_ID_MAX                  // 按键数量
} key_id_t;

// 按键数据结构
typedef struct {
    GPIO_Regs*  port;           // GPIO端口
    uint32_t    pin;            // GPIO引脚
    key_state_t state;          // 当前状态
    uint8_t     current_level;  // 当前电平
    uint8_t     last_level;     // 上次电平
    uint32_t    press_time;     // 按下时间戳
    uint32_t    release_time;   // 释放时间戳
    uint32_t    last_event_time;// 上次事件时间
    uint8_t     click_count;    // 连击计数
    bool        long_press_flag;// 长按标志
} key_info_t;
// 函数声明

/**
 * @brief 按键系统初始化
 * @note 初始化所有按键的GPIO和状态
 */
void Key_Init(void);

/**
 * @brief 按键扫描处理 (需要在主循环或定时器中调用)
 * @note 建议调用间隔: 5-10ms
 */
void Key_Scan(void);

/**
 * @brief 获取指定按键的事件
 * @param key_id 按键ID
 * @return 按键事件类型
 */
key_event_t Key_GetEvent(key_id_t key_id);

/**
 * @brief 检查指定按键是否按下
 * @param key_id 按键ID
 * @return true=按下, false=释放
 */
bool Key_IsPressed(key_id_t key_id);

/**
 * @brief 检查指定按键的短按事件
 * @param key_id 按键ID
 * @return true=检测到短按, false=无短按
 */
bool Key_IsShortPress(key_id_t key_id);

/**
 * @brief 检查指定按键的长按事件
 * @param key_id 按键ID
 * @return true=检测到长按, false=无长按
 */
bool Key_IsLongPress(key_id_t key_id);

/**
 * @brief 检查指定按键的双击事件
 * @param key_id 按键ID
 * @return true=检测到双击, false=无双击
 */
bool Key_IsDoubleClick(key_id_t key_id);

/**
 * @brief 清除指定按键的所有事件
 * @param key_id 按键ID
 */
void Key_ClearEvent(key_id_t key_id);

/**
 * @brief 清除所有按键的所有事件
 */
void Key_ClearAllEvents(void);

// 便捷宏定义 - 兼容原有接口
#define Key_Read()                  Key_IsPressed(KEY_ID_1)
#define Key_short_press()           Key_IsShortPress(KEY_ID_1)
#define Key_long_press()            Key_IsLongPress(KEY_ID_1)
#define Key_double_click()          Key_IsDoubleClick(KEY_ID_1)

#endif // KEY_H