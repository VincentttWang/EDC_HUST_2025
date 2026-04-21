#include "ioi2c.h"

/**************************************************************************
Function: Simulate IIC start signal
Input   : none
Output  : 1
函数功能：模拟IIC起始信号
入口参数：无
返回  值：1
**************************************************************************/
int IIC_Start(void)
{
	SDA_OUT();     //sda线输出	sda line output
	IIC_SDA=1;
	if(!READ_SDA)return 0;	
	IIC_SCL=1;
	delay_us(1);
 	IIC_SDA=0; //START:when CLK is high,DATA change form high to low 
	if(READ_SDA)return 0;
	delay_us(1);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 	Clamp the I2C bus and prepare to send or receive data
	return 1;
}

/**************************************************************************
Function: Analog IIC end signal
Input   : none
Output  : none
函数功能：模拟IIC结束信号
入口参数：无
返回  值：无
**************************************************************************/	  
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出	sda line output
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(1);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号	Send I2C bus end signal
	delay_us(1);							   	
}

/**************************************************************************
Function: IIC wait the response signal
Input   : none
Output  : 0：No response received；1：Response received
函数功能：IIC等待应答信号
入口参数：无
返回  值：0：没有收到应答；1：收到应答
**************************************************************************/
int IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  SDA is set as input
	IIC_SDA=1;
	delay_us(1);	   
	IIC_SCL=1;
	delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>50)
		{
			IIC_Stop();
			return 0;
		}
	  delay_us(1);
	}
	IIC_SCL=0;//时钟输出0 	 Clock output 0  
	return 1;  
} 

/**************************************************************************
Function: IIC response
Input   : none
Output  : none
函数功能：IIC应答
入口参数：无
返回  值：无
**************************************************************************/
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(1);
	IIC_SCL=1;
	delay_us(1);
	IIC_SCL=0;
}
	
/**************************************************************************
Function: IIC don't reply
Input   : none
Output  : none
函数功能：IIC不应答
入口参数：无
返回  值：无
**************************************************************************/    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(1);
	IIC_SCL=1;
	delay_us(1);
	IIC_SCL=0;
}
/**************************************************************************
Function: IIC sends a byte
Input   : txd：Byte data sent
Output  : none
函数功能：IIC发送一个字节
入口参数：txd：发送的字节数据
返回  值：无
**************************************************************************/	  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	  SDA_OUT(); 	    
    IIC_SCL=0;//拉低时钟开始数据传输	Pull the clock low to start data transmission
    for(t=0;t<8;t++)
    {              
			IIC_SDA=(txd&0x80)>>7;
			txd<<=1; 	  
			delay_us(1);   
			IIC_SCL=1;
			delay_us(1); 
			IIC_SCL=0;	
			delay_us(1);
    }	 
} 	 
  
/**************************************************************************
Function: IIC write data to register
Input   : addr：Device address；reg：Register address；len;Number of bytes；data：Data
Output  : 0：Write successfully；1：Failed to write
函数功能：IIC写数据到寄存器
入口参数：addr：设备地址；reg：寄存器地址；len;字节数；data：数据
返回  值：0：成功写入；1：没有成功写入
**************************************************************************/
int i2cWrite(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *data)
{
		int i;
    if (!IIC_Start())
        return 1;
    IIC_Send_Byte(addr << 1 );
    if (!IIC_Wait_Ack()) {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
		for (i = 0; i < len; i++) {
        IIC_Send_Byte(data[i]);
        if (!IIC_Wait_Ack()) {
            IIC_Stop();
            return 0;
        }
    }
    IIC_Stop();
    return 0;
}
/**************************************************************************
Function: IIC read register data
Input   : addr：Device address；reg：Register address；len;Number of bytes；*buf：Data read out
Output  : 0：Read successfully；1：Failed to read
函数功能：IIC读寄存器的数据
入口参数：addr：设备地址；reg：寄存器地址；len;字节数；*buf：读出数据缓存
返回  值：0：成功读出；1：没有成功读出
**************************************************************************/

int i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
    if (!IIC_Start())
        return 1;
    IIC_Send_Byte(addr << 1);
    if (!IIC_Wait_Ack()) {
        IIC_Stop();
        return 1;
    }
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte((addr << 1)+1);
    IIC_Wait_Ack();
    while (len) {
        if (len == 1)
            *buf = IIC_Read_Byte(0);
        else
            *buf = IIC_Read_Byte(1);
        buf++;
        len--;
    }
    IIC_Stop();
    return 0;
}

/**************************************************************************
Function: IIC reads a byte
Input   : ack：Send response signal or not；1：Send；0：Do not send
Output  : receive：Data read
函数功能：IIC读取一个位
入口参数：ack：是否发送应答信号；1：发送；0：不发送
返回  值：receive：读取的数据
**************************************************************************/ 
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入	SDA is set as input
    for(i=0;i<8;i++ )
	 {
			IIC_SCL=0; 
			delay_us(2);
			IIC_SCL=1;
			receive<<=1;
			if(READ_SDA)receive++;   
			delay_us(2); 
    }					 
    if (ack)
        IIC_Ack(); //发送ACK 	Send ACK
    else
        IIC_NAck();//发送nACK 	Send nACK 
    return receive;
}
