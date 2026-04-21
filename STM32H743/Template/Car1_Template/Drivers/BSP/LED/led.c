#include "led.h"

void RED_up()
{
	HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, GPIO_PIN_SET);
}
void RED_down()
{
    HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, GPIO_PIN_RESET);
}
void GREEN_up()
{
    HAL_GPIO_WritePin(GREEN_GPIO_Port, GREEN_Pin, GPIO_PIN_SET);
}
void GREEN_down()
{
    HAL_GPIO_WritePin(GREEN_GPIO_Port, GREEN_Pin, GPIO_PIN_RESET);
}
void YELLOW_up()
{
    HAL_GPIO_WritePin(YELLOW_GPIO_Port, YELLOW_Pin, GPIO_PIN_SET);
}
void YELLOW_down()
{
    HAL_GPIO_WritePin(YELLOW_GPIO_Port, YELLOW_Pin, GPIO_PIN_RESET);
}

void Alldown()
{
    RED_down();
    GREEN_down();
    YELLOW_down();
}

void SwitchLED(short led)
{
    Alldown(); // 先关闭所有LED灯
    switch(led)
    {
        case 0:
            RED_up();
            break;
        case 1:
            GREEN_up();
            break;
        case 2:
            YELLOW_up();
            break;
        default:
            break;
    }
}