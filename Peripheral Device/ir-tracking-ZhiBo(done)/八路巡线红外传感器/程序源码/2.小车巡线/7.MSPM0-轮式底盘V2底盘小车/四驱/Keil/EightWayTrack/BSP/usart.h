#ifndef	__USART_H__
#define __USART_H__

#include "ti_msp_dl_config.h"
#include "stdio.h"


void USART_Init(void);

void USART_SendData(unsigned char data);

#endif
