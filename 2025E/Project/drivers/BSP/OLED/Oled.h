/**
 * @file      Oled.h
 * @brief     SSD1306 OLED 显示屏驱动库头文件 (软件I2C)
 * @details   通过软件模拟 I2C 协议驱动 OLED，提供初始化、显示字符、
 *            数字、字符串、位图等功能。
 * @author    Jianing Wang
 * @version   1.1
 * @date      2025-07-31
 * @note      - 需要提供微秒级延时函数 Delay_us() 的实现。
 *            - 使用前请在 SysConfig 中配置 SCL 和 SDA 引脚，并在此文件中
 *              正确配置 OLED_GPIO_PORT、OLED_SCL_PIN 和 OLED_SDA_PIN。
 */

#ifndef OLED_H
#define OLED_H

#include "ti_msp_dl_config.h" // TI MSPM0 底层配置
#include <stdint.h>          // 标准整数类型

//==================================================================================
// 用户配置区 (User Configuration)
//==================================================================================

// 定义 OLED 连接的 GPIO 端口
// 例如：如果 SCL 和 SDA 连接到 GPIOA，则使用 GPIOA
#define OLED_GPIO_PORT      (GPIOA)


//==================================================================================
// 内部宏定义 (Internal Macros)
//==================================================================================

#define OLED_CMD            (0) // 写命令模式
#define OLED_DATA           (1) // 写数据模式

//----------------------------------------------------------------------------------
// OLED SSD1306 I2C 时钟 SCL 引脚操作
#define OLED_SCL_Set()      (DL_GPIO_setPins(OLED_GPIO_PORT, OLED_SCL_PIN))
#define OLED_SCL_Clr()      (DL_GPIO_clearPins(OLED_GPIO_PORT, OLED_SCL_PIN))

//----------------------------------------------------------------------------------
// OLED SSD1306 I2C 数据 SDA 引脚操作
#define OLED_SDA_Set()      (DL_GPIO_setPins(OLED_GPIO_PORT, OLED_SDA_PIN))
#define OLED_SDA_Clr()      (DL_GPIO_clearPins(OLED_GPIO_PORT, OLED_SDA_PIN))
#define OLED_SDA_Read()     (DL_GPIO_readPins(OLED_GPIO_PORT, OLED_SDA_PIN))


//==================================================================================
// 函数原型声明 (Function Prototypes)
//==================================================================================


// OLED 控制与显示函数
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