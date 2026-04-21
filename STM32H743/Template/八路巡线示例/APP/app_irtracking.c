#include "app_irtracking.h"

#define IRTrack_Trun_KP (15)
#define IRTrack_Trun_KI (0.2) 
#define IRTrack_Trun_KD (2) 
#define IRR_SPEED 			  200  //巡线速度   Patrol speed
// 定义一个阈值，用于判断传感器数据变化是否剧烈
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
void LineWalking(void)
{
    static int8_t err = 0;
    static u8 x1,x2,x3,x4,x5,x6,x7,x8;
    
    x1 = IR_Data_number[0];
    x2 = IR_Data_number[1];
    x3 = IR_Data_number[2];
    x4 = IR_Data_number[3];
    x5 = IR_Data_number[4];
    x6 = IR_Data_number[5];
    x7 = IR_Data_number[6];
    x8 = IR_Data_number[7];

    // 将传感器数据组合成一个 8 位二进制数 / Combine sensor data into an 8-bit binary number
    u8 sensor_pattern = (x1 << 7) | (x2 << 6) | (x3 << 5) | (x4 << 4) |
                        (x5 << 3) | (x6 << 2) | (x7 << 1) | x8;

    // 使用 switch-case 处理不同模式 / Use switch-case to handle different patterns
    switch (sensor_pattern)
    {
        // err = -50 的情况 / Cases for err = -50
        case 0b00000111: // 0000 0111
        case 0b00001111: // 0000 1111
        case 0b01111111: // 0111 1111
            err = -50;
            delay_ms(100);
            break;

        // err = -60 的情况 / Cases for err = -60
        case 0b00011111: // 0001 1111
            err = -60;
            delay_ms(100);
            break;

        // err = 60 的情况 / Cases for err = 60
        case 0b11100000: // 1110 0000
            err = 60;
            delay_ms(100);
            break;

        // err = 50 的情况 / Cases for err = 50
        case 0b11110000: // 1111 0000
        case 0b11111000: // 1111 1000
        case 0b11111110: // 1111 1110
            err = 50;
            delay_ms(100);
            break;

        // err = 70 的情况 / Cases for err = 70
        case 0b00010001:
        case 0b00001110:
        case 0b00011110:
            err = 70;
            delay_ms(100);
            break;

        // err = -70 的情况 / Cases for err = -70
        case 0b01100000:
        case 0b01100001:
        case 0b01100010:
        case 0b01100011:
            err = -70;
            delay_ms(100);
            break;

        // err = 0 的情况 / Cases for err = 0
        case 0b00000000: // 直行 / Straight
        case 0b11100111: // 直走 / Go straight
            err = 0;
            if (trun_flag == 1)
            {
                trun_flag = 0; // 走到圈了 / Walking in circles
            }
            break;

        // err = -1 的情况 / Cases for err = -1
        case 0b11101111: // 1110 1111
            err = -1;
            break;

        // err = -2 的情况 / Cases for err = -2
        case 0b11001111: // 1100 1111
        case 0b10011111: // 1001 1111
        case 0b00111111: // 0011 1111
            err = -2;
            break;

        // err = 1 的情况 / Cases for err = 1
        case 0b11110111: // 1111 0111
            err = 1;
            break;

        // err = 2 的情况 / Cases for err = 2
        case 0b11110011: // 1111 0011
        case 0b11111100: // 1111 1100
            err = 2;
            break;

        // err = 3 的情况 / Cases for err = 3
        case 0b11111001: // 1111 1001
            err = 3;
            break;

        default:
            // 保持上一个状态 / Keep the previous state
            break;
    }

    // 计算 PID 输出并控制小车运动 / Calculate PID output and control car movement
    pid_output_IRR = (int)(PID_IR_Calc(err));
    Motion_Car_Control(IRR_SPEED, 0, pid_output_IRR);
}
//删减注释
//void LineWalking(void)
//{
//	static int8_t err = 0;
//	static u8 x1,x2,x3,x4,x5,x6,x7,x8;
//	
//	x1 = IR_Data_number[0];
//	x2 = IR_Data_number[1];
//	x3 = IR_Data_number[2];
//	x4 = IR_Data_number[3];
//	x5 = IR_Data_number[4];
//	x6 = IR_Data_number[5];
//	x7 = IR_Data_number[6];
//	x8 = IR_Data_number[7];
//	


////优先判断是否到直角或锐角  Prioritize whether to right angles or acute angles
//	 if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 0 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 0000 0111
//	{
//		err = -50;
//        delay_ms(100);
//	}
//	
//	//右锐角 写在这里测试可以生效
//	else if(x1==0 && x8 ==0 && x4 ==1 && x5 ==1)
//{
//    err = 70;  // 增加转向幅度
//    delay_ms(100);  // 统一延迟时间
//}

//	else if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 0 && x5 ==1  && x6 == 1  && x7 == 1 && x8 == 1) // 0000 1111
//	{
//		err = -50;
//        delay_ms(100);
//	}

//		else if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 00011111
//	{
//		err = -60;
//        delay_ms(100);
//	}
//    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0) // 1110 0000
//	{
//		err = 60;
//        delay_ms(100);
//	}
//	    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0) // 1111 0000
//	{
//		err = 50;
//        delay_ms(100);
//	}
//		 else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 0 && x8 == 0) // 1111 1000
//	{
//		err = 50;
//        delay_ms(100);
//	}
//	
//	else if((x1 == 0 && x2 == 1 && x3 == 1) && (x6 == 0 || x7 == 0 || x8 == 0))
//{
//    err = -70;  // 增加转向幅度
//    delay_ms(100);  // 统一延迟时间
//}
//// 优化右锐角弯道判断

//	else if((x8 == 0 && x4 == 0 && x5 == 1)||(x8 == 0 && x7==0&& x1==0 &&x2==0&& x4 == 1 && x5 == 1)||((x8 == 0 && x7 == 1 && x6 == 1) && (x1 == 0 || x2 == 0 || x3 == 0)))
//{
//    err = 70;  // 增加转向幅度
//    delay_ms(100);  // 统一延迟时间
//}

//  else if(x1 == 0 &&  x2 == 0  && x7 == 0 && x8 == 0 ) //俩边都亮，直跑    Both sides are lit. Run straight.
//	{
//		err = 0;
//		if(trun_flag == 1)
//		{
//			trun_flag = 0;//走到圈了    Walking in circles.
//		}
//	}
//	

//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1110 1111
//	{
//		err = -1;
//	}
//	else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1100 1111
//	{
//		err = -2;
//	}

//	else if(x1 == 1 && x2 == 0  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1001 1111
//	{
//		err = -2;
//	}
//    
//		else if(x1 == 0 && x2 == 0  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0011 1111
//	{
//		err = -2;  
//	}
//	else if(x1 == 0 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0111 1111
//	{
//		err = -50; 
//	}

//	
//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 1111 0111
//	{
//		err = 1;
//	} 
//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 0  && x7 == 1 && x8 == 1) // 1111 0011
//	{
//		err = 2;
//	}

//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 0 && x8 == 1) // 1111 1001
//	{
//		err = 3;
//	}
//	

//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 0 && x8 == 0) // 1111 1100
//	{
//		err = 2; 
//	}
//		else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 0) // 1111 1110
//	{
//		err = 50;
//	}
//	

//	
// 
//	else if(x1 == 1 &&x2 == 1 &&x3 == 1 && x4 == 0 && x5 == 0 && x6 == 1 && x7 == 1&& x8 == 1) //直走 go straight
//	{
//		err = 0;
//	}
//    
//	//剩下的就保持上一个状态	    The rest will stay the same.
//    
//    
//	pid_output_IRR = (int)(PID_IR_Calc(err));

//	Motion_Car_Control(IRR_SPEED, 0, pid_output_IRR);	

//}








//注释未删减版
//void LineWalking(void)
//{
//	static int8_t err = 0;
//	static u8 x1,x2,x3,x4,x5,x6,x7,x8;
//	
//	x1 = IR_Data_number[0];
//	x2 = IR_Data_number[1];
//	x3 = IR_Data_number[2];
//	x4 = IR_Data_number[3];
//	x5 = IR_Data_number[4];
//	x6 = IR_Data_number[5];
//	x7 = IR_Data_number[6];
//	x8 = IR_Data_number[7];
//	


////优先判断是否到直角或锐角  Prioritize whether to right angles or acute angles
//	 if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 0 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 0000 0111
//	{
//		err = -50;
//        delay_ms(100);
//	}
//	
//	//右锐角 写在这里测试可以生效
//	else if(x1==0 && x8 ==0 && x4 ==1 && x5 ==1)
//{
//    err = 70;  // 增加转向幅度
//    delay_ms(100);  // 统一延迟时间
//}

//	else if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 0 && x5 ==1  && x6 == 1  && x7 == 1 && x8 == 1) // 0000 1111
//	{
//		err = -50;
//        delay_ms(100);
//	}

//		else if(x1 == 0 && x2 == 0  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 00011111
//	{
//		err = -60;
//        delay_ms(100);
//	}
//    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 0 && x6 == 0  && x7 == 0 && x8 == 0) // 1110 0000
//	{
//		err = 60;
//        delay_ms(100);
//	}
//	    else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x1 == 0 && x6 == 0  && x7 == 0 && x8 == 0) // 1111 0000
//	{
//		err = 50;
//        delay_ms(100);
//	}
//		 else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 0 && x8 == 0) // 1111 1000
//	{
//		err = 50;
//        delay_ms(100);
//	}
////左右锐角弯道
////	else if( x1==0&&x2==1&&(x6 == 0 || x7 == 0))
////	{
////	err = -65;
////        delay_ms(150);
////	}
////		else if( x8==0&&x7==1&&(x2== 0 || x3== 0))
////	{
////	err = 65;
////        delay_ms(150);
////	}
////			
//	else if((x1 == 0 && x2 == 1 && x3 == 1) && (x6 == 0 || x7 == 0 || x8 == 0))
//{
//    err = -70;  // 增加转向幅度
//    delay_ms(100);  // 统一延迟时间
//}
//// 优化右锐角弯道判断

//	else if((x8 == 0 && x4 == 0 && x5 == 1)||(x8 == 0 && x7==0&& x1==0 &&x2==0&& x4 == 1 && x5 == 1)||((x8 == 0 && x7 == 1 && x6 == 1) && (x1 == 0 || x2 == 0 || x3 == 0)))
//{
//    err = 70;  // 增加转向幅度
//    delay_ms(100);  // 统一延迟时间
//}

//  else if(x1 == 0 &&  x2 == 0  && x7 == 0 && x8 == 0 ) //俩边都亮，直跑    Both sides are lit. Run straight.
//	{
//		err = 0;
//		if(trun_flag == 1)
//		{
//			trun_flag = 0;//走到圈了    Walking in circles.
//		}
//	}
//	

//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1110 1111
//	{
//		err = -1;
//	}
//	else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 0 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1100 1111
//	{
//		err = -2;
//	}
////	else if(x1 == 1 && x2 == 1  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1101 1111
////	{
////		err = -2;
////	}
//	
////	else if(x1 == 1 && x2 == 0  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1011 1111
////	{
////		err = -3;
////	}
//	else if(x1 == 1 && x2 == 0  && x3 == 0&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 1001 1111
//	{
//		err = -2;
//	}
//    
//		else if(x1 == 0 && x2 == 0  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0011 1111
//	{
//		err = -2;  
//	}
//	else if(x1 == 0 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 1) // 0111 1111
//	{
//		err = -50; 
//	}

//	
//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 1  && x7 == 1 && x8 == 1) // 1111 0111
//	{
//		err = 1;
//	} 
//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 0 && x6 == 0  && x7 == 1 && x8 == 1) // 1111 0011
//	{
//		err = 2;
//	}
////	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 1 && x8 == 1) // 1111 1011
////	{
////		err = 2;
////	}
//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 0  && x7 == 0 && x8 == 1) // 1111 1001
//	{
//		err = 3;
//	}
//	
////	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 0 && x8 == 1) // 1111 1101
////	{
////		err = 3;
////	}
//	else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 0 && x8 == 0) // 1111 1100
//	{
//		err = 2; 
//	}
//		else if(x1 == 1 && x2 == 1  && x3 == 1&& x4 == 1 && x5 == 1 && x6 == 1  && x7 == 1 && x8 == 0) // 1111 1110
//	{
//		err = 50;
//	}
//	

//	
// 
//	else if(x1 == 1 &&x2 == 1 &&x3 == 1 && x4 == 0 && x5 == 0 && x6 == 1 && x7 == 1&& x8 == 1) //直走 go straight
//	{
//		err = 0;
//	}
//    
//	//剩下的就保持上一个状态	    The rest will stay the same.
//    
//    
//	pid_output_IRR = (int)(PID_IR_Calc(err));

//	Motion_Car_Control(IRR_SPEED, 0, pid_output_IRR);	

//}

