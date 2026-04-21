#include "bsp.h"
void BSP_init(void)
{
	SystemInit();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); 
	
	
	delay_init();
//	delay_ms(1000); //等待红外稳定    Wait for infrared to stabilize
	
	
	USART1_init(115200);
	USART2_init(115200);//使用串口2 接收红外    Use serial port 2 to receive infrared
	
    IIC_Motor_Init();//四路电机通信初始化    Four-way motor communication initialization


	//放到最后才生效，不然还是无法正常使用    It will take effect at the end, otherwise it will not work properly.
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//禁用jlink 只用SWD调试口，PA15、PB3、4做普通IO  Disable jlink and use only SWD debug port, PA15, PB3, 4 as normal IO
	
	
}
