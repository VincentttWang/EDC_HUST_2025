/*
		Author Qinghan Yang
		Date 6.9.2025
		
		Program for controlling motor by tb6612fng
		Structure and enum definitions are in .h file.
		Functions are in .c files.
*/

#ifndef TB6612FNG_H
#define TB6612FNG_H

#include "main.h"

typedef enum{
	FOR = 0,
	BACK = 1,
	BREAK = 2,//Stop in a higher speed, like breaking(Quick stop)
	SLIDE = 3,//Stop in a lower speed(Slide
	SLEEP = 4 //Out of control,which uses less energy. And either motor or Hall encoder are out of control.
}MOVETYPE;

typedef struct {
    GPIO_TypeDef* port;
    uint16_t pin;
}GPIO_Pin;

typedef struct {
	TIM_HandleTypeDef* pwm_timer;     // timer
    uint32_t pwm_channel;             
    uint16_t current_duty;            //0-1000 (determined by your pwm timer counter period.
}PWM;

typedef struct{
	GPIO_Pin p;
	GPIO_Pin n;
	PWM speed;
}Motor;


#ifdef __cplusplus
extern "C" {
#endif

void MotorInit(Motor* M,
		GPIO_TypeDef* p_port,
		uint16_t p_pin,
		GPIO_TypeDef* n_port,
		uint16_t n_pin,
		TIM_HandleTypeDef* pwm_timer,    
    uint32_t pwm_channel,             
    uint16_t current_duty);

void MotorSet(MOVETYPE type, uint16_t duty, Motor* M);

#endif
