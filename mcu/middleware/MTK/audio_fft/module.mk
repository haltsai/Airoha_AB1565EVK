
AUDIO_FFT_SRC =  middleware/MTK/audio_fft

C_FILES  += $(AUDIO_FFT_SRC)/src/Audio_FFT.c

#################################################################################
#include path
CFLAGS     += -I$(SOURCE_DIR)/middleware/MTK/audio_fft/inc
