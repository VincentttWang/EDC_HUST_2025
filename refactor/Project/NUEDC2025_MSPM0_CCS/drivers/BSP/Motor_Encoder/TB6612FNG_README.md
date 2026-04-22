# TI MSPM0G3507 TB6612FNG电机驱动

## 概述

这是一个专为TI MSPM0G3507微控制器重构的TB6612FNG双电机驱动程序。原来基于STM32 HAL库的代码已经完全重构，使用TI DriverLib库实现。

## 主要改动

### 1. 数据结构更改

**原代码:**
```c
typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
} GPIO_Pin;

typedef struct {
    TIM_HandleTypeDef* pwm_timer;
    uint32_t pwm_channel;
    uint16_t current_duty;
} PWM;
```

**移植后:**
```c
typedef struct {
    GPIO_Regs* port;        // GPIO端口 (GPIOA, GPIOB等)
    uint32_t pin;           // GPIO引脚掩码 (DL_GPIO_PIN_x)
} GPIO_Pin;

typedef struct {
    GPTIMER_Regs* pwm_timer;     // 定时器实例
    DL_TIMER_CC_INDEX pwm_channel; // PWM通道索引             
    uint16_t current_duty;       // 0-1000占空比
} PWM;
```

### 2. HAL函数到DriverLib函数映射

| STM32 HAL函数 | TI DriverLib函数 | 说明 |
|---------------|------------------|------|
| `HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET)` | `DL_GPIO_setPins(port, pin)` | 设置GPIO高电平 |
| `HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET)` | `DL_GPIO_clearPins(port, pin)` | 设置GPIO低电平 |
| `HAL_TIM_PWM_Start(timer, channel)` | `DL_TimerG_startCounter(timer)` | 启动PWM |
| `__HAL_TIM_SET_COMPARE(timer, channel, value)` | `DL_TimerG_setCaptureCompareValue(timer, value, channel)` | 设置PWM占空比 |

### 3. 函数接口更改

**原代码:**
```c
void Motor_UI_Init(Motor* M, GPIO_TypeDef* p_port, uint16_t p_pin,
                   GPIO_TypeDef* n_port, uint16_t n_pin,
                   TIM_HandleTypeDef* pwm_timer, uint32_t pwm_channel,
                   uint16_t current_duty);
```

**移植后:**
```c
void Motor_UI_Init(Motor* M, GPIO_Regs* p_port, uint32_t p_pin,
                   GPIO_Regs* n_port, uint32_t n_pin,
                   GPTIMER_Regs* pwm_timer, DL_TIMER_CC_INDEX pwm_channel,
                   uint16_t current_duty);
```

## 硬件连接

### TB6612FNG模块连接
- **VCC**: 3.3V (逻辑电源)
- **GND**: GND
- **VM**: 6-12V (电机电源)
- **STBY**: 3.3V (使能，接高电平)

### 电机A控制
- **AIN1**: PA7 (方向控制1)
- **AIN2**: PA6 (方向控制2)  
- **PWMA**: PB7 (PWM控制，连接TIMG7_CCP0)

### 电机B控制  
- **BIN1**: PA9 (方向控制1)
- **BIN2**: PA8 (方向控制2)
- **PWMB**: PA3 (PWM控制，连接TIMG6_CCP0)

### 电机输出
- **Motor A+/A-**: 电机A正负极
- **Motor B+/B-**: 电机B正负极

## 使用方法

### 1. 初始化电机参数
```c
Motor motor_left;

// 初始化电机参数：减速比30:1，满速3000RPM，轮径66mm
Motor_Param_Init(&motor_left, 30.0, 3000.0, 66);
```

### 2. 初始化硬件接口
```c
// 初始化电机硬件接口
Motor_UI_Init(&motor_left, 
              (GPIO_Regs*)GPIOA_BASE, DL_GPIO_PIN_7,  // AIN1
              (GPIO_Regs*)GPIOA_BASE, DL_GPIO_PIN_6,  // AIN2  
              Motor_INST, DL_TIMER_CC_0_INDEX,        // PWM
              0);                                     // 初始占空比
```

### 3. 控制电机

#### 方法1：直接设置占空比
```c
Motor_UI_Set(FOR, 500, &motor_left);    // 正转，50%占空比
Motor_UI_Set(BACK, 300, &motor_left);   // 反转，30%占空比  
Motor_UI_Set(BREAK, 0, &motor_left);    // 刹车停止
Motor_UI_Set(SLIDE, 0, &motor_left);    // 滑行停止
```

#### 方法2：按速度控制
```c
MotorSet(FOR, 0.5, &motor_left);        // 正转，0.5m/s
MotorSet(BACK, 0.3, &motor_left);       // 反转，0.3m/s
MotorSet(SLIDE, 0, &motor_left);        // 停止
```

## 电机控制模式

| 模式 | AIN1 | AIN2 | 说明 |
|------|------|------|------|
| `FOR` | 高 | 低 | 正转 |
| `BACK` | 低 | 高 | 反转 |
| `BREAK` | 高 | 高 | 刹车(快速停止) |
| `SLIDE` | 低 | 低 | 滑行(自然停止) |
| `SLEEP` | - | - | 睡眠模式 |

## SysConfig配置

### GPIO配置
```
PA6: 输出引脚 (AIN2)
PA7: 输出引脚 (AIN1)  
PA8: 输出引脚 (BIN2)
PA9: 输出引脚 (BIN1)
```

### PWM配置
```
Motor (TIMG7):
- 模式: PWM
- 频率: 1kHz
- 占空比: 0-100%
- 输出引脚: PB7 (CCP0)

SMotor (TIMG6):  
- 模式: PWM
- 频率: 1kHz
- 占空比: 0-100%
- 输出引脚: PA3 (CCP0)
```

### PWM计算
- 系统时钟: 32MHz
- PWM频率: 1kHz
- 预分频: 31 (32分频)
- 计数周期: 1000 (对应占空比0-1000)

## 速度计算公式

```c
// 电机转速转换为轮子线速度
wheel_rps = motor_rpm / reduce_ratio / 60.0
wheel_circumference = wheel_diameter * π / 1000.0  // mm转m
linear_speed = wheel_rps * wheel_circumference     // m/s

// 速度转占空比  
duty = (speed / max_speed) * 1000
```

## 注意事项

1. **电源要求**: VM电源电压6-12V，电流根据电机功率选择
2. **散热**: 大功率电机需要考虑TB6612FNG散热
3. **PWM频率**: 建议1kHz，过高可能增加开关损耗
4. **占空比范围**: 0-1000，对应0-100%占空比
5. **方向切换**: 方向切换前建议先停止电机
6. **参数设置**: 根据实际电机参数设置减速比、满速RPM、轮径

## 文件结构
```
BSP/Motor_Encoder/
├── tb6612fng.h                 // 头文件
├── tb6612fng.c                 // 主实现文件  
├── TB6612FNG_Example.c         // 使用示例
└── TB6612FNG_README.md         // 说明文档
```

## 兼容性
- 支持TI MSPM0G3507微控制器
- 基于TI DriverLib库
- 兼容Code Composer Studio和其他TI开发环境
- 支持双电机独立控制
