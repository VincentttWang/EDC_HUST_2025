# 通常与硬件相关,包含对图像传感器,显示器和媒体处理的操作
from media.sensor import *
from media.display import *
from media.media import *
# 用于检测应用的类,具体是处理数字识别任务
from libs.PlatTasks import DetectionApp
# 一些工具函数,可能用于图像处理,配置读取等
from libs.Utils import *
# 配置串口通信和GPIO(FPIOA)
from machine import UART, FPIOA
# 图像处理相关库,用于生成or处理图像数据
import image
# 标准库,分别用于时间操作,垃圾回收和多线程
import time, gc, _thread

# ---------- 全局配置 ----------
sensor = None               # 存储图像传感器的对象
display_size = [800, 480]   # 显示器的尺寸//
rgb888p_size = [1280, 720]  # 图像分辨率//
det_stop = False            # 控制数字检测线程(AI推理识别图像)的停止
det_osd_img = None          # 存储用于显示的图像

# last_detected_num 和 x_index 在当前逻辑中已不再直接用于发送，但为了兼容性可以保留
# 它们会更新为上次发送的第一个数字（如果存在）
x_index = -1
last_detected_num = -1

# 新增全局变量：存储上一次成功识别到的所有数字信息 (只存数字和X坐标用于排序，X坐标不发送)//
# 格式为列表，每个元素是一个字典，如 [{"digit": 5, "x_coord": 100}, {"digit": 2, "x_coord": 250}]
last_successful_detections = []

# ---------- 串口配置 ----------
fpioa = FPIOA()
fpioa.set_function(11, FPIOA.UART2_TXD)
fpioa.set_function(12, FPIOA.UART2_RXD)
uart = UART(UART.UART2, baudrate=115200, timeout=100)

# ---------- 状态变量 ----------
frame_count = 0

def media_init():
    global sensor, det_osd_img
    # 初始化,选择使用ST7701显示屏,配置分辨率,to_ide为将屏幕显示传输到IDEosd_num:在 show_image 时支持的 LAYER 数量
    Display.init(Display.ST7701, width=display_size[0], height=display_size[1], to_ide=True, osd_num=1)


    sensor = Sensor(fps=30) # id,[width, height, fps]  ,  图像宽度默认1920,高度默认1080,帧率默认为30
    sensor.reset() # 复位和初始化摄像头

    # 图像显示通道[0]，(chn_1不常用),YUV格式,（用于LCD/HDMI）
    sensor.set_framesize(w=display_size[0], h=display_size[1], chn=CAM_CHN_ID_0) # 通道图像输出尺寸
    sensor.set_pixformat(Sensor.YUV420SP, chn=CAM_CHN_ID_0) # 设置图像像素格式(更多)
    # 图像识别通道[2]，RGB格式，分辨率为 224x224（用于神经网络）
    sensor.set_framesize(w=rgb888p_size[0], h=rgb888p_size[1], chn=CAM_CHN_ID_2)
    sensor.set_pixformat(Sensor.RGBP888, chn=CAM_CHN_ID_2)

    # 函数解释: x , y 把图像显示在屏幕的左上角(0,0)位置     chn:表示从CSI0接收的图像通道获取图像
    sensor_bind_info = sensor.bind_info(x=0, y=0, chn=CAM_CHN_ID_0) #绑定摄像头图象到显示层
    # 把上一步返回的摄像头图像，显示在指定的显示层上,LAYER_VIDEO1 是主图层（显示视频或摄像头图像）,LAYER_OSD0/OSD1 是叠加图层（文字、框框、识别结果）
    Display.bind_layer(**sensor_bind_info, layer=Display.LAYER_VIDEO1)
    # 创建一个图像对象,用于显示识别结果,用于画框、画文字、图像叠加显示    带透明的通道的图像形式
    det_osd_img = image.Image(display_size[0], display_size[1], image.ARGB8888)

    # 初始化媒体管理器，可能是用于管理显示和摄像头的资源,必须在sensor.run前,在Display.init后
    MediaManager.init()
    # 启动图像传感器的输出。必须在调用 MediaManager.init() 之后执行此操作!!!(同样,停止输出在Media.deinit(简写)前)
    sensor.run() # 启动
    return True

def media_deinit():
    global sensor # 全局化,确保这个函数内部操作的是主摄像头实例
    os.exitpoint(os.EXITPOINT_ENABLE_SLEEP)#作用:允许MCU进入睡眠状态,节能,一般在资源释放、摄像头不再工作时启用
    sensor.stop() # 停止图像采集,释放CSI资源,否则下一次初始化sensor.reset()会报错或者卡住
    Display.deinit()
    time.sleep_ms(50)
    MediaManager.deinit()

 # ---------- 重复识别辅助函数 ----------   
def calculate_iou(box1, box2):
    """
    计算两个边界框的交并比 (Intersection over Union, IoU)。
    框的格式：[x1, y1, x2, y2]
    """
    # 计算交集矩形的坐标
    x1_inter = max(box1[0], box2[0])
    y1_inter = max(box1[1], box2[1])
    x2_inter = min(box1[2], box2[2])
    y2_inter = min(box1[3], box2[3])

    # 计算交集面积
    inter_area = max(0, x2_inter - x1_inter) * max(0, y2_inter - y1_inter)

    # 计算各自的面积
    area1 = (box1[2] - box1[0]) * (box1[3] - box1[1])
    area2 = (box2[2] - box2[0]) * (box2[3] - box2[1])

    # 计算并集面积
    union_area = area1 + area2 - inter_area

    # 计算 IoU
    iou = inter_area / union_area if union_area > 0 else 0
    return iou

# ---------- 数字识别线程 ----------
def digit_recognition_thread():
    # 声明全局变量... (与之前相同)
    global sensor, det_osd_img, last_detected_num, frame_count, x_index, last_successful_detections

    # 加载模型配置... (与之前相同)
    root_path = "/sdcard/mp_deployment_source/"
    deploy_conf = read_json(root_path + "deploy_config.json")
    if "categories" not in deploy_conf:
        deploy_conf["categories"] = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]

    kmodel_path = root_path + deploy_conf["kmodel_path"]
    labels = deploy_conf["categories"]
    confidence_threshold = deploy_conf.get("confidence_threshold", 0.6)
    nms_threshold = deploy_conf.get("nms_threshold", 0.3)
    model_input_size = deploy_conf.get("img_size", [224, 224])
    model_type = deploy_conf.get("model_type", "YoloV5")
    anchors = []
    if model_type == "AnchorBaseDet":
        anchors = deploy_conf.get("anchors", [])
        if anchors:
            anchors = anchors[0] + anchors[1] + anchors[2]
            
    det_app = DetectionApp("video", kmodel_path, labels, model_input_size, anchors,
                           model_type, confidence_threshold, nms_threshold,
                           rgb888p_size, display_size)
    det_app.config_preprocess()

    while not det_stop:
        frame_count += 1
        try:
            # 图像捕捉与模型推理... (与之前相同)
            img = sensor.snapshot(chn=CAM_CHN_ID_2)
            if img is None:
                time.sleep_ms(50)
                continue
            img_np = img.to_numpy_ref()
            res = det_app.run(img_np)
            
            scores = res.get("scores", [])
            idxs = res.get("idx", [])
            boxes = res.get("boxes", [])

            # NMS前置信度筛选... (与之前相同)
            all_detections = []
            for i in range(len(idxs)):
                if scores[i] >= confidence_threshold:
                    all_detections.append({
                        "box": boxes[i],
                        "score": scores[i],
                        "class_id": idxs[i],
                        "label": labels[idxs[i]]
                    })
            
            all_detections.sort(key=lambda d: d["score"], reverse=True)

            # 手动NMS去重... (与之前相同)
            final_detections = []
            manual_nms_threshold = 0.4
            while len(all_detections) > 0:
                best_detection = all_detections.pop(0)
                final_detections.append(best_detection)
                remaining_detections = []
                for det in all_detections:
                    iou = calculate_iou(best_detection["box"], det["box"])
                    if iou < manual_nms_threshold:
                        remaining_detections.append(det)
                all_detections = remaining_detections

            # 准备当前帧数据... (与之前相同)
            current_frame_detections = []
            for det in final_detections:
                name = det["label"]
                if name.isdigit():
                    detected_digit = int(name)
                    x_coordinate = int(det["box"][0])
                    current_frame_detections.append({
                        "digit": detected_digit,
                        "x_coord": x_coordinate
                    })
            current_frame_detections.sort(key=lambda d: d["x_coord"])

            # 确定要发送的数据... (与之前相同)
            if current_frame_detections:
                detections_to_send = current_frame_detections
                last_successful_detections = list(current_frame_detections)
            else:
                detections_to_send = last_successful_detections

            # ---- 这里是本次修改的核心 ----
            # 6. 根据数字个数决定是否构建数据包并发送
            num_digits_to_send = len(detections_to_send)
            
            # 只有当检测到的数字个数是1, 2, 或 4 时，才允许发送
            if num_digits_to_send in [1, 2, 4]:
                # 构建和发送数据包的逻辑被移到这个if判断内部
                data_payload = [0x12, num_digits_to_send]
                for det_info in detections_to_send:
                    data_payload.append(det_info["digit"])
                data_payload.append(0x5B)
                uart.write(bytes(data_payload))

                # 更新旧的全局变量（可选，为兼容性保留）
                if num_digits_to_send > 0:
                    last_detected_num = detections_to_send[0]["digit"]
                    x_index = detections_to_send[0]["x_coord"]
                else:
                    last_detected_num = -1
                    x_index = -1
            # 当 num_digits_to_send 为 3 (或其他不允许的值) 时，此代码块将被跳过，不会发送任何数据。

            # 7. 在屏幕上绘制结果 (不受影响，依然显示所有识别内容)
            filtered_res_for_display = {
                "boxes": [d["box"] for d in final_detections],
                "scores": [d["score"] for d in final_detections],
                "idx": [d["class_id"] for d in final_detections]
            }
            det_osd_img.clear()
            det_app.draw_result(det_osd_img, filtered_res_for_display)
            Display.show_image(det_osd_img)

        except Exception as e:
            print(f"Error in digit_recognition_thread: {e}")
            pass

        time.sleep_ms(50)

def main_loop():
    # 主循环不再发送数据，因为 digit_recognition_thread 已经负责全部发送。
    # 如果需要心跳包或其他定时任务，可以在这里添加。
    try:
        while True:
            # print("Main loop running...") # 调试用，可以移除
            time.sleep(1) # 简单休眠，保持线程活跃
    except Exception as e:
        print(f"Error in main_loop: {e}")
        pass

if __name__ == "__main__":
    try:
        if media_init():
            _thread.start_new_thread(digit_recognition_thread, ())
            main_loop()
    finally:
        det_stop = True
        time.sleep(1)
        media_deinit()
        gc.collect()
