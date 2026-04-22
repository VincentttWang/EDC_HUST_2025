#include <ti/devices/msp/msp.h>
#include "ti_msp_dl_config.h"
#include "Delay.h"

extern uint32_t tick;

void Delay_us(uint32_t xus)
{
    uint32_t start_tick = SysTick->VAL;
    uint32_t ticks_per_us = 32;
    uint32_t ticks_to_wait = xus * ticks_per_us;
    uint32_t elapsed_ticks = 0;

    if (ticks_to_wait == 0) return;

    do {
        uint32_t current_tick = SysTick->VAL;
        if (current_tick > start_tick)
            elapsed_ticks += (start_tick + SysTick->LOAD + 1 - current_tick);
        else
            elapsed_ticks += (start_tick - current_tick);
        start_tick = current_tick;
    } while (elapsed_ticks < ticks_to_wait);
}

void Delay_ms(uint32_t xms)
{
    uint32_t start_time;
    __disable_irq();
    start_time = tick;
    __enable_irq();

    while (1) {
        uint32_t current_tick;
        __disable_irq();
        current_tick = tick;
        __enable_irq();
        if ((uint32_t)(current_tick - start_time) >= xms)
            break;
    }
}
