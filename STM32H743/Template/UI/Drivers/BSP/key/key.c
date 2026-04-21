#include "key.h"

/**
  ******************************************************************************
  * @author  Qinghan Yang
  * @date    2025-07-29
  * @file    key.c
  * @brief   This file contains the implementation of key handling for the
  *          peripheral devices UI drivers.
  ******************************************************************************
  * @attention
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

bool Key1_Read(void) {
    return HAL_GPIO_ReadPin(KEY1_Port, KEY1_Pin) == KEY_PRESSED; // Return true if key 1 is pressed
}

bool Key2_Read(void) {
    return HAL_GPIO_ReadPin(KEY2_Port, KEY2_Pin) == KEY_PRESSED; // Return true if key 2 is pressed
}

bool Key1_short_press(void) {
    static uint32_t last_press_time = 0;
    if (Key1_Read()) {
        if (HAL_GetTick() - last_press_time > 50) { // Check if the key is pressed for more than 50 ms
            last_press_time = HAL_GetTick();
            return true; // Short press detected
        }
    } else {
        last_press_time = HAL_GetTick(); // Reset the timer if the key is not pressed
    }
    return false; // No short press detected
}

bool Key2_short_press(void) {
    static uint32_t last_press_time = 0;
    if (Key2_Read()) {
        if (HAL_GetTick() - last_press_time > 50) { // Check if the key is pressed for more than 50 ms
            last_press_time = HAL_GetTick();
            return true; // Short press detected
        }
    } else {
        last_press_time = HAL_GetTick(); // Reset the timer if the key is not pressed
    }
    return false; // No short press detected
}

