#ifndef HALL_ENCODER_H
#define HALL_ENCODER_H

#include "main.h"
#include "tim.h"
#include "Mem.h"


#define PI 3.1415926535 // Pi constant
#define MAXENCODERNUM 10 // The maximum number of encoders that can be used in this code

typedef struct{
	int wheelLength; // The length of wheel in mm
	int ppr; // The Pulses Per Revolution of encoder
}EncoderParam;

typedef struct {
	int speed; // The speed of wheel in m/s
	float dis; // The distance traveled by the wheel in m
	TIM_HandleTypeDef *timer; // The timer used to measure the speed
	TIM_HandleTypeDef *realTimer; // The timer used to measure real time(reload time)
	uint32_t reloadFre; // The frequency of the timer used to measure real time
	EncoderParam param; // The parameter of the encoder, like wheel length and ppr
	int16_t prev_cnt;
} EncoderSpeed;


#ifdef __cplusplus
extern "C" {
#endif

// ...existing code...

void EncoderInit(TIM_HandleTypeDef *Timer, uint32_t chan1, uint32_t chan2, TIM_HandleTypeDef *realTimer, 
	int wheelLength, int ppr);
// Initialize the encoder with the given parameters
void EncoderParamInit(EncoderParam *param, int wheelLength, int ppr);
void LRInit(TIM_HandleTypeDef *LTimer, uint32_t Lchan1, uint32_t Lchan2,
			TIM_HandleTypeDef *RTimer, uint32_t Rchan1, uint32_t Rchan2,
			int wheelLength, int ppr, // The last two parameters are for encoder parameters	
			TIM_HandleTypeDef *realTimer);

int getTIMx_DetaCnt(TIM_HandleTypeDef *htim);
void UpdateSpeed(int i, TIM_HandleTypeDef *reload_tim);
void UpdateAllSpeed(TIM_HandleTypeDef *reload_tim);

double getSpeed(int index);
double getDis(int index);
double cSpeed();
double lSpeed();
double rSpeed();
double lDis();
double rDis();

double StoDis(EncoderSpeed e);

uint32_t ReloadTime(TIM_TypeDef* tim);

#endif // HALL_ENCODER_H
