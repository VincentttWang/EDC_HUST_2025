#include "app_motor.h"

static float speed_lr = 0;
static float speed_fb = 0;
static float speed_spin = 0;
static int speed_L1_setup = 0;
static int speed_L2_setup = 0;
static int speed_R1_setup = 0;
static int speed_R2_setup = 0;

// ���ص�ǰС����������͵�һ��  Returns half of the sum of the axle spacings of the current trolley wheels.
static float Motion_Get_APB(void)
{
    return Car_APB;
}

void Set_Motor(int MOTOR_TYPE)
{
    if(MOTOR_TYPE == 1)
    {
        send_motor_type(1);//���õ������	Configure motor type
        delay_ms(100);
        send_pulse_phase(30);//���ü��ٱ� �����ֲ�ó�	Configure the reduction ratio. Check the motor manual to find out
        delay_ms(100);
        send_pulse_line(11);//���ôŻ��� �����ֲ�ó�	Configure the magnetic ring wire. Check the motor manual to get the result.
        delay_ms(100);
        send_wheel_diameter(67.00);//��������ֱ��,�����ó�		Configure the wheel diameter and measure it
        delay_ms(100);
        send_motor_deadzone(1900);//���õ������,ʵ��ó�	Configure the motor dead zone, and the experiment shows
        delay_ms(100);
    }
    
    else if(MOTOR_TYPE == 2)
    {
        send_motor_type(2);
        delay_ms(100);
        send_pulse_phase(20);
        delay_ms(100);
        send_pulse_line(13);
        delay_ms(100);
        send_wheel_diameter(48.00);
        delay_ms(100);
        send_motor_deadzone(1600);
        delay_ms(100);
    }
    
    else if(MOTOR_TYPE == 3)
    {
        send_motor_type(3);
        delay_ms(100);
        send_pulse_phase(45);
        delay_ms(100);
        send_pulse_line(13);
        delay_ms(100);
        send_wheel_diameter(68.00);
        delay_ms(100);
        send_motor_deadzone(1600);
        delay_ms(100);
    }
    
    else if(MOTOR_TYPE == 4)
    {
        send_motor_type(4);
        delay_ms(100);
        send_pulse_phase(48);
        delay_ms(100);
        send_motor_deadzone(1000);
        delay_ms(100);
    }
    
    else if(MOTOR_TYPE == 5)
    {
        send_motor_type(1);
        delay_ms(100);
        send_pulse_phase(40);
        delay_ms(100);
        send_pulse_line(11);
        delay_ms(100);
        send_wheel_diameter(67.00);
        delay_ms(100);
        send_motor_deadzone(1900);
        delay_ms(100);
    }
}

//ֱ�ӿ���pwm

void Motion_Car_Control(int16_t V_x, int16_t V_y, int16_t V_z)             
{	
	float robot_APB = Motion_Get_APB();
    speed_fb = V_x;
    speed_spin = (V_z / 1000.0f) * robot_APB; //*188
    if (V_x == 0 && V_y == 0 && V_z == 0)
    {
        Contrl_Speed(0,0,0,0);
        return;
    }

	//speed_L1_setup = speed_fb + speed_spin;
    speed_L2_setup = speed_fb + speed_spin;
    //speed_R1_setup = speed_fb  - speed_spin;
    speed_R2_setup = speed_fb  - speed_spin;
		
//	if (speed_L1_setup > 1000) speed_L1_setup = 1000;
//	if (speed_L1_setup < -1000) speed_L1_setup = -1000;
	if (speed_L2_setup > 1000) speed_L2_setup = 1000;
	if (speed_L2_setup < -1000) speed_L2_setup = -1000;
//	if (speed_R1_setup > 1000) speed_R1_setup = 1000;
//	if (speed_R1_setup < -1000) speed_R1_setup = -1000;
	if (speed_R2_setup > 1000) speed_R2_setup = 1000;
	if (speed_R2_setup < -1000) speed_R2_setup = -1000;
		
	Contrl_Speed(0, speed_L2_setup, 0, speed_R2_setup);
		
}