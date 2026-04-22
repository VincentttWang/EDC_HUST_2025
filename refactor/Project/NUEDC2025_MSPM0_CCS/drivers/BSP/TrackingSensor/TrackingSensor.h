#ifndef TRACKING_H
#define TRACKING_H

#include <stdint.h> // 包含stdint.h以使用uint8_t类型

// 获取循迹传感器数据函数
void getTrackingSensorData(uint8_t digitalArray[8]);

#endif // TRACKING_H