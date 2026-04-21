#include "Initialize.h"

//以下段落为车辆实例化代码，不改动车体的情况下，不进行改动

Motor Le, Ri;  // Declare motors for left and right
CarState car; // Declare car state for kinematics
Data current_data;
extern double GyroscopeChannelData[10];

void MEInit(Motor* L, Motor* R)
{
    Motor_UI_Init(L, LEFT_MOTOR_IN1_PORT, LEFT_MOTOR_IN1_PIN,
        LEFT_MOTOR_IN2_PORT, LEFT_MOTOR_IN2_PIN,
        LEFT_MOTOR_PWM_TIMER, LEFT_MOTOR_PWM_CHANNEL, LEFT_MOTOR_INIT_DUTY);
    Motor_UI_Init(R, RIGHT_MOTOR_IN1_PORT, RIGHT_MOTOR_IN1_PIN,
        RIGHT_MOTOR_IN2_PORT, RIGHT_MOTOR_IN2_PIN,
        RIGHT_MOTOR_PWM_TIMER, RIGHT_MOTOR_PWM_CHANNEL, RIGHT_MOTOR_INIT_DUTY);

    LRInit(LEFT_ENCODER_TIMER, LEFT_ENCODER_CHANNEL_A, LEFT_ENCODER_CHANNEL_B,
           RIGHT_ENCODER_TIMER, RIGHT_ENCODER_CHANNEL_A, RIGHT_ENCODER_CHANNEL_B,
           WHEEL_DIAMETER, PPR * REDUCE, ENCODER_REAL_TIMER ); // Initialize the encoders with wheel length and pulses per revolution
    
    Le = *L;  // Assign the left motor to Le
    Ri = *R;  // Assign the right motor to Ri

    CarState_Init(&car); // Initialize the car state
    // Set the initial speed to default
    car.pose.initial_theta = CalibrateYawOffset(); // Calibrate the initial yaw offset
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

float getYaw()
{
    // Get the yaw angle from the JY61P sensor
    return GyroscopeChannelData[8]; // Return the yaw angle
}

float getWz()
{
    // Get the angular velocity (wz) from the JY61P sensor
    return GyroscopeChannelData[5]; // Return the angular velocity around the z-axis
}

float CalibrateYawOffset()
{
    float sum = 0;
    int N = 100;
    for(int i = 0; i < N; ++i) {
        sum += getYaw();
        HAL_Delay(10);  // 等待模块稳定输出
    }
    float yaw_offset = sum / N;
    // 计算平均值作为偏移量
    return yaw_offset; // 返回偏移量
}

void UpdateData()
{
    static uint32_t last_time = 0; // Last update time
    uint32_t now = HAL_GetTick(); // Get the current time
    static bool first_run = true; // Flag to indicate the first run
    current_data.speed.linear_velocity = cSpeed(); // Get the current speed
    current_data.speed.angular_velocity = getWz(); // Get the current angular velocity
    current_data.yaw = getYaw(); // Get the current yaw angle
    if(first_run)
    {
        first_run = false; // Set the flag to false after the first run
        current_data.dt = 0; // Initialize dt on the first run
    }
    else
    {
        current_data.dt = (now - last_time) * 1e-3f; // Calculate the time difference in seconds
    }
    if(current_data.dt <= 0.0f) current_data.dt = 0.01f; // Ensure dt is not zero to avoid division by zero 
    last_time = now; // Initialize last_time on the first run

    return; // Exit the function
}

void UpdateData_Car()
{
    // Update the car state with the current data
    UpdateData(); // Update the current data
    CarState_Update(&car, current_data); // Update the car state with the current data
}



//以下代码为特殊题目下用到的车辆控制函数，在出现新题的情况下可以更改

bool isInTheYaw(float targetYaw, float tolerance)
{
    // Check if the current yaw is within the specified tolerance of the target yaw
    float currentYaw = getYaw(); // Get the current yaw angle
    float dif = fabs(sumTheta(currentYaw, -targetYaw)); // Calculate the difference between current and target yaw
    // If the absolute difference is less than the tolerance, return true
    if (dif < tolerance || 180 - dif < tolerance) {
        return true; // If within tolerance, return true
    } else {
        return false; // Otherwise, return false
    }
}

void Back(float theta)
{
    float speed = 0.35f; // Set the speed for backward movement
    float targetYaw = sumTheta(car.pose.initial_theta, theta); // Calculate the target yaw angle
    Straight(0.3, speed, targetYaw, BACKWARD); // Move backward with the specified speed and target yaw
}