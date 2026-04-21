'''
实验名称：定时器
版本：v1.0
作者：01Studio
实验平台：01Studio CanMV K230
说明：通过定时器让LED周期性每秒闪烁1次。
'''

from machine import Pin,Timer,FPIOA
import time


#将GPIO52配置为普通GPIO
fpioa = FPIOA()
fpioa.set_function(52,FPIOA.GPIO52)

led=Pin(52,Pin.OUT) #构建led对象

Counter = 0
Fun_Num = 0

def fun(tim):

    global Counter
    Counter = Counter + 1
    print(Counter)
    led.value(Counter%2)

#使用软件定时器，编号-1
tim = Timer(-1)
tim.init(period=1000, mode=Timer.PERIODIC,callback=fun) #周期为1000ms

while True:

    time.sleep(0.01) #避免CPU满跑
