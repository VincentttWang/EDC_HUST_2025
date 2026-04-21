'''
实验名称：线段检测
实验平台：01Studio CanMV K230
教程：wiki.01studio.cc
说明：推荐使用320x240以下分辨率，分辨率过大会导致帧率下降。
'''

import time, os, sys

from media.sensor import * #导入sensor模块，使用摄像头相关接口
from media.display import * #导入display模块，使用display相关接口
from media.media import * #导入media模块，使用meida相关接口

enable_lens_corr = False # 设为True可以获得更直的线段

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

    if enable_lens_corr: img.lens_corr(1.8) # for 2.8mm lens...

    # `merge_distance` 控制相近的线段是否合并.  数值 0 (默认值)表示不合并。数值
    #为1时候表示相近1像素的线段被合并。因此你可以通过改变这个参数来控制检测到线
    #段的数量。

    # `max_theta_diff` 控制相差一定角度的线段合并，默认是15度，表示15度内的线
    # 段都会合并

    for l in img.find_line_segments(merge_distance = 0, max_theta_diff = 5):

        img.draw_line(l.line(), color = (255, 0, 0), thickness=2)
        print(l)

    #Display.show_image(img) #显示图片

    #显示图片，仅用于LCD居中方式显示
    Display.show_image(img, x=round((800-sensor.width())/2),y=round((480-sensor.height())/2))

    print(clock.fps()) #打印FPS
