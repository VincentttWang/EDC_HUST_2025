#ifndef KEY_H
#define KEY_H

#include "stm32h7xx_hal.h"
#include "gpio.h"
#include <stdbool.h>
#include <stdint.h>

#define KEY1_Port GPIOC
#define KEY1_Pin GPIO_PIN_2
#define KEY2_Port GPIOC
#define KEY2_Pin GPIO_PIN_3

#define KEY_PRESSED GPIO_PIN_RESET
/**
 * @brief Reads the state of Key 1.
 * @return true if Key 1 is pressed, false otherwise.
 */

bool Key1_Read(void);
bool Key2_Read(void);
bool Key1_short_press(void);
bool Key2_short_press(void);

#endif // KEY_H