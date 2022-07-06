/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
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


#ifndef __RACE_CMD_NVDM_H__
#define __RACE_CMD_NVDM_H__


#include "race_cmd_feature.h"
#ifdef RACE_NVDM_CMD_ENABLE
#include "stdint.h"
#include "race_cmd.h"


/**
 * @addtogroup NVKEY
 * @{
 */
////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/** <pre>
 * <b>[Command]</b>
 *    0x055A
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x06
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0A 0x00
 * <tr><td> NVKEY_ID                     <td> 2 byte            <td> 0xE1 0x30
 * <tr><td> Length of Read Bytes         <td> 2 byte            <td> 0x00 0x10
 * </table>
 * <b>[Field Description]</b>
 *    NVKEY read.
 * <b>[Example]</b>
 *    An example for reading the NVKEY is shown as below.
 *    0x05 0x5A 0x06 0x00 0x00 0x0A 0x30 0xE1 0x10 0x00
 * <b>[Note]</b>
 *    Length of Read Bytes is unused, and report total NVKEY data length.
 * </pre>
 */

 /** <pre>
 * <HR>
 * <b>[Response]</b>
 *    0x055B
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x08
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0A 0x00
 * <tr><td> Length Bytes                 <td> 2 byte            <td> 0x00 0x04
 * <tr><td> payload                      <td> Length Bytes      <td> 0x11 0x22 0x33 0x44
 * </table>
 * <b>[Field Description]</b>
 *    The response will report payload with expected length of data.
 * <b>[Example]</b>
 *    An example for a response with the payload is shown as below.
 *    0x05 0x5B 0x08 0x00 0x00 0x0A 0x04 0x00 0x44 0x33 0x22 0x11
 * <b>[Note]</b>
 *    None
 * </pre>
 */
#define RACE_NVKEY_READFULLKEY				0x0A00

/**
*@}
*/

/**
 * @addtogroup NVKEY
 * @{
 */
 
/** <pre>
 * <b>[Command]</b>
 *    0x055A
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x06
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0A 0x01
 * <tr><td> NVKEY_ID                     <td> 2 byte            <td> user-specified
 * <tr><td> NVKEY_VALUE			         <td> N byte            <td> depends on NVKEY_ID
 * </table>
 * <b>[Field Description]</b>
 *    NVKEY write.
 * <b>[Example]</b>
 *    An example for writing the NVKEY is shown as below.
 *    0x05 0x5A 0x05 0x00 0x01 0x0A 0x00 0x3A 0x01
 * <b>[Note]</b>
 *    
 * </pre>
 */

 /** <pre>
 * <HR>
 * <b>[Response]</b>
 *    0x055B
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00 0x03
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0A 0x01
 * <tr><td> Status		                 <td> 1 byte            <td> 0x00 for success, others for failure
 * </table>
 * <b>[Field Description]</b>
 *    The response will report NVKEY writing status.
 * <b>[Example]</b>
 *    An example for a response with succeeded status is shown as below.
 *    0x05 0x5B 0x03 0x00 0x01 0x0A 0x00
 * <b>[Note]</b>
 *    None
 * </pre>
 */
 #define RACE_NVKEY_WRITEFULLKEY				0x0A01

/**
*@}
*/

#define RACE_NVKEY_NEXT						0x0A02
#define RACE_NVKEY_RECLAIM					0x0A03
#define RACE_NVDM_GETALL					0x0A07
#define RACE_SWITCH_FUNC					0x0A08
#define RACE_RELOAD_NVKEY_TO_RAM      0x0A09
#define RACE_NVKEY_READFULLKEY_RESP_NVID	0x0A0C
#define RACE_NVKEY_WRITEFULLKEY_RESP_NVID	0x0A0D

/**
*@}
*/

#define NVDM_DATA_LENGTH 1000


////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*!
  @brief Process NVDM related RACE commands.

  @param pRaceHeaderCmd This parameter represents the raw data such as "05 5A...".
  @param Lenth Total bytes of this RACE command.
  @param channel_id Channel identifier
*/
void* RACE_CmdHandler_NVDM(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id);


#ifdef AIR_RACE_CO_SYS_ENABLE
void race_cosys_nvkey_init(void);
#endif

#endif /* RACE_NVDM_CMD_ENABLE */
#endif /* __RACE_CMD_NVDM_H__ */

