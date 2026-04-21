'''
实验名称：获取主控ID编号
实验平台：01Studio CanMV K230
教程：wiki.01studio.cc
'''

import machine

#原始ID，32位
chipid = machine.chipid()
print(chipid)

#转成字符串方便查看
hex_str = ''
for i in range(len(chipid)):
    hex_str = hex_str + "{:02x}".format(chipid[i])
print(hex_str)
