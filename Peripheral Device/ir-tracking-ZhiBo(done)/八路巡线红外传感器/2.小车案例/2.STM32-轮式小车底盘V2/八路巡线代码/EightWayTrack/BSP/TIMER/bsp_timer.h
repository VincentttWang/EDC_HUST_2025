#ifndef __BSP_TIMER_H__
#define __BSP_TIMER_H__

#include "stm32f10x.h"
#include "AllHeader.h"

void TIM6_Init(void);
void TIM7_Init(void);

void my_delay_10ms(u16 time);

#endif
