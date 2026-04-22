#ifndef INITIALIZE_H
#define INITIALIZE_H

// �˶�ѧͷ�ļ�������������ֱ��ת�亯��
#include "kinematics.h"
#include "AllHeader.h"
//激光通信头文件
#include "Laser_USART.h"

// Parameters for the encoder
#define PPR 13  // The Pulses Per Revolution of encoder

// Parameters for the motor
#define REDUCE 28
#define FULL_SPEED_RPM 300

// Parameters for the wheel
#define WHEEL_DIAMETER 65 // The diameter of the wheel in mm
#define WHEEL_DIS      115

// ====================电机和编码器参数定义====================
// 根据重构的驱动修改初始化参数

// 右电机初始化参数 (对应Motor_UI_Init(&Right, ...))
#define RIGHT_MOTOR_IN1_PORT    Motor_IO_AIN1_PORT
#define RIGHT_MOTOR_IN1_PIN     Motor_IO_AIN1_PIN
#define RIGHT_MOTOR_IN2_PORT    Motor_IO_AIN2_PORT
#define RIGHT_MOTOR_IN2_PIN     Motor_IO_AIN2_PIN
#define RIGHT_MOTOR_PWM_TIMER   Motor_INST
#define RIGHT_MOTOR_PWM_CHANNEL DL_TIMER_CC_0_INDEX
#define RIGHT_MOTOR_INIT_DUTY   0

// 左电机初始化参数 (对应Motor_UI_Init(&Left, ...))
#define LEFT_MOTOR_IN1_PORT     Motor_IO_BIN1_PORT
#define LEFT_MOTOR_IN1_PIN      Motor_IO_BIN1_PIN
#define LEFT_MOTOR_IN2_PORT     Motor_IO_BIN2_PORT
#define LEFT_MOTOR_IN2_PIN      Motor_IO_BIN2_PIN
#define LEFT_MOTOR_PWM_TIMER    Motor_INST
#define LEFT_MOTOR_PWM_CHANNEL  DL_TIMER_CC_1_INDEX
#define LEFT_MOTOR_INIT_DUTY    0


#ifdef __cplusplus
extern "C" {
#endif

extern Data current_data; // Declare current data for kinematics
extern uint8_t Digital[8]; // Declare an array for digital sensor readings
extern char error_message[100]; // Buffer for error messages

extern uint32_t tick; // Global tick counter for timing

extern float sInedge; // Variable to track distance traveled in the edge
extern int edge; // Variable to track the current edge for target coordinate calculation
extern int isturn; // Variable to track if the robot is turning

extern char message[100]; // Buffer for messages

extern int Laser_error ;
extern int Rect_error  ;
// CANMV相关数组
extern uint16_t Laser_Loc[Laser_RX_Num / 2] ;		// 激光位置数组,0-3为值 01 target xy, 23 real xy
extern uint16_t  Rect_Loc[Rect_RX_Num  / 2] ;		// 矩形位置数组,0-3为值

void MECInit();
void LMotorSet(MOVETYPE type, uint16_t duty);
void RMotorSet(MOVETYPE type, uint16_t duty);

void LSet(int16_t duty);
void RSet(int16_t duty);

void Break(); // Set both motors to break mode
// This function sets both motors to break mode, stopping them quickly

void UpdateSInedge(void);

void error_handler(void); // Handle errors by stopping motors and displaying error messages
void delay_cycles(uint32_t cycles); // Simple delay function for TI platform

#ifdef __cplusplus
}
#endif
#endif /* INITIALIZE_H */