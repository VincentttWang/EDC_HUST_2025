/**
 ****************************************************************************************************
 * @file        atk_ms601m_uart.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS601M模块UART接口驱动代码
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

#ifndef __ATK_MS601M_UART_H
#define __ATK_MS601M_UART_H

#include "./SYSTEM/sys/sys.h"

/* 引脚定义 */
#define ATK_MS601M_UART_TX_GPIO_PORT            GPIOB
#define ATK_MS601M_UART_TX_GPIO_PIN             GPIO_PIN_10
#define ATK_MS601M_UART_TX_GPIO_AF              GPIO_AF7_USART3
#define ATK_MS601M_UART_TX_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)     /* PB口时钟使能 */

#define ATK_MS601M_UART_RX_GPIO_PORT            GPIOB
#define ATK_MS601M_UART_RX_GPIO_PIN             GPIO_PIN_11
#define ATK_MS601M_UART_RX_GPIO_AF              GPIO_AF7_USART3
#define ATK_MS601M_UART_RX_GPIO_CLK_ENABLE()    do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)     /* PB口时钟使能 */
                                                
#define ATK_MS601M_UART_INTERFACE               USART3
#define ATK_MS601M_UART_IRQn                    USART3_IRQn
#define ATK_MS601M_UART_IRQHandler              USART3_IRQHandler
#define ATK_MS601M_UART_CLK_ENABLE()            do{ __HAL_RCC_USART3_CLK_ENABLE(); }while(0)    /* USART3 时钟使能 */

/* UART接收FIFO缓冲大小 */
#define ATK_MS601M_UART_RX_FIFO_BUF_SIZE        128

/* 操作函数 */
uint8_t atk_ms601m_uart_rx_fifo_write(uint8_t *dat, uint16_t len);  /* ATK-MS601M UART接收FIFO写入数据 */
uint16_t atk_ms601m_uart_rx_fifo_read(uint8_t *dat, uint16_t len);  /* ATK-MS601M UART接收FIFO读取数据 */
void atk_ms601m_rx_fifo_flush(void);                                /* ATK-MS601M UART接收FIFO清空 */
void atk_ms601m_uart_send(uint8_t *dat, uint8_t len);               /* ATK-MS601M UART发送数据 */
void atk_ms601m_uart_init(uint32_t baudrate);                       /* ATK-MS601M UART初始化 */

#endif
