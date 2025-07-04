/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "AllHeader.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//?????????????
atk_ms601m_attitude_data_t attitude_dat;
atk_ms601m_gyro_data_t gyro_dat;
atk_ms601m_accelerometer_data_t accelerometer_dat;
char message[256]; 

//??·??????????
extern uint8_t g_new_package_flag;
uint8_t Rx_Temp; // ???????USART????????????

//TB6612???????????
Motor Left, Right;  // ???????????
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Initialize(void);
static void MPU_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM7_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_TIM6_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
	//??????????
	atk_ms601m_init(115200);
	
	//????????????????·?????????????
	HAL_Delay(1000);
	HAL_UART_Receive_IT(&huart6, &Rx_Temp, 1);
	HAL_UART_Transmit(&huart6, (uint8_t *)"$0,0,1#", 7, HAL_MAX_DELAY);
	HAL_UART_Receive_IT(&huart6, &Rx_Temp, 1);
	
  Motor Left, Right;  
	MEInit(&Left, &Right); // 电机和编码器初始化
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		//传感器数据读取
		atk_ms601m_get_attitude(&attitude_dat, 100);
		atk_ms601m_get_gyro_accelerometer(&gyro_dat, &accelerometer_dat, 100);
		
		//八路巡线传感器数据读取
		if(g_new_package_flag == 1)
		{
			g_new_package_flag = 0;
			Deal_Usart_Data();
		}
		
		//电机和编码器测试代码
		static uint32_t test_timer = 0;
		static uint8_t test_phase = 0;
		
  
    switch(test_phase)
    {
      case 0: // 前进测试
        sprintf(message, "Phase 0: Forward - L=%.3f, R=%.3f m/s\r\n", lSpeed(), rSpeed());
        HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);
        
        Motor_UI_Set(FOR, 300, &Left);   // 左轮前进
        Motor_UI_Set(FOR, 300, &Right);  // 右轮前进
        break;
        
      case 1: // 左转测试
        sprintf(message, "Phase 1: Turn Left - L=%.3f, R=%.3f m/s\r\n", lSpeed(), rSpeed());
        HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);
        
        Motor_UI_Set(SLIDE, 0, &Left);    // 左轮停止
        Motor_UI_Set(FOR, 400, &Right);   // 右轮前进
        break;
        
      case 2: // 右转测试
        sprintf(message, "Phase 2: Turn Right - L=%.3f, R=%.3f m/s\r\n", lSpeed(), rSpeed());
        HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);
        
        Motor_UI_Set(FOR, 400, &Left);    // 左轮前进
        Motor_UI_Set(SLIDE, 0, &Right);   // 右轮停止
        break;
        
      case 3: // 后退测试
        sprintf(message, "Phase 3: Backward - L=%.3f, R=%.3f m/s\r\n", lSpeed(), rSpeed());
        HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);
        
        Motor_UI_Set(BACK, 300, &Left);   // 左轮后退
        Motor_UI_Set(BACK, 300, &Right);  // 右轮后退
        break;
        
      case 4: // 停止测试
        sprintf(message, "Phase 4: Stop - L=%.3f, R=%.3f m/s\r\n", lSpeed(), rSpeed());
        HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 1000);
        
        Motor_UI_Set(BREAK, 0, &Left);    // 左轮制动
        Motor_UI_Set(BREAK, 0, &Right);   // 右轮制动
        break;
        
      default: // 重启测试循环
        test_phase = -1; // 下次会变成0
        break;
    }
    test_phase++;
  HAL_Delay(1000); // 每个测试阶段间隔1秒

    
  
  // 实时速度显示（每1秒显示一次）
  sprintf(message, "Speed: L=%.3f, R=%.3f, C=%.3f m/s\r\n", 
          lSpeed(), rSpeed(), cSpeed());
  HAL_UART_Transmit(&huart3, (uint8_t*)message, strlen(message), 1000);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//??????????1us?????
void delay_us_hal(uint16_t nus)
{
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    HAL_TIM_Base_Start(&htim6);       
    while (__HAL_TIM_GET_COUNTER(&htim6) < nus); 
    HAL_TIM_Base_Stop(&htim6);        
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // 判断是否是USART6的中断
    if (huart->Instance == USART6)
    {
        // 处理接收到的数据
        Deal_IR_Usart(Rx_Temp);
        
        // 重新启动UART接收中断
        HAL_UART_Receive_IT(&huart6, &Rx_Temp, 1);
    }
}

// 定时器中断回调函数，用于更新编码器速度
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    // 更新所有编码器的速度数据
    UpdateAllSpeed(htim);
}
/* USER CODE END 4 */

/* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x24000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x30000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_128KB;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x30020000;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER3;
  MPU_InitStruct.BaseAddress = 0x30040000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_32KB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Number = MPU_REGION_NUMBER4;
  MPU_InitStruct.BaseAddress = 0x38000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_64KB;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
