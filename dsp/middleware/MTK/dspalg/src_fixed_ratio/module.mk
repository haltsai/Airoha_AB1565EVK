
###################################################
SRC_FIXED_RATIO_PATH = middleware/MTK/dspalg/src_fixed_ratio

C_SRC += $(SRC_FIXED_RATIO_PATH)/src/src_fixed_ratio_interface.c
LIBS += $(strip $(LIBDIR2))/src_fixed_ratio/libsampler_by_n.a

###################################################
# include path

INC += $(SRC_FIXED_RATIO_PATH)/inc