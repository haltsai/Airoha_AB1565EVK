
Airoha IoT Development Platform provides a comprehensive software solution for devices based
on the chips.

Chips of each product line are shown as below
    • Airoha IoT SDK for Smart MCU: AM255x
    • Airoha IoT SDK for BT Audio: AB155x/AB1565/AB1568

1. Getting started
   Follow the instructions at <sdk_root>/dsp/doc/Airoha_IoT_SDK_DSP_Get_Started_Guide.pdf
   to use Xtensa development tools to build your first project.

2. Folder structure
  \config                   ->  Includes make and compile configuration files for compiling a binary project.
  \doc                      ->  Includes SDK related documentation, such as developer and SDK API reference guides.
  \driver                   ->  Includes common driver files, such as board drivers and peripheral interface drivers.
  \kernel                   ->  Includes the underlying RTOS and system services for exception handling and error logging.
  \middleware\MTK           ->  Includes middleware files created by Airoha.
  \middleware\third_party   ->  Includes middleware files created by third parties. Such as dspalg.
  \prebuilt                 ->  Contains binary files, libraries, header files, makefiles and other pre-built files.
  \project\<board>          ->  The SDK includes example projects with pre-configured module features.
  \tools                    ->  Includes tools to compile and build projects using the SDK.
