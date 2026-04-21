/*
 * bsp_usart.h
 *
 *  Created on: 2023年10月11日
 *      Author: YB-101
 */

#ifndef BSP_USART_H_
#define BSP_USART_H_

#include "stdio.h"
#include "main.h"
extern UART_HandleTypeDef huart1;

void USART1_UART_Init(void);

void USART1_DataByte(uint8_t data_byte);
void USART1_DataString(uint8_t * data_str, uint16_t datasize);


#endif /* BSP_USART_H_ */
