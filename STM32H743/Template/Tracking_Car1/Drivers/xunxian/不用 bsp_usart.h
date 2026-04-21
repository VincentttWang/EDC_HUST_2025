#ifndef __BSP_USART_H_
#define __BSP_USART_H_

#include "stm32h7xx_hal.h" // 替换为HAL库头文件
#include "app_usart2.h"
#include "stdio.h"
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
// HAL库通常会使用HAL_UART_Transmit和HAL_UART_Receive，这些函数会处理数据的发送和接收。
// 因此，你可能不再需要直接暴露USART1_Send_U8和USART1_Send_ArrayU8，
// 但为了保持API一致性，可以保留它们作为对HAL函数的封装。

void uart_init(u32 bound); // 将作为USART1的初始化函数
void USART1_Send_U8(uint8_t ch);
void USART1_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length);


void USART2_init(u32 baudrate); // 将作为USART2的初始化函数
void USART2_Send_U8(uint8_t ch);
void USART2_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length);

#endif