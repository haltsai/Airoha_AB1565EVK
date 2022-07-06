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


#ifndef __RACE_EVENT_H__
#define __RACE_EVENT_H__


#include "race_cmd_feature.h"
#include "race_cmd.h"


/**
 * @addtogroup Race_CMD_Group Race CMD
 * @{
 * @addtogroup  Race_Event Race Event
 * @{
 * Race event is a sub-module of the race cmd module. It provides an event
 * mechanism for other modules to receive and process the race events they
 * need to.
 */


#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup race_event_define Define
 * @{
 */
////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Race event types.
 */
typedef enum
{
    RACE_EVENT_TYPE_NONE,    /**< NONE type for initialization. */

    RACE_EVENT_TYPE_CONN_USB_CONNECT,    /**< USB is connected. */
    RACE_EVENT_TYPE_CONN_BLE_CONNECT,    /**< BLE is connected. */
    RACE_EVENT_TYPE_CONN_SPP_CONNECT,    /**< SPP is connected. */
    RACE_EVENT_TYPE_CONN_AIRUPDATE_CONNECT,    /**< AIRUPDATE is connected. */
    RACE_EVENT_TYPE_CONN_IAP2_CONNECT,    /**< iAP2 is connected. */
    RACE_EVENT_TYPE_CONN_GATT_OVER_BREDR_CONNECT,    /**< GATT is connected. */

    RACE_EVENT_TYPE_CONN_USB_DISCONNECT,    /**< USB is disconnected. */
    RACE_EVENT_TYPE_CONN_BLE_DISCONNECT,    /**< BLE is disconnected. */
    RACE_EVENT_TYPE_CONN_SPP_DISCONNECT,    /**< SPP is disconnected. */
    RACE_EVENT_TYPE_CONN_AIRUPDATE_DISCONNECT,    /**< AIRUPDATE is disconnected. */
    RACE_EVENT_TYPE_CONN_IAP2_DISCONNECT,    /**< iAP2 is disconnected. */
    RACE_EVENT_TYPE_CONN_GATT_OVER_BREDR_DISCONNECT,    /**< GATT is disconnected. */

    /* Only Agent needs to call RHO API. */
    RACE_EVENT_TYPE_BT_NEED_RHO,    /**< RHO is needed. */

    /* Beware that when Partner receives this event, RHO may has already been done! */
    RACE_EVENT_TYPE_BT_RHO_START,    /**< RHO starts. */
    /* Only Agent will receive this event. */
    RACE_EVENT_TYPE_BT_RHO_PREPARE,    /**< Prepare for RHO. */
    /* race_event_rho_result_param_struct */
    RACE_EVENT_TYPE_BT_RHO_RESULT,    /**< RHO result. */

    RACE_EVENT_TYPE_FOTA_START,    /**< FOTA starts. */
    RACE_EVENT_TYPE_FOTA_TRANSFER_COMPLETE,    /**< FOTA download completes both in Agent and Partner. */
    RACE_EVENT_TYPE_FOTA_NEED_REBOOT,    /**< Reboot is needed. */
    /* race_event_cancelling_param_struct */
    RACE_EVENT_TYPE_FOTA_CANCELLING,    /**< FOTA is cancelling. */
    /* race_event_cancel_param_struct */
    RACE_EVENT_TYPE_FOTA_CANCEL,    /**< FOTA is cancelled. */
#ifdef RACE_FIND_ME_ENABLE
    RACE_EVENT_TYPE_FIND_ME,        /**< Find Me. */
#endif
#ifdef RACE_FCD_CMD_ENABLE
        RACE_EVENT_TYPE_RESSI,    /**< Get RSSI. */
#endif
    RACE_EVENT_TYPE_KEY,    /**< A race command of key event is received. */
    RACE_EVENT_RELOAD_NVKEY_TO_RAM,    /**< Notification that a NVDM is changed. */
    RACE_EVENT_SYSTEM_POWER,    /**< Need to enter RTC/POWEROFF/SLEEP mode. */

    RACE_EVENT_TYPE_CMD_VP,     /**< Race command vp PLAY/STOP request. */
    RACE_EVENT_TYPE_AUDIO_DSP_REALTIME,     /**< Race command audio dsp realtime request. */
#ifdef MTK_ANC_ENABLE
#ifdef MTK_USER_TRIGGER_FF_ENABLE
    RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_START,    /**< Notify APP to start ANC adaptive test. */
    RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_CANCEL,    /**< Notify APP to cancel ANC adaptive test. */
    RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_END,    /**< ANC adaptive test ends. */
#endif
    RACE_EVENT_TYPE_ANC_GAIN_USER_UNAWARE,    /**< Notify APP to read and reply ANC USER_UNAWARE Gain. */
    RACE_EVENT_TYPE_ANC_GAIN_WIND_NOISE,      /**< Notify APP to read and reply ANC WIND_NOISE Gain. */
    RACE_EVENT_TYPE_ANC_GAIN_ENVIRONMENT_DETECTION,      /**< Notify APP to read and reply ANC ENVIRONMENT_DETECTION Gain. */
    RACE_EVENT_TYPE_ANC_GAIN_CONTROL,         /**< Notify APP to enable/disable the above Gain feature. */
#endif
    RACE_EVENT_TYPE_MAX    /**< The maximum value of this enum. */
} race_event_type_enum;


/**
 * @brief FOA stop originator types.
 */
typedef enum
{
    RACE_FOTA_STOP_ORIGINATOR_NONE,    /**< NONE type for initialization. */
    RACE_FOTA_STOP_ORIGINATOR_SP,    /**< The smartphone originates the FOTA stop. */
    RACE_FOTA_STOP_ORIGINATOR_AGENT,    /**< The Agent originates the FOTA stop. */
    RACE_FOTA_STOP_ORIGINATOR_PARTNER,    /**< The Partner originates the FOTA stop. */

    RACE_FOTA_STOP_ORIGINATOR_MAX    /**< The maximum value of this enum. */
}race_fota_stop_originator_enum;


/**
 * @brief FOA stop reason types.
 */
typedef enum
{
    RACE_FOTA_STOP_REASON_CANCEL = 0,    /**< FOTA is cancelled. */
    RACE_FOTA_STOP_REASON_FAIL,    /**< A failure occurs. */
    RACE_FOTA_STOP_REASON_TIMEOUT,    /**< A timer expires. */
    RACE_FOTA_STOP_REASON_PARTNER_LOST,    /**< Contact cannot be made with Partner. */
    RACE_FOTA_STOP_REASON_NOT_ALLOWED,    /**< An operation is not allowed. */

    /* Device used only */
    RACE_FOTA_STOP_REASON_SP_LOST = 0xF0,    /**< Contact cannot be made with the smartphone. */
    RACE_FOTA_STOP_REASON_AGENT_LOST,    /**< Contact cannot be made with Agent. */
    RACE_FOTA_STOP_REASON_BT_OFF,    /**< BT is off. */
    RACE_FOTA_STOP_REASON_UNEXPECTED_RHO_ONGOING,    /**< Unexpected RHO is on-going. */
    RACE_FOTA_STOP_REASON_UNEXPECTED_RHO_DONE,    /**< Unexpected RHO is done. */

    RACE_FOTA_STOP_REASON_MAX = 0xFF    /**< The maximum value of this enum. */
}race_fota_stop_reason_enum;

/**
 * @}
 */


/**
 * @defgroup race_event_struct Struct
 * @{
 */

/**
 * @brief Key event types.
 */
typedef enum
{
    RACE_KEY_FACT_RST,    /**< Need to do factory reset. */
    RACE_KEY_FACT_RST_POWER_OFF,     /**< Need to do factory reset and power off. */
    RACE_KEY_POWER_OFF    /**< Need to poweroff. */
}race_key_event_enum;


/**
 * @brief System power event types.
 */
typedef enum
{
    RACE_SYSTEM_POWER_PMUOFF,    /**< Need to do PMUOFF. */
    RACE_SYSTEM_POWER_RTC,    /**< Need to enter RTC mode. */
    RACE_SYSTEM_POWER_SLEEP    /**< Need to enter sleep mode. */
}race_system_power_event_enum;


/**
 * @brief Race command vp type.
 */
typedef enum
{
    RACE_CMD_VP_LEAKAGE_DETECTION,  /**< The type of leakage detection. */
}race_cmd_vp_enum;


/**
 * @brief The structure for RACE_EVENT_TYPE_FOTA_START event.
 */
typedef struct
{
    bool is_dual_fota;    /**< Is it the dual device FOTA or the single device FOTA. */
    bool is_active_fota;    /**< Is it the active FOTA or the background FOTA. */
}race_event_start_param_struct;


/**
 * @brief The structure for RACE_EVENT_TYPE_FOTA_CANCELLING event.
 */
typedef struct
{
    race_fota_stop_originator_enum originator;    /**< The originator of the FOTA cancel */
    race_fota_stop_reason_enum reason;    /**< The reason FOTA was cancelled */
}race_event_cancelling_param_struct;


/**
 * @brief The structure for RACE_EVENT_TYPE_FOTA_CANCEL event.
 */
typedef struct
{
    bool result;    /**< TRUE, FOTA cancel succeeds; FALSE, FOTA cancel fails. */
    race_fota_stop_originator_enum originator;    /**< The originator of the FOTA cancel */
    race_fota_stop_reason_enum reason;    /**< The reason of the FOTA cancel */
}race_event_cancel_param_struct;


/**
 * @brief This macro defines the maximum length of the remote device address.
 */
#define RACE_EVENT_REMOTE_DEVICE_ADDRESS_LENGTH (6)


/**
 * @brief The structure for RACE_EVENT_TYPE_FOTA_NEED_REBOOT event.
 */
typedef struct
{
    uint8_t address[RACE_EVENT_REMOTE_DEVICE_ADDRESS_LENGTH];    /**< It is the address related to the current FOTA transferring method. If it is all zero, it is an invalid address. */
    uint32_t address_length;    /**< It is the valid length of the address above. */
}race_event_need_reboot_param_struct;


/**
 * @brief The structure for RACE_EVENT_TYPE_BT_RHO_RESULT event.
 */
typedef struct
{
    bool result;    /**< TRUE, RHO succeeds; FALSE, RHO fails. */
}race_event_rho_result_param_struct;


#ifdef RACE_FIND_ME_ENABLE
/**
 * @brief The structure for RACE_EVENT_TYPE_FIND_ME event.
 */
typedef struct
{
    uint8_t is_blink;	/**< 1, Blink; 0, No blink. */
    uint8_t is_tone;	/**< 1, Must blink; FALSE, RHO fails. */
}race_event_find_me_param_struvt;
#endif


/**
 * @brief The structure for RACE_EVENT_TYPE_CMD_VP event.
 */
typedef struct
{
    race_cmd_vp_enum vp_type;   /**< The vp type. */
    bool play_flag;     /**< True means PLAY request; False means STOP request */
}race_cmd_vp_struct;


/**
 * @}
 */


#ifdef MTK_RACE_EVENT_ID_ENABLE
/**
 * @brief The callback prototype used in race_event_register(). When any race
 * event occurs, the callback will be invoked.
 * @param[in] register_id    The same register_id obtained when calling race_event_register().
 * @param[in] event_type    The type of the event occurs
 * @param[in] param    The parameter of the event
 * @param[in] user_data    The same user_data input in race_event_register()
 * @return RACE_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
typedef RACE_ERRCODE (*race_event_callback)(int32_t register_id, race_event_type_enum event_type, void *param, void *user_data);
#else
/**
 * @brief The callback prototype used in race_event_register(). When any race
 * event occurs, the callback will be invoked.
 * @param[in] event_type    The type of the event occurs
 * @param[in] param    The parameter of the event
 * @param[in] user_data    The same user_data input in race_event_register()
 * @return RACE_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
typedef RACE_ERRCODE (*race_event_callback)(race_event_type_enum event_type, void *param, void *user_data);
#endif

////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#ifdef MTK_RACE_EVENT_ID_ENABLE
/**
 * @brief Function for registering the race events. The modules which want to
 * process race events must call this API.
 * @param[in] callback When any race event occurs, the callback registed will be invoked.
 * @param[in] user_data When the callback is invoked, user_data is added to it.
 * @param[out] register_id It identifies the modules which register the RACE events.
 * @return RACE_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
RACE_ERRCODE race_event_register(int32_t *register_id, race_event_callback callback, void *user_data);
#else
/**
 * @brief Function for registering the race events. The modules which want to
 * process race events must call this API.
 * @param[in] callback When any race event occurs, the callback registed will be invoked.
 * @param[in] user_data When the callback is invoked, user_data is added to it.
 * @return RACE_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
RACE_ERRCODE race_event_register(race_event_callback callback, void *user_data);
#endif

#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 */
#endif /* __RACE_EVENT_H__ */
