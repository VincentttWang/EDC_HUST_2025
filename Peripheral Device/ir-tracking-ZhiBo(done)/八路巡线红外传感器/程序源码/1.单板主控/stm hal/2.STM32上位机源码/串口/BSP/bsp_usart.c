#include "bsp_usart.h"
#include "string.h" // 可能需要这个头文件

// 声明UART句柄
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

// HAL库支持printf函数的方法。通常不再需要__use_no_semihosting
// 而是通过重定向fputc到HAL库的发送函数
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF); // 通过UART1发送
  return ch;
}


/**************************************************************************
Function: Serial port 1 initialization
Input   : bound：Baud rate
Output  : none
函数功能：串口1初始化
入口参数：bound：波特率
返回  值：无
**************************************************************************/
void uart_init(u32 bound)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = bound;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16; // STM32F1 HAL库通常设置为16

  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    // 初始化错误处理，例如进入死循环或报错
    while(1);
  }

  // 启用接收中断 (HAL库会通过回调函数处理)
  // HAL_UART_Receive_IT(&huart1, &Rx1_Temp, 1); // 如果你需要每个字节都触发中断，可以在这里启动
}

// HAL库的底层初始化函数，会在HAL_UART_Init()中被调用
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(huart->Instance == USART1)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE(); // 启用GPIOA时钟

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL; // 浮空输入
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
  else if(huart->Instance == USART2)
  {
    /* Peripheral clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE(); // 启用GPIOA时钟

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL; // 浮空输入
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  }
}

// 发送一个字节
void USART1_Send_U8(uint8_t ch)
{
  HAL_UART_Transmit(&huart1, &ch, 1, 0xFFFF); // 0xFFFF是超时时间
}

// 发送字节数组
void USART1_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length)
{
  HAL_UART_Transmit(&huart1, BufferPtr, Length, 0xFFFF);
}

// 串口1中断服务函数 (HAL库会自动调用回调函数)
// 你需要在stm32f1xx_it.c中找到USART1_IRQHandler，并确保它调用了HAL_UART_IRQHandler(&huart1);
// 通常在CubeMX生成的代码中，这已经为你做好了。
// 接收到数据后的处理，通过HAL库的回调函数实现
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1)
  {
    uint8_t Rx1_Temp = 0; // 假设你在HAL_UART_Receive_IT中接收到一个字节
    // 从UART接收数据（这通常在启动中断接收时就已经完成）
    // HAL_UART_Receive_IT(&huart1, &Rx1_Temp, 1); // 重新启动接收
    // 这里需要根据你实际的接收方式来处理，如果是中断接收，你应该已经在中断回调中处理了。
    // 为了简单起见，这里假设你需要将接收到的字节立即发送回去
    USART1_Send_U8(Rx1_Temp); // 回显
  }
  else if(huart->Instance == USART2)
  {
    uint8_t Rx2_Temp = 0; // 假设你在HAL_UART_Receive_IT中接收到一个字节
    // 从UART接收数据
    // HAL_UART_Receive_IT(&huart2, &Rx2_Temp, 1); // 重新启动接收
    Deal_IR_Usart(Rx2_Temp); // 处理数据
    // USART2_Send_U8(Rx2_Temp); // 只收发测试
  }
}


// USART2初始化
void USART2_init(u32 baudrate)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = baudrate;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    // 初始化错误处理
    while(1);
  }
  // 启动USART2接收中断，通常在初始化后立即启动
  // 这里假设每次只接收一个字节，并在回调中重新启动接收
  // 在main函数或其他地方调用 HAL_UART_Receive_IT(&huart2, &Rx2_Temp_Buffer, 1);
  // 注意：Rx2_Temp_Buffer需要是全局变量或者静态变量，或者通过其他方式传递
}


// 发送一个字符
void USART2_Send_U8(uint8_t ch)
{
  HAL_UART_Transmit(&huart2, &ch, 1, 0xFFFF);
}

// 发送一个字符串
void USART2_Send_ArrayU8(uint8_t *BufferPtr, uint16_t Length)
{
  HAL_UART_Transmit(&huart2, BufferPtr, Length, 0xFFFF);
}

// USART2中断服务函数
// 同样，你需要在stm32f1xx_it.c中找到USART2_IRQHandler，并确保它调用了HAL_UART_IRQHandler(&huart2);
// 具体的接收数据处理在HAL_UART_RxCpltCallback中完成。