DSP1_PRE_LOAD_LIB_DEMO = kernel/service/pre_libloader/dsp1/dsp1_pic_demo_portable

ifeq ($(PRELOADER_ENABLE),y)
ifeq ($(PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1),y)
ifeq ($(DSP1_PISPLIT_DEMO_LIBRARY),y)

#CCFLAG += -DDSP1_PISPLIT_DEMO_LIBRARY
C_SRC  += $(DSP1_PRE_LOAD_LIB_DEMO)/dsp1_pisplit_demo_library_portable.c

endif
endif
endif
#################################################################################
#include path
INC  += kernel/service/pre_libloader/dsp1/dsp1_pic_demo_portable

