#include "tracking_delay.h"

// HAL库已经处理了SysTick的初始化，所以delay_init通常不需要了。
// 如果你想保留它，可以将其内容清空或用于其他初始化。
void delay_init(void)
{
    // HAL库初始化后，SysTick已经配置好了，通常不需要额外的初始化
}

/**********************************************************
** 函数名: delay_ms
** 功能描述: 延时nms
** 输入参数: nms
** 输出参数: 无
** 说明：直接使用HAL_Delay
***********************************************************/
void delay_ms(uint16_t nms)
{
    HAL_Delay(nms);
}

/**********************************************************
** 函数名: delay_us
** 功能描述: 延时nus，nus为要延时的us数.
** 输入参数: nus
** 输出参数: 无
** 说明：HAL库没有直接的微秒延时函数，需要自己实现。
        这里提供一个基于__NOP()的简单实现，但精度不高且受优化影响。
        更精确的实现需要使用定时器，例如TIM或DWT。
***********************************************************/
void delay_us(uint32_t nus)
{
    uint32_t start_tick = SysTick->VAL;
    uint32_t ticks_per_us = SystemCoreClock / 1000000;
    uint32_t ticks_to_wait = nus * ticks_per_us;
    uint32_t current_tick;
    uint32_t elapsed_ticks = 0;

    if (ticks_to_wait == 0) return;

    do {
        current_tick = SysTick->VAL;
        if (current_tick > start_tick) { // 计数器从LOAD到0，所以这里是倒计数
            elapsed_ticks += (start_tick + SysTick->LOAD + 1 - current_tick);
        } else {
            elapsed_ticks += (start_tick - current_tick);
        }
        start_tick = current_tick;
    } while (elapsed_ticks < ticks_to_wait);
}