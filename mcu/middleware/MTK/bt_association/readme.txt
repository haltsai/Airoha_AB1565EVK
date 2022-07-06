AMA module usage guide
Brief:           This module is for middleware bt association implementation.
                 Bt association is used to exchange the information by BLE connetion between the
                 Agent and the Client/Partner before the AWS link was established.The information
                 includes Address,AWS Secret key and so on.
Usage:           GCC:  For BT Association,
                       1) Add the following module.mk for libs and source file:
                         include $(SOURCE_DIR))/middleware/MTK/bt_association/module.mk
                 KEIL: No support.
                 IAR:  No support.
Dependency:      N/A
Notice:          N/A
Relative doc:    N/A
Example project: mcu/project/ab155x_evk/apps/speaker_ref_design
