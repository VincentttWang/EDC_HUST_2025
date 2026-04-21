#ifndef __APP_USART2_H_
#define __APP_USART2_H_


#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#include "stm32h7xx_hal.h" // 替换为HAL库头文件
#include "AllHeader.h"


#define IR_Num 8 //探头数量


extern uint8_t IR_Data_number[];
extern uint16_t IR_Data_Anglo[];
extern uint8_t g_Amode_Data ;//模拟型标志
extern uint8_t g_Dmode_Data ;//数字型标志

void Deal_IR_Usart(uint8_t rxtemp);
void Deal_Usart_Data(void);
void Deal_Usart_AData(void);
void send_control_data(uint8_t adjust,uint8_t aData,uint8_t dData);
void splitString(char* mystrArray[],char *str, const char *delimiter) ;
#endif