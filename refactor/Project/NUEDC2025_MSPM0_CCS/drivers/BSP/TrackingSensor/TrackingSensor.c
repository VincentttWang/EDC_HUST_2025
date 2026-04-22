#include "TrackingSensor.h"
#include "AllHeader.h"

/**
  * @brief  ��ȡ��·ѭ�������������ݲ���䵽�����С�
  * @param  digitalArray: ָ��һ��8��Ԫ�ص�uint8_t�����ָ�룬���ڴ洢���������ݡ�
  * �����е�ÿ��Ԫ�ؽ��洢��Ӧ��������������״̬��0��1����
  * @retval ��
  */
void getTrackingSensorData(uint8_t digitalArray[8])
{
	// 判断每个引脚的读取结果是否等于0
	// 如果是低电平 (函数返回0), "== 0" 结果为 true (1)
	// 如果是高电平 (函数返回非0), "== 0" 结果为 false (0)
	digitalArray[7] = (DL_GPIO_readPins(Tracking_Tracking_1_PORT, Tracking_Tracking_1_PIN) == 0);
	digitalArray[6] = (DL_GPIO_readPins(Tracking_Tracking_2_PORT, Tracking_Tracking_2_PIN) == 0);
	digitalArray[5] = (DL_GPIO_readPins(Tracking_Tracking_3_PORT, Tracking_Tracking_3_PIN) == 0);
	digitalArray[4] = (DL_GPIO_readPins(Tracking_Tracking_4_PORT, Tracking_Tracking_4_PIN) == 0);
	digitalArray[3] = (DL_GPIO_readPins(Tracking_Tracking_5_PORT, Tracking_Tracking_5_PIN) == 0);
	digitalArray[2] = (DL_GPIO_readPins(Tracking_Tracking_6_PORT, Tracking_Tracking_6_PIN) == 0);
	digitalArray[1] = (DL_GPIO_readPins(Tracking_Tracking_7_PORT, Tracking_Tracking_7_PIN) == 0);
	digitalArray[0] = (DL_GPIO_readPins(Tracking_Tracking_8_PORT, Tracking_Tracking_8_PIN) == 0);
}