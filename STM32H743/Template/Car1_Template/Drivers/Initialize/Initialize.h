#ifndef INITIALIZE_H
#define INITIALIZE_H

// �˶�ѧͷ�ļ�������������ֱ��ת�亯��
#include "kinematics.h"
#include "AllHeader.h"

// Parameters for the encoder
#define PPR 13  // The Pulses Per Revolution of encoder

// Parameters for the motor
#define REDUCE 28
#define FULL_SPEED_RPM 300

// Parameters for the wheel
#define WHEEL_DIAMETER 65 // The diameter of the wheel in mm
#define WHEEL_DIS      115

// ====================��???�����������궨??====================
// �������д���ĳ�ʼ������

// ���ֵ����ʼ������ (��ӦMotorInit(&Right, ...))
#define RIGHT_MOTOR_IN1_PORT    GPIOC
#define RIGHT_MOTOR_IN1_PIN     GPIO_PIN_5
#define RIGHT_MOTOR_IN2_PORT    GPIOB
#define RIGHT_MOTOR_IN2_PIN     GPIO_PIN_1
#define RIGHT_MOTOR_PWM_TIMER   &htim5
#define RIGHT_MOTOR_PWM_CHANNEL TIM_CHANNEL_1
#define RIGHT_MOTOR_INIT_DUTY   0

// ���ֵ����ʼ������ (��ӦMotorInit(&Left, ...))
#define LEFT_MOTOR_IN1_PORT     GPIOC
#define LEFT_MOTOR_IN1_PIN      GPIO_PIN_4
#define LEFT_MOTOR_IN2_PORT     GPIOB
#define LEFT_MOTOR_IN2_PIN      GPIO_PIN_0
#define LEFT_MOTOR_PWM_TIMER    &htim5
#define LEFT_MOTOR_PWM_CHANNEL  TIM_CHANNEL_2
#define LEFT_MOTOR_INIT_DUTY    0

// LRInit�������� (��������ʼ��)
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
extern int Digital[8]; // Declare an array for digital sensor readings
extern char error_message[100]; // Buffer for error messages
extern double GyroscopeChannelData[10]; // Array to hold gyroscope data

void MECInit();
void LMotorSet(MOVETYPE type, uint16_t duty);
void RMotorSet(MOVETYPE type, uint16_t duty);

void LSet(int16_t duty);
void RSet(int16_t duty);

void Break(); // Set both motors to break mode
// This function sets both motors to break mode, stopping them quickly

// RotationAngles getRotationAngles(); // Get the current rotation angles from the MS601M sensor
float getYaw(); // Get the current yaw angle from the MS601M sensor
float getWz(); // Get the current angular velocity (wz) from the MS601M sensor
float CalibrateYawOffset(); // Calibrate the yaw offset

// Get the current data and update the car state
void UpdateData(); // Update the current data with the latest sensor readings
void UpdateData_Car(); // Update the car state with the latest sensor readings

void error_handler(void); // Handle errors by stopping motors and displaying error messages

#ifdef __cplusplus
}
#endif
#endif /* INITIALIZE_H */