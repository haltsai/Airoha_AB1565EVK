OPUS encoder prebuilt module usage guide

Brief:          This module contains the third party library for OPUS encoder.

Usage:          GCC:  For OPUS encoder prebuilt module, make sure to include the following:
                      1) Add the following module.mk for libs and source file:
                         include $(ROOTDIR)/middleware/third_party/dspalg/opus_encoder/module.mk
                      2) Module.mk provides different options to enable or disable according to the profiles.
                         Please configure the related options on the specified XT-XCC/feature.mk.
                         MTK_RECORD_OPUS_ENABLE = y

Dependency:     OPUS encoder prebuilt module is only effective when OPUS encoder module also exists.

Notice:         1) MTK_RECORD_OPUS_ENABLE must be set as "y" on the specified DSP side XT-XCC/feature.mk.

Relative doc:   None

Example project:None