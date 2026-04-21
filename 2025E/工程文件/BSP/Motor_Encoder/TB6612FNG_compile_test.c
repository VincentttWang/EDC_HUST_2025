/*
 * 编译测试文件 - 验证重构后的TB6612FNG电机驱动是否能正确编译
 */

#include "tb6612fng.h"

// 模拟的配置定义（实际应该在ti_msp_dl_config.h中）
#ifndef Motor_INST
#define Motor_INST ((GPTIMER_Regs*)0x40086000UL)  // TIMG7基地址
#endif

#ifndef SMotor_INST  
#define SMotor_INST ((GPTIMER_Regs*)0x40085000UL) // TIMG6基地址
#endif

#ifndef GPIOA_BASE
#define GPIOA_BASE 0x400A0000UL
#endif

#ifndef GPIOB_BASE
#define GPIOB_BASE 0x400A2000UL  
#endif

// 测试电机参数初始化
void test_motor_param_init(void)
{
    Motor test_motor;
    
    // 测试电机参数初始化
    Motor_Param_Init(&test_motor, 30.0, 3000.0, 66);
    
    // 验证参数设置
    assert(test_motor.param.reduce == 30.0);
    assert(test_motor.param.full_speed_rpm == 3000.0);
    assert(test_motor.param.wheel_diameter == 66);
}

// 测试电机硬件初始化
void test_motor_ui_init(void) 
{
    Motor test_motor;
    
    // 测试硬件接口初始化
    Motor_UI_Init(&test_motor,
                  (GPIO_Regs*)GPIOA_BASE, DL_GPIO_PIN_7,  // AIN1
                  (GPIO_Regs*)GPIOA_BASE, DL_GPIO_PIN_6,  // AIN2
                  Motor_INST, DL_TIMER_CC_0_INDEX,        // PWM
                  0);                                     // 初始占空比
    
    // 验证参数设置
    assert(test_motor.p.port == (GPIO_Regs*)GPIOA_BASE);
    assert(test_motor.p.pin == DL_GPIO_PIN_7);
    assert(test_motor.n.port == (GPIO_Regs*)GPIOA_BASE);  
    assert(test_motor.n.pin == DL_GPIO_PIN_6);
    assert(test_motor.speed.pwm_timer == Motor_INST);
    assert(test_motor.speed.pwm_channel == DL_TIMER_CC_0_INDEX);
    assert(test_motor.speed.current_duty == 0);
}

// 测试电机控制
void test_motor_control(void)
{
    Motor test_motor;
    
    // 先初始化
    Motor_Param_Init(&test_motor, 30.0, 3000.0, 66);
    Motor_UI_Init(&test_motor,
                  (GPIO_Regs*)GPIOA_BASE, DL_GPIO_PIN_7,
                  (GPIO_Regs*)GPIOA_BASE, DL_GPIO_PIN_6,
                  Motor_INST, DL_TIMER_CC_0_INDEX,
                  0);
    
    // 测试不同控制模式
    Motor_UI_Set(FOR, 500, &test_motor);
    assert(test_motor.speed.current_duty == 500);
    
    Motor_UI_Set(BACK, 300, &test_motor);  
    assert(test_motor.speed.current_duty == 300);
    
    Motor_UI_Set(BREAK, 0, &test_motor);
    assert(test_motor.speed.current_duty == 0);
    
    Motor_UI_Set(SLIDE, 0, &test_motor);
    assert(test_motor.speed.current_duty == 0);
}

// 测试速度转换
void test_speed_conversion(void)
{
    Motor test_motor;
    
    // 初始化电机参数
    Motor_Param_Init(&test_motor, 30.0, 3000.0, 66);
    
    // 测试速度到占空比转换
    int duty1 = speed_to_duty(0.5, &test_motor);  // 0.5m/s
    int duty2 = speed_to_duty(1.0, &test_motor);  // 1.0m/s
    int duty3 = speed_to_duty(0.0, &test_motor);  // 0m/s
    
    // 验证转换结果合理性
    assert(duty1 > 0 && duty1 < 1000);
    assert(duty2 > duty1);  // 更高速度应该有更高占空比
    assert(duty3 == 0);     // 零速度应该是零占空比
    
    // 测试负速度处理
    int duty4 = speed_to_duty(-0.5, &test_motor);
    assert(duty4 == duty1);  // 负速度应该转为正值
}

// 测试高级电机控制
void test_motor_set(void)
{
    Motor test_motor;
    
    // 初始化
    Motor_Param_Init(&test_motor, 30.0, 3000.0, 66);
    Motor_UI_Init(&test_motor,
                  (GPIO_Regs*)GPIOA_BASE, DL_GPIO_PIN_7,
                  (GPIO_Regs*)GPIOA_BASE, DL_GPIO_PIN_6, 
                  Motor_INST, DL_TIMER_CC_0_INDEX,
                  0);
    
    // 测试速度控制
    MotorSet(FOR, 0.5, &test_motor);   // 正转0.5m/s
    MotorSet(BACK, 0.3, &test_motor);  // 反转0.3m/s
    MotorSet(SLIDE, 0, &test_motor);   // 停止
    
    // 测试负速度处理（应该自动变为反转）
    MotorSet(FOR, -0.4, &test_motor);  // 负速度应该变为反转
}

// 简单的断言实现
void assert(int condition)
{
    if (!condition) {
        // 在实际应用中，这里可以触发错误处理
        while(1); // 停止程序
    }
}

int main(void)
{
    // 运行所有测试
    test_motor_param_init();
    test_motor_ui_init();
    test_motor_control();
    test_speed_conversion();
    test_motor_set();
    
    // 所有测试通过
    return 0;
}
