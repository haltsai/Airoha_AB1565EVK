
###################################################
AAC_CODEC_PATH = middleware/MTK/dspalg/aac_decoder

ifeq ($(MTK_BT_A2DP_AAC_ENABLE), y)
    #C_SRC += $(AAC_CODEC_PATH)/src/aac_api.c
    C_SRC += $(AAC_CODEC_PATH)/src/aac_dec_interface.c
    
ifeq ($(MTK_BT_A2DP_AAC_USE_PIC),y)
    PIC   += $(strip $(LIBDIR2))/aac_decoder/$(IC_CONFIG)/pisplit_AAC_dec_5x.o
    C_SRC += $(AAC_CODEC_PATH)/portable/aac_decoder_portable.c
else
    LIBS += $(strip $(LIBDIR2))/aac_decoder/$(IC_CONFIG)/libAACdec_68.a
	#LIBS += $(strip $(LIBDIR2))/aac_decoder/libxa_aac_loas_dec.a
endif

endif

###################################################
# include path

ifeq ($(MTK_BT_A2DP_AAC_ENABLE), y)
    INC += $(AAC_CODEC_PATH)/inc
ifeq ($(MTK_BT_A2DP_AAC_USE_PIC),y)
	INC += $(AAC_CODEC_PATH)/portable
endif
endif
