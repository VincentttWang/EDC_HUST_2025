/**
 * @file      OLED.c
 * @brief     SSD1306 OLED显示屏驱动库的实现文件 (软件I2C)
 * @details   实现了通过软件模拟I2C协议驱动OLED的所有函数。
 * @author    Jianing Wang
 * @version   1.1
 * @date      2025-07-31
 */

#include "OLED.h"
#include "OLED_Font.h"
#include <stdint.h>

// 外部依赖一个微秒延时函数，请确保其已在项目中实现
// extern void Delay_us(uint32_t us);
// 如果您的延时函数在AllHeader.h中，请包含它
#include "AllHeader.h"


//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127
 
 
//反显函数
void OLED_ColorTurn(uint8_t i)
{
    if(i==0)
    {
        OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
    }
    if(i==1)
    {
        OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
    }
}
 
////屏幕旋转180度
//void OLED_DisplayTurn(uint8_t i)
//{
//if(i==0)
//    {
//        OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
//        OLED_WR_Byte(0xA1,OLED_CMD);
//    }
//    if(i==1)
//    {
//        OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
//        OLED_WR_Byte(0xA0,OLED_CMD);
//    }
//}
 
//起始信号
void I2C_Start(void)
{
    OLED_SDA_Set();
    OLED_SCL_Set();
    OLED_SDA_Clr();Delay_us(1);
    OLED_SCL_Clr();
}
 
//结束信号
void I2C_Stop(void)
{
    OLED_SDA_Clr();
    OLED_SCL_Set();
    OLED_SDA_Set();
}
 
//等待信号响应
uint8_t OLED_I2C_WaitAck(void) //测数据信号的电平
{
    uint8_t ack = 1;  // 默认无ACK
    OLED_SDA_Set();   // 释放SDA
    OLED_SCL_Set();   // 拉高SCL，等待OLED发送ACK
    if (OLED_SDA_Read() == 0) {  // OLED拉低SDA表示ACK
        ack = 0;
    }
    OLED_SCL_Clr();   // 拉低SCL，结束ACK检测
    return ack;  // 返回0表示成功，1表示失败
}
 
//写入一个字节
void OLED_Send_Byte(uint8_t dat)
{
    uint8_t i;
    for(i=0;i<8;i++)
    {
        OLED_SCL_Clr();
        
        if(dat&0x80) OLED_SDA_Set();
        else OLED_SDA_Clr();
        Delay_us(1);
        OLED_SCL_Set();  // 拉高SCL，OLED读取数据
        Delay_us(1);
        OLED_SCL_Clr();
        dat<<=1;
    }
}
 
//发送一个字节
//向SSD1306写入一个字节。
//mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(uint8_t dat,uint8_t mode)
{
    I2C_Start();
    OLED_Send_Byte(0x78);
    if (OLED_I2C_WaitAck() != 0) { I2C_Stop(); return; }  // 地址无响应，退出
 
    if(mode) OLED_Send_Byte(0x40);
    else OLED_Send_Byte(0x00);
    if (OLED_I2C_WaitAck() != 0) { I2C_Stop(); return; }  // 控制字节无响应
 
    OLED_Send_Byte(dat);
    if (OLED_I2C_WaitAck() != 0) { I2C_Stop(); return; }  // 数据无响应
 
    I2C_Stop();
}
 
void OLED_Write_ContinuousCmd(uint8_t *cmds, uint8_t len)
{
    if (len == 0) return;
 
    I2C_Start();
    OLED_Send_Byte(0x78);  // OLED地址
    if (OLED_I2C_WaitAck() != 0) { I2C_Stop(); return; }
 
    OLED_Send_Byte(0x00);  // 命令模式
    if (OLED_I2C_WaitAck() != 0) { I2C_Stop(); return; }
 
    // 连续发送所有命令
    for (uint8_t i = 0; i < len; i++) {
        OLED_Send_Byte(cmds[i]);
        if (OLED_I2C_WaitAck() != 0) { I2C_Stop(); return; }
    }
 
    I2C_Stop();
}
 
//坐标设置
void OLED_Set_Pos(uint8_t x, uint8_t y) 
{ 
//    OLED_WR_Byte(0xb0+y,OLED_CMD);
//    OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
//    OLED_WR_Byte((x&0x0f),OLED_CMD);
    uint8_t cmds[3];
    cmds[0] = 0xb0 + y;                  // 页地址
    cmds[1] = ((x & 0xf0) >> 4) | 0x10;  // 列高地址
    cmds[2] = x & 0x0f;                  // 列低地址
    OLED_Write_ContinuousCmd(cmds, 3);   // 一次传输3个命令
}
 
//开启OLED显示    
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X14,OLED_CMD);  //DCDC ON
    OLED_WR_Byte(0XAF,OLED_CMD);  //DISPLAY ON
}
 
//关闭OLED显示     
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D,OLED_CMD);  //SET DCDC命令
    OLED_WR_Byte(0X10,OLED_CMD);  //DCDC OFF
    OLED_WR_Byte(0XAE,OLED_CMD);  //DISPLAY OFF
}
// 连续发送数据时跳过ACK检查（仅清屏用）
void OLED_Write_ContinuousData_NoAck(uint8_t *data, uint16_t len)
{
    if (len == 0) return;
 
    I2C_Start();
    OLED_Send_Byte(0x78);
    (void)OLED_I2C_WaitAck();  // 忽略ACK
 
    OLED_Send_Byte(0x40);
    (void)OLED_I2C_WaitAck();  // 忽略ACK
 
    for (uint16_t i = 0; i < len; i++) {
        OLED_Send_Byte(data[i]);
        (void)OLED_I2C_WaitAck();  // 忽略ACK
    }
 
    I2C_Stop();
}
//清屏函数,清完屏,整个屏幕是黑色的!和没点亮一样!!!	  
void OLED_Clear(void)  
{  
//    uint8_t i,n;		    
//    for(i=0;i<8;i++)  
//    {  
//        OLED_WR_Byte (0xb0+i,OLED_CMD);    //设置页地址（0~7）
//        OLED_WR_Byte (0x00,OLED_CMD);      //设置显示位置—列低地址
//        OLED_WR_Byte (0x10,OLED_CMD);      //设置显示位置—列高地址   
//        for(n=0;n<128;n++)OLED_WR_Byte(0,OLED_DATA); 
//    } //更新显示
    static uint8_t clear_buf[128] = {0};  // 静态缓冲区，避免每次初始化
    // 循环8页，每页设置位置后连续写128字节0
    for (uint8_t i = 0; i < 8; i++) {  
        OLED_Set_Pos(0, i);  // 优化后：一次I2C传输3个命令
        // 用无ACK检查的连续数据传输（仅清屏用）
        OLED_Write_ContinuousData_NoAck(clear_buf, 128);  
    }
}
 
//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63				 
//sizey:选择字体 6x8  8x16
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t sizey)
{      	
    uint8_t c=0,sizex=sizey/2;
    uint16_t i=0,size1;
    if(sizey==8)size1=6;
    else size1=(sizey/8+((sizey%8)?1:0))*(sizey/2);
    c=chr-' ';//得到偏移后的值
    OLED_Set_Pos(x,y);
    for(i=0;i<size1;i++)
    {
        if(i%sizex==0&&sizey!=8) OLED_Set_Pos(x,y++);
        if(sizey==8) OLED_WR_Byte(asc2_0806[c][i],OLED_DATA);//6X8字号
        else if(sizey==16) OLED_WR_Byte(asc2_1608[c][i],OLED_DATA);//8x16字号
        //		else if(sizey==xx) OLED_WR_Byte(asc2_xxxx[c][i],OLED_DATA);//用户添加字号
        else return;
    }
}
 
//m^n函数
uint32_t oled_pow(uint8_t m,uint8_t n)
{
    uint32_t result=1;	 
    while(n--)result*=m;    
    return result;
}
 
//显示数字
//x,y :起点坐标
//num:要显示的数字
//len :数字的位数
//sizey:字体大小		  
void OLED_ShowNum(uint8_t x,uint8_t y,uint32_t num,uint8_t len,uint8_t sizey)
{         	
    uint8_t t,temp,m=0;
    uint8_t enshow=0;
    if(sizey==8)m=2;
    for(t=0;t<len;t++)
    {
        temp=(num/oled_pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                OLED_ShowChar(x+(sizey/2+m)*t,y,' ',sizey);
                continue;
            }else enshow=1;
        }
        OLED_ShowChar(x+(sizey/2+m)*t,y,temp+'0',sizey);
    }
}
 
//显示一个字符号串
void OLED_ShowString(uint8_t x,uint8_t y,char *chr,uint8_t sizey)
{
    uint8_t j=0;
    while (chr[j]!='\0')
    {		
        OLED_ShowChar(x,y,chr[j++],sizey);
        if(sizey==8)x+=6;
        else x+=sizey/2;
    }
}
 


 
//初始化SSD1306					    
void OLED_Init(void)
{
    Delay_ms(100);
    OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
    OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
    OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
    OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
    OLED_WR_Byte(0xCF,OLED_CMD); // Set SEG Output Current Brightness
    OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
    OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
    OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WR_Byte(0x00,OLED_CMD);//-not offset
    OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
    OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
    OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
    OLED_WR_Byte(0x12,OLED_CMD);
    OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
    OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
    OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WR_Byte(0x02,OLED_CMD);//
    OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
    OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
    OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
    OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
    OLED_Clear();
    OLED_WR_Byte(0xAF,OLED_CMD); /*display ON*/ 
} 
