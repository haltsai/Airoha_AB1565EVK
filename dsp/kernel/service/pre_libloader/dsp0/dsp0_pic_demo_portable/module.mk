DSP0_PRE_LOAD_LIB_DEMO = kernel/service/pre_libloader/dsp0/dsp0_pic_demo_portable

ifeq ($(PRELOADER_ENABLE),y)
ifeq ($(DSP0_PISPLIT_DEMO_LIBRARY),y)
   #if enable DSP0_PISPLIT_DEMO_LIBRARY, should extern defined "dsp0_pisplit_library_build_demo" in preloader_pisplit_configure.h, 
   #and add "dsp0_pisplit_library_build_demo" to "PIC_LIB_LIST"
#CCFLAG += -DDSP0_PISPLIT_DEMO_LIBRARY
#ASFLAG += -DDSP0_PISPLIT_DEMO_LIBRARY
C_SRC  += $(DSP0_PRE_LOAD_LIB_DEMO)/dsp0_pisplit_demo_library_portable.c
PIC     += $(ROOTDIR)/project/ab1558_evk/templates/dsp_pisplit_library_build_demo/dsp0/Build/lib/dsp0_pisplit_library_build_demo.o
endif
endif
#################################################################################
#include path
INC  += kernel/service/pre_libloader/dsp0/dsp0_pic_demo_portable

