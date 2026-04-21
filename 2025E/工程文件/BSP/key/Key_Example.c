/**
 * @file Key_Example.c
 * @brief TI MSPM0G3507平台按键驱动使用示例
 * @author 移植并重构自STM32平台
 * @date 2025-07-31
 * 
 * 本文件演示如何在TI MSPM0G3507平台上使用按键驱动
 * 包括短按、长按、双击等功能的检测和处理
 */

#include "key.h"
#include "ti_msp_dl_config.h"

/**
 * @brief 按键基本功能测试
 */
void Key_Basic_Test(void)
{
    // 初始化按键
    Key_Init();
    
    while(1) {
        // 按键扫描 (建议在定时器中断中调用)
        Key_Scan();
        
        // 检查按键事件
        if (Key_short_press()) {
            // 按键短按处理
            // 例如：切换LED状态
        }
        
        if (Key_long_press()) {
            // 按键长按处理
            // 例如：进入设置模式
        }
        
        if (Key_double_click()) {
            // 按键双击处理
            // 例如：复位系统
        }
        }
        
        // 5ms延时 (按键扫描间隔)
        DL_Common_delayCycles(160000);  // 5ms @ 32MHz
    }
}

/**
 * @brief 使用事件模式的按键处理示例
 */
void Key_Event_Mode_Example(void)
{
    Key_Init();
    
    while(1) {
        Key_Scan();
        
        // 获取按键事件
        key_event_t key_event = Key_GetEvent(KEY_ID_1);
        switch(key_event) {
            case KEY_EVENT_PRESS:
                // 按键按下
                break;
                
            case KEY_EVENT_RELEASE:
                // 按键释放
                break;
                
            case KEY_EVENT_SHORT_PRESS:
                // 按键短按
                break;
                
            case KEY_EVENT_LONG_PRESS:
                // 按键长按
                break;
                
            case KEY_EVENT_LONG_PRESS_REPEAT:
                // 按键长按重复
                break;
                
            case KEY_EVENT_DOUBLE_CLICK:
                // 按键双击
                break;
                
            default:
                break;
        }
        
        DL_Common_delayCycles(160000);  // 5ms
    }
}

/**
 * @brief 菜单控制示例 (单按键控制)
 */
void Key_Menu_Control_Example(void)
{
    static uint8_t menu_index = 0;
    static uint8_t menu_level = 0;
    const uint8_t max_menu_items = 4;
    
    Key_Init();
    
    while(1) {
        Key_Scan();
        
        // 短按: 菜单导航
        if (Key_short_press()) {
            menu_index = (menu_index + 1) % max_menu_items;
            // 更新菜单显示
        }
        
        // 长按: 确认选择/进入子菜单
        if (Key_long_press()) {
            if (menu_level == 0) {
                // 执行当前菜单项功能或进入子菜单
                switch(menu_index) {
                    case 0:
                        // 菜单项0功能
                        break;
                    case 1:
                        // 菜单项1功能
                        break;
                    case 2:
                        // 菜单项2功能
                        break;
                    case 3:
                        // 菜单项3功能
                        break;
                }
            }
        }
        
        // 双击: 返回上级菜单
        if (Key_double_click()) {
            if (menu_level > 0) {
                menu_level = 0;
                menu_index = 0;
                // 返回主菜单
            }
        }
        
        // 双击任意键: 回到主菜单
        if (Key1_double_click() || Key2_double_click()) {
            menu_level = 0;
            menu_index = 0;
            // 回到主菜单
        }
        
        DL_Common_delayCycles(160000);
    }
}

/**
 * @brief LED控制示例
 */
void Key_LED_Control_Example(void)
{
    static bool led_state = false;
    static uint8_t brightness = 50;  // 亮度百分比
    
    Key_Init();
    
    while(1) {
        Key_Scan();
        
        // 短按: 切换LED开关
        if (Key_short_press()) {
            led_state = !led_state;
            if (led_state) {
                // 打开LED
                DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_0);
            } else {
                // 关闭LED
                DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_0);
            }
        }
        
        // 长按重复: 调节亮度
        if (Key_GetEvent(KEY_ID_1) == KEY_EVENT_LONG_PRESS_REPEAT) {
            brightness += 10;
            if (brightness > 100) brightness = 10;
            // 设置PWM占空比调节亮度
            // set_led_brightness(brightness);
        }
        
        // 双击: 复位到默认设置
        if (Key_double_click()) {
            brightness = 50;
            led_state = false;
            DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_0);
                both_press_time = 0;
            }
        } else {
            both_press_time = 0;
        }
        
        DL_Common_delayCycles(160000);
    }
}

/**
 * @brief 数值调节示例 (单按键)
 */
void Key_Value_Adjust_Example(void)
{
    static int32_t value = 0;
    static int32_t step = 1;
    static bool increment_mode = true;  // true=增加模式, false=减少模式
    
    Key_Init();
    
    while(1) {
        Key_Scan();
        
        // 获取按键事件
        key_event_t key_event = Key_GetEvent(KEY_ID_1);
        switch(key_event) {
            case KEY_EVENT_SHORT_PRESS:
                // 短按: 根据当前模式增加或减少数值
                if (increment_mode) {
                    value += step;
                } else {
                    value -= step;
                }
                break;
                
            case KEY_EVENT_LONG_PRESS:
                step = 10;  // 长按时步长变大
                if (increment_mode) {
                    value += step;
                } else {
                    value -= step;
                }
                break;
                
            case KEY_EVENT_LONG_PRESS_REPEAT:
                // 长按重复: 连续调节
                if (increment_mode) {
                    value += step;
                } else {
                    value -= step;
                }
                break;
                
            case KEY_EVENT_DOUBLE_CLICK:
                // 双击: 切换增减模式
                increment_mode = !increment_mode;
                break;
        
        // 限制数值范围
        if (value > 999) value = 999;
        if (value < -999) value = -999;
        
        DL_Common_delayCycles(160000);
    }
}

/**
 * @brief 定时器中断方式的按键扫描示例
 */
void Key_Timer_Interrupt_Example(void)
{
    Key_Init();
    
    // 配置定时器每5ms中断一次调用Key_Scan()
    // 在定时器中断服务函数中调用Key_Scan();
    
    while(1) {
        // 主循环处理按键事件
        if (Key_short_press()) {
            // 处理按键短按
        }
        
        if (Key_long_press()) {
            // 处理按键长按
        }
        
        if (Key_double_click()) {
            // 处理按键双击
        }
        
        // 主循环其他任务
        DL_Common_delayCycles(32000);  // 1ms
    }
}

// 定时器中断服务函数示例
void TIMER_IRQHandler(void)
{
    if (DL_Timer_isInterruptPending(TIMER_0_INST, DL_TIMER_INTERRUPT_ZERO_EVENT)) {
        Key_Scan();  // 每5ms调用一次按键扫描
        DL_Timer_clearInterruptStatus(TIMER_0_INST, DL_TIMER_INTERRUPT_ZERO_EVENT);
    }
}
