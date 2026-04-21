#ifndef INITIALIZE_H
#define INITIALIZE_H

// 运动学头文件，包括常见的直行转弯函数
#include "kinematics.h"
#include "AllHeader.h"

// Parameters for the encoder
#define PPR 13  // The Pulses Per Revolution of encoder

// Parameters for the motor
#define REDUCE 20
#define FULL_SPEED_RPM 300

// Parameters for the wheel
#define WHEEL_DIAMETER 48 // The diameter of the wheel in mm
#define WHEEL_DIS      130

// ====================初始化函数参数宏定义====================
// 基于现有代码的初始化参数

// 右轮电机初始化参数 (对应MotorInit(&Right, ...))
#define RIGHT_MOTOR_IN1_PORT    GPIOC
#define RIGHT_MOTOR_IN1_PIN     GPIO_PIN_5
#define RIGHT_MOTOR_IN2_PORT    GPIOB
#define RIGHT_MOTOR_IN2_PIN     GPIO_PIN_1
#define RIGHT_MOTOR_PWM_TIMER   &htim5
#define RIGHT_MOTOR_PWM_CHANNEL TIM_CHANNEL_1
#define RIGHT_MOTOR_INIT_DUTY   0

// 左轮电机初始化参数 (对应MotorInit(&Left, ...))
#define LEFT_MOTOR_IN1_PORT     GPIOC
#define LEFT_MOTOR_IN1_PIN      GPIO_PIN_4
#define LEFT_MOTOR_IN2_PORT     GPIOB
#define LEFT_MOTOR_IN2_PIN      GPIO_PIN_0
#define LEFT_MOTOR_PWM_TIMER    &htim5
#define LEFT_MOTOR_PWM_CHANNEL  TIM_CHANNEL_2
#define LEFT_MOTOR_INIT_DUTY    0

// LRInit函数参数 (编码器初始化)
#define LEFT_ENCODER_TIMER      &htim3
#define LEFT_ENCODER_CHANNEL_A  TIM_CHANNEL_2
#define LEFT_ENCODER_CHANNEL_B  TIM_CHANNEL_1
#define RIGHT_ENCODER_TIMER     &htim1
#define RIGHT_ENCODER_CHANNEL_A TIM_CHANNEL_1
#define RIGHT_ENCODER_CHANNEL_B TIM_CHANNEL_2
#define ENCODER_REAL_TIMER      &htim7


#ifdef __cplusplus
extern "C" {
#endif
extern CarState car; // Declare car state for kinematics
extern Data current_data; // Declare current data for kinematics

void MEInit(Motor* L, Motor* R);
void LMotorSet(MOVETYPE type, uint16_t duty);
void RMotorSet(MOVETYPE type, uint16_t duty);

void LSet(int16_t duty);
void RSet(int16_t duty);

void Break(); // Set both motors to break mode
// This function sets both motors to break mode, stopping them quickly

bool isInTheYaw(float targetYaw, float tolerance);

float getYaw(); // 获取当前的偏航角
float getWz(); // 获取当前的角速度
// 获取数据函数声明
void UpdateData(); // 更新当前数据
void UpdateData_Car(); // 更新汽车状态数据

void Back(float theta); // 后退函数，theta为目标偏航角

#endif /* INITIALIZE_H */