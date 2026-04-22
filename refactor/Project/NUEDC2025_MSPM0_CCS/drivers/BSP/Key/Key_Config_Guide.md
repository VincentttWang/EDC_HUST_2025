# 按键驱动配置指南 - TI MSPM0G3507平台

## SysConfig配置详细步骤

### 1. GPIO配置
1. **打开SysConfig工具**
2. **添加GPIO模块**:
   - 点击左侧 "+" 按钮
   - 选择 "GPIO"

### 2. 按键1配置 (KEY1)
```
Pin Configuration:
├─ Pin: PA18 (可根据实际硬件调整)
├─ Direction: Input
├─ Internal Resistor: Pull-up
├─ Interrupt: Disable
└─ Pin Function: GPIO
```

### 3. 按键2配置 (KEY2)
```
Pin Configuration:
├─ Pin: PA19 (可根据实际硬件调整)  
├─ Direction: Input
├─ Internal Resistor: Pull-up
├─ Interrupt: Disable
└─ Pin Function: GPIO
```

### 4. 可选定时器配置 (用于中断扫描模式)
```
Timer Configuration:
├─ Timer Instance: TIMG0
├─ Clock Source: BUSCLK
├─ Timer Period: 160000 (5ms @ 32MHz)
├─ Interrupt Enable: ✓
└─ Timer Mode: Periodic
```

## 硬件连接指南

### 标准按键连接
```
按键类型        连接方式                     说明
轻触开关        一端接GPIO，另一端接GND       最常用
薄膜开关        一端接GPIO，另一端接GND       面板应用
机械开关        一端接GPIO，另一端接GND       高可靠性
旋转编码器      A/B相接GPIO，C相接GND        特殊应用
```

### 推荐硬件电路
```
3.3V ──┬── 10kΩ ──┬── PA18 (KEY1)
       │          │
       └── 10kΩ ──┴── PA19 (KEY2)
                  │
              [按键开关]
                  │
                 GND
```

### 上拉电阻选择
| 阻值 | 特点 | 适用场景 |
|------|------|----------|
| 4.7kΩ | 响应快，功耗稍高 | 高频操作 |
| 10kΩ | 平衡选择 | 一般应用 |
| 47kΩ | 功耗低，抗干扰强 | 低功耗应用 |

## 代码配置

### 1. 头文件包含
```c
#include "key.h"
#include "ti_msp_dl_config.h"
```

### 2. 引脚配置检查
确保在 `key.h` 中的引脚配置与SysConfig一致：
```c
// 检查这些定义是否与实际硬件匹配
#define KEY1_PORT               GPIOA
#define KEY1_PIN                DL_GPIO_PIN_18
#define KEY2_PORT               GPIOA
#define KEY2_PIN                DL_GPIO_PIN_19
```

### 3. 时间参数调整
根据实际需求调整时间参数：
```c
// 在key.h中修改这些参数
#define KEY_DEBOUNCE_TIME       20      // 消抖时间
#define KEY_SHORT_PRESS_TIME    50      // 短按最小时间
#define KEY_LONG_PRESS_TIME     1000    // 长按判定时间
#define KEY_REPEAT_TIME         200     // 长按重复间隔
#define KEY_DOUBLE_CLICK_TIME   300     // 双击间隔时间
```

### 4. 逻辑电平配置
根据硬件电路配置逻辑电平：
```c
// 按键按下时的电平 (大多数情况下是0)
#define KEY_PRESSED_LEVEL       0       // 低电平有效
// #define KEY_PRESSED_LEVEL    1       // 高电平有效 (特殊情况)
```

## 使用模式配置

### 模式1: 主循环扫描模式
```c
void main(void) {
    SYSCFG_DL_init();
    Key_Init();
    
    while(1) {
        Key_Scan();                     // 主循环扫描
        
        // 处理按键事件
        if (Key1_short_press()) {
            // 按键处理
        }
        
        DL_Common_delayCycles(160000);  // 5ms延时
    }
}
```

### 模式2: 定时器中断扫描模式
1. **SysConfig配置定时器**:
   ```
   Timer: TIMG0
   Period: 160000 (5ms)
   Interrupt: Enable
   ```

2. **中断服务函数**:
   ```c
   void TIMG0_IRQHandler(void) {
       if (DL_Timer_isInterruptPending(TIMER_0_INST, DL_TIMER_INTERRUPT_ZERO_EVENT)) {
           Key_Scan();
           DL_Timer_clearInterruptStatus(TIMER_0_INST, DL_TIMER_INTERRUPT_ZERO_EVENT);
       }
   }
   ```

3. **主循环**:
   ```c
   void main(void) {
       SYSCFG_DL_init();
       Key_Init();
       
       while(1) {
           // 处理按键事件 (扫描在中断中进行)
           if (Key1_short_press()) {
               // 按键处理
           }
           
           // 其他任务
           other_tasks();
       }
   }
   ```

## 不同应用场景配置

### 1. 简单开关控制
```c
// 配置建议
#define KEY_DEBOUNCE_TIME       20      // 标准消抖
#define KEY_SHORT_PRESS_TIME    50      // 快速响应
#define KEY_LONG_PRESS_TIME     2000    // 较长的长按时间
// 不需要双击和重复功能
```

### 2. 菜单导航控制
```c
// 配置建议
#define KEY_DEBOUNCE_TIME       15      // 快速响应
#define KEY_SHORT_PRESS_TIME    30      // 快速短按
#define KEY_LONG_PRESS_TIME     800     // 适中的长按时间
#define KEY_REPEAT_TIME         150     // 快速重复
#define KEY_DOUBLE_CLICK_TIME   250     // 快速双击
```

### 3. 数值调节控制
```c
// 配置建议
#define KEY_DEBOUNCE_TIME       20      // 标准消抖
#define KEY_SHORT_PRESS_TIME    40      // 精确控制
#define KEY_LONG_PRESS_TIME     600     // 较短的长按时间
#define KEY_REPEAT_TIME         100     // 快速重复调节
#define KEY_DOUBLE_CLICK_TIME   300     // 双击复位
```

### 4. 游戏控制
```c
// 配置建议
#define KEY_DEBOUNCE_TIME       10      // 最快响应
#define KEY_SHORT_PRESS_TIME    20      // 极快短按
#define KEY_LONG_PRESS_TIME     500     // 快速长按
#define KEY_REPEAT_TIME         50      // 极快重复
#define KEY_DOUBLE_CLICK_TIME   200     // 快速双击
```

## 引脚选择指南

### 可用GPIO引脚
| 引脚 | 特点 | 推荐用途 |
|------|------|----------|
| PA0-PA7 | 低编号引脚 | 优先选择 |
| PA14-PA21 | 中等编号引脚 | 常用选择 |
| PA24-PA31 | 高编号引脚 | 备用选择 |

### 引脚冲突避免
确保按键引脚不与以下功能冲突：
- I2C通信引脚 (SCL/SDA)
- SPI通信引脚 (SCLK/MOSI/MISO/CS)
- UART通信引脚 (TX/RX)
- PWM输出引脚
- ADC输入引脚

### 推荐引脚组合
```c
// 组合1: 基本按键
#define KEY1_PIN    DL_GPIO_PIN_18
#define KEY2_PIN    DL_GPIO_PIN_19

// 组合2: 扩展按键
#define KEY1_PIN    DL_GPIO_PIN_14
#define KEY2_PIN    DL_GPIO_PIN_15
#define KEY3_PIN    DL_GPIO_PIN_16
#define KEY4_PIN    DL_GPIO_PIN_17

// 组合3: 避免冲突
#define KEY1_PIN    DL_GPIO_PIN_24
#define KEY2_PIN    DL_GPIO_PIN_25
```

## 调试配置

### 1. 添加调试功能
在 `key.h` 中添加：
```c
#define KEY_DEBUG_ENABLE    1

#if KEY_DEBUG_ENABLE
#define KEY_DEBUG(x)   printf x
#else
#define KEY_DEBUG(x)
#endif
```

### 2. 状态监控
```c
void debug_key_status(void) {
    static uint32_t last_debug_time = 0;
    uint32_t current_time = DL_Common_getTick();
    
    if (current_time - last_debug_time > 1000) {  // 每秒打印一次
        for (int i = 0; i < KEY_ID_MAX; i++) {
            KEY_DEBUG(("Key%d: state=%d, level=%d\n", 
                      i, keys[i].state, keys[i].current_level));
        }
        last_debug_time = current_time;
    }
}
```

### 3. 事件记录
```c
void debug_key_event(key_id_t key_id, key_event_t event) {
    const char* event_names[] = {
        "NONE", "PRESS", "RELEASE", "SHORT_PRESS", 
        "LONG_PRESS", "LONG_PRESS_REPEAT", "DOUBLE_CLICK"
    };
    
    KEY_DEBUG(("Key%d: %s at %d ms\n", 
              key_id, event_names[event], DL_Common_getTick()));
}
```

## 性能优化配置

### 1. 减少CPU占用
```c
// 降低扫描频率 (适用于响应要求不高的场景)
#define KEY_SCAN_INTERVAL   10      // 10ms扫描间隔

// 在主循环中
static uint32_t last_scan_time = 0;
uint32_t current_time = DL_Common_getTick();
if (current_time - last_scan_time >= KEY_SCAN_INTERVAL) {
    Key_Scan();
    last_scan_time = current_time;
}
```

### 2. 减少内存占用
```c
// 如果只需要基本功能，可以禁用高级功能
#define KEY_ENABLE_DOUBLE_CLICK     0
#define KEY_ENABLE_LONG_REPEAT      0
#define KEY_ENABLE_EVENT_HISTORY    0
```

### 3. 电源优化
```c
// 低功耗模式配置
#define KEY_LOW_POWER_MODE          1
#define KEY_SCAN_INTERVAL_LP        50      // 低功耗模式下的扫描间隔

// 在低功耗模式下降低扫描频率
void key_low_power_scan(void) {
    static uint32_t last_scan = 0;
    if (DL_Common_getTick() - last_scan > KEY_SCAN_INTERVAL_LP) {
        Key_Scan();
        last_scan = DL_Common_getTick();
    }
}
```

## 测试配置

### 1. 功能测试程序
```c
void test_key_functions(void) {
    Key_Init();
    
    printf("按键功能测试开始...\n");
    printf("请按下KEY1进行短按测试\n");
    
    while(1) {
        Key_Scan();
        
        // 测试所有事件类型
        key_event_t event1 = Key_GetEvent(KEY_ID_1);
        key_event_t event2 = Key_GetEvent(KEY_ID_2);
        
        if (event1 != KEY_EVENT_NONE) {
            printf("KEY1事件: %d\n", event1);
        }
        
        if (event2 != KEY_EVENT_NONE) {
            printf("KEY2事件: %d\n", event2);
        }
        
        DL_Common_delayCycles(160000);
    }
}
```

### 2. 时序测试
```c
void test_key_timing(void) {
    Key_Init();
    uint32_t press_start = 0;
    
    while(1) {
        Key_Scan();
        
        if (Key_GetEvent(KEY_ID_1) == KEY_EVENT_PRESS) {
            press_start = DL_Common_getTick();
            printf("按键按下: %d ms\n", press_start);
        }
        
        if (Key_GetEvent(KEY_ID_1) == KEY_EVENT_RELEASE) {
            uint32_t press_duration = DL_Common_getTick() - press_start;
            printf("按键释放: 持续时间 %d ms\n", press_duration);
        }
        
        DL_Common_delayCycles(160000);
    }
}
```

## 配置验证清单

### 硬件检查
- [ ] 按键硬件连接正确
- [ ] 上拉电阻配置正确
- [ ] 引脚没有冲突
- [ ] 电源供应稳定

### 软件配置检查
- [ ] SysConfig中GPIO配置正确
- [ ] 引脚定义与实际硬件匹配
- [ ] 时间参数设置合理
- [ ] 逻辑电平配置正确
- [ ] 包含必要的头文件

### 功能测试检查
- [ ] 按键基本读取功能正常
- [ ] 短按检测准确
- [ ] 长按检测准确
- [ ] 双击检测正常
- [ ] 消抖功能有效
- [ ] 响应时间合适
