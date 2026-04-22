/*
		Author Qinghan Yang
		Date 6.9.2025
		
		Program for controlling motor by tb6612fng
		Structure and enum definitions are in .h file.
		Functions are in .c files.
		
		Ported to TI MSPM0G3507 platform
*/

#ifndef TB6612FNG_H
#define TB6612FNG_H

#include "ti_msp_dl_config.h"

typedef enum{
	FOR = 0,
	BACK = 1,
	BREAK = 2,//Stop in a higher speed, like breaking(Quick stop)
	SLIDE = 3,//Stop in a lower speed(Slide
	SLEEP = 4 //Out of control,which uses less energy. And either motor or Hall encoder are out of control.
}MOVETYPE;

typedef struct {
    GPIO_Regs* port;        // GPIO端口 (GPIOA, GPIOB等)
    uint32_t pin;           // GPIO引脚掩码 (DL_GPIO_PIN_x)
}GPIO_Pin;

typedef struct {
	GPTIMER_Regs* pwm_timer;     // 定时器实例
    DL_TIMER_CC_INDEX pwm_channel; // PWM通道索引             
    uint16_t current_duty;            //0-1000 (determined by your pwm timer counter period.
}PWM;

typedef struct {
	double reduce; //The reduce ratio of the motor
	double full_speed_rpm; //The full speed of the motor in r/min
	int wheel_diameter; //The diameter of the wheel in mm
}MotorParam;

typedef struct{
	GPIO_Pin p;
	GPIO_Pin n;
	PWM speed;
	MotorParam param; //The parameter of the motor, like reduce ratio and full speed
}Motor;


#ifdef __cplusplus
extern "C" {
#endif

void Motor_Param_Init(Motor* M,	//In the Init function, the motor was set in sleep modle.
		double reduce, //The reduce ratio of the motor
		double full_speed_rpm, //The full speed of the motor in r/min
		int wheel_diameter); //The diameter of the wheel in mm
void Motor_UI_Init(Motor* M,	//In the Init function, the motor was set in sleep modle.
		GPIO_Regs* p_port, //The GPIO port of the positive pin. Example: GPIOA
		uint32_t p_pin,				//The GPIO pin mask. Example: DL_GPIO_PIN_1
		GPIO_Regs* n_port,
		uint32_t n_pin,
		GPTIMER_Regs* pwm_timer,  //The pointer of pwm_timer
		DL_TIMER_CC_INDEX pwm_channel,          //The channel of pwm pin you use
		uint16_t current_duty);				 //Initial value of the duty.

void Motor_UI_Set(MOVETYPE type, uint16_t duty, Motor* M);
	//duty is the value between 1 to 1000.
	//Regarding that the speed and the duty is not in perfect linear function, 
	//I use integer variable(duty) instead of double variable(speed) to control the speed.	

void MotorSet(MOVETYPE type, double speed, Motor* M);
//speed is in m/s, and the return value is in 1-1000.
int speed_to_duty(double speed, Motor* M);
//This function is used to convert the speed to duty, which is used in MotorSet function
#endif
