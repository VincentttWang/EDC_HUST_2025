/**
 ****************************************************************************************************
 * @file        atk_ms601m.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS601M模块驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 阿波罗 F767开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "./BSP/ATK_MS601M/atk_ms601m.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"

/* ATK-MS601M UART通讯帧头 */
#define ATK_MS601M_FRAME_HEAD_L             0x55
#define ATK_MS601M_FRAME_HEAD_UPLOAD_H      0x55    /* 高位主动上传帧头 */
#define ATK_MS601M_FRAME_HEAD_ACK_H         0xAF    /* 高位应答帧头 */

/* ATK-MS601M读写寄存器ID */
#define ATK_MS601M_READ_REG_ID(id)         (id | 0x80)
#define ATK_MS601M_WRITE_REG_ID(id)        (id)

typedef struct
{
    uint8_t head_l;                                 /* 低位帧头 */
    uint8_t head_h;                                 /* 高位帧头 */
    uint8_t id;                                     /* 帧ID */
    uint8_t len;                                    /* 数据长度 */
    uint8_t dat[ATK_MS601M_FRAME_DAT_MAX_SIZE];     /* 数据 */
    uint8_t check_sum;                              /* 校验和 */
} atk_ms601m_frame_t;                               /* ATK-MS601M UART通讯帧结构体 */

typedef enum
{
    wait_for_head_l = 0x00,                         /* 等待低位帧头 */
    wait_for_head_h = 0x01,                         /* 等待高位帧头 */
    wait_for_id     = 0x02,                         /* 等待帧ID */
    wait_for_len    = 0x04,                         /* 等待数据长度 */
    wait_for_dat    = 0x08,                         /* 等待数据 */
    wait_for_sum    = 0x16,                         /* 等待校验和 */
} atk_ms601m_handle_state_t;                        /* 帧处理状态机状态枚举 */

/* 陀螺仪、加速度计满量程表 */
static const uint16_t g_atk_ms601m_gyro_fsr_table[4] = {250, 500, 1000, 2000};
static const uint8_t g_atk_ms601m_accelerometer_fsr_table[4] = {2, 4, 8, 16};

static struct
{
    uint8_t gyro;                                   /* 陀螺仪满量程 */
    uint8_t accelerometer;                          /* 加速度计满量程 */
} g_atk_ms601m_fsr;                                 /* ATK-MS601M满量程数据 */

/**
 * @brief       通过指定帧ID获取接收到的数据帧
 * @param       frame  : 接收到的数据帧
 *              id     : 指定帧ID
 *              id_type: 帧ID类型，
 *                       ATK_MS601M_FRAME_ID_TYPE_UPLOAD: ATK-MS601M主动上传帧ID
 *                       ATK_MS601M_FRAME_ID_TYPE_ACK   : ATK-MS601M应答帧ID
 *              timeout: 等待数据帧最大超时时间，单位：毫秒（ms）
 * @retval      ATK_MS601M_EOK     : 获取指定数据帧成功
 *              ATK_MS601M_EINVAL  : 错误函数参数，获取指定数据帧失败
 *              ATK_MS601M_ETIMEOUT: 接收数据帧超时，获取指定数据帧失败
 */
static uint8_t atk_ms601m_get_frame_by_id(atk_ms601m_frame_t *frame, uint8_t id, uint8_t id_type, uint32_t timeout)
{
    uint8_t dat;
    atk_ms601m_handle_state_t handle_state = wait_for_head_l;
    uint8_t dat_index = 0;
    uint16_t timeout_index = 0;
    
    while (1)
    {
        if (timeout == 0)
        {
            return ATK_MS601M_ETIMEOUT;
        }
        
        if (atk_ms601m_uart_rx_fifo_read(&dat, 1) == 0)
        {
            delay_us(1);
            timeout_index++;
            if (timeout_index == 1000)
            {
                timeout_index = 0;
                timeout--;
            }
            continue;
        }
        
        switch (handle_state)
        {
            case wait_for_head_l:
            {
                if (dat == ATK_MS601M_FRAME_HEAD_L)
                {
                    frame->head_l = dat;
                    frame->check_sum = frame->head_l;
                    handle_state = wait_for_head_h;
                }
                else
                {
                    handle_state = wait_for_head_l;
                }
                break;
            }
            case wait_for_head_h:
            {
                switch (id_type)
                {
                    case ATK_MS601M_FRAME_ID_TYPE_UPLOAD:
                    {
                        if (dat == ATK_MS601M_FRAME_HEAD_UPLOAD_H)
                        {
                            frame->head_h = dat;
                            frame->check_sum += frame->head_h;
                            handle_state = wait_for_id;
                        }
                        else
                        {
                            handle_state = wait_for_head_l;
                        }
                        break;
                    }
                    case ATK_MS601M_FRAME_ID_TYPE_ACK:
                    {
                        if (dat == ATK_MS601M_FRAME_HEAD_ACK_H)
                        {
                            frame->head_h = dat;
                            frame->check_sum += frame->head_h;
                            handle_state = wait_for_id;
                        }
                        else
                        {
                            handle_state = wait_for_head_l;
                        }
                        break;
                    }
                    default:
                    {
                        return ATK_MS601M_EINVAL;
                    }
                }
                break;
            }
            case wait_for_id:
            {
                if (dat == id)
                {
                    frame->id = dat;
                    frame->check_sum += frame->id;
                    handle_state = wait_for_len;
                }
                else
                {
                    handle_state = wait_for_head_l;
                }
                break;
            }
            case wait_for_len:
            {
                if (dat > ATK_MS601M_FRAME_DAT_MAX_SIZE)
                {
                    handle_state = wait_for_head_l;
                }
                else
                {
                    frame->len = dat;
                    frame->check_sum += frame->len;
                    if (frame->len == 0)
                    {
                        handle_state = wait_for_sum;
                    }
                    else
                    {
                        handle_state = wait_for_dat;
                    }
                }
                break;
            }
            case wait_for_dat:
            {
                frame->dat[dat_index] = dat;
                frame->check_sum += frame->dat[dat_index];
                dat_index++;
                if (dat_index == frame->len)
                {
                    dat_index = 0;
                    handle_state = wait_for_sum;
                }
                break;
            }
            case wait_for_sum:
            {
                if (dat == frame->check_sum)
                {
                    return ATK_MS601M_EOK;
                }
                handle_state = wait_for_head_l;
                break;
            }
            default:
            {
                handle_state = wait_for_head_l;
                break;
            }
        }
        delay_us(1);
        timeout_index++;
        if (timeout_index == 1000)
        {
            timeout_index = 0;
            timeout--;
        }
    }
}

/**
 * @brief       通过帧ID读取ATK-MS601M寄存器
 * @param       id     : 寄存器对应的通讯帧ID
 *              dat    : 读取到的数据
 *              timeout: 等待数据的最大超时时间，单位：毫秒（ms）
 * @retval      0: 读取失败
 *              其他值: 读取到数据的长度
 */
uint8_t atk_ms601m_read_reg_by_id(uint8_t id, uint8_t *dat, uint32_t timeout)
{
    uint8_t buf[7];
    uint8_t ret;
    atk_ms601m_frame_t frame = {0};
    uint8_t dat_index;
    
    buf[0] = ATK_MS601M_FRAME_HEAD_L;
    buf[1] = ATK_MS601M_FRAME_HEAD_ACK_H;
    buf[2] = ATK_MS601M_READ_REG_ID(id);
    buf[3] = 1;
    buf[4] = 0;
    buf[5] = buf[0] + buf[1] + buf[2] + buf[3] + buf[4];
    atk_ms601m_uart_send(buf, 6);
    ret = atk_ms601m_get_frame_by_id(&frame, id, ATK_MS601M_FRAME_ID_TYPE_ACK, timeout);
    if (ret != ATK_MS601M_EOK)
    {
        return 0;
    }
    
    for (dat_index=0; dat_index<frame.len; dat_index++)
    {
        dat[dat_index] = frame.dat[dat_index];
    }
    
    return frame.len;
}

/**
 * @brief       通过帧ID写入ATK-MS601M寄存器
 * @param       id : 寄存器对应的通讯帧ID
 *              len: 待写入数据长度（1或2）
 *              dat: 待写入的数据
 * @retval      ATK_MS601M_EOK   : 寄存器写入成功
 *              ATK_MS601M_EINVAL: 函数参数len有误
 */
uint8_t atk_ms601m_write_reg_by_id(uint8_t id, uint8_t len, uint8_t *dat)
{
    uint8_t buf[7];
    
    buf[0] = ATK_MS601M_FRAME_HEAD_L;
    buf[1] = ATK_MS601M_FRAME_HEAD_ACK_H;
    buf[2] = ATK_MS601M_WRITE_REG_ID(id);
    buf[3] = len;
    if (len == 1)
    {
        buf[4] = dat[0];
        buf[5] = buf[0] + buf[1] + buf[2] + buf[3] + buf[4];
        atk_ms601m_uart_send(buf, 6);
    }
    else if (len == 2)
    {
        buf[4] = dat[0];
        buf[5] = dat[1];
        buf[6] = buf[0] + buf[1] + buf[2] + buf[3] + buf[4] + buf[5];
        atk_ms601m_uart_send(buf, 7);
    }
    else
    {
        return ATK_MS601M_EINVAL;
    }
    
    return ATK_MS601M_EOK;
}

/**
 * @brief       ATK-MS601M初始化
 * @param       buadrate: ATK-MS601M UART通讯波特率
 * @retval      ATK_MS601M_EOK  : ATK-MS601M初始化成功
 *              ATK_MS601M_ERROR: ATK-MS601M初始化失败
 */
uint8_t atk_ms601m_init(uint32_t baudrate)
{
    uint8_t ret;
    
    /* ATK-MS601M UART初始化 */
    atk_ms601m_uart_init(baudrate);
    
    /* 获取ATK-MS601M陀螺仪满量程 */
    ret = atk_ms601m_read_reg_by_id(ATK_MS601M_FRAME_ID_REG_GYROFSR, &g_atk_ms601m_fsr.gyro, 100);
    if (ret == 0)
    {
        return ATK_MS601M_ERROR;
    }
    
    /* 获取ATK-MS601M加速度计满量程 */
    ret = atk_ms601m_read_reg_by_id(ATK_MS601M_FRAME_ID_REG_ACCFSR, &g_atk_ms601m_fsr.accelerometer, 100);
    if (ret == 0)
    {
        return ATK_MS601M_ERROR;
    }
    
    return ATK_MS601M_EOK;
}

/**
 * @brief       获取ATK-MS601M姿态角数据
 * @param       attitude_dat: 姿态角数据结构体
 *              timeout     : 获取数据最大等待超时时间，单位：毫秒（ms）
 * @retval      ATK_MS601M_EOK  : 获取ATK-MS601M姿态角数据成功
 *              ATK_MS601M_ERROR: 获取ATK-MS601M姿态角数据失败
 */
uint8_t atk_ms601m_get_attitude(atk_ms601m_attitude_data_t *attitude_dat, uint32_t timeout)
{
    uint8_t ret;
    atk_ms601m_frame_t frame = {0};
    
    if (attitude_dat == NULL)
    {
        return ATK_MS601M_ERROR;
    }
    
    ret = atk_ms601m_get_frame_by_id(&frame, ATK_MS601M_FRAME_ID_ATTITUDE, ATK_MS601M_FRAME_ID_TYPE_UPLOAD, timeout);
    if (ret != ATK_MS601M_EOK)
    {
        return ATK_MS601M_ERROR;
    }
    
    attitude_dat->roll = (float)((int16_t)(frame.dat[1] << 8) | frame.dat[0]) / 32768 * 180;
    attitude_dat->pitch = (float)((int16_t)(frame.dat[3] << 8) | frame.dat[2]) / 32768 * 180;
    attitude_dat->yaw = (float)((int16_t)(frame.dat[5] << 8) | frame.dat[4]) / 32768 * 180;
    
    return ATK_MS601M_EOK;
}

/**
 * @brief       获取ATK-MS601M四元数数据
 * @param       quaternion_dat: 四元数数据结构体
 *              timeout       : 获取数据最大等待超时时间，单位：毫秒（ms）
 * @retval      ATK_MS601M_EOK  : 获取ATK-MS601M四元数数据成功
 *              ATK_MS601M_ERROR: 获取ATK-MS601M四元数数据失败
 */
uint8_t atk_ms601m_get_quaternion(atk_ms601m_quaternion_data_t *quaternion_dat, uint32_t timeout)
{
    uint8_t ret;
    atk_ms601m_frame_t frame = {0};
    
    if (quaternion_dat == NULL)
    {
        return ATK_MS601M_ERROR;
    }
    
    ret = atk_ms601m_get_frame_by_id(&frame, ATK_MS601M_FRAME_ID_QUAT, ATK_MS601M_FRAME_ID_TYPE_UPLOAD, timeout);
    if (ret != ATK_MS601M_EOK)
    {
        return ATK_MS601M_ERROR;
    }
    
    quaternion_dat->q0 = (float)((int16_t)(frame.dat[1] << 8) | frame.dat[0]) / 32768;
    quaternion_dat->q1 = (float)((int16_t)(frame.dat[3] << 8) | frame.dat[2]) / 32768;
    quaternion_dat->q2 = (float)((int16_t)(frame.dat[5] << 8) | frame.dat[4]) / 32768;
    quaternion_dat->q3 = (float)((int16_t)(frame.dat[7] << 8) | frame.dat[6]) / 32768;
    
    return ATK_MS601M_EOK;
}

/**
 * @brief       获取ATK-MS601M陀螺仪、加速度计数据
 * @param       gyro_dat         : 陀螺仪数据结构体
 *              accelerometer_dat: 加速度计数据结构体
 *              timeout          : 获取数据最大等待超时时间，单位：毫秒（ms）
 * @retval      ATK_MS601M_EOK  : 获取ATK-MS601M陀螺仪、加速度计数据成功
 *              ATK_MS601M_ERROR: 获取ATK-MS601M陀螺仪、加速度计数据失败
 */
uint8_t atk_ms601m_get_gyro_accelerometer(atk_ms601m_gyro_data_t *gyro_dat, atk_ms601m_accelerometer_data_t *accelerometer_dat, uint32_t timeout)
{
    uint8_t ret;
    atk_ms601m_frame_t frame = {0};
    
    if ((gyro_dat == NULL) && (accelerometer_dat == NULL))
    {
        return ATK_MS601M_ERROR;
    }
    
    ret = atk_ms601m_get_frame_by_id(&frame, ATK_MS601M_FRAME_ID_GYRO_ACCE, ATK_MS601M_FRAME_ID_TYPE_UPLOAD, timeout);
    if (ret != ATK_MS601M_EOK)
    {
        return ATK_MS601M_ERROR;
    }
    
    if (gyro_dat != NULL)
    {
        gyro_dat->raw.x = (int16_t)(frame.dat[7] << 8) | frame.dat[6];
        gyro_dat->raw.y = (int16_t)(frame.dat[9] << 8) | frame.dat[8];
        gyro_dat->raw.z = (int16_t)(frame.dat[11] << 8) | frame.dat[10];
        
        gyro_dat->x = (float)gyro_dat->raw.x / 32768 * g_atk_ms601m_gyro_fsr_table[g_atk_ms601m_fsr.gyro];
        gyro_dat->y = (float)gyro_dat->raw.y / 32768 * g_atk_ms601m_gyro_fsr_table[g_atk_ms601m_fsr.gyro];
        gyro_dat->z = (float)gyro_dat->raw.z / 32768 * g_atk_ms601m_gyro_fsr_table[g_atk_ms601m_fsr.gyro];
    }
    
    if (accelerometer_dat != NULL)
    {
        accelerometer_dat->raw.x = (int16_t)(frame.dat[1] << 8) | frame.dat[0];
        accelerometer_dat->raw.y = (int16_t)(frame.dat[3] << 8) | frame.dat[2];
        accelerometer_dat->raw.z = (int16_t)(frame.dat[5] << 8) | frame.dat[4];
        
        accelerometer_dat->x = (float)accelerometer_dat->raw.x / 32768 * g_atk_ms601m_accelerometer_fsr_table[g_atk_ms601m_fsr.accelerometer];
        accelerometer_dat->y = (float)accelerometer_dat->raw.y / 32768 * g_atk_ms601m_accelerometer_fsr_table[g_atk_ms601m_fsr.accelerometer];
        accelerometer_dat->z = (float)accelerometer_dat->raw.z / 32768 * g_atk_ms601m_accelerometer_fsr_table[g_atk_ms601m_fsr.accelerometer];
    }
    
    return ATK_MS601M_EOK;
}

/**
 * @brief       获取ATK-MS601M端口数据
 * @param       port_dat: 端口数据结构体
 *              timeout : 获取数据最大等待超时时间，单位：毫秒（ms）
 * @retval      ATK_MS601M_EOK  : 获取ATK-MS601M端口数据成功
 *              ATK_MS601M_ERROR: 获取ATK-MS601M端口数据失败
 */
uint8_t atk_ms601m_get_port(atk_ms601m_port_data_t *port_dat, uint32_t timeout)
{
    uint8_t ret;
    atk_ms601m_frame_t frame = {0};
    
    if (port_dat == NULL)
    {
        return ATK_MS601M_ERROR;
    }
    
    ret = atk_ms601m_get_frame_by_id(&frame, ATK_MS601M_FRAME_ID_PORT, ATK_MS601M_FRAME_ID_TYPE_UPLOAD, timeout);
    if (ret != ATK_MS601M_EOK)
    {
        return ATK_MS601M_ERROR;
    }
    
    port_dat->d0 = (uint16_t)(frame.dat[1] << 8) | frame.dat[0];
    port_dat->d1 = (uint16_t)(frame.dat[3] << 8) | frame.dat[2];
    port_dat->d2 = (uint16_t)(frame.dat[5] << 8) | frame.dat[4];
    port_dat->d3 = (uint16_t)(frame.dat[7] << 8) | frame.dat[6];
    
    return ATK_MS601M_EOK;
}

/**
 * @brief       获取ATK-MS601M LED灯状态
 * @param       state: LED灯状态
 *              timeout: 获取数据最大等待超时时间，单位：毫秒（ms）
 * @retval      ATK_MS601M_EOK  : 获取ATK-MS601M LED灯状态成功
 *              ATK_MS601M_ERROR: 获取ATK-MS601M LED灯状态失败
 */
uint8_t atk_ms601m_get_led_state(atk_ms601m_led_state_t *state, uint32_t timeout)
{
    uint8_t ret;
    
    ret = atk_ms601m_read_reg_by_id(ATK_MS601M_FRAME_ID_REG_LEDOFF, (uint8_t *)state, timeout);
    if (ret == 0)
    {
        return ATK_MS601M_ERROR;
    }
    
    return ATK_MS601M_EOK;
}

/**
 * @brief       设置ATK-MS601M LED灯状态
 * @param       state: LED灯状态
 *              timeout: 获取数据最大等待超时时间，单位：毫秒（ms）
 * @retval      ATK_MS601M_EOK  : 设置ATK-MS601M LED灯状态成功
 *              ATK_MS601M_ERROR: 设置ATK-MS601M LED灯状态失败
 */
uint8_t atk_ms601m_set_led_state(atk_ms601m_led_state_t state, uint32_t timeout)
{
    uint8_t ret;
    atk_ms601m_led_state_t state_recv;
    
    ret = atk_ms601m_write_reg_by_id(ATK_MS601M_FRAME_ID_REG_LEDOFF, 1, (uint8_t *)&state);
    if (ret != ATK_MS601M_EOK)
    {
        return ATK_MS601M_ERROR;
    }
    
    ret = atk_ms601m_get_led_state(&state_recv, timeout);
    if (ret != ATK_MS601M_EOK)
    {
        return ATK_MS601M_ERROR;
    }
    
    if (state_recv != state)
    {
        return ATK_MS601M_ERROR;
    }
    
    return ATK_MS601M_EOK;
}

/**
 * @brief       获取ATK-MS601M指定端口模式
 * @param       port   : 指定端口
 *              mode   : 端口的模式
 *              timeout: 获取数据最大等待超时时间，单位：毫秒（ms）
 * @retval      ATK_MS601M_EOK  : 获取ATK-MS601M指定端口模式成功
 *              ATK_MS601M_ERROR: 获取ATK-MS601M指定端口模式失败
 */
uint8_t atk_ms601m_get_port_mode(atk_ms601m_port_t port, atk_ms601m_port_mode_t *mode, uint32_t timeout)
{
    uint8_t ret;
    uint8_t id;
    
    if (port == ATK_MS601M_PORT_D0)
    {
        id = ATK_MS601M_FRAME_ID_REG_D0MODE;
    }
    else if (port == ATK_MS601M_PORT_D1)
    {
        id = ATK_MS601M_FRAME_ID_REG_D1MODE;
    }
    else if (port == ATK_MS601M_PORT_D2)
    {
        id = ATK_MS601M_FRAME_ID_REG_D2MODE;
    }
    else if (port == ATK_MS601M_PORT_D3)
    {
        id = ATK_MS601M_FRAME_ID_REG_D3MODE;
    }
    else
    {
        return ATK_MS601M_ERROR;
    }
    
    ret = atk_ms601m_read_reg_by_id(id, (uint8_t *)mode, timeout);
    if (ret == 0)
    {
        return ATK_MS601M_ERROR;
    }
    
    return ATK_MS601M_EOK;
}

/**
 * @brief       设置ATK-MS601M指定端口模式
 * @param       port   : 指定端口
 *              mode   : 端口的模式
 *              timeout: 获取数据最大等待超时时间，单位：毫秒（ms）
 * @retval      ATK_MS601M_EOK  : 设置ATK-MS601M指定端口模式成功
 *              ATK_MS601M_ERROR: 设置ATK-MS601M指定端口模式失败
 */
uint8_t atk_ms601m_set_port_mode(atk_ms601m_port_t port, atk_ms601m_port_mode_t mode, uint32_t timeout)
{
    uint8_t ret;
    uint8_t id;
    atk_ms601m_port_mode_t mode_recv;
    
    if (port == ATK_MS601M_PORT_D0)
    {
        if (mode == ATK_MS601M_PORT_MODE_OUTPUT_PWM)
        {
            return ATK_MS601M_ERROR;
        }
        id = ATK_MS601M_FRAME_ID_REG_D0MODE;
    }
    else if (port == ATK_MS601M_PORT_D1)
    {
        id = ATK_MS601M_FRAME_ID_REG_D1MODE;
    }
    else if (port == ATK_MS601M_PORT_D2)
    {
        if (mode == ATK_MS601M_PORT_MODE_OUTPUT_PWM)
        {
            return ATK_MS601M_ERROR;
        }
        id = ATK_MS601M_FRAME_ID_REG_D2MODE;
    }
    else if (port == ATK_MS601M_PORT_D3)
    {
        id = ATK_MS601M_FRAME_ID_REG_D3MODE;
    }
    else
    {
        return ATK_MS601M_ERROR;
    }
    
    ret = atk_ms601m_write_reg_by_id(id, 1, (uint8_t *)&mode);
    if (ret != ATK_MS601M_EOK)
    {
        return ATK_MS601M_ERROR;
    }
    
    ret = atk_ms601m_get_port_mode(port, &mode_recv, timeout);
    if (ret != ATK_MS601M_EOK)
    {
        return ATK_MS601M_ERROR;
    }
    else
    {
        if (mode_recv != mode)
        {
            return ATK_MS601M_ERROR;
        }
    }
    
    return ATK_MS601M_EOK;
}

/**
 * @brief       获取ATK-MS601M指定端口PWM高电平的宽度
 * @param       port   : 指定端口
 *              pulse  : 端口PWM高电平的宽度，单位：纳秒（ns）
 *              timeout: 获取数据最大等待超时时间，单位：毫秒（ms）
 * @retval      ATK_MS601M_EOK  : 获取ATK-MS601M指定端口PWM高电平的宽度成功
 *              ATK_MS601M_ERROR: 获取ATK-MS601M指定端口PWM高电平的宽度失败
 */
uint8_t atk_ms601m_get_port_pwm_pulse(atk_ms601m_port_t port, uint16_t *pulse, uint32_t timeout)
{
    uint8_t ret;
    uint8_t id;
    
    if (port == ATK_MS601M_PORT_D0)
    {
        return ATK_MS601M_ERROR;
    }
    else if (port == ATK_MS601M_PORT_D1)
    {
        id = ATK_MS601M_FRAME_ID_REG_D1PULSE;
    }
    else if (port == ATK_MS601M_PORT_D2)
    {
        return ATK_MS601M_ERROR;
    }
    else if (port == ATK_MS601M_PORT_D3)
    {
        id = ATK_MS601M_FRAME_ID_REG_D3PULSE;
    }
    else
    {
        return ATK_MS601M_ERROR;
    }
    
    ret = atk_ms601m_read_reg_by_id(id, (uint8_t *)pulse, 100);
    if (ret == 0)
    {
        return ATK_MS601M_ERROR;
    }
    
    return ATK_MS601M_EOK;
}

/**
 * @brief       设置ATK-MS601M指定端口PWM高电平的宽度
 * @param       port   : 指定端口
 *              pulse  : 端口PWM高电平的宽度，单位：纳秒（ns）
 *              timeout: 获取数据最大等待超时时间，单位：毫秒（ms）
 * @retval      ATK_MS601M_EOK  : 设置ATK-MS601M指定端口PWM高电平的宽度成功
 *              ATK_MS601M_ERROR: 设置ATK-MS601M指定端口PWM高电平的宽度失败
 */
uint8_t atk_ms601m_set_port_pwm_pulse(atk_ms601m_port_t port, uint16_t pulse, uint32_t timeout)
{
    uint8_t ret;
    uint8_t id;
    uint16_t pulse_recv;
    
    if (port == ATK_MS601M_PORT_D0)
    {
        return ATK_MS601M_ERROR;
    }
    else if (port == ATK_MS601M_PORT_D1)
    {
        id = ATK_MS601M_FRAME_ID_REG_D1PULSE;
    }
    else if (port == ATK_MS601M_PORT_D2)
    {
        return ATK_MS601M_ERROR;
    }
    else if (port == ATK_MS601M_PORT_D3)
    {
        id = ATK_MS601M_FRAME_ID_REG_D3PULSE;
    }
    else
    {
        return ATK_MS601M_ERROR;
    }
    
    ret = atk_ms601m_write_reg_by_id(id, 2, (uint8_t *)&pulse);
    if (ret != ATK_MS601M_EOK)
    {
        return ATK_MS601M_ERROR;
    }
    
    ret = atk_ms601m_get_port_pwm_pulse(port, &pulse_recv, timeout);
    if (ret != ATK_MS601M_EOK)
    {
        return ATK_MS601M_ERROR;
    }
    
    if (pulse_recv != pulse)
    {
        return ATK_MS601M_ERROR;
    }
    
    return ATK_MS601M_EOK;
}

/**
 * @brief       获取ATK-MS601M指定端口PWM周期
 * @param       port   : 指定端口
 *              period : 端口PWM周期，单位：纳秒（ns）
 *              timeout: 获取数据最大等待超时时间，单位：毫秒（ms）
 * @retval      ATK_MS601M_EOK  : 获取ATK-MS601M指定端口PWM周期成功
 *              ATK_MS601M_ERROR: 获取ATK-MS601M指定端口PWM周期失败
 */
uint8_t atk_ms601m_get_port_pwm_period(atk_ms601m_port_t port, uint16_t *period, uint32_t timeout)
{
    uint8_t ret;
    uint8_t id;
    
    if (port == ATK_MS601M_PORT_D0)
    {
        return ATK_MS601M_ERROR;
    }
    else if (port == ATK_MS601M_PORT_D1)
    {
        id = ATK_MS601M_FRAME_ID_REG_D1PERIOD;
    }
    else if (port == ATK_MS601M_PORT_D2)
    {
        return ATK_MS601M_ERROR;
    }
    else if (port == ATK_MS601M_PORT_D3)
    {
        id = ATK_MS601M_FRAME_ID_REG_D3PERIOD;
    }
    else
    {
        return ATK_MS601M_ERROR;
    }
    
    ret = atk_ms601m_read_reg_by_id(id, (uint8_t *)period, timeout);
    if (ret == 0)
    {
        return ATK_MS601M_ERROR;
    }
    
    return ATK_MS601M_EOK;
}

/**
 * @brief       设置ATK-MS601M指定端口PWM周期
 * @param       port   : 指定端口
 *              period : 端口PWM周期，单位：纳秒（ns）
 *              timeout: 获取数据最大等待超时时间，单位：毫秒（ms）
 * @retval      ATK_MS601M_EOK  : 设置ATK-MS601M指定端口PWM周期成功
 *              ATK_MS601M_ERROR: 设置ATK-MS601M指定端口PWM周期失败
 */
uint8_t atk_ms601m_set_port_pwm_period(atk_ms601m_port_t port, uint16_t period, uint32_t timeout)
{
    uint8_t ret;
    uint8_t id;
    uint16_t period_recv;
    
    if (port == ATK_MS601M_PORT_D0)
    {
        return ATK_MS601M_ERROR;
    }
    else if (port == ATK_MS601M_PORT_D1)
    {
        id = ATK_MS601M_FRAME_ID_REG_D1PERIOD;
    }
    else if (port == ATK_MS601M_PORT_D2)
    {
        return ATK_MS601M_ERROR;
    }
    else if (port == ATK_MS601M_PORT_D3)
    {
        id = ATK_MS601M_FRAME_ID_REG_D3PERIOD;
    }
    else
    {
        return ATK_MS601M_ERROR;
    }
    
    ret = atk_ms601m_write_reg_by_id(id, 2, (uint8_t *)&period);
    if (ret != ATK_MS601M_EOK)
    {
        return ATK_MS601M_ERROR;
    }
    
    ret = atk_ms601m_get_port_pwm_period(port, &period_recv, timeout);
    if (ret != ATK_MS601M_EOK)
    {
        return ATK_MS601M_ERROR;
    }
    
    if (period_recv != period)
    {
        return ATK_MS601M_ERROR;
    }
    
    return ATK_MS601M_EOK;
}
