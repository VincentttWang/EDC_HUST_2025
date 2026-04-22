#ifndef SENSOR_PROC_H
#define SENSOR_PROC_H

#include <stdbool.h>
#include "BspCommon.h"

#define PAPERWIDE 315
#define PAPERHIGHT 212
#define CIRCLERADIUS 60

#define XDISM 500
#define YDIS 500
#define LINELENGTH 1000

#define SENSOR_COUNT 8
#define DisSensorToWheel 195
#define DisSensor 12

Coordinate paper_to_camera(Coordinate paper);
Coordinate get_target_coordinate(void);

float Grayscale_Num_To_Theta(int num);
float thetaGrayscale(void);
bool Road_detect(int nummin, int nummax);
bool half_Detect(void);
bool cross_Roads_Detect(void);
bool empty_Detect(void);
bool centerDetect(void);

#endif /* SENSOR_PROC_H */
