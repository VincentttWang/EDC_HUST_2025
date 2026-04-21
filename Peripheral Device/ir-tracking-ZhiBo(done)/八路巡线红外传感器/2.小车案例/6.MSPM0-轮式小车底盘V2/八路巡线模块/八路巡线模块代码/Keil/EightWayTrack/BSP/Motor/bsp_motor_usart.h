#ifndef __BSP_MOTOR_USART_H_
#define __BSP_MOTOR_USART_H_

#include "ti_msp_dl_config.h"
#include "app_motor_usart.h"

void Motor_Usart_init (void);
void Send_Motor_U8(uint8_t Data);
void Send_Motor_ArrayU8(uint8_t *pData, uint16_t Length);
void UART_Console_write(uint8_t *data, uint16_t size);


#endif

