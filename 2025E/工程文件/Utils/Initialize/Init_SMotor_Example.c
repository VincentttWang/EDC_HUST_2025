/**
 * @file Init_SMotor_Example.c
 * @brief TI MSPM0G3507平台双轴步进电机使用示例
 * @author 移植自STM32平台
 * @date 2024
 * 
 * 本文件演示如何在TI MSPM0G3507平台上使用双轴步进电机控制
 * 适用于云台、摄像头支架、机械臂等应用
 */

#include "Init_SMotor.h"
#include <math.h>

/**
 * @brief 辅助函数：移动到指定位置
 * @param target_yaw 目标YAW角度
 * @param target_pitch 目标PITCH角度
 */
void move_to_position(float target_yaw, float target_pitch) {
    float tolerance = 2.0f;  // 2度容差
    float max_speed = 60.0f; // 最大速度
    
    while(1) {
        YP_SMotor_UpdateState();
        
        double current_yaw = GetYaw();
        double current_pitch = GetPitch();
        
        float yaw_error = target_yaw - current_yaw;
        float pitch_error = target_pitch - current_pitch;
        
        // 检查是否到达目标位置
        if(fabs(yaw_error) < tolerance && fabs(pitch_error) < tolerance) {
            YP_SMotor_SetSpeed(0.0f, 0.0f);
            break;
        }
        
        // 比例控制
        float yaw_speed = yaw_error * 1.5f;
        float pitch_speed = pitch_error * 1.5f;
        
        // 限制最大速度
        if(yaw_speed > max_speed) yaw_speed = max_speed;
        if(yaw_speed < -max_speed) yaw_speed = -max_speed;
        if(pitch_speed > max_speed) pitch_speed = max_speed;
        if(pitch_speed < -max_speed) pitch_speed = -max_speed;
        
        YP_SMotor_SetSpeed(yaw_speed, pitch_speed);
        
        DL_Common_delayCycles(160000);  // 5ms延时
    }
}

/**
 * @brief 辅助函数：更新太阳位置 (简化算法)
 * @param azimuth 方位角指针
 * @param elevation 高度角指针
 */
void update_sun_position(float *azimuth, float *elevation) {
    // 这里是一个简化的太阳位置计算
    // 实际应用中需要根据时间、日期、地理位置计算
    static uint32_t time_counter = 0;
    time_counter++;
    
    *azimuth = 90.0f * sin(time_counter * 0.01f);      // 简化的方位角变化
    *elevation = 30.0f + 15.0f * cos(time_counter * 0.005f); // 简化的高度角变化
}

/**
 * @brief 双轴步进电机基本使用示例
 */
void YP_SMotor_Basic_Example(void)
{
    // 1. 初始化双轴步进电机系统
    YP_SMotor_Init();
    
    // 2. 基本运动控制
    // YAW轴正向旋转90度/秒, PITCH轴负向旋转45度/秒
    YP_SMotor_SetSpeed(90.0f, -45.0f);
    
    // 运行2秒
    DL_Common_delayCycles(64000000);  // 2秒延时 (32MHz * 2)
    
    // 3. 改变运动方向
    // YAW轴负向旋转60度/秒, PITCH轴正向旋转30度/秒
    YP_SMotor_SetSpeed(-60.0f, 30.0f);
    
    // 运行1.5秒
    DL_Common_delayCycles(48000000);  // 1.5秒延时
    
    // 4. 停止所有运动
    YP_SMotor_SetSpeed(0.0f, 0.0f);
    
    // 5. 主循环中定期更新状态
    while(1) {
        YP_SMotor_UpdateState();
        
        // 获取当前位置
        double current_yaw = GetYaw();
        double current_pitch = GetPitch();
        
        // 这里可以添加位置反馈控制逻辑
        // ...
        
        DL_Common_delayCycles(320000);  // 10ms延时
    }
}

/**
 * @brief 云台扫描模式示例
 * @note 在YAW和PITCH轴之间进行周期性扫描
 */
void YP_SMotor_Scan_Example(void)
{
    YP_SMotor_Init();
    
    float yaw_target = 90.0f;      // YAW目标角度
    float pitch_target = 45.0f;    // PITCH目标角度
    float scan_speed = 30.0f;      // 扫描速度
    
    uint32_t scan_time = 0;
    
    while(1) {
        YP_SMotor_UpdateState();
        
        double current_yaw = GetYaw();
        double current_pitch = GetPitch();
        
        // 简单的位置控制逻辑
        float yaw_error = yaw_target - current_yaw;
        float pitch_error = pitch_target - current_pitch;
        
        // 根据误差设置速度
        float yaw_speed = (fabs(yaw_error) > 5.0f) ? 
                         ((yaw_error > 0) ? scan_speed : -scan_speed) : 0.0f;
        float pitch_speed = (fabs(pitch_error) > 5.0f) ? 
                           ((pitch_error > 0) ? scan_speed : -scan_speed) : 0.0f;
        
        YP_SMotor_SetSpeed(yaw_speed, pitch_speed);
        
        // 每5秒改变目标位置
        scan_time++;
        if(scan_time > 500) {  // 5秒 (假设10ms循环)
            yaw_target = -yaw_target;      // 反向YAW目标
            pitch_target = -pitch_target;  // 反向PITCH目标
            scan_time = 0;
        }
        
        DL_Common_delayCycles(320000);  // 10ms延时
    }
}

/**
 * @brief 精确位置控制示例
 * @param target_yaw 目标YAW角度
 * @param target_pitch 目标PITCH角度
 * @param tolerance 位置容差 (度)
 */
void YP_SMotor_Position_Control(float target_yaw, float target_pitch, float tolerance)
{
    float max_speed = 90.0f;  // 最大速度
    
    while(1) {
        YP_SMotor_UpdateState();
        
        double current_yaw = GetYaw();
        double current_pitch = GetPitch();
        
        float yaw_error = target_yaw - current_yaw;
        float pitch_error = target_pitch - current_pitch;
        
        // 检查是否到达目标位置
        if(fabs(yaw_error) < tolerance && fabs(pitch_error) < tolerance) {
            YP_SMotor_SetSpeed(0.0f, 0.0f);  // 停止运动
            break;  // 退出控制循环
        }
        
        // 比例控制算法
        float yaw_speed = (yaw_error / 180.0f) * max_speed;   // 简单比例控制
        float pitch_speed = (pitch_error / 90.0f) * max_speed;
        
        // 限制最大速度
        if(yaw_speed > max_speed) yaw_speed = max_speed;
        if(yaw_speed < -max_speed) yaw_speed = -max_speed;
        if(pitch_speed > max_speed) pitch_speed = max_speed;
        if(pitch_speed < -max_speed) pitch_speed = -max_speed;
        
        YP_SMotor_SetSpeed(yaw_speed, pitch_speed);
        
        DL_Common_delayCycles(160000);  // 5ms延时
    }
}

/**
 * @brief 电机状态监控示例
 */
void YP_SMotor_Monitor_Example(void)
{
    YP_SMotor_Init();
    
    uint32_t monitor_counter = 0;
    
    // 启动一个简单的运动
    YP_SMotor_SetSpeed(45.0f, 30.0f);
    
    while(1) {
        YP_SMotor_UpdateState();
        
        // 每秒输出一次状态信息 (这里仅作示例，实际使用中可通过串口输出)
        monitor_counter++;
        if(monitor_counter >= 100) {  // 假设10ms循环，100次 = 1秒
            double yaw_pos = GetYaw();
            double pitch_pos = GetPitch();
            
            // 这里可以通过串口或其他方式输出状态信息
            // printf("YAW: %.2f°, PITCH: %.2f°\n", yaw_pos, pitch_pos);
            
            monitor_counter = 0;
        }
        
        DL_Common_delayCycles(320000);  // 10ms延时
    }
}
