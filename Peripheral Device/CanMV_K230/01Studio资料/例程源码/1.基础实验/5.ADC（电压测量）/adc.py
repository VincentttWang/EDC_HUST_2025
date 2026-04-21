'''
实验名称：ADC（电压测量）
版本：v1.0
作者：01Studio
实验平台：01Studio CanMV K230
说明：ADC共4个通道，其中通道0、1实际量程为0-3.6V，通道2、3量程为0-1.8V。
    （请勿超出测量量程, 可能导致主控芯片烧坏！）
'''

from machine import ADC
import time

'''
构建ADC对象:
ADC0（排针32引脚，量程0-3.6V）, ADC1（排针36引脚，量程0-3.6V）,
ADC2（排针38引脚，量程0-1.8V）, ADC3（排针40引脚，量程0-1.8V）。
'''
adc = ADC(0) #通道0

while True:

    print(adc.read_u16()) # 获取ADC通道采样值

    # 获取ADC通道电压值，保留2为小数。通道0、1实际量程为0-3.6V，返回值x2。
    print('%.2f'%(adc.read_uv()/1000000*2), "V")

    time.sleep(1) #延时1秒
