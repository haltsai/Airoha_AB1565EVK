DSP0_LOAD_DSP1_PRE_LOAD_LIB_DEMO = kernel/service/pre_libloader/dsp0/dsp0_load_dsp1_pic_demo_portable

ifeq ($(PRELOADER_ENABLE),y)
ifeq ($(PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1),y)
ifeq ($(DSP1_PISPLIT_DEMO_LIBRARY),y)
   #if enable DSP1_PISPLIT_DEMO_LIBRARY, should extern defined "dsp1_pisplit_library_build_demo" in preloader_pisplit_configure.h, 
   #and add "dsp1_pisplit_library_build_demo" to "PIC_LIB_LIST"
   
CCFLAG += -DDSP1_PISPLIT_DEMO_LIBRARY
C_SRC  += $(DSP0_LOAD_DSP1_PRE_LOAD_LIB_DEMO)/dsp0_load_dsp1_pisplit_demo_library_portable.c
PIC     += $(ROOTDIR)/project/ab1558_evk/templates/dsp_pisplit_library_build_demo/dsp1/Build/lib/dsp1_pisplit_library_build_demo.o
endif
endif

endif
#################################################################################
#include path
INC  += kernel/service/pre_libloader/dsp0/dsp0_load_dsp1_pic_demo_portable

