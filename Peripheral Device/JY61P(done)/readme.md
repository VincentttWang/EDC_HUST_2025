## 注意：

- 每次使用最好先进行校准（Z轴归零、加计校准），校准需要用上位机（MiniIMU)

  > 通过上位机可以修改传输速率、波特率等参数！

- USART2开启，并开启中断

  **波特率为115200！**

- 引入JY61P库文件

## 代码

```c
/* USER CODE BEGIN PTD */
//JY61P陀螺仪数据变量
uint8_t GyroscopeUsart3RxBuffer[33];      //接收缓存
double GyroscopeChannelData[10];
uint8_t tempBuffer=0,RxBuffer;
/* USER CODE END PTD */

/* USER CODE BEGIN 2 */
//开启JY61P接收中断，注意波特率为9600
JY61P_Init(&huart2);
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) //中断处理函数
{
  if(huart->Instance == USART2)
	{
		IT_JY61P();
	}
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) //中断回调函数
{
        if(RESET != __HAL_UART_GET_FLAG(huart,UART_FLAG_IDLE))   //判断是否是空闲中断
        {
            __HAL_UART_CLEAR_IDLEFLAG(&huart2);                     //清楚空闲中断标志（否则会一直不断进入中断）
            HAL_UART_RxCpltCallback(&huart2);                          //调用中断处理函数
        }	
}
/* USER CODE END 4 */
//数据调用方式：(都为double型变量)
        //GyroscopeChannelData[0]	//X轴加速度（m/s*s）
        //GyroscopeChannelData[1]	//Y轴加速度
        //GyroscopeChannelData[2]	//Z轴加速度
        //GyroscopeChannelData[3]	//X轴角速度（°/s）
        //GyroscopeChannelData[4]	//Y轴角速度
        //GyroscopeChannelData[5]	//Z轴角速度            
        //GyroscopeChannelData[6]	//X轴(滚转角)角度（°）
        //GyroscopeChannelData[7]	//Y轴(俯仰角)角度
        //GyroscopeChannelData[8]	//Z轴(偏航角)角度
        //GyroscopeChannelData[9]	//实时温度（摄氏度）
```

需要在AllHeader.h中

```c
#include "REG.h"
#include "wit_c_sdk.h"
//注意，两者顺序不可调换
```

