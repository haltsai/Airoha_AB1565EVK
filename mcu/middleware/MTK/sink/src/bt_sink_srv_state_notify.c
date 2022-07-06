/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#include "bt_sink_srv.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_state_notify.h"

static bt_sink_srv_state_t bt_sink_srv_state = BT_SINK_SRV_STATE_NONE, g_bt_sink_state;
static bt_sink_srv_state_flags_t bt_sink_srv_state_flag = BT_SINK_SRV_STATE_FLAG_NONE;

extern bt_sink_srv_state_t le_sink_srv_get_state(void);
extern uint16_t bt_sink_srv_get_state_map_table(uint16_t le_state, uint16_t edr_state);
static bt_sink_srv_state_t bt_sink_srv_get_remap_state(bt_sink_srv_state_t state, bt_sink_srv_state_t other_state, bt_sink_srv_state_t curr_state);

/*NEW Design*/
#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:ble_vcs_init_parameter=ble_vcs_init_parameter_default")
#pragma comment(linker, "/alternatename:ble_vcs_get_attribute_handle_tbl=ble_vcs_get_attribute_handle_tbl_default")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_sink_srv_get_state_map_table = bt_sink_srv_get_state_map_table_default
#pragma weak bt_sink_srv_edr_state_change_callback = bt_sink_srv_edr_state_change_callback_default
#else
#error "Unsupported Platform"
#endif

static void bt_sink_srv_state_change_notify(bt_sink_srv_state_t previous, bt_sink_srv_state_t now)
{
    bt_sink_srv_event_param_t *params = bt_sink_srv_memory_alloc(sizeof(*params));

    params->state_change.previous = previous;
    params->state_change.current = now;

    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_STATE_CHANGE, params, sizeof(*params));
    bt_sink_srv_memory_free(params);
}

void bt_sink_srv_state_set(bt_sink_srv_state_t state)
{
    if (BT_SINK_SRV_STATE_CONNECTED == state) {
        bt_sink_srv_state_flag |=  BT_SINK_SRV_STATE_FLAG_CONNECTED;
    } else if (BT_SINK_SRV_STATE_POWER_ON == state ||BT_SINK_SRV_STATE_NONE == state ) {
        bt_sink_srv_state_flag &= ~BT_SINK_SRV_STATE_FLAG_CONNECTED;
    }
    bt_sink_srv_report_id("[Sink][State] set prev:0x%x, next:0x%x", 2, bt_sink_srv_state, state);
/* don't notify BT_SINK_SRV_STATE_CONNECTED && BT_SINK_SRV_STATE_POWER_ON for new CM structure */
#ifdef MTK_BT_CM_SUPPORT
    if (BT_SINK_SRV_STATE_CONNECTED == state || BT_SINK_SRV_STATE_POWER_ON == state) {
        state = BT_SINK_SRV_STATE_NONE;
        bt_sink_srv_report_id("[Sink][State] set prev:0x%x, re-assign next:0x%x", 2, bt_sink_srv_state, state);
    }
#endif
    if (bt_sink_srv_state != state) {
#if defined (AIR_LE_AUDIO_ENABLE)
        bt_sink_srv_edr_state_change_callback(bt_sink_srv_state, state);
        bt_sink_srv_map_new_state_notify(state, BT_SINK_SRV_STATE_EDR_TYPE);
#else
        bt_sink_srv_state_change_notify(bt_sink_srv_state, state);
#endif
        bt_sink_srv_state = state;
    }
}

void bt_sink_srv_state_reset(bt_sink_srv_state_t state)
{
    bt_sink_srv_report_id("[Sink][State] reset prev:0x%x, next:0x%x", 2, bt_sink_srv_state, state);

   if (state == BT_SINK_SRV_STATE_STREAMING) {
        if (bt_sink_srv_state >= BT_SINK_SRV_STATE_INCOMING && bt_sink_srv_state <= BT_SINK_SRV_STATE_MULTIPARTY) {
            return;
        }
/* don't notify BT_SINK_SRV_STATE_CONNECTED && BT_SINK_SRV_STATE_POWER_ON for new CM structure */
#ifdef MTK_BT_CM_SUPPORT
        state = BT_SINK_SRV_STATE_NONE;
#else
        if (bt_sink_srv_state_flag & BT_SINK_SRV_STATE_FLAG_CONNECTED) {
            state = BT_SINK_SRV_STATE_CONNECTED;
        } else {
            state = BT_SINK_SRV_STATE_POWER_ON;
        }
#endif
   }
    if (bt_sink_srv_state !=state) {
#if defined (AIR_LE_AUDIO_ENABLE)
        bt_sink_srv_edr_state_change_callback(bt_sink_srv_state, state);
        bt_sink_srv_map_new_state_notify(state, BT_SINK_SRV_STATE_EDR_TYPE);
#else
        bt_sink_srv_state_change_notify(bt_sink_srv_state, state);
#endif
        bt_sink_srv_state = state;
    }
    bt_sink_srv_report_id("[Sink][State] final :0x%x", 1, bt_sink_srv_state);
}

bt_sink_srv_state_t bt_sink_srv_get_state(void)
{
    return bt_sink_srv_state;
}



/************************************************
* Static functions
*************************************************/
uint16_t bt_sink_srv_get_state_map_table_default(uint16_t le_state, uint16_t edr_state)
{
    return 0;
}

void bt_sink_srv_edr_state_change_callback_default(bt_sink_srv_state_t previous, bt_sink_srv_state_t now)
{
}

static bt_sink_srv_state_t bt_sink_srv_get_remap_state(bt_sink_srv_state_t state, bt_sink_srv_state_t other_state, bt_sink_srv_state_t curr_state)
{
    uint16_t map_state;
    if (other_state == 0) {/*one state is 0*/
        map_state = bt_sink_srv_get_state_map_table(state, curr_state);
    } else {
        map_state = bt_sink_srv_get_state_map_table(state, other_state);
    }
    return map_state;
}


static bool bt_sink_srv_check_need_remap(bt_sink_srv_state_t state, bt_sink_srv_state_type_t type)
{
    bool need_remap = true;
    bt_sink_srv_state_t le_state = le_sink_srv_get_state();
    bt_sink_srv_state_t edr_state = bt_sink_srv_get_state();
    if (le_state  <  BT_SINK_SRV_STATE_STREAMING && edr_state < BT_SINK_SRV_STATE_STREAMING) {
        if (state < BT_SINK_SRV_STATE_STREAMING) {
            // reset DO nothing
            need_remap = false;
        }
    } else if (le_state <  BT_SINK_SRV_STATE_STREAMING && type == BT_SINK_SRV_STATE_EDR_TYPE) {
        //no do change
        need_remap = false;
    } else if (edr_state <  BT_SINK_SRV_STATE_STREAMING && type == BT_SINK_SRV_STATE_LE_AUDIO_TYPE) {
        //no do change
        need_remap = false;
    }
    bt_sink_srv_report_id("[Sink][State]new_state_notify,need_remap:0x%x", 1, need_remap);
    return need_remap;

}


void bt_sink_srv_map_new_state_notify(bt_sink_srv_state_t state, bt_sink_srv_state_type_t type)
{
    bt_sink_srv_state_t temp_state = 0, other_temp_state = 0;
    bt_sink_srv_state_t le_state = le_sink_srv_get_state();
    bt_sink_srv_state_t edr_state = bt_sink_srv_get_state();
    bt_sink_srv_report_id("[Sink][State]new_state_notify,state:0x%x, type:0x%x", 2, state, type);

    bool check_need_remap = bt_sink_srv_check_need_remap(state, type);

   bt_sink_srv_report_id("[Sink][State]map_new_state_notify, curr_le_state:0x%x, curr_edr_state:0x%x, state:%x", 3, le_state, edr_state, g_bt_sink_state);

    if (check_need_remap) {
        if (g_bt_sink_state <  BT_SINK_SRV_STATE_STREAMING) {
            if (state < BT_SINK_SRV_STATE_STREAMING) {
                return;/*in idle , idle not notify*/
            }
        } else if  (g_bt_sink_state >= BT_SINK_SRV_STATE_STREAMING) {
            if (type == BT_SINK_SRV_STATE_EDR_TYPE) {
                temp_state = le_state;
                other_temp_state = edr_state;
            } else if (type == BT_SINK_SRV_STATE_LE_AUDIO_TYPE) {
                temp_state = edr_state;
                other_temp_state = le_state;
            }
            if (temp_state >= BT_SINK_SRV_STATE_STREAMING) {
                if (state > BT_SINK_SRV_STATE_STREAMING) {
                    if (state > g_bt_sink_state) {
                        if (g_bt_sink_state > BT_SINK_SRV_STATE_STREAMING) {
                            state = bt_sink_srv_get_remap_state(state, temp_state, other_temp_state);
                            if (state == BT_SINK_STATE_INVAILD) {
                                return;
                            }
                        } else {
                            //one same type nothing to do /*streaming --> active /incoming*/
                        }
                    } else {
                        state = bt_sink_srv_get_remap_state(state, temp_state, other_temp_state);
                        if (state == BT_SINK_STATE_INVAILD) {
                            return;
                        }
                    }
                } else {
                    if (g_bt_sink_state > BT_SINK_SRV_STATE_ACTIVE) {/*before has streaming now streaming is terminate*/
                        state = temp_state;/*combin state --> single state*/
                    } else {
                        if (other_temp_state <= temp_state) {
                            return; /*one end, on streaming/incoming and le &edr are not same*/
                        } else {
                            state = temp_state;/*4-->20 , 20 -->0 should notify , 20-->4*/
                        }
                    }
                }
            }
        }
    }
    
    bt_sink_srv_report_id("[Sink][State]map_state_notify,final,sink_state:0x%x, state:0x%x", 2, g_bt_sink_state, state);
    if (g_bt_sink_state != state) {
        bt_sink_srv_state_change_notify(g_bt_sink_state, state);
        g_bt_sink_state = state;
        bt_sink_srv_report_id("[Sink][State]map_new_state_notify,bt_state:0x%x", 1, g_bt_sink_state);
    }
}

