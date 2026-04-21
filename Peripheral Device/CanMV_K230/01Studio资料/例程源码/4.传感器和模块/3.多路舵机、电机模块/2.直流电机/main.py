'''
# Copyright (c) [2025] [01Studio]. Licensed under the MIT License.

实验名称：pyMotors多功能电机模块
实验平台：01Studio CanMV K230
说明：同时控制4路直流减速电机
'''

from machine import I2C,FPIOA
from motor import Motors
import time

#将GPIO11,12配置为I2C2功能
fpioa = FPIOA()
fpioa.set_function(11, FPIOA.IIC2_SCL)
fpioa.set_function(12, FPIOA.IIC2_SDA)

i2c = I2C(2) #构建I2C对象
#print(i2c.scan())

#构建4路直流电机对象
m=Motors(i2c)

#直流电机对象使用用法，详情参看motor.py文件
#
#m.speed(index, value=0)
#index: 0~3表示4路直流电机
#value: 速度。-4095~4095，正负表示转向，绝对值越大速度越大


#前进
m.speed(0,4095) #直流电机0正转，速度0~4095,4095表示最快速度
m.speed(1,4095)
m.speed(2,4095)
m.speed(3,4095)

time.sleep(3)

#后退
m.speed(0,-4095) #直流电机0反转，速度0~-4095,-4095表示反转最快速度
m.speed(1,-4095)
m.speed(2,-4095)
m.speed(3,-4095)

time.sleep(3)

#制动停止
for i in range(4):
    m.brake(i)
