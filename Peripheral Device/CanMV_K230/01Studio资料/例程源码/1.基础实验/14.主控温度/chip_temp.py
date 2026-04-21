'''
实验名称：获取主控温度
实验平台：01Studio CanMV K230
教程：wiki.01studio.cc
'''

import machine

temp = machine.temperature() #获取温度值

print(temp) #打印温度值
