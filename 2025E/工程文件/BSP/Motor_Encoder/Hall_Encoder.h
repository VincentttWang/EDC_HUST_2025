#ifndef _HALL_ENCODER_H_
#define _HALL_ENCODER_H_

#include "ti_msp_dl_config.h"

typedef enum {
    D,  // 正向
    R  // 反向
} ENCODER_DIR;

typedef struct {
    volatile long long temp_count; //保存实时计数值
    int count;         						 //根据定时器时间更新的计数值
    ENCODER_DIR dir;            	 //旋转方向
} ENCODER_RES;


void encoder_init(void);
int get_encoder_count(void);
ENCODER_DIR get_encoder_dir(void);
void encoder_update(void);
void timer_init(void);
double getSpeed(void);
#endif