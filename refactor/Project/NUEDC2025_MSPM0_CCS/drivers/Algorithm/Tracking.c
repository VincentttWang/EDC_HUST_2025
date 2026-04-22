#include "Tracking.h"
#include "AllHeader.h"

#define IRR_SPEED_HIGH  300
#define IRR_SPEED_LOW   350

#define IRTrack_Trun_KP_HIGH 50
#define IRTrack_Trun_KI_HIGH 0
#define IRTrack_Trun_KD_HIGH 0

#define IRTrack_Trun_KP_LOW  30
#define IRTrack_Trun_KI_LOW  0
#define IRTrack_Trun_KD_LOW  1.5

static const float pid_out_max = 5000.0f;
static const float Integral_max = 500.0f;
static int pid_output_IRR = 0;
static int8_t err = 0;
extern uint8_t Digital[8];

float PID_IR_Calc_Custom(int16_t actual_value, float kp, float ki, float kd)
{
    float pid_out = 0;
    int16_t error;
    static int16_t error_last = 0;
    static float Integral = 0;

    error = actual_value;
    Integral += error;
    if (Integral > Integral_max) Integral = Integral_max;
    if (Integral < -Integral_max) Integral = -Integral_max;

    pid_out = error * kp + ki * Integral + (error - error_last) * kd;
    error_last = error;

    if (pid_out > pid_out_max) pid_out = pid_out_max;
    if (pid_out < -pid_out_max) pid_out = -pid_out_max;

    return pid_out;
}

void lineWalking_low(void)
{
    lineWalking_core(IRR_SPEED_LOW, IRTrack_Trun_KP_LOW, IRTrack_Trun_KI_LOW, IRTrack_Trun_KD_LOW);
}

void lineWalking_core(int16_t speed, float kp, float ki, float kd)
{
    float sum_position = 0;
    int num_active_sensors = 0;
    float sensor_positions[] = {-3.5f, -2.5f, -1.5f, -0.5f, 0.5f, 1.5f, 2.5f, 3.5f};

    for (int i = 0; i < 8; i++) {
        if (Digital[i] == 0) {
            sum_position += sensor_positions[i];
            num_active_sensors++;
        }
    }

    if (num_active_sensors > 0) {
        float average_position = sum_position / num_active_sensors;
        err = (int8_t)(average_position * 10.0f);
    }

    pid_output_IRR = (int)(PID_IR_Calc_Custom(err, kp, ki, kd));
    Motion_Car_Control(speed, 0, pid_output_IRR);
}

static int speed_L2_setup = 0;
static int speed_R2_setup = 0;

void Motion_Car_Control(int16_t V_x, int16_t V_y, int16_t V_z)
{
    float robot_APB = 115;
    float speed_fb = V_x;
    float speed_spin = (V_z / 1000.0f) * robot_APB;

    if (V_x == 0 && V_y == 0 && V_z == 0) {
        Motor_SetRight(0);
        Motor_SetLeft(0);
        return;
    }

    speed_L2_setup = speed_fb + speed_spin;
    speed_R2_setup = speed_fb - speed_spin;

    if (speed_L2_setup > 1000) speed_L2_setup = 1000;
    if (speed_L2_setup < -1000) speed_L2_setup = -1000;
    if (speed_R2_setup > 1000) speed_R2_setup = 1000;
    if (speed_R2_setup < -1000) speed_R2_setup = -1000;

    Motor_SetLeft(speed_L2_setup);
    Motor_SetRight(speed_R2_setup);
}
