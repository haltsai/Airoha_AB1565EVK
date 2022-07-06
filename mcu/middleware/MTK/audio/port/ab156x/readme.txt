AB1565/AB1568 Audio Port initialization module usage guide

Brief:          This module is the audio middleare's chip porting layer for the AB1565/AB1568 chipsets.

Usage:          GCC:  For audio middleare's chip porting layer, include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR)/middleware/MTK/audio/port/ab156x/module.mk
                      2) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/audio/port/ab156x/inc

Dependency:     None.

Notice:         None.

Relative doc:   None.

Example project:None.
