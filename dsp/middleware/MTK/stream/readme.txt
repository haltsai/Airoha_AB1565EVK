verno module usage guide

Brief:          This module is used to create a stream driver of the DSP & describe stream interface of audio streaming usage.

Usage:          XT-XCC:  For streaming, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(ROOTDIR)/middleware/MTK/stream/module.mk
                      2) Module.mk provides different options to enable or disable according profiles, please configure these options on the specified XT-XCC/feature.mk:
                         None
                      3) Add the header file path:
                         CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/stream/inc
                      4) Please call API in stream.h to create the source and the sink, Such as StreamAudioAfeSource() & StreamAudioAfeSink().

Dependency:     None

Notice:         None

Relative doc:   None

Example project: None
