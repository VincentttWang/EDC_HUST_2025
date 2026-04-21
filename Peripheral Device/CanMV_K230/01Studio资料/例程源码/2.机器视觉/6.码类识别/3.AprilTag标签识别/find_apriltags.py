'''
实验名称：AprilTags标签识别
实验平台：01Studio CanMV K230
教程：wiki.01studio.cc
说明：推荐使用QVGA（320x240）分辨率，分辨率太高帧率会下降。
'''

import time, math, os, gc

from media.sensor import * #导入sensor模块，使用摄像头相关接口
from media.display import * #导入display模块，使用display相关接口
from media.media import * #导入media模块，使用meida相关接口
# apriltag代码最多支持可以同时处理6种tag家族。
# 返回的tag标记对象，将有其tag标记家族及其在tag标记家族内的id。

tag_families = 0
tag_families |= image.TAG16H5 # 注释掉，禁用这个家族
tag_families |= image.TAG25H7 # 注释掉，禁用这个家族
tag_families |= image.TAG25H9 # 注释掉，禁用这个家族
tag_families |= image.TAG36H10 # 注释掉，禁用这个家族
tag_families |= image.TAG36H11 # 注释掉以禁用这个家族(默认家族)
tag_families |= image.ARTOOLKIT # 注释掉，禁用这个家族

#标签系列有什么区别？ 那么，例如，TAG16H5家族实际上是一个4x4的方形标签。
#所以，这意味着可以看到比6x6的TAG36H11标签更长的距离。
#然而，较低的H值（H5对H11），意味着4x4标签的假阳性率远高于6x6标签。
#所以，除非你有理由使用其他标签系列，否则使用默认族TAG36H11。

def family_name(tag):
    if(tag.family() == image.TAG16H5):
        return "TAG16H5"
    if(tag.family() == image.TAG25H7):
        return "TAG25H7"
    if(tag.family() == image.TAG25H9):
        return "TAG25H9"
    if(tag.family() == image.TAG36H10):
        return "TAG36H10"
    if(tag.family() == image.TAG36H11):
        return "TAG36H11"
    if(tag.family() == image.ARTOOLKIT):
        return "ARTOOLKIT"


sensor = Sensor(width=1280, height=960) #构建摄像头对象，将摄像头长宽设置为4:3
sensor.reset() #复位和初始化摄像头
sensor.set_framesize(width=320, height=240) #设置帧大小为LCD分辨率(800x480)，默认通道0
sensor.set_pixformat(Sensor.RGB565) #设置输出图像格式，默认通道0

Display.init(Display.ST7701, to_ide=True) #同时使用3.5寸mipi屏和IDE缓冲区显示图像，800x480分辨率
#Display.init(Display.VIRT, sensor.width(), sensor.height()) #只使用IDE缓冲区显示图像

MediaManager.init() #初始化media资源管理器

sensor.run() #启动sensor

clock = time.clock()

while True:

    clock.tick()

    img = sensor.snapshot() #拍摄图片


    for tag in img.find_apriltags(families=tag_families): # 如果没有给出家族，默认TAG36H11。

            img.draw_rectangle(tag.rect(), color = (255, 0, 0), thickness=4)
            img.draw_cross(tag.cx(), tag.cy(), color = (0, 255, 0), thickness=2)
            print_args = (family_name(tag), tag.id(), (180 * tag.rotation()) / math.pi) #打印标签信息
            print("Tag Family %s, Tag ID %d, rotation %f (degrees)" % print_args)
            #img.draw_string_advanced(0, 0, 30, code.payload(), color = (255, 255, 255)) #图像显示条码信息

    #Display.show_image(img) #显示图片

    #显示图片，LCD居中方式显示
    Display.show_image(img, x=round((800-sensor.width())/2),y=round((480-sensor.height())/2)) #显示图片

    print(clock.fps()) #打印帧率
