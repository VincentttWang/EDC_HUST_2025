'''
实验名称：UART（串口通信）
作者：01Studio
实验平台：01Studio CanMV K230
说明：通过编程实现串口通信，跟电脑串口助手实现数据收发。
'''

#导入串口模块
from machine import UART
from machine import FPIOA
import time

fpioa = FPIOA()

# UART1代码
fpioa.set_function(3,FPIOA.UART1_TXD)
fpioa.set_function(4,FPIOA.UART1_RXD)

uart=UART(UART.UART1,115200) #设置串口号1和波特率

'''
# UART2代码
fpioa.set_function(11,FPIOA.UART2_TXD)
fpioa.set_function(12,FPIOA.UART2_RXD)

uart=UART(UART.UART2,115200) #设置串口号2和波特率
'''

uart.write('Hello 01Studio!')#发送一条数据

while True:

    text=uart.read(128) #接收128个字符
    if text != None:
        print(text) #通过REPL打印串口3接收的数据

    time.sleep(0.1) #100ms
