# Init_SMotor 配置指南 - TI MSPM0G3507平台

## SysConfig配置步骤

### 1. 添加PWM模块
1. 在SysConfig中点击 "+" 按钮
2. 选择 "Timer" -> "PWM"
3. 重命名为 "PWM_0" (用于YAW轴)
4. 再添加一个PWM，重命名为 "PWM_1" (用于PITCH轴)

### 2. PWM_0 配置 (YAW轴)
```
Timer Instance: TIMG0
Clock Source: BUSCLK (32MHz)
Timer Period: 32000 (1kHz PWM频率)
Capture Compare 0:
  - Enable: ✓
  - Initial Value: 0
  - Pin: 根据板子选择合适引脚
```

### 3. PWM_1 配置 (PITCH轴)  
```
Timer Instance: TIMG1
Clock Source: BUSCLK (32MHz)
Timer Period: 32000 (1kHz PWM频率)
Capture Compare 0:
  - Enable: ✓
  - Initial Value: 0
  - Pin: 根据板子选择合适引脚
```

### 4. GPIO配置
1. 添加GPIO模块
2. 配置以下引脚为输出:
   - PA0: YAW方向控制
   - PA1: PITCH方向控制
   - 可根据实际硬件调整引脚分配

### 5. 生成代码
1. 点击右上角 "Generate" 按钮
2. 确认生成 `ti_msp_dl_config.h` 和 `ti_msp_dl_config.c`

## 硬件连接

### 步进电机驱动器连接
```
驱动器引脚    MSPM0G3507引脚    说明
PUL+         PWM输出引脚       脉冲信号
PUL-         GND              脉冲信号地
DIR+         GPIO输出引脚      方向控制
DIR-         GND              方向控制地
ENA+         VCC 或 GPIO      使能控制 (可选)
ENA-         GND              使能控制地
VCC          外部电源+         驱动器电源
GND          外部电源-         驱动器电源地
```

### 推荐引脚分配
```
功能          引脚    SysConfig配置
YAW_PWM       PA17    PWM_0 CC0 输出
PITCH_PWM     PA31    PWM_1 CC0 输出  
YAW_DIR       PA0     GPIO输出
PITCH_DIR     PA1     GPIO输出
```

## 电机参数配置

### 常见步进电机参数
| 电机型号 | 步进角 | 推荐细分 | 最大频率 |
|----------|--------|----------|----------|
| 28BYJ-48 | 5.625° | 1        | 500Hz    |
| NEMA17   | 1.8°   | 16       | 2000Hz   |
| NEMA23   | 1.8°   | 32       | 1500Hz   |

### 修改配置参数
在 `Init_SMotor.h` 中修改:
```c
// 根据实际电机修改步进角
#define YAW_SMOTOR_STEP_ANGULAR 1.8f    // 步进角度

// 根据驱动器设置修改细分
#define YAW_SMOTOR_STEP_DIVISOR 16.0f   // 16细分

// 根据实际引脚修改GPIO
#define YAW_SMOTOR_DIR_PIN DL_GPIO_PIN_0
```

## 测试步骤

### 1. 基本功能测试
```c
void basic_test(void) {
    YP_SMotor_Init();
    
    // 测试YAW轴
    YP_SMotor_SetSpeed(30.0f, 0.0f);  // YAW轴转动
    DL_Common_delayCycles(32000000);  // 1秒
    
    // 测试PITCH轴  
    YP_SMotor_SetSpeed(0.0f, 30.0f);  // PITCH轴转动
    DL_Common_delayCycles(32000000);  // 1秒
    
    // 停止
    YP_SMotor_SetSpeed(0.0f, 0.0f);
}
```

### 2. 方向测试
```c
void direction_test(void) {
    YP_SMotor_Init();
    
    // 正向
    YP_SMotor_SetSpeed(45.0f, 45.0f);
    DL_Common_delayCycles(32000000);
    
    // 反向
    YP_SMotor_SetSpeed(-45.0f, -45.0f);  
    DL_Common_delayCycles(32000000);
    
    YP_SMotor_SetSpeed(0.0f, 0.0f);
}
```

## 常见问题解决

### 问题1: 电机不转动
**可能原因:**
- PWM信号未输出
- 驱动器无电源  
- 连线错误

**解决方法:**
1. 用示波器检查PWM输出
2. 确认驱动器电源指示灯
3. 检查所有连线

### 问题2: 方向错误
**可能原因:**
- 方向信号极性错误
- 配置中方向定义错误

**解决方法:**
1. 交换DIR+和DIR-连线
2. 修改配置中的ANTI_DIR定义

### 问题3: 运动不平滑
**可能原因:**
- PWM频率过高
- 负载过大
- 细分设置不当

**解决方法:**
1. 降低PWM频率 (增大Timer Period)
2. 减少负载或降低速度
3. 调整驱动器细分开关

### 问题4: 位置不准确
**可能原因:**
- 细分设置不匹配
- 步进角度配置错误
- 负载导致失步

**解决方法:**
1. 确认软硬件细分设置一致
2. 检查步进角度配置
3. 降低加速度和最大速度

## 性能优化建议

### 1. 速度优化
- 根据负载调整最大速度
- 使用适当的加速度曲线
- 避免频繁的速度变化

### 2. 精度优化
- 使用更高的细分设置
- 增加状态更新频率
- 添加位置反馈传感器

### 3. 稳定性优化  
- 添加软件滤波
- 使用合适的电源供应
- 注意EMI防护
