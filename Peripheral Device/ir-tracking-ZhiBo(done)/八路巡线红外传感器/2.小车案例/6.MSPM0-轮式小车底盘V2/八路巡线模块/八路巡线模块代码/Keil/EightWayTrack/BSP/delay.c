#include "delay.h"

volatile unsigned int delay_times = 0;

//搭配滴答定时器实现的精确us延时
//Accurate us delay with tick timer
void delay_us(unsigned long __us) 
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 38;

    // 计算需要的时钟数 = 延迟微秒数 * 每微秒的时钟数
	// Calculate the number of clocks required = delay microseconds * number of clocks per microsecond
    ticks = __us * (32000000 / 1000000);

    // 获取当前的SysTick值
	// Get the current SysTick value
    told = SysTick->VAL;

    while (1)
    {
        // 重复刷新获取当前的SysTick值
		// Repeatedly refresh to get the current SysTick value
        tnow = SysTick->VAL;

        if (tnow != told)
        {
            if (tnow < told)
                tcnt += told - tnow;
            else
                tcnt += SysTick->LOAD - tnow + told;

            told = tnow;

            // 如果达到了需要的时钟数，就退出循环
			// If the required number of clocks is reached, exit the loop
            if (tcnt >= ticks)
                break;
        }
    }
}
//搭配滴答定时器实现的精确ms延时
//Accurate ms delay with tick timer
void delay_ms(unsigned long ms) 
{
	delay_us( ms * 1000 );
}

//void SysTick_Handler(void)
//{
//	if(delay_times != 0)
//	{
//		delay_times--;
//	}
//}
