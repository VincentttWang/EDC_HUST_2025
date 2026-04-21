/*
 * 编译测试文件 - 验证重构后的霍尔编码器驱动是否能正确编译
 */

#include "Hall_Encoder.h"

// 模拟的配置定义（实际应该在ti_msp_dl_config.h中）
#ifndef QEI_0_INST
#define QEI_0_INST ((GPTIMER_Regs*)0x40086000UL)  // TIMG8基地址
#endif

#ifndef TIMG1
#define TIMG1 ((GPTIMER_Regs*)0x40084000UL)       // TIMG1基地址  
#endif

#ifndef CPUCLK_FREQ
#define CPUCLK_FREQ 32000000                      // 32MHz
#endif

// 测试初始化
void test_encoder_init(void)
{
    // 测试单个编码器初始化
    EncoderInit(QEI_0_INST, TIMG1, 66, 360);
    
    // 测试编码器参数初始化
    EncoderParam param;
    EncoderParamInit(&param, 66, 360);
}

// 测试速度获取
void test_speed_functions(void)
{
    // 模拟更新速度
    UpdateAllSpeed(TIMG1);
    
    // 获取速度和距离
    double speed = getSpeed(0);
    double distance = getDis(0);
    
    // 测试左右轮函数
    double center = cSpeed();
    double left = lSpeed();
    double right = rSpeed();
    double ldist = lDis();
    double rdist = rDis();
    
    // 避免未使用变量警告
    (void)speed;
    (void)distance;
    (void)center;
    (void)left;
    (void)right;
    (void)ldist;
    (void)rdist;
}

int main(void)
{
    test_encoder_init();
    test_speed_functions();
    return 0;
}
