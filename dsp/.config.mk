################################################################################
# Compiler and Commands Configuration
################################################################################
ifeq ($(USE_CCACHE),1)
    CCACHE    := ccache
endif

CROSS       := xt-
#           C Compiler
CC          := $(CCACHE) $(CROSS)xcc -c
#           Preprocessor
CCP         := $(CCACHE) $(CROSS)xcc -E
#           Assembler
AS          :=  $(CROSS)xcc -c
#           Archiver
AR          :=  $(CROSS)ar
SIZE        :=  $(CROSS)size
#           Linker
LD          :=  $(CROSS)ld
OBJDUMP     :=  $(CROSS)objdump
OBJCOPY     :=  $(CROSS)objcopy
NM          :=  $(CROSS)nm
READELF     :=  readelf
RM          :=  dtidel
CP          :=  xcopy /T /Y /I

