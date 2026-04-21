#include "ti_msp_dl_config.h"
#include "delay.h"
#include "usart.h"
#include "bsp_motor_usart.h"
#include "app_irtracking.h"
#include "app_motor.h"
                    
#define MOTOR_TYPE 2   //1:520电机 2:310电机 3:测速码盘TT电机 4:TT直流减速电机 5:L型520电机
                       //1:520 motor 2:310 motor 3:speed code disc TT motor 4:TT DC reduction motor 5:L type 520 motor

int main(void)
{
    USART_Init();//打印串口初始化 Print serial port initialization
    printf("please wait...\r\n");

	 //使能DMA通道  Enable DMA Channel
    NVIC_ClearPendingIRQ(UART_1_INST_INT_IRQN);
    DL_DMA_enableChannel(DMA, DMA_CH0_CHAN_ID);
    NVIC_EnableIRQ(UART_1_INST_INT_IRQN);
    
//    /*八路巡线模块初始化 Initialization of the eight-way patrol module */
//    IRI2C_WriteByte(0x01,1);//控制进入校准    Control Access Calibration
//    delay_ms(200);	
//    IRI2C_WriteByte(0x01,0);//控制退出校准    Control exit calibration
//    delay_ms(200);
    
    //设置电机类型    Set motor type
    Set_Motor(MOTOR_TYPE);
    
    //修改电机PID，这里的参数是为四驱310底盘配置的，其他底盘需要自己测试修改
    //Modify the motor PID, the parameters here are configured for the 4WD 310 chassis, other chassis need to test and modify their own!
	send_motor_PID(1.9,0.2,0.8);

    printf("Initialization Succeed\r\n");
    
	while(1)
	{
        LineWalking();//开始八路巡线  Starting eight-way patrols.
    }
	
}

