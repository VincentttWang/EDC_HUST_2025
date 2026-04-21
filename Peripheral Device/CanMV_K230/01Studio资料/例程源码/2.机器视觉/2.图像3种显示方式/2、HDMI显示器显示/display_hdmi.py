'''
实验名称：图像3种显示方式
实验平台：01Studio CanMV K230
说明：实现摄像头图像采集通过IDE、HDMI和MIPI屏显示
'''

import time, os, sys

from media.sensor import * #导入sensor模块，使用摄像头相关接口
from media.display import * #导入display模块，使用display相关接口
from media.media import * #导入media模块，使用meida相关接口

sensor = Sensor() #构建摄像头对象
sensor.reset() #复位和初始化摄像头
sensor.set_framesize(Sensor.FHD) #设置帧大小FHD(1920x1080)，缓冲区和HDMI用,默认通道0
#sensor.set_framesize(width=800,height=480) #设置帧大小800x480,LCD专用,默认通道0
sensor.set_pixformat(Sensor.RGB565) #设置输出图像格式，默认通道0

#################################
## 图像3种不同显示方式（修改注释实现）
#################################

#Display.init(Display.VIRT, sensor.width(), sensor.height()) #通过IDE缓冲区显示图像
Display.init(Display.LT9611, to_ide=True) #通过HDMI显示图像
#Display.init(Display.ST7701, to_ide=True) #通过01Studio 3.5寸mipi显示屏显示图像

MediaManager.init() #初始化media资源管理器

sensor.run() #启动sensor

clock = time.clock()

while True:

    ####################
    ## 这里编写主要代码
    ####################
    clock.tick()

    img = sensor.snapshot() #拍摄一张图

    Display.show_image(img) #显示图片

    print(clock.fps()) #打印FPS


