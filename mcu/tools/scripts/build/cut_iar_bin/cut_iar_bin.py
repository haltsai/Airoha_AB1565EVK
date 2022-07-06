import sys
import os

def show_usage():
    print("cut_iar_bin: Cut sub-bin from IAR single full bin, v1.0.0")
    print("Usage:")
    print("cut_iar_bin <chip_type> <iar_bin_path> [offset] [length]")
    print("    Example: python3 <cut_iar_bin.py> 7686 <iar_bin_path>, get CM4 main bin as OTA image for default memory layout")
    print("    Example: python3 <cut_iar_bin.py> 7687 <iar_bin_path> 0 0x8000")

def check_chip_valid(chip_name):
    if chip_name.find("7698") >= 0 \
        or chip_name.find("7682") >= 0 or chip_name.find("7686") >= 0 \
        or chip_name.find("7687") >= 0 or chip_name.find("7697") >= 0 \
        or chip_name.find("2523") >= 0 or chip_name.find("2533") >= 0:
        return True
    else:
        return False

def get_chip_type(chip_name):
    if chip_name.find("7698") >= 0:
        return "7698"
    elif chip_name.find("7682") >= 0:
        return "7682"
    elif chip_name.find("7686") >= 0:
        return "7686"
    elif chip_name.find("7687") >= 0:
        return "7687"
    elif chip_name.find("7697") >= 0:
        return "7697"
    elif chip_name.find("2523") >= 0:
        return "2523"
    elif chip_name.find("2533") >= 0:
        return "2533"
    else:
        return ""

def get_offset_length_by_chip_type(chip_type):
    offset = 0
    length = 0
    if chip_type == "7698":
        # H1(0x0000->0x1000)? + H2(0x1000->0x1000)? + BL(0x2000->0x10000) + CM4(0x12000->0x24A000)
        offset = 0x10000
        length = 0x24A000
    elif chip_type == "7682m" or chip_type == "7682" or chip_type == "7686":
        # 7682m BL(0x2000->0x10000) + CM4(0x12000->0x121000)
        # 7682 BL(0x2000->0x10000) + CM4(0x12000->0x86000)
        # 7686 BL(0x2000->0x10000) + CM4(0x12000->0x24A000)
        offset = 0x10000
        if (chip_type == "7682m"):
            length = 0x121000
        elif (chip_type == "7682"):
            length = 0x86000
        else:
            length = 0x24A000
    elif chip_type == "7687" or chip_type == "7697":
        # 7687 BL(0x0000->0x8000) + RESERVED(0x8000->0x8000) + N9(0x10000->0x69000) + CM4(0x79000->0xBF000)
        # 7687 BL(0x0000->0x8000) + RESERVED(0x8000->0x8000) + N9(0x10000->0x69000) + CM4(0x79000->0x1ED000)
        offset = 0x79000
        if (chip_type == "7687"):
            length = 0xBF000
        elif (chip_type == "7697"):
            length = 0x1ED000
    elif chip_type == "2523" or chip_type == "2533":
        # 2523 BL(0x0000->0x10000) + CM4(0x10000->0x250000)
        offset = 0x10000
        length = 0x250000
    return [offset, length]

def check_and_handle_argv(argv):
    result = True
    arg_num = len(argv)
    chip_name = ""
    iar_bin_path = ""
    offset = -1
    length = -1
    if arg_num == 1:
        show_usage()
        result = False
    elif arg_num == 2:
        print("Error: invalid single arg")
        result = False

        # Add to post-build CMD Line in IAR IDE
        # check_chip_name = False
        # check_iar_bin_path = False
        # single_arg = argv[1]
        # print("Single Arg: {0}".format(argv[1]))
        # if (single_arg.find("_hdk") >= 0 or single_arg.find("_evk") >= 0) \
        #     and single_arg.find("Exe") >= 0 \
        #     and single_arg.find("out_iar") >= 0:
        #     arg_array = single_arg.split("\\")
        #     for arg in arg_array:
        #         if (arg.find("_hdk") >= 0 or arg.find("_evk") >= 0):
        #             chip_name = arg
        #             check_chip_name = True
        #             break
        #     iar_bin_path = single_arg + ".bin"
        #     if (os.path.exists(iar_bin_path)):
        #         check_iar_bin_path = True
        #     else:
        #         print("Error: iar_bin file not exist {0}".format(iar_bin_path))
        #     if (check_chip_name and check_iar_bin_path):
        #         result = True
        # else:
        #     print("Error: invalid single_arg {0}".format(single_arg))
        #     result = False
    elif arg_num == 3 or arg_num == 5:
        chip_name = argv[1]
        iar_bin_path = argv[2]
        if (not check_chip_valid(chip_name)):
            print("Error: can't support the chip {0}".format(chip_name))
            result = False
        elif (not iar_bin_path.endswith(".bin")):
            print("Error: invalid suffix name, <iar_bin> must be '.bin' file")
            result = False
        elif not os.path.exists(iar_bin_path):
            print("Error: iar_bin file not exist")
            result = False
        if arg_num == 5:
            offset = argv[3]
            length = argv[4]
            try:
                if (offset.startswith("0x")):
                    offset = int(offset, 16)
                else:
                    offset = int(offset, 10)
                if (length.startswith("0x")):
                    length = int(length, 16)
                else:
                    length = int(length, 10)
            except BaseException:
                print("Error: invalid offset({0}) or length({1})".format(offset, length))
                result = False
            else:
                if offset < 0:
                    print("Error: invalid offset({0})".format(offset))
                    result = False
                elif length <= 0:
                    print("Error: invalid length({0})".format(length))
                    result = False
    else:
        print("Error: please check parameter number")
        result = False
    chip_type = get_chip_type(chip_name)
    return [result, chip_type, iar_bin_path, offset, length]

# main
[result, chip_type, iar_bin_path, offset, length] = check_and_handle_argv(sys.argv)
if (result):
    if (offset == -1 and length == -1):
        # Get [offset, length] according to chip_type
        [offset, length] = get_offset_length_by_chip_type(chip_type)
    print("chip_type={0} iar_bin_path={1} offset={2} length={3}".format(chip_type, iar_bin_path, offset, length))
    # Get real length
    iar_bin_size = os.path.getsize(iar_bin_path)
    if (iar_bin_size <= 0 or iar_bin_size <= offset):
        print("Error: invalid iar_bin size")
    else:
        remain_bin_size = iar_bin_size - offset
        if (remain_bin_size < length):
            length = remain_bin_size
        print("iar_bin_size={0} remain={1} real length={2}".format(iar_bin_size, remain_bin_size, length))
    # Open & Read file
    iar_file_in = open(iar_bin_path,'rb')
    iar_file_in.seek(offset, 0)
    inbytes = iar_file_in.read(length)
    iar_file_in.close()
    # Create & Write file
    cur_dir = os.path.dirname(iar_bin_path)
    output_bin = cur_dir + "\\output.bin"
    iar_file_out = open(output_bin,'wb')
    iar_file_out.write(inbytes)
    iar_file_out.close()
    if (os.path.getsize(output_bin) == length):
        print("PASS: {0}".format(output_bin))