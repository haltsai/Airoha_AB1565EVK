
###################################################
SBC_CODEC_PATH = middleware/MTK/dspalg/sbc_decoder

ifeq ($(MTK_BT_A2DP_SBC_ENABLE), y)
    C_SRC += $(SBC_CODEC_PATH)/src/sbc_interface.c
    C_SRC += $(SBC_CODEC_PATH)/src/sbc_header_parse.c


    
ifneq ($(MTK_BT_A2DP_SBC_USE_PIC),y)
    LIBS += $(strip $(LIBDIR2))/sbc_decoder/$(IC_CONFIG)/libSBCdec_68.a
else
	PIC     += $(strip $(LIBDIR2))/sbc_decoder/$(IC_CONFIG)/pisplit/pisplit_sbc_dec.o
	C_SRC += $(SBC_CODEC_PATH)/portable/sbc_decoder_portable.c
endif

endif

###################################################
# include path

ifeq ($(MTK_BT_A2DP_SBC_ENABLE), y)
    INC += $(SBC_CODEC_PATH)/inc

ifeq ($(MTK_BT_A2DP_SBC_USE_PIC), y)
	INC += $(SBC_CODEC_PATH)/portable
endif

endif