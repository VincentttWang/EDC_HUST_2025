#ifndef SMOTOR_H
#define SMOTOR_H

#include "main.h"

#define STEP_ANGULAR 1.8f
#define STEP_DIVISOR 32.0f // 32 steps per revolution

typedef enum {
    ANTI_CLOCKWISE = 0, // Counter-clockwise rotation
    CLOCKWISE = 1,      // Clockwise rotation
    SLEEP_SMOTOR = 2,   // Sleep mode
} SMOTOR_MODE;

typedef struct {
    GPIO_PinState Anti_Dir; // Anti-clockwise direction pin state
    float step_angular; // Step angular speed in degrees
    float step_divisor; // Step divisor for speed calculation
} SMotorParameters;

typedef struct{
    double angular_speed; // Angular speed in degrees per second
    uint32_t last_update_time; // Last update time in milliseconds
    double current_position; // Current position in degrees
    SMOTOR_MODE mode; // Current motor mode
} SMotorState;

typedef struct {
    GPIO_TypeDef *Dir_port; // GPIO port for direction pin
    uint16_t Dir_pin; // GPIO pin for direction pin
    TIM_HandleTypeDef *pwm_timer; // Timer handle for PWM
    uint32_t pwm_channel; // PWM channel
    SMotorParameters parameters; // Motor parameters
    SMotorState state; // Motor state
    // Add any additional fields as needed
} SMotor;

void SMotor_Init(SMotor *motor,
                 GPIO_TypeDef *Dir_port, uint16_t Dir_pin,
                 TIM_HandleTypeDef *pwm_timer, uint32_t pwm_channel);
void SMotor_SetSpeed(SMotor *motor, float angular_speed);
void SMotor_Parameters_Init(SMotor *motor, GPIO_PinState Anti_Dir, float step_angular, float step_divisor);
void SMotor_UpdateState(SMotor *motor);
uint32_t GetClockFre(TIM_HandleTypeDef *htim);
uint32_t GetStepFrequency(float angular_speed, SMotor *motor);

#endif