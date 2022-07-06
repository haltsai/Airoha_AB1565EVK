Audio PLC module usage guide

Brief:          This audio plc support for audio packet loss concealment.

Usage:          GCC:  For audio plc, include the module with
                      1) Add the following module.mk for libs and source file:
                         include $(ROOTDIR)/middleware/third_party/dspalg/audio_plc/module.mk
                      2) Module.mk provide different options to enable or disable according profiles, please configure these options on specified GCC/feature.mk:
                         MTK_AUDIO_PLC_ENABLE
                      3) Add the module.mk path to the [project]/XT-XCC/Makefile
                         # audio middleware files
                         include $(ROOTDIR)/middleware/third_party/dspalg/audio_plc/module.mk
                         in your XT-XCC project Makefile.

Dependency:     none.

Notice:         none.

Relative doc:   none.

Example project:none.
