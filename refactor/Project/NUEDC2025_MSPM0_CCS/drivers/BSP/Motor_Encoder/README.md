# TI MSPM0G3507 霍尔编码器驱动

## 概述

这是一个专为TI MSPM0G3507微控制器重构的霍尔编码器驱动程序。原来基于STM32 HAL库的代码已经完全重构，使用TI DriverLib库实现。

## 主要改动

### 1. 头文件更改
- 替换 `#include "main.h"`, `#include "tim.h"` 为 `#include "ti_msp_dl_config.h"`
- 将 `TIM_HandleTypeDef` 替换为 `GPTIMER_Regs`

### 2. 硬件抽象层更改
- STM32 HAL函数替换为TI DriverLib函数：
  - `HAL_TIM_PWM_Start()` → `DL_TimerG_setQEIConfig()` + `DL_TimerG_enableQEI()`
  - `HAL_TIM_Base_Start_IT()` → `DL_TimerG_enableInterrupt()` + `DL_TimerG_startCounter()`
  - `timer->Instance->CNT` → `DL_TimerG_getCounterValue()`
  - `HAL_RCC_GetPCLK1Freq()` → `CPUCLK_FREQ`

### 3. QEI (正交编码器接口) 配置
- 使用TIMG8作为QEI_0
- 支持正交编码器模式
- 自动方向检测

## 硬件连接

### QEI_0 (TIMG8)
- **PHA (A相)**: PA21 (PINCM46)
- **PHB (B相)**: PA2 (PINCM7)

### 时间基准定时器
- 使用TIMG1作为速度采样定时器
- 中断频率建议：100Hz (10ms)

## 使用方法

### 1. 初始化单个编码器
```c
#include "Hall_Encoder.h"

// 初始化编码器
// 参数：编码器定时器, 采样定时器, 轮径(mm), 脉冲数/转
EncoderInit(QEI_0_INST, TIMG1, 66, 360);
```

### 2. 初始化左右编码器
```c
// 初始化左右编码器
LRInit(QEI_0_INST, QEI_1_INST, 66, 360, TIMG1);
```

### 3. 定时器中断处理
```c
void TIMG1_IRQHandler(void)
{
    if (DL_TimerG_getPendingInterrupt(TIMG1) & DL_TIMER_INTERRUPT_ZERO_EVENT) 
    {
        UpdateAllSpeed(TIMG1);
        DL_TimerG_clearInterruptStatus(TIMG1, DL_TIMER_INTERRUPT_ZERO_EVENT);
    }
}
```

### 4. 获取速度和距离
```c
// 获取编码器速度 (m/s)
double speed = getSpeed(0);

// 获取累计距离 (m)  
double distance = getDis(0);

// 左右轮专用函数 (需要先调用LRInit)
double left_speed = lSpeed();
double right_speed = rSpeed();
double center_speed = cSpeed();
```

## SysConfig配置

### QEI配置
1. 添加TIMG8实例
2. 配置为QEI模式
3. 设置PHA和PHB引脚

### 定时器配置  
1. 添加TIMG1实例
2. 配置为定时器模式
3. 设置周期为10ms (100Hz)
4. 启用中断

## 注意事项

1. **时钟频率**: 代码假设系统时钟为32MHz (`CPUCLK_FREQ`)
2. **编码器分辨率**: 根据实际编码器调整PPR参数
3. **轮径设置**: 以毫米为单位设置轮径
4. **中断优先级**: 确保定时器中断优先级设置正确
5. **引脚配置**: 在SysConfig中正确配置QEI引脚

## 支持的编码器数量
- 最大支持10个编码器 (`MAXENCODERNUM`)
- 可根据需要修改此限制

## 文件结构
```
BSP/Motor_Encoder/
├── Hall_Encoder.h              // 头文件
├── Hall_Encoder.c              // 主实现文件  
├── Hall_Encoder_Example.c      // 使用示例
└── README.md                   // 说明文档
```

## 兼容性
- 支持TI MSPM0G3507微控制器
- 基于TI DriverLib库
- 兼容Code Composer Studio和其他TI开发环境
