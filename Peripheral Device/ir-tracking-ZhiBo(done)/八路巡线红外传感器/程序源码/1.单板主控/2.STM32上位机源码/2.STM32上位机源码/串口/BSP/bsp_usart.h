#ifndef __BSP_USART_H_
#define __BSP_USART_H_


#include "stm32f10x.h"
#include "app_usart2.h"
#include "stdio.h"

void uart_init(u32 bound);
void USART1_Send_U8(uint8_t ch);
void USART1_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length);


void USART2_init(u32 baudrate);
void USART2_Send_U8(uint8_t ch);
void USART2_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length);

#endif

