# NUEDC 2025 嵌入式项目开发规范

> 适用平台：TI MSPM0G3507 (Cortex-M0+)
> 工具链：CCS Studio + ticlang
> 最后更新：2026-04-22

---

## 1. 目录结构

```
NUEDC2025_MSPM0_CCS/
├── app/                    # 应用入口
│   └── main.c
├── config/                 # 外设配置与链接脚本
│   ├── empty.syscfg        # SysConfig 外设配置文件
│   ├── ti_msp_dl_config.*  # SysConfig 生成的驱动配置
│   └── stack_heap.cmd      # 自定义栈/堆大小
├── drivers/                # 驱动层（核心代码）
│   ├── AllHeader.h         # 全局头文件汇总
│   ├── Common/             # 公共类型定义
│   │   └── BspCommon.h     # BSP_Status、Coordinate、Attitude 等
│   ├── BSP/                # 板级支持包（硬件驱动）
│   │   ├── Imu/            # JY61P IMU (WitMotion SDK)
│   │   ├── Key/            # 按键检测
│   │   ├── Laser/          # CanMV 视觉模块通信
│   │   ├── Motor/          # TB6612FNG 电机驱动 + 霍尔编码器
│   │   ├── Oled/           # SSD1306 OLED 显示 (软件I2C)
│   │   ├── StepMotor/      # 步进电机驱动
│   │   └── TrackingSensor/ # 8路灰度循迹传感器
│   ├── Algorithm/          # 算法层
│   │   ├── Pid.c/h         # PID 控制器
│   │   ├── Kinematics.c/h  # 运动学计算
│   │   ├── Rotation.c/h    # 旋转矩阵运算
│   │   ├── Tracking.c/h    # 巡线算法
│   │   ├── SensorProc.c/h  # 传感器数据处理
│   │   └── StepMotorCtrl.c/h # 步进电机 PID 控制
│   ├── System/             # 系统初始化
│   │   ├── Delay.c/h       # 延时函数
│   │   ├── Initialize.c/h  # 电机系统初始化
│   │   └── InitStepMotor.c/h # 步进电机初始化
│   └── App/                # 应用逻辑
│       ├── Mode.c/h        # 题目模式状态机
│       ├── Menu.c/h        # OLED 菜单系统
│       ├── ModeTree.c/h    # 模式树数据结构
│       └── CircleList.c/h  # 循环链表
└── ticlang/                # 编译器相关
    └── *.projectspec       # CCS 导入描述文件
```

### 层级依赖规则

- `App` → 可依赖 `Algorithm`、`System`、`BSP`
- `Algorithm` → 可依赖 `BSP`、`Common`
- `System` → 可依赖 `BSP`、`Common`
- `BSP` → 仅依赖 `Common` 和 TI SDK
- `Common` → 仅依赖标准库和 TI SDK

禁止反向依赖（如 BSP 依赖 Algorithm）。

---

## 2. 命名规范

### 2.1 文件命名

| 类型 | 规则 | 示例 |
|------|------|------|
| 源文件/头文件 | PascalCase | `HallEncoder.c`、`StepMotorCtrl.h` |
| 配置文件 | 小写+下划线 | `ti_msp_dl_config.h`、`stack_heap.cmd` |

### 2.2 函数命名

采用 `模块_动作` 模式（PascalCase）：

```c
// BSP 层
BSP_Status Motor_Init(MotorPWM *motor, ...);
BSP_Status Motor_SetDuty(MotorPWM *motor, int duty);
BSP_Status Encoder_Init(EncoderState *encoder, ...);
float      Encoder_GetSpeed(EncoderState *encoder);
void       TrackingSensor_Read(uint8_t digitalArray[8]);

// Algorithm 层
void PID_Init(PIDController *pid, float Kp, float Ki, float Kd, float limit);
void PID_Reset(PIDController *pid);
void PID_Update(PIDController *pid, float target, float current, float dt);
float PID_Compute(PIDController *pid);

// System 层
void Motor_SystemInit(void);
void Motor_SetLeft(int speed);
void Motor_Brake(void);

// App 层
void mode_problem_b_1(void);
void mode_test_tracking(void);
```

### 2.3 类型命名

| 类型 | 规则 | 示例 |
|------|------|------|
| 结构体 | PascalCase | `MotorPWM`、`PIDController`、`Coordinate` |
| 枚举类型 | PascalCase | `BSP_Status`、`MotorMoveType`、`EncoderDir` |
| 枚举值 | 全大写+下划线 | `MOTOR_FORWARD`、`BSP_OK`、`CANMV_ERR_NONE` |
| 宏定义 | 全大写+下划线 | `WHEEL_DIS`、`DEG_TO_RAD(x)` |
| 全局变量 | `g_` 前缀 | `g_motor_left`、`g_motor_right` |
| 静态模块变量 | `static` 修饰 | `static int edge = 0;` |

### 2.4 头文件保护

使用 `文件名_H` 格式（全大写）：

```c
#ifndef HALL_ENCODER_H
#define HALL_ENCODER_H
// ...
#endif /* HALL_ENCODER_H */
```

---

## 3. 注释规范

### 3.1 文件头注释

每个 `.c` 和 `.h` 文件必须包含文件头注释，使用 Doxygen 格式：

```c
/**
 * @file  HallEncoder.h
 * @brief 霍尔编码器驱动接口，提供转速与方向检测
 */
```

可选字段：`@author`、`@version`、`@date`、`@note`。

### 3.2 函数注释

公开 API 函数在头文件中添加简要说明：

```c
/**
 * @brief 初始化 PID 控制器
 * @param pid            控制器指针
 * @param Kp/Ki/Kd       PID 增益参数
 * @param integral_limit 积分限幅值
 */
void PID_Init(PIDController *pid, float Kp, float Ki, float Kd, float integral_limit);
```

内部实现函数（`static`）一般不需要注释，除非逻辑复杂。

### 3.3 行内注释

- 仅在逻辑不直观时添加，说明"为什么"而非"做了什么"
- 使用中文书写
- 注释与代码之间保留一个空格

```c
Motor_SetLeft(-120); // 反转左电机，执行转弯
sInedge = 0;         // 转弯完成后重置里程计
```

### 3.4 禁止事项

- 不要保留乱码注释或无意义的注释
- 不要注释掉大段代码（应直接删除，由 git 管理历史）
- 不要在注释中引用 issue 编号或临时任务描述

---

## 4. 编码风格

### 4.1 缩进与格式

- 缩进：4 个空格（不使用 Tab）
- 大括号：函数体另起一行，`if/else/for/while` 同行

```c
void Motor_Brake(void)
{
    if (g_motor_left != NULL) {
        Motor_SetDuty(g_motor_left, 0);
    }
}
```

### 4.2 错误处理

BSP 层函数统一返回 `BSP_Status` 枚举：

```c
typedef enum {
    BSP_OK          =  0,
    BSP_ERR_NULL    = -1,
    BSP_ERR_PARAM   = -2,
    BSP_ERR_TIMEOUT = -3,
    BSP_ERR_BUSY    = -4,
} BSP_Status;
```

调用示例：

```c
if (Motor_Init(&motor, ...) != BSP_OK) {
    error_handler();
}
```

CanMV 视觉模块使用 `CanMV_Error` 枚举，判断时使用显式比较：

```c
if (Laser_error == CANMV_ERR_NONE) {
    // 正常处理
}
```

### 4.3 参数校验

- BSP 层函数必须检查指针参数是否为 NULL
- Algorithm/App 层信任内部调用，不做冗余校验
- 外部输入（UART 数据、用户按键）在入口处校验

### 4.4 全局变量

- 尽量避免使用全局变量
- 必须使用时加 `g_` 前缀
- 模块内部变量使用 `static` 限制作用域
- 跨模块共享的变量通过 `extern` 在头文件中声明

---

## 5. 构建与工程管理

### 5.1 CCS 工程导入

1. CCS → File → Import → CCS Projects
2. 选择 `refactor/Project/NUEDC2025_MSPM0_CCS` 目录
3. 勾选 "Copy projects into workspace" 或直接原地导入

### 5.2 构建注意事项

- `Debug/` 目录为自动生成的构建产物，已在 `.gitignore` 中排除
- 重命名或移动源文件后，必须执行 Project → Clean 再重新构建
- 自定义栈/堆大小在 `config/stack_heap.cmd` 中配置，不要修改 SysConfig 生成的 `device_linker.cmd`

### 5.3 SysConfig

- 外设配置统一在 `config/empty.syscfg` 中管理
- 修改 `.syscfg` 后 CCS 会自动重新生成 `ti_msp_dl_config.*`
- 引脚分配变更需同步更新对应 BSP 驱动中的宏定义

### 5.4 编译开关

在 `config/project_build_config.h` 中管理项目级编译开关：

```c
#define PROJECT_ENABLE_TEST_MODES  1  // 启用测试模式菜单项
```

---

## 6. Git 规范

### 6.1 分支策略

| 分支 | 用途 |
|------|------|
| `main` | 稳定版本，可直接烧录 |
| `develop` | 开发分支，日常开发在此进行 |

### 6.2 提交信息格式

采用 Conventional Commits 格式：

```
<类型>(<范围>): <简要描述>

[可选正文]
```

常用类型：

| 类型 | 说明 |
|------|------|
| `feat` | 新功能 |
| `fix` | 修复 Bug |
| `refactor` | 重构（不改变功能） |
| `docs` | 文档变更 |
| `chore` | 构建/工具链变更 |

示例：

```
fix(linker): remove duplicate device_linker.cmd causing memory region conflicts
refactor(drivers): normalize BSP naming to Module_Action pattern
docs(comments): 修复乱码注释，补充中文文件头注释
```

### 6.3 工作日志

重要变更记录在 `refactor/LOGS/WORK_LOG.md` 中，按日期倒序排列。
