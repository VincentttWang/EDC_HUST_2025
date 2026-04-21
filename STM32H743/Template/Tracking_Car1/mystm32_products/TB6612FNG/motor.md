# TB6612FNG 电机驱动库说明

本库用于基于 STM32 的 TB6612FNG 电机驱动芯片控制，支持电机正转、反转、刹车、滑行等多种工作模式，并可通过 PWM 占空比调节速度。

---

## 主要函数用法

### 1. MotorInit

```c
void MotorInit(
    Motor* M,
    GPIO_TypeDef* p_port, uint16_t p_pin,
    GPIO_TypeDef* n_port, uint16_t n_pin,
    TIM_HandleTypeDef* pwm_timer, uint32_t pwm_channel,
    uint16_t current_duty
);
```

**功能说明：**  
初始化电机结构体，配置正反向引脚、PWM 定时器及初始占空比，并将电机设置为休眠（SLEEP）模式。

**参数说明：**
- `M`：指向 Motor 结构体的指针
- `p_port`、`p_pin`：正向控制引脚的 GPIO 组和引脚号（如 GPIOA, GPIO_PIN_1）
- `n_port`、`n_pin`：反向控制引脚的 GPIO 组和引脚号
- `pwm_timer`：PWM 定时器句柄指针
- `pwm_channel`：PWM 通道号（如 TIM_CHANNEL_1）
- `current_duty`：初始 PWM 占空比（1~1000）

**使用示例：**
```c
Motor motorA;
MotorInit(&motorA, GPIOA, GPIO_PIN_1, GPIOA, GPIO_PIN_2, &htim1, TIM_CHANNEL_1, 500);
```

---

### 2. MotorSet

```c
void MotorSet(MOVETYPE type, uint16_t duty, Motor* M);
```

**功能说明：**  
设置电机的运动模式和 PWM 占空比，实现前进、后退、刹车、滑行等功能。

**参数说明：**
- `type`：运动模式（FOR 前进，BACK 后退，BREAK 刹车，SLIDE 滑行，SLEEP 休眠）
- `duty`：PWM 占空比（1~1000）
- `M`：指向 Motor 结构体的指针

**使用示例：**
```c
MotorSet(FOR, 800, &motorA);   // 电机正转，速度较快
MotorSet(BACK, 400, &motorA);  // 电机反转，速度较慢
MotorSet(BREAK, 0, &motorA);   // 电机制动
MotorSet(SLIDE, 0, &motorA);   // 电机滑行
```

---

## 其他说明

- 结构体 `Motor` 及枚举 `MOVETYPE` 需在 `tb6612fng.h` 中定义。
- PWM 占空比与实际速度非线性关系，建议根据实际需求调整 duty 值。
- 若传入的指针为 NULL，函数将直接返回，不进行任何操作。

---

## 注意事项

- 使用前需包含 `tb6612fng.h` 并正确初始化相关外设（GPIO、定时器等）。
- 电机控制引脚及 PWM 通道需与硬件连接一致。
- 若需调试，可取消注释代码中的 UART 输出语句，并替换为实际串口句柄。
- duty 建议设置在 1~1000 范围内，避免超出 PWM 定时器的计数范围。

---

如有疑问或需进一步扩展，请参考源码注释或联系开发者。