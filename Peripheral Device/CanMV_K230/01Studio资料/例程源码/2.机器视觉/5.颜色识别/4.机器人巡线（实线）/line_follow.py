'''
实验名称：机器人巡线（实线）
实验平台：01Studio CanMV K230
教程：wiki.01studio.cc

# 黑色灰度线巡线跟踪示例
#
#做一个跟随机器人的机器人需要很多的努力。这个示例脚本
#演示了如何做机器视觉部分的线跟随机器人。你
#可以使用该脚本的输出来驱动一个差分驱动机器人
#跟着一条线走。这个脚本只生成一个表示的旋转值（偏离角度）
#你的机器人向左或向右。
#
# 为了让本示例正常工作，你应该将摄像头对准一条直线（实线）
#并将摄像头调整到水平面45度位置。请保证画面内只有1条直线。
'''

import time, os, sys, math

from media.sensor import * #导入sensor模块，使用摄像头相关接口
from media.display import * #导入display模块，使用display相关接口
from media.media import * #导入media模块，使用meida相关接口

# 追踪黑线。使用 [(128, 255)] 追踪白线.
GRAYSCALE_THRESHOLD = [(0, 64)]

# 下面是一个roi【区域】元组列表。每个 roi 用 (x, y, w, h)表示的矩形。

'''
#采样图像QQVGA 160*120，列表把roi把图像分成3个矩形，越靠近的摄像头视野（通常为图像下方）的矩形权重越大。
ROIS = [ # [ROI, weight]
        (0, 100, 160, 20, 0.7), # 可以根据不同机器人情况进行调整。
        (0,  50, 160, 20, 0.3),
        (0,   0, 160, 20, 0.1)
       ]
'''
#采样图像为QVGA 320*240，列表把roi把图像分成3个矩形，越靠近的摄像头视野（通常为图像下方）的矩形权重越大。
ROIS = [ # [ROI, weight]
        (0, 200, 320, 40, 0.7), # 可以根据不同机器人情况进行调整。
        (0,  100, 320, 40, 0.3),
        (0,   0, 320, 40, 0.1)
       ]

# 计算以上3个矩形的权值【weight】的和，和不需要一定为1.
weight_sum = 0
for r in ROIS: weight_sum += r[4] # r[4] 为矩形权重值.

sensor = Sensor(width=1280, height=960) #构建摄像头对象，将摄像头长宽设置为4:3
sensor.reset() #复位和初始化摄像头
sensor.set_framesize(width=320, height=240) #设置帧大小，默认通道0
sensor.set_pixformat(Sensor.GRAYSCALE) #设置输出图像格式，默认通道0

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

    centroid_sum = 0

    for r in ROIS:
        blobs = img.find_blobs(GRAYSCALE_THRESHOLD, roi=r[0:4], merge=True) # r[0:4] 是上面定义的roi元组.

        if blobs:
            # Find the blob with the most pixels.
            largest_blob = max(blobs, key=lambda b: b.pixels())

            # Draw a rect around the blob.
            img.draw_rectangle(largest_blob.rect())
            img.draw_cross(largest_blob.cx(),
                           largest_blob.cy())

            centroid_sum += largest_blob.cx() * r[4] # r[4] 是每个roi的权重值.

    center_pos = (centroid_sum / weight_sum) # 确定直线的中心.

    # 将直线中心位置转换成角度，便于机器人处理.
    deflection_angle = 0

    # 使用反正切函数计算直线中心偏离角度。可以自行画图理解
    #权重X坐标落在图像左半部分记作正偏，落在右边部分记为负偏，所以计算结果加负号。

    #deflection_angle = -math.atan((center_pos-80)/60) #采用图像为QQVGA 160*120时候使用

    deflection_angle = -math.atan((center_pos-160)/120) #采用图像为QVGA 320*240时候使用

    # 将偏离值转换成偏离角度.
    deflection_angle = math.degrees(deflection_angle)

    # 计算偏离角度后可以控制机器人进行调整.
    print("Turn Angle: %f" % deflection_angle)

    # LCD显示偏移角度,scale参数可以改变字体大小
    img.draw_string_advanced(2,2,20, str('%.1f' % deflection_angle), color=(255,255,255))

    #Display.show_image(img) #显示图片

    #显示图片，仅用于LCD居中方式显示
    Display.show_image(img, x=round((800-sensor.width())/2),y=round((480-sensor.height())/2))

    print(clock.fps()) #打印FPS
