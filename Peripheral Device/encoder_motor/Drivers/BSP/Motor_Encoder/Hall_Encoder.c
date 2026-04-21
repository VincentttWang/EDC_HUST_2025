#include "Hall_Encoder.h"

EncoderSpeed encoders[MAXENCODERNUM]; // Array to store encoder data
int numEncoders = 0; // Number of encoders initialized
int LRFlag = 0; // Flag to indicate if left and right encoders are initialized

void EncoderInit(TIM_HandleTypeDef *Timer, uint32_t chan1, uint32_t chan2, TIM_HandleTypeDef *realTimer)
	//set global variables for encoders
{
	if (numEncoders >= MAXENCODERNUM)
	{
		return; // Maximum number of encoders reached
	}

	encoders[numEncoders].timer = Timer;
	encoders[numEncoders].realTimer = realTimer;
	
	HAL_TIM_PWM_Start(Timer,chan1);//?
	HAL_TIM_PWM_Start(Timer,chan2);//?
		
	__HAL_TIM_CLEAR_FLAG(realTimer,TIM_FLAG_UPDATE);
	HAL_TIM_Base_Start_IT(realTimer);
	
	encoders[numEncoders].reloadFre = ReloadTime(realTimer->Instance);
	numEncoders++;
	return;
}

void LRInit(TIM_HandleTypeDef *LTimer, uint32_t Lchan1, uint32_t Lchan2,//The first two parameters are for left encoder
			TIM_HandleTypeDef *RTimer, uint32_t Rchan1, uint32_t Rchan2,//The second two parameters are for right encoder
			TIM_HandleTypeDef *realTimer)
	//set global variables for left and right encoders
{
	EncoderInit(LTimer, Lchan1, Lchan2, realTimer);
	EncoderInit(RTimer, Rchan1, Rchan2, realTimer);
	
	LRFlag = 1; // Set flag to indicate left and right encoders are initialized
}

int getTIMx_DetaCnt(TIM_HandleTypeDef *htim)
{
    int cnt;
    cnt = htim->Instance->CNT - 0x7FFF;
    htim->Instance->CNT = 0x7FFF;
    return cnt;
}

void UpdateSpeed(int i, TIM_HandleTypeDef *reload_tim)
	//get the speed of the motor
	//E is the encoder speed struct, reload_tim is the timer used to measure real time
{	
	if (encoders[i].timer == NULL || encoders[i].realTimer == NULL)
	{
		return; // Timer not initialized
	}
	if(reload_tim == encoders[i].realTimer)
	{
		encoders[i].speed = getTIMx_DetaCnt(encoders[i].timer);
	}
}

void UpdateAllSpeed(TIM_HandleTypeDef *reload_tim)
	//update all encoders' speed
{
	for (int i = 0; i < numEncoders; i++)
	{
		UpdateSpeed(i, reload_tim);
	}
}

//Get wheels' speed with the unit of m/s
//'+' means forwards and '-' means backwards.
//When you are using the code, please connect PE9 to E1A, PE11 to E1B, 
double getSpeed(int index)
{
	return encoders[index].speed * 
		WHEEL_CIRCUMFERENCE * 
		encoders[index].reloadFre / (PPR * 2);
		//4.394215799e-3;
		// Left number is computed by (length of the circle) * (frequence of the clock / (leftSpeed add number for every single circle
		// which is (6.6e-2 * pi * (240e6 / ((65535 + 1) * (239 + 1)) / (360 * 2))
}

//Functions below are speciafic for left and right encoders
double cSpeed()
{
	if(LRFlag == 0)
	{
		return 0.0; // No left and right encoders initialized
	}
	return (getSpeed(0) + getSpeed(1)) / 2.0; // Average speed of left and right encoders
}
double lSpeed()
{
	if(LRFlag == 0)
	{
		return 0.0; // No left encoder initialized
	}
	return getSpeed(0); // Speed of left encoder
}
double rSpeed()
{
	if(LRFlag == 0)
	{
		return 0.0; // No right encoder initialized
	}
	return getSpeed(1); // Speed of right encoder
}

uint32_t ReloadTime(TIM_TypeDef* tim)
{
	uint32_t pclk1 = HAL_RCC_GetPCLK1Freq();
	uint32_t tim_clk;
	
	uint32_t ppre1_bits = (RCC->CFGR >> 10) & 0x7;

	if (ppre1_bits == 0x0) {
			tim_clk = pclk1 * 2; // APB1 ?? -> ?????
	} else {
			tim_clk = pclk1;
	}
	uint32_t psc = tim->PSC;
	uint32_t arr = tim->ARR;
	
	return tim_clk / ((psc + 1) * (arr + 1));
}
