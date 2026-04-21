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
#include "AllHeader.h"

#define SystemClock 32000000

/**
 * @brief 微秒级延时函数（基于 SysTick）
 * @param xus 延时时长，单位：微秒（最大值根据时钟而定）
 */
void Delay_us(uint32_t xus)
{
//    // 假设 SystemCoreClock 已经定义，并等于 CPU 主频（例如 48MHz）
//    uint32_t ticks = (SystemClock / 1000000) * xus;

//    SysTick->LOAD  = ticks - 1;  // 设置重载值
//    SysTick->VAL   = 0;          // 清空当前值
//    SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;  // 启动 SysTick（使用内核时钟）

//    while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == 0); // 等待计数到0

//    SysTick->CTRL  = 0; // 关闭 SysTick
}

/**
 * @brief 毫秒级延时函数
 * @param xms 延时时长，单位：毫秒
 */
void Delay_ms(uint32_t xms)
{
	uint32_t start_time, current_tick;
      __disable_irq(); // 禁用中断
      start_time = tick;
      __enable_irq(); // 恢复中断

			while(1) {
        __disable_irq();
        current_tick = tick;
        __enable_irq();
        if ((uint32_t)(current_tick - start_time) >= xms) {
            break;
        }
    }
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


uint32_t Gettick()
{
	__disable_irq(); // 禁用中断
      uint32_t t = tick;
      __enable_irq(); // 恢复中断
	return t;
} 