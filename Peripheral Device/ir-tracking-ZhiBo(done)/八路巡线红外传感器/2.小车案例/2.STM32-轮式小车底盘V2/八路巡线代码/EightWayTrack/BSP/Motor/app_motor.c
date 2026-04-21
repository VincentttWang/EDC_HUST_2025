#include "app_motor.h"

static float speed_lr = 0;
static float speed_fb = 0;
static float speed_spin = 0;
static int speed_L1_setup = 0;
static int speed_L2_setup = 0;
static int speed_R1_setup = 0;
static int speed_R2_setup = 0;

// 返回当前小车轮子轴间距和的一半  Returns half of the current sum of the wheel axle distances
static float Motion_Get_APB(void)
{
    return Car_APB;
}

void Set_Motor(int MOTOR_TYPE)
{
    if(MOTOR_TYPE == 1)
    {
        Set_motor_type(1);//配置电机类型	Configure motor type
        delay_ms(100);
        Set_Pluse_Phase(30);//配置减速比 查电机手册得出	Configure the reduction ratio. Check the motor manual to find out
        delay_ms(100);
        Set_Pluse_line(11);//配置磁环线 查电机手册得出	Configure the magnetic ring wire. Check the motor manual to get the result.
        delay_ms(100);
        Set_Wheel_dis(67.00);//配置轮子直径,测量得出		Configure the wheel diameter and measure it
        delay_ms(100);
        Set_motor_deadzone(1600);//配置电机死区,实验得出	Configure the motor dead zone, and the experiment shows
        delay_ms(100);
    }
    
    else if(MOTOR_TYPE == 2)
    {
        Set_motor_type(2);
        delay_ms(100);
        Set_Pluse_Phase(20);
        delay_ms(100);
        Set_Pluse_line(13);
        delay_ms(100);
        Set_Wheel_dis(48.00);
        delay_ms(100);
        Set_motor_deadzone(1900);
        delay_ms(100);
    }
    
    else if(MOTOR_TYPE == 3)
    {
        Set_motor_type(3);
        delay_ms(100);
        Set_Pluse_Phase(45);
        delay_ms(100);
        Set_Pluse_line(13);
        delay_ms(100);
        Set_Wheel_dis(68.00);
        delay_ms(100);
        Set_motor_deadzone(1250);
        delay_ms(100);
    }
    
    else if(MOTOR_TYPE == 4)
    {
        Set_motor_type(4);
        delay_ms(100);
        Set_Pluse_Phase(48);
        delay_ms(100);
        Set_motor_deadzone(1000);
        delay_ms(100);
    }
    
    else if(MOTOR_TYPE == 5)
    {
        Set_motor_type(1);
        delay_ms(100);
        Set_Pluse_Phase(40);
        delay_ms(100);
        Set_Pluse_line(11);
        delay_ms(100);
        Set_Wheel_dis(67.00);
        delay_ms(100);
        Set_motor_deadzone(1600);
        delay_ms(100);
    }
}

//直接控制pwm   Directly control pwm
void Motion_Car_Control(int16_t V_x, int16_t V_y, int16_t V_z)
{
	float robot_APB = Motion_Get_APB();
	speed_lr = 0;
    speed_fb = V_x;
    speed_spin = (V_z / 1000.0f) * robot_APB;
    if (V_x == 0 && V_y == 0 && V_z == 0)
    {
        control_speed(0,0,0,0);
        return;
    }

    speed_L1_setup = speed_fb + speed_spin;
    speed_L2_setup = speed_fb + speed_spin;
    speed_R1_setup = speed_fb  - speed_spin;
    speed_R2_setup = speed_fb  - speed_spin;
		
    if (speed_L1_setup > 1000) speed_L1_setup = 1000;
    if (speed_L1_setup < -1000) speed_L1_setup = -1000;
    if (speed_L2_setup > 1000) speed_L2_setup = 1000;
    if (speed_L2_setup < -1000) speed_L2_setup = -1000;
    if (speed_R1_setup > 1000) speed_R1_setup = 1000;
    if (speed_R1_setup < -1000) speed_R1_setup = -1000;
    if (speed_R2_setup > 1000) speed_R2_setup = 1000;
    if (speed_R2_setup < -1000) speed_R2_setup = -1000;
    
    //printf("%d\t,%d\t,%d\t,%d\r\n",speed_L1_setup,speed_L2_setup,speed_R1_setup,speed_R2_setup);
    
    control_speed(speed_L1_setup, speed_L2_setup, speed_R1_setup, speed_R2_setup);
		
}


