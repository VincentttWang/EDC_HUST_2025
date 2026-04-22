#ifndef ALLHEADERS_H
#define ALLHEADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "ti_msp_dl_config.h"
#include "OLED.h"
#include "Delay.h"
#include "tb6612fng.h"

//巡线程序头文件
#include "tracking.h"
#include "tracking_delay.h"

//OLED头文件
#include "OLED.h"

// 霍尔编码器头文件
#include "Hall_Encoder.h"

// 电机和编码器初始化头文件（内部包括了运动控制头文件
#include "Initialize.h"

//pid基本函数头文件
#include "pid.h"

// 传感器信息处理头文件
#include "SensorProc.h"
#include "Rotation.h"

#include "SMotor.h"

#include "Init_SMotor.h"


//八路传感器头文件
#include "TrackingSensor.h"

//状态机
#include "menu.h"
#include "mode_tree.h"
#include "circle_list.h"
#include "mode.h"
#include "key.h"
#endif // ALLHEADERS_H