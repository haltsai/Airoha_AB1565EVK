# BT sink source files
NVDM_DSP0_SRC = middleware/MTK/audio/port/ab155x

C_FILES += $(NVDM_DSP0_SRC)/src/nvdm/audio_dsp0_nvdm.c
C_FILES += $(NVDM_DSP0_SRC)/src/nvdm/sysram_nvdm.c


# Include bt sink path
CFLAGS += -I$(SOURCE_DIR)/$(NVDM_DSP0_SRC)/inc/nvdm
CFLAGS += -mno-unaligned-access


