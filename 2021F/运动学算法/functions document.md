# EDC运动学控制库函数说明文档
**先看简述以及隔壁注意事项**

## 简述
- 直行以及转圆周用straight，runcircle
- 获取车辆线性速度，角速度，坐标，以及相对角度用全局变量car 
**注意在主程序里面写入while（1）循环里面写入UpdateData_Car函数，同时保持高频调用以减少误差**
- 简单motor控制用LSet(int) RSet(int) Break()（刹车）
**while(1)循环之前加入MEinit()**
- 已经实现了底层pid控制，理论上可以实相对复杂的运动


## 目录
- [1. 初始化函数 (Initialize.h)](#1-初始化函数-initializeh)
- [2. 运动学控制函数 (kinematics.h)](#2-运动学控制函数-kinematicsh)
- [3. 数据结构说明](#3-数据结构说明)
- [4. 使用示例](#4-使用示例)

---

## 1. 初始化函数 (Initialize.h)
- 库函数目的：为每一个车提供一个示例化接口，同时写入特殊的题中所要用到的函数

### 1.1 电机和编码器初始化

#### `void MEInit(Motor* L, Motor* R)`
**功能：** 初始化左右电机和编码器系统  
**参数：**
- `L`: 左电机结构体指针
- `R`: 右电机结构体指针

**使用说明：**
- 必须在主函数开始时调用
- 自动完成电机PWM、编码器定时器、小车状态的初始化
- 调用后全局变量`car`和`current_data`可用

```c
Motor Left, Right;
MEInit(&Left, &Right); // 初始化电机和编码器
```

---

### 1.2 电机控制函数

#### `void LSet(int16_t duty)` / `void RSet(int16_t duty)`
**功能：** 设置左/右轮电机PWM占空比  
**参数：**
- `duty`: PWM占空比，范围[-1000, 1000]
  - 正值：前进
  - 负值：后退
  - 0：停止

**特点：**
- 自动判断方向（FOR/BACK）
- 自动限幅到±1000
- 推荐的底层电机控制接口

```c
LSet(500);   // 左轮前进，50%占空比
RSet(-300);  // 右轮后退，30%占空比
```

#### `void LMotorSet(MOVETYPE type, uint16_t duty)` / `void RMotorSet(MOVETYPE type, uint16_t duty)`
**功能：** 设置左/右轮电机运动类型和PWM占空比  
**参数：**
- `type`: 运动类型（FOR前进/BACK后退/BREAK制动）
- `duty`: PWM占空比，范围[0, 1000]

```c
LMotorSet(FOR, 400);    // 左轮前进，40%占空比
RMotorSet(BREAK, 0);    // 右轮制动
```

#### `void Break()`
**功能：** 紧急停车，两轮同时制动  
**使用场景：** 需要快速停车时调用

```c
Break(); // 紧急停车
```

---

### 1.3 传感器数据获取

#### `float getYaw()`
**功能：** 获取当前偏航角  
**返回值：** 偏航角，单位度，范围[-180, 180]

#### `float getWz()`
**功能：** 获取当前角速度  
**返回值：** 绕Z轴角速度，单位度/秒

#### `void UpdateData()`
**功能：** 更新全局数据结构`current_data`  
**使用说明：**
- **必须在主循环每周期调用**
- 更新线速度、角速度、偏航角、时间间隔
- 所有运动学函数依赖此数据

#### `void UpdateData_Car()`
**功能：** 更新汽车状态数据`car`  
**使用说明：**
- 内部调用`UpdateData()`
- 额外更新小车位置坐标(x,y)和朝向

```c
while(1) {
    UpdateData();        // 必须每周期调用
    // 或
    UpdateData_Car();    // 如需位置信息
    
    // 运动控制代码...
}
```

---

### 1.4 辅助函数

#### `bool isInTheYaw(float targetYaw, float tolerance)`
**功能：** 检查当前偏航角是否在目标角度附近  
**参数：**
- `targetYaw`: 目标偏航角，单位度
- `tolerance`: 允许误差，单位度

**返回值：** 在误差范围内返回true，否则false

```c
if(isInTheYaw(90.0f, 3.0f)) {
    // 当前朝向在90±3度范围内
}
```

#### `void Back(float theta)`
**功能：** 简单后退函数  
**参数：**
- `theta`: 目标偏航角相对于初始角度的偏移，单位度

**使用说明：**
- 后退距离固定0.3米，速度0.3m/s
- 适合简单的后退操作

```c
Back(0); // 保持当前朝向后退0.3米
```

---

## 2. 运动学控制函数 (kinematics.h)

### 2.1 PID控制

#### `void PID_Init(PIDdata *pid)`
**功能：** 初始化PID数据结构

#### `void PID_Update(PIDdata *pid, float target, float current, float dt)`
**功能：** 更新PID计算数据

#### `float PID_Compute(PIDdata *pid, float Kp, float Ki, float Kd)`
**功能：** 计算PID输出

---

### 2.2 核心运动函数

#### `Speed PID_Move(float v, float w, short isreload)`
**功能：** PID速度控制函数  
**参数：**
- `v`: 目标线速度，单位m/s
- `w`: 目标角速度，单位度/s
- `isreload`: 1-重新初始化PID，0-正常控制

**返回值：** 当前实际速度

**使用说明：**
- 高级速度控制接口，自动PID调节
- 第一次调用或切换目标时设置`isreload=1`
- 后续持续调用设置`isreload=0`

```c
// 前进0.2m/s
PID_Move(0.2f, 0.0f, 1);  // 初始化
while(1) {
    UpdateData();
    PID_Move(0.2f, 0.0f, 0); // 持续调用
}

// 原地左转30度/s
PID_Move(0.0f, 30.0f, 1);  // 初始化
while(1) {
    UpdateData();
    PID_Move(0.0f, 30.0f, 0); // 持续调用
}
```

#### `bool Straight(float distance, float speed, float target_yaw, DIR dir)`
**功能：** 直线行驶控制  
**参数：**
- `distance`: 目标距离，单位米
- `speed`: 目标速度，单位m/s
- `target_yaw`: 目标偏航角，单位度
- `dir`: 方向（FORWARD前进/BACKWARD后退）

**返回值：** 暂时返回false（距离完成时可返回true）

**使用说明：**
- 自动保持直线和目标角度
- 需要在主循环中持续调用
- 内置距离积分和角度修正

```c
float start_yaw = getYaw();
while(1) {
    UpdateData();
    Straight(1.0f, 0.2f, start_yaw, FORWARD); // 前进1米
    // 可根据返回值判断是否完成
}
```

#### `bool runCircle(float radius, float speed, float angle, DIR dir)`
**功能：** 圆周运动控制  
**参数：**
- `radius`: 转弯半径，单位米
- `speed`: 线速度，单位m/s
- `angle`: 转动角度，单位度
- `dir`: 转向（LEFT左转/RIGHT右转）

**返回值：** 暂时返回false（角度完成时可返回true）

**使用说明：**
- 按指定半径做圆周运动
- 自动计算角速度
- 需要在主循环中持续调用

```c
while(1) {
    UpdateData();
    runCircle(0.5f, 0.2f, 90.0f, LEFT); // 半径0.5米左转90度
    // 可根据返回值判断是否完成
}
```

---

### 2.3 辅助函数

#### `WheelSpeed SpeedToWheelSpeed(Speed speed)`
**功能：** 将车体速度转换为左右轮速度

#### `float sumTheta(float theta1, float theta2)`
**功能：** 角度相加并归一化到[-180, 180]

---

## 3. 数据结构说明

### `Pose`
```c
typedef struct{
    float x;
    float y;
    float theta; //偏移角，以上次调用的MEinit时为0°，逆时针为正方向
}Pose;
```

### `Speed`
```c
typedef struct{
    float linear_velocity;  // 线速度，m/s
    float angular_velocity; // 角速度，度/s
}Speed;
```

### `CarState`
```c
typedef struct{
    Pose pose;              // 位置和方向
    Speed speed;            // 当前速度
    WheelSpeed wheel_speed; // 左右轮速度
}CarState;
```

### `Data`
```c
typedef struct{
    Speed speed;  // 当前速度
    float yaw;    // 当前偏航角
    float dt;     // 时间间隔
}Data;
```

---

## 4. 使用示例

### 4.1 基本初始化
```c
int main(void)
{
    // HAL初始化...
    
    Motor Left, Right;
    MEInit(&Left, &Right);
    
    while(1) {
        UpdateData(); // 必须每周期调用
        
        // 运动控制代码...
    }
}
```

### 4.2 简单前进后退
```c
while(1) {
    UpdateData();
    
    LSet(300);  // 左轮前进30%
    RSet(300);  // 右轮前进30%
    HAL_Delay(2000);
    
    LSet(-200); // 左轮后退20%
    RSet(-200); // 右轮后退20%
    HAL_Delay(2000);
}
```

### 4.3 PID速度控制
```c
PID_Move(0.2f, 0.0f, 1); // 初始化，前进0.2m/s

while(1) {
    UpdateData();
    Speed current = PID_Move(0.2f, 0.0f, 0);
    
    // 可以获取当前实际速度进行监控
    printf("Current speed: %.3f m/s\n", current.linear_velocity);
}
```

### 4.4 直线和转弯组合
```c
float start_yaw = getYaw();

while(1) {
    UpdateData();
    
    // 第一阶段：直行1米
    if(!Straight(1.0f, 0.2f, start_yaw, FORWARD)) {
        continue;
    }
    
    // 第二阶段：左转90度
    if(!runCircle(0.3f, 0.15f, 90.0f, LEFT)) {
        continue;
    }
    
    // 完成所有动作
    Break();
    break;
}
```

---
