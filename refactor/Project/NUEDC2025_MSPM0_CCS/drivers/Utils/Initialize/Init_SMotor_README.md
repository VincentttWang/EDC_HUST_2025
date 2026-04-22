# Init_SMotor 双轴步进电机控制 - TI MSPM0G3507移植版

## 概述
本模块从STM32平台移植到TI MSPM0G3507平台，提供YAW(偏航)和PITCH(俯仰)双轴步进电机的统一控制接口。适用于云台、摄像头支架、机械臂等需要精确双轴控制的应用场景。

## 功能特性
- **双轴同步控制**: 同时控制YAW和PITCH两个轴
- **独立速度设置**: 每个轴可以独立设置角速度
- **位置反馈**: 实时获取两轴的当前角度位置
- **状态更新**: 自动维护电机运行状态和位置信息
- **灵活配置**: 支持不同的步进角度和细分设置

## 硬件配置

### SysConfig配置要求
1. **PWM定时器配置**:
   - PWM_0_INST: 用于YAW轴控制
   - PWM_1_INST: 用于PITCH轴控制 (或者使用同一定时器的不同通道)
   - 配置为PWM模式，频率范围100Hz-10kHz

2. **GPIO配置**:
   - YAW方向控制: GPIOA Pin 0
   - YAW使能控制: GPIOA Pin 1 (相邻引脚)
   - PITCH方向控制: GPIOA Pin 1  
   - PITCH使能控制: GPIOA Pin 2 (相邻引脚)

### 引脚连接示例
```
MSPM0G3507          YAW步进驱动器      PITCH步进驱动器
PWM_0 输出      ->  PUL+ (YAW)         
PWM_1 输出      ->                      PUL+ (PITCH)
PA0             ->  DIR+ (YAW)
PA1             ->  ENA+ (YAW)          DIR+ (PITCH)
PA2             ->                      ENA+ (PITCH)
GND             ->  PUL-, DIR-, ENA-    PUL-, DIR-, ENA-
VCC             ->  VCC                 VCC
```

## 配置参数

### 默认配置
```c
// YAW轴配置
#define YAW_SMOTOR_PWM_TIMER PWM_0_INST
#define YAW_SMOTOR_PWM_CHANNEL DL_TIMER_CC_0_INDEX
#define YAW_SMOTOR_DIR_PORT GPIOA
#define YAW_SMOTOR_DIR_PIN DL_GPIO_PIN_0
#define YAW_SMOTOR_STEP_ANGULAR 1.8f        // 1.8度/步
#define YAW_SMOTOR_STEP_DIVISOR 32           // 32细分

// PITCH轴配置
#define PITCH_SMOTOR_PWM_TIMER PWM_1_INST
#define PITCH_SMOTOR_PWM_CHANNEL DL_TIMER_CC_0_INDEX
#define PITCH_SMOTOR_DIR_PORT GPIOA
#define PITCH_SMOTOR_DIR_PIN DL_GPIO_PIN_1
#define PITCH_SMOTOR_STEP_ANGULAR 1.8f      // 1.8度/步
#define PITCH_SMOTOR_STEP_DIVISOR 32         // 32细分
```

## API接口

### 核心函数

#### `YP_SMotor_Init()`
```c
void YP_SMotor_Init(void);
```
初始化YAW和PITCH双轴步进电机系统。

**使用说明:**
- 必须在使用其他函数前调用
- 自动配置PWM定时器和GPIO
- 设置电机参数为默认值

#### `YP_SMotor_SetSpeed()`
```c
void YP_SMotor_SetSpeed(float yaw_speed, float pitch_speed);
```
设置双轴步进电机的角速度。

**参数:**
- `yaw_speed`: YAW轴角速度 (度/秒)
  - 正值: 顺时针旋转
  - 负值: 逆时针旋转
  - 0: 停止
- `pitch_speed`: PITCH轴角速度 (度/秒)
  - 正值: 向上旋转
  - 负值: 向下旋转  
  - 0: 停止

#### `YP_SMotor_UpdateState()`
```c
void YP_SMotor_UpdateState(void);
```
更新双轴步进电机状态。

**使用说明:**
- 必须在主循环中定期调用
- 更新位置信息和时间戳
- 建议调用频率: 100Hz (10ms间隔)

#### `GetYaw()` / `GetPitch()`
```c
double GetYaw(void);
double GetPitch(void);
```
获取当前轴的角度位置。

**返回值:**
- 当前角度位置 (度)
- 基于启动时的零点计算

## 使用示例

### 基本使用
```c
#include "Init_SMotor.h"

void basic_control_example(void) {
    // 初始化双轴电机
    YP_SMotor_Init();
    
    // 设置运动速度
    YP_SMotor_SetSpeed(90.0f, 45.0f);  // YAW: 90°/s, PITCH: 45°/s
    
    // 主循环
    while(1) {
        YP_SMotor_UpdateState();
        
        // 获取当前位置
        double yaw_pos = GetYaw();
        double pitch_pos = GetPitch();
        
        DL_Common_delayCycles(320000);  // 10ms延时
    }
}
```

### 云台扫描控制
```c
void gimbal_scan_example(void) {
    YP_SMotor_Init();
    
    float scan_range = 90.0f;  // 扫描范围
    float scan_speed = 30.0f;  // 扫描速度
    
    // 水平扫描
    YP_SMotor_SetSpeed(scan_speed, 0.0f);
    
    while(1) {
        YP_SMotor_UpdateState();
        
        double yaw = GetYaw();
        
        // 到达边界时反向
        if(yaw >= scan_range || yaw <= -scan_range) {
            scan_speed = -scan_speed;
            YP_SMotor_SetSpeed(scan_speed, 0.0f);
        }
        
        DL_Common_delayCycles(320000);
    }
}
```

### 精确位置控制
```c
void position_control_example(void) {
    YP_SMotor_Init();
    
    float target_yaw = 45.0f;
    float target_pitch = 30.0f;
    float tolerance = 2.0f;  // 2度容差
    
    while(1) {
        YP_SMotor_UpdateState();
        
        double current_yaw = GetYaw();
        double current_pitch = GetPitch();
        
        float yaw_error = target_yaw - current_yaw;
        float pitch_error = target_pitch - current_pitch;
        
        // 到达目标位置
        if(fabs(yaw_error) < tolerance && fabs(pitch_error) < tolerance) {
            YP_SMotor_SetSpeed(0.0f, 0.0f);
            break;
        }
        
        // 比例控制
        float yaw_speed = yaw_error * 2.0f;    // 比例系数
        float pitch_speed = pitch_error * 2.0f;
        
        YP_SMotor_SetSpeed(yaw_speed, pitch_speed);
        
        DL_Common_delayCycles(160000);  // 5ms延时
    }
}
```

## 应用场景

### 1. 摄像头云台
```c
void camera_gimbal_application(void) {
    YP_SMotor_Init();
    
    // 自动巡航模式
    float patrol_positions[][2] = {
        {0.0f, 0.0f},      // 中心
        {90.0f, 30.0f},    // 右上
        {0.0f, 45.0f},     // 上
        {-90.0f, 30.0f},   // 左上
        {-90.0f, -30.0f},  // 左下
        {90.0f, -30.0f}    // 右下
    };
    
    int position_count = sizeof(patrol_positions) / sizeof(patrol_positions[0]);
    int current_target = 0;
    
    while(1) {
        // 移动到目标位置
        move_to_position(patrol_positions[current_target][0], 
                        patrol_positions[current_target][1]);
        
        // 在位置停留3秒
        DL_Common_delayCycles(96000000);
        
        current_target = (current_target + 1) % position_count;
    }
}
```

### 2. 太阳能板跟踪
```c
void solar_tracker_application(void) {
    YP_SMotor_Init();
    
    float sun_elevation = 30.0f;  // 太阳高度角
    float sun_azimuth = 0.0f;     // 太阳方位角
    
    while(1) {
        // 根据时间计算太阳位置 (简化算法)
        update_sun_position(&sun_azimuth, &sun_elevation);
        
        // 控制面板朝向太阳
        YP_SMotor_SetSpeed((sun_azimuth - GetYaw()) * 0.5f,
                          (sun_elevation - GetPitch()) * 0.5f);
        
        YP_SMotor_UpdateState();
        DL_Common_delayCycles(32000000);  // 1秒更新一次
    }
}
```

## 性能参数

| 参数 | 数值 | 说明 |
|------|------|------|
| 步进精度 | 0.05625° | 1.8°/32细分 |
| 最大转速 | ~180°/s | 取决于负载 |
| 位置精度 | ±0.1° | 理论精度 |
| 控制频率 | 100Hz | 推荐更新频率 |
| 响应时间 | <50ms | 速度变化响应 |

## 注意事项

1. **硬件配置**: 确保SysConfig中正确配置了PWM定时器和GPIO

2. **电源要求**: 步进电机驱动器需要适当的电源供应

3. **负载考虑**: 大负载时需要降低最大速度以避免失步

4. **更新频率**: `YP_SMotor_UpdateState()`的调用频率影响位置精度

5. **初始化顺序**: 必须先调用`YP_SMotor_Init()`再使用其他函数

6. **细分设置**: 硬件驱动器的细分设置必须与软件配置一致

## 故障排除

### 常见问题

1. **电机不转动**
   - 检查PWM输出是否正常
   - 确认驱动器电源和连线
   - 验证GPIO配置

2. **位置不准确**
   - 增加`YP_SMotor_UpdateState()`调用频率
   - 检查细分设置是否一致
   - 验证步进角度配置

3. **运动不平滑**
   - 降低最大速度
   - 检查负载是否过大
   - 调整PWM频率

4. **方向错误**
   - 交换驱动器的方向信号线
   - 修改`ANTI_DIR`配置

## 移植说明

从STM32平台移植的主要更改：
- `#include "main.h"` -> `#include "ti_msp_dl_config.h"`
- `&htim5` -> `PWM_0_INST` / `PWM_1_INST`
- `TIM_CHANNEL_1` -> `DL_TIMER_CC_0_INDEX`
- `DIR_GPIO_Port` -> `GPIOA`
- `DIR_Pin` -> `DL_GPIO_PIN_0`
- `GPIO_PIN_SET` -> `SMOTOR_DIR_FORWARD`

## 文件结构
```
Utils/Initialize/
├── Init_SMotor.h              # 头文件定义
├── Init_SMotor.c              # 双轴控制实现
├── Init_SMotor_Example.c      # 使用示例代码
└── README.md                  # 本说明文档
```
