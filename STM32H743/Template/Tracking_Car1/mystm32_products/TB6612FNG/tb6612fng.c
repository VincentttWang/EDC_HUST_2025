/*
		Author Qinghan Yang
		Date 6.9.2025
		
		Program for controlling motor by tb6612fng
		Structure and enum definitions are in .h file.
		Functions are in .c files.
*/

#include "tb6612fng.h"

void MotorInit(Motor* M,	//In the Init function, the motor was set in sleep modle.
		GPIO_TypeDef* p_port, //The GPIO group of the positive pin. Example: GPIOA for PA1
		uint16_t p_pin,				//The number of pin. Example: GPIO_PIN_1 for PA1
		GPIO_TypeDef* n_port,
		uint16_t n_pin,
		TIM_HandleTypeDef* pwm_timer,  //The pointer of pwm_timer  
    uint32_t pwm_channel,          //The channel of pwm pin you use
    uint16_t current_duty)				 //Initial value of the duty.
{
	if(M == NULL || pwm_timer == NULL)
	{
		//HAL_UART_Transmit(&huart1, (uint8_t *)"test3", 5, 1000);
		//If you need test, please use these sentence and exchange huart1 to your setted huart.
		return;
	}
	M->p.port = p_port;
	M->p.pin = p_pin;
	M->n.port = n_port;
	M->n.pin = n_pin;
	M->speed.pwm_timer = pwm_timer;
	M->speed.pwm_channel = pwm_channel;
	M->speed.current_duty = current_duty;
	

	HAL_GPIO_WritePin(M->n.port, M->n.pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(M->p.port, M->p.pin, GPIO_PIN_RESET);
	HAL_TIM_PWM_Start(M->speed.pwm_timer, M->speed.pwm_channel);
	__HAL_TIM_SET_COMPARE(M->speed.pwm_timer, M->speed.pwm_channel, M->speed.current_duty);
}
	
void MotorSet(MOVETYPE type, uint16_t duty, Motor* M)
	//duty is the value between 1 to 1000.
	//Regarding that the speed and the duty is not in perfect linear function, 
	//I use integer variable(duty) instead of float variable(speed) to control the speed.
{
	if (M == NULL) {
        //HAL_UART_Transmit(&huart1, (uint8_t*)"NULL POINTER DETECTED!\r\n", 25, 1000); 
				//If you need test, please use these sentence and exchange huart1 to your setted huart.
        return;
    }
	switch(type)
	{
		case FOR:
			HAL_GPIO_WritePin(M->p.port, M->p.pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(M->n.port, M->n.pin, GPIO_PIN_RESET);
		break;
		case BACK:
			HAL_GPIO_WritePin(M->p.port, M->p.pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(M->n.port, M->n.pin, GPIO_PIN_SET);
		break;
		case BREAK:
			HAL_GPIO_WritePin(M->p.port, M->p.pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(M->n.port, M->n.pin, GPIO_PIN_SET);
		break;
		case SLIDE:
			HAL_GPIO_WritePin(M->p.port, M->p.pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(M->n.port, M->n.pin, GPIO_PIN_RESET);
		break;
		case SLEEP:
		break;
	}
	M->speed.current_duty = duty;
	__HAL_TIM_SET_COMPARE(M->speed.pwm_timer, M->speed.pwm_channel, M->speed.current_duty);
	return;
}
