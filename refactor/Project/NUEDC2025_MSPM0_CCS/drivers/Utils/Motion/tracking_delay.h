#ifndef __DELAY_H
#define __DELAY_H
#include <stdint.h>

// 自定义常用数据类型
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;

// 如果也需要有符号类型，可以添加
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;

void delay_init(void); // 此函数可能不再需要，因为SysTick由HAL库管理
// void delay_ms(uint16_t nms); // 可以直接使用HAL_Delay
void delay_us(uint32_t nus); // 需要自己实现或者替换为其他更精确的延时方式

#endif