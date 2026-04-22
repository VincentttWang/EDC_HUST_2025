/*
#include <stdint.h>
#include "ti_msp_dl_config.h"

void Delay_ms(volatile uint32_t ms)
{
	while(ms--)
		delay_cycles(CPUCLK_FREQ / 1000) ;
}

void Delay_us(volatile uint32_t us)
{
	while(us--)
		delay_cycles(CPUCLK_FREQ / 1000000) ;
}
*/

#include <ti/devices/msp/msp.h>  // 重要：包含 SysTick 的定义
#include "ti_msp_dl_config.h"

#define SystemClock 32000000

/**
 * @brief 微秒级延时函数（基于 SysTick）
 * @param xus 延时时长，单位：微秒（最大值根据时钟而定）
 */
void Delay_us(uint32_t xus)
{
    // 假设 SystemCoreClock 已经定义，并等于 CPU 主频（例如 48MHz）
    uint32_t ticks = (SystemClock / 1000000) * xus;

    SysTick->LOAD  = ticks - 1;  // 设置重载值
    SysTick->VAL   = 0;          // 清空当前值
    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;  // 启动 SysTick（使用内核时钟）

    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0); // 等待计数到0

    SysTick->CTRL  = 0; // 关闭 SysTick
}

/**
 * @brief 毫秒级延时函数
 * @param xms 延时时长，单位：毫秒
 */
void Delay_ms(uint32_t xms)
{
    while (xms--)
        Delay_us(1000);
}

/**
 * @brief 秒级延时函数
 * @param xs 延时时长，单位：秒
 */
void Delay_s(uint32_t xs)
{
    while (xs--)
        Delay_ms(1000);
}

