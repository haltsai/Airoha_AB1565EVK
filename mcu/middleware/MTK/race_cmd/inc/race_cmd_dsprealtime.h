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


#ifndef __RACE_CMD_DSPREALTIME_H__
#define __RACE_CMD_DSPREALTIME_H__
//#define MTK_AIRDUMP_EN_MIC_RECORD

#include "race_cmd_feature.h"
#include "race_cmd.h"
#include "bt_sink_srv_ami.h"
#ifdef RACE_DSP_REALTIME_CMD_ENABLE
#include "stdint.h"
#ifdef MTK_ANC_ENABLE
#ifdef MTK_ANC_V2
  #include "anc_control_api.h"
#else
  #include "anc_control.h"
#endif
#endif
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#endif



////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define RACE_DSPREALTIME_BEGIN_ID               0x0E01

#define RACE_DSPREALTIME_SUSPEND                0x0E01
#define RACE_DSPREALTIME_RESUME	    	        0x0E02
#define RACE_DSPREALTIME_PEQ                    0x0E03
#define RACE_DSPSOUND_EFFECT                    0x0E04
#define RACE_DSPREALTIME_GET_REFRENCE_GAIN      0x0E05
/**
 * @addtogroup DSPREALTIME
 * @{
 */

/** <pre>
 * <b>[Command]</b>
 *    0x055A
 * <table>
 * <tr><th> Type                    <th> Bytes            <th> Value
 * <tr><td> Length                  <td> 2 bytes          <td> 0x00  0x07
 * <tr><td> ID                      <td> 2 bytes          <td> 0x0E  0x06
 * <tr><td> Status                  <td> 1 bytes          <td> 0x00: success \n else: fail
 * <tr><td> Event                   <td> 1 bytes          <td> 0x0A: enable anc \n 0x0B: disable anc \n 0x0C: set anc gain \n 0x0D: read anc calibration value in nvkey
 * <tr><td> Filter index            <td> 1 bytes          <td> 0x0x: anc filter id
 * <tr><td> ANC type                <td> 1 bytes          <td> 0x00: hybrid anc \n 0x01: FeedForward anc only \n 0x02: FeedBack anc only
 * <tr><td> Sync mode               <td> 1 bytes          <td> 0x00: enable /disable anc only in agent \n 0x01: enable /disable anc only in agent & partner
 * </table>
 * <b>[Field Description]</b>
 *    Trigger ANC enable/disable
 * <b>[Example]</b>
 *    An Example for enabling ANC
 *    0x05 0x5A 0x07 0x00 0x06 0x0E 0x00 0x0A 0x01 0x00 0x00
 * <b>[Note]</b>
 *    None
 * </pre>
 */

 /** <pre>
 * <HR>
 * <b>[Response]</b>
 *    0x055B
 * <table>
 * <tr><th> Type                    <th> Bytes            <th> Value
 * <tr><td> Length                  <td> 2 bytes          <td> 0x00  0x08
 * <tr><td> ID                      <td> 2 bytes          <td> 0x0E  0x06
 * <tr><td> Status                  <td> 1 bytes          <td> 0x00: success \n else: fail
 * <tr><td> Event                   <td> 1 bytes          <td> 0x0A: enable anc \n 0x0B: disable anc \n 0x0C: set anc gain \n 0x0D: read anc calibration value in nvkey
 * <tr><td> Filter index            <td> 1 bytes          <td> 0x0x: anc filter id
 * <tr><td> ANC type                <td> 1 bytes          <td> 0x00: hybrid anc \n 0x01: FeedForward anc only \n 0x02: FeedBack anc only
 * <tr><td> Sync mode               <td> 1 bytes          <td> 0x00: enable /disable anc only in agent \n 0x01: enable /disable anc only in agent & partner
 * <tr><td> Reserved                <td> 1 bytes          <td> Reserved byte
 * </table>
 * <b>[Field Description]</b>
 *    The status has different meaning according to the mode.
 * <b>[Example]</b>
 *    An example for a response with the status of 0x00 is shown as below.
 *    0x05 0x5B 0x08 0x00 0x06 0x0E 0x00 0x0A 0x01 0x00 0x00 0x00
 * <b>[Note]</b>
 *    None
 * </pre>
 */
#define RACE_DSPREALTIME_ANC                    0x0E06
/**
*@}
*/
#define RACE_DSPREALTIME_AIRDUMP_ENTRY          0x0E0A
#define RACE_DSPREALTIME_AIRDUMP_ON_OFF         0x0E0B
/**
 * @addtogroup DSPREALTIME
 * @{
 */

/** <pre>
 * <b>[Command]</b>
 *    0x055A
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00  0x03
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0E  0x0C
 * <tr><td> Swap Mic                     <td> 1 bytes           <td> 0x00: Mic0 \n 0x01: Mic1 \n 0x02: Mic2 \n 0x03: Mic3 \n 0x04: Mic4 \n 0x05: Mic5
 * </table>
 * <b>[Field Description]</b>
 *    Mic Swap
 * <b>[Example]</b>
 *    An Example for Swap Mic 0
 *    0x05 0x5A 0x03 0x00 0x0C 0x0E 0x00
 * <b>[Note]</b>
 *    None
 * </pre>
 */

 /** <pre>
 * <HR>
 * <b>[Response]</b>
 *    0x055B
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00  0x03
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0E  0x0C
 * <tr><td> Status                       <td> 1 bytes           <td> 0x00: Success \n Else: Fail
 * </table>
 * <b>[Field Description]</b>
 *    The status has different meaning according to the mode.
 * <b>[Example]</b>
 *    An example for a response with the status of 0x00 is shown as below.
 *    0x05 0x5B 0x03 0x00 0x0C 0x0E 0x00
 * <b>[Note]</b>
 *    None
 * </pre>
 */
#define RACE_DSPREALTIME_2MIC_SWAP              0x0E0C
/**
*@}
*/

/**
 * @addtogroup DSPREALTIME
 * @{
 */

/** <pre>
 * <b>[Command]</b>
 *    0x055A
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00  0x03
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0E  0x0D
 * <tr><td> ECNR enable / disable        <td> 1 bytes           <td> 0x1:On  0x0:Off
 * </table>
 * <b>[Field Description]</b>
 *    ECNR real-time enable / disable
 * <b>[Example]</b>
 *    An Example for Enable ECNR
 *    0x05 0x5A 0x03 0x00 0x0D 0x0E 0x01
 *    An Example for Disable ECNR
 *    0x05 0x5A 0x03 0x00 0x0D 0x0E 0x00
 * <b>[Note]</b>
 *    None
 * </pre>
 */

 /** <pre>
 * <HR>
 * <b>[Response]</b>
 *    0x055B
 * <table>
 * <tr><th> Type                         <th> Bytes             <th> Value
 * <tr><td> Length                       <td> 2 bytes           <td> 0x00  0x03
 * <tr><td> ID                           <td> 2 bytes           <td> 0x0E  0x0D
 * <tr><td> Status                       <td> 1 bytes           <td> 0x00: Success \n Else: Fail
 * </table>
 * <b>[Field Description]</b>
 *    The status to show Success or Fail.
 * <b>[Example]</b>
 *    An example for a response with the status of success (0x00) is shown as below.
 *    0x05 0x5B 0x03 0x00 0x0D 0x0E 0x00
 * <b>[Note]</b>
 *    None
 * </pre>
 */
#define RACE_DSPREALTIME_AECNR_ON_OFF           0x0E0D
/**
*@}
*/

#define RACE_DSPREALTIME_PASS_THROUGH_ON_OFF    0x0E0E
#define RACE_DSPREALTIME_PASS_THROUGH_TEST      0x0E0F
#define RACE_DSPREALTIME_SWGAIN_BYPASS          0x0E10
#define RACE_DSPREALTIME_PASS_THROUGH_TEST_MUTE 0x0E11
#define RACE_DSPREALTIME_OPEN_ALL_MIC           0x0E12

#define RACE_DSPREALTIME_OPEN_ALL_MIC_EXTEND    0x0E20

#define RACE_DSPREALTIME_QUERY_LIB_VERSION      0x0E21

/**
 * @addtogroup DSPREALTIME
 * @{
 */

/** <pre>
 * <b>[Command]</b>
 *    0x055A
 * <table>
 * <tr><th> Type             <th> Bytes             <th> Value
 * <tr><td> Length           <td> 2 bytes           <td> 0x00  0x04
 * <tr><td> ID               <td> 2 bytes           <td> 0x0E  0x13
 * <tr><td> Mode             <td> 1 bytes           <td> 0x00: leakage detection mode \n 0x02: adaptive FF ANC mode
 * <tr><td> Enable           <td> 1 bytes           <td> 0x00: disable \n 0x01: enable
 * </table>
 * <b>[Field Description]</b>
 *    Trigger ANC relative function
 * <b>[Example]</b>
 *    An Example for triggering leakage detection
 *    0x05 0x5A 0x04 0x00 0x13 0x0E 0x00 0x01
 * <b>[Note]</b>
 *    None
 * </pre>
 */

 /** <pre>
 * <HR>
 * <b>[Response]</b>
 *    0x055B
 * <table>
 * <tr><th> Type             <th> Bytes             <th> Value
 * <tr><td> Length           <td> 2 bytes           <td> 0x00  0x04
 * <tr><td> ID               <td> 2 bytes           <td> 0x0E  0x13
 * <tr><td> Status           <td> 1 bytes           <td> According to the mode
 * <tr><td> Mode             <td> 1 bytes           <td> 0x00: leakage detection mode \n 0x02: adaptive FF ANC mode
 * </table>
 * <b>[Field Description]</b>
 *    The status has different meaning according to the mode.
 * <b>[Example]</b>
 *    An example for a response with the status of 0x00 is shown as below.
 *    0x05 0x5B 0x04 0x00 0x13 0x0E 0x00 0x00
 * <b>[Note]</b>
 *    None
 * </pre>
 */

/** <pre>
 * <HR>
 * <b>[Notification]</b>
 *    0x055D
 * <table>
 * <tr><th> Type                       <th> Bytes             <th> Value
 * <tr><td> Length                     <td> 2 bytes           <td> 0x00 0x0C + N
 * <tr><td> ID                         <td> 2 bytes           <td> 0x0E  0x13
 * <tr><td> start_or_stop              <td> 1 byte            <td> 0x00: stop \n 0x01: start/continue
 * <tr><td> mode                       <td> 1 byte            <td> 0x00: leakage detection mode \n 0x02: adaptive FF ANC mode
 * <tr><td> bit_per_sample             <td> 1 bytes           <td> 0x00: 16bits_deinterleave \n 0x01: 32bits_deinterleave \n 0x02: not pcm data
 * <tr><td> channel_num                <td> 1 bytes           <td> The channel number
 * <tr><td> frame_size                 <td> 2 bytes           <td> sample number per frame
 * <tr><td> sequence_num               <td> 1 bytes           <td> The sequence number
 * <tr><td> total_data_length          <td> 4 bytes           <td> The total data length for all packets
 * <tr><td> data_length                <td> 2 bytes           <td> The data length of this packet
 * <tr><td> data                       <td> N bytes           <td> Data
 * </table>
 * <b>[Field Description]</b>
 *    Send variable length of data
 * <b>[Example]</b>
 *    An example for a notification with 2 bytes data length.
 *    0x05 0x5D 0x10 0x00 0x13 0x0E 0x01 0x00 0x02 0x00 0x00 0x00 0x02 0x00 0x00 0x00 0x02 0x00 0x00 0x00
 * <b>[Note]</b>
 *    None
 * </pre>
 */

#define RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK     0x0E13
/**
*@}
*/
#define RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT    0x0E14
#define RACE_DSPREALTIME_ANC_NOTIFY_GAIN        0x0E15
#define RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND    0x0E16
#define RACE_DSPREALTIME_ANC_GAIN_CONTROL       0x0E17

#define RACE_DSPREALTIME_END_ID                 0x0E21

#define RACE_DSPREALTIME_RELAY_ID_LIST \
{\
    RACE_DSPREALTIME_SUSPEND,                   /*   0x0E01                                     */\
    RACE_DSPREALTIME_RESUME,                    /*   0x0E02                                     */\
    RACE_DSPREALTIME_PEQ,                       /*   0x0E03                                     */\
    RACE_DSPSOUND_EFFECT,                       /*   0x0E04                                     */\
    /*RACE_DSPREALTIME_ANC,                          0x0E06. Need to check sub-ID     */\
    RACE_DSPREALTIME_AIRDUMP_ENTRY,             /*   0x0E0A                                     */\
    RACE_DSPREALTIME_AIRDUMP_ON_OFF,            /*   0x0E0B                                     */\
    RACE_DSPREALTIME_2MIC_SWAP,                 /*   0x0E0C                                     */\
    RACE_DSPREALTIME_AECNR_ON_OFF,              /*   0x0E0D                                     */\
    RACE_DSPREALTIME_PASS_THROUGH_ON_OFF,       /*   0x0E0E                                     */\
    RACE_DSPREALTIME_PASS_THROUGH_TEST,         /*   0x0E0F                                     */\
    RACE_DSPREALTIME_SWGAIN_BYPASS,             /*   0x0E10                                     */\
    RACE_DSPREALTIME_PASS_THROUGH_TEST_MUTE,    /*   0x0E11                                     */\
    RACE_DSPREALTIME_OPEN_ALL_MIC,              /*   0x0E12                                     */\
    RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK,        /*   0x0E13                                     */\
    RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT,       /*   0x0E14                                     */\
    RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND,       /*   0x0E16                                     */\
    RACE_DSPREALTIME_ANC_GAIN_CONTROL,          /*   0x0E17                                     */\
    RACE_DSPREALTIME_OPEN_ALL_MIC_EXTEND,       /*   0x0E20                                     */\
};
////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/** @brief ...... */
typedef struct
{
/** @brief ...... */
	uint8_t Data[0x18];
}PACKED race_dsprealtime_get_refrence_gain_noti_struct;

typedef struct
{
/** @brief ...... */
    uint32_t type;
    uint32_t length;
    uint8_t version[32];
}PACKED race_dsprealtime_query_lib_version_noti_struct_t;

/**
 * @brief The structure for race_dsprealtime_notify_struct event.
 */
typedef struct
{
    uint16_t dsp_realtime_race_id;
    uint16_t dsp_realtime_race_evet;
    uint16_t result;
    uint16_t extend_use;
}race_dsprealtime_notify_struct;


#define RACE_ANC_ON         10
#define RACE_ANC_OFF        11
#define RACE_ANC_SET_VOL    12
#define RACE_ANC_READ_NVDM  13
#define RACE_ANC_WRITE_NVDM 14
#define RACE_ANC_SET_RUNTIME_VOL    15
#define RACE_ANC_SET_SYNC_TIME      21  //for MP tool

#ifdef MTK_ANC_V2
#define RACE_ANC_ENTER_MP_MODE       16 //for MP tool
#define RACE_ANC_LEAVE_MP_MODE       17 //for MP tool
#define RACE_ANC_GET_HYBRID_CAP_V2   22
#define RACE_ANC_REATIME_UPDATE_COEF 23
#define RACE_ANC_EM_CMD              32 //EM race cmd for specific use
#else
#define RACE_ANC_GET_HYBRID_CAP     16
#define RACE_ANC_READ_PARTNER_NVDM  17  //for MP tool
#define RACE_ANC_WRITE_PARTNER_NVDM 18  //for MP tool
#define RACE_ANC_SET_AGENT_VOL      19  //for MP tool
#define RACE_ANC_SET_PARTNER_VOL    20  //for MP tool
#endif
typedef uint8_t race_dsprealtime_anc_id_t;

#define RACE_ANC_AGENT      (1<<0)
#define RACE_ANC_PARTNER    (1<<1)
#define RACE_ANC_BOTH       (RACE_ANC_AGENT | RACE_ANC_PARTNER)

#ifdef MTK_ANC_V2
#define RACE_DSPREALTIME_RELAY_0x0E06_SUBID_LIST \
{\
    RACE_ANC_ON,                    /*  10  */\
    RACE_ANC_OFF,                   /*  11  */\
    RACE_ANC_SET_VOL,               /*  12  */\
    RACE_ANC_WRITE_NVDM,            /*  14  */\
    RACE_ANC_SET_RUNTIME_VOL,       /*  15  */\
    \
    RACE_ANC_ENTER_MP_MODE,         /*  16  */\
    RACE_ANC_LEAVE_MP_MODE,         /*  17  */\
    RACE_ANC_GET_HYBRID_CAP_V2,     /*  22  */\
    RACE_ANC_REATIME_UPDATE_COEF,   /*  23  */\
    \
    RACE_ANC_SET_SYNC_TIME,         /*  21  */\
};
#else
#define RACE_DSPREALTIME_RELAY_0x0E06_SUBID_LIST \
{\
    RACE_ANC_ON,                    /*  10  */\
    RACE_ANC_OFF,                   /*  11  */\
    RACE_ANC_SET_VOL,               /*  12  */\
    RACE_ANC_WRITE_NVDM,            /*  14  */\
    RACE_ANC_SET_RUNTIME_VOL,       /*  15  */\
    \
    RACE_ANC_WRITE_PARTNER_NVDM,    /*  18  */\
    RACE_ANC_SET_AGENT_VOL,         /*  19  */\
    RACE_ANC_SET_PARTNER_VOL,       /*  20  */\
    \
    RACE_ANC_SET_SYNC_TIME,         /*  21  */\
};
#endif


#ifdef MTK_ANC_ENABLE

#ifdef MTK_ANC_V2
typedef struct
{
    uint8_t status;
    uint8_t ancId;
}PACKED RACE_ANC_PASSTHRU_HEADER;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    uint8_t flash_no;
    uint8_t ancType;
    uint8_t syncMode;
} PACKED RACE_CMD_ANC_PASSTHRU_ON_PARAM;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    uint8_t flash_no;
    uint8_t ancType;
    uint8_t syncMode;
    uint8_t reserved;
}PACKED RACE_RSP_ANC_PASSTHRU_ON_PARAM;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    uint8_t syncMode;
} PACKED RACE_CMD_ANC_PASSTHRU_OFF_PARAM;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    uint8_t syncMode;
    uint8_t reserved[3];
}PACKED RACE_RSP_ANC_PASSTHRU_OFF_PARAM;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    int16_t gainFFl;
    int16_t gainFBl;
    int16_t gainFFr;
    int16_t gainFBr;
}PACKED RACE_CMD_ANC_PASSTHRU_SET_VOL_PARAM;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    int16_t gainFFl;
    int16_t gainFBl;
    int16_t gainFFr;
    int16_t gainFBr;
}PACKED RACE_RSP_ANC_PASSTHRU_SET_VOL_PARAM;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
}PACKED RACE_CMD_ANC_PASSTHRU_READ_NVDM;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    int16_t gainFFl;
    int16_t gainFBl;
    int16_t gainFFr;
    int16_t gainFBr;
}PACKED RACE_RSP_ANC_PASSTHRU_READ_NVDM;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    int16_t gainFFl;
    int16_t gainFBl;
    int16_t gainFFr;
    int16_t gainFBr;
}PACKED RACE_CMD_ANC_PASSTHRU_WRITE_NVDM;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    int16_t gainFFl;
    int16_t gainFBl;
    int16_t gainFFr;
    int16_t gainFBr;
}PACKED RACE_RSP_ANC_PASSTHRU_WRITE_NVDM;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    int16_t gain;
    uint8_t syncMode;
}PACKED RACE_CMD_ANC_PASSTHRU_SET_RUNTIME_VOL;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    int16_t gain;
    uint8_t syncMode;
    uint8_t reserved;
}PACKED RACE_RSP_ANC_PASSTHRU_SET_RUNTIME_VOL;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    uint8_t hybridEnable;
}PACKED RACE_RSP_ANC_PASSTHRU_GET_HYBRID_CAP;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    uint8_t ch;
    uint8_t data[1];
}PACKED RACE_CMD_ANC_PASSTHRU_SET_COEF;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
}PACKED RACE_RSP_ANC_PASSTHRU_SET_COEF;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    uint16_t sync_time;
}PACKED RACE_CMD_ANC_PASSTHRU_SET_SYNC_TIME;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    uint16_t sync_time;
    uint16_t reserved;
}PACKED RACE_RSP_ANC_PASSTHRU_SET_SYNC_TIME;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
}PACKED RACE_CMD_ANC_PASSTHRU_ENTER_LEAVE_MP_MODE;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
}PACKED RACE_RSP_ANC_PASSTHRU_ENTER_LEAVE_MP_MODE;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
    uint8_t   event;
    uint8_t   sub_type;
    uint32_t  target;
    uint32_t  value_1;
    uint32_t  value_2;
}PACKED RACE_CMD_ANC_PASSTHRU_EM_MODE_PARAM;

typedef struct
{
    RACE_ANC_PASSTHRU_HEADER header;
}PACKED RACE_RSP_ANC_PASSTHRU_EM_MODE_PARAM;

typedef union RACE_ANC_PASSTHRU
{
    RACE_ANC_PASSTHRU_HEADER header;

    RACE_CMD_ANC_PASSTHRU_ON_PARAM      onCmd;
    RACE_CMD_ANC_PASSTHRU_OFF_PARAM     offCmd;
    RACE_CMD_ANC_PASSTHRU_SET_VOL_PARAM gainCmd;
    RACE_CMD_ANC_PASSTHRU_READ_NVDM     readNVDMCmd;
    RACE_CMD_ANC_PASSTHRU_WRITE_NVDM    writeNVDMCmd;
    RACE_CMD_ANC_PASSTHRU_SET_RUNTIME_VOL runtimeGainCmd;
    RACE_CMD_ANC_PASSTHRU_SET_COEF        setCoefCmd;
    RACE_CMD_ANC_PASSTHRU_SET_SYNC_TIME   setSyncTimeCmd;
    RACE_CMD_ANC_PASSTHRU_ENTER_LEAVE_MP_MODE   MPmodeCmd;
    RACE_CMD_ANC_PASSTHRU_EM_MODE_PARAM         EMCmd;

    RACE_RSP_ANC_PASSTHRU_ON_PARAM      onRsp;
    RACE_RSP_ANC_PASSTHRU_OFF_PARAM     offRsp;
    RACE_RSP_ANC_PASSTHRU_SET_VOL_PARAM gainRsp;
    RACE_RSP_ANC_PASSTHRU_READ_NVDM     readNVDMRsp;
    RACE_RSP_ANC_PASSTHRU_WRITE_NVDM    writeNVDMRsp;
    RACE_RSP_ANC_PASSTHRU_SET_RUNTIME_VOL runtimeGainRsp;
    RACE_RSP_ANC_PASSTHRU_SET_COEF        setCoefRsp;
    RACE_RSP_ANC_PASSTHRU_SET_SYNC_TIME   setSyncTimeRsp;
    RACE_RSP_ANC_PASSTHRU_ENTER_LEAVE_MP_MODE   MPmodeRsp;
    RACE_RSP_ANC_PASSTHRU_EM_MODE_PARAM         EMCmdRsp;

    RACE_RSP_ANC_PASSTHRU_GET_HYBRID_CAP  getHybridRsp;
}PACKED RACE_ANC_PASSTHRU_UNION;

typedef struct
{
    RACE_ANC_PASSTHRU_UNION param;
} PACKED race_dsprealtime_anc_struct;
#else
typedef struct
{
    uint8_t anc_filter_type;
    uint8_t anc_mode;           // 0: normal, 1: ff_only, 2: fb_only
} PACKED anc_on_param_t;

typedef union {
    anc_on_param_t anc_on_param;//for anc on
    anc_sw_gain_t gain;         //for set vol, rw vol nv
    int16_t runtime_gain;       //for set runtime vol
    anc_hybrid_cap_t hybrid_cap;//for get hybrid capability
    uint16_t u2param;           //for set sync time
} race_dsprealtime_anc_param_struct;

typedef struct
{
    uint8_t status;
    race_dsprealtime_anc_id_t anc_id;
    race_dsprealtime_anc_param_struct param;
} PACKED race_dsprealtime_anc_struct;
#endif

//#ifdef MTK_LEAKAGE_DETECTION_ENABLE
//ANC adaptive check mode
#define LEAKAGE_DETECTION_MODE         0
#define SCENARIO_CLASSIFICATION_MODE   1
#define ADAPTIVE_FF_ANC_MODE           2
#define ADAPTIVE_FB_ANC_MODE           3
#define USER_UNAWARE_DUBUG_MODE        9
#define USER_UNAWARE_MODE             (10)
#define ADAPTIVE_ANC_STREAM_MODE      (11)
#define ADAPTIVE_ANC_STREAM_STATUS    (12)



typedef struct
{
    uint8_t start_or_stop;
    uint8_t mode;
    uint8_t bit_per_sample;
    uint8_t channel_num;
    uint16_t frame_size;
    uint8_t seq_num;
    uint32_t total_data_length;
    uint16_t data_length;
} PACKED adaptive_check_notify_t;
//#endif

typedef struct
{
    uint8_t mode;
    uint16_t data_len;
} PACKED RACE_CMD_ANC_ADAPTIVE_EXTEND_HDR_T;

typedef struct
{
    RACE_CMD_ANC_ADAPTIVE_EXTEND_HDR_T hdr;
    uint8_t sub_mode;
} PACKED RACE_CMD_ANC_ADAPTIVE_EXTEND_USER_UNAWARE_T;

typedef struct
{
    uint8_t status;
    uint8_t mode;
    uint16_t data_len;
} PACKED RACE_RSP_ANC_ADAPTIVE_EXTEND_HDR_T;

typedef struct
{
    RACE_RSP_ANC_ADAPTIVE_EXTEND_HDR_T hdr;
    uint8_t sub_mode;
    uint16_t real_data_length;
    uint8_t enable;//1: enable, else: disable
} PACKED RACE_RSP_ANC_ADAPTIVE_EXTEND_USER_UNAWARE_GET_EN_STAT_T;

typedef union
{
    RACE_CMD_ANC_ADAPTIVE_EXTEND_USER_UNAWARE_T UUCmd;

} PACKED RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND_UNION;

typedef struct
{
    RACE_DSPREALTIME_ANC_ADAPTIVE_EXTEND_UNION param;
} PACKED race_dsprealtime_anc_adaptive_extend_t;
#endif

#ifdef MTK_AIRDUMP_EN_MIC_RECORD
#define Airdump_mic_record  10
typedef uint8_t race_dsprealtime_airdump_id_t;

typedef struct
{
    uint8_t  Dump_Enable;  // 0: Enable, 1: Disable
    uint8_t  Dump_Request; // Mic_mic : 0x1 << 0,  Ref_mic: 0x1 << 1, Echo_ref: 0x1 << 2
} PACKED airdump_mic_record_request_param_t;

typedef struct
{
    uint8_t  Data_sequence_number;  //For checking packet loss.
    uint8_t  Data_size_per_sample;  //bytes
    uint32_t Data_length;
    uint8_t  Dump_Request; // Mic_mic : 0x1 << 0,  Ref_mic: 0x1 << 1, Echo_ref: 0x1 << 2
} PACKED airdump_mic_record_notify_param_t;

typedef union {
    airdump_mic_record_request_param_t air_dump_mic_record_request;
} race_dsprealtime_airdump_request_param_struct;

typedef union {
    airdump_mic_record_notify_param_t  air_dump_mic_record_notify;
} race_dsprealtime_airdump_notify_param_struct;

typedef struct
{
    race_dsprealtime_airdump_id_t                 airdump_id;
    race_dsprealtime_airdump_request_param_struct param;
} PACKED race_dsprealtime_airdump_common_request_struct;

typedef struct
{
    uint8_t status;
} PACKED race_dsprealtime_airdump_common_response_struct;

typedef struct
{
    race_dsprealtime_airdump_id_t                airdump_id;
    race_dsprealtime_airdump_notify_param_struct param;
    int16_t airdump_data[256];
} PACKED race_dsprealtime_airdump_common_notify_struct;
#endif

#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
#define MAX_PEQ_BANDS       (10)
#define MAX_PEQ_ELEMENT     (4)
typedef struct dsp_peq_param_s
{
    uint16_t elementID;
    uint16_t numOfParameter;
    uint16_t peq_inter_param[5*MAX_PEQ_BANDS*2+2+1];
} PACKED DSP_PEQ_PARAM_t;
typedef struct DSP_PEQ_NVKEY_s
{
    uint16_t numOfElement;
    uint16_t peqAlgorithmVer;
    DSP_PEQ_PARAM_t peq_element_param[MAX_PEQ_ELEMENT];
} PACKED DSP_PEQ_NVKEY_t;
#endif

#ifdef MTK_AIRDUMP_EN
#define AEC_PRELIM_COEF_SIZE              (9)
#define AEC_REF_GAIN_SIZE                 (8)
#define AEC_DUALMIC_POWER_COEF_SIZE       (1)
#define AEC_INEAR_SIZE                    (6)
#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
#define AEC_AIRDUMP_FRAME_SIZE            (AEC_PRELIM_COEF_SIZE+AEC_REF_GAIN_SIZE+AEC_DUALMIC_POWER_COEF_SIZE+AEC_INEAR_SIZE)*2//byte
#else
#define AEC_AIRDUMP_FRAME_SIZE            (AEC_PRELIM_COEF_SIZE+AEC_REF_GAIN_SIZE)*2//byte
#endif
#define AEC_AIRDUMP_FRAME_NUM               (10)
#define AEC_AIRDUMP_FRAME_NUM_HALF          (AEC_AIRDUMP_FRAME_NUM/2)

typedef struct {
   uint32_t read_offset;
   uint32_t write_offset;
   uint16_t length;
   uint8_t  notify_count;
   uint8_t  data[AEC_AIRDUMP_FRAME_SIZE*AEC_AIRDUMP_FRAME_NUM];
} AIRDUMP_BUFFER_INFO;

typedef struct
{
   uint8_t data[AEC_AIRDUMP_FRAME_SIZE*AEC_AIRDUMP_FRAME_NUM_HALF];
}PACKED RACE_DSPREALTIME_AIRDUMP_EVT_NOTI_STRU;

typedef enum {
    AIRDUMP_EXECUTION_NONE    = -2,
    AIRDUMP_EXECUTION_FAIL    = -1,
    AIRDUMP_EXECUTION_SUCCESS =  0,
} airdump_timer_result_t;
#endif

typedef enum {
    DUAL_MIC   = 0,
    SINGLE_MIC = 1,
    MIC_L_ONLY = 2,
    MIC_R_ONLY = 3,
    MIC_3_ONLY = 4,
    MIC_4_ONLY = 5,
    MIC_5_ONLY = 6,
    MIC_6_ONLY = 7,
} mic_swap_e;

typedef enum {
    AEC_NR_DISABLE   = 0,
    AEC_NR_ENABLE    = 1,
} aec_nr_e;

typedef enum {
    SW_GAIN_DISABLE   = 0,
    SW_GAIN_ENABLE    = 1,
} sw_gain_e;

#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
typedef struct {
    race_send_pkt_t* pSndPkt;
    uint32_t port_handle;
    uint32_t completed_len;
}anc_user_trigger_ff_keep_relay_data_t;
#endif

typedef struct{
    uint32_t event_id;
}PACKED RACE_DSPREALTIME_COSYS_EVT_STRU;

/** @brief Define race_dsp_realtime callback function prototype.
 *  @param[in] event_id.
 */
typedef void (*race_dsp_realtime_callback_t)(uint32_t event_id);

typedef struct {
    uint8_t source_channel_id;
    uint8_t send_to_follower;   //True: Receiver send CMD to follower, False: Get response from follower
    uint8_t follower_result;
    uint8_t _reserved;
    race_pkt_t race_cmd_pkt;
} race_dsprealtime_relay_packet_t, *race_dsprealtime_relay_packet_ptr_t;
////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
extern uint32_t race_dsprt_peq_realtime_data(uint8_t phase_id, uint8_t setting_mode, uint32_t target_bt_clk, uint8_t *p_coef, uint32_t coef_size, am_feature_type_t audio_path_id);
extern uint32_t race_dsprt_peq_change_mode_data(uint8_t phase_id, uint8_t setting_mode, uint32_t target_bt_clk, uint8_t enable, uint8_t sound_mode, am_feature_type_t audio_path_id);
#ifdef MTK_AWS_MCE_ENABLE
extern int32_t race_dsprt_peq_collect_data(bt_aws_mce_report_info_t *info);
extern int32_t race_dsprt_peq_get_target_bt_clk(bt_aws_mce_role_t role, uint8_t *setting_mode, bt_clock_t *target_bt_clk);
#endif
#endif
#ifdef MTK_ANC_ENABLE
#ifndef MTK_ANC_V2
#ifdef MTK_AWS_MCE_ENABLE
extern uint32_t race_dsprt_anc_feedback_data(aws_mce_report_anc_param_t *anc_param);
#endif
extern void race_dsprt_anc_notify(anc_control_event_t event_id, uint8_t fromPartner, uint32_t arg);
#endif
#endif
#ifdef MTK_AIRDUMP_EN
airdump_timer_result_t airdump_timer_create(void);
void airdump_timer_callback(void);airdump_timer_result_t airdump_timer_create(void);
extern void airdump_timer_delete(void);
void RACE_AirDump_Send(uint8_t channelId,uint8_t RaceType, uint8_t *ptr, uint16_t len);
#endif
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
void anc_leakage_detection_racecmd_callback(uint16_t leakage_status);
void anc_leakage_detection_racecmd_response(uint16_t leakage_status, uint8_t fromPartner);
#endif
#ifdef MTK_ANC_ENABLE
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
void race_dsprealtime_anc_adaptive_trigger_ff();
void race_dsprealtime_anc_adaptive_notify_end();
void race_dsprealtime_anc_adaptive_abort();
#else
void race_dsprealtime_anc_adaptive_response(bool success, bool enable);
void user_trigger_adaptive_ff_racecmd_response(uint8_t mode, uint8_t data_id, uint8_t *data_buff, uint16_t data_len, uint8_t start_or_stop, uint8_t seq_num, uint32_t completed_len);
#endif
#endif



// Notify ANC Gain Result to Phone APP
#ifdef MTK_ANC_ENABLE
// User Unaware
#ifdef AIR_ANC_USER_UNAWARE_ENABLE
void race_dsprealtime_anc_user_unaware_response(int16_t gain);
void race_dsprealtime_anc_user_unaware_notify(int16_t gain);
#endif
// Wind Noise
#ifdef AIR_ANC_WIND_DETECTION_ENABLE
void race_dsprealtime_anc_wind_noise_response(int16_t gain);
void race_dsprealtime_anc_wind_noise_notify(int16_t gain);
#endif
// Environment Detection
#ifdef AIR_ANC_ENVIRONMENT_DETECTION_ENABLE
void race_dsprealtime_anc_environment_detection_response(uint8_t level, int16_t ff_gain, int16_t fb_gain,
                                                         int16_t local_stationary_noise, int16_t peer_stationary_noise);
void race_dsprealtime_anc_environment_detection_notify(uint8_t level, int16_t ff_gain, int16_t fb_gain,
                                                       int16_t local_stationary_noise, int16_t peer_stationary_noise);
#endif

typedef enum {
    RACE_ANC_GAIN_STATUS_SUCCESS = 0,
    RACE_ANC_GAIN_STATUS_ANC_NOT_ON,
    RACE_ANC_GAIN_STATUS_AWS_NOT_CONNTECT,
    RACE_ANC_GAIN_STATUS_FEATURE_NOT_ENABLE,
    RACE_ANC_GAIN_STATUS_UNKNOWN_ERROR
} anc_gain_race_status_t;

void race_dsprealtime_anc_notify_gain_error(uint8_t status, uint8_t event_id);

typedef enum {
    RACE_ANC_GAIN_CONTROL_DISABLE = 0,
    RACE_ANC_GAIN_CONTROL_ENABLE,
    RACE_ANC_GAIN_CONTROL_QUERY
} anc_gain_control_action_t;

typedef struct {
    uint8_t  type;
    uint8_t  action;
} PACKED anc_gain_control_param_t;

void race_dsprealtime_anc_gain_control_response(uint8_t status, uint8_t type, bool enable);
#endif /* MTK_ANC_ENABLE */



#endif
/*!
  @brief Process DSP REALTIME related RACE commands.

  @param pRaceHeaderCmd This parameter represents the raw data such as "05 5A...".
  @param Lenth Total bytes of this RACE command.
  @param channel_id Channel identifier
*/
void* RACE_CmdHandler_DSPREALTIME(ptr_race_pkt_t pRaceHeaderCmd, uint16_t Length, uint8_t channel_id);

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
void race_dsprealtime_cosys_relay_callback(bool is_critical, uint8_t *buff, uint32_t len);
#endif

#endif /* RACE_DSP_REALTIME_CMD_ENABLE */
#endif /* __RACE_CMD_DSPREALTIME_H__ */

