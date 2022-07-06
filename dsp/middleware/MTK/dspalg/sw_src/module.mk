
###################################################
SW_SRC_PATH = middleware/MTK/dspalg/sw_src
#ifeq ($(AIR_SOFTWARE_SRC_ENABLE), y)
C_SRC += $(SW_SRC_PATH)/src/sw_src_interface.c
LIBS += $(strip $(LIBDIR2))/sw_src/ab156x/libswsrc_1565.a
#endif

###################################################
# include path

INC += $(SW_SRC_PATH)/inc