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

#ifndef __RACE_CMD_H__
#define __RACE_CMD_H__


#include "race_cmd_feature.h"
#include "stdint.h"
#include "assert.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define Vendor_SPACE                  (' ')
#define Vendor_EQUAL                  ('=')
#define Vendor_COMMA                  (')')
#define Vendor_SEMICOLON              (';')
#define Vendor_COLON                  (':')
#define Vendor_AT                     ('@')
#define Vendor_HAT                    ('^')
#define Vendor_DOUBLE_QUOTE           ('"')
#define Vendor_QUESTION_MARK          ('?')
#define Vendor_EXCLAMATION_MARK       ('!')
#define Vendor_FORWARD_SLASH          ('/')
#define Vendor_L_ANGLE_BRACKET        ('<')
#define Vendor_R_ANGLE_BRACKET        ('>')
#define Vendor_L_SQ_BRACKET           ('[')
#define Vendor_R_SQ_BRACKET           (']')
#define Vendor_L_CURLY_BRACKET        ('{')
#define Vendor_R_CURLY_BRACKET        ('}')
#define Vendor_CHAR_STAR              ('*')
#define Vendor_CHAR_POUND             ('#')
#define Vendor_CHAR_AMPSAND           ('&')
#define Vendor_CHAR_PERCENT           ('%')
#define Vendor_CHAR_PLUS              ('+')
#define Vendor_CHAR_MINUS             ('-')
#define Vendor_CHAR_DOT               ('.')
#define Vendor_CHAR_ULINE             ('_')
#define Vendor_CHAR_TILDE             ('~')
#define Vendor_CHAR_REVERSE_SOLIDUS   ('\\')
#define Vendor_CHAR_VERTICAL_LINE     ('|')
#define Vendor_END_OF_STRING_CHAR     ('\0')
#define Vendor_CHAR_0                 ('0')
#define Vendor_CHAR_1                 ('1')
#define Vendor_CHAR_2                 ('2')
#define Vendor_CHAR_3                 ('3')
#define Vendor_CHAR_4                 ('4')
#define Vendor_CHAR_5                 ('5')
#define Vendor_CHAR_6                 ('6')
#define Vendor_CHAR_7                 ('7')
#define Vendor_CHAR_8                 ('8')
#define Vendor_CHAR_9                 ('9')
#define Vendor_CHAR_A                 ('A')
#define Vendor_CHAR_B                 ('B')
#define Vendor_CHAR_C                 ('C')
#define Vendor_CHAR_D                 ('D')
#define Vendor_CHAR_E                 ('E')
#define Vendor_CHAR_F                 ('F')
#define Vendor_CHAR_G                 ('G')
#define Vendor_CHAR_H                 ('H')
#define Vendor_CHAR_I                 ('I')
#define Vendor_CHAR_J                 ('J')
#define Vendor_CHAR_K                 ('K')
#define Vendor_CHAR_L                 ('L')
#define Vendor_CHAR_M                 ('M')
#define Vendor_CHAR_N                 ('N')
#define Vendor_CHAR_O                 ('O')
#define Vendor_CHAR_P                 ('P')
#define Vendor_CHAR_Q                 ('Q')
#define Vendor_CHAR_R                 ('R')
#define Vendor_CHAR_S                 ('S')
#define Vendor_CHAR_T                 ('T')
#define Vendor_CHAR_U                 ('U')
#define Vendor_CHAR_V                 ('V')
#define Vendor_CHAR_W                 ('W')
#define Vendor_CHAR_X                 ('X')
#define Vendor_CHAR_Y                 ('Y')
#define Vendor_CHAR_Z                 ('Z')
#define Vendor_char_a                 ('a')
#define Vendor_char_b                 ('b')
#define Vendor_char_c                 ('c')
#define Vendor_char_d                 ('d')
#define Vendor_char_e                 ('e')
#define Vendor_char_f                 ('f')
#define Vendor_char_g                 ('g')
#define Vendor_char_h                 ('h')
#define Vendor_char_i                 ('i')
#define Vendor_char_j                 ('j')
#define Vendor_char_k                 ('k')
#define Vendor_char_l                 ('l')
#define Vendor_char_m                 ('m')
#define Vendor_char_n                 ('n')
#define Vendor_char_o                 ('o')
#define Vendor_char_p                 ('p')
#define Vendor_char_q                 ('q')
#define Vendor_char_r                 ('r')
#define Vendor_char_s                 ('s')
#define Vendor_char_t                 ('t')
#define Vendor_char_u                 ('u')
#define Vendor_char_v                 ('v')
#define Vendor_char_w                 ('w')
#define Vendor_char_x                 ('x')
#define Vendor_char_y                 ('y')
#define Vendor_char_z                 ('z')
#define Vendor_R_BRACKET              (')')
#define Vendor_L_BRACKET              ('(')
#define Vendor_MONEY                  ('$')

#define RACE_INVALID_CMD_ID (0xFFFF)

/*RACE global error code*/

typedef enum
{
    RACE_ERRCODE_SUCCESS = 0,

    /* Fail error code */
    RACE_ERRCODE_FAIL,
    RACE_ERRCODE_NOT_SUPPORT,
    RACE_ERRCODE_PARAMETER_ERROR,
    RACE_ERRCODE_NOT_ENOUGH_MEMORY,
    RACE_ERRCODE_MORE_OPERATION,
    RACE_ERRCODE_MAX_RETRY,
    RACE_ERRCODE_STORAGE_READ_FAIL,
    RACE_ERRCODE_CONNECTION_BROKEN,
    RACE_ERRCODE_NOT_ALLOWED,
    RACE_ERRCODE_CONFLICT,
    RACE_ERRCODE_WRONG_STATE,
    RACE_ERRCODE_NOT_INITIALIZED,
    RACE_ERRCODE_CHECK_INTEGRITY_FAIL,
    RACE_ERRCODE_WOULDBLOCK,
    RACE_ERRCODE_REJECT_FOR_CALL_ONGOING,

    RACE_ERRCODE_MAX = 0xFF
}race_errcode_enum, RACE_ERRCODE;

/** @brief
 * This enum defines the RACE status types.
 */
typedef enum {
    RACE_STATUS_REGISTRATION_FAILURE = -2,   /**< Failed to register the RACE command handler. */
    RACE_STATUS_ERROR = -1,                  /**< An error occurred during the function call. */
    RACE_STATUS_OK = 0                       /**< No error occurred during the function call. */
} race_status_t;

/*RACE type*/
#define RACE_TYPE_COMMAND 0x5A
#define RACE_TYPE_RESPONSE 0x5B
#define RACE_TYPE_COMMAND_WITHOUT_RSP 0x5C
#define RACE_TYPE_NOTIFICATION 0x5D

/*RACE done*/
#define RACE_DONE_SUCCESS 0xFF
#define RACE_DONE_UNKNOWN 0xFE

/* RACE channel Type */
#define RACE_CHANNEL_TYPE_UART     0
#define RACE_CHANNEL_TYPE_USB      1
#define RACE_CHANNEL_TYPE_AIRAPP   2
#define RACE_CHANNEL_TYPE_SOFTWARE 3

#define RACE_MAX_GNENERAL_TABLE_NUM    (20)

#ifndef PACKED
#define PACKED  __attribute__((packed))
#endif

#ifndef _WIN32
#define OFFSET_OF(type,member) ((uint32_t)&(((type *)0)->member))
#else
#define OFFSET_OF(type,member) ((unsigned int)&(((type *)0)->member))
#endif

#define CONTAINER_OF(ptr,type,member) ((type *)((uint8_t *)(ptr) - OFFSET_OF(type,member)))


//typedef struct HandlerData *Handler;

//typedef struct HandlerData { uint32_t (*handler)(Handler, uint16_t, void*, uint32_t); } HandlerData;


/** @defgroup atci_define Define
  * @{
  */

/** @brief This macro defines the data length of the AT command response. The length defined in response_buf
  * of #atci_response_t structure cannot be larger than this macro.
  */

/* UART related */
//#define RACE_UART_RX_FIFO_ALERT_SIZE        (50)
//#define RACE_UART_RX_FIFO_THRESHOLD_SIZE    (128)
//#define RACE_UART_TX_FIFO_THRESHOLD_SIZE    (51)


/**
  * @}
  */

/** @defgroup atci_enum Enums
  * @{
  */
typedef union {
    struct {
        uint8_t ch_byte: 4;
        uint8_t app_id: 4;
    } field;
    uint8_t value;
} RACE_CH_BYTE;

typedef struct {
	RACE_CH_BYTE 	pktId;
	uint8_t 	type;
	uint16_t	length;
	uint16_t	id;

}PACKED RACE_COMMON_HDR_STRU, *PTR_RACE_COMMON_HDR_STRU;


/** @brief
 * This enum defines the ATCI status types.
 */

typedef struct {
	RACE_COMMON_HDR_STRU hdr;
	uint8_t	payload[0];

}PACKED race_pkt_t, *ptr_race_pkt_t;

typedef struct {
    uint16_t offset;
    uint16_t length;
    uint8_t channel_id;
    uint8_t reserve;
    race_pkt_t race_data;
}PACKED race_send_pkt_t, *ptr_race_send_pkt_t;

typedef struct {
    uint16_t id_start;
    uint16_t id_end;
    void *(*handler)(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id);
} RACE_HANDLER;

typedef enum
{
    RACE_DBG_CMD,
    RACE_DBG_EVT,
    RACE_DBG_EVT_APP,
    RACE_DBG_IF_RELAY,
    RACE_DBG_FLUSH,
    RACE_DBG_MAX,
}race_debug_type_enum;

#if 0
/*!
    @brief Race packet without payload and id.
*/
typedef struct
{
    uint8_t pktId;       /*!< Packet type, usually is 0x5*/
    uint8_t type;        /*!< Type field*/
    uint16_t length;     /*!< length field*/
}PACKED RACE_COMMON_HDR_STRU, *PTR_RACE_COMMON_HDR_STRU;

/*!
    @brief Race packet without payload.
*/
typedef struct
{
    RACE_COMMON_HDR_STRU hdr;   /*!<Race packet without payload and id*/
    uint16_t id;                     /*!< ID field*/
}PACKED RACE_GLOBAL_HDR_STRU, *PTR_RACE_GLOBAL_HDR_STRU;

#endif

/** @defgroup atci_typedef Typedef
  * @{
  */




/**
  * @}
  */


/**
 * @brief This function initializes the ATCI module. It is used to set the port configuration for data transmission.
 * @param[in] port is the serial port number used to data transmission for ATCI. For more details about this parameter, please refer to #hal_uart_port_t or #serial_port_dev_t.
 * @return   #ATCI_STATUS_OK the ATCI initialized successfully. \n
 *               #ATCI_STATUS_ERROR the UART initialization or the ATCI local initialization failed due to the ATCI initialization failure.
 * @note     The #atci_init() can select the UART through HAL UART port directly, or select UART, USB or BT SPP server/client port through port service for data transmission.
 * @par       Example
 * @code
 *       ret = atci_init(port);
 *       if (ret == ATCI_STATUS_OK) {
 *          // The ATCI initialized successfully.
 *          atci_register_handler(table, hdlr_number);
 *          // Create an ATCI task.
 *       } else {
 *          // The ATCI initialization failed.
 *       }
 * @endcode
 */


/**
 * @brief This function parses the input command to find the corresponding command handler and handle the response data.
 * @return    void.
 */
extern void race_dump(uint8_t *data, race_debug_type_enum type);

extern void *RACE_CmdHandler(race_pkt_t *pMsg, uint8_t channel_id);
extern void* RACE_ClaimPacket(uint8_t race_type, uint16_t race_id, uint16_t dat_len, uint8_t channel_id);
extern void* RACE_ClaimPacketAppID(uint8_t app_id, uint8_t race_type, uint16_t race_id, uint16_t dat_len, uint8_t channel_id);


void RACE_FreePacket(void *data);
race_status_t RACE_Register_Handler(RACE_HANDLER *pHandler);

bool race_cmd_is_to_remote(race_pkt_t *pMsg);
/**
  * @}
  */
#ifdef __cplusplus
}
#endif


#endif
