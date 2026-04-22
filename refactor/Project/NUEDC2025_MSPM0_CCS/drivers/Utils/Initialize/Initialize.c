#include "Initialize.h"
#include "AllHeader.h"

Motor g_motor_left, g_motor_right;
CarState car;
Data current_data;
uint8_t Digital[8];
char error_message[100];
double GyroscopeChannelData[10] = {0};
float sInedge = 0.0f;
int edge = 0;
char message[100];

void Motor_SystemInit(void)
{
    Motor_Init(&g_motor_left, LEFT_MOTOR_IN1_PORT, LEFT_MOTOR_IN1_PIN,
               LEFT_MOTOR_IN2_PORT, LEFT_MOTOR_IN2_PIN,
               LEFT_MOTOR_PWM_TIMER, LEFT_MOTOR_PWM_CHANNEL, LEFT_MOTOR_INIT_DUTY);

    Motor_Init(&g_motor_right, RIGHT_MOTOR_IN1_PORT, RIGHT_MOTOR_IN1_PIN,
               RIGHT_MOTOR_IN2_PORT, RIGHT_MOTOR_IN2_PIN,
               RIGHT_MOTOR_PWM_TIMER, RIGHT_MOTOR_PWM_CHANNEL, RIGHT_MOTOR_INIT_DUTY);
}

void Motor_SetLeftRaw(MotorMoveType type, uint16_t duty)
{
    Motor_SetDuty(type, duty, &g_motor_left);
}

void Motor_SetRightRaw(MotorMoveType type, uint16_t duty)
{
    Motor_SetDuty(type, duty, &g_motor_right);
}

void Motor_SetLeft(int16_t duty)
{
    MotorMoveType type = MOTOR_FORWARD;
    if (duty < 0) {
        type = MOTOR_BACKWARD;
        duty = -duty;
    }
    if (duty > 1000)
        duty = 1000;
    Motor_SetLeftRaw(type, duty);
}

void Motor_SetRight(int16_t duty)
{
    MotorMoveType type = MOTOR_FORWARD;
    if (duty < 0) {
        type = MOTOR_BACKWARD;
        duty = -duty;
    }
    if (duty > 1000)
        duty = 1000;
    Motor_SetRightRaw(type, duty);
}

void Motor_Brake(void)
{
    Motor_SetLeftRaw(MOTOR_BRAKE, 0);
    Motor_SetRightRaw(MOTOR_BRAKE, 0);
}

void error_handler(void)
{
    Motor_Brake();
    OLED_Clear();
    OLED_ShowString(0, 0, error_message, 8);
    while (1)
        ;
}

void UpdateSInedge(void)
{
    static uint32_t last_time = 0;
    uint32_t now = tick;

    if (now - last_time >= 10) {
        float dt = (now - last_time) * 1e-3f;
        last_time = now;
        sInedge += Encoder_GetSpeed() * dt;
    }
}
