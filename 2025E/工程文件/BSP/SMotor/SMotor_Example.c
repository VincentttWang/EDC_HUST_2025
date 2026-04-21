/**
 * @file SMotor_Example.c
 * @brief TI MSPM0G3507平台步进电机驱动使用示例
 * @author 移植自STM32平台
 * @date 2024
 * 
 * 本文件提供了在TI MSPM0G3507平台上使用步进电机驱动的完整示例
 * 包括初始化、参数设置、速度控制和状态更新
 */

#include "SMotor.h"
#include "ti_msp_dl_config.h"

// 全局步进电机实例
SMotor stepper_motor;

/**
 * @brief 步进电机使用示例
 * @note 需要在SysConfig中配置对应的PWM Timer和GPIO
 */
void SMotor_Example(void)
{
    // 1. 初始化步进电机硬件
    SMotor_Init(&stepper_motor, 
                PWM_0_INST,              // PWM Timer实例 (需要在SysConfig中配置)
                DL_TIMER_CC_0_INDEX,     // PWM通道
                GPIOA,                   // 方向控制GPIO端口
                DL_GPIO_PIN_0,           // 方向控制引脚
                GPIOA,                   // 使能控制GPIO端口  
                DL_GPIO_PIN_1);          // 使能控制引脚

    // 2. 设置步进电机参数
    SMotor_Parameters_Init(&stepper_motor,
                          1.8f,         // 步进角度 (度)
                          1,            // 步进细分
                          200);         // 最大步进频率 (Hz)

    // 3. 设置步进电机速度 (正向旋转)
    SMotor_SetSpeed(&stepper_motor, 90.0f);  // 90度/秒

    // 延时让电机运行一段时间
    DL_Common_delayCycles(32000000);  // 1秒延时 (32MHz时钟)

    // 4. 改变旋转方向 (反向旋转)
    SMotor_SetSpeed(&stepper_motor, -60.0f);  // -60度/秒

    // 延时让电机运行一段时间
    DL_Common_delayCycles(32000000);  // 1秒延时

    // 5. 停止电机
    SMotor_SetSpeed(&stepper_motor, 0.0f);

    // 6. 在主循环中定期更新电机状态
    while(1) {
        SMotor_UpdateState(&stepper_motor);
        
        // 其他应用代码...
        
        DL_Common_delayCycles(320000);  // 10ms延时
    }
}

/**
 * @brief 获取步进电机当前位置
 * @return 当前位置 (度)
 */
float Get_Motor_Position(void)
{
    return stepper_motor.state.current_position;
}

/**
 * @brief 获取步进电机当前角速度
 * @return 当前角速度 (度/秒)
 */
float Get_Motor_Speed(void)
{
    return stepper_motor.state.angular_speed;
}
