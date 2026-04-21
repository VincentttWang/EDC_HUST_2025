#include "stm32f10x.h"
#include "delay.h"
#include "bsp_usart.h"
#include "bsp_irtracking.h"


extern u8 g_new_package_flag;

int main()
{
	
	SystemInit();
	delay_init();
	delay_ms(1000);//µÈ´ıºìÍâÎÈ¶¨
	
	uart_init(115200);
	irtracking_init();
	
	while(1)
	{
		printf_IR_IO_Data();
		delay_ms(300);
		
	}
}

