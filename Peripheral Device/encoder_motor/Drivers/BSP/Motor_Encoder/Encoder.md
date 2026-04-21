# Hall_Encoder 编码器测速模块

本模块为 STM32 平台下的霍尔编码器测速库，适用于双轮差速小车等场景。通过定时器和编码器接口，实时获取轮速，支持多编码器管理，并可灵活集成到实际项目中。

---

## 功能简介

- 支持多路编码器测速（最多 10 路）
- 通过定时器中断自动更新速度
- 速度单位为 m/s，正负号区分前进/后退
- 提供简单易用的初始化和查询接口

---

## 主要函数说明

### 1. `void EncoderInit(TIM_HandleTypeDef *Timer, uint32_t chan1, uint32_t chan2, TIM_HandleTypeDef *realTimer);`

**功能**：  
初始化一个编码器测速通道，配置相关定时器和通道，并启动 PWM 及定时中断。

**参数**：
- `Timer`：编码器定时器句柄
- `chan1`、`chan2`：编码器通道编号（如 `TIM_CHANNEL_1`、`TIM_CHANNEL_2`）
- `realTimer`：用于测速的定时器句柄

**使用示例**：
```c
EncoderInit(&htim2, TIM_CHANNEL_1, TIM_CHANNEL_2, &htim7);
EncoderInit(&htim1, TIM_CHANNEL_1, TIM_CHANNEL_2, &htim7);
```
> 上述代码分别初始化了两个编码器，测速定时器均为 TIM7。
> 如果你使用源代码，且板子型号为stm32h743vitx系列板子，可将PE11与右轮a口相连，PE9与右轮b口相连，PA0与左轮a口相连，PA1与b口相连，可正好实现前进后退方向正确的测试

---

### 2. `void UpdateAllSpeed(TIM_HandleTypeDef *reload_tim);`

**功能**：  
更新所有已初始化编码器的速度。通常在定时器溢出中断中调用，实现自动测速。

**典型用法**（放在定时器中断回调函数中）：
```c
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    UpdateAllSpeed(htim);
}
```
> 推荐将测速定时器（如 TIM7）的中断服务函数中调用 `UpdateAllSpeed`，实现定时自动测速。

---

### 3. `double getSpeed(int index);`

**功能**：  
获取指定编码器的当前速度，单位为 m/s。`index` 为初始化顺序（0 表示第一个编码器）。

**示例**：
```c
double left_speed = getSpeed(0);
double right_speed = getSpeed(1);
```

---

## 其他函数简介

- `int getTIMx_DetaCnt(TIM_HandleTypeDef *htim);`  
  获取编码器定时器自上次调用后的计数变化量，并重置计数器。

- `void UpdateSpeed(int i, TIM_HandleTypeDef *reload_tim);`  
  更新指定编码器的速度（一般无需手动调用，由 `UpdateAllSpeed` 管理）。

- `uint32_t ReloadTime(TIM_TypeDef* tim);`  
  计算定时器的实际溢出频率，用于速度换算。

---

## 注意事项

- 使用前需确保相关定时器、GPIO 已在 CubeMX 或手动初始化完成。
- 支持多编码器，初始化顺序决定 `getSpeed(index)` 的索引。
- 推荐将 `UpdateAllSpeed` 放在测速定时器的中断回调中，保证测速实时性。
- 速度计算依赖定时器参数和编码器参数，需根据实际硬件配置调整宏定义。

---

如需详细了解更多接口和用法，请参考源码注释或联系作者。