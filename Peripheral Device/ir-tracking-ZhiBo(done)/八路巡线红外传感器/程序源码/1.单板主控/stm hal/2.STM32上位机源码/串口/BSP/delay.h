#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f1xx_hal.h" // 替换为HAL库头文件

void delay_init(void); // 此函数可能不再需要，因为SysTick由HAL库管理
// void delay_ms(uint16_t nms); // 可以直接使用HAL_Delay
void delay_us(uint32_t nus); // 需要自己实现或者替换为其他更精确的延时方式

#endif