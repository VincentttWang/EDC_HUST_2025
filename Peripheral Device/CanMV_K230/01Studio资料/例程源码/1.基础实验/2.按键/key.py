'''
实验名称：按键
版本：v1.0
作者：01Studio
实验平台：01Studio CanMV K230
说明：通过按键改变LED的亮灭状态
'''

from machine import Pin
from machine import FPIOA
import time


#将GPIO52、GPIO21配置为普通GPIO模式
fpioa = FPIOA()
fpioa.set_function(52,FPIOA.GPIO52)
fpioa.set_function(21,FPIOA.GPIO21)

LED=Pin(52,Pin.OUT) #构建LED对象,开始熄灭
KEY=Pin(21,Pin.IN,Pin.PULL_UP) #构建KEY对象

state=0 #LED引脚状态

while True:

    if KEY.value()==0:   #按键被按下
        time.sleep_ms(10) #消除抖动
        if KEY.value()==0: #确认按键被按下

            state=not state  #使用not语句而非~语句
            LED.value(state) #LED状态翻转
            print('KEY')

            while not KEY.value(): #检测按键是否松开
                pass
