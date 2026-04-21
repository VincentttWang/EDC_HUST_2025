'''
实验名称：RTC实时时钟
说明：实时时钟使用
教程：wiki.01studio.cc
'''

# 导入相关模块
from machine import RTC
import time

# 构建RTC对象
rtc = RTC()

# 首次上电设置RTC日期和时间。(2024, 1, 1, 0, 0, 0, 0, 0)按顺序分别表示（年，月，日，星期，时，分，秒，微妙），
# 其中星期使用0-6表示星期一到星期日。
if rtc.datetime()[0] != 2024:
    rtc.datetime((2024, 1, 1, 0, 0, 0, 0, 0))

while True:

    print(rtc.datetime()) #打印时间

    time.sleep(1) #延时1秒
