'''
实验名称：以太网有线连接
实验平台：01Studio CanMV K230
说明：编程实现USB转以太网卡有线连接到路由器，将IP地址等相关信息打印出来。
教程：wiki.01studio.cc
'''

import network,time
from machine import Pin

#有线以太网连接函数
def Lan_Connect():

    LAN_LED=Pin(52, Pin.OUT) #初始化联网指示灯

    lan = network.LAN()

    if lan.isconnected(): #连接成功

        #LED蓝灯点亮
        LAN_LED.value(1)

        #串口打印信息
        print('network information:', lan.ifconfig())

    else: #连接失败

        #LED闪烁3次提示
        for i in range(3):
            LAN_LED.value(1)
            time.sleep_ms(300)
            LAN_LED.value(0)
            time.sleep_ms(300)

#执行以太网连接函数
Lan_Connect()
