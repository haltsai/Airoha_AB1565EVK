
###################################################
OPUS_ENCODER_PATH = middleware/third_party/dspalg/opus_encoder
ifeq ($(MTK_RECORD_OPUS_ENABLE), y)
    C_SRC += $(OPUS_ENCODER_PATH)/src/opus_encoder_interface.c
ifneq ($(MTK_RECORD_OPUS_USE_PIC),y)
    LIBS += $(strip $(LIBDIR3))/opus_encoder/$(IC_CONFIG)/libopus_enc.a
else
    PIC     += $(strip $(LIBDIR3))/opus_encoder/$(IC_CONFIG)/pisplit/pisplit_opus_enc.o
    C_SRC += $(OPUS_ENCODER_PATH)/portable/opus_enc_portable.c
endif
endif

###################################################
# include path

ifeq ($(MTK_RECORD_OPUS_ENABLE), y)
    INC += $(OPUS_ENCODER_PATH)/inc
    #INC += $(OPUS_ENCODER_PATH)/portable
endif
