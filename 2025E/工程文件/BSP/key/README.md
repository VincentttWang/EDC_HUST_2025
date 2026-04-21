# 按键驱动 - TI MSPM0G3507重构版

## 概述
本驱动程序从STM32平台移植到TI MSPM0G3507平台，并完全重新设计了按键检测架构。支持多按键的长短按检测、双击检测、长按重复等高级功能，提供完整的按键事件处理机制。

## 功能特性
- **多按键支持**: 支持最多8个按键 (可扩展)
- **长短按检测**: 精确区分短按和长按操作
- **双击检测**: 支持双击事件检测
- **长按重复**: 长按时自动重复触发事件
- **硬件消抖**: 内置软件消抖算法
- **事件驱动**: 基于事件的按键处理模式
- **状态机设计**: 稳定可靠的状态机架构
- **可配置参数**: 消抖时间、长按时间等可配置

## 硬件连接

### 标准连接方式
```
按键开关        MSPM0G3507      说明
KEY1_SW     ->  PA18            按键1输入 (内置上拉)
KEY1_GND    ->  GND             按键1另一端接地
KEY2_SW     ->  PA19            按键2输入 (内置上拉)  
KEY2_GND    ->  GND             按键2另一端接地
```

### 上拉电阻配置
- **内置上拉**: 推荐使用芯片内置上拉电阻
- **外部上拉**: 也可使用10kΩ外部上拉电阻到3.3V
- **按键逻辑**: 按下时为低电平，释放时为高电平

## SysConfig配置

### GPIO配置步骤
1. **添加GPIO模块**:
   - 在SysConfig中添加GPIO
   - 配置为输入模式

2. **按键1配置 (PA18)**:
   ```
   Pin: PA18
   Direction: Input
   Internal Resistor: Pull-up
   Interrupt: Disable (软件扫描模式)
   ```

3. **按键2配置 (PA19)**:
   ```
   Pin: PA19  
   Direction: Input
   Internal Resistor: Pull-up
   Interrupt: Disable (软件扫描模式)
   ```

## 按键事件类型

### 事件定义
```c
typedef enum {
    KEY_EVENT_NONE = 0,         // 无事件
    KEY_EVENT_PRESS,            // 按键按下
    KEY_EVENT_RELEASE,          // 按键释放
    KEY_EVENT_SHORT_PRESS,      // 短按
    KEY_EVENT_LONG_PRESS,       // 长按
    KEY_EVENT_LONG_PRESS_REPEAT,// 长按重复
    KEY_EVENT_DOUBLE_CLICK      // 双击
} key_event_t;
```

### 时间参数配置
```c
#define KEY_DEBOUNCE_TIME       20      // 消抖时间 (ms)
#define KEY_SHORT_PRESS_TIME    50      // 短按最小时间 (ms)
#define KEY_LONG_PRESS_TIME     1000    // 长按判定时间 (ms)
#define KEY_REPEAT_TIME         200     // 长按重复间隔 (ms)
#define KEY_DOUBLE_CLICK_TIME   300     // 双击间隔时间 (ms)
```

## API接口

### 初始化函数

#### `Key_Init()`
```c
void Key_Init(void);
```
初始化按键系统。

**使用说明:**
- 必须在使用其他函数前调用
- 初始化所有按键的GPIO和状态
- 配置按键参数

### 扫描函数

#### `Key_Scan()`
```c
void Key_Scan(void);
```
按键扫描处理函数。

**使用说明:**
- 需要在主循环或定时器中断中定期调用
- 推荐调用间隔: 5-10ms
- 负责按键状态机处理和事件生成

### 事件获取函数

#### `Key_GetEvent()`
```c
key_event_t Key_GetEvent(key_id_t key_id);
```
获取指定按键的事件。

**参数:**
- `key_id`: 按键ID (KEY_ID_1, KEY_ID_2)

**返回值:**
- 按键事件类型，获取后自动清除

### 状态查询函数

#### `Key_IsPressed()`
```c
bool Key_IsPressed(key_id_t key_id);
```
检查指定按键是否按下。

#### `Key_IsShortPress()`
```c
bool Key_IsShortPress(key_id_t key_id);
```
检查指定按键的短按事件。

#### `Key_IsLongPress()`
```c
bool Key_IsLongPress(key_id_t key_id);
```
检查指定按键的长按事件。

#### `Key_IsDoubleClick()`
```c
bool Key_IsDoubleClick(key_id_t key_id);
```
检查指定按键的双击事件。

### 兼容接口

为保持与原STM32代码的兼容性，提供了宏定义接口：
```c
#define Key1_Read()                 Key_IsPressed(KEY_ID_1)
#define Key2_Read()                 Key_IsPressed(KEY_ID_2)
#define Key1_short_press()          Key_IsShortPress(KEY_ID_1)
#define Key2_short_press()          Key_IsShortPress(KEY_ID_2)
#define Key1_long_press()           Key_IsLongPress(KEY_ID_1)
#define Key2_long_press()           Key_IsLongPress(KEY_ID_2)
```

## 使用示例

### 基本使用 (兼容模式)
```c
#include "key.h"

void basic_key_example(void) {
    Key_Init();
    
    while(1) {
        Key_Scan();
        
        // 使用原有接口
        if (Key1_short_press()) {
            // 按键1短按处理
        }
        
        if (Key2_long_press()) {
            // 按键2长按处理
        }
        
        DL_Common_delayCycles(160000);  // 5ms延时
    }
}
```

### 事件模式使用
```c
void event_mode_example(void) {
    Key_Init();
    
    while(1) {
        Key_Scan();
        
        key_event_t event = Key_GetEvent(KEY_ID_1);
        switch(event) {
            case KEY_EVENT_SHORT_PRESS:
                // 短按处理
                break;
                
            case KEY_EVENT_LONG_PRESS:
                // 长按处理
                break;
                
            case KEY_EVENT_DOUBLE_CLICK:
                // 双击处理
                break;
                
            default:
                break;
        }
        
        DL_Common_delayCycles(160000);
    }
}
```

### 菜单控制应用
```c
void menu_control_example(void) {
    static uint8_t menu_index = 0;
    const uint8_t max_items = 4;
    
    Key_Init();
    
    while(1) {
        Key_Scan();
        
        // 按键1: 菜单切换
        if (Key1_short_press()) {
            menu_index = (menu_index + 1) % max_items;
            update_menu_display(menu_index);
        }
        
        // 按键2: 确认选择
        if (Key2_short_press()) {
            execute_menu_function(menu_index);
        }
        
        // 长按任意键: 返回主菜单
        if (Key1_long_press() || Key2_long_press()) {
            menu_index = 0;
            show_main_menu();
        }
        
        DL_Common_delayCycles(160000);
    }
}
```

### 数值调节应用
```c
void value_adjust_example(void) {
    static int32_t value = 0;
    static int32_t step = 1;
    
    Key_Init();
    
    while(1) {
        Key_Scan();
        
        // 按键1: 增加数值
        key_event_t key1_event = Key_GetEvent(KEY_ID_1);
        switch(key1_event) {
            case KEY_EVENT_SHORT_PRESS:
                value += step;
                break;
                
            case KEY_EVENT_LONG_PRESS:
                step = 10;  // 长按时步长变大
                break;
                
            case KEY_EVENT_LONG_PRESS_REPEAT:
                value += step;
                break;
                
            case KEY_EVENT_RELEASE:
                step = 1;   // 释放时恢复小步长
                break;
        }
        
        // 按键2: 减少数值
        if (Key2_short_press()) {
            value -= step;
        }
        
        // 双击复位
        if (Key1_double_click()) {
            value = 0;
        }
        
        DL_Common_delayCycles(160000);
    }
}
```

## 定时器中断模式

### 配置定时器
在SysConfig中配置定时器每5ms产生一次中断：
```
Timer Period: 160000 (5ms @ 32MHz)
Interrupt Enable: ✓
```

### 中断服务函数
```c
void TIMER_IRQHandler(void) {
    if (DL_Timer_isInterruptPending(TIMER_0_INST, DL_TIMER_INTERRUPT_ZERO_EVENT)) {
        Key_Scan();  // 定时扫描按键
        DL_Timer_clearInterruptStatus(TIMER_0_INST, DL_TIMER_INTERRUPT_ZERO_EVENT);
    }
}
```

### 主循环处理
```c
void main_loop(void) {
    Key_Init();
    
    while(1) {
        // 处理按键事件 (扫描在中断中进行)
        if (Key1_short_press()) {
            // 处理按键事件
        }
        
        // 其他任务处理
        other_tasks();
    }
}
```

## 状态机原理

### 状态定义
```c
typedef enum {
    KEY_STATE_IDLE = 0,         // 空闲状态
    KEY_STATE_DEBOUNCE,         // 消抖状态
    KEY_STATE_PRESSED,          // 按下状态
    KEY_STATE_LONG_PRESS,       // 长按状态
    KEY_STATE_WAIT_DOUBLE       // 等待双击状态
} key_state_t;
```

### 状态转换图
```
IDLE ─── 按下 ──→ DEBOUNCE ─── 消抖完成 ──→ PRESSED
 ↑                   │                       │
 │                抖动返回                    │
 │                   ↓                       ↓
 └────────────── 释放 ←───── 短按释放 ──→ WAIT_DOUBLE
                                              │
                                              ↓
                LONG_PRESS ←── 长按超时 ──────┘
                     │
                     └── 长按释放 ──→ IDLE
```

## 参数配置

### 时间参数调整
根据实际需求调整以下参数：
```c
// 消抖时间: 根据按键质量调整 (10-50ms)
#define KEY_DEBOUNCE_TIME       20

// 短按时间: 最小有效按键时间 (30-100ms)  
#define KEY_SHORT_PRESS_TIME    50

// 长按时间: 长按判定时间 (500-2000ms)
#define KEY_LONG_PRESS_TIME     1000

// 重复间隔: 长按重复频率 (100-500ms)
#define KEY_REPEAT_TIME         200

// 双击间隔: 双击检测窗口 (200-500ms)
#define KEY_DOUBLE_CLICK_TIME   300
```

### 硬件参数配置
```c
// 按键逻辑电平 (根据硬件电路调整)
#define KEY_PRESSED_LEVEL       0    // 0=低电平有效, 1=高电平有效

// 按键引脚配置 (根据实际连接调整)
#define KEY1_PORT               GPIOA
#define KEY1_PIN                DL_GPIO_PIN_18
#define KEY2_PORT               GPIOA  
#define KEY2_PIN                DL_GPIO_PIN_19
```

## 扩展功能

### 添加更多按键
1. 修改按键数量：
   ```c
   typedef enum {
       KEY_ID_1 = 0,
       KEY_ID_2,
       KEY_ID_3,        // 新增按键3
       KEY_ID_4,        // 新增按键4
       KEY_ID_MAX
   } key_id_t;
   ```

2. 添加引脚配置：
   ```c
   #define KEY3_PORT               GPIOA
   #define KEY3_PIN                DL_GPIO_PIN_20
   #define KEY4_PORT               GPIOA
   #define KEY4_PIN                DL_GPIO_PIN_21
   ```

3. 在`Key_Init()`中初始化新按键。

### 组合按键支持
```c
bool Key_IsCombinationPressed(key_id_t key1, key_id_t key2) {
    return Key_IsPressed(key1) && Key_IsPressed(key2);
}
```

### 按键锁定功能
```c
static bool key_locked = false;

void Key_Lock(void) {
    key_locked = true;
}

void Key_Unlock(void) {
    key_locked = false;
}
```

## 性能参数

| 参数 | 数值 | 说明 |
|------|------|------|
| 扫描频率 | 100-200Hz | 推荐5-10ms间隔 |
| 消抖时间 | 20ms | 可根据按键调整 |
| CPU占用 | <1% | 扫描和处理 |
| 内存占用 | <200字节 | 静态变量 |
| 响应延迟 | <30ms | 包含消抖时间 |

## 故障排除

### 常见问题

1. **按键无响应**
   - 检查GPIO配置是否正确
   - 确认按键硬件连接
   - 验证上拉电阻配置
   - 检查是否调用`Key_Scan()`

2. **按键误触发**
   - 增加消抖时间
   - 检查硬件抗干扰措施
   - 确认按键质量

3. **长按检测不准确**
   - 调整长按时间参数
   - 检查扫描频率是否稳定
   - 确认时间获取函数正确

4. **双击检测失效**
   - 调整双击间隔时间
   - 检查短按时间设置
   - 确认状态机逻辑

### 调试方法

1. **状态监控**:
   ```c
   void debug_key_state(void) {
       for(int i = 0; i < KEY_ID_MAX; i++) {
           printf("Key%d: state=%d, level=%d\n", 
                  i, keys[i].state, keys[i].current_level);
       }
   }
   ```

2. **事件记录**:
   ```c
   void debug_key_event(key_id_t key_id, key_event_t event) {
       printf("Key%d Event: %d at %d ms\n", 
              key_id, event, DL_Common_getTick());
   }
   ```

## 移植说明

从STM32平台移植的主要更改：
- `#include "stm32h7xx_hal.h"` -> `#include "ti_msp_dl_config.h"`
- `HAL_GPIO_ReadPin()` -> `DL_GPIO_readPins()`
- `HAL_GetTick()` -> `DL_Common_getTick()`
- `GPIO_PIN_SET/RESET` -> 数字逻辑电平
- 完全重新设计状态机和事件处理架构

## 文件结构
```
BSP/key/
├── key.h              # 头文件定义
├── key.c              # 驱动实现
├── Key_Example.c      # 使用示例
└── README.md          # 本说明文档
```
