#ifndef __LASER_USART_H
#define __LASER_USART_H
#include "ti_msp_dl_config.h"
#include <stdbool.h>

// 接受数组长度
#define USART_LASER_RX_BUF_LEN 50
// 串口宏定义
#define LASER_UART UART0
#define LASER_INI_IRQn	UART0_INT_IRQn
// 数据个数处理
#define Laser_Msg_Num 19 	// 帧格式的长度 = K230_MSG_DATA_NUM * 2 + 3 (高低位+头尾帧+数据个数标定)

// 数据存储位置
#define Laser_Type 1
#define Rect_Type  2

#define Laser_Num 4
#define Rect_Num  4

// 串口操作函数
void Laser_send_char(char ch);
void Laser_send_string(char* str);
void Laser_USART_Init(void) ;
// 数据处理
void Get_Laser_Msg(char uart_data) ;	// 得到串口数据
void Laser_Loc_Update( void ) ;				// 更新激光位置
void Fill_Data(uint16_t data[] , int begin_index , int data_num) ;	// 内部函数:填充相应数组
int Get_K230_data(int DataType , uint16_t receive_data[]) ;					// 得到指定类型数据
// 错误处理
void clear_array(uint8_t arr[], int len) ;
bool Array_Empty(uint16_t arr[], int len) ;
// 数组回调 + 错误处理
int Get_Laser_Loc(uint16_t data[]) ;
int Get_Rect_Loc(uint16_t data[]) ;
// 串口模式
void K230_UASRT_Mode(void);
#endif
