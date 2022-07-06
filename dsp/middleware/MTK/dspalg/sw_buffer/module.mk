
###################################################
SW_BUFFER_PATH = middleware/MTK/dspalg/sw_buffer
#ifeq ($(AIR_SOFTWARE_BUFFER_ENABLE), y)
C_SRC += $(SW_BUFFER_PATH)/src/sw_buffer_interface.c
#endif

###################################################
# include path

INC += $(SW_BUFFER_PATH)/inc