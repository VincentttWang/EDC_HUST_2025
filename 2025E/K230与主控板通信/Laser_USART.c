#include "Laser_USART.h"

/*
1.数据格式 激光串口接收数组(初筛):激光位置返回帧: 0x12 数据个数-> 大量数据(高位和低位) 0x5B
2.参数更改:
3.调用方法:
4.注意点:
*/

// 串口接收数据函数
uint8_t USART_LASER_RX_BUF[USART_LASER_RX_BUF_LEN] = {0};
// 激光串口位置数组(含值)
int Laser_Loc[USART_LASER_RX_BUF_LEN] = {0};

// ********************可变区域********************
// 标志位处理
int Laser_flag = 1 ; // flag = 0 : 正常	// flag = 1 : 没找到(初始状态)	// flag = 2 : 丢失		// flag = 3 : 掉帧(肯定是一起掉帧)
int Rect_flag  = 1 ; // flag = 0 : 正常	// flag = 1 : 没找到(初始状态)	// flag = 2 : 丢失		// flag = 3 : 掉帧(肯定是一起掉帧)

uint16_t Laser_Location[20] = {0};		// 存储激光坐标:target_x , target_y , real_x , real_y
uint16_t  Rect_Location[20] = {0};		// 存储矩形坐标:
int error_Laser = -1;
int error_Rect = -1;
// ********************可变区域********************

// 串口数据接收处理
int Laser_Rx_Count = 0 ;
bool is_RX_Complete = false ;


// ****************串口相关*******************

// 激光串口初始化
void Laser_USART_Init(void)
{
		//清除串口中断标志
    NVIC_ClearPendingIRQ(LASER_INI_IRQn);
    //使能串口中断
    NVIC_EnableIRQ(LASER_INI_IRQn);
}

//串口发送单个字符
void Laser_send_char(char ch)
{
    //当串口0忙的时候等待，不忙的时候再发送传进来的字符
    while( DL_UART_isBusy(LASER_UART) == true );
    //发送单个字符
    DL_UART_Main_transmitData(LASER_UART, ch);
}

//串口发送字符串
void Laser_send_string(char* str)
{
    //当前字符串地址不在结尾 并且 字符串首地址不为空
    while(*str!=0&&str!=0)
    {
        //发送字符串首地址中的字符，并且在发送完成之后首地址自增
        Laser_send_char(*str++);
    }
}
// ****************激光相关*******************

// *************重点区域******************

// 串口传输数据模式
void K230_UASRT_Mode(void)
{
	// 无条件刷新错误状态
	error_Laser = Laser_flag;
	error_Rect  = Rect_flag;
	// 处理数据(直接不要变量接收了,直接函数return为参数)
	Get_Laser_Msg(DL_UART_Main_receiveData(LASER_UART)) ;
	if (is_RX_Complete == true)
		Laser_Loc_Update() ;
	error_Laser = Get_Laser_Loc(Laser_Location) ;
	error_Rect = Get_Rect_Loc(Rect_Location) ;
}

// 串口处理数据
void Get_Laser_Msg(char uart_data)
{
		// 接收头帧数据并检查
		if (Laser_Rx_Count == 0)
		{
			if (uart_data != 0x12)
			{
				is_RX_Complete = false ;
				Laser_Rx_Count = 0;
				Laser_flag = 3 ;	// 掉帧
				Rect_flag = 3 ;		// 掉帧
			}
			else
			{
				// 正确帧头来了，可以清除掉帧状态（关键）
        Laser_flag = (Laser_flag == 3) ? 0 : Laser_flag;
        Rect_flag  = (Rect_flag == 3) ? 0 : Rect_flag;
				USART_LASER_RX_BUF[Laser_Rx_Count] = uart_data ;
				Laser_Rx_Count = (Laser_Rx_Count + 1) % Laser_Msg_Num;
				
			}
		}
		else if (Laser_Rx_Count == Laser_Msg_Num - 1)	// 接收尾帧数据并检查
		{
			if (uart_data != 0x5B)
			{
				Laser_Rx_Count = 0;
				Laser_flag = 3 ;  // 掉帧
				Rect_flag = 3 ;		// 掉帧
				is_RX_Complete = false ;
			}
			else			
			{
				USART_LASER_RX_BUF[Laser_Rx_Count] = uart_data ;
				Laser_Rx_Count = (Laser_Rx_Count + 1) % Laser_Msg_Num;
				is_RX_Complete = true ;
			}
		}
		else if (Laser_Rx_Count > 0 && Laser_Rx_Count < Laser_Msg_Num - 1 )
		{
			USART_LASER_RX_BUF[Laser_Rx_Count] = uart_data ;
			Laser_Rx_Count = (Laser_Rx_Count + 1) % Laser_Msg_Num;
		}
		else
	 {
			Laser_Rx_Count = 0 ;
			Laser_flag = 3 ;  // 掉帧
			Rect_flag = 3 ;		// 掉帧
		}
	 if (Laser_flag == 3 || Rect_flag == 3)
		 clear_array( USART_LASER_RX_BUF , USART_LASER_RX_BUF_LEN ) ;		 // 掉帧就清空接收数组
}

// 错误返回函数1
int Get_Laser_Loc(uint16_t data[])
{
	// 定义初始化没找到状态
	static int Find_Laser_None = 1 ;
	// 得到相关数组
	Get_K230_data(Laser_Type , data) ;
	// 以后再也不会有初始化没找到的情况了
	if (Laser_flag != 1)
		Find_Laser_None = 0 ;
	// 掉帧导致数组值永远不变
	if (Laser_flag == 3)
		return Laser_flag ; 
	// 对于数组为空的情况
	if ( Array_Empty(data , Laser_Num) ) 
	{
		// 第一次丢失判定为初始化没找到
		if (Find_Laser_None == 1)
			Laser_flag = 1 ;
		else		// 以后再丢失判定为追踪丢失
			Laser_flag = 2 ;
	}
	else
		Laser_flag = 0 ;	// 不空判定为正常(前提是没有掉帧)
	return Laser_flag ;
}

// 错误返回函数2
int Get_Rect_Loc(uint16_t data[])
{
	// 定义初始化没找到状态
	static int Find_Rect_None = 1 ;
	// 得到相关数组
	Get_K230_data(Rect_Type , data) ;
	// 掉帧导致数组值永远不变
	if (Laser_flag == 3)
		return Laser_flag ; 
	// 以后再也不会有初始化没找到的情况了
	if (Rect_flag != 1)
		Find_Rect_None = 0 ;
	if ( Array_Empty(data , Rect_Num) ) 
	{
		// 第一次丢失判定为初始化没找到
		if (Find_Rect_None == 1)
		{
			DL_GPIO_togglePins(LED_PORT , LED_LED0_PIN ) ;
			Rect_flag = 1 ;
		}
		else		// 以后再丢失判定为丢失
			Rect_flag = 2 ;
	}
	else
		Laser_flag = 0 ;	// 不空判定为正常
	return Laser_flag ;
}

// 得到摄像头指定数据
int Get_K230_data(int DataType , uint16_t receive_data[])
{
	int begin_index = 0 ;
	int data_num = 0 ;
	switch (DataType)
	{
		case Laser_Type:
			begin_index = 0 ;
			data_num = Laser_Num ;
			break ;
		case Rect_Type :
			begin_index = 4 ;
			data_num = Rect_Num ;
			break ;
		default:
			return 0 ;
	}
	Fill_Data(receive_data , begin_index , data_num ) ;
	return 1 ;
}

// *********************************************************

// 发送顺序：先高位，再低位
// 得到数据高位
uint8_t Get_High_Val_Of_Hex( uint16_t value )
{
	return (value >> 8) & 0xFF;
}
// 得到数据低位
uint8_t Get_Low_Val_Of_Hex( uint16_t value )
{
	return value & 0xFF;
}
// 高位+低位转换为完整数据
uint16_t Get_Val_Of_Hex( uint8_t high , uint8_t low )
{
	return ((uint16_t)high << 8) | low;
}
// 更新激光信息地址
void Laser_Loc_Update( void )
{
	// 0:起始帧(0x12)	1:数据个数(偶数)	 (偶数 + 奇数 : 数据位)	(最后一位:结束帧(0x5B))
	int data_num = USART_LASER_RX_BUF[1] ; // 得到数据个数,如果掉帧了,接收数组清空,那么数据个数就是0,那么自然原数组不变
	for (	int i = 0  ; i < data_num ; i += 2 )
	{
		Laser_Loc[i / 2] = Get_Val_Of_Hex(USART_LASER_RX_BUF[i + 2], USART_LASER_RX_BUF[i + 3]);
	}
}

// 填充相关数据
void Fill_Data(uint16_t data[] , int begin_index , int data_num)
{
	for (int i = 0 ; i < data_num ; i ++)
	{
		data[i] = Laser_Loc[begin_index + i] ;
	}
}

// 检测数组是否为空
bool Array_Empty(uint16_t arr[], int len)
{
    for (int i = 0; i < len; i++)
    {
        if ( arr[i] != 0 )
            return false;
    }
    return true;
}
// 清空数组
void clear_array(uint8_t arr[], int len)
{
    for (int i = 0; i < len; i++)
    {
        arr[i] = 0;
    }
}

