//巡线要想在高难度巡线地图上运行，则需要修改电机的PID才能达到更好的巡线效果
//这个工程是使用四驱310底盘来调的效果，这里使用的电机PID为：P:1.9,I:0.2,D:0.8
//IIC驱动四路电机模块无法更改PID值，因此需要使用电脑串口助手使用串口的命令去修改PID值
//！其余底盘使用这个电机PID和巡线PID，效果可能没有四驱310的好，需要自行去调节！

//Patrol to run on difficult patrol maps, it is necessary to modify the PID of the motor in order to achieve better patrol results
//This project is the use of four-wheel drive 310 chassis to adjust the effect of the motor PID used here: P: 1.9, I: 0.2, D: 0.8
//IIC drive four-way motor module can not change the PID value, so you need to use the computer serial port assistant to use the serial port command to modify the PID value
//! The rest of the chassis use this motor PID and patrol PID, the effect may not be as good as the 4WD 310, you need to adjust yourself!

#include "AllHeader.h"

#define MOTOR_TYPE 2   //1:520电机 2:310电机 3:测速码盘TT电机 4:TT直流减速电机 5:L型520电机
                       //1:520 motor 2:310 motor 3:speed code disc TT motor 4:TT DC reduction motor 5:L type 520 motor

int main(void)
{		
	
	//硬件初始化 Hardware Initialization
	BSP_init();
    
    Set_Motor(MOTOR_TYPE);//设置电机参数  Setting motor parameters
    
	send_control_data(0,0,1); //设置只接收数值型数据  Set to receive only numeric data
	
	while(1)
	{
		LineWalking(); //开始八路巡线  Starting eight-way patrols.
	}
}


