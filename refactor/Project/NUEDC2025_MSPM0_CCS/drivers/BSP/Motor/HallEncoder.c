#include "HallEncoder.h"

static EncoderState motor_encoder;

void Encoder_Init(void)
{
    NVIC_ClearPendingIRQ(GPIOA_INT_IRQn);
    NVIC_EnableIRQ(GPIOA_INT_IRQn);
}

int Encoder_GetCount(void)
{
    return motor_encoder.count;
}

EncoderDir Encoder_GetDir(void)
{
    return motor_encoder.dir;
}

void Encoder_Update(void)
{
    motor_encoder.count = motor_encoder.temp_count;
    motor_encoder.dir = (motor_encoder.count >= 0) ? ENCODER_DIR_FORWARD : ENCODER_DIR_REVERSE;
    motor_encoder.temp_count = 0;
}

void GROUP1_IRQHandler(void)
{
    uint32_t gpio_status;
    gpio_status = DL_GPIO_getEnabledInterruptStatus(Motor_IO_E1A_PORT, Motor_IO_E1A_PIN | Motor_IO_E2A_PIN);

    if ((gpio_status & Motor_IO_E1A_PIN) == Motor_IO_E1A_PIN) {
        if (!DL_GPIO_readPins(Motor_IO_E1A_PORT, Motor_IO_E2A_PIN))
            motor_encoder.temp_count--;
        else
            motor_encoder.temp_count++;
    } else if ((gpio_status & Motor_IO_E2A_PIN) == Motor_IO_E2A_PIN) {
        if (!DL_GPIO_readPins(Motor_IO_E1A_PORT, Motor_IO_E1A_PIN))
            motor_encoder.temp_count++;
        else
            motor_encoder.temp_count--;
    }

    DL_GPIO_clearInterruptStatus(Motor_IO_E1A_PORT, Motor_IO_E1A_PIN | Motor_IO_E2A_PIN);
}

void Encoder_TimerInit(void)
{
    NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
    NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
}

void TIMER_0_INST_IRQHandler(void)
{
    if (DL_TimerA_getPendingInterrupt(TIMER_0_INST) == DL_TIMER_IIDX_ZERO)
        Encoder_Update();
}

double Encoder_GetSpeed(void)
{
    int speed = Encoder_GetCount();
    int dir = (Encoder_GetDir() == ENCODER_DIR_FORWARD) ? 1 : -1;
    return speed * 0.0561 * dir * 0.108 * 1.05;
}
