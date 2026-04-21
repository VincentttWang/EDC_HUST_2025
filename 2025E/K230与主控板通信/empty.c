/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include "OLED.h"
#include "Delay.h"

#include "Laser_USART.h"
	
extern uint16_t Laser_Location[20];		// 存储激光坐标:target_x , target_y , real_x , real_y
extern uint16_t Rect_Location[20];		// 存储矩形坐标:
extern bool is_RX_Complete ;
extern int error_Laser;
extern int error_Rect;
extern int a ;

int main(void)
{
    SYSCFG_DL_init();
		
		// ****** 串口操作 ******
		Laser_USART_Init() ;	// 初始化
		
    while (1) 
		{
			
    }
}

//串口的中断服务函数
void UART_0_INST_IRQHandler(void)
{
    //如果产生了串口中断
    switch( DL_UART_getPendingInterrupt(LASER_UART) )
    {
			  //如果是接收中断
        case DL_UART_IIDX_RX:
					  K230_UASRT_Mode() ;
            break;
        default://其他的串口中断
            break;
    }
}

//void SysTick_Handler(void)
//{
//		// 自分频
//		static int k = 0 ;
//		k ++ ;
//		if (k == 1000)
//		{
//			k = 0 ;
////			DL_GPIO_togglePins(LED_PORT , LED_LED0_PIN) ;
//		}
//}

