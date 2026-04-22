#include "Tb6612fng.h"

BSP_Status Motor_Init(Motor *M,
        GPIO_Regs *p_port, uint32_t p_pin,
        GPIO_Regs *n_port, uint32_t n_pin,
        GPTIMER_Regs *pwm_timer, DL_TIMER_CC_INDEX pwm_channel,
        uint16_t initial_duty)
{
    if (M == NULL || pwm_timer == NULL)
        return BSP_ERR_NULL;

    M->p.port = p_port;
    M->p.pin = p_pin;
    M->n.port = n_port;
    M->n.pin = n_pin;
    M->speed.pwm_timer = pwm_timer;
    M->speed.pwm_channel = pwm_channel;
    M->speed.current_duty = initial_duty;

    DL_GPIO_clearPins(M->n.port, M->n.pin);
    DL_GPIO_clearPins(M->p.port, M->p.pin);
    DL_TimerG_startCounter(M->speed.pwm_timer);
    DL_TimerG_setCaptureCompareValue(M->speed.pwm_timer, M->speed.current_duty, M->speed.pwm_channel);
    return BSP_OK;
}

BSP_Status Motor_ParamInit(Motor *M,
        double reduce, double full_speed_rpm, int wheel_diameter)
{
    if (M == NULL)
        return BSP_ERR_NULL;

    M->param.reduce = reduce;
    M->param.full_speed_rpm = full_speed_rpm;
    M->param.wheel_diameter = wheel_diameter;
    return BSP_OK;
}

BSP_Status Motor_SetDuty(MotorMoveType type, uint16_t duty, Motor *M)
{
    if (M == NULL)
        return BSP_ERR_NULL;

    switch (type) {
        case MOTOR_FORWARD:
            DL_GPIO_setPins(M->p.port, M->p.pin);
            DL_GPIO_clearPins(M->n.port, M->n.pin);
            break;
        case MOTOR_BACKWARD:
            DL_GPIO_clearPins(M->p.port, M->p.pin);
            DL_GPIO_setPins(M->n.port, M->n.pin);
            break;
        case MOTOR_BRAKE:
            DL_GPIO_setPins(M->p.port, M->p.pin);
            DL_GPIO_setPins(M->n.port, M->n.pin);
            break;
        case MOTOR_SLIDE:
            DL_GPIO_clearPins(M->p.port, M->p.pin);
            DL_GPIO_clearPins(M->n.port, M->n.pin);
            break;
        case MOTOR_SLEEP:
            break;
    }
    M->speed.current_duty = duty;
    DL_TimerG_setCaptureCompareValue(M->speed.pwm_timer, M->speed.current_duty, M->speed.pwm_channel);
    return BSP_OK;
}

int Motor_SpeedToDuty(double speed, Motor *M)
{
    if (M == NULL)
        return 0;
    double max_speed = M->param.full_speed_rpm / 60.0 * M->param.wheel_diameter * 3.14159 / 1000.0;
    if (max_speed <= 0)
        return 0;
    int duty = (int)(speed / max_speed * 1000.0);
    if (duty > 1000) duty = 1000;
    if (duty < -1000) duty = -1000;
    return duty;
}

BSP_Status Motor_SetSpeed(MotorMoveType type, double speed, Motor *M)
{
    if (M == NULL)
        return BSP_ERR_NULL;
    int duty = Motor_SpeedToDuty(speed, M);
    if (duty < 0) duty = -duty;
    return Motor_SetDuty(type, (uint16_t)duty, M);
}
