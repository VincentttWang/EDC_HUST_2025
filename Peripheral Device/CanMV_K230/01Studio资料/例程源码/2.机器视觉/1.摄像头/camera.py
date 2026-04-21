'''
实验名称：摄像头使用
实验平台：01Studio CanMV K230
说明：实现摄像头图像采集显示
'''

import time, os, sys

from media.sensor import * #导入sensor模块，使用摄像头相关接口
from media.display import * #导入display模块，使用display相关接口
from media.media import * #导入media模块，使用meida相关接口

sensor = Sensor() #构建摄像头对象
sensor.reset() #复位和初始化摄像头
sensor.set_framesize(Sensor.FHD) #设置帧大小FHD(1920x1080)，默认通道0
sensor.set_pixformat(Sensor.RGB565) #设置输出图像格式，默认通道0

#使用IDE缓冲区输出图像,显示尺寸和sensor配置一致。
Display.init(Display.VIRT, sensor.width(), sensor.height())

MediaManager.init() #初始化media资源管理器

sensor.run() #启动sensor

clock = time.clock()

while True:

    ################
    ## 这里编写代码 ##
    ################
    clock.tick()

    img = sensor.snapshot() #拍摄一张图

    Display.show_image(img) #显示图片

    print(clock.fps()) #打印FPS

