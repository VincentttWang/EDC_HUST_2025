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

// 测试区
int a ;


int main(void)
{
    SYSCFG_DL_init();

	  // OLED配置,需要在C/C++添加路径: 				D:\appDownload\STM32\Ti\mspm0_sdk_2_04_00_06\test3_OLED\ , 否则找不到路径
		OLED_Init() ;
		OLED_Clear() ;
		OLED_ShowString(1,1,"hello") ;

    while (1) 
		{
			Delay_ms(1000) ;
//			DL_GPIO_togglePins(LED_PORT , LED_LED0_PIN) ;
			OLED_ShowNum(2,1,a++,3) ;
    }
}

void SysTick_Handler(void)
{
		// 自分频
		static int k = 0 ;
		k ++ ;
		if (k == 1000)
		{
			k = 0 ;
			DL_GPIO_togglePins(LED_PORT , LED_LED0_PIN) ;
		}
}

//#include "ti_msp_dl_config.h"

//volatile unsigned int delay_times = 0;

//void delay_ms(unsigned int ms) 
//{
//        delay_times = ms;
//        while( delay_times != 0 );
//}        

//int main(void)
//{
//    
//    SYSCFG_DL_init();
//    
//    while (1) 
//    {                        
//       
//        DL_GPIO_setPins(LED_PORT, LED_LED0_PIN);
//        delay_ms(1000);
//       
//        DL_GPIO_clearPins(LED_PORT, LED_LED0_PIN);
//        delay_ms(1000);
//    }
//}

//void SysTick_Handler(void)
//{
//        if( delay_times != 0 )
//        {
//                delay_times--;
//        }
//}




