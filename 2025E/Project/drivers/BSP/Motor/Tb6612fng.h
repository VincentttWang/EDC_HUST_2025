/**
 * @file  Tb6612fng.h
 * @brief TB6612FNG 直流电机驱动接口，提供 PWM 调速与方向控制
 */
#ifndef TB6612FNG_H
#define TB6612FNG_H

#include "ti_msp_dl_config.h"
#include "BspCommon.h"

typedef enum {
    MOTOR_FORWARD = 0,
    MOTOR_BACKWARD = 1,
    MOTOR_BRAKE = 2,
    MOTOR_SLIDE = 3,
    MOTOR_SLEEP = 4,
} MotorMoveType;

typedef struct {
    GPIO_Regs *port;
    uint32_t pin;
} GPIO_Pin;

typedef struct {
    GPTIMER_Regs *pwm_timer;
    DL_TIMER_CC_INDEX pwm_channel;
    uint16_t current_duty;
} MotorPWM;

typedef struct {
    double reduce;
    double full_speed_rpm;
    int wheel_diameter;
} MotorParam;

typedef struct {
    GPIO_Pin p;
    GPIO_Pin n;
    MotorPWM speed;
    MotorParam param;
} Motor;

#ifdef __cplusplus
extern "C" {
#endif

BSP_Status Motor_Init(Motor *M,
        GPIO_Regs *p_port, uint32_t p_pin,
        GPIO_Regs *n_port, uint32_t n_pin,
        GPTIMER_Regs *pwm_timer, DL_TIMER_CC_INDEX pwm_channel,
        uint16_t initial_duty);

BSP_Status Motor_ParamInit(Motor *M,
        double reduce, double full_speed_rpm, int wheel_diameter);

BSP_Status Motor_SetDuty(MotorMoveType type, uint16_t duty, Motor *M);

BSP_Status Motor_SetSpeed(MotorMoveType type, double speed, Motor *M);

int Motor_SpeedToDuty(double speed, Motor *M);

#ifdef __cplusplus
}
#endif

#endif /* TB6612FNG_H */
