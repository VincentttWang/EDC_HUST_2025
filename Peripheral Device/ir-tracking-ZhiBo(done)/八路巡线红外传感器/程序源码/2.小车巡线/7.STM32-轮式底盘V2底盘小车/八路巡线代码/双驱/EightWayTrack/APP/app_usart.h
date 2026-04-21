#ifndef __APP_USART2_H_
#define __APP_USART2_H_


#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#include "stm32f10x.h"

#include "AllHeader.h"

#define IR_Num 8 //探头数量


extern u8 IR_Data_number[];
extern u16 IR_Data_Anglo[];
extern u8 g_Amode_Data ;//模拟型标志
extern u8 g_Dmode_Data ;//数字型标志

void Deal_IR_Usart(u8 rxtemp);
void Deal_Usart_Data(void);
void Deal_Usart_AData(void);
void send_control_data(u8 adjust,u8 aData,u8 dData);
#endif

