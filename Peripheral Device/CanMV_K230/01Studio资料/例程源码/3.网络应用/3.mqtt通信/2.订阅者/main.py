'''
实验名称：MQTT通信（订阅者）
作者：01Studio
实验平台：01Studio CanMV K230
说明：编程实现MQTT通信，实现订阅（接收）数据。
'''
import network,time
from simple import MQTTClient #导入MQTT板块
from machine import Pin,Timer

#WIFI连接函数
def WIFI_Connect():

    WIFI_LED=Pin(52, Pin.OUT) #初始化WIFI指示灯

    wlan = network.WLAN(network.STA_IF) #STA模式
    wlan.active(True)                   #激活接口
    start_time=time.time()              #记录时间做超时判断

    if not wlan.isconnected():

        print('connecting to network...')

        for i in range(3): #重复连接3次

            #输入WIFI账号密码（仅支持2.4G信号）, 连接超过5秒为超时
            wlan.connect('01Studio', '88888888')

            if wlan.isconnected(): #连接成功
                break

    if wlan.isconnected(): #连接成功

        #LED蓝灯点亮
        WIFI_LED.value(1)

        #等待获取IP地址
        while wlan.ifconfig()[0] == '0.0.0.0':
            pass

        #串口打印信息
        print('network information:', wlan.ifconfig())

        return True

    else: #连接失败

        #LED闪烁3次提示
        for i in range(3):
            WIFI_LED.value(1)
            time.sleep_ms(300)
            WIFI_LED.value(0)
            time.sleep_ms(300)

        wlan.active(False)

        return False


#设置MQTT回调函数,有信息时候执行
def MQTT_callback(topic, msg):
    print('topic: {}'.format(topic))
    print('msg: {}'.format(msg))

#接收数据任务
def MQTT_Rev(tim):
    try:
        client.check_msg()
    except OSError:
        pass

#执行WIFI连接函数并判断是否已经连接成功
if WIFI_Connect():

    SERVER = 'mq.tongxinmao.com'
    PORT = 18830
    CLIENT_ID = '01Studio-CanMV-K230' # 客户端ID
    TOPIC = '/public/01Studio/2' # TOPIC名称

    client = MQTTClient(CLIENT_ID, SERVER, PORT) #建立客户端对象
    client.set_callback(MQTT_callback)  #配置回调函数
    client.connect()
    client.subscribe(TOPIC) #订阅主题

    #开启RTOS定时器，编号为-1,周期300ms，执行MQTT通信接收任务
    tim = Timer(-1)
    tim.init(period=300, mode=Timer.PERIODIC, callback=MQTT_Rev)

while True:
    pass
