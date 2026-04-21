#include "bsp_IR_i2c.h"
extern I2C_HandleTypeDef hi2c2;


//进入校准
void set_adjust_mode(u8 mode)
{
	HAL_I2C_Mem_Write(&hi2c2, (0x12<<1), 0x01, I2C_MEMADD_SIZE_8BIT, &mode, 1, 10);//1:进入校准  0:退出校准
}
	


//读取红外数据
static void read_IRdata(u8 *buf)
{
	HAL_I2C_Mem_Read(&hi2c2, (0x12<<1), 0x30, I2C_MEMADD_SIZE_8BIT, buf, 1, 10);
}
void deal_IRdata(u8 *x1,u8 *x2,u8 *x3,u8 *x4,u8 *x5,u8 *x6,u8 *x7,u8 *x8)
{
	u8 IRbuf = 0xFF;
	read_IRdata(&IRbuf);
	
	*x1 = (IRbuf>>7)&0x01;
	*x2 = (IRbuf>>6)&0x01;
	*x3 = (IRbuf>>5)&0x01;
	*x4 = (IRbuf>>4)&0x01;
	*x5 = (IRbuf>>3)&0x01;
	*x6 = (IRbuf>>2)&0x01;
	*x7 = (IRbuf>>1)&0x01;
	*x8 = (IRbuf>>0)&0x01;
}


