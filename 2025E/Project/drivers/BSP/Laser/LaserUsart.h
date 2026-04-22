/**
 * @file  LaserUsart.h
 * @brief CanMV 视觉模块 UART 通信接口，提供坐标解析与目标定位
 */
#ifndef LASER_USART_H
#define LASER_USART_H

#include "ti_msp_dl_config.h"
#include "BspCommon.h"
#include <stdbool.h>
#include <string.h>

#define USART_LASER_RX_BUF_LEN 100

#define LASER_UART    UART2
#define LASER_INI_IRQn UART2_INT_IRQn

#define Laser_Begin   2
#define Laser_RX_Num  8

#define Rect_Begin    10
#define Rect_RX_Num   16

void Laser_USART_Init(void);
void Laser_SendChar(char ch);
void Laser_SendString(char *str);

void Laser_GetLoc(uint16_t Loc[]);
void Rect_GetLoc(uint16_t Loc[]);
void CanMV_Process(void);

#endif /* LASER_USART_H */
