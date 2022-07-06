
###################################################
SBC_CODEC_PATH = middleware/third_party/dspalg/sbc_decoder

#ifeq ($(MTK_BT_A2DP_SBC_ENABLE), y)
    C_SRC += $(SBC_CODEC_PATH)/src/sbc_tables.c
    C_SRC += $(SBC_CODEC_PATH)/src/sbc_interface.c
    C_SRC += $(SBC_CODEC_PATH)/src/sbc_header_parse.c
    C_SRC += $(SBC_CODEC_PATH)/src/sbc_dec_api.c

    LIBS += $(strip $(LIBDIR3))/sbc_decoder/$(IC_CONFIG)/libSBCdec_155x.a
    
 ifneq ($(MTK_BT_A2DP_SBC_USE_PIC),y)
    LIBS += $(strip $(LIBDIR3))/sbc_decoder/$(IC_CONFIG)/libxa_sbc_dec.a
else
	PIC     += $(strip $(LIBDIR3))/sbc_decoder/$(IC_CONFIG)/pisplit/pisplit_xa_sbc_dec.o
   	C_SRC += $(SBC_CODEC_PATH)/portable/sbc_decoder_portable.c
endif

#endif

###################################################
# include path

#ifeq ($(MTK_BT_A2DP_SBC_ENABLE), y)
    INC += $(SBC_CODEC_PATH)/inc
	INC += $(SBC_CODEC_PATH)/portable
#endif
