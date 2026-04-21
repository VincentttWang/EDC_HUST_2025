#include "Hall_Encoder.h"

EncoderSpeed encoders[MAXENCODERNUM]; // Array to store encoder data
int numEncoders = 0; // Number of encoders initialized
int LRFlag = 0; // Flag to indicate if left and right encoders are initialized

void EncoderInit(TIM_HandleTypeDef *Timer, uint32_t chan1, uint32_t chan2, TIM_HandleTypeDef *realTimer,
	int wheelLength, int ppr)
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
	encoders[numEncoders].speed = 0; // Initialize speed to 0
	encoders[numEncoders].dis = 0.0; // Initialize distance to 0
	encoders[numEncoders].timer->Instance->CNT = 0;
  encoders[numEncoders].prev_cnt = encoders[numEncoders].timer->Instance->CNT; // 初始化 prev_cnt


	EncoderParamInit(&encoders[numEncoders].param, wheelLength, ppr);
	numEncoders++;
	return;
}

void EncoderParamInit(EncoderParam *param, int wheelLength, int ppr)
	//set the parameter of the encoder
{
	if (param == NULL)
	{
		return; // Parameter pointer is NULL
	}
	param->wheelLength = wheelLength; // Set wheel length in mm
	param->ppr = ppr; // Set Pulses Per Revolution
}

void LRInit(TIM_HandleTypeDef *LTimer, uint32_t Lchan1, uint32_t Lchan2,//The first two parameters are for left encoder
			TIM_HandleTypeDef *RTimer, uint32_t Rchan1, uint32_t Rchan2,//The second two parameters are for right encoder
			int wheelLength, int ppr, // The last two parameters are for encoder parameters
			TIM_HandleTypeDef *realTimer)
	//set global variables for left and right encoders
{
	EncoderInit(LTimer, Lchan1, Lchan2, realTimer, wheelLength, ppr);
	EncoderInit(RTimer, Rchan1, Rchan2, realTimer, wheelLength, ppr);
	
	LRFlag = 1; // Set flag to indicate left and right encoders are initialized
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
		 int16_t current_cnt = encoders[i].timer->Instance->CNT; // 读取当前计数值
        // 计算增量，处理潜在的 16 位定时器溢出/下溢
        // 假设使用 16 位定时器，且计数值在 int16_t 范围内
     encoders[i].speed = (int16_t)(current_cnt - encoders[i].prev_cnt);

     encoders[i].prev_cnt = current_cnt; // 更新前一个计数值，供下次迭代使用
     encoders[i].dis += StoDis(encoders[i]);
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

double StoDis(EncoderSpeed e)
{
	return e.speed * e.param.wheelLength * 1e-3 * PI 
	/ (e.param.ppr * 2);
	// Compute the distance traveled by the wheel
}

//Get wheels' speed with the unit of m/s
//'+' means forwards and '-' means backwards.
//When you are using the code, please connect PE9 to E1A, PE11 to E1B, 
double getSpeed(int index)
{
	return encoders[index].speed * 
		encoders[index].param.wheelLength * 1e-3 * PI * 
		encoders[index].reloadFre / (encoders[index].param.ppr * 2);
		//4.394215799e-3;
		// Left number is computed by (length of the circle) * (frequence of the clock / (leftSpeed add number for every single circle
		// which is (6.6e-2 * pi * (240e6 / ((65535 + 1) * (239 + 1)) / (360 * 2))
}

double getDis(int index)
{
	return encoders[index].dis; // Return the distance traveled by the wheel
	// The distance is already updated in UpdateSpeed function
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

double lDis()
{
	if(LRFlag == 0)
	{
		return 0.0; // No left encoder initialized
	}
	return getDis(0); // Distance traveled by left wheel
}
double rDis()
{
	if(LRFlag == 0)
	{
		return 0.0; // No right encoder initialized
	}
	return getDis(1); // Distance traveled by right wheel
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
