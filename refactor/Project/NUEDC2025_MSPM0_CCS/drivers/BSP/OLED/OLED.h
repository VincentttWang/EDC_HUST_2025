/**
 * @file      OLED.h
 * @brief     SSD1306 OLED��ʾ���������ͷ�ļ� (����I2C)
 * @details   ������ʹ������ģ��I2CЭ����OLEDͨ�š����ṩ��OLED��ʼ����
 * ��ʾ�ַ������֡��ַ�����λͼ�ȹ��ܡ�
 * @author    Jianing Wang
 * @version   1.1
 * @date      2025-07-31
 * @note      - ������������һ���ṩ΢�뼶��ʱ���� `Delay_us()` ����ʱ�⡣
 * - ʹ��ǰ����ȷ������SysConfig��������SCL��SDA���ţ����ڴ��ļ���
 * ��ȷ������`OLED_GPIO_PORT`��`OLED_SCL_PIN`��`OLED_SDA_PIN`��
 */

#ifndef OLED_H
#define OLED_H

#include "ti_msp_dl_config.h" // ����TI MSPM0�ĵײ��
#include <stdint.h>          // �����׼��������

//==================================================================================
// �û������� (User Configuration)
//==================================================================================

// ����OLED���ӵ�GPIO�˿�
// ���磺�������SCL��SDA�����ӵ�GPIOA�������ʹ��GPIOA
#define OLED_GPIO_PORT      (GPIOA)


//==================================================================================
// �ڲ��궨�� (Internal Macros)
//==================================================================================

#define OLED_CMD            (0) // д����ģʽ
#define OLED_DATA           (1) // д����ģʽ

//----------------------------------------------------------------------------------
// OLED SSD1306 I2C ʱ�� SCL ���Ų���
#define OLED_SCL_Set()      (DL_GPIO_setPins(OLED_GPIO_PORT, OLED_SCL_PIN))
#define OLED_SCL_Clr()      (DL_GPIO_clearPins(OLED_GPIO_PORT, OLED_SCL_PIN))

//----------------------------------------------------------------------------------
// OLED SSD1306 I2C ���� SDA ���Ų���
#define OLED_SDA_Set()      (DL_GPIO_setPins(OLED_GPIO_PORT, OLED_SDA_PIN))
#define OLED_SDA_Clr()      (DL_GPIO_clearPins(OLED_GPIO_PORT, OLED_SDA_PIN))
#define OLED_SDA_Read()     (DL_GPIO_readPins(OLED_GPIO_PORT, OLED_SDA_PIN))


//==================================================================================
// ����ԭ������ (Function Prototypes)
//==================================================================================


//OLED�����ú���
void OLED_ColorTurn(uint8_t i);
void OLED_DisplayTurn(uint8_t i);
void OLED_WR_Byte(uint8_t dat,uint8_t cmd);
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t sizey);
uint32_t oled_pow(uint8_t m,uint8_t n);
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t sizey);
void OLED_ShowString(uint8_t x,uint8_t y,char *chr,uint8_t sizey);
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t no,uint8_t sizey);
void OLED_DrawBMP(uint8_t x,uint8_t y,uint8_t sizex, uint8_t sizey,uint8_t BMP[]);
void OLED_Init(void);

#endif /* OLED_H */