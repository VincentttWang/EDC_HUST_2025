#include "bsp_motor_iic.h"

int Encoder_Offset[4];
int Encoder_Now[4];

  
//浮点型转bytes位 //bytes 4个长度 因为float是4字节	Convert float to bytes //bytes 4 in length because float is 4 bytes
void float_to_bytes(float f, uint8_t *bytes) 
{
    memcpy(bytes, &f, sizeof(float));
}

//bytes位转成浮点型	Convert bytes to floating point
float char2float(char *p)
{
  float *p_Int;
  p_Int = (float *)malloc(sizeof(float));
  memcpy(p_Int, p, sizeof(float));
  float x = *p_Int;
  free(p_Int);
  return x;
}

void IIC_Motor_Init(void)
{			
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能PB端口时钟	Enable PB port clock
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;//PB10->SCL	  PB11->SDA
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      //推挽输出	Push-pull output
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	  //50M
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//配置电机	Configure the motor
void Set_motor_type(uint8_t data)
{	
	i2cWrite(Motor_model_ADDR,MOTOR_TYPE_REG,2,&data);
}

//配置死区	Configuring Dead Zone
void Set_motor_deadzone(uint16_t data)
{
	static uint8_t buf_tempzone[2];
	
	buf_tempzone[0] = (data>>8)&0xff;
	buf_tempzone[1] = data;
	
	i2cWrite(Motor_model_ADDR,MOTOR_DeadZONE_REG,2,buf_tempzone);
}

//配置磁环线	Configuring magnetic loop
void Set_Pluse_line(uint16_t data)
{
	static uint8_t buf_templine[2];
	
	buf_templine[0] = (data>>8)&0xff;
	buf_templine[1] = data;
	
	i2cWrite(Motor_model_ADDR,MOTOR_PluseLine_REG,2,buf_templine);
}

//配置减速比	Configure the reduction ratio
void Set_Pluse_Phase(uint16_t data)
{
	static uint8_t buf_tempPhase[2];
	
	buf_tempPhase[0] = (data>>8)&0xff;
	buf_tempPhase[1] = data;
	
	i2cWrite(Motor_model_ADDR,MOTOR_PlusePhase_REG,2,buf_tempPhase);
}


//配置直径	Configuration Diameter
void Set_Wheel_dis(float data)
{
	static uint8_t bytes[4];
	
	float_to_bytes(data,bytes);
	
	i2cWrite(Motor_model_ADDR,WHEEL_DIA_REG,4,bytes);
}

//只能控制带编码器类型的电机	Can only control motors with encoders
//传入参数:4个电机的速度		Input parameters: speed of 4 motors
void control_speed(int16_t m1,int16_t m2 ,int16_t m3,int16_t m4)
{
	static uint8_t speed[8];
	
	speed[0] = (m1>>8)&0xff;
	speed[1] = (m1)&0xff;
	
	speed[2] = (m2>>8)&0xff;
	speed[3] = (m2)&0xff;
	
	speed[4] = (m3>>8)&0xff;
	speed[5] = (m3)&0xff;
	
	speed[6] = (m4>>8)&0xff;
	speed[7] = (m4)&0xff;
	
	i2cWrite(Motor_model_ADDR,SPEED_Control_REG,8,speed);

}


//控制带编码器类型的电机	Control the motor with encoder type
//传入参数:4个电机的pwm	PWM of 4 motors
//此函数可以结合实时编码器的数据，来实现control_speed的功能	This function can combine the data of real-time encoder to realize the function of control_speed
void control_pwm(int16_t m1,int16_t m2 ,int16_t m3,int16_t m4)
{
	static uint8_t pwm[8];
	
	pwm[0] = (m1>>8)&0xff;
	pwm[1] = (m1)&0xff;
	
	pwm[2] = (m2>>8)&0xff;
	pwm[3] = (m2)&0xff;
	
	pwm[4] = (m3>>8)&0xff;
	pwm[5] = (m3)&0xff;
	
	pwm[6] = (m4>>8)&0xff;
	pwm[7] = (m4)&0xff;
	
	i2cWrite(Motor_model_ADDR,PWM_Control_REG,8,pwm);

}


//读取相对时间的编码器的数据 10ms的	Read the data of the encoder of relative time 10ms
void Read_10_Enconder(void)
{
	static int8_t buf[2];
		
	//M1电机编码器的数据	M1 motor encoder data
	i2cRead(Motor_model_ADDR, READ_TEN_M1Enconer_REG, 2, buf);
	Encoder_Offset[0] = buf[0]<<8|buf[1]; 
	
	//M2电机编码器的数据	M2 motor encoder data
	i2cRead(Motor_model_ADDR, READ_TEN_M2Enconer_REG, 2, buf);
	Encoder_Offset[1] = buf[0]<<8|buf[1];
	
	//M3电机编码器的数据	M3 motor encoder data
	i2cRead(Motor_model_ADDR, READ_TEN_M3Enconer_REG, 2, buf);
	Encoder_Offset[2] = buf[0]<<8|buf[1];
	
	//M4电机编码器的数据	M4 motor encoder data
	i2cRead(Motor_model_ADDR, READ_TEN_M4Enconer_REG, 2, buf);
	Encoder_Offset[3] = buf[0]<<8|buf[1];
	
}

//读取电机转动的编码器数据	Read the encoder data of the motor rotation
void Read_ALL_Enconder(void)
{
	static uint8_t buf[2];
	static uint8_t buf2[2];
	
//	M1电机编码器的数据	M1 motor encoder data
//	i2cRead(Motor_model_ADDR, READ_ALLHigh_M1_REG, 2, buf);
//	i2cRead(Motor_model_ADDR, READ_ALLLOW_M1_REG, 2, buf2);
	
	Encoder_Now[0] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1]; 
	
	//M2电机编码器的数据	M2 motor encoder data
	i2cRead(Motor_model_ADDR, READ_ALLHigh_M2_REG, 2, buf);
	i2cRead(Motor_model_ADDR, READ_ALLLOW_M2_REG, 2, buf2);
	Encoder_Now[1] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1];
	
//	M3电机编码器的数据	M3 motor encoder data
//	i2cRead(Motor_model_ADDR, READ_ALLHigh_M3_REG, 2, buf);
//	i2cRead(Motor_model_ADDR, READ_ALLLOW_M3_REG, 2, buf2);
//	Encoder_Now[2] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1];
	
	
	//M4电机编码器的数据	M4 motor encoder data
	i2cRead(Motor_model_ADDR, READ_ALLHigh_M4_REG, 2, buf);
	i2cRead(Motor_model_ADDR, READ_ALLLOW_M4_REG, 2, buf2);
	Encoder_Now[3] = buf[0]<<24|buf[1]<<16|buf2[0]<<8|buf2[1];
	
}
