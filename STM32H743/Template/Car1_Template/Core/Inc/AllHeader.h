#ifndef ALLHEADER_H
#define ALLHEADER_H

#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "stm32h7xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// JY61P 传感器头文件
#include "REG.h"
#include "wit_c_sdk.h"

// 感为灰度传感器头文件
#include "GanWei_i2c.h"
#include "gw_grayscale_sensor.h"

//巡线程序头文件
#include "tracking.h"
#include "tracking_delay.h"

// 电机驱动头文件
#include "tb6612fng.h"

// 霍尔编码器头文件
#include "Hall_Encoder.h"

// 内存管理头文件
#include "Mem.h"

// 电机和编码器初始化头文件（内部包括了运动控制头文件
#include "Initialize.h"

//pid基本函数头文件
#include "pid.h"

//OLED显示屏头文件
#include "oled.h"

//视觉通道头文件
#include "visual.h"

// led头文件
#include "led.h"

// 传感器信息处理头文件
#include "SensorProc.h"
#include "Rotation.h"

#ifdef __cplusplus
extern "C" {
#endif

#endif /* ALLHEADER_H */