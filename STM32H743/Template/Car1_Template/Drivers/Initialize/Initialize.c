#include "Initialize.h"


Motor Le, Ri;  // Declare motors for left and right
CarState car; // Declare car state for kinematics
Data current_data;
int Digital[8];
char error_message[100]; // Buffer for error messages


void MECInit()
{
    Motor_UI_Init(&Le, LEFT_MOTOR_IN1_PORT, LEFT_MOTOR_IN1_PIN,
        LEFT_MOTOR_IN2_PORT, LEFT_MOTOR_IN2_PIN,
        LEFT_MOTOR_PWM_TIMER, LEFT_MOTOR_PWM_CHANNEL, LEFT_MOTOR_INIT_DUTY);
    Motor_UI_Init(&Ri, RIGHT_MOTOR_IN1_PORT, RIGHT_MOTOR_IN1_PIN,
        RIGHT_MOTOR_IN2_PORT, RIGHT_MOTOR_IN2_PIN,
        RIGHT_MOTOR_PWM_TIMER, RIGHT_MOTOR_PWM_CHANNEL, RIGHT_MOTOR_INIT_DUTY);

    LRInit(LEFT_ENCODER_TIMER, LEFT_ENCODER_CHANNEL_A, LEFT_ENCODER_CHANNEL_B,
           RIGHT_ENCODER_TIMER, RIGHT_ENCODER_CHANNEL_A, RIGHT_ENCODER_CHANNEL_B,
           WHEEL_DIAMETER, PPR * REDUCE, ENCODER_REAL_TIMER ); // Initialize the encoders with wheel length and pulses per revolution
    
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
    float yaw = GyroscopeChannelData[8];
    if(yaw < -360.0f || yaw > 360.0f) {
        snprintf(error_message, sizeof(error_message), "Yaw value out of range: %.2f", yaw);
        Error_Handler(); // Handle error if yaw is out of range
    }
    if(yaw < -180.0f) {
        yaw += 360.0f; // Normalize yaw to the range [-180, 180]
    } else if(yaw > 180.0f) {
        yaw -= 360.0f; // Normalize yaw to the range [-180, 180]
    }
    return yaw; // Return the normalized yaw value
}

/* RotationAngles getRotationAngles()
{
    // Get the rotation angles from the MS601M sensor
    atk_ms601m_attitude_data_t attitude_dat;
    atk_ms601m_get_attitude(&attitude_dat, 10);
    
    RotationAngles angles;
    angles.yaw = attitude_dat.yaw; // Set yaw angle
    angles.pitch = attitude_dat.pitch; // Set pitch angle
    angles.roll = attitude_dat.roll; // Set roll angle
    
    return angles; // Return the rotation angles
} */

float getWz()
{
    return GyroscopeChannelData[5]; // Return the angular velocity around the z-axis
}

float CalibrateYawOffset()
{
    float sum = 0;
    int N = 100;
    for(int i = 0; i < N; ++i) {
        sum += getYaw();
        HAL_Delay(10);  
    }
    float yaw_offset = sum / N;
    // Calculate the average yaw offset over N samples
    // This helps to reduce noise and improve accuracy
    return yaw_offset; // Return the calibrated yaw offset
}

void UpdateData()
{
    const float speedCorrection = 1.03f; // Speed correction factor
    static uint32_t last_time = 0; // Last update time
    uint32_t now = HAL_GetTick(); // Get the current time
    static volatile bool first_run = true; // Flag to indicate the first run
    /* static float last_yaw = 0.0f; // Last yaw value for the first run
 */
    current_data.speed.linear_velocity = cSpeed() * speedCorrection; // Get the current speed
    current_data.yaw = getYaw();// current_data.angles = getRotationAngles(); // Get the current rotation angles
    current_data.speed.angular_velocity = getWz(); // Get the current angular velocity
    IIC_Get_Digital(Digital);

    if(first_run)
    {
        first_run = false; // Set the flag to false after the first run
        current_data.dt = 0; // Initialize dt on the first run
        /* current_data.speed.angular_velocity = getWz(); // 因为没有相对时间，所以在第一次运行时直接获取当前的角速度
 */   }
    else
    {
        current_data.dt = (now - last_time) * 1e-3f; // Calculate the time difference in seconds
        //current_data.speed.angular_velocity = (current_data.yaw - last_yaw) / current_data.dt; // Calculate the angular velocity
    }
    if(current_data.dt <= 0.0f) current_data.dt = 0.01f; // Ensure dt is not zero to avoid division by zero 
    last_time = now; // Initialize last_time on the first run
		//last_yaw = current_data.yaw; // Store the last yaw value

    return; // Exit the function
}

void UpdateData_Car()
{
    // Update the car state with the current data
    UpdateData(); // Update the current data
    CarState_Update(&car, current_data); // Update the car state with the current data
}

void error_handler(void)
{
    RED_up(); // Turn on the red LED to indicate an error
    Break(); // Stop the motors
    OLED_Clear(); // Clear the OLED display
    OLED_ShowString(0, 0, error_message, 8); // Display the error message on the OLED
    while(1); // Infinite loop to halt the program
}
