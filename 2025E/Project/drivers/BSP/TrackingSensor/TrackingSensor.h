/**
 * @file  TrackingSensor.h
 * @brief 8 路灰度循迹传感器接口
 */
#ifndef TRACKING_SENSOR_H
#define TRACKING_SENSOR_H

#include <stdint.h>

void TrackingSensor_Read(uint8_t digitalArray[8]);

#endif /* TRACKING_SENSOR_H */
