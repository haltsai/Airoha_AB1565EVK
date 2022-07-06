
###################################################
AAC_CODEC_PATH = middleware/third_party/dspalg/aac_decoder

ifeq ($(MTK_BT_A2DP_AAC_ENABLE), y)
    C_SRC += $(AAC_CODEC_PATH)/src/aac_api.c
    C_SRC += $(AAC_CODEC_PATH)/src/aac_dec_interface.c
    
ifneq ($(MTK_BT_A2DP_AAC_USE_PIC),y)
    LIBS += $(strip $(LIBDIR3))/aac_decoder/$(IC_CONFIG)/libxa_aac_loas_dec.a
else
	PIC     += $(strip $(LIBDIR3))/aac_decoder/$(IC_CONFIG)/pisplit/pisplit_xa_aac_dec.o
   	C_SRC += $(AAC_CODEC_PATH)/portable/aac_decoder_portable.c
   
endif

endif

###################################################
# include path

ifeq ($(MTK_BT_A2DP_AAC_ENABLE), y)
    INC += $(AAC_CODEC_PATH)/inc
    INC += $(AAC_CODEC_PATH)/portable
endif
