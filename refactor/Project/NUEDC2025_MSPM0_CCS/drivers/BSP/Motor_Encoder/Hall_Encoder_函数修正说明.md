# 霍尔编码器驱动函数修正说明

## 修正内容

根据用户反馈，霍尔编码器驱动中的TI DriverLib函数调用存在错误，现已修正为正确的函数。

## 主要修正

### 1. QEI计数值读取函数

**错误的函数:**
```c
DL_TimerG_getCounterValue(Timer)
```

**正确的函数:**
```c
DL_Timer_getTimerCount(Timer)
```

### 2. QEI初始化方式

**错误的初始化:**
```c
// 这些函数不存在或不适用于QEI
DL_TimerG_setQEIConfig(Timer, ...);
DL_TimerG_enableQEI(Timer);
DL_TimerG_setCounterValue(Timer, 0);
```

**正确的初始化:**
```c
// QEI模式在SysConfig中配置，代码中只需启动计数器
DL_TimerG_startCounter(Timer);
// 计数器重置功能可能不可用，因此注释掉或删除
```

## 正确的TI DriverLib QEI函数

### 核心函数

1. **启动QEI计数器:**
   ```c
   DL_TimerG_startCounter(QEI_0_INST);
   ```

2. **获取QEI计数值:**
   ```c
   uint32_t count = DL_Timer_getTimerCount(QEI_0_INST);
   ```

3. **获取旋转方向:**
   ```c
   DL_TIMER_QEI_DIRECTION direction = DL_TimerG_getQEIDirection(QEI_0_INST);
   ```

4. **启用NVIC中断:**
   ```c
   NVIC_EnableIRQ(QEI_0_INST_INT_IRQN);
   ```

5. **检查挂起中断:**
   ```c
   uint32_t pending = DL_TimerG_getPendingInterrupt(QEI_0_INST);
   ```

6. **清除中断标志:**
   ```c
   DL_TimerG_clearInterruptStatus(QEI_0_INST, interrupt_flag);
   ```

### 中断类型

- `DL_TIMER_IIDX_DIR_CHANGE`: 方向改变中断
- `DL_TIMER_INTERRUPT_OVERFLOW_EVENT`: 计数器溢出中断
- `DL_TIMER_INTERRUPT_UNDERFLOW_EVENT`: 计数器下溢中断

### 方向枚举

- `DL_TIMER_QEI_DIR_UP`: 正方向（顺时针）
- `DL_TIMER_QEI_DIR_DOWN`: 负方向（逆时针）

## 修正后的代码结构

### EncoderInit函数
```c
void EncoderInit(GPTIMER_Regs *Timer, GPTIMER_Regs *realTimer,
    int wheelLength, int ppr)
{
    // ... 参数检查和设置 ...
    
    // 启动QEI计数器（QEI模式在SysConfig中配置）
    DL_TimerG_startCounter(Timer);
    
    // 设置实时定时器用于速度采样
    DL_TimerG_clearInterruptStatus(realTimer, DL_TIMER_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableInterrupt(realTimer, DL_TIMER_INTERRUPT_ZERO_EVENT);
    DL_TimerG_startCounter(realTimer);
    
    // 初始化计数值
    encoders[numEncoders].prev_cnt = DL_Timer_getTimerCount(Timer);
    
    // ... 其他初始化 ...
}
```

### UpdateSpeed函数
```c
void UpdateSpeed(int i, GPTIMER_Regs *reload_tim)
{
    if(reload_tim == encoders[i].realTimer)
    {
        // 使用正确的函数获取计数值
        int16_t current_cnt = DL_Timer_getTimerCount(encoders[i].timer);
        
        // 计算速度差值
        encoders[i].speed = (int16_t)(current_cnt - encoders[i].prev_cnt);
        encoders[i].prev_cnt = current_cnt;
        encoders[i].dis += StoDis(encoders[i]);
    }
}
```

### 中断处理函数示例
```c
void QEI_0_INST_IRQHandler(void)
{
    uint32_t pending_interrupt = DL_TimerG_getPendingInterrupt(QEI_0_INST);
    
    if (pending_interrupt & DL_TIMER_IIDX_DIR_CHANGE) 
    {
        DL_TIMER_QEI_DIRECTION direction = DL_TimerG_getQEIDirection(QEI_0_INST);
        
        if (direction == DL_TIMER_QEI_DIR_UP) 
        {
            // 正方向处理
            DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
        } 
        else 
        {
            // 负方向处理
            DL_GPIO_clearPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
        }
        
        DL_TimerG_clearInterruptStatus(QEI_0_INST, DL_TIMER_IIDX_DIR_CHANGE);
    }
}
```

## SysConfig配置要求

QEI必须在SysConfig中进行配置：

1. **定时器模式**: 选择QEI (Quadrature Encoder Interface)
2. **QEI模式**: Quadrature counting  
3. **通道配置**: 
   - Channel 0: PHA输入
   - Channel 1: PHB输入
4. **引脚配置**:
   - PA21: QEI PHA功能
   - PA2: QEI PHB功能
5. **中断配置**: 根据需要启用方向改变、溢出等中断

## 修正文件

已修正的文件：
- `Hall_Encoder.c`: 主驱动文件
- `Hall_Encoder_Example.c`: 基本示例
- `Hall_Encoder_Corrected_Example.c`: 完整修正示例

## 总结

修正后的代码使用了正确的TI DriverLib函数，符合MSPM0G3507的QEI功能特性。主要改进：

1. 使用`DL_Timer_getTimerCount()`读取QEI计数
2. 移除不存在的QEI配置函数（在SysConfig中配置）
3. 添加了完整的QEI中断处理示例
4. 提供了正确的函数调用方式

感谢用户的指正，这确保了代码的正确性和可用性。
