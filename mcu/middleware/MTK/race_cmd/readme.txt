Race cmd module usage guide

Brief:          This module is the Race command interface.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/MTK/race_cmd/module.mk" in your GCC project Makefile.
                KEIL: Drag the middleware/MTK/race_cmd folder to your project. Add middleware/MTK/race_cmd/inc to include paths.
                IAR: Drag the middleware/MTK/race_cmd folder to your project. Add middleware/MTK/race_cmd/inc to include paths.
Dependency:     Please define HAL_UART_MODULE_ENABLED in the hal_feature_config.h under the project inc folder.  
Notice:         To enable the Mediatek default Race command provided under middleware/MTK/race_cmd folder, please define MTK_RACE_CMD_ENABLE.  
Relative doc:   Please refer to the API reference under the doc folder for more detail.
Example project:Please find the project under project folder with fota_ prefix.
