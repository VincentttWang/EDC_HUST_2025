/*
 * bsp_usart.c
 *
 *  Created on: 2023年10月11日
 *      Author: YB-101
 */

#include "bsp_usart.h"
#include "usart.h"

uint8_t RxTemp = 0;
#define USART_DEBUG huart1

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&USART_DEBUG, (uint8_t *)&ch, 1, 0xFFFF); // 阻塞方式打印,串口x
  return ch;
}
/*
 * 初始化串口1
 * */
void Bsp_UART1_Init(void)
{
  HAL_UART_Receive_IT(&huart1, (uint8_t *)&RxTemp, 1);
}
/*
 * 重写串口1接收函数 Rewrite usart1 receiving function
 * */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  UNUSED(huart);

  HAL_UART_Receive_IT(&huart1, (uint8_t *)&RxTemp, 1);

  HAL_UART_Transmit(&huart1, (uint8_t *)&RxTemp, 1, 0xFFFF);
}
