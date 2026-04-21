/*
 * SensorProc.c
 *
 *  Created on: 2025-7-24
 *  Author: Qinghan Yang
 *  Description: Source file for sensor processing functions
 */
#include "SensorProc.h"
#include "AllHeader.h"

Coordinate paperCornerC[4];
extern int edge;
extern float sInedge;

//以下函数用于处理视觉模块，主要在绘图的时候使用(未测)
Coordinate paper_to_camera(Coordinate paper)
{
    paper.x /= PAPERWIDE;
    paper.y /= PAPERHIGHT;
    paperCornerC[3].x = Rect_Loc[0];
    paperCornerC[3].y = Rect_Loc[1];
    paperCornerC[2].x = Rect_Loc[2];
    paperCornerC[2].y = Rect_Loc[3];
    paperCornerC[1].x = Rect_Loc[4];
    paperCornerC[1].y = Rect_Loc[5];
    paperCornerC[0].x = Rect_Loc[6];
    paperCornerC[0].y = Rect_Loc[7];
    Coordinate camera;
     float u = paper.x / PAPERWIDE;
    float v = paper.y / PAPERHIGHT;

    // 步骤 2: 沿四边形的顶部边缘（从角点0到角点1）进行线性插值。
    float top_x = (1.0f - u) * paperCornerC[0].x + u * paperCornerC[1].x;
    float top_y = (1.0f - u) * paperCornerC[0].y + u * paperCornerC[1].y;

    // 步骤 3: 沿四边形的底部边缘（从角点3到角点2）进行线性插值。
    float bottom_x = (1.0f - u) * paperCornerC[3].x + u * paperCornerC[2].x;
    float bottom_y = (1.0f - u) * paperCornerC[3].y + u * paperCornerC[2].y;

    // 步骤 4: 在顶部插值点和底部插值点之间，进行最终的垂直方向插值。
    Coordinate camera_coord;
    camera_coord.x = (1.0f - v) * top_x + v * bottom_x;
    camera_coord.y = (1.0f - v) * top_y + v * bottom_y;

    return camera_coord;
}

//以下函数用于通过视觉传感器以及编码器获得所需的光点坐标（未测）
Coordinate get_target_coordinate()
{
    Coordinate target;
    float target_theta = (edge - 1) * 90.0f + sInedge; // 计算目标角度 有个疑虑，在出发点edge = 0 而且编码器的数值还没叠加起来
    target.x = CIRCLERADIUS * cosf(DEG_TO_RAD(target_theta));
    target.y = CIRCLERADIUS * sinf(DEG_TO_RAD(target_theta));
    // 将目标坐标转换为相对于纸张中心的坐标
    target.x += PAPERWIDE / 2.0f;
    target.y += PAPERHIGHT / 2.0f;
    return target;
}

Coordinate getCenter(void)
{
    Coordinate center;
    center.x = PAPERWIDE / 2.0f;
    center.y = PAPERHIGHT / 2.0f;
    return center;
}
//以下函数用于处理灰度传感器
float Grayscale_Num_To_Theta(int num)
{
    // Convert the sensor number to an angle in degrees
    // The angle is calculated based on the sensor's position
    return RAD_TO_DEG(atan(-(num - (SENSOR_COUNT - 1) / 2.0f) * DisSensor / DisSensorToWheel));
}
 
float thetaGrayscale()
{
    float theta = 0.0f;
    int sum = 0;
    // 计算灰度传感器的偏差而导致的误差值
   for(int i = 0; i < SENSOR_COUNT; i++)
    {
        if(Digital[i] == 0) 
        {
            theta += Grayscale_Num_To_Theta(i);
            sum++;
        }
    }
		if(sum == 0)
			return 0.0;
    return theta / sum;
}

bool Road_detect(int nummin, int nummax)
{
    static int last_time = 0;
//    if (tick - last_time < 100) // 防抖动
//    {
//        return false; // 如果上次检测时间小于100ms，则不进行检测
//    }
    int sumIRDetect = 0;
    int i = 0;
    while(i < 8){	if(Digital[i]==0) sumIRDetect ++; i++;}
    if(sumIRDetect <= nummax && sumIRDetect >= nummin)
		{
		//last_time = tick;
			return 1;
		}
    else return 0;
}
bool half_Detect(){
//三岔路口检测
	return Road_detect(3, 6);
}

bool cross_Roads_Detect(){
//十字路口（丁字路口）检测
	return Road_detect(7, 8);
}

bool empty_Detect()
{
	return Road_detect(0, 0);
}

bool centerDetect()
{
	return Digital[3] == 0;
}