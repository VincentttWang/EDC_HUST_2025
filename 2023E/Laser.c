#include "RTE_Components.h"             // Component selection
#include "Laser.h"

/*
 -----------串口通信相关引脚配置----------
 串口接线:(按照我的f4板子和CanMV)
 STM32_TX == PA9  ----- 12 == CanMV_RX(上)
 STM32_RX == PA10 ----- 11 == CanMV_TX(下)
*/


/* ------串口-激光通信相关参数初始化------ */

// 接收状态和缓冲区(用于该串口通信的各种信息的提取)
int USART_LASER_RX_BUF[USART_LASER_RX_BUF_LEN] ;

// 激光坐标记录数组
int Laser_Loc[USART_LASER_RX_BUF_LEN] ;

// 报错机制
extern int error ;

/* ---------串口-激光通信相关函数--------- */

// 发布激光坐标数据请求
void Laser_Ask_for_Loc ()
{
		if (HUART_LASER.gState == HAL_UART_STATE_READY) 
		{ 
			HAL_UART_Transmit_IT(&HUART_LASER, (uint8_t *)"L", 1); // 发送请求
		}
}

// 得到激光屏幕位置
int Laser_Get_Loc( uint8_t USART_LASER_RX_BUF[] )
{
		HAL_GPIO_TogglePin(LED0_GPIO_Port,  LED0_Pin) ;
		// 激光位置返回帧: 0x12 target_x target_y real_x real_y 0x5B
		if ( USART_LASER_RX_BUF[0] == 0x12 && USART_LASER_RX_BUF[5] == 0x5B )
		{			
			// 得到激光相关数据
			for ( int j = 0 ; j < 4; j ++ )
			{
				Laser_Loc[j] = (int)USART_LASER_RX_BUF[j + 1] * 4 ; // 得到真实坐标
			}
			return 1 ;
		}
		else
		{
			// 报错机制
			return -1 ;
		}
}

// 激光坐标模式
void Laser_Mode( uint8_t USART_LASER_RX_BUF[] , UART_HandleTypeDef * huart)
{
	if (huart->Instance == USART_LASER)
	{
 		// ------------激光模式判定------------
			Laser_Ask_for_Loc() ;
		
			error = Laser_Get_Loc(USART_LASER_RX_BUF) ;
		
		// 重启接收前清除缓存区
		memset(USART_LASER_RX_BUF, 0, USART_LASER_RX_BUF_LEN);
 
	  // 再次接收
		HAL_UARTEx_ReceiveToIdle_IT(&HUART_LASER, USART_LASER_RX_BUF, USART_LASER_RX_BUF_LEN);
	}
}


