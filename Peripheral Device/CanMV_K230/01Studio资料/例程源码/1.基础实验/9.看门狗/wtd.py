'''
实验名称：看门狗
版本： v1.0
作者：01Studio
实验平台：01Studio CanMV K230
说明：看门狗测试。
'''

from machine import WDT #导入线程模块
import time

#构建看门狗对象。
wdt = WDT(1,3) #看门狗编号1，超时时间3秒。


#每隔1秒喂一次狗，执行3次。
for i in range(3):

    time.sleep(1)
    print(i)

    wdt.feed() #喂狗

#停止喂狗，系统会重启。
while True:

    time.sleep(0.01) #防止CPU满跑
