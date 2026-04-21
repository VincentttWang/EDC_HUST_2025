'''
实验名称：点亮LED蓝灯
版本：v1.0
作者：01Studio
实验平台：01Studio CanMV K230
教程：wiki.01studio.cc
'''

from machine import Pin #导入Pin模块
from machine import FPIOA
import time

#将GPIO52配置为普通GPIO
fpioa = FPIOA()
fpioa.set_function(52,FPIOA.GPIO52)

LED=Pin(52,Pin.OUT) #构建led对象，GPIO52,输出
LED.value(1) #点亮LED，也可以使用led.on()
