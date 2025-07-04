/**
 **********************************************************************************************************
 * 实验简介
 * 实验名称：ATK-MS601M模块测试实验
 * 实验平台：正点原子 阿波罗 H743开发板
 * 实验目的：学习ATK-MS601M模块的使用方法

 **********************************************************************************************************
 * 硬件资源及引脚分配
 * 1 RGB灯
     RED   : LED0 - PB1
 * 2 独立按键
     KEY0  - PH3
 * 3 串口1 (PA9/PA10连接在板载USB转串口芯片CH340上面)
 * 4 正点原子 2.8/3.5/4.3/7寸LCD模块(包括MCU屏和RGB屏,都支持)
 * 5 ATK-MS601M模块

 **********************************************************************************************************
 * 实验现象
 * 1 程序烧录后，自动完成ATK-MS601M模块及其相关软硬件的初始化，若初始化失败，则在串口调试助手上输出相关初
     始化失败信息，并且LED0闪烁。
 * 2 当按下按键0后，获取ATK-MS601M自动回传的数据信息，包括姿态角、陀螺仪、加速度计等的数据，
     然后将其打印到串口调试助手上，并且在LCD上进行显示。

 **********************************************************************************************************
 * 注意事项
 * 1 电脑端串口调试助手波特率必须是115200
 * 2 请使用XCOM/SSCOM串口调试助手,其他串口助手可能控制DTR/RTS导致MCU复位/程序不运行
 * 3 串口输入字符串以回车换行结束
 * 4 请用USB线连接在USB_UART,找到USB转串口后测试本例程
 * 5 P10的PA9/PA10必须通过跳线帽连接在RXD/TXD上
 * 6 4.3寸和7寸屏需要比较大电流,USB供电可能不足,请用外部电源适配器(推荐外接12V 1A电源).

 **********************************************************************************************************
 * 公司名称：广州市星翼电子科技有限公司（正点原子）
 * 电话号码：020-38271790
 * 传真号码：020-36773971
 * 公司网址：www.alientek.com
 * 购买地址：zhengdianyuanzi.tmall.com
 * 技术论坛：http://www.openedv.com/forum.php
 * 最新资料：www.openedv.com/docs/index.html
 *
 * 在线视频：www.yuanzige.com
 * B 站视频：space.bilibili.com/394620890
 * 公 众 号：mp.weixin.qq.com/s/y--mG3qQT8gop0VRuER9bw
 * 抖    音：douyin.com/user/MS4wLjABAAAAi5E95JUBpqsW5kgMEaagtIITIl15hAJvMO8vQMV1tT6PEsw-V5HbkNLlLMkFf1Bd
 **********************************************************************************************************
 */