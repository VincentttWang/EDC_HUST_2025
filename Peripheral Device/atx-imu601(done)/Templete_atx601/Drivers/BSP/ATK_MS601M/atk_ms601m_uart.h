/**
 ****************************************************************************************************
 * @file        atk_ms601m_uart.h
 * @author      ����ԭ���Ŷ�(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS601Mģ��UART�ӿ���������
 * @license     Copyright (c) 2020-2032, �������������ӿƼ����޹�??
 ****************************************************************************************************
 * @attention
 *
 * ʵ��ƽ̨:����ԭ�� ������?? F767������??
 * ������???:www.yuanzige.com
 * ������̳:www.openedv.com
 * ��˾��ַ:www.alientek.com
 * ������???:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#ifndef __ATK_MS601M_UART_H
#define __ATK_MS601M_UART_H

#include "AllHeader.h"

/* ����???����?? */
//#define ATK_MS601M_UART_TX_GPIO_PORT            GPIOA
//#define ATK_MS601M_UART_TX_GPIO_PIN             GPIO_PIN_2
#define ATK_MS601M_UART_TX_GPIO_AF              GPIO_AF7_USART2

//#define ATK_MS601M_UART_RX_GPIO_PORT            GPIOA
//#define ATK_MS601M_UART_RX_GPIO_PIN             GPIO_PIN_3
#define ATK_MS601M_UART_RX_GPIO_AF              GPIO_AF7_USART2
                                                
#define ATK_MS601M_UART_INTERFACE               USART2
#define ATK_MS601M_UART_IRQn                    USART2_IRQn
//#define ATK_MS601M_UART_IRQHandler              USART2_IRQHandler
/* UART����FIFO������??? */
#define ATK_MS601M_UART_RX_FIFO_BUF_SIZE        128

/* �������� */
uint8_t atk_ms601m_uart_rx_fifo_write(uint8_t *dat, uint16_t len);  /* ATK-MS601M UART����FIFOд������ */
uint16_t atk_ms601m_uart_rx_fifo_read(uint8_t *dat, uint16_t len);  /* ATK-MS601M UART����FIFO��ȡ���� */
void atk_ms601m_rx_fifo_flush(void);                                /* ATK-MS601M UART����FIFO���????? */
void atk_ms601m_uart_send(uint8_t *dat, uint8_t len);               /* ATK-MS601M UART�������� */
void atk_ms601m_uart_init(uint32_t baudrate);                       /* ATK-MS601M UART��???�� */
void ATK_MS601M_UART_IRQHandler(void);

#endif
