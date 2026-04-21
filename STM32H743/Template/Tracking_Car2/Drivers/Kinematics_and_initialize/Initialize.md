# Initialize.c 函数库使用说明

## 概述

Initialize.c 是一个电机和编码器初始化封装库，基于 TB6612FNG 电机驱动芯片和霍尔编码器。该库提供了简化的初始化和控制接口，使得电机控制更加便捷。

## 硬件配置

### 电机连接 (TB6612FNG)
```
电机A (左轮):
- AIN1: PC4 (方向控制1)
- AIN2: PB0 (方向控制2)  
- PWMA: PA1 (TIM5_CH2, PWM输出)

电机B (右轮):
- BIN1: PC5 (方向控制1)
- BIN2: PB1 (方向控制2)
- PWMB: PA0 (TIM5_CH1, PWM输出)
```

### 编码器连接
```
左轮编码器 (TIM3):
- E2A: PA6 (TIM3_CH1)
- E2B: PA7 (TIM3_CH2)

右轮编码器 (TIM1):  
- E1A: PE11 (TIM1_CH1)
- E1B: PE9 (TIM1_CH2)

实时定时器: TIM7 (用于速度计算)
```

## 函数接口

### 1. MEInit() - 主初始化函数

```c
void MEInit(Motor* L, Motor* R)
```

**功能**: 一键初始化左右电机和编码器系统

**参数**:
- `L`: 左轮电机结构体指针
- `R`: 右轮电机结构体指针

**功能说明**:
- 自动初始化左右电机的GPIO和PWM配置
- 自动初始化左右编码器的定时器配置
- 设置轮径为65mm，编码器总分辨率为364(13×28)

**使用示例**:
```c
Motor Left, Right;
MEInit(&Left, &Right);

// 设置电机参数
Motor_Param_Init(&Left, 28, 300, 65);   // 减速比28, 最大转速300rpm, 轮径65mm
Motor_Param_Init(&Right, 28, 300, 65);
```

### 2. LMotorSet() - 左轮电机控制

```c
void LMotorSet(MOVETYPE type, uint16_t duty)
```

**功能**: 控制左轮电机运动

**参数**:
- `type`: 运动类型
  - `FOR`: 前进
  - `BACK`: 后退
  - `BREAK`: 制动停止
  - `SLIDE`: 滑行停止
- `duty`: PWM占空比 (0-1000)

**使用示例**:
```c
LMotorSet(FOR, 300);    // 左轮前进，速度300
LMotorSet(BACK, 400);   // 左轮后退，速度400
LMotorSet(BREAK, 0);    // 左轮制动停止
```

### 3. RMotorSet() - 右轮电机控制

```c
void RMotorSet(MOVETYPE type, uint16_t duty)
```

**功能**: 控制右轮电机运动

**参数**: 
- `type`: 运动类型 (同LMotorSet)
- `duty`: PWM占空比 (0-1000)

**使用示例**:
```c
RMotorSet(FOR, 300);    // 右轮前进，速度300
RMotorSet(BACK, 400);   // 右轮后退，速度400
RMotorSet(BREAK, 0);    // 右轮制动停止
```

## 运动控制示例

### 基本运动模式

```c
// 1. 直线前进
LMotorSet(FOR, 300);
RMotorSet(FOR, 300);

// 2. 直线后退
LMotorSet(BACK, 300);
RMotorSet(BACK, 300);

// 3. 左转 (右轮转，左轮停)
LMotorSet(SLIDE, 0);
RMotorSet(FOR, 400);

// 4. 右转 (左轮转，右轮停)
LMotorSet(FOR, 400);
RMotorSet(SLIDE, 0);

// 5. 原地左转 (差速转向)
LMotorSet(BACK, 200);
RMotorSet(FOR, 200);

// 6. 原地右转 (差速转向)
LMotorSet(FOR, 200);
RMotorSet(BACK, 200);

// 7. 停止
LMotorSet(BREAK, 0);
RMotorSet(BREAK, 0);
```

### 配合编码器的速度控制

```c
// 获取编码器速度 (需要在定时器中断中调用UpdateAllSpeed)
double left_speed = lSpeed();   // 左轮速度 (m/s)
double right_speed = rSpeed();  // 右轮速度 (m/s)
double center_speed = cSpeed(); // 中心速度 (m/s)

// 根据速度反馈调整电机
if(left_speed < 0.2) {
    LMotorSet(FOR, 350);  // 增加左轮速度
}
if(right_speed < 0.2) {
    RMotorSet(FOR, 350);  // 增加右轮速度
}
```

## 参数配置

### 预定义参数
```c
#define PPR 13              // 编码器每转脉冲数
#define REDUCE 28           // 电机减速比
#define FULL_SPEED_RPM 300  // 电机最大转速
#define WHEEL_DIAMETER 65   // 轮径 (mm)
```

### 速度换算
```c
// 编码器总分辨率 = PPR × REDUCE = 13 × 28 = 364 脉冲/转
// 轮周长 = π × WHEEL_DIAMETER = π × 65 ≈ 204.2 mm
// 速度(m/s) = (脉冲频率 / 364) × 0.2042
```

## 注意事项

1. **初始化顺序**: 必须先调用MEInit()，再调用Motor_Param_Init()
2. **定时器配置**: TIM5必须配置为PWM输出，TIM1/TIM3配置为编码器模式，TIM7配置为定时器
3. **编码器更新**: 需要在TIM7中断中调用`UpdateAllSpeed(&htim7)`来更新速度数据
4. **速度范围**: PWM duty范围0-1000，对应0-100%占空比
5. **方向定义**: 根据实际接线可能需要调整前进/后退方向

## 完整使用示例

```c
#include "Initialize.h"

Motor Left, Right;

int main(void)
{
    // HAL库初始化...
    
    // 电机和编码器初始化
    MEInit(&Left, &Right);
    Motor_Param_Init(&Left, 28, 300, 65);
    Motor_Param_Init(&Right, 28, 300, 65);
    
    while(1)
    {
        // 前进2秒
        LMotorSet(FOR, 300);
        RMotorSet(FOR, 300);
        HAL_Delay(2000);
        
        // 左转1秒
        LMotorSet(SLIDE, 0);
        RMotorSet(FOR, 400);
        HAL_Delay(1000);
        
        // 停止1秒
        LMotorSet(BREAK, 0);
        RMotorSet(BREAK, 0);
        HAL_Delay(1000);
        
        // 输出速度信息
        printf("Left: %.3f m/s, Right: %.3f m/s\n", lSpeed(), rSpeed());
    }
}

// 定时器中断回调 (TIM7)
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim == &htim7) {
        UpdateAllSpeed(&htim7);  // 更新编码器速度
    }
}
```

这个库大大简化了TB6612电机驱动的使用，提供了清晰的接口来控制双轮差速小车的运