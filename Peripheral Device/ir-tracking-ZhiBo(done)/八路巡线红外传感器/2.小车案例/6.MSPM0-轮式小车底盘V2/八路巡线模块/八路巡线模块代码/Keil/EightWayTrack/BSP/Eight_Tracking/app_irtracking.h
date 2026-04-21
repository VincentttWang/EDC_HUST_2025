#ifndef _APP_IRTRACKING_H_
#define _APP_IRTRACKING_H_


#include "ti_msp_dl_config.h"
#include "app_motor_usart.h"
#include "usart.h"
#include "app_motor.h"

#define u8 uint8_t
#define u16 uint16_t

#define IR_I2C_ADDR 0x12

void IRI2C_WriteByte(uint8_t addr, uint8_t dat);
uint8_t IRI2C_ReadByte(uint8_t addr);
void printf_i2c_data(void);
void LineWalking(void);

#endif

