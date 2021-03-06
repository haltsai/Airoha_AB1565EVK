#!/usr/bin/python
# -*- coding: UTF-8 -*-

import re
import binascii
import os

exception_data_file = "temp/excep_data.txt"
reg_dump_file_name = "temp/dump_reg_cmds_2811.txt"
mem_dump_file_name = "temp/dump_mem_cmds_2811.txt"

register_list = []
regist_spec_name_tuple = ('epc1','excsave1','ps','pc','exccause','excvaddr','windowbase','windowstart')

mem_dump_region_list = []
mem_dump_pre = "temp/dump_mem_cmds_2811_"
mem_dump_post = ".bin"

def addOneRegisterValue(name , value):
    global regist_spec_name_tuple
    global register_list

    reg_name_matched = re.match('(ar\d+)',name)
    if reg_name_matched:
        register_list.append("set $" + name + '=' + value)
        return

    for spec_name in regist_spec_name_tuple:
        if spec_name == name:
            register_list.append("set $" + name + '=' + value)
            return

#set $ar0=0x6000d4f0
def genRegDumpCmdTxt(reg_list):
    global reg_dump_file_name

    DUMP_FILE = open(reg_dump_file_name, "w")
    for reg in reg_list:
        DUMP_FILE.write(reg)
        DUMP_FILE.write("\n")
    DUMP_FILE.close()

#restore memdump.bin  binary 0x63fffc00
def genMEMDumpCmdTxt(addr_list):

    global mem_dump_pre
    global mem_dump_post
    global mem_dump_file_name

    DUMP_FILE = open(mem_dump_file_name, "w")
    for addr in addr_list:
        mem_file_n = mem_dump_pre + str(addr_list.index(addr)) + mem_dump_post
        content_line = "restore " + mem_file_n + " binary " + addr + "\n"
        DUMP_FILE.write(content_line)

    DUMP_FILE.close()

print "Parsing crash log in path : " + os.getcwd()
#open exception log file,multi_chip_excep.txt, excep_data3
EXCEP_FILE = open(exception_data_file,"r")

excep_data = EXCEP_FILE.read()

print "read data : \n"
excep_array = excep_data.split("\n",-1)

#parse register data
#[16:23:03]reg r1 = 0x10149297
is_begin_for_register = 0
last_mem_addre = 0
for line in excep_array:
    if is_begin_for_register == 1:
        is_end = re.match(r'.*Register dump end\:',line)
        if is_end:
            is_begin_for_register = 0
            genRegDumpCmdTxt(register_list)
            print register_list
            continue

        matched = re.match(r'.*?([a-zA-Z]\w+) += +(0x[a-zA-Z0-9]+)', line)
        if matched:
            #        print "matched : " + matched.group()
            #print "matched 1 : " + matched.group(1)
            #print "matched 2 : " + matched.group(2)
            addOneRegisterValue(matched.group(1), matched.group(2))
        continue

    if is_begin_for_register==0:
        is_begin = re.match(r'.*Register dump begin',line)
        if is_begin:
            is_begin_for_register = 1
            continue

    data_matched = re.match(r'.*?(0x[a-zA-Z0-9]+): +([a-zA-Z0-9]+) +([a-zA-Z0-9]+) +([a-zA-Z0-9]+) +([a-zA-Z0-9]+)',line)

    if data_matched:
        #print "matched Data : " + data_matched.group(1) + " " + data_matched.group(2) + " " + data_matched.group(3) \
        #      + " " + data_matched.group(4) + " " + data_matched.group(5)

        cur_addr = int(data_matched.group(1)[2:10], 16)
        #print "cur_addr = 0x%x , str = %s" % (cur_addr,data_matched.group(1)[2:10])
        if last_mem_addre == 0:
            dumpfile_name = mem_dump_pre + '0' + mem_dump_post
            if (os.path.exists(dumpfile_name)):
                os.remove(dumpfile_name)
            dumpdata_file = open(dumpfile_name, "wb")
            mem_dump_region_list.append(data_matched.group(1))
        else:
            if (cur_addr - last_mem_addre != 16):
                dumpdata_file.close()
                dumpfile_name = mem_dump_pre + str(mem_dump_region_list.__len__()) + mem_dump_post
                if (os.path.exists(dumpfile_name)):
                    os.remove(dumpfile_name)
                dumpdata_file = open(dumpfile_name, "wb")
                mem_dump_region_list.append(data_matched.group(1))
            #elif (cur_addr - last_mem_addre < 4):
            #    raise Exception("MEM DATA in log file has a fetal error, err id : -10001")

        last_mem_addre = cur_addr

        for i in range(2,data_matched.groups().__len__() + 1):
            #print "matched : " + data_matched.group(i)
            count = 7
            right_value =""
            while (count>0):
                right_value += data_matched.group(i)[count-1] + data_matched.group(i)[count]
                count -= 2
            dumpdata_file.write(binascii.a2b_hex(right_value))
    else:
        pass

dumpdata_file.close()
genMEMDumpCmdTxt(mem_dump_region_list)

print mem_dump_region_list

EXCEP_FILE.close()
