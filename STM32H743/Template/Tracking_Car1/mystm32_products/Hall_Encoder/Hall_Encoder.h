#ifndef HALL_ENCODER_H
#define HALL_ENCODER_H

#include "main.h"
#include "tim.h"
#include "Mem.h"

#define WHEELLENGTH 66e-3 //The length of wheel
#define PPR 360 // The Pulses Per Revolution of encoder
#define PI 3.1415926 // Pi constant
#define WHEEL_CIRCUMFERENCE (WHEELLENGTH * PI) // The circumference of wheel
// The speed of wheel in m/s
// The speed of wheel in m/s is computed by (length of the circle) * (frequence of the clock / (leftSpeed add number for every single circle
// which is (WHEELLENGTH * PI * (240e6 / ((65535 + 1) * (239 + 1)) / (PPR * 2))
#define MAXENCODERNUM 10 // The maximum number of encoders that can be used in this code

typedef struct {
	int speed; // The speed of wheel in m/s
	TIM_HandleTypeDef *timer; // The timer used to measure the speed
	TIM_HandleTypeDef *realTimer; // The timer used to measure real time(reload time)
	uint32_t reloadFre; // The frequency of the timer used to measure real time
} EncoderSpeed;


#ifdef __cplusplus
extern "C" {
#endif

// ...existing code...

void EncoderInit(TIM_HandleTypeDef *Timer, uint32_t chan1, uint32_t chan2, TIM_HandleTypeDef *realTimer);
int getTIMx_DetaCnt(TIM_HandleTypeDef *htim);
void UpdateSpeed(int i, TIM_HandleTypeDef *reload_tim);
void UpdateAllSpeed(TIM_HandleTypeDef *reload_tim);
double getSpeed(int index);
uint32_t ReloadTime(TIM_TypeDef* tim);

#endif // HALL_ENCODER_H
