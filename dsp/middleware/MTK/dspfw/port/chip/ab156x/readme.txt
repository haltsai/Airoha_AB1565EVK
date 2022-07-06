dspfw module usage guide

Brief:          This module contains the major part of DSP middleware streaming callback driver and the audio input/output driver.

Usage:          XT-XCC:  For stream, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(ROOTDIR)/middleware/MTK/dspfw/port/chip/ab156x/module.mk
                      2) Module.mk provides different options to enable or disable according profiles, please configure these options on the specified XT-XCC/feature.mk:
                         None
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/dspfw/port/chip/ab156x/inc
                      4) The dspfw module is the major part of DSP middleware. It must exist to enable audio streamming functionally.


Dependency:     None

Notice:         None

Relative doc:   None

Example project: None
