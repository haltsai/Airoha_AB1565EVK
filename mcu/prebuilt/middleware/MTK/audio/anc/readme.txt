ANC module usage guide

Brief:          This module is the Active Noise Cancellation (ANC) implementation. It provides support for noise cancellation.

Usage:          GCC:  For ANC,
                       1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR))/prebuilt/middleware/MTK/audio/anc/module.mk
                       2) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/prebuilt/middleware/MTK/audio/anc/inc

Dependency:      N/A

Notice:          N/A

Relative doc:    N/A

Example project: N/A
