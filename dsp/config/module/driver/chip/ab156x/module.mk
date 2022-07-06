include $(ROOTDIR)/driver/chip/ab156x/src/XT-XCC/module.mk

include $(ROOTDIR)/driver/board/$(BOARD_CONFIG)/hw_resource_assignment/module.mk

ifneq ($(wildcard $(strip $(ROOTDIR))/driver/chip/ab156x/src_core/),)
include $(ROOTDIR)/driver/chip/ab156x/src_core/XT-XCC/module.mk
else
LIBS += $(ROOTDIR)/prebuilt/driver/chip/ab156x/lib/libhal_core_CM4_GCC.a
endif

#ifneq ($(wildcard $(strip $(ROOTDIR))/driver/chip/ab156x/src_protected/),)
#include $(ROOTDIR)/driver/chip/ab156x/src_protected/XT-XCC/module.mk
#else
#LIBS += $(ROOTDIR)/prebuilt/driver/chip/ab156x/lib/libhal_protected_CM4_GCC.a
#endif
