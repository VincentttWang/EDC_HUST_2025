## 硬件连接：

- USART3连接八路巡线器
- USART1连接串口助手

## 工程配置：

- USART3需要打开global interrupt，波特率115200
- Driver库中导入BSP/ir-tracking文件夹，内含库文件

## 代码：

```c
/* USER CODE BEGIN Includes */
#include "AllHeader.h"
/* USER CODE END Includes */

/* USER CODE BEGIN PV */
char message[100];
extern uint8_t g_new_package_flag;
uint8_t Rx3_Temp; // 用于接收USART3数据的临时变量
/* USER CODE END PV */

/* USER CODE BEGIN 2 */
//先等待红外稳定，再将八路巡线器初始化
HAL_Delay(1000);
HAL_UART_Receive_IT(&huart3, &Rx3_Temp, 1);
HAL_UART_Transmit(&huart3, (uint8_t *)"$0,0,1#", 7, HAL_MAX_DELAY);
HAL_UART_Receive_IT(&huart3, &Rx3_Temp, 1);
/* USER CODE END 2 */

/* USER CODE BEGIN WHILE */
while (1)
{
    if(g_new_package_flag == 1)//当接收到新数据包时
    {
        g_new_package_flag = 0;
        Deal_Usart_Data();//将数据转换成数组量
        sprintf(message,"x1:%d,x2:%d,x3:%d,x4:%d,x5:%d,x6:%d,x7:%d,x8:%d\r\n",IR_Data_number[0],IR_Data_number[1],IR_Data_number[2],IR_Data_number[3],IR_Data_number[4],IR_Data_number[5],IR_Data_number[6],IR_Data_number[7]);
        HAL_UART_Transmit(&huart1,(uint8_t *)message, strlen(message), HAL_MAX_DELAY);  
        //巡线器数据存在IR_Data_number[]数组中
    }
/* USER CODE END WHILE */

/* USER CODE BEGIN 3 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // 判断中断是否来自USART3
    if (huart->Instance == USART3)
    {
        // 此时，接收到的一个字节数据已经自动存入了Rx3_Temp变量中
        // 调用数据处理函数
        Deal_IR_Usart(Rx3_Temp);
        
        // 再次调用HAL_UART_Receive_IT()函数，以准备下一次的数据接收
        HAL_UART_Receive_IT(&huart3, &Rx3_Temp, 1);
    }
}
/* USER CODE END 4 */
```

