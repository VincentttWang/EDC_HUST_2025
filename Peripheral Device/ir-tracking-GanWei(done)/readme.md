## 注意：

更换位置或路线时，需校准，校准方式见使用文档

## 工程配置：

- 硬件I2C：打开I2C1设置为I2C模式
- 引入GanWei库文件

## 接线：

PB6     ------> I2C1_SCL
PB7     ------> I2C1_SDA

八路传感器跳帽：scl sda加跳帽；ad1，ad0，pull不加跳帽

## 代码：

```c
/* USER CODE BEGIN PV */
int Digtal[8];
/* USER CODE END PV */

/* USER CODE BEGIN WHILE */
while (1)
{
    IIC_Get_Digtal(Digtal);		//获取传感器数字量结果,存储在Digtal[7]数组中 
    HAL_Delay(1000);
/* USER CODE END WHILE */
```

