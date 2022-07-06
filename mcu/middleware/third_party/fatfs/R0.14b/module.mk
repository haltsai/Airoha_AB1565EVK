
###################################################
# Sources
FATFS_SRC = middleware/third_party/fatfs/R0.14b/source

C_FILES  += $(FATFS_SRC)/ff.c
C_FILES  += $(FATFS_SRC)/ffunicode.c
C_FILES  += $(FATFS_SRC)/ffsystem.c

CFLAGS   += -fwide-exec-charset=UTF-16LE
CFLAGS   += -I$(SOURCE_DIR)/$(FATFS_SRC)
