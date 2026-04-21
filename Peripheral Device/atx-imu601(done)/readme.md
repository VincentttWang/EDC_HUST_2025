## 注意：

- 每次使用最好先进行校准，校准需要用上位机（ATX-IMU1.0.4)

## 硬件连接：

- USART2连接陀螺仪
- USART1连接串口助手

## 工程配置：

- USART2需要打开global interrupt，波特率115200
- TIM6设置为所需函数delay_us_hal的使能时钟（1us），prescaler为240-1
- Driver库中导入BSP文件夹，内含库文件

## 代码：
- 函数用法
```c
    atk_ms601m_get_attitude(&attitude_dat, 100);                            /* 姿态角 */
    atk_ms601m_get_gyro_accelerometer(&gyro_dat, &accelerometer_dat, 100);  /* 角速度和加速度 */
```
		这两个函数并不返回值，实际上传入的结构体的值进行更新

-**这两个函数的运行时长一般在10ms左右，高频次中断调用容易发生阻塞**

- 结构体解析
- `atk_ms601m_attitude_data_t` 结构体用于存储姿态角数据，包括 `roll`（横滚角）、`pitch`（俯仰角）、`yaw`（航向角），单位为度。
- `atk_ms601m_gyro_data_t` 结构体用于存储角速度数据，包含 `x`、`y`、`z` 三个分量，单位为度每秒（°/s），需要注意的是角速度的正负方向使用右手定则，即从该坐标轴的正方向向负方向看去，逆时针为正方向。
- `atk_ms601m_accelerometer_data_t` 结构体用于存储加速度数据，包含 `x`、`y`、`z` 三个分量，单位为 g（重力加速度）。

- main.c

```c
/* USER CODE BEGIN PV */
atk_ms601m_attitude_data_t attitude_dat;
atk_ms601m_gyro_data_t gyro_dat;
atk_ms601m_accelerometer_data_t accelerometer_dat;
/* USER CODE END PV */


  /* USER CODE BEGIN 2 */
	atk_ms601m_init(115200);
  /* USER CODE END 2 */

  /* USER CODE BEGIN WHILE */
  while (1)
  {
	/* 获取陀螺仪数据 */
    atk_ms601m_get_attitude(&attitude_dat, 100);                            /* 姿态角 */
    atk_ms601m_get_gyro_accelerometer(&gyro_dat, &accelerometer_dat, 100);  /* 角速度和加速度 */
		
		
	//后可引用姿态角数据attitude_dat.roll, attitude_dat.pitch, attitude_dat.yaw
	//后可引用角速度数据gyro_dat.x, gyro_dat.y, gyro_dat.z
	//后可引用加速度数据accelerometer_dat.x, accelerometer_dat.y, accelerometer_dat.z)

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
void delay_us_hal(uint16_t nus)
{
    __HAL_TIM_SET_COUNTER(&htim6, 0);
    HAL_TIM_Base_Start(&htim6);       
    while (__HAL_TIM_GET_COUNTER(&htim6) < nus); 
    HAL_TIM_Base_Stop(&htim6);        
}
/* USER CODE END 4 */
```

- main.h

  ```c
  /* USER CODE BEGIN EFP */
  void delay_us_hal(uint16_t nus);
  /* USER CODE END EFP */
  ```

  

- **需要将在atk_ms601m_uart.h有关#define ATK_MS601M_UART_IRQHandler              USART2_IRQHandler的宏定义注释或删除，并把这个函数给声明，然后再it文件中写入#include "AllHeader.h" 并在USART2_IRQHandler上中写入ATK_MS601M_UART_IRQHandler**

- **需要在atk_ms601m_uart.h中将引脚定义替换成所用串口引脚**

- **init前增加适当延时可以提高稳定性**
