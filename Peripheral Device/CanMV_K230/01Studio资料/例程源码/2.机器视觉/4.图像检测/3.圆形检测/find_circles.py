'''
实验名称：圆形检测
实验平台：01Studio CanMV K230
教程：wiki.01studio.cc
说明：推荐使用320x240以下分辨率，分辨率过大会导致帧率下降。
'''

import time, os, sys

from media.sensor import * #导入sensor模块，使用摄像头相关接口
from media.display import * #导入display模块，使用display相关接口
from media.media import * #导入media模块，使用meida相关接口

sensor = Sensor(width=1280, height=960) #构建摄像头对象，将摄像头长宽设置为4:3
sensor.reset() #复位和初始化摄像头
sensor.set_framesize(width=320, height=240) #设置帧大小，默认通道0
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

    img = sensor.snapshot() #拍摄一张图片

    # 圆形类有 4 个参数值： 圆心(x, y), r (半径)和 magnitude（量级）；
    # 量级越大说明识别到的圆可信度越高。
    # `threshold` 参数控制找到圆的数量，数值的提升会降低识别圆形的总数。
    # `x_margin`, `y_margin`, and `r_margin`控制检测到接近圆的合并调节.
    # r_min, r_max, and r_step 用于指定测试圆的半径范围。
    for c in img.find_circles(threshold = 2000, x_margin = 10, y_margin= 10,
                              r_margin = 10,r_min = 2, r_max = 100, r_step = 2):
        #画红色圆做指示
        img.draw_circle(c.x(), c.y(), c.r(), color = (255, 0, 0),thickness=2)

        print(c) #打印圆形的信息

    #Display.show_image(img) #显示图片

    #显示图片，仅用于LCD居中方式显示
    Display.show_image(img, x=round((800-sensor.width())/2),y=round((480-sensor.height())/2))

    print(clock.fps()) #打印FPS

