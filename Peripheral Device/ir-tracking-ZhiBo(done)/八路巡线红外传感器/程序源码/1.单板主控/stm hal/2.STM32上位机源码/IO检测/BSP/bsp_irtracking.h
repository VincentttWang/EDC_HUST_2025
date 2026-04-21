#ifndef _BSP_IRTRACKING_H_
#define _BSP_IRTRACKING_H_

#include "stm32f10x.h"
#include "delay.h"
#include "bsp_usart.h"

//X1\2\3\4\5\6\7\8对应模块的旋钮开关   使用电磁的接口吧

 #define IR_X1_Clk 	RCC_APB2Periph_GPIOC
#define IR_X1_Pin 	GPIO_Pin_0
#define IR_X1_Port 	GPIOC

#define IR_X2_Clk 	RCC_APB2Periph_GPIOC
#define IR_X2_Pin 	GPIO_Pin_1
#define IR_X2_Port 	GPIOC

#define IR_X3_Clk 	RCC_APB2Periph_GPIOC
#define IR_X3_Pin 	GPIO_Pin_2
#define IR_X3_Port 	GPIOC

#define IR_X4_Clk 	RCC_APB2Periph_GPIOC
#define IR_X4_Pin 	GPIO_Pin_3
#define IR_X4_Port 	GPIOC

#define IR_X5_Clk 	RCC_APB2Periph_GPIOA
#define IR_X5_Pin 	GPIO_Pin_4
#define IR_X5_Port 	GPIOA

#define IR_X6_Clk 	RCC_APB2Periph_GPIOA
#define IR_X6_Pin 	GPIO_Pin_5
#define IR_X6_Port 	GPIOA

#define IR_X7_Clk 	RCC_APB2Periph_GPIOB
#define IR_X7_Pin 	GPIO_Pin_0
#define IR_X7_Port 	GPIOB

#define IR_X8_Clk 	RCC_APB2Periph_GPIOB
#define IR_X8_Pin 	GPIO_Pin_1
#define IR_X8_Port 	GPIOB




//红外状态
#define IN_X1 GPIO_ReadInputDataBit(IR_X1_Port, IR_X1_Pin)
#define IN_X2 GPIO_ReadInputDataBit(IR_X2_Port, IR_X2_Pin)

#define IN_X3 GPIO_ReadInputDataBit(IR_X3_Port, IR_X3_Pin)
#define IN_X4 GPIO_ReadInputDataBit(IR_X4_Port, IR_X4_Pin)

#define IN_X5 GPIO_ReadInputDataBit(IR_X5_Port, IR_X5_Pin)
#define IN_X6 GPIO_ReadInputDataBit(IR_X6_Port, IR_X6_Pin)

#define IN_X7 GPIO_ReadInputDataBit(IR_X7_Port, IR_X7_Pin)
#define IN_X8 GPIO_ReadInputDataBit(IR_X8_Port, IR_X8_Pin)


void irtracking_init(void);
void printf_IR_IO_Data(void);

#endif

