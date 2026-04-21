#include "Initialize.h"
#include "AllHeader.h"


Motor Le, Ri;  // Declare motors for left and right
CarState car; // Declare car state for kinematics
Data current_data;
uint8_t Digital[8];
char error_message[100]; // Buffer for error messages
double GyroscopeChannelData[10] = {0};
float sInedge = 0.0f; // Initialize sInedge for tracking distance
int edge = 0; // Initialize edge variable for target coordinate calculation
char message[100]; // Buffer for messages

void MECInit()
{
    // 初始化左电机硬件接口
    Motor_UI_Init(&Le, LEFT_MOTOR_IN1_PORT, LEFT_MOTOR_IN1_PIN,
                  LEFT_MOTOR_IN2_PORT, LEFT_MOTOR_IN2_PIN,
                  LEFT_MOTOR_PWM_TIMER, LEFT_MOTOR_PWM_CHANNEL, LEFT_MOTOR_INIT_DUTY);
    
    // 初始化右电机硬件接口
    Motor_UI_Init(&Ri, RIGHT_MOTOR_IN1_PORT, RIGHT_MOTOR_IN1_PIN,
                  RIGHT_MOTOR_IN2_PORT, RIGHT_MOTOR_IN2_PIN,
                  RIGHT_MOTOR_PWM_TIMER, RIGHT_MOTOR_PWM_CHANNEL, RIGHT_MOTOR_INIT_DUTY);
}

void LMotorSet(MOVETYPE type, uint16_t duty)
{
    Motor_UI_Set(type, duty, &Le);
}
void RMotorSet(MOVETYPE type, uint16_t duty)
{
    Motor_UI_Set(type, duty, &Ri);
}

void LSet(int16_t duty)
{
    MOVETYPE type = FOR; // Default to forward motion
    
    if(duty < 0) {
        type = BACK; // If duty is negative, set to backward motion
        duty = -duty; // Convert to positive duty cycle
    }
    if(duty > 1000) {
        duty = 1000; // Limit the duty cycle to a maximum of 1000
    }
    LMotorSet(type, duty); // Set the left motor with the specified type and duty cycle
}

void RSet(int16_t duty)
{
    MOVETYPE type = FOR; // Default to forward motion
    
    if(duty < 0) {
        type = BACK; // If duty is negative, set to backward motion
        duty = -duty; // Convert to positive duty cycle
    }
    if(duty > 1000) {
        duty = 1000; // Limit the duty cycle to a maximum of 1000
    }
    RMotorSet(type, duty); // Set the right motor with the specified type and duty cycle
}

void Break()
{
    LMotorSet(BREAK, 0); // Set both motors to break mode
    RMotorSet(BREAK, 0);
}



void error_handler(void)
{
    Break(); // Stop the motors
    OLED_Clear(); // Clear the OLED display
    OLED_ShowString(0, 0, error_message,8); // Display the error message on the OLED
    while(1); // Infinite loop to halt the program
}

void UpdateSInedge(void)
{
    static uint32_t last_time = 0; // Last update time
    uint32_t now = tick; // Get the current time (TI equivalent)

    if (now - last_time >= 10) // Update every 10ms
    {
        float dt = (now - last_time) * 1e-3f; // Calculate the time difference in seconds
        last_time = now;
        sInedge += getSpeed() * dt; // Update sInedge with the current speed
    }
}

