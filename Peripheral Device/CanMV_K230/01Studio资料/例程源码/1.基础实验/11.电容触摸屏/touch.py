'''
实验名称：电容触摸屏
实验平台：01Studio CanMV K230
教程：wiki.01studio.cc
'''

from machine import TOUCH
import time

# 实例化TOUCH设备0
tp = TOUCH(0)

while True:

    # 获取TOUCH数据
    p = tp.read()

    if p != (): #发生触摸事件

        print(p) #打印原始触摸数据

        #打印每个点坐标信息，最大5点。
        for i in range(len(p)):

            print('x'+str(i)+'=',p[i].x, 'y'+str(i)+'=',p[i].y)

    time.sleep_ms(50)
