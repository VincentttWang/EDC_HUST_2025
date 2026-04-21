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

# ---------- 数字识别线程 ----------
def digit_recognition_thread():
    # 声明所有需要在此函数中修改的全局变量
    global sensor, det_osd_img, last_detected_num, frame_count, x_index, last_successful_detections

    # 加载数字识别模型,路径一般包括:deploy_config.json：模型配置 , .kmodel 文件：神经网络模型本体 等
    root_path = "/sdcard/mp_deployment_source/" # 下面进行字符串拼接
    deploy_conf = read_json(root_path + "deploy_config.json") # read_json:读取一个 JSON 文件并解析为 Python 字典。
    # 如果里面没有categories,程序自动补上
    if "categories" not in deploy_conf:
        deploy_conf["categories"] = ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]

    #从 SD 卡加载部署配置文件，读取模型路径、标签、置信度阈值等配置信息。
    kmodel_path = root_path + deploy_conf["kmodel_path"] # 得到 .kmodel 神经网络文件的真实位置
    labels = deploy_conf["categories"] # 读取 "categories" 键对应的值（一般是字符串列表）,表示模型输出的所有类别名称
    confidence_threshold = deploy_conf.get("confidence_threshold", 0.5) # 表示一个预测结果最小可信度阈值，用于过滤低概率结果,没有就默认0.5
    nms_threshold = deploy_conf.get("nms_threshold", 0.3) # 取非极大值抑制（NMS）阈值，默认 0.3,控制模型输出多个框时的“去重”策略,假如模型输出了两个很相似的框（重叠度高），就只保留得分高的一个
    model_input_size = deploy_conf.get("img_size", [224, 224]) # 获取模型输入尺寸，默认 224x224,图像送入模型前需 resize 成这个大小
    model_type = deploy_conf.get("model_type", "YoloV5") # 获取模型类型字段，默认是 "YoloV5"
    anchors = [] # 初始化空列表,某些模型（如 YOLO）需要提前提供 anchor 信息，用于定位边框

    # 检查当前模型类型是否是 AnchorBaseDet,说明当前加载的是一种基于 Anchor（锚框）机制的目标检测模型
    # 比如YOLOv3、YOLOv5,YoloFastest,Tiny-YOLO等
    if model_type == "AnchorBaseDet":
        anchors = deploy_conf.get("anchors", [])
        if anchors:
            anchors = anchors[0] + anchors[1] + anchors[2] # 把 anchors 合并成一个一维列表（扁平化）
    # 根据配置文件获取模型路径、标签、置信度阈值等参数
    det_app = DetectionApp("video", kmodel_path, labels, model_input_size, anchors,
                           model_type, confidence_threshold, nms_threshold,
                           rgb888p_size, display_size)
    det_app.config_preprocess() # 创建数字识别的应用对象 DetectionApp，加载模型，配置参数

    while not det_stop:
        frame_count += 1 # 记录当前是第几帧图像（可以用于帧率测试、调试、定期打印结果等）
        try:
            img = sensor.snapshot(chn=CAM_CHN_ID_2)
            if img is None: # 检查图像是否为空（某些帧读取失败），避免程序崩溃
                time.sleep_ms(50)
                continue

            img_np = img.to_numpy_ref() # 把图像对象转换为模型输入用的 Numpy-like 数据结构
            det_app.cur_result = {"boxes": [], "scores": [], "idx": []} # 清空上一帧的识别结果
            res = det_app.run(img_np) # 运行模型推理，将当前帧图像送入 DetectionApp，返回检测结果
            # 拿出推理结果
            scores = res.get("scores", [])
            idxs = res.get("idx", [])
            boxes = res.get("boxes", [])
            # 初始化当前帧的识别结果列表
            current_frame_detections = [] # 存储当前帧所有有效检测到的数字信息，包含X坐标用于排序

            # 遍历所有检测结果并筛选
            for i in range(len(idxs)):
                class_id = idxs[i]
                score = scores[i]

                if i >= len(boxes) or class_id < 0 or class_id >= len(labels):
                    continue

                box = boxes[i]
                name = labels[class_id]

                # 筛选：只处理数字类别且置信度高于阈值的检测结果
                if name.isdigit() and score >= confidence_threshold:
                    detected_digit = int(name)
                    x_coordinate = int(box[0]) # 获取左上角的x1坐标，用于排序

                    current_frame_detections.append({
                        "digit": detected_digit,
                        "x_coord": x_coordinate # X坐标用于排序，但不发送
                    })
                    print(detected_digit)

            # 对当前帧检测到的数字按X轴坐标从小到大排序
            current_frame_detections.sort(key=lambda d: d["x_coord"])

            # 确定本次要发送的数据：如果当前帧有检测到数字，就发送当前帧的；否则发送上一次的
            if current_frame_detections:
                # 当前帧有检测到数字，更新 last_successful_detections
                detections_to_send = current_frame_detections
                last_successful_detections = list(current_frame_detections) # 深拷贝，防止后续修改
            else:
                # 当前帧没有检测到数字，发送上一次成功识别的信息
                detections_to_send = last_successful_detections


            # 构建发送数据包
            # 格式: [帧头(0x12), 数字个数(N), D1, D2, ..., DN, 帧尾(0x5B)]
            if detections_to_send:
                # 数字个数不能超过255，这里做个简单限制
                num_digits = min(len(detections_to_send), 255)
                data_payload = [0x12, num_digits] # 帧头 + 数字个数

                for i in range(num_digits):
                    det_info = detections_to_send[i]
                    digit = det_info["digit"]
                    data_payload.append(digit) # 只添加数字，不添加X坐标

                data_payload.append(0x5B) # 帧尾
                print(data_payload)

                uart.write(bytes(data_payload))
                # 调试打印发送的数据
                # print(f"Sent multi-digit data: {bytes(data_payload)}")
                # print(f"Detected {num_digits} digits: {[d['digit'] for d in detections_to_send]}")

                # 更新全局变量 last_detected_num 和 x_index 为当前发送列表中的第一个数字（如果存在）
                if num_digits > 0:
                    last_detected_num = detections_to_send[0]["digit"]
                    x_index = detections_to_send[0]["x_coord"] # X坐标仍用于更新此全局变量
                else:
                    last_detected_num = -1
                    x_index = -1
            else:
                # 如果 last_successful_detections 也为空（从未成功识别过），则不发送任何数据
                # 此时 last_detected_num 和 x_index 保持为 -1
                pass


            # 在屏幕上显示结果
            det_app.draw_result(det_osd_img, res) # res 包含所有检测结果，包括未发送的低置信度结果
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
