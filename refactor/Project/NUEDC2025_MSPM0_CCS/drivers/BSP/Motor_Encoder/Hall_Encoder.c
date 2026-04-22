#include "Hall_Encoder.h"

static ENCODER_RES motor_encoder;

//编码器初始化
void encoder_init(void)
{
	//编码器引脚外部中断
	NVIC_ClearPendingIRQ(GPIOA_INT_IRQn);
	NVIC_EnableIRQ(GPIOA_INT_IRQn);
}

//获取编码器的值
int get_encoder_count(void)
{
	return motor_encoder.count;
}
//获取编码器的方向
ENCODER_DIR get_encoder_dir(void)
{
	return motor_encoder.dir;
}

//编码器数据更新
//请间隔一定时间更新
void encoder_update(void)
{
	motor_encoder.count = motor_encoder.temp_count;
	//确定方向
	motor_encoder.dir = ( motor_encoder.count >= 0 ) ? D : R;

	motor_encoder.temp_count = 0;//编码器计数值清零
}

//外部中断处理函数
void GROUP1_IRQHandler(void)
{
	uint32_t gpio_status;

	//获取中断信号情况
	gpio_status = DL_GPIO_getEnabledInterruptStatus(Motor_IO_E1A_PORT, Motor_IO_E1A_PIN | Motor_IO_E2A_PIN);
	//编码器A相上升沿触发
	if((gpio_status & Motor_IO_E1A_PIN) == Motor_IO_E1A_PIN)
	{
		//如果在A相上升沿下，B相为低电平
		if(!DL_GPIO_readPins(Motor_IO_E1A_PORT,Motor_IO_E2A_PIN))
		{
			motor_encoder.temp_count--;
		}
		else
		{
			motor_encoder.temp_count++;
		}
	}//编码器B相上升沿触发
	else if((gpio_status & Motor_IO_E2A_PIN)==Motor_IO_E2A_PIN)
	{
		//如果在B相上升沿下，A相为低电平
		if(!DL_GPIO_readPins(Motor_IO_E1A_PORT,Motor_IO_E1A_PIN))
		{
			motor_encoder.temp_count++;
		}
		else
		{
			motor_encoder.temp_count--;
		}
	}
	//清除状态
	DL_GPIO_clearInterruptStatus(Motor_IO_E1A_PORT,Motor_IO_E1A_PIN|Motor_IO_E2A_PIN);
}

void timer_init(void)
{
    //定时器中断
	NVIC_ClearPendingIRQ(TIMER_0_INST_INT_IRQN);
	NVIC_EnableIRQ(TIMER_0_INST_INT_IRQN);
}

//电机编码器脉冲计数
void TIMER_0_INST_IRQHandler(void)
{
	//20ms归零中断触发
	if( DL_TimerA_getPendingInterrupt(TIMER_0_INST) == DL_TIMER_IIDX_ZERO )
	{
		//编码器更新
		encoder_update();


	}
}

double getSpeed(void)//编码器，第一步
{
		static int speed = 255;
		static int dir = 255;
		char* disp_buff[50]={0};
		
		speed = get_encoder_count();
		dir = (get_encoder_dir() == D)? 1: -1;
		
		return speed * 0.0561 * dir * 0.108 * 1.05;
		//0.01440    speed * 0.0561 * dir * 0.108 * 1.05;
		// Left number is computed by (length of the circle) * (frequence of the clock / (leftSpeed add number for every single circle
		// which is (6.6e-2 * pi * 100 / (360 * 2))
}
