'''
实验名称：二维码识别
实验平台：01Studio CanMV K230
说明：编程实现摄像头识别二维码
教程：wiki.01studio.cc
'''

import time, math, os, gc

from media.sensor import * #导入sensor模块，使用摄像头相关接口
from media.display import * #导入display模块，使用display相关接口
from media.media import * #导入media模块，使用meida相关接口

sensor = Sensor() #构建摄像头对象
sensor.reset() #复位和初始化摄像头
sensor.set_framesize(width=800, height=480) #设置帧大小为LCD分辨率(800x480)，默认通道0
sensor.set_pixformat(Sensor.RGB565) #设置输出图像格式，默认通道0

Display.init(Display.ST7701, to_ide=True) #同时使用3.5寸mipi屏和IDE缓冲区显示图像，800x480分辨率
#Display.init(Display.VIRT, sensor.width(), sensor.height()) #只使用IDE缓冲区显示图像

MediaManager.init() #初始化media资源管理器

sensor.run() #启动sensor

clock = time.clock()

while True:

    clock.tick()

    img = sensor.snapshot() #拍摄图片

    res = img.find_qrcodes() #寻找二维码

    if len(res) > 0: #在图片和终端显示二维码信息

        img.draw_rectangle(res[0].rect(), thickness=2)
        img.draw_string_advanced(0, 0, 30, res[0].payload(), color = (255, 255, 255))

        print(res[0].payload()) #串口终端打印

    Display.show_image(img) #显示图片

    print(clock.fps()) #打印帧率
