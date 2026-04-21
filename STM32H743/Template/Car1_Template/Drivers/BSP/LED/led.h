#ifndef LED_H
#define LED_H

#include "AllHeader.h"

#ifdef __cplusplus
extern "C" {
#endif

void RED_up();
void RED_down();
void GREEN_up();
void GREEN_down();
void YELLOW_up();
void YELLOW_down();
void Alldown();
void SwitchLED(short led);

#ifdef __cplusplus
}
#endif

#endif /* LED_H */