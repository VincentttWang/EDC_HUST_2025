#include "SMotor.h"
#include "AllHeader.h"

extern SMotor yawMotor;
void SMotor_Init(SMotor *motor, GPIO_Regs *Dir_port, uint32_t Dir_pin,
                 GPTIMER_Regs *pwm_timer, DL_TIMER_CC_INDEX pwm_channel) {
    if (motor == NULL || pwm_timer == NULL) {
        #ifdef INITIALIZE_H
        sprintf(error_message, "SMotor_Init: motor or pwm_timer is NULL");
        error_handler();
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
    DL_GPIO_clearPins(motor->Dir_port, motor->Dir_pin);
    DL_TimerG_startCounter(motor->pwm_timer); // Start PWM timer
    DL_TimerG_setCaptureCompareValue(motor->pwm_timer, 0, motor->pwm_channel); // Set initial PWM duty cycle to 0
}

void SMotor_Parameters_Init(SMotor *motor, SMOTOR_DIR_STATE Anti_Dir, float step_angular, float step_divisor) {
    if (motor == NULL) {
        #ifdef INITIALIZE_H
        sprintf(error_message, "SMotor_Parameters_Init: motor is NULL");
        error_handler();
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
        error_handler();
        #endif
        return;
    }
    motor->state.angular_speed = angular_speed;
		
						if( motor == &yawMotor && DL_GPIO_readPins(motor->Dir_port, motor->Dir_pin) == 0){
					int a = 1 ;}
						
					
    if(fabs(angular_speed) == 0) {
        DL_TimerG_setCaptureCompareValue(motor->pwm_timer, 0, motor->pwm_channel); // Stop PWM
        return; // Nospeed set, stop the motor
    } else if(angular_speed > 0) {	
					
        if(motor->parameters.Anti_Dir == SMOTOR_DIR_HIGH) {
            DL_GPIO_setPins(motor->Dir_port, motor->Dir_pin); // Set direction to clockwise
        } else {
            DL_GPIO_clearPins(motor->Dir_port, motor->Dir_pin);
        }
    } else {
        if(motor->parameters.Anti_Dir == SMOTOR_DIR_HIGH) {
            DL_GPIO_clearPins(motor->Dir_port, motor->Dir_pin); // Set direction to counter-clockwise
        } else {
					DL_GPIO_setPins(motor->Dir_port, motor->Dir_pin);
        }
        angular_speed = -angular_speed; // Ensure speed is positive for PWM
    }
    
    uint32_t tim_clk = GetClockFre(motor->pwm_timer);
    uint32_t target_frequency = GetStepFrequency(angular_speed, motor);
    uint32_t now_prescaler = 32 * 8 * 2; // Default prescaler value
    uint32_t target_arr = tim_clk / (target_frequency * now_prescaler) - 1; // Calculate ARR value
    if (target_arr > 65535) {
        target_arr = 65535; // Limit to maximum value for 16-bit timer
    }
		
    DL_TimerG_setLoadValue(motor->pwm_timer, target_arr); // 设置新的ARR值
    DL_TimerG_setCaptureCompareValue(motor->pwm_timer, target_arr / 2, motor->pwm_channel); // 占空比50%
}

void SMotor_UpdateState(SMotor *motor) {
    if (motor == NULL) {
        #ifdef INITIALIZE_H
        sprintf(error_message, "SMotor_UpdateState: motor is NULL");
        error_handler();
        #endif
        return;
    }
    if(motor->state.last_update_time == -1) {
        motor->state.last_update_time = tick; // Initialize last update time
        return;
    }
    uint32_t current_time = tick;
    motor->state.current_position += motor->state.angular_speed * (current_time - motor->state.last_update_time) * 1e-3; // Update current position
		motor->state.last_update_time = current_time;
}

uint32_t GetClockFre(GPTIMER_Regs* timer) {
    if (timer == NULL) {
        #ifdef INITIALIZE_H
        sprintf(error_message, "GetClockFre: timer is NULL");
        error_handler();
        #endif
        return 0;
    }
    // TI MSPM0 系统时钟通常为32MHz
    // 对于DriverLib中的timer，获取时钟频率的方式需要根据具体配置
    // 这里返回系统时钟频率，实际使用时可能需要根据timer的分频设置调整
    return 32000000; // 32MHz system clock
}

uint32_t GetStepFrequency(float angular_speed, SMotor *motor) {
    if (motor == NULL) {
        #ifdef INITIALIZE_H
        sprintf(error_message, "GetStepFrequency: motor is NULL");
        error_handler();
        #endif
        return 0;
    }
    if (angular_speed < 0) {
        angular_speed = -angular_speed; // Ensure speed is positive
    }
    uint32_t step_frequency = (uint32_t)(angular_speed / motor->parameters.step_angular * motor->parameters.step_divisor);
    return step_frequency;
}