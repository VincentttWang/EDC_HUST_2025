#ifndef __BSP_MOTOR_IIC_H_
#define __BSP_MOTOR_IIC_H_

#include "AllHeader.h"

#define Motor_model_ADDR    0x26


typedef enum __Motor_IIC_ADDR_
{
	//Wirte Reg
	MOTOR_TYPE_REG = 0x01, //电机类型	Motor Type
	MOTOR_DeadZONE_REG = 0x02, //死区配置	Dead zone configuration
	MOTOR_PluseLine_REG = 0x03, //磁环线数	Magnetic loop number
	MOTOR_PlusePhase_REG = 0x04, //减速比	Reduction ratio
	WHEEL_DIA_REG = 0x05, //轮子直径	Wheel diameter
	SPEED_Control_REG = 0x06, //速度控制	Speed control
	PWM_Control_REG = 0x07, //pwm控制	PWM control
	
	
	//Read Reg
	READ_TEN_M1Enconer_REG = 0x10, //10ms实时编码器读取M1	10ms real-time encoder reading M1
	READ_TEN_M2Enconer_REG = 0x11, //10ms实时编码器读取M2	10ms real-time encoder reading M2
	READ_TEN_M3Enconer_REG = 0x12, //10ms实时编码器读取M3	10ms real-time encoder reading M3
	READ_TEN_M4Enconer_REG = 0x13, //10ms实时编码器读取M4	10ms real-time encoder reading M4
	
	READ_ALLHigh_M1_REG = 0x20, //读取M1总的脉冲数 高位	Read the total number of pulses of M1 High
	READ_ALLLOW_M1_REG = 0x21, //读取M1总的脉冲数 低位	Read the total number of pulses of M1 low
	
	READ_ALLHigh_M2_REG = 0x22, //读取M2总的脉冲数 高位	Read the total number of pulses of M2 High
	READ_ALLLOW_M2_REG = 0x23,  //读取M2总的脉冲数 低位	Read the total number of pulses of M2 low
	
	READ_ALLHigh_M3_REG = 0x24, //读取M3总的脉冲数 高位	Read the total number of pulses of M3 High
	READ_ALLLOW_M3_REG = 0x25, //读取M3总的脉冲数 低位	Read the total number of pulses of M3 low
	
	READ_ALLHigh_M4_REG = 0x26, //读取M4总的脉冲数 高位	Read the total number of pulses of M4 High
	READ_ALLLOW_M4_REG = 0x27, //读取M4总的脉冲数 低位	Read the total number of pulses of M4 low
	
	IIC_REG_MAX //最大的	The largest
	

}Motor_IIC_ADDR_t;


//引出编码器变量，供外部使用	Lead out encoder variables for external use
extern int Encoder_Offset[4];
extern int Encoder_Now[4];



void IIC_Motor_Init(void);
void control_speed(int16_t m1,int16_t m2 ,int16_t m3,int16_t m4);
void control_pwm(int16_t m1,int16_t m2 ,int16_t m3,int16_t m4);
void Set_motor_type(uint8_t data);
void Read_10_Enconder(void);
void Read_ALL_Enconder(void);
void Set_motor_deadzone(uint16_t data);
void Set_Pluse_line(uint16_t data);
void Set_Pluse_Phase(uint16_t data);
void Set_Wheel_dis(float data);

#endif
