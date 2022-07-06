
###################################################
AUDIO_PLC_PATH = middleware/third_party/dspalg/audio_plc
ifeq ($(MTK_AUDIO_PLC_ENABLE), y)
    C_SRC += $(AUDIO_PLC_PATH)/src/audio_plc_interface.c
    #C_SRC += $(AUDIO_PLC_PATH)/src/celt_lpc.c
    #C_SRC += $(AUDIO_PLC_PATH)/src/mathops.c
    #C_SRC += $(AUDIO_PLC_PATH)/src/pitch.c
    #C_SRC += $(AUDIO_PLC_PATH)/src/plc.c
    #C_SRC += $(AUDIO_PLC_PATH)/src/xt_memory.c
ifneq ($(MTK_AUDIO_PLC_USE_PIC),y)
    LIBS += $(strip $(LIBDIR3))/audio_plc/$(IC_CONFIG)/libplc.a
else
    #PIC     += $(strip $(LIBDIR3))/Audio_PLC/pisplit/pisplit_audio_plc.o
    #C_SRC += $(AUDIO_PLC_PATH)/portable/audio_plc_portable.c
endif
endif

###################################################
# include path

ifeq ($(MTK_AUDIO_PLC_ENABLE), y)
    INC += $(AUDIO_PLC_PATH)/inc
    #INC += $(AUDIO_PLC_PATH)/portable
endif