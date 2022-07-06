PRE_LOAD_LIB = kernel/service/pre_libloader

ifeq ($(PRELOADER_ENABLE),y)
C_SRC  += $(PRE_LOAD_LIB)/dsp0/src/pi_library_load.c
C_SRC  += $(PRE_LOAD_LIB)/dsp0/src/pi_library_symbols.c
C_SRC  += $(PRE_LOAD_LIB)/dsp0/src/pi_relocate_lib.c
C_SRC  += $(PRE_LOAD_LIB)/dsp0/src/preloader_pisplit.c
C_SRC  += $(PRE_LOAD_LIB)/dsp0/src/preloader_pisplit_internal.c
C_SRC  += $(PRE_LOAD_LIB)/dsp0/src/common.c

ifeq ($(DSP0_PISPLIT_DEMO_LIBRARY),y)
include $(ROOTDIR)/kernel/service/pre_libloader/dsp0/dsp0_pic_demo_portable/module.mk
endif

ifeq ($(PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1),y)
ifeq ($(DSP1_PISPLIT_DEMO_LIBRARY),y)
include $(ROOTDIR)/kernel/service/pre_libloader/dsp0/dsp0_load_dsp1_pic_demo_portable/module.mk
endif
endif

endif


#################################################################################
#include path
INC  += kernel/service/pre_libloader/inc
INC  += kernel/service/pre_libloader/dsp0/inc

