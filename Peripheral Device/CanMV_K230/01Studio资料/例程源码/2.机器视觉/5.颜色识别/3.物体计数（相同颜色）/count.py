'''
实验名称：物体计数
实验平台：01Studio CanMV K230
说明：物体计数（相同颜色物体）
教程：wiki.01studio.cc
'''

import time, os, sys

from media.sensor import * #导入sensor模块，使用摄像头相关接口
from media.display import * #导入display模块，使用display相关接口
from media.media import * #导入media模块，使用meida相关接口

thresholds = [(18, 72, -13, 31, 18, 83)] #黄色跳线帽阈值

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

    ################
    ## 这里编写代码 ##
    ################
    clock.tick()

    img = sensor.snapshot()
    blobs = img.find_blobs([thresholds[0]])

    if blobs: #画框显示
        for b in blobs:
            tmp=img.draw_rectangle(b[0:4])
            tmp=img.draw_cross(b[5], b[6])

    #显示计算信息
    img.draw_string_advanced(0, 0, 30, 'FPS: '+str("%.3f"%(clock.fps()))+'       Num: '
                             +str(len(blobs)), color = (255, 255, 255))

    Display.show_image(img)

    print(clock.fps()) #打印FPS
