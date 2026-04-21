#include "Cont_SMotor.h"
#include <math.h>
#include <stdbool.h>
#include "AllHeader.h"

Coordinate laser_position = {0.0f, 0.0f}; // 激光雷达位置
Coordinate target_position = {0.0f, 0.0f}; // 目标位置
bool is_updated = false; // 是否更新了数据
bool is_new_mode = true; // 是否进入了新模式
Attitude cor = {0.0f, 0.0f}; // 校准数据

#define LP_X 300 // 线性位置X坐标
#define LP_Y 225 // 线性位置Y坐标

extern bool turning;

void PID_SMotor_Cont(void)
{
    static PIDdata pid_x, pid_y;
    static uint32_t last_update_time = 0;
    static float last_output_wyaw = 0.0f; // 上一次偏航速度输出
    static float last_output_wpitch = 0.0f;
    PIDConfig config_x = {0.2f, 0, 0};
    PIDConfig config_y = {0.2f, 0, 0};
    uint32_t current_time = tick;
    if(is_new_mode) {
        PID_Init(&pid_x);
        PID_Init(&pid_y);
        is_new_mode = false; // 重置新模式标志
        last_update_time = current_time; // 更新上次更新时间
        return; // 如果是新模式则不进行PID计算
    }
    float output_wyaw = 0.0f; // 输出的偏航速度
    float output_wpitch = 0.0f;
    if(!is_updated && (!Laser_error )) {
        output_wyaw = last_output_wyaw; // 如果没有更新数据，则使用上一次的输出
        output_wpitch = last_output_wpitch;
    }
    else if(is_updated) {
        is_updated = false; // 重置更新标志
        float dt = (current_time - last_update_time) / 1000.0f; // 时间间隔
        PID_Update(&pid_x, target_position.x, laser_position.x, dt);
        PID_Update(&pid_y, target_position.y, laser_position.y, dt);
        last_update_time = current_time; // 更新上次更新时间
        // 计算PID输出
        output_wyaw = -1 * PID_Compute(&pid_x, config_x.Kp, config_x.Ki, config_x.Kd);
        output_wpitch = PID_Compute(&pid_y, config_y.Kp, config_y.Ki, config_y.Kd);
        // 发送控制命令
    }
    output_wpitch += cor.pitch; // 添加俯仰角校准
    output_wyaw += cor.yaw; // 添加偏航角校准
		
		char message [50];	
		snprintf(message, 50, "PID Yaw = %.2f", output_wyaw);
		OLED_ShowString(0, 4, message, 8);
		
    YP_SMotor_SetSpeed( output_wyaw, output_wpitch); // 设置舵机速度
    YP_SMotor_UpdateState(); // 更新舵机状态
}

void SetTargetCenter(void)
{
    // 计算目标中心位置
    if(!Laser_error && Laser_Loc[0] != 0 && Laser_Loc[1] != 0 )
    {
        target_position.x = Laser_Loc[0];
        target_position.y = 480 - Laser_Loc[1];
        is_updated = true; // 设置数据已更新标志
    }
}

void SetLaserPosition(void)
{
    if(!Laser_error)
    {
        laser_position.x = Laser_Loc[2]; // 设置激光雷达位置X坐标
        laser_position.y = 480 - Laser_Loc[3]; // 设置激光雷达位置Y坐标
    }
}

void SetTargetCircle(void)
{
    if(!Rect_error)
    {
        target_position = paper_to_camera(get_target_coordinate()); // 获取目标坐标并转换为相机坐标系
        is_updated = true; // 设置数据已更新标志
    }
}

void Compute_excur(void)
{
    if(turning) 
    {
        const float w = 120.0f; // 目标偏航角
        cor.yaw = -w; // 计算偏航角
				DL_GPIO_setPins(LED_PORT, LED_LED0_PIN);
		return; // 退出函数
    }
						DL_GPIO_clearPins(LED_PORT, LED_LED0_PIN);
    float d = getDistance(); // 获取距离
    // 计算偏差
    switch(edge % 4)
    {
        case 0:
            cor.yaw = - (getSpeed() * fabs(0.5f - sInedge) / d) / d; // 计算偏差
            break;
        case 1:
            // 处理边缘情况1
            cor.yaw = (getSpeed() * 0.5f / d) / d;
            break;
        case 2:
            // 处理边缘情况2
            cor.yaw = (getSpeed() * fabs(0.5f - sInedge) / d) / d;
            break;
        case 3:
            // 处理边缘情况3
            cor.yaw = - (getSpeed() * 0.5f / d) / d;
            break;
        default:
            // 处理其他情况
            break;
    }
    return; // 退出函数
}

float getDistance(void)
{
    switch(edge % 4)
    {
        case 0:
            return sqrt(pow(0.5f - sInedge, 2) + pow(0.5f, 2)); // 计算距离
        case 1:
            return sqrt(pow(0.5f, 2) + pow(0.5f + sInedge, 2)); // 计算距离
        case 2:
            return sqrt(pow(0.5f - sInedge, 2) + pow(1.5f, 2));
        case 3:
            return sqrt(pow(0.5f, 2) + pow(1.5f - sInedge, 2));
    }
		return 0;
}