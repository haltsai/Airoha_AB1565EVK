opus encoder module usage guide

Brief:          This opus encoder support for 16K sample rate,mono, frame duration of 20 ms .

Usage:          GCC:  For opus encoder, include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(ROOTDIR)/middleware/third_party/dspalg/opus_encoder/module.mk
                      2) Module.mk provide different options to enable or disable according profiles, please configure these options on specified GCC/feature.mk:
                         MTK_RECORD_OPUS_ENABLE
                      3) Add the module.mk path to the [project]/XT-XCC/Makefile
                         # audio middleware files
                         include $(ROOTDIR)/middleware/third_party/dspalg/opus_encoder/module.mk
                         in your XT-XCC project Makefile.

Dependency:     none.

Notice:         none.

Relative doc:   none.

Example project:none.
