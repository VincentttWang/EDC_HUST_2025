#include "app_irtracking.h"

#define IRTrack_Trun_KP (15)
#define IRTrack_Trun_KI (0.2) 
#define IRTrack_Trun_KD (2) 
#define IRR_SPEED 			  200  //巡线速度   Patrol speed
#define CHANGE_THRESHOLD 3
const float pid_out_max = 5000.0f; 
const float Integral_max = 500.0f; // 积分限幅值 
int pid_output_IRR = 0;
u8 trun_flag = 0;
static int8_t err = 0;
// 存储上一次的传感器数据组合
static uint8_t prev_sensor_data = 0;



float PID_IR_Calc(int16_t actual_value)
{
    float pid_out = 0;
    int16_t error; 
    static int16_t error_last = 0; //上次的误差初始为0  Last error
    static float Integral = 0; // 初始化积分项 Initialize integral term

    error = actual_value;
//    if(err == 0)          
//    {
//        Integral = 0;          //积分清零   Integral cleared
//    }
    Integral += error;           // 更新积分项，并进行限幅 Update the integral term and limit it
    if (Integral > Integral_max) Integral = Integral_max;               //积分限幅 Integral limiting
    if (Integral < -Integral_max) Integral = -Integral_max;             //积分限幅 Integral limiting

    // 位置式 PID
    pid_out = error * IRTrack_Trun_KP
              + IRTrack_Trun_KI * Integral
              + (error - error_last) * IRTrack_Trun_KD;

    error_last = error;       // 更新积分项，并进行限幅 Update the integral term and limit it

    // 对输出进行限幅Output limiting value
    if (pid_out > pid_out_max) pid_out = pid_out_max;  
    if (pid_out < -pid_out_max) pid_out = -pid_out_max;

    return pid_out;
}
//I2Cд????    I2C Write Data
void IRI2C_WriteByte(uint8_t addr, uint8_t dat) {
    uint8_t temp[2];
    temp[0] = addr;
    temp[1] = dat;
 
    DL_I2C_fillControllerTXFIFO(I2C_1_INST, temp, 2);
    while (!(DL_I2C_getControllerStatus(I2C_1_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
 
    DL_I2C_startControllerTransfer(I2C_1_INST, IR_I2C_ADDR, DL_I2C_CONTROLLER_DIRECTION_TX, 2);
    while (DL_I2C_getControllerStatus(I2C_1_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C_1_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_flushControllerTXFIFO(I2C_1_INST);
}

//I2C??????    I2C Read Data
uint8_t IRI2C_ReadByte(uint8_t addr) {
    uint8_t data;
 
    DL_I2C_fillControllerTXFIFO(I2C_1_INST, &addr, 1);
	
	DL_I2C_disableInterrupt(I2C_1_INST, DL_I2C_INTERRUPT_CONTROLLER_TXFIFO_TRIGGER);
	
    while (!(DL_I2C_getControllerStatus(I2C_1_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    DL_I2C_startControllerTransfer(I2C_1_INST, IR_I2C_ADDR, DL_I2C_CONTROLLER_DIRECTION_TX, 1);
    while (DL_I2C_getControllerStatus(I2C_1_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C_1_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
//    DL_I2C_flushControllerTXFIFO(I2C_1_INST);
 
    DL_I2C_startControllerTransfer(I2C_1_INST, IR_I2C_ADDR, DL_I2C_CONTROLLER_DIRECTION_RX, 1);
    while (DL_I2C_getControllerStatus(I2C_1_INST) & DL_I2C_CONTROLLER_STATUS_BUSY_BUS);
    while (!(DL_I2C_getControllerStatus(I2C_1_INST) & DL_I2C_CONTROLLER_STATUS_IDLE));
    data = DL_I2C_receiveControllerData(I2C_1_INST);
 
    return data;
}

//?????·????????????   Getting data from the 8-way patrol module
void deal_IRdata(u8 *x1,u8 *x2,u8 *x3,u8 *x4,u8 *x5,u8 *x6,u8 *x7,u8 *x8)
{
	u8 IRbuf = 0xFF;
	IRbuf = IRI2C_ReadByte(0x30);
	
	*x1 = (IRbuf>>7)&0x01;
	*x2 = (IRbuf>>6)&0x01;
	*x3 = (IRbuf>>5)&0x01;
	*x4 = (IRbuf>>4)&0x01;
	*x5 = (IRbuf>>3)&0x01;
	*x6 = (IRbuf>>2)&0x01;
	*x7 = (IRbuf>>1)&0x01;
	*x8 = (IRbuf>>0)&0x01;
}
void printf_i2c_data(void)
{
    static uint8_t ir_x1,ir_x2,ir_x3,ir_x4,ir_x5,ir_x6,ir_x7,ir_x8;
    deal_IRdata(&ir_x1,&ir_x2,&ir_x3,&ir_x4,&ir_x5,&ir_x6,&ir_x7,&ir_x8);
    printf("x1:%d,x2:%d,x3:%d,x4:%d,x5:%d,x6:%d,x7:%d,x8:%d\r\n",ir_x1,ir_x2,ir_x3,ir_x4,ir_x5,ir_x6,ir_x7,ir_x8);
}
void LineWalking(void)
{
	static int8_t err = 0;
	static u8 x1,x2,x3,x4,x5,x6,x7,x8;
	
	
 	deal_IRdata(&x1,&x2,&x3,&x4,&x5,&x6,&x7,&x8);
//优先判断是否到直角或锐角  Prioritize whether to right angles or acute angles
	 if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 0 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 0000 0111
	{
		err = -50;
       delay_ms(100);
	}
	
	//右锐角 写在这里测试可以生效
	else if(x1==0 && x8 ==0 && x4 ==1 && x5 ==1)
{
   err = 70;  // 增加转向幅度
   delay_ms(100);  // 统一延迟时间
}

	else if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 0 && x5 ==1  && x6 == 1  && x7 == 1 && x8 == 1) // 0000 1111
	{
		err = -50;
       delay_ms(100);
	}

		else if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 00011111
	{
		err = -60;
       delay_ms(100);
	}
   else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0) // 1110 0000
	{
		err = 60;
       delay_ms(100);
	}
	    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0) // 1111 0000
	{
		err = 50;
       delay_ms(100);
	}
		 else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 0 && x8 == 0) // 1111 1000
	{
		err = 50;
       delay_ms(100);
	}
	
	else if((x1 == 0 && x2 == 1 && x3 == 1) && (x6 == 0 || x7 == 0 || x8 == 0))
{
   err = -70;  // 增加转向幅度
   delay_ms(100);  // 统一延迟时间
}
// 优化右锐角弯道判断

	else if((x8 == 0 && x4 == 0 && x5 == 1)||(x8 == 0 && x7==0&& x1==0 &&x2==0&& x4 == 1 && x5 == 1)||((x8 == 0 && x7 == 1 && x6 == 1) && (x1 == 0 || x2 == 0 || x3 == 0)))
{
   err = 70;  // 增加转向幅度
   delay_ms(100);  // 统一延迟时间
}

 else if(x1 == 0 &&  x2 == 0  && x7 == 0 && x8 == 0 ) //俩边都亮，直跑    Both sides are lit. Run straight.
	{
		err = 0;
		if(trun_flag == 1)
		{
			trun_flag = 0;//走到圈了    Walking in circles.
		}
	}
	

	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1110 1111
	{
		err = -1;
	}
	else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1100 1111
	{
		err = -2;
	}

	else if(x1 == 1 && x2 == 0  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1001 1111
	{
		err = -2;
	}
   
		else if(x1 == 0 && x2 == 0  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0011 1111
	{
		err = -2;  
	}
	else if(x1 == 0 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0111 1111
	{
		err = -50; 
	}

	
	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 1111 0111
	{
		err = 1;
	} 
	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 0  && x7 == 1 && x8 == 1) // 1111 0011
	{
		err = 2;
	}

	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 0 && x8 == 1) // 1111 1001
	{
		err = 3;
	}
	

	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 0 && x8 == 0) // 1111 1100
	{
		err = 2; 
	}
		else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 0) // 1111 1110
	{
		err = 50;
	}
	

	

	else if(x1 == 1 &&x2 == 1 &&x3 == 1 && x4 == 0 && x5 == 0 && x6 == 1 && x7 == 1&& x8 == 1) //直走 go straight
	{
		err = 0;
	}
   
	//剩下的就保持上一个状态	    The rest will stay the same.
   
   
	pid_output_IRR = (int)(PID_IR_Calc(err));

	Motion_Car_Control(IRR_SPEED, 0, pid_output_IRR);	

}

// void LineWalking(void)
// {
// 	static int8_t err = 0;
// 	static u8 x1,x2,x3,x4,x5,x6,x7,x8;
	
// 	deal_IRdata(&x1,&x2,&x3,&x4,&x5,&x6,&x7,&x8);
	
// 	///L1?X1???????????1??????????0     L1 is X1, 1 when the white background is off, 0 when the black line is on///
	
// //?????ж?  Priority judgment
//     //1100 0011
// //	if(x1 == 1 && x2 == 1 &&x3 == 0 &&  x4 == 0  && x5 == 0 && x6  == 0 && x7 == 1 && x8 == 1 ) //??????   transverse acute angle
// //	{
// //		err = 15; 
// //	}
// //	else if(x1 == 1 && x2 == 1 &&x3 == 1 &&  x4 == 1  && x5 == 1 && x6  == 1 && x7 == 1 && x8 == 1 ) //??????  transverse acute angle
// //	{
// //		if(trun_flag == 0) //??????    out of the line
// //		{
// //			err = 15; 
// //			trun_flag = 1;
// //		}
// //		//?????????????????    Otherwise, the situation remains the same as before.
// //	}

// //?????ж??????????????  Prioritize whether to right angles or acute angles
// 	 if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 0 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 0000 0111
// 	{
// 		err = -15;
//         delay_ms(100);
// 	}
//     else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0) // 1110 0000
// 	{
// 		err = 15;
//         delay_ms(100);
// 	}

//   else if(x1 == 0 &&  x2 == 0  && x7 == 0 && x8 == 0 ) //????????????    Both sides are lit. Run straight.
// 	{
// 		err = 0;
// 		if(trun_flag == 1)
// 		{
// 			trun_flag = 0;//??????    Walking in circles.
// 		}
// 	}
	
// // else if(x1 == 0 &&  x3 == 0 && x4 == 0 && x5 == 0 && x8 == 0 )
// //	{
// //		err = 0;
// //	}
// //	//???????  Add Right Angle
// //	else if((x1 == 0 || x2 == 0 ) && x8 == 1) 
// //	{
// //		err = -15; 
// //	}
// //	//???????  Add Right Angle
// //	else if((x7 == 0 ||  x8 == 0) && x1 == 1) 
// //	{
// //		err = 15 ;
// //	}
	


// 	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1110 1111
// 	{
// 		err = -1;
// 	}
// 	else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1100 1111
// 	{
// 		err = -2;
// 	}
// //	else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1101 1111
// //	{
// //		err = -2;
// //	}
	
// //	else if(x1 == 1 && x2 == 0  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1011 1111
// //	{
// //		err = -3;
// //	}
// 	else if(x1 == 1 && x2 == 0  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1001 1111
// 	{
// 		err = -8;
// 	}
    
// //		else if(x1 == 0 && x2 == 0  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0011 1111
// //	{
// //		err = -4;   //?????????????? Note, when treated as a right angle
// //	}
// 	else if(x1 == 0 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0111 1111
// 	{
// 		err = -10; 
// 	}

	

	
	
	
// 	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 1111 0111
// 	{
// 		err = 1;
// 	} 
// 	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 0  && x7 == 1 && x8 == 1) // 1111 0011
// 	{
// 		err = 2;
// 	}
// //	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 1 && x8 == 1) // 1111 1011
// //	{
// //		err = 2;
// //	}
// 	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 0 && x8 == 1) // 1111 1001
// 	{
// 		err = 8;
// 	}
	
// //	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 0 && x8 == 1) // 1111 1101
// //	{
// //		err = 3;
// //	}
// //	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 0 && x8 == 0) // 1111 1100
// //	{
// //		err = 4; ///??????????  treat as a right angle
// //	}
// 		else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 0) // 1111 1110
// 	{
// 		err = 10;
// 	}
	

	
 
// 	else if(x1 == 1 &&x2 == 1 &&x3 == 1 && x4 == 0 && x5 == 0 && x6 == 1 && x7 == 1&& x8 == 1) //??? go straight
// 	{
// 		err = 0;
// 	}
    
// 	//??μ????????????	    The rest will stay the same.
    
    
// 	pid_output_IRR = (int)(PID_IR_Calc(err));

// 	Motion_Car_Control(IRR_SPEED, 0, pid_output_IRR);

// }





