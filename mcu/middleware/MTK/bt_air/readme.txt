bt_air module usage guide

Brief:          This module is the BT AIR Service implementation. It includes 3 modules (BLE AIR, SPP AIR, AirUpdate). It can help the AIR application communicate with peer device
                by low-energy link or Bluetooth SPP profile or AirUpdate Fixed channel. This module manages all user's registered callbacks and notify
                all users when RX data event occurs from the peer device.
Usage:          GCC: 1. Include the module with "include $(SOURCE_DIR)/$(MIDDLEWARE_PROPRIETARY)/bt_air/module.mk" in the GCC project Makefile
                KEIL: 1. Drag the $(MIDDLEWARE_PROPRIETARY)/bt_air folder to the project.
                      2. Add $(MIDDLEWARE_PROPRIETARY)/bt_air/inc to include paths.
                IAR: 1. Drag the $(MIDDLEWARE_PROPRIETARY)/bt_air folder to the project.
                     2. Add $(MIDDLEWARE_PROPRIETARY)/bt_air/inc to include paths.
Dependency:     This module depends on Bluetooth. Please set MTK_BT_ENABLE to be "y" in feature.mk and include Bluetooth module.mk in your project.
Notice:         None.
Relative doc:   None.
Example project:project/mt2811s_evb/apps/headset_ref_design


