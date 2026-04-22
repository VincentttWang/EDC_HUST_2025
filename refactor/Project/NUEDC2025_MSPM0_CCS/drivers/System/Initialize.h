#ifndef INITIALIZE_H
#define INITIALIZE_H

#include <stdint.h>
#include "Kinematics.h"
#include "Tb6612fng.h"
#include "ti_msp_dl_config.h"
#include "LaserUsart.h"

#define PPR 13
#define REDUCE 28
#define FULL_SPEED_RPM 300

#define WHEEL_DIAMETER 65
#define WHEEL_DIS      115

#define RIGHT_MOTOR_IN1_PORT    Motor_IO_AIN1_PORT
#define RIGHT_MOTOR_IN1_PIN     Motor_IO_AIN1_PIN
#define RIGHT_MOTOR_IN2_PORT    Motor_IO_AIN2_PORT
#define RIGHT_MOTOR_IN2_PIN     Motor_IO_AIN2_PIN
#define RIGHT_MOTOR_PWM_TIMER   Motor_INST
#define RIGHT_MOTOR_PWM_CHANNEL DL_TIMER_CC_0_INDEX
#define RIGHT_MOTOR_INIT_DUTY   0

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

extern Data current_data;
extern uint8_t Digital[8];
extern char error_message[100];

extern uint32_t tick;

extern float sInedge;
extern int edge;
extern int isturn;

extern char message[100];

extern CanMV_Error Laser_error;
extern CanMV_Error Rect_error;
extern uint16_t Laser_Loc[Laser_RX_Num / 2];
extern uint16_t Rect_Loc[Rect_RX_Num / 2];

void Motor_SystemInit(void);
void Motor_SetLeftRaw(MotorMoveType type, uint16_t duty);
void Motor_SetRightRaw(MotorMoveType type, uint16_t duty);

void Motor_SetLeft(int16_t duty);
void Motor_SetRight(int16_t duty);

void Motor_Brake(void);

void UpdateSInedge(void);

void error_handler(void);

#ifdef __cplusplus
}
#endif
#endif /* INITIALIZE_H */
