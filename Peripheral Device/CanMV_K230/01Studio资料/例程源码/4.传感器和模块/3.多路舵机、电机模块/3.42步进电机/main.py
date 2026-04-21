'''
# Copyright (c) [2025] [01Studio]. Licensed under the MIT License.

实验名称：pyMotors多功能电机模块
实验平台：01Studio CanMV K230
说明：同时控制2路42步进电机
'''

from machine import I2C,FPIOA
from stepper import Steppers
import time

#将GPIO11,12配置为I2C2功能
fpioa = FPIOA()
fpioa.set_function(11, FPIOA.IIC2_SCL)
fpioa.set_function(12, FPIOA.IIC2_SDA)

i2c = I2C(2) #构建I2C对象
#print(i2c.scan())

#构建2路42步进电机对象
s=Steppers(i2c)

#42步进电机对象使用用法，详情参看stepper.py文件
#
#s.Step(index, steps=0, direction=1,mode=4, interval=2)
#index: 0~1表示2路42步进电机
#steps: 转动步数
#direction：方向,1为正转，0为反转
#mode: 4表示4拍，8表示八拍
#interval：每拍间隔时间，单位为ms

#2路顺时针连续旋转
while True:
    s.Step(0,1)
    s.Step(1,1)
