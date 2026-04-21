# Camera Example
#
# Note: You will need an SD card to run this example.
#
# You can start camera preview and capture yuv image.
import time, os, sys

from media.sensor import *
from media.display import *
from media.media import *

from time import ticks_ms
from machine import FPIOA
from machine import Pin
from machine import PWM
from machine import Timer
import time

# save image raw data, use 7yuv to preview

def save_img(img, chn):
    if img.format() == image.YUV420:
        suffix = "yuv420sp"
    elif img.format() == image.RGB888:
        suffix = "rgb888"
    elif img.format() == image.RGBP888:
        suffix = "rgb888p"
    else:
        suffix = "unkown"

    filename = f"/sdcard/camera_chn_{chn:02d}_{img.width()}x{img.height()}.{suffix}"
    print("save capture image to file:", filename)
    img.save(filename)

sensor = None

try:
    fpioa = FPIOA()
    fpioa.help()
    fpioa.set_function(21, FPIOA.GPIO21)

    key = Pin(21, Pin.IN, Pin.PULL_UP)

    print("camera_test")
    # construct a Sensor object with default configure
    sensor = Sensor()
    # sensor reset
    sensor.reset()
    # set hmirror
    # sensor.set_hmirror(False)
    # sensor vflip
    # sensor.set_vflip(False)

    # set chn0 output size, 1920x1080
    sensor.set_framesize(Sensor.FHD)
    # set chn0 output format
    sensor.set_pixformat(Sensor.YUV420SP)
    # bind sensor chn0 to display layer video 1
    bind_info = sensor.bind_info()
    Display.bind_layer(**bind_info, layer = Display.LAYER_VIDEO1)

    # set chn1 output format
    sensor.set_framesize(width=1024, height=768, chn = CAM_CHN_ID_1)
    sensor.set_pixformat(Sensor.RGB565, chn = CAM_CHN_ID_1)

    # set chn2 output format
    sensor.set_framesize(width=800,height=480,chn = CAM_CHN_ID_2) #设置帧大小800x480,LCD专用,通道2
    sensor.set_pixformat(Sensor.RGB565, chn = CAM_CHN_ID_2) #设置输出图像格式，通道2

    # use mipi as display output
    Display.init(Display.ST7701, to_ide=True) #通过01Studio 3.5寸mipi显示屏显示图像
    # init media manager
    MediaManager.init()
    # sensor start run
    sensor.run()
    clock = time.clock()

    counter = 0
    save_folder = "/data/image/"
    #class_lst = ["one", "two", "three", "four", "five", \
                 #"six", "seven", "eight","two-pic-1","two-pic-2","two-pic-3","two-pic-4","four-pic-1","four-pic-2","four-pic-3","four-pic-4",]
    class_lst = ["one", "three", "five", "seven", "eight", "two-pic-1","two-pic-2", "four-pic-1", "four-pic-2"]
    class_id = -1
    prefix = "batch_1_"
    while True:
        clock.tick()
        os.exitpoint()
        img = sensor.snapshot(chn=CAM_CHN_ID_2)
        if key.value() == 0:
            class_id = (class_id + 1) % len(class_lst)
            os.mkdir(save_folder+class_lst[class_id])
            for i in range(3):
                print("will collect {} class in {} s".format(class_lst[class_id], 3-i))
                img = sensor.snapshot(chn=CAM_CHN_ID_2)
                img.draw_string_advanced(360, 200, 40, "{}".format(3-i), color=(255, 255, 255))
                Display.show_image(img)
                time.sleep_ms(1000)
            counter = 10
        if not counter == 0:
            time.sleep_ms(500)
            file_name = "{}_{}_{}.jpg".format(prefix, class_lst[class_id], str(counter))
            img = sensor.snapshot(chn=CAM_CHN_ID_1)
            save_img = img.compress(95)
            file_path = save_folder + class_lst[class_id] + "/" + file_name
            with open(file_path, 'wb') as f:
                f.write(save_img)
            print("img saved to \"{}\"".format(file_path))
            counter -= 1
        img = sensor.snapshot(chn=CAM_CHN_ID_2)
        img.draw_string_advanced(50, 30, 30, "fps: {}".format(clock.fps()), color=(255, 0, 0))
        if counter == 0:
            img.draw_string_advanced(50, 400, 40, "Future Task:  {}".format(class_lst[class_id+1]),color=(255,255,255))
        else:
            img.draw_string_advanced(700, 400, 40, "{}".format(10-counter), color=(255, 255, 255))
        Display.show_image(img)


except KeyboardInterrupt as e:
    print("用户停止: ", e)
except BaseException as e:
    print(f"异常: {e}")
finally:
    if isinstance(sensor, Sensor):
        sensor.stop()
    Display.deinit()
    os.exitpoint(os.EXITPOINT_ENABLE_SLEEP)
    time.sleep_ms(100)
    MediaManager.deinit()
