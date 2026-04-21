#include "bsp_timer.h"

u16 timer_delay_cnt = 0;



void my_delay_10ms(u16 time)
{
	timer_delay_cnt = time;
	while(timer_delay_cnt != 0);//不为0
}

/**************************************************************************
函数功能：TIM7初始化，定时10us
入口参数：无
返回  值：无
**************************************************************************/
void TIM7_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); //使能定时器的时钟
	TIM_TimeBaseStructure.TIM_Prescaler = 71;			 // 预分频器
	TIM_TimeBaseStructure.TIM_Period = 9;				 //设定计数器自动重装值
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
	TIM_ClearFlag(TIM7, TIM_FLAG_Update);               //清除TIM的更新标志位
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;			  //TIM中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  //从优先级1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);							  //初始化NVIC寄存器

	TIM_Cmd(TIM7, ENABLE);
}


// TIM7中断
void TIM7_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) //检查TIM更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update);    //清除TIMx更新中断标志
	}
}
