#include "app_motor_usart.h"

#define RXBUFF_LEN 256

uint8_t send_buff[50];

float g_Speed[4];
int Encoder_Offset[4];
int Encoder_Now[4];

uint8_t g_recv_flag; 
uint8_t g_recv_buff[RXBUFF_LEN];
uint8_t g_recv_buff_deal[RXBUFF_LEN];

//////////********************发送部分********************///////////
//////////******************Sending part*****************///////////

//发送电机类型	Transmitter motor type
void send_motor_type(motor_type_t data)
{
	sprintf((char*)send_buff,"$mtype:%d#",data);
	Send_Motor_ArrayU8(send_buff, strlen((char*)send_buff));
	
}

//发送电机死区	Send motor dead zone
void send_motor_deadzone(uint16_t data)
{
	sprintf((char*)send_buff,"$deadzone:%d#",data);
	Send_Motor_ArrayU8(send_buff, strlen((char*)send_buff));
}

//发送电机磁环脉冲	Send motor magnetic ring pulse
void send_pulse_line(uint16_t data)
{
	sprintf((char*)send_buff,"$mline:%d#",data);
	Send_Motor_ArrayU8(send_buff, strlen((char*)send_buff));
}

//发送电机减速比	Transmitting motor reduction ratio
void send_pulse_phase(uint16_t data)
{
	sprintf((char*)send_buff,"$mphase:%d#",data);
	Send_Motor_ArrayU8(send_buff, strlen((char*)send_buff));
}

//发送轮子直径	Send wheel diameter
void send_wheel_diameter(float data)
{
	sprintf((char*)send_buff,"$wdiameter:%.3f#",data);
	Send_Motor_ArrayU8(send_buff, strlen((char*)send_buff));
}

//发送PID参数	Send PID parameters
void send_motor_PID(float P,float I,float D)
{
	sprintf((char*)send_buff,"$mpid:%.3f,%.3f,%.3f#",P,I,D);
	Send_Motor_ArrayU8(send_buff, strlen((char*)send_buff));
}

//需要接收数据的开关	Switch that needs to receive data
void send_upload_data(bool ALLEncoder_Switch,bool TenEncoder_Switch,bool Speed_Switch)
{
	sprintf((char*)send_buff,"$upload:%d,%d,%d#",ALLEncoder_Switch,TenEncoder_Switch,Speed_Switch);
	Send_Motor_ArrayU8(send_buff, strlen((char*)send_buff));
}

//控制速度	Controlling Speed
void Contrl_Speed(int16_t M1_speed,int16_t M2_speed,int16_t M3_speed,int16_t M4_speed)
{
	sprintf((char*)send_buff,"$spd:%d,%d,%d,%d#",M1_speed,M2_speed,M3_speed,M4_speed);
	UART_Console_write(send_buff, strlen((char*)send_buff));
//    Send_Motor_ArrayU8(send_buff, strlen((char*)send_buff));
}


//控制pwm	Control PWM
void Contrl_Pwm(int16_t M1_pwm,int16_t M2_pwm,int16_t M3_pwm,int16_t M4_pwm)
{
	sprintf((char*)send_buff,"$pwm:%d,%d,%d,%d#",M1_pwm,M2_pwm,M3_pwm,M4_pwm);
    UART_Console_write(send_buff, strlen((char*)send_buff));
//	Send_Motor_ArrayU8(send_buff, strlen((char*)send_buff));
}


//////////********************接收部分********************///////////
//////////*****************Receiving part****************///////////

//传入参数：保留的字符串(指针数组)  原始字符串  分隔符号
//Incoming parameters: reserved string (pointer array) original string separator
void splitString(char* mystrArray[],char *str, const char *delimiter) 
{
    char *token = strtok(str, delimiter); //这是第一次分割,第一个字符值	This is the first split, the first character value
		mystrArray[0] = token; //保留第一次分割的字符		Keep the first split character
    int i =1;
	
    while (token != NULL) 
    {
        token = strtok(NULL, delimiter);
        mystrArray[i] = token;
        i++;
    }
}

//检验从驱动板发送过来的数据，符合通讯协议的数据则保存下来
//Check the data sent from the driver board, and save the data that meets the communication protocol
void Deal_Control_Rxtemp(uint8_t rxtemp)
{
	static u8 step = 0;
	static u8 start_flag = 0;

	if(rxtemp == '$' && 	start_flag == 0)
	{
		start_flag = 1;
		memset(g_recv_buff,0,RXBUFF_LEN);//清空数据	Clear data
	}
	
	else if(start_flag == 1)
	{
			if(rxtemp == '#')
			{
				start_flag = 0;
				step = 0;
				g_recv_flag = 1;
				memcpy(g_recv_buff_deal,g_recv_buff,RXBUFF_LEN); //只有正确才会赋值	Only correct ones will be assigned
			}
			else
			{
				if(step > RXBUFF_LEN)
				{
					start_flag = 0;
					step = 0;
					memset(g_recv_buff,0,RXBUFF_LEN);//清空接收数据	Clear received data
				}
				else
				{
					g_recv_buff[step] = rxtemp;
					step++;
				}
			}
	}
	
}

//将从驱动板保存到的数据进行格式处理，然后准备打印
//Format the data saved from the driver board and prepare it for printing
void Deal_data_real(void)
{
	 static uint8_t data[RXBUFF_LEN];
   uint8_t  length = 0;
	
	//总体的编码器	Overall encoder
	 if ((strncmp("MAll",(char*)g_recv_buff_deal,4)==0))
    {
        length = strlen((char*)g_recv_buff_deal)-5;
        for (uint8_t i = 0; i < length; i++)
        {
            data[i] = g_recv_buff_deal[i+5]; //去掉冒号	Remove the colon
        }  
				data[length] = '\0';	

					
				char* strArray[10];//指针数组 长度根据分割号定义  char 1字节   char* 4字节	 Pointer array The length is defined by the split number char 1 byte char* 4 bytes
				char mystr_temp[4][10] = {'\0'}; 
				splitString(strArray,(char*)data, ", ");//以逗号切割	Split by comma
				for (int i = 0; i < 4; i++)
				{
						strcpy(mystr_temp[i],strArray[i]);
						Encoder_Now[i] = atoi(mystr_temp[i]);
				}
				
		}
		//10ms的实时编码器数据	10ms real-time encoder data
		else if	((strncmp("MTEP",(char*)g_recv_buff_deal,4)==0))
    {
        length = strlen((char*)g_recv_buff_deal)-5;
        for (uint8_t i = 0; i < length; i++)
        {
            data[i] = g_recv_buff_deal[i+5]; //去掉冒号	Remove the colon
        }  
				data[length] = '\0';		

				char* strArray[10];//指针数组 长度根据分割号定义  char 1字节   char* 4字节		Pointer array The length is defined by the split number char 1 byte char* 4 bytes
				char mystr_temp[4][10] = {'\0'}; 
				splitString(strArray,(char*)data, ", ");//以逗号切割	Split by comma
				for (int i = 0; i < 4; i++)
				{
						strcpy(mystr_temp[i],strArray[i]);
						Encoder_Offset[i] = atoi(mystr_temp[i]);
				}
		}
		//速度	Speed
		else if	((strncmp("MSPD",(char*)g_recv_buff_deal,4)==0))
    {
        length = strlen((char*)g_recv_buff_deal)-5;
        for (uint8_t i = 0; i < length; i++)
        {
            data[i] = g_recv_buff_deal[i+5]; //去掉冒号	Remove the colon
        }  
				data[length] = '\0';	
				
				char* strArray[10];//指针数组 长度根据分割号定义  char 1字节   char* 4字节		Pointer array The length is defined by the split number char 1 byte char* 4 bytes
				char mystr_temp[4][10] = {'\0'}; 
				splitString(strArray,(char*)data, ", ");//以逗号切割	Split by comma
				for (int i = 0; i < 4; i++)
				{
						strcpy(mystr_temp[i],strArray[i]);
						g_Speed[i] = atof(mystr_temp[i]);
				}
		}
}
