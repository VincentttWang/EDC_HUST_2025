'''
实验名称：继电器
实验平台：01Studio CanMV K230
说明：通过按键改变继电器通断状态
教程：wiki.01studio.cc
'''

from machine import Pin
from machine import FPIOA
import time


#将GPIO4、GPIO21配置为普通GPIO模式
fpioa = FPIOA()
fpioa.set_function(4,FPIOA.GPIO2)
fpioa.set_function(21,FPIOA.GPIO21)

RELAY=Pin(4,Pin.OUT) #构建继电器对象
KEY=Pin(21,Pin.IN,Pin.PULL_UP) #构建KEY对象

state=0 #继电器通断状态

while True:

    if KEY.value()==0:   #按键被按下
        time.sleep_ms(10) #消除抖动
        if KEY.value()==0: #确认按键被按下

            state=not state  #使用not语句而非~语句
            RELAY.value(state) #继电器通断状态翻转
            print('KEY')

            while not KEY.value(): #检测按键是否松开
                pass
