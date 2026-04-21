#ifndef __LASER_H
#define __LASER_H

#include "usart.h"		// 串口用
#include "string.h"		// 字符串/数组处理
#include <stdint.h>		// uint类型使用

// 定义串口接受长度
#define USART_LASER_RX_BUF_LEN 25

// 定义激光模式串口号
#define HUART_LASER huart1
#define USART_LASER USART1

// 发布激光坐标数据请求
void Laser_Ask_for_Loc () ;
	
// 得到激光屏幕位置
int Laser_Get_Loc(uint8_t USART_RX_BUF[] ) ;

void Laser_Mode( uint8_t USART_RX_BUF[] ,UART_HandleTypeDef * huart) ;

#endif
