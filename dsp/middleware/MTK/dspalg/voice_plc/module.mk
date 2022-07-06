
###################################################
PLC_FUNC_PATH = middleware/MTK/dspalg/voice_plc


C_SRC += $(PLC_FUNC_PATH)/voice_plc_interface.c

ifeq ($(AIR_VOICE_PLC_ENABLE),y)
ifneq ($(MTK_PLC_USE_PIC),y)
    LIBS += $(strip $(LIBDIR2))/voice_plc/$(IC_CONFIG)/libplc_pitch.a
else
    PIC     += $(strip $(LIBDIR2))/voice_plc/$(IC_CONFIG)/pisplit/pisplit_plc_pitch.o
    C_SRC += $(PLC_FUNC_PATH)/portable/plc_portable.c
endif
endif

###################################################
# include path


INC += $(PLC_FUNC_PATH)
INC += $(PLC_FUNC_PATH)/portable