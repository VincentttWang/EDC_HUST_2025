#ifndef SMOTOR_H
#define SMOTOR_H

#include "ti_msp_dl_config.h"

#define STEP_ANGULAR 1.8f
#define STEP_DIVISOR 32.0f // 32 steps per revolution

typedef enum {
    ANTI_CLOCKWISE = 0, // Counter-clockwise rotation
    CLOCKWISE = 1,      // Clockwise rotation
    SLEEP_SMOTOR = 2,   // Sleep mode
} SMOTOR_MODE;

typedef enum {
    SMOTOR_DIR_LOW = 0,  // 方向引脚低电平
    SMOTOR_DIR_HIGH = 1, // 方向引脚高电平
} SMOTOR_DIR_STATE;

typedef struct {
    SMOTOR_DIR_STATE Anti_Dir; // Anti-clockwise direction pin state
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
    GPIO_Regs *Dir_port; // GPIO port for direction pin
    uint32_t Dir_pin; // GPIO pin mask for direction pin
    GPTIMER_Regs *pwm_timer; // Timer instance for PWM
    DL_TIMER_CC_INDEX pwm_channel; // PWM channel index
    SMotorParameters parameters; // Motor parameters
    SMotorState state; // Motor state
    // Add any additional fields as needed
} SMotor;

void SMotor_Init(SMotor *motor,
                 GPIO_Regs *Dir_port, uint32_t Dir_pin,
                 GPTIMER_Regs *pwm_timer, DL_TIMER_CC_INDEX pwm_channel);
void SMotor_SetSpeed(SMotor *motor, float angular_speed);
void SMotor_Parameters_Init(SMotor *motor, SMOTOR_DIR_STATE Anti_Dir, float step_angular, float step_divisor);
void SMotor_UpdateState(SMotor *motor);
uint32_t GetClockFre(GPTIMER_Regs *timer);
uint32_t GetStepFrequency(float angular_speed, SMotor *motor);

#endif