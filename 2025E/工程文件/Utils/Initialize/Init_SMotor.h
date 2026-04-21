#ifndef INIT_SMOTOR_H
#define INIT_SMOTOR_H

#include "SMotor.h"
#include "ti_msp_dl_config.h"

// YAW步进电机配置 (偏航控制)
#define YAW_SMOTOR_PWM_TIMER SMotor_2_INST                    // PWM定时器实例
#define YAW_SMOTOR_PWM_CHANNEL DL_TIMER_CC_1_INDEX         // PWM通道
#define YAW_SMOTOR_DIR_PORT SMotor_IO_PORT                   // 方向控制GPIO端口
#define YAW_SMOTOR_DIR_PIN SMotor_IO_DIR2_PIN                   // 方向控制引脚
#define YAW_SMOTOR_ANTI_DIR SMOTOR_DIR_HIGH                // 反向状态定义
#define YAW_SMOTOR_STEP_ANGULAR 1.8f                      // 步进角度
#define YAW_SMOTOR_STEP_DIVISOR 32.0f                      // 32细分

// PITCH步进电机配置 (俯仰控制)  
#define PITCH_SMOTOR_PWM_TIMER   SMotor_1_INST        // PWM定时器实例
#define PITCH_SMOTOR_PWM_CHANNEL DL_TIMER_CC_0_INDEX       // PWM通道
#define PITCH_SMOTOR_DIR_PORT SMotor_IO_PORT                 // 方向控制GPIO端口
#define PITCH_SMOTOR_DIR_PIN SMotor_IO_DIR1_PIN              // 方向控制引脚
#define PITCH_SMOTOR_ANTI_DIR SMOTOR_DIR_LOW               // 正向状态定义
#define PITCH_SMOTOR_STEP_ANGULAR 1.8f                    // 步进角度
#define PITCH_SMOTOR_STEP_DIVISOR 32.0f                    // 32细分

/**
 * @brief 初始化YAW和PITCH步进电机
 * @note 需要在SysConfig中配置对应的PWM定时器和GPIO
 */
void YP_SMotor_Init(void);

/**
 * @brief 设置YAW和PITCH步进电机速度
 * @param yaw_speed YAW电机角速度 (度/秒)
 * @param pitch_speed PITCH电机角速度 (度/秒)
 */
void YP_SMotor_SetSpeed(float yaw_speed, float pitch_speed);

/**
 * @brief 更新YAW和PITCH步进电机状态
 * @note 应在主循环中定期调用
 */
void YP_SMotor_UpdateState(void);

/**
 * @brief 获取当前YAW角度
 * @return 当前YAW位置 (度)
 */
double GetYaw(void);

/**
 * @brief 获取当前PITCH角度  
 * @return 当前PITCH位置 (度)
 */
double GetPitch(void);

#endif // INIT_SMOTOR_H