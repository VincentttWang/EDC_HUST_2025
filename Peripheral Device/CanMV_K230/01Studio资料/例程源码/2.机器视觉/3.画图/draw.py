'''
实验名称：画图
实验平台：01Studio CanMV K230
说明：画各种图形和写字符, 通过IDE和LCD显示。
'''

import time, os, sys

from media.sensor import * #导入sensor模块，使用摄像头相关接口
from media.display import * #导入display模块，使用display相关接口
from media.media import * #导入media模块，使用meida相关接口

sensor = Sensor() #构建摄像头对象
sensor.reset() #复位和初始化摄像头
sensor.set_framesize(width=800, height=480) #设置帧大小VGA，默认通道0
sensor.set_pixformat(Sensor.RGB565) #设置输出图像格式，默认通道0

#Display.init(Display.VIRT, sensor.width(), sensor.height()) #使用IDE缓冲区输出图像
Display.init(Display.ST7701,to_ide=True) #通过01Studio 3.5寸mipi显示屏显示图像
MediaManager.init() #初始化media资源管理器

sensor.run() #启动sensor

clock = time.clock()

while True:

    ################
    ## 这里编写代码 ##
    ################
    clock.tick()

    img = sensor.snapshot()
    # 画线段：从 x0, y0 到 x1, y1 坐标的线段，颜色红色，线宽度 2。
    img.draw_line(20, 20, 100, 20, color = (255, 0, 0), thickness = 2)

    #画矩形：绿色不填充。
    img.draw_rectangle(150, 20, 100, 30, color = (0, 255, 0), thickness = 2, fill = False)

    #画圆：蓝色不填充。
    img.draw_circle(60, 120, 30, color = (0, 0, 255), thickness = 2, fill = False)

    #画箭头：白色。
    img.draw_arrow(150, 120, 250, 120, color = (255, 255, 255), size = 20, thickness = 2)

    #画十字交叉。
    img.draw_cross(60, 200, color = (255, 255, 255), size = 20, thickness = 2)

    #写字符。
    #img.draw_string(150, 200, "Hello 01Studio!", color = (255, 255, 255), scale = 4, mono_space = False)

    #写字符，支持中文。
    img.draw_string_advanced(150, 180, 30, "Hello 01Studio", color = (255, 255, 255))
    img.draw_string_advanced(40, 300, 30, "人生苦短, 我用Python", color = (255, 255, 255))

    Display.show_image(img)

    print(clock.fps()) #打印FPS
