#include "Cont_SMotor.h"
#include <math.h>
#include <stdbool.h>

Coordinate laser_position = {0.0f, 0.0f}; // 激光雷达位置
Coordinate target_position = {0.0f, 0.0f}; // 目标位置
bool is_updated = false; // 是否更新了数据
bool is_new_mode = true; // 是否进入了新模式
TargetPositionFunctions target_functions = {
    .t_to_x = NULL, // 时间到X轴位置的转换函数
    .t_to_y = NULL, // 时间到Y轴位置的转换函数
    .init_t = 0.0f // 初始化时间
};

float r = 0;
float angular_speed = 0;
Coordinate O_pos = {0.0f, 0.0f}; // 圆心位置


Attitude CoordinateToAttitude(Coordinate coord) {
    Attitude attitude;
    // 计算方向角和俯仰角
    attitude.yaw = -atan2f(XDIS_MOTOR_O + coord.x, ZDIS_MOTOR_O) * 180.0f / C_PI; // 转换为角度
    attitude.pitch = atan2f(YDIS_MOTOR_O + coord.y, sqrtf(powf(XDIS_MOTOR_O + coord.x, 2) + powf(ZDIS_MOTOR_O, 2))) * 180.0f / C_PI;
    return attitude;
}

void PID_SMotor_Cont(void)
{
    static PIDdata pid_yaw, pid_pitch;
    static uint32_t last_update_time = 0;
    PIDConfig config_yaw = {1.0f, 0.1f, 0.01f};
    PIDConfig config_pitch = {1.0f, 0.1f, 0.01f};
    uint32_t current_time = HAL_GetTick();
    if(is_new_mode) {
        PID_Init(&pid_yaw);
        PID_Init(&pid_pitch);
        is_new_mode = false; // 重置新模式标志
        last_update_time = current_time; // 更新上次更新时间
        return; // 如果是新模式则不进行PID计算
    }
    if(!is_updated) return; // 如果数据未更新则返回
    is_updated = false; // 重置更新标志
    float dt = (current_time - last_update_time) / 1000.0f; // 时间间隔
    Attitude target_attitude = CoordinateToAttitude(target_position);
    Attitude attitude = CoordinateToAttitude(laser_position);
    PID_Update(&pid_yaw, target_attitude.yaw, attitude.yaw, dt);
    PID_Update(&pid_pitch, target_attitude.pitch, attitude.pitch, dt);
    last_update_time = current_time; // 更新上次更新时间
    // 计算PID输出
    float output_yaw = PID_Compute(&pid_yaw, config_yaw.Kp, config_yaw.Ki, config_yaw.Kd);
    float output_pitch = PID_Compute(&pid_pitch, config_pitch.Kp, config_pitch.Ki, config_pitch.Kd);
    // 发送控制命令
    YP_SMotor_SetSpeed(output_yaw, output_pitch); // 设置舵机速度
    YP_SMotor_UpdateState(); // 更新舵机状态
    #ifdef SMOTOR_DEBUG
    target_position.x = 1.0f; // 调试时将目标位置设置为激光雷达位置
    target_position.y = 1.0f; // 调试时将目标位置设置为激光雷达位置
    laser_position.x += output_yaw * dt * (-0.01f); // 模拟激光雷达位置变化
    laser_position.y += output_pitch * dt * (0.01f); // 模拟激光雷达位置变化
    is_updated = true; // 设置数据已更新标志
    #endif
}

void TargetPositionUpdate(void)
{
    target_position.x = target_functions.t_to_x(HAL_GetTick() * 0.001f - target_functions.init_t);
    target_position.y = target_functions.t_to_y(HAL_GetTick() * 0.001f - target_functions.init_t);
    return;
}

void TargetPositionSetFunctions(float (*t_to_x)(float), float (*t_to_y)(float), float init_t)
{
    target_functions.t_to_x = t_to_x;
    target_functions.t_to_y = t_to_y;
    target_functions.init_t = init_t;
    return;
}

float circle_t_to_x(float t)
{
    return O_pos.x + r * cosf(angular_speed * t);
}

float circle_t_to_y(float t)
{
    return O_pos.y + r * sinf(angular_speed * t);
}

void SetTargetCircle(float radius, float center_x, float center_y, float angular_velocity)
{
    r = radius; // 圆的半径
    O_pos.x = center_x; // 圆心X坐标
    O_pos.y = center_y; // 圆心Y坐标
    angular_speed = angular_velocity; // 角速度
    // 设置时间到X轴和Y轴位置的转换函数
    TargetPositionSetFunctions(circle_t_to_x, circle_t_to_y, HAL_GetTick() * 0.001f);
}

