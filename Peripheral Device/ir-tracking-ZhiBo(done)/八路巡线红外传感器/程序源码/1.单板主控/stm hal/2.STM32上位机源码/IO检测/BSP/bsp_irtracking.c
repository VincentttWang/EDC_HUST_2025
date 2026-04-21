/*4路循迹驱动文件*/

#include "bsp_irtracking.h"

void irtracking_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(IR_X1_Clk|IR_X2_Clk|IR_X3_Clk|IR_X4_Clk, ENABLE); 
	RCC_APB2PeriphClockCmd(IR_X5_Clk|IR_X6_Clk|IR_X7_Clk|IR_X8_Clk, ENABLE); 
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin=IR_X1_Pin;
  GPIO_Init(IR_X1_Port,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin=IR_X2_Pin;
  GPIO_Init(IR_X2_Port,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin=IR_X3_Pin;
  GPIO_Init(IR_X3_Port,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin=IR_X4_Pin;
  GPIO_Init(IR_X4_Port,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin=IR_X5_Pin;
  GPIO_Init(IR_X5_Port,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin=IR_X6_Pin;
  GPIO_Init(IR_X6_Port,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin=IR_X7_Pin;
  GPIO_Init(IR_X7_Port,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin=IR_X8_Pin;
  GPIO_Init(IR_X8_Port,&GPIO_InitStructure);
	
	
}

//巡线探头的处理
static void track_deal_four(u8 *s1,u8 *s2,u8 *s3,u8 *s4,u8 *s5,u8 *s6,u8 *s7,u8 *s8)
{
	*s1 = IN_X1;
	*s2 = IN_X2;
	*s3 = IN_X3;
	*s4 = IN_X4;
	
	*s5 = IN_X5;
	*s6 = IN_X6;
	*s7 = IN_X7;
	*s8 = IN_X8;
}


void printf_IR_IO_Data()
{
	static u8 x1,x2,x3,x4,x5,x6,x7,x8;
	track_deal_four(&x1,&x2,&x3,&x4,&x5,&x6,&x7,&x8);
	printf("%d\t %d\t %d\t %d\t %d\t %d\t %d\t %d\t \r\n",x1,x2,x3,x4,x5,x6,x7,x8);

	
}


