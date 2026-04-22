/**
 * @file  BspCommon.h
 * @brief BSP 公共类型定义，包含错误码、坐标、姿态等共享数据结构
 */
#ifndef BSP_COMMON_H
#define BSP_COMMON_H

#include <stdint.h>

typedef enum {
    BSP_OK          = 0,
    BSP_ERR_NULL    = -1,
    BSP_ERR_PARAM   = -2,
    BSP_ERR_TIMEOUT = -3,
    BSP_ERR_BUSY    = -4,
} BSP_Status;

typedef enum {
    CANMV_ERR_INIT      = -1,
    CANMV_ERR_NONE      =  0,
    CANMV_ERR_NOT_FOUND =  1,
    CANMV_ERR_LOST      =  2,
    CANMV_ERR_FRAME_DROP =  3,
} CanMV_Error;

typedef struct {
    float x;
    float y;
} Coordinate;

typedef struct {
    float yaw;
    float pitch;
} Attitude;

#endif /* BSP_COMMON_H */
