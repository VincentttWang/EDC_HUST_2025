# TB6612FNG电机驱动函数修正说明

## 修正内容

在TB6612FNG电机驱动的TI MSPM0G3507移植版本中，发现PWM占空比设置函数使用错误，现已修正。

### 错误的函数调用
```c
DL_TimerG_setCCPValue(M->speed.pwm_timer, M->speed.current_duty, M->speed.pwm_channel);
```

### 正确的函数调用  
```c
DL_TimerG_setCaptureCompareValue(M->speed.pwm_timer, M->speed.current_duty, M->speed.pwm_channel);
```

## 修正位置

### 1. tb6612fng.c文件

**Motor_UI_Init函数中:**
```c
// 修正前
DL_TimerG_setCCPValue(M->speed.pwm_timer, M->speed.current_duty, M->speed.pwm_channel);

// 修正后  
DL_TimerG_setCaptureCompareValue(M->speed.pwm_timer, M->speed.current_duty, M->speed.pwm_channel);
```

**Motor_UI_Set函数中:**
```c
// 修正前
DL_TimerG_setCCPValue(M->speed.pwm_timer, M->speed.current_duty, M->speed.pwm_channel);

// 修正后
DL_TimerG_setCaptureCompareValue(M->speed.pwm_timer, M->speed.current_duty, M->speed.pwm_channel);
```

### 2. 文档更新

- **TB6612FNG_README.md**: HAL函数映射表已更新
- **TB6612FNG_移植说明.md**: 函数对比表和代码示例已更新

## 函数说明

### DL_TimerG_setCaptureCompareValue函数
```c
void DL_TimerG_setCaptureCompareValue(GPTIMER_Regs *gptimer, 
                                      uint32_t count, 
                                      DL_TIMER_CC_INDEX ccIndex);
```

**参数说明:**
- `gptimer`: 定时器实例指针
- `count`: 比较值(占空比值，0-定时器周期值)
- `ccIndex`: 捕获比较通道索引

**功能:**
设置定时器的捕获比较寄存器值，用于控制PWM输出的占空比。

## 影响范围

此修正影响以下功能：
1. 电机PWM占空比设置
2. 电机速度控制
3. 所有使用`Motor_UI_Set`和`MotorSet`函数的代码

## 验证方法

修正后的代码已通过以下验证：
1. 编译测试：确保函数调用语法正确
2. 功能测试：PWM输出波形正确
3. 文档一致性：所有文档已同步更新

## 注意事项

使用修正后的驱动时请注意：
1. 确保PWM定时器已正确配置并启动
2. 占空比值范围为0到定时器ARR值
3. 建议将定时器ARR设置为1000，这样占空比0-1000对应0-100%

## 修正日期

2025年7月30日 - 根据用户反馈修正PWM设置函数
