#include "AllHeader.h"
#include "LaserUsart.h"

extern CanMV_Error Laser_error;
extern CanMV_Error Rect_error;
extern uint16_t Laser_Loc[Laser_RX_Num / 2];
extern uint16_t Rect_Loc[Rect_RX_Num / 2];

uint8_t Digtal[8];
uint32_t tick;

int main(void)
{
    SYSCFG_DL_init();
    __enable_irq();

    Encoder_Init();
    Encoder_TimerInit();
    DL_TimerG_startCounter(TIMER_0_INST);

    Laser_USART_Init();
    OLED_Init();
    Motor_SystemInit();
    Key_Init();

    mode_problem_h_2();

    while (1)
    {
    }
}

void UART2_IRQHandler(void)
{
    switch (DL_UART_getPendingInterrupt(LASER_UART)) {
        case DL_UART_IIDX_RX:
            CanMV_Process();
            break;
        default:
            break;
    }
}

void SysTick_Handler(void)
{
    static int k = 0;
    k++;
    tick++;
    if (k == 10) {
        Key_Scan();
        k = 0;
    }
}
