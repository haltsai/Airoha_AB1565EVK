ifeq ($(CCCI_ENABLE), y)
CCNI_PATH = kernel/service/ccci

C_SRC  += $(CCNI_PATH)/src/ccci.c

CCFLAG += -DCCCI_ENABLE
ASFLAG += -DCCCI_ENABLE
CCFLAG += -DCCCI_SUPPORT_NON_BLOCKING_SEND
#################################################################################
#include path
INC  += kernel/service/ccci/inc
endif
