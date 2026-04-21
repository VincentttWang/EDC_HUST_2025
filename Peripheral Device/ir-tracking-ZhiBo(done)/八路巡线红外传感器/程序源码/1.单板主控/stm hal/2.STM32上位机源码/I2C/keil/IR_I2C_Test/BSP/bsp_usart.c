#include <bsp_usart.h>

extern UART_HandleTypeDef huart1;
uint8_t RxTemp = 0;

/* USER CODE BEGIN 0 */
//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
		while((USART1->SR&0X40)==0);//Flag_Show!=0  使用串口1   
		USART1->DR = (uint8_t) ch;      
		return ch;
}

#endif




/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
//void USART1_UART_Init(void)
//{
//	//配置不配中断,不需要此串口中断
////  // Start receiving interrupt 启动接收中断
////  HAL_UART_Receive_IT(&huart1, (uint8_t *)&RxTemp, 1);
//}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  UNUSED(huart);

  HAL_UART_Receive_IT(&huart1, (uint8_t *)&RxTemp, 1);

  HAL_UART_Transmit(&huart1, (uint8_t *)&RxTemp, 1, 0xFFFF);
}
