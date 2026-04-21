/*
 * Hall_Encoder_Corrected_Example.c
 * 
 * 修正后的霍尔编码器驱动在TI MSPM0G3507上的使用示例
 * 基于正确的TI DriverLib函数
 * 
 * 硬件连接：
 * QEI_0 (TIMG8):
 * - PHA (A相): PA21 (PINCM46) 
 * - PHB (B相): PA2 (PINCM7)
 * 
 * 时间基准定时器: TIMG1
 */

#include "Hall_Encoder.h"

// 全局变量
volatile uint32_t encoder_count = 0;
volatile DL_TIMER_QEI_DIRECTION current_direction = DL_TIMER_QEI_DIR_UP;

// TIMG1定时器中断处理函数（用于速度采样）
void TIMG1_IRQHandler(void)
{
    // 检查是否是零事件中断
    if (DL_TimerG_getPendingInterrupt(TIMG1) & DL_TIMER_INTERRUPT_ZERO_EVENT) 
    {
        // 更新所有编码器速度
        UpdateAllSpeed(TIMG1);
        
        // 清除中断标志
        DL_TimerG_clearInterruptStatus(TIMG1, DL_TIMER_INTERRUPT_ZERO_EVENT);
    }
}

// QEI中断处理函数（可选功能）
void QEI_0_INST_IRQHandler(void)
{
    uint32_t pending_interrupt = DL_TimerG_getPendingInterrupt(QEI_0_INST);
    
    // 检查方向改变中断
    if (pending_interrupt & DL_TIMER_IIDX_DIR_CHANGE) 
    {
        // 获取当前旋转方向
        current_direction = DL_TimerG_getQEIDirection(QEI_0_INST);
        
        if (current_direction == DL_TIMER_QEI_DIR_UP) 
        {
            // 正方向（顺时针）- 点亮LED1
            DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
        } 
        else 
        {
            // 负方向（逆时针）- 熄灭LED1  
            DL_GPIO_clearPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
        }
        
        // 清除中断标志
        DL_TimerG_clearInterruptStatus(QEI_0_INST, DL_TIMER_IIDX_DIR_CHANGE);
    }
    
    // 可以添加其他QEI中断处理，如溢出/下溢等
    if (pending_interrupt & DL_TIMER_INTERRUPT_OVERFLOW_EVENT) 
    {
        // 处理计数器溢出
        DL_TimerG_clearInterruptStatus(QEI_0_INST, DL_TIMER_INTERRUPT_OVERFLOW_EVENT);
    }
}

// 初始化函数
void Encoder_Corrected_Init(void)
{
    // 初始化系统配置（在SysConfig中已配置QEI模式）
    SYSCFG_DL_init();
    
    // 初始化编码器驱动
    // 参数：QEI定时器, 采样定时器, 轮径66mm, 360PPR
    EncoderInit(QEI_0_INST, TIMG1, 66, 360);
    
    // 启用NVIC中断
    NVIC_EnableIRQ(TIMG1_INT_IRQn);      // 采样定时器中断
    NVIC_EnableIRQ(QEI_0_INST_INT_IRQN); // QEI中断（可选）
}

// 主循环示例
void Encoder_Main_Loop(void)
{
    while(1)
    {
        // 获取当前编码器计数
        encoder_count = DL_Timer_getTimerCount(QEI_0_INST);
        
        // 获取编码器速度 (m/s)
        double speed = getSpeed(0);
        
        // 获取累计距离 (m)
        double distance = getDis(0);
        
        // 获取当前方向
        DL_TIMER_QEI_DIRECTION direction = DL_TimerG_getQEIDirection(QEI_0_INST);
        
        // 简单的软件延时
        delay_cycles(3200000); // 约100ms延时 (32MHz时钟)
        
        // 这里可以通过调试器或串口输出数据
        // Debug_printf("Count: %d, Speed: %.3f m/s, Distance: %.3f m\n", 
        //              encoder_count, speed, distance);
    }
}

// 双编码器初始化示例
void Dual_Encoder_Init(void)
{
    // 初始化系统配置
    SYSCFG_DL_init();
    
    // 初始化左右编码器
    // 假设有QEI_0_INST (TIMG8) 和 QEI_1_INST (TIMG9)
    LRInit(QEI_0_INST, QEI_1_INST, 66, 360, TIMG1);
    
    // 启用中断
    NVIC_EnableIRQ(TIMG1_INT_IRQn);
    NVIC_EnableIRQ(QEI_0_INST_INT_IRQN);
    // NVIC_EnableIRQ(QEI_1_INST_INT_IRQN);  // 如果有第二个QEI
}

// 双编码器使用示例
void Dual_Encoder_Usage(void)
{
    if (LRFlag) 
    {
        // 获取左右轮速度
        double left_speed = lSpeed();
        double right_speed = rSpeed();
        double center_speed = cSpeed();
        
        // 获取左右轮距离
        double left_distance = lDis();
        double right_distance = rDis();
        
        // 计算车辆角速度（假设轮距为200mm）
        double wheel_base = 0.2; // 0.2m
        double angular_velocity = (right_speed - left_speed) / wheel_base;
        
        // 这里可以用于车辆控制算法
        // 如PID控制、里程计算等
    }
}

// 延时函数
void delay_cycles(uint32_t cycles)
{
    volatile uint32_t i;
    for(i = 0; i < cycles; i++)
    {
        __NOP(); // 空操作
    }
}

/*
 * SysConfig配置要点：
 * 
 * QEI_0 (TIMG8) 配置:
 * 1. Timer Mode: QEI (Quadrature Encoder Interface)
 * 2. QEI Mode: Quadrature counting
 * 3. Channel 0: PHA input (PA21)
 * 4. Channel 1: PHB input (PA2)
 * 5. Counter Period: 根据编码器分辨率设置（如65535）
 * 6. Enable Direction Change Interrupt (可选)
 * 7. Enable Overflow/Underflow Interrupt (可选)
 * 
 * TIMG1 配置:
 * 1. Timer Mode: Periodic Timer
 * 2. Period: 10ms (100Hz采样频率)
 * 3. Enable Zero Event Interrupt
 * 4. Prescaler: 根据需要设置
 * 
 * GPIO配置:
 * 1. PA21: QEI PHA功能
 * 2. PA2: QEI PHB功能
 * 3. LED引脚: 输出模式（用于方向指示，可选）
 */
