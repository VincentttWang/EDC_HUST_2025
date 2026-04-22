#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <stdbool.h>

#define PI 3.1416

#define DEG_TO_RAD(degrees) ((degrees) * (PI / 180.0f))
// 弧度到角度的转换宏 (如果您也需要)
#define RAD_TO_DEG(radians) ((radians) * (180.0f / PI))

#define STOD (999/1.30)

typedef enum {
    LEFT = 0,
    RIGHT = 1,
    FORWARD,
    BACKWARD,
    UNSTABLE,
}DIR;

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float yaw;   // 偏航角
    float pitch; // 俯仰角
    float roll;  // 翻滚角
} RotationAngles;

typedef struct{
    float x; // x轴位置
    float y; // y轴位置
    float theta; // 方向角
    float initial_theta; // 初始方向角
}Pose;

typedef struct{
    float linear_velocity; // 线速度
    float angular_velocity; // 角速度
}Speed;

typedef struct{
    float left_wheel_speed; // 左轮速度
    float right_wheel_speed; // 右轮速度
}WheelSpeed;

typedef struct{
    Speed speed; // 线速度和角速度
    float yaw;//RotationAngles angles; // 方向角
	float dt;
}Data; //这个数据结构用于从编码器和陀螺仪中获取数据

typedef struct{
    Pose pose; // 位置和方向
    Speed speed; // 线速度和角速度
    WheelSpeed wheel_speed; // 左右轮速度
}CarState;

//小车状态处理函数声明
void CarState_Init(CarState *state);
void CarState_Update(CarState *state,  Data d);

// 小车速度换算函数
WheelSpeed SpeedToWheelSpeed(Speed speed);
float sumTheta(float, float);

// 运动学函数声明
Speed PID_Move(float v, float w, short isreload);
bool runCircle(float radius, float speed, float angle, DIR dir); // 圆周运动函数
bool Straight(float distance, float speed, float target_theta, DIR dir); // 直行函数
void track(float linear_velocity); // 巡线函数

#endif