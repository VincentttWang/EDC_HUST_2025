'''
实验名称：Socket通讯
实验平台：01Studio CanMV K230
说明：通过Socket编程实现CanMV K230与电脑服务器助手建立TCP连接，相互收发数据。
'''

#导入相关模块
import network,socket,time
from machine import Pin,Timer

#WIFI连接函数
def WIFI_Connect():

    WIFI_LED=Pin(52, Pin.OUT) #初始化WIFI指示灯

    wlan = network.WLAN(network.STA_IF) #STA模式
    wlan.active(True)                   #激活接口

    if not wlan.isconnected():

        print('connecting to network...')

        for i in range(3): #重复连接3次

            #输入WIFI账号密码（仅支持2.4G信号）, 连接超过5秒为超时
            wlan.connect('01Studio', '88888888')

            if wlan.isconnected(): #连接成功
                break

    if wlan.isconnected(): #连接成功

        print('connect success')

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

#判断WIFI是否连接成功
if WIFI_Connect(): #连接成功

    #创建socket连接TCP类似，连接成功后发送“Hello 01Studio！”给服务器。
    s=socket.socket()
    addr=socket.getaddrinfo('192.168.1.115',10000)[0][-1] #服务器IP和端口
    print(addr)
    s.connect(addr)
    s.settimeout(0)#接收不阻塞
    s.send('Hello 01Studio!')

    while True:

        text=s.recv(128) #单次最多接收128字节
        if text == b'':
            pass

        else: #打印接收到的信息为字节，可以通过decode('utf-8')转成字符串
            print(text)
            s.send('I got:'+text.decode('utf-8'))

        time.sleep_ms(50)
