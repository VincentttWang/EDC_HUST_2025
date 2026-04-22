/**
 * @file  TrackingSensor.c
 * @brief 8 路灰度循迹传感器驱动，读取各通道数字量
 */
#include "TrackingSensor.h"
#include "ti_msp_dl_config.h"

void TrackingSensor_Read(uint8_t digitalArray[8])
{
    digitalArray[7] = (DL_GPIO_readPins(Tracking_Tracking_1_PORT, Tracking_Tracking_1_PIN) == 0);
    digitalArray[6] = (DL_GPIO_readPins(Tracking_Tracking_2_PORT, Tracking_Tracking_2_PIN) == 0);
    digitalArray[5] = (DL_GPIO_readPins(Tracking_Tracking_3_PORT, Tracking_Tracking_3_PIN) == 0);
    digitalArray[4] = (DL_GPIO_readPins(Tracking_Tracking_4_PORT, Tracking_Tracking_4_PIN) == 0);
    digitalArray[3] = (DL_GPIO_readPins(Tracking_Tracking_5_PORT, Tracking_Tracking_5_PIN) == 0);
    digitalArray[2] = (DL_GPIO_readPins(Tracking_Tracking_6_PORT, Tracking_Tracking_6_PIN) == 0);
    digitalArray[1] = (DL_GPIO_readPins(Tracking_Tracking_7_PORT, Tracking_Tracking_7_PIN) == 0);
    digitalArray[0] = (DL_GPIO_readPins(Tracking_Tracking_8_PORT, Tracking_Tracking_8_PIN) == 0);
}
