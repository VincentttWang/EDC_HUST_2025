#include "tracking.h"
#include "AllHeader.h"

#define IRR_SPEED_HIGH  400  // 高速巡线速度
#define IRR_SPEED_LOW   200  // 低速巡线速度

// 高速PID参数
#define IRTrack_Trun_KP_HIGH 50
#define IRTrack_Trun_KI_HIGH 0
#define IRTrack_Trun_KD_HIGH 0

// 低速PID参数
#define IRTrack_Trun_KP_LOW  20
#define IRTrack_Trun_KI_LOW  0
#define IRTrack_Trun_KD_LOW  0.5

const float pid_out_max = 5000.0f; 
const float Integral_max = 500.0f; // 积分限幅值 
int pid_output_IRR = 0;
u8 trun_flag = 0;
static int8_t err = 0;
extern int Digital[8];

// --- PID计算函数，参数可配置 ---
float PID_IR_Calc_Custom(int16_t actual_value, float kp, float ki, float kd)
{
    float pid_out = 0;
    int16_t error; 
    static int16_t error_last = 0; //上次的误差初始为0
    static float Integral = 0; // 初始化积分项

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

// --- 高速巡线 ---
void lineWalking_high(void)
{
    lineWalking_core(IRR_SPEED_HIGH, IRTrack_Trun_KP_HIGH, IRTrack_Trun_KI_HIGH, IRTrack_Trun_KD_HIGH);
}

// --- 低速巡线 ---
void lineWalking_low(void)
{
    lineWalking_core(IRR_SPEED_LOW, IRTrack_Trun_KP_LOW, IRTrack_Trun_KI_LOW, IRTrack_Trun_KD_LOW);
}

// --- 核心巡线逻辑，参数可配置 ---
void lineWalking_core(int16_t speed, float kp, float ki, float kd)
{
    // 在 lineWalking_core 函数中
	// ...
	float sum_position = 0;
	int num_active_sensors = 0;

	// 为每个传感器分配一个位置权重，中心为0
	// 对于8个传感器，可以这样分配：
	// 索引:   0    1    2    3    4    5    6    7
	// 权重: -3.5 -2.5 -1.5 -0.5  0.5  1.5  2.5  3.5
	float sensor_positions[] = {-3.5f, -2.5f, -1.5f, -0.5f, 0.5f, 1.5f, 2.5f, 3.5f};
    
	for (int i = 0; i < 8; i++) {
		if (Digital[i] == 0) { // 如果传感器检测到线
				sum_position += sensor_positions[i];
				num_active_sensors++;
		}
	}

	if (num_active_sensors > 0) {
		float average_position = sum_position / num_active_sensors;
		// 将平均位置乘以一个缩放因子，得到您期望的误差范围
		// 例如，乘以10或20，让err的绝对值更大，以便KP能更明显地作用
		err = (int8_t)(average_position * 10.0f); // 这里的10.0f是示例，需要根据实际效果调整
	} else {
		// 如果所有传感器都没有检测到线，通常意味着机器人完全偏离了。
		// 您需要在这里实现一个“失线”恢复策略，例如停车、旋转寻找线等。
		// 暂时可以保持上一次的err，或者根据实际情况设定一个默认值。
	}

	// 然后将这个更精细的 err 传递给 PID_IR_Calc_Custom
		pid_output_IRR = (int)(PID_IR_Calc_Custom(err, kp, ki, kd)); // 注意这里不再需要 err * 3 
    Motion_Car_Control(speed, 0, pid_output_IRR);
}

static float speed_lr = 0;
static float speed_fb = 0;
static float speed_spin = 0;
static int speed_L1_setup = 0;
static int speed_L2_setup = 0;
static int speed_R1_setup = 0;
static int speed_R2_setup = 0;


void Motion_Car_Control(int16_t V_x, int16_t V_y, int16_t V_z)
{
	float robot_APB = 115; //轮距的一半
	speed_lr = 0;
    speed_fb = V_x;
    speed_spin = (V_z / 1000.0f) * robot_APB;
    if (V_x == 0 && V_y == 0 && V_z == 0)
    {
        RSet(0);
				LSet(0);
        return;
    }

//    speed_L1_setup = speed_fb + speed_spin;
    speed_L2_setup = speed_fb + speed_spin;
//    speed_R1_setup = speed_fb  - speed_spin;
    speed_R2_setup = speed_fb  - speed_spin;
		
//    if (speed_L1_setup > 1000) speed_L1_setup = 1000;
//    if (speed_L1_setup < -1000) speed_L1_setup = -1000;
    if (speed_L2_setup > 1000) speed_L2_setup = 1000;
    if (speed_L2_setup < -1000) speed_L2_setup = -1000;
//    if (speed_R1_setup > 1000) speed_R1_setup = 1000;
//    if (speed_R1_setup < -1000) speed_R1_setup = -1000;
    if (speed_R2_setup > 1000) speed_R2_setup = 1000;
    if (speed_R2_setup < -1000) speed_R2_setup = -1000;
    
    //printf("%d\t,%d\t,%d\t,%d\r\n",speed_L1_setup,speed_L2_setup,speed_R1_setup,speed_R2_setup);
    
		LSet(speed_L2_setup);
		RSet(speed_R2_setup);
		
}
