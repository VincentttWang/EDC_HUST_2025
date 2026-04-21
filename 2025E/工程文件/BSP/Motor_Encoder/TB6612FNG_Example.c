/*
 * TB6612FNG_Example.c
 * 
 * TB6612FNG电机驱动在TI MSPM0G3507上的使用示例
 * 
 * 硬件连接：
 * 电机驱动板TB6612FNG连接：
 * - AIN1 -> PA7  (正向控制引脚)
 * - AIN2 -> PA6  (反向控制引脚)  
 * - PWMA -> PB7  (PWM控制引脚，连接到TIMG7_CCP0)
 * - STBY -> 3.3V (使能引脚，接高电平)
 * - VCC -> 3.3V
 * - GND -> GND
 * - VM -> 电机电源(6-12V)
 * - Motor A+ -> 电机正极
 * - Motor A- -> 电机负极
 */

#include "tb6612fng.h"

// 定义电机实例
Motor motor_left, motor_right;

// 电机初始化示例
void Motor_Example_Init(void)
{
    // 初始化系统配置
    SYSCFG_DL_init();
    
    // 初始化左电机参数
    // 参数：减速比30:1，满速3000RPM，轮径66mm
    Motor_Param_Init(&motor_left, 30.0, 3000.0, 66);
    
    // 初始化左电机硬件接口
    // AIN1=PA7, AIN2=PA6, PWM=TIMG7_CCP0, 初始占空比=0
    Motor_UI_Init(&motor_left, 
                  (GPIO_Regs*)GPIOA_BASE, DL_GPIO_PIN_7,  // AIN1
                  (GPIO_Regs*)GPIOA_BASE, DL_GPIO_PIN_6,  // AIN2  
                  Motor_INST, DL_TIMER_CC_0_INDEX,        // PWM
                  0);                                     // 初始占空比
    
    // 如果有右电机，可以类似初始化
    Motor_Param_Init(&motor_right, 30.0, 3000.0, 66);
    Motor_UI_Init(&motor_right,
                  (GPIO_Regs*)GPIOA_BASE, DL_GPIO_PIN_9,  // BIN1
                  (GPIO_Regs*)GPIOA_BASE, DL_GPIO_PIN_8,  // BIN2
                  SMotor_INST, DL_TIMER_CC_0_INDEX,       // PWM
                  0);                                     // 初始占空比
}

// 基本控制示例
void Motor_Basic_Control_Example(void)
{
    // 方法1：直接设置占空比(0-1000)
    Motor_UI_Set(FOR, 500, &motor_left);    // 正转，50%占空比
    delay_ms(2000);                          // 运行2秒
    
    Motor_UI_Set(BACK, 300, &motor_left);   // 反转，30%占空比  
    delay_ms(2000);                          // 运行2秒
    
    Motor_UI_Set(BREAK, 0, &motor_left);    // 刹车停止
    delay_ms(1000);                          // 停止1秒
    
    Motor_UI_Set(SLIDE, 0, &motor_left);    // 滑行停止
    delay_ms(1000);
}

// 速度控制示例  
void Motor_Speed_Control_Example(void)
{
    // 方法2：按速度控制(m/s)
    MotorSet(FOR, 0.5, &motor_left);        // 正转，0.5m/s
    delay_ms(3000);                          // 运行3秒
    
    MotorSet(BACK, 0.3, &motor_left);       // 反转，0.3m/s
    delay_ms(3000);                          // 运行3秒
    
    MotorSet(SLIDE, 0, &motor_left);        // 停止
    delay_ms(1000);
}

// 双电机差速控制示例
void Motor_Differential_Control_Example(void)
{
    // 直行：两电机同速
    MotorSet(FOR, 0.4, &motor_left);
    MotorSet(FOR, 0.4, &motor_right);
    delay_ms(2000);
    
    // 右转：左快右慢
    MotorSet(FOR, 0.5, &motor_left);
    MotorSet(FOR, 0.2, &motor_right);
    delay_ms(1000);
    
    // 左转：右快左慢
    MotorSet(FOR, 0.2, &motor_left);  
    MotorSet(FOR, 0.5, &motor_right);
    delay_ms(1000);
    
    // 原地右转：左正转右反转
    MotorSet(FOR, 0.3, &motor_left);
    MotorSet(BACK, 0.3, &motor_right);
    delay_ms(1000);
    
    // 停止
    MotorSet(SLIDE, 0, &motor_left);
    MotorSet(SLIDE, 0, &motor_right);
}

// 速度转换测试
void Motor_Speed_Conversion_Test(void)
{
    // 测试速度到占空比的转换
    double test_speeds[] = {0.1, 0.2, 0.5, 1.0}; // m/s
    int duty;
    
    for(int i = 0; i < 4; i++)
    {
        duty = speed_to_duty(test_speeds[i], &motor_left);
        // Debug_printf("Speed: %.1f m/s -> Duty: %d\n", test_speeds[i], duty);
        
        // 实际运行测试
        MotorSet(FOR, test_speeds[i], &motor_left);
        delay_ms(2000);
        MotorSet(SLIDE, 0, &motor_left);
        delay_ms(500);
    }
}

// 简单的延时函数
void delay_ms(uint32_t ms)
{
    uint32_t i, j;
    for(i = 0; i < ms; i++)
        for(j = 0; j < 1000; j++); // 粗略延时，实际应使用定时器
}

/*
 * SysConfig配置说明：
 * 
 * 1. GPIO配置：
 *    - PA6, PA7, PA8, PA9 配置为输出引脚
 *    - 用于控制电机方向
 * 
 * 2. PWM配置：
 *    - Motor (TIMG7): PWM频率1kHz, 占空比0-100%
 *    - SMotor (TIMG6): PWM频率1kHz, 占空比0-100%  
 *    - PWM引脚：PB7(TIMG7_CCP0), PA3(TIMG6_CCP0)
 * 
 * 3. 定时器周期计算：
 *    - 32MHz系统时钟，1kHz PWM频率
 *    - 预分频 = 31 (32分频)  
 *    - 周期 = 1000 (对应占空比0-1000)
 */
