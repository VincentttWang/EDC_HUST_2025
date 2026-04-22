# SMotor 步进电机驱动 - TI MSPM0G3507移植版

## 概述
本驱动程序从STM32平台移植到TI MSPM0G3507平台，提供完整的步进电机控制功能，包括：
- PWM频率控制
- 方向控制
- 使能控制
- 位置和速度状态跟踪

## 硬件需求

### SysConfig配置要求
1. **PWM Timer配置**:
   - 配置一个GPTIMER用于PWM输出
   - 设置为PWM模式
   - 建议使用PWM_0_INST

2. **GPIO配置**:
   - 方向控制引脚：配置为数字输出
   - 使能控制引脚：配置为数字输出

### 引脚连接
```
MSPM0G3507     步进电机驱动器
PWM引脚    ->  PUL+ (脉冲信号)
方向引脚    ->  DIR+ (方向控制)  
使能引脚    ->  ENA+ (使能控制)
GND        ->  PUL-, DIR-, ENA-
```

## 函数说明

### 核心函数

#### `SMotor_Init()`
```c
void SMotor_Init(SMotor *motor, GPTIMER_Regs* pwm_timer, 
                 DL_TIMER_CC_INDEX pwm_channel, GPIO_Regs* dir_port, 
                 uint32_t dir_pin, GPIO_Regs* en_port, uint32_t en_pin);
```
初始化步进电机硬件接口。

**参数:**
- `motor`: 步进电机结构体指针
- `pwm_timer`: PWM定时器实例
- `pwm_channel`: PWM通道索引
- `dir_port`: 方向控制GPIO端口
- `dir_pin`: 方向控制引脚
- `en_port`: 使能控制GPIO端口  
- `en_pin`: 使能控制引脚

#### `SMotor_Parameters_Init()`
```c
void SMotor_Parameters_Init(SMotor *motor, float step_angular, 
                           uint8_t step_divisor, uint32_t max_step_frequency);
```
设置步进电机参数。

**参数:**
- `step_angular`: 步进角度 (度)，常见值：1.8°
- `step_divisor`: 步进细分，常见值：1, 2, 4, 8, 16
- `max_step_frequency`: 最大步进频率 (Hz)

#### `SMotor_SetSpeed()`
```c
void SMotor_SetSpeed(SMotor *motor, float angular_speed);
```
设置步进电机角速度。

**参数:**
- `angular_speed`: 目标角速度 (度/秒)
  - 正值：顺时针旋转
  - 负值：逆时针旋转  
  - 0：停止

#### `SMotor_UpdateState()`
```c
void SMotor_UpdateState(SMotor *motor);
```
更新步进电机状态，应在主循环中定期调用。

### 工具函数

#### `GetClockFre()`
```c
uint32_t GetClockFre(GPTIMER_Regs* timer);
```
获取定时器时钟频率，返回32MHz系统时钟。

#### `GetStepFrequency()`
```c
uint32_t GetStepFrequency(float angular_speed, SMotor *motor);
```
根据角速度计算对应的步进频率。

## 使用示例

### 基本使用
```c
#include "SMotor.h"

SMotor my_motor;

void setup_motor(void) {
    // 初始化电机
    SMotor_Init(&my_motor, PWM_0_INST, DL_TIMER_CC_0_INDEX,
                GPIOA, DL_GPIO_PIN_0, GPIOA, DL_GPIO_PIN_1);
    
    // 设置参数：1.8°步进角，1细分，200Hz最大频率
    SMotor_Parameters_Init(&my_motor, 1.8f, 1, 200);
    
    // 设置转速：90度/秒
    SMotor_SetSpeed(&my_motor, 90.0f);
}

void main_loop(void) {
    while(1) {
        SMotor_UpdateState(&my_motor);
        DL_Common_delayCycles(320000);  // 10ms延时
    }
}
```

### 位置控制示例
```c
void rotate_to_position(float target_angle) {
    float current_pos = my_motor.state.current_position;
    float error = target_angle - current_pos;
    
    if (fabs(error) > 1.0f) {  // 1度容差
        float speed = (error > 0) ? 90.0f : -90.0f;
        SMotor_SetSpeed(&my_motor, speed);
    } else {
        SMotor_SetSpeed(&my_motor, 0.0f);  // 到达目标位置，停止
    }
}
```

## 注意事项

1. **时钟配置**: 驱动假设系统时钟为32MHz，如有不同需修改`GetClockFre()`函数

2. **PWM频率范围**: 建议PWM频率范围100Hz-10kHz，过高可能导致步进电机失步

3. **错误处理**: 所有函数都包含NULL指针检查，需要`Initialize.h`中的错误处理函数

4. **状态更新**: 必须定期调用`SMotor_UpdateState()`来维护位置和时间状态

5. **细分设置**: 步进细分需要与硬件驱动器设置一致

## 移植说明

从STM32平台移植的主要更改：
- `HAL_GetTick()` -> `DL_Common_getTick()`
- `HAL_GPIO_WritePin()` -> `DL_GPIO_setPins()` / `DL_GPIO_clearPins()`
- `__HAL_TIM_SET_PRESCALER()` -> TI DriverLib定时器配置
- `__HAL_TIM_SET_AUTORELOAD()` -> TI DriverLib定时器配置
- `HAL_TIM_PWM_Start()` -> `DL_TimerG_startCounter()`
- `HAL_TIM_PWM_Stop()` -> `DL_TimerG_stopCounter()`

## 文件结构
```
BSP/SMotor/
├── SMotor.h           # 头文件定义
├── SMotor.c           # 驱动实现
├── SMotor_Example.c   # 使用示例
└── README.md          # 本说明文档
```
