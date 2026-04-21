#include "SMotor.h"

void SMotor_Init(SMotor *motor, GPIO_TypeDef *Dir_port, uint16_t Dir_pin,
                 TIM_HandleTypeDef *pwm_timer, uint32_t pwm_channel) {
    if (motor == NULL || pwm_timer == NULL) {
        #ifdef INITIALIZE_H
        sprintf(error_message, "SMotor_Init: motor or pwm_timer is NULL");
        Error_Handler();
        #endif
        return;
    }
    motor->Dir_port = Dir_port;
    motor->Dir_pin = Dir_pin;
    motor->pwm_timer = pwm_timer;
    motor->pwm_channel = pwm_channel;
    motor->state.angular_speed = 0.0;
    motor->state.last_update_time = -1; // Initialize to an invalid time
    motor->state.current_position = 0.0;

    // Initialize GPIO pins
    HAL_GPIO_WritePin(motor->Dir_port, motor->Dir_pin, GPIO_PIN_RESET);
     HAL_TIM_PWM_Start(motor->pwm_timer, motor->pwm_channel); // Start PWM
    __HAL_TIM_SET_COMPARE(motor->pwm_timer, motor->pwm_channel, 0); // Set initial PWM duty cycle to 0
}

void SMotor_Parameters_Init(SMotor *motor, GPIO_PinState Anti_Dir, float step_angular, float step_divisor) {
    if (motor == NULL) {
        #ifdef INITIALIZE_H
        sprintf(error_message, "SMotor_Parameters_Init: motor is NULL");
        Error_Handler();
        #endif
        return;
    }
    motor->parameters.Anti_Dir = Anti_Dir;
    motor->parameters.step_angular = step_angular;
    motor->parameters.step_divisor = step_divisor;
}

void SMotor_SetSpeed(SMotor *motor, float angular_speed) {
    if (motor == NULL || motor->pwm_timer == NULL) {
        #ifdef INITIALIZE_H
        sprintf(error_message, "SMotor_SetSpeed: motor or pwm_timer is NULL");
        Error_Handler();
        #endif
        return;
    }
    double aspeed = angular_speed;
    if(angular_speed == 0) {
        __HAL_TIM_SET_COMPARE(motor->pwm_timer, motor->pwm_channel, 0); // Stop PWM
        return; // No speed set, stop the motor
    } else if(angular_speed > 0) {
        HAL_GPIO_WritePin(motor->Dir_port, motor->Dir_pin, motor->parameters.Anti_Dir); // Set direction to clockwise
    } else {
        HAL_GPIO_WritePin(motor->Dir_port, motor->Dir_pin, motor->parameters.Anti_Dir == GPIO_PIN_SET ? GPIO_PIN_RESET : GPIO_PIN_SET); // Set direction to counter-clockwise
        angular_speed = -angular_speed; // Ensure speed is positive for PWM
    }
    uint32_t tim_clk = GetClockFre(motor->pwm_timer);
    uint32_t target_frequency = GetStepFrequency(angular_speed, motor);
    uint32_t target_prcs = (tim_clk / target_frequency / (__HAL_TIM_GET_AUTORELOAD(motor->pwm_timer) + 1)) - 1; // Calculate prescaler value
    float target_arr = 1;
    if (target_prcs > 0xFFFF ) {
        motor->state.angular_speed = aspeed * (0xFFFF / (double)target_prcs); // Adjust speed to fit within prescaler limits
        target_prcs = 0xFFFF; // Limit prescaler to maximum value
   }else if(target_prcs < 1) {
        motor->state.angular_speed = aspeed * (1.0 / target_prcs); // Use original speed if prescaler is less than 1
        target_prcs = 1; // Ensure prescaler is at least 1
    }else {
        motor->state.angular_speed = aspeed; // Use original speed
    }
    __HAL_TIM_SET_PRESCALER(motor->pwm_timer, target_prcs); // Set prescaler
    __HAL_TIM_SET_COMPARE(motor->pwm_timer, motor->pwm_channel, motor->pwm_timer->Instance->ARR / 2); // Set duty cycle to 50%
}

void SMotor_UpdateState(SMotor *motor) {
    if (motor == NULL) {
        #ifdef INITIALIZE_H
        sprintf(error_message, "SMotor_UpdateState: motor is NULL");
        Error_Handler();
        #endif
        return;
    }
    if(motor->state.last_update_time == -1) {
        motor->state.last_update_time = HAL_GetTick(); // Initialize last update time
        return;
    }
    uint32_t current_time = HAL_GetTick();
    motor->state.current_position += motor->state.angular_speed * (current_time - motor->state.last_update_time) * 1e-3; // Update current position
		motor->state.last_update_time = current_time;
}

uint32_t GetClockFre(TIM_HandleTypeDef *htim) {
    if (htim == NULL || htim->Instance == NULL) {
        #ifdef INITIALIZE_H
        sprintf(error_message, "GetClockFre: htim or htim->Instance is NULL");
        Error_Handler();
        #endif
        return 0;
    }
    uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
    uint32_t tim_clk;

    uint32_t ppre1_bits = (RCC->CFGR >> 10) & 0x7;

    if (ppre1_bits == 0x0) {
        tim_clk = pclk1 * 2; // APB1 clock frequency
    } else {
        tim_clk = pclk1;
    }

    return tim_clk;
}

uint32_t GetStepFrequency(float angular_speed, SMotor *motor) {
    if (motor == NULL) {
        #ifdef INITIALIZE_H
        sprintf(error_message, "GetStepFrequency: motor is NULL");
        Error_Handler();
        #endif
        return 0;
    }
    if (angular_speed < 0) {
        angular_speed = -angular_speed; // Ensure speed is positive
    }
    uint32_t step_frequency = (uint32_t)(angular_speed / motor->parameters.step_angular * motor->parameters.step_divisor);
    return step_frequency;
}