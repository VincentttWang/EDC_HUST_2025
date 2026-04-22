#include "tracking_delay.h"
#include <ti/devices/msp/msp.h>

void delay_us(uint32_t nus)
{
    uint32_t start_tick = SysTick->VAL;
    uint32_t ticks_per_us = 32;
    uint32_t ticks_to_wait = nus * ticks_per_us;
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
