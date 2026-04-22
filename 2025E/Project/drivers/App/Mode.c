#include "Mode.h"
#include "AllHeader.h"

extern char CircleNum; // Variable to hold the current circle number
//Test function for the mode system


int isturn = 0; // Variable to track if the robot is turning
bool turning = false;

void mode_test_distance(void) // 测试距离
{
    while(1)
		{
			Motor_SetLeft(-200);
			Motor_SetRight(200);
		}
		
}

void mode_test_coordinate(void) // 测试坐标与转向
{
	YP_SMotor_Init();
//    // Implement the functionality for test_Cordi here
	while(1)
	{
	YP_SMotor_SetSpeed(180,0);
		Motor_SetLeft(300);Motor_SetRight(300);
		Delay_ms(500);
				Motor_SetLeft(-300);Motor_SetRight(-300);
			YP_SMotor_SetSpeed(-180,0);
		Delay_ms(500);
	}
}

void mode_test_circle(void) // 圆周运动测试
{
	while(1)
	{
		UpdateSInedge();
		TrackingSensor_Read(Digital);
        if(!mode_turn_step())
		{
			Motor_Brake();
			break;
		}
		Delay_ms(10);
		
	}
}

void mode_test_connection(void)
{
    char message[50];
    while(1)
    {
		Coordinate cor = {0.0f, 0.0f};
        Coordinate paper = {0.1, 0.1}; // Get the center coordinates of the paper
        cor = paper_to_camera(paper);
        sprintf(message, "Camera: (%.2f, %.2f)", cor.x, cor.y); // Format the message
        OLED_ShowString(0, 0, message, 8);
    }
}

void mode_test_tracking(void)
{
    // Test the track function with a linear velocity of 0.3
    while(1)
    {
		TrackingSensor_Read(Digital);
        lineWalking_low();
        if(empty_Detect()) // Check if the empty detection condition is met
        {
            Motor_Brake(); // Motor_Brake the loop if the condition is met
            return; // Exit the function
        }
    }
}

void mode_problem_b_1(void) // 题目B第一问
{
    int cn = mode_set_circle_num(CircleNum);
    #ifdef MODE_DEBUG
    char debug_message[50];
    sprintf(debug_message, "CircleNum: %d", cn);
    OLED_ShowString(0, 0, debug_message, 8); // Display the circle number on the OLED
    #endif
    while(1)
    {
		TrackingSensor_Read(Digital);
			UpdateSInedge();
        if(half_Detect() && (cn * 4 == edge - 1))
        {
            Motor_Brake(); // Motor_Brake the loop if the condition is met
            return; // Exit the function
        }
        if(!mode_turn_step()) // Check if the robot is turning
        {
            lineWalking_low(); // Call the track function with a linear velocity of 0.3
        }
		Delay_ms(10);
    }
}

void mode_problem_b_2_3(void)
{
    #ifdef MODE_DEBUG
    OLED_ShowString(0, 0, "ProB2/3", 8); // Display the mode name on the OLED
    #endif
	YP_SMotor_Init();
	while(1){
		SetLaserPosition(); // Set the laser position based on the current mode
    SetTargetCenter(); // Set the target center for the robot
    if(mode_init_guard())
    {
        PID_SMotor_Cont(); // Call the PID control function for the motor
    }
		 Delay_ms(10); // Delay for 10 milliseconds
	}
}

void mode_problem_h_1(void)
{
    #ifdef MODE_DEBUG
    OLED_ShowString(0, 0, "ProH1", 8); // Display the mode name on the OLED
    #endif
//    int cn = mode_set_circle_num(CircleNum);
        int cn = mode_set_circle_num(1);
	
    YP_SMotor_Init();
    while(1)
    {
		TrackingSensor_Read(Digital);
        UpdateSInedge(); // Update the sInedge variable with the current speed

		if(half_Detect() && (cn * 4 == edge)) // Check if the half detection condition is met
        {
            Motor_Brake(); // Motor_Brake the loop if the condition is met
            return; // Exit the function
        }
        if(!mode_turn_step()) // Check if the robot is turning
        {
					  DL_GPIO_setPins(LED_PORT, LED_LED0_PIN);
            lineWalking_low(); // Call the track function with a linear velocity of 0.3
        }
				else{				DL_GPIO_clearPins(LED_PORT, LED_LED0_PIN);}
        SetLaserPosition(); // Set the laser position based on the current mode
        SetTargetCenter(); // Set the target center for the robot
        Compute_excur();
        PID_SMotor_Cont(); // Call the PID control function for the motor
        Delay_ms(50); // Delay for 10 milliseconds   
        // Implement the functionality for ProH1 here
    }
}

void mode_problem_h_2(void)
{
    #ifdef MODE_DEBUG
    OLED_ShowString(0, 0, "ProH2", 8); // Display the mode name on the OLED
    #endif
    YP_SMotor_Init();
    while(1)
    {
        TrackingSensor_Read(Digital);
        UpdateSInedge(); // Update the sInedge variable with the current speed

        if(half_Detect() && (4 == edge)) // Check if the half detection condition is met
        {
            Motor_Brake(); // Motor_Brake the loop if the condition is met
						DL_GPIO_clearPins(SMotor_IO_PORT, SMotor_IO_EN1_PIN);
						DL_GPIO_clearPins(SMotor_IO_PORT, SMotor_IO_EN2_PIN);
            return; // Exit the function
        }
        if(!mode_turn_step()) // Check if the robot is turning
        {
            lineWalking_low(); // Call the track function with a linear velocity of 0.3
        }
        SetLaserPosition(); // Set the laser position for the robot
        SetTargetCircle(); // Set the target circle for the robot
        Compute_excur();
        PID_SMotor_Cont(); // Call the PID control function for the motor
        Delay_ms(10); // Delay for 10 milliseconds   
        // Implement the functionality for ProH1 here
    }

}

int mode_set_circle_num(char num)
{
    // Convert the character to an integer
    if (num >= '0' && num <= '9') {
        return num - '0'; // Convert character to integer
    } else {
        sprintf(error_message, "Invalid CircleNum input: %c\n", num);
        error_handler();
        return -1; // Invalid input
    }
}

bool mode_turn_step(void)// 转弯步进控制
{
    static float nowSInedge = 0; // Variable to track the current sInedge value
    if(half_Detect() && isturn == 0) // Check if the half detection condition is met
    {
        isturn = 1; // Set the isturn flag to indicate that the robot is turning
        nowSInedge = sInedge; // Store the current sInedge value
    }
    if(isturn == 1) // Check if the robot is turning
    {
			float first_dis = DisSensorToWheel * 1e-3 + nowSInedge  - 0.06;
			float second_dis = first_dis+ DEG_TO_RAD(90) * WHEEL_DIS * 1e-3 * 0.8;
        if(sInedge < first_dis) // 距离未达阈值，直行前进
        {
            Motor_SetLeft(200); // 直行时设置左电机速度
            Motor_SetRight(200); // Set the right motor speed to 300
            return true; // Return true to indicate that the robot is turning
        }
        else if(sInedge >= first_dis &&
             sInedge < second_dis ) // 达到转弯区间
        {
						turning = true;
            Motor_SetLeft(-120); // 反转左电机，执行转弯
            Motor_SetRight(120); // Set the right motor speed to 300
            return true; // Return true to indicate that the robot is turning
        }
				
				turning = false;
        sInedge = 0; // Reset the sInedge variable after the turn
        isturn = 0;
        edge++; // Increment the edge variable after the turn
    }
		return false;
		
}

bool mode_init_guard(void)
{
    if(Laser_error == CANMV_ERR_NOT_FOUND)
    {
        YP_SMotor_SetSpeed(-90, 0); // Set the speed of the motors to -120
        return false; // Return false to indicate initialization failure
    }
    return true; // Return true to indicate successful initialization
}