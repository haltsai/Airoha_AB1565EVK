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


#include "race_cmd_feature.h"
#ifdef RACE_LPCOMM_ENABLE
#include "race_lpcomm_conn.h"
#include "race_xport.h"
#include "race_event_internal.h"
#ifdef RACE_FOTA_CMD_ENABLE
#include "race_fota_util.h"
#include "race_fota.h"
#endif

/* On attachment event, there must be some device info to identify different devices.
  * For data send API, input the same device info as the attachment event to let the transport layer get
  * to know which device will the data be sent to.
  * For data receive event, there must be the same info as the attachment event to let the device know
  * which device the data is from.
  * Assign device_id to the new attached device.
  * For multiple partners case,
  * 1) SP should query the partner attachment status and tell the Agent which device it wants to
  *     communicate with by adding device_id in the race cmd id or by other methods.
  * 2) Agent communicates with the partner identified by the device_id from the SP.
  * 3) Partner receives the data with the device_info of the Agent and find its device_id. It sends the data
  *     to the Agent by input the device_id in the send API.
  */


race_lpcomm_conn_info_struct g_lpcomm_conn_info[RACE_LPCOMM_CONN_MAX_NUM];


/* If RACE_LPCOMM_MULTIPLE_LINK_ENABLE is defined, the SP should provide the device_id.
  *   If it's not defined, use RACE_LPCOMM_DEFAULT_DEVICE_ID.*/
bool race_lpcomm_is_attached(uint8_t device_id)
{
    int32_t i = 0;

    for (i = 0; i < RACE_LPCOMM_CONN_MAX_NUM; i++)
    {
        if (g_lpcomm_conn_info[i].is_used &&
            device_id == g_lpcomm_conn_info[i].device_id)
        {
            return TRUE;
        }
    }

    return FALSE;
}


#ifdef RACE_LPCOMM_MULTIPLE_LINK_ENABLE
/* 0 is the invalid device_id. */
uint8_t race_lpcomm_gen_device_id_int(void)
{
    static uint8_t device_id = 0;

    device_id = (++device_id) % 0xFF;
    return 0 == device_id ? ++device_id : device_id;
}
#endif


uint8_t race_lpcomm_gen_device_id(void)
{
#ifdef RACE_LPCOMM_MULTIPLE_LINK_ENABLE
    uint8_t device_id = race_lpcomm_gen_device_id_int();
    uint8_t max_count = RACE_LPCOMM_CONN_MAX_NUM;

    /* RACE_LPCOMM_CONN_MAX_NUM is less than the number of  device_id pool(0xFF -1). */
    while (max_count)
    {
        if (!race_lpcomm_is_attached(device_id))
        {
            break;
        }
        device_id = race_lpcomm_gen_device_id_int();
        max_count--;
    }

    return device_id;
#else
    return RACE_LPCOMM_DEFAULT_DEVICE_ID;
#endif
}


race_lpcomm_conn_info_struct *race_lpcomm_find_conn_info(void *device_data,
                                                                  uint32_t device_data_len,
                                                                  race_lpcomm_role_enum device_role,
                                                                  race_lpcomm_trans_method_enum trans_method)
{

    int32_t i = 0;

    if ((!device_data && device_data_len) ||
        (device_data && !device_data_len))
    {
        return NULL;
    }

    for (i = 0; i < RACE_LPCOMM_CONN_MAX_NUM; i++)
    {
        if (g_lpcomm_conn_info[i].is_used &&
            !g_lpcomm_conn_info[i].device_id)
        {
            RACE_LOG_MSGID_W("Invalid device_id!!", 0);
            g_lpcomm_conn_info[i].is_used = FALSE;
            continue;
        }

        if (g_lpcomm_conn_info[i].is_used &&
            // device_role == g_lpcomm_conn_info[i].device_role &&
            trans_method == g_lpcomm_conn_info[i].trans_method &&
            ((device_data && g_lpcomm_conn_info[i].device_data &&
              0 == memcmp(device_data, g_lpcomm_conn_info[i].device_data, device_data_len)) ||
             (!device_data && !g_lpcomm_conn_info[i].device_data)))
        {
            RACE_LOG_MSGID_I("Found conn_info %d", 1, i);
            return &g_lpcomm_conn_info[i];
        }
    }

    return NULL;
}


/* Return value:
* SUCCESS: device_id.
* FAIL:         0 (0 is the invalid device_id.) */
RACE_ERRCODE race_lpcomm_attach_proc(uint8_t *device_id,
                                         void *device_data,
                                         uint32_t device_data_len,
                                         race_lpcomm_role_enum device_role,
                                         race_lpcomm_trans_method_enum trans_method)
{
    int32_t i = 0;
    race_lpcomm_conn_info_struct *conn_info = NULL;

    if (!device_id ||
        (!device_data && device_data_len) ||
        (device_data && !device_data_len))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    *device_id = 0;

    /* Device has already been attached. */
    conn_info = race_lpcomm_find_conn_info(device_data,
                                           device_data_len,
                                           device_role,
                                           trans_method);
    if (conn_info)
    {
        *device_id = conn_info->device_id;
        return RACE_ERRCODE_SUCCESS;
    }

    /* New attached Device. */
    for (i = 0; i < RACE_LPCOMM_CONN_MAX_NUM; i++)
    {
        if (!g_lpcomm_conn_info[i].is_used)
        {
            g_lpcomm_conn_info[i].is_used = TRUE;
            g_lpcomm_conn_info[i].device_role = device_role;
            g_lpcomm_conn_info[i].trans_method = trans_method;
            g_lpcomm_conn_info[i].device_id = race_lpcomm_gen_device_id();
            *device_id = g_lpcomm_conn_info[i].device_id;
            /* device_data & device_data_len is reserved for the future use. */
            g_lpcomm_conn_info[i].device_data = NULL;
            return RACE_ERRCODE_SUCCESS;
        }
    }


    RACE_LOG_MSGID_W("Not enough slot to stored the info of the attached device.", 0);

    for (i = 0; i < RACE_LPCOMM_CONN_MAX_NUM; i++)
    {
        if (g_lpcomm_conn_info[i].is_used)
        {
            RACE_LOG_MSGID_W("%d conn ifo: device_role:%d trans_method:%d device_id:%d device_data:%x", 5,
                       i,
                       g_lpcomm_conn_info[i].device_role,
                       g_lpcomm_conn_info[i].trans_method,
                       g_lpcomm_conn_info[i].device_id,
                       g_lpcomm_conn_info[i].device_data);
        }
    }
    return RACE_ERRCODE_NOT_ENOUGH_MEMORY;
}


RACE_ERRCODE race_lpcomm_deattach_proc(void *device_data,
                                                uint32_t device_data_len,
                                                race_lpcomm_role_enum device_role,
                                                race_lpcomm_trans_method_enum trans_method)
{
    race_lpcomm_conn_info_struct *conn_info = NULL;

    if ((!device_data && device_data_len) ||
        (device_data && !device_data_len))
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    /* Device has already been attached. */
    conn_info = race_lpcomm_find_conn_info(device_data,
                                           device_data_len,
                                           device_role,
                                           trans_method);
    if (conn_info)
    {
        conn_info->is_used = FALSE;

#ifdef RACE_FOTA_CMD_ENABLE
        /* There is the case that only Agent received the detach event and the partner did not.
                * And AWS attached again very soon. So do not stop FOTA on receiving detach event.
                * FOTA STOP does not clear retry list actually and Ping mechanism will handle the error
                * it may cause.
                */
#ifdef RACE_FOTA_STOP_WHEN_AWS_DETACH
        if (race_fota_is_race_fota_running()
#ifdef RACE_FOTA_DELAY_COMMIT_ENABLE
            && RACE_FOTA_DOWNLOAD_STATE_WAIT_FOR_COMMIT != race_fota_dl_state_get()
#endif
            )
        {
            race_lpcomm_role_enum role = race_fota_get_role();

            race_fota_cntx_struct *fota_cntx = race_fota_cntx_get();

            if (fota_cntx)
            {
                fota_cntx->lpcomm_peer_online = FALSE;
            }

            if (RACE_LPCOMM_ROLE_AGENT == role)
            {
                race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_AGENT,
                               RACE_FOTA_STOP_REASON_PARTNER_LOST);
            }
            else if (RACE_LPCOMM_ROLE_PARTNER == role)
            {
                race_fota_stop(RACE_FOTA_STOP_ORIGINATOR_PARTNER,
                               RACE_FOTA_STOP_REASON_AGENT_LOST);
            }
        }
#endif
#endif
    }

    return RACE_ERRCODE_SUCCESS;
}


#endif /* RACE_LPCOMM_ENABLE */

