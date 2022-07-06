
###################################################
# Sources
FATFS_SRC = middleware/third_party/fatfs/R0.12b

C_FILES  += $(FATFS_SRC)/src/ff.c
C_FILES  += $(FATFS_SRC)/src/option/unicode.c
C_FILES  += $(FATFS_SRC)/src/option/syscall.c

CFLAGS   += -fwide-exec-charset=UTF-16LE
CFLAGS   += -I$(SOURCE_DIR)/$(FATFS_SRC)/src
