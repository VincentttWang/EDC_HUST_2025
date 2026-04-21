'''
实验名称：pyMotors多功能电机模块
版本：v1.0
日期：2025-3-21
作者：01Studio
运行平台：01Studio CanMV K230
说明：同时控制16路舵机旋转
'''
from machine import I2C,FPIOA
from servo import Servos
import time

#将GPIO11,12配置为I2C2功能
fpioa = FPIOA()
fpioa.set_function(11, FPIOA.IIC2_SCL)
fpioa.set_function(12, FPIOA.IIC2_SDA)

i2c = I2C(2) #构建I2C对象
#print(i2c.scan())

#构建16路舵机对象
s=Servos(i2c)

#舵机对象使用用法, 详情参看servo.py文件
#
#s.position(index, degrees=None)
#index: 0~15表示16路舵机;
#degrees: 角度，0~180。

while True:

    for i in range(16):
        s.position(i,0) #i为0~15的16路输出，0表示角度0度。
    time.sleep(1)

    for i in range(16):
        s.position(i,180) #i为0~15的16路输出，180表示角度180度。
    time.sleep(1)
