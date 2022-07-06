/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

/**
 * @addtogroup ab1565_ab1568 ab1565_ab1568
 * @{
 * @addtogroup ab1565_ab1568_apps apps
 * @{
 * @addtogroup ab1565_ab1568_headset_ref_design headset_ref_design
 * @{

@par Overview
  - Application description
    - This application demonstrates headset features based on AB1565_AB1568 for the production of
      headset.
  - Application features
    - 1. User Interface, such as LEDs for indication, keys for function control,
         and voice prompts.
    - 2. Call management, such as answering or rejecting incoming calls, redialing on the
         smartphone, hanging up or holding an active call, switching audio paths, handling
         a 3-way call.
    - 3. Bluetooth music. Streaming music from an audio device. Streaming controls include:
         playing or pausing the music; moving to the previous or next track. For more
	 details on AWS, please contact Airoha customer support.

@par Hardware and software environment
   - Supported platform
    - AB1565_AB1568 evaluation Kit (EVK).

@par Directory contents
  - Source and header files
    - \b inc:                   Common header files.
    - \b inc/boards:            header files for board.
    - \b inc/apps:              Header files of demo APPs.
    - \b src:                   Common files for system start.
    - \b src/boards:            Common source files for board.
    - \b src/apps:               Source code of demo APPs
  - Project configuration files using GCC
    - \b GCC/feature.mk:        The feature configuration file.
    - \b GCC/feature_"board".mk:The feature configuration file for a different board.
    - \b Makefile:              The Makefile.
    - \b GCC/ab156x_flash.ld:   The linker script.
    - \b GCC/syscalls.c:        The syscalls for GCC.
    - \b GCC/startup_ab156x.s:  The startup file for GCC.

@par Run the application
  - How to build the headset_ref_design application
  - GCC version
    - make command "./build.sh ab1565_ab1568_evk headset_ref_design -f=feature_<board>.mk"
      under the SDK/mcu root folder.
    - make command "./build.sh ab1565_ab1568_evk headset_ref_design -fm=feature_<board>.mk
      -fd0=feature_<board>.mk" under the SDK root folder.
  - How to download the headset_ref_design application
    - Install and use Flash Tool ("sdk_root/tools/PC_tool_Win.zip") to download the
      application binary headset_ref_design.bin.
  - How to run each feature
    - Power on the board and use the smartphone to search for and connect to the BT device.
      Then use BT device as a headset.

@par Troubleshooting
  - Use AB1565_AB1568_Airoha_Tool_Kit(ATK)_vx.x.x.7z to connect to to the board and receive the logs.

@par Contact information
Please report bugs or submit any questions to the Airoha AE Team.
*/
/**
 * @}
 * @}
 * @}
 * @}
 */
