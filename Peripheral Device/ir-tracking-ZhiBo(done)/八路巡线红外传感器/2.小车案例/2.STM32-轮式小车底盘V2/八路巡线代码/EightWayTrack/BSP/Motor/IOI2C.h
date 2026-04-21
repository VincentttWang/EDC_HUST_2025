#ifndef __IOI2C_H
#define __IOI2C_H

#include "AllHeader.h"

//IO方向设置	IO direction setting
#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=8<<12;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=3<<12;}

//IO操作函数	 IO operation function
#define IIC_SCL    PBout(10) //SCL
#define IIC_SDA    PBout(11) //SDA	 
#define READ_SDA   PBin(11)  //输入SDA 	Input SDA


//IIC所有操作函数	IIC all operation functions
int IIC_Start(void);				//发送IIC开始信号	Send IIC start signal
void IIC_Stop(void);	  			//发送IIC停止信号	Send IIC stop signal
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节	IIC sends a byte
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节	IIC reads a byte
int IIC_Wait_Ack(void); 			//IIC等待ACK信号		IIC waits for ACK signal
void IIC_Ack(void);					//IIC发送ACK信号		IIC sends ACK signal
void IIC_NAck(void);				//IIC不发送ACK信号	IIC does not send ACK signal

int i2cWrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data);
int i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);

#endif

//------------------End of File----------------------------
