/**
 * @file  HallEncoder.h
 * @brief 霍尔编码器驱动接口，提供转速与方向检测
 */
#ifndef HALL_ENCODER_H
#define HALL_ENCODER_H

#include "ti_msp_dl_config.h"
#include "BspCommon.h"

typedef enum {
    ENCODER_DIR_FORWARD = 0,
    ENCODER_DIR_REVERSE = 1,
} EncoderDir;

typedef struct {
    volatile long long temp_count;
    int count;
    EncoderDir dir;
} EncoderState;

void Encoder_Init(void);
int Encoder_GetCount(void);
EncoderDir Encoder_GetDir(void);
void Encoder_Update(void);
void Encoder_TimerInit(void);
double Encoder_GetSpeed(void);

#endif /* HALL_ENCODER_H */
