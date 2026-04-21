#include "RTE_Components.h"             // Component selection
#include "Laser.h"
#include  "Cont_SMotor.h"
#include "stdbool.h"
/*
 -----------����ͨ�������������----------
 ���ڽ���:(�����ҵ�f4���Ӻ�CanMV)
 STM32_TX == PA9  ----- 12 == CanMV_RX(��)
 STM32_RX == PA10 ----- 11 == CanMV_TX(��)
*/


/* ------����-����ͨ����ز�����ʼ��------ */

// ����״̬�ͻ�����(���ڸô���ͨ�ŵĸ�����Ϣ����ȡ)
int USART_LASER_RX_BUF[USART_LASER_RX_BUF_LEN] ;

// ���������¼����
int Laser_Loc[USART_LASER_RX_BUF_LEN] ;

// ��������
extern int error ;

extern bool is_updated; // 是否更新了数据
extern Coordinate target_position; // 目标位置
extern Coordinate laser_position; // 激光雷达位置

/* ---------����-����ͨ����غ���--------- */

// ��������������������
void Laser_Ask_for_Loc ()
{
		if (HUART_LASER.gState == HAL_UART_STATE_READY) 
		{ 
			HAL_UART_Transmit_IT(&HUART_LASER, (uint8_t *)"L", 1); // 请求激光雷达位置
		}
}

// �õ�������Ļλ��
int Laser_Get_Loc( uint8_t USART_LASER_RX_BUF[] )
{
		// ����λ�÷���֡: 0x12 target_x target_y real_x real_y 0x5B
		if ( USART_LASER_RX_BUF[0] == 0x12 && USART_LASER_RX_BUF[5] == 0x5B )
		{			
			// �õ������������
			for ( int j = 0 ; j < 4; j ++ )
			{
				Laser_Loc[j] = (int)USART_LASER_RX_BUF[j + 1] * 4 ; // �õ���ʵ����
			}
			
			is_updated = true; // 设置数据已更新标志
			target_position.x = Laser_Loc[0] / 100.0f; // 目标位置X
			target_position.y = Laser_Loc[1] / 100.0f; // 目标位置Y
			laser_position.x = Laser_Loc[2] / 100.0f; // 激光雷达位置X
			laser_position.y = Laser_Loc[3] / 100.0f; // 激光雷达位置Y
			if(target_position.x == 0 && target_position.y == 0 || laser_position.x == 0 && laser_position.y == 0) {
				is_updated = false; // 设置数据未更新标志
			}
			return 1 ;
		}
		else
		{
			// ��������
			return -1 ;
		}
}

// ��������ģʽ
void Laser_Mode( uint8_t USART_LASER_RX_BUF[] , UART_HandleTypeDef * huart)
{
	if (huart->Instance == USART_LASER)
	{
 		// ------------����ģʽ�ж�------------
		#ifdef DEBUG 
		bulbula your debug code
		#endif
			Laser_Ask_for_Loc() ;
		
			error = Laser_Get_Loc(USART_LASER_RX_BUF) ;
		
		// ��������ǰ���������
		memset(USART_LASER_RX_BUF, 0, USART_LASER_RX_BUF_LEN);
 
	  // �ٴν���
		HAL_UARTEx_ReceiveToIdle_IT(&HUART_LASER, USART_LASER_RX_BUF, USART_LASER_RX_BUF_LEN);
	}
	else
	{
		error = 2;
		memset(USART_LASER_RX_BUF, 0, USART_LASER_RX_BUF_LEN);
		HAL_UARTEx_ReceiveToIdle_IT(&HUART_LASER, USART_LASER_RX_BUF, USART_LASER_RX_BUF_LEN);
	}
}


