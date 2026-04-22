/**
 * @file Init_SMotor.c
 * @brief TI MSPM0G3507平台双轴步进电机初始化和控制
 * @author 移植自STM32平台
 * @date 2024
 * 
 * 本文件提供YAW(偏航)和PITCH(俯仰)双轴步进电机的统一控制接口
 * 适用于云台、摄像头支架等需要两轴控制的应用场景
 */

#include "Init_SMotor.h"
#include "AllHeader.h"

// 全局步进电机实例
SMotor yawMotor = {0};      // YAW偏航电机
SMotor pitchMotor = {0};    // PITCH俯仰电机

void YP_SMotor_Init(void) {
    // 初始化YAW步进电机 (偏航控制)
    DL_GPIO_setPins(SMotor_IO_PORT, SMotor_IO_EN1_PIN);
    DL_GPIO_setPins(SMotor_IO_PORT, SMotor_IO_EN2_PIN);
    SMotor_Init(&yawMotor, 
                YAW_SMOTOR_DIR_PORT,        // 方向控制GPIO端口
                YAW_SMOTOR_DIR_PIN,         // 方向控制引脚
                YAW_SMOTOR_PWM_TIMER,       // PWM定时器
                YAW_SMOTOR_PWM_CHANNEL);    // PWM通道
    
    // 设置YAW电机参数
    SMotor_Parameters_Init(&yawMotor, 
                          YAW_SMOTOR_ANTI_DIR,       // 反向状态
                          YAW_SMOTOR_STEP_ANGULAR,   // 步进角度
                          YAW_SMOTOR_STEP_DIVISOR);  // 细分数
    
    // 初始化PITCH步进电机 (俯仰控制)
    SMotor_Init(&pitchMotor, 
                PITCH_SMOTOR_DIR_PORT,      // 方向控制GPIO端口
                PITCH_SMOTOR_DIR_PIN,       // 方向控制引脚
                PITCH_SMOTOR_PWM_TIMER,     // PWM定时器
                PITCH_SMOTOR_PWM_CHANNEL);  // PWM通道
    
    // 设置PITCH电机参数
    SMotor_Parameters_Init(&pitchMotor, 
                          PITCH_SMOTOR_ANTI_DIR,     // 反向状态
                          PITCH_SMOTOR_STEP_ANGULAR, // 步进角度
                          PITCH_SMOTOR_STEP_DIVISOR); // 细分数
}

void YP_SMotor_SetSpeed(float yaw_speed, float pitch_speed) {
    // 设置YAW电机速度 (偏航控制)
	if(pitchMotor.state.current_position > 45 || pitchMotor.state.current_position < -45) {
        SMotor_SetSpeed(&pitchMotor,0);
				SMotor_SetSpeed(&yawMotor, 0);
        return; // 如果俯仰角度超过限制，停止电机
    }
		SMotor_SetSpeed(&yawMotor, yaw_speed);
    SMotor_SetSpeed(&pitchMotor, pitch_speed);
}

void YP_SMotor_UpdateState(void) {
    // 更新两个电机的状态
    SMotor_UpdateState(&yawMotor);
    SMotor_UpdateState(&pitchMotor);
}

double GetYaw(void) {
    // 返回YAW电机当前位置 (偏航角度)
    return (double)yawMotor.state.current_position;
}

double GetPitch(void) {
    // 返回PITCH电机当前位置 (俯仰角度)
    return (double)pitchMotor.state.current_position;
}