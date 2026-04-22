/**
 * @file  StepMotor.c
 * @brief 步进电机 BSP 驱动实现，提供初始化、调速与状态更新
 */
#include "StepMotor.h"
#include <math.h>

BSP_Status SMotor_Init(SMotor *motor, GPIO_Regs *Dir_port, uint32_t Dir_pin,
                       GPTIMER_Regs *pwm_timer, DL_TIMER_CC_INDEX pwm_channel) {
    if (motor == NULL || pwm_timer == NULL)
        return BSP_ERR_NULL;

    motor->Dir_port = Dir_port;
    motor->Dir_pin = Dir_pin;
    motor->pwm_timer = pwm_timer;
    motor->pwm_channel = pwm_channel;
    motor->state.angular_speed = 0.0;
    motor->state.last_update_time = (uint32_t)-1;
    motor->state.current_position = 0.0;

    DL_GPIO_clearPins(motor->Dir_port, motor->Dir_pin);
    DL_TimerG_startCounter(motor->pwm_timer);
    DL_TimerG_setCaptureCompareValue(motor->pwm_timer, 0, motor->pwm_channel);
    return BSP_OK;
}

BSP_Status SMotor_ParamInit(SMotor *motor, SMOTOR_DIR_STATE Anti_Dir,
                            float step_angular, float step_divisor) {
    if (motor == NULL)
        return BSP_ERR_NULL;

    motor->parameters.Anti_Dir = Anti_Dir;
    motor->parameters.step_angular = step_angular;
    motor->parameters.step_divisor = step_divisor;
    return BSP_OK;
}

BSP_Status SMotor_SetSpeed(SMotor *motor, float angular_speed) {
    if (motor == NULL || motor->pwm_timer == NULL)
        return BSP_ERR_NULL;

    motor->state.angular_speed = angular_speed;

    if (fabsf(angular_speed) == 0.0f) {
        DL_TimerG_setCaptureCompareValue(motor->pwm_timer, 0, motor->pwm_channel);
        return BSP_OK;
    }

    if (angular_speed > 0) {
        if (motor->parameters.Anti_Dir == SMOTOR_DIR_HIGH)
            DL_GPIO_setPins(motor->Dir_port, motor->Dir_pin);
        else
            DL_GPIO_clearPins(motor->Dir_port, motor->Dir_pin);
    } else {
        if (motor->parameters.Anti_Dir == SMOTOR_DIR_HIGH)
            DL_GPIO_clearPins(motor->Dir_port, motor->Dir_pin);
        else
            DL_GPIO_setPins(motor->Dir_port, motor->Dir_pin);
        angular_speed = -angular_speed;
    }

    uint32_t tim_clk = SMotor_GetClockFreq(motor->pwm_timer);
    uint32_t target_frequency = SMotor_GetStepFreq(angular_speed, motor);
    uint32_t now_prescaler = 32 * 8 * 2;
    uint32_t target_arr = tim_clk / (target_frequency * now_prescaler) - 1;
    if (target_arr > 65535)
        target_arr = 65535;

    DL_TimerG_setLoadValue(motor->pwm_timer, target_arr);
    DL_TimerG_setCaptureCompareValue(motor->pwm_timer, target_arr / 2, motor->pwm_channel);
    return BSP_OK;
}

BSP_Status SMotor_UpdateState(SMotor *motor) {
    extern uint32_t tick;
    if (motor == NULL)
        return BSP_ERR_NULL;

    if (motor->state.last_update_time == (uint32_t)-1) {
        motor->state.last_update_time = tick;
        return BSP_OK;
    }
    uint32_t current_time = tick;
    motor->state.current_position += motor->state.angular_speed *
        (current_time - motor->state.last_update_time) * 1e-3;
    motor->state.last_update_time = current_time;
    return BSP_OK;
}

uint32_t SMotor_GetClockFreq(GPTIMER_Regs *timer) {
    if (timer == NULL)
        return 0;
    return 32000000;
}

uint32_t SMotor_GetStepFreq(float angular_speed, SMotor *motor) {
    if (motor == NULL)
        return 0;
    if (angular_speed < 0)
        angular_speed = -angular_speed;
    return (uint32_t)(angular_speed / motor->parameters.step_angular * motor->parameters.step_divisor);
}
