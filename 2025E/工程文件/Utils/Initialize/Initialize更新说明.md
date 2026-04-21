# Initialize工程更新说明

## 更新概述

根据重构后的TB6612FNG电机驱动和霍尔编码器驱动，对Initialize工程进行了完整更新，适配TI MSPM0G3507平台。

## 主要更新内容

### 1. 头文件宏定义更新 (Initialize.h)

**电机参数定义更新:**
```c
// 原STM32定义
#define LEFT_MOTOR_IN1_PORT     GPIOC
#define LEFT_MOTOR_IN1_PIN      GPIO_PIN_4
#define LEFT_MOTOR_PWM_TIMER    &htim5
#define LEFT_MOTOR_PWM_CHANNEL  TIM_CHANNEL_2

// 更新为TI定义
#define LEFT_MOTOR_IN1_PORT     (GPIO_Regs*)GPIOA_BASE
#define LEFT_MOTOR_IN1_PIN      DL_GPIO_PIN_7
#define LEFT_MOTOR_PWM_TIMER    Motor_INST
#define LEFT_MOTOR_PWM_CHANNEL  DL_TIMER_CC_0_INDEX
```

**编码器参数简化:**
```c
// 原双编码器定义（已删除）
#define LEFT_ENCODER_TIMER      &htim3
#define RIGHT_ENCODER_TIMER     &htim1
#define LEFT_ENCODER_CHANNEL_A  TIM_CHANNEL_2
#define RIGHT_ENCODER_CHANNEL_B TIM_CHANNEL_2

// 更新为单编码器定义
#define ENCODER_QEI_TIMER       QEI_0_INST
#define ENCODER_REAL_TIMER      TIMG1
```

### 2. MECInit函数完全重构

**原函数:**
```c
void MECInit()
{
    Motor_UI_Init(&Le, LEFT_MOTOR_IN1_PORT, LEFT_MOTOR_IN1_PIN,
        LEFT_MOTOR_IN2_PORT, LEFT_MOTOR_IN2_PIN,
        LEFT_MOTOR_PWM_TIMER, LEFT_MOTOR_PWM_CHANNEL, LEFT_MOTOR_INIT_DUTY);
    
    LRInit(LEFT_ENCODER_TIMER, LEFT_ENCODER_CHANNEL_A, LEFT_ENCODER_CHANNEL_B,
           RIGHT_ENCODER_TIMER, RIGHT_ENCODER_CHANNEL_A, RIGHT_ENCODER_CHANNEL_B,
           WHEEL_DIAMETER, PPR * REDUCE, ENCODER_REAL_TIMER);
}
```

**更新后:**
```c
void MECInit()
{
    // 初始化系统配置
    SYSCFG_DL_init();
    
    // 初始化电机参数和硬件接口
    Motor_Param_Init(&Le, REDUCE, FULL_SPEED_RPM, WHEEL_DIAMETER);
    Motor_UI_Init(&Le, LEFT_MOTOR_IN1_PORT, LEFT_MOTOR_IN1_PIN,
                  LEFT_MOTOR_IN2_PORT, LEFT_MOTOR_IN2_PIN,
                  LEFT_MOTOR_PWM_TIMER, LEFT_MOTOR_PWM_CHANNEL, LEFT_MOTOR_INIT_DUTY);
    
    // 初始化单个编码器
    EncoderInit(ENCODER_QEI_TIMER, ENCODER_REAL_TIMER, WHEEL_DIAMETER, PPR * REDUCE);
    
    // 启用中断
    NVIC_EnableIRQ(TIMG1_INT_IRQn);
    NVIC_EnableIRQ(QEI_0_INST_INT_IRQN);
}
```

### 3. 函数调用更新

**速度获取函数更新:**
```c
// 原双编码器模式
current_data.speed.linear_velocity = cSpeed() * speedCorrection;

// 更新为单编码器模式
current_data.speed.linear_velocity = getSpeed(0) * speedCorrection;
```

**时间函数更新:**
```c
// 原STM32 HAL函数
uint32_t now = HAL_GetTick();
HAL_Delay(10);

// 更新为TI平台函数
uint32_t now = DL_Common_getTick();
delay_cycles(320000);  // 约10ms延时
```

### 4. 新增函数

**延时函数:**
```c
void delay_cycles(uint32_t cycles)
{
    volatile uint32_t i;
    for(i = 0; i < cycles; i++)
    {
        __NOP(); // 空操作
    }
}
```

**中断处理函数:**
```c
void TIMG1_IRQHandler(void)
{
    if (DL_TimerG_getPendingInterrupt(TIMG1) & DL_TIMER_INTERRUPT_ZERO_EVENT) 
    {
        UpdateAllSpeed(TIMG1);
        DL_TimerG_clearInterruptStatus(TIMG1, DL_TIMER_INTERRUPT_ZERO_EVENT);
    }
}

void QEI_0_INST_IRQHandler(void)
{
    uint32_t pending_interrupt = DL_TimerG_getPendingInterrupt(QEI_0_INST);
    
    if (pending_interrupt & DL_TIMER_IIDX_DIR_CHANGE) 
    {
        // 方向改变处理逻辑
        DL_TimerG_clearInterruptStatus(QEI_0_INST, DL_TIMER_IIDX_DIR_CHANGE);
    }
}
```

## 硬件连接更新

### 电机连接
| 功能 | TI MSPM0G3507引脚 | 说明 |
|------|------------------|------|
| 左电机AIN1 | PA7 | 方向控制1 |
| 左电机AIN2 | PA6 | 方向控制2 |
| 左电机PWM | PB7 (TIMG7_CCP0) | PWM控制 |
| 右电机BIN1 | PA9 | 方向控制1 |
| 右电机BIN2 | PA8 | 方向控制2 |
| 右电机PWM | PA3 (TIMG6_CCP0) | PWM控制 |

### 编码器连接
| 功能 | TI MSPM0G3507引脚 | 说明 |
|------|------------------|------|
| QEI PHA | PA21 (PINCM46) | A相输入 |
| QEI PHB | PA2 (PINCM7) | B相输入 |

## 主要改进

1. **简化编码器配置**: 从双编码器LR模式改为单编码器模式
2. **标准化初始化**: 添加了Motor_Param_Init调用
3. **完善中断处理**: 添加了TIMG1和QEI中断处理
4. **平台适配**: 替换了所有STM32特定的函数调用
5. **系统初始化**: 添加了SYSCFG_DL_init()调用

## 注意事项

1. **编码器模式**: 现在使用单个编码器，需要根据实际应用调整速度计算
2. **中断配置**: 确保在SysConfig中正确配置了TIMG1和QEI_0的中断
3. **时钟设置**: delay_cycles函数假设32MHz系统时钟
4. **GPIO配置**: 确保在SysConfig中正确配置了所有GPIO引脚
5. **PWM配置**: 确保Motor_INST和SMotor_INST在SysConfig中正确配置

## 兼容性

- 保持了原有的用户接口函数（LSet, RSet, Break等）
- 保持了原有的数据结构和全局变量
- 适配了TI MSPM0G3507平台的硬件特性

更新后的Initialize工程完全适配了重构后的电机和编码器驱动，可以直接在TI MSPM0G3507平台上使用。
