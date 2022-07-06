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

#include "hal_captouch.h"
#ifdef HAL_CAPTOUCH_MODULE_ENABLED
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hal_log.h"
#include <string.h>

#include "hal_gpt.h"
#include "airo_key_event.h"
#include "airo_key_event_internal.h"
#include "ept_keypad_drv.h"
#include "hal_captouch_internal.h"
#include "nvdm.h"
#include "nvkey.h"
#include "nvkey_id_list.h"

#define AIRO_CAPTOUCH_DEBUG

#ifdef HAL_CPT_FEATURE_4CH
#define CAPTOUCH_MAX_CHANNEL    4
uint8_t default_mavg_r[CAPTOUCH_MAX_CHANNEL] = {10,10,10,10};
uint8_t default_avg_s[CAPTOUCH_MAX_CHANNEL] = {5,5,5,5};
uint8_t default_coarse_cap[CAPTOUCH_MAX_CHANNEL] = {3,3,3,3};
int16_t default_high_thr[CAPTOUCH_MAX_CHANNEL] = {150, 150, 150, 150};
int16_t default_low_thr[CAPTOUCH_MAX_CHANNEL] = {50, 50, 50, 50};
#else
#define CAPTOUCH_MAX_CHANNEL    8
#endif

static void airo_captouch_callback(void *parameter)
{
    hal_captouch_event_t    hal_captouch_event;
    airo_key_mapping_event_t key_event;
    hal_captouch_status_t   ret;

    ret = hal_captouch_get_event(&hal_captouch_event);

    if (ret == HAL_CAPTOUCH_STATUS_NO_EVENT) {
        return;
    }

    key_event.state        = hal_captouch_event.state;
    key_event.time_stamp   = hal_captouch_event.time_stamp;
    key_event.key_data     = hal_captouch_event.key_data;
    LOG_W(MPLOG, "[captouch] state=%d, key_data=%d  0x%x\r\n", key_event.state, key_event.key_data, key_event.time_stamp );
    airo_key_process_key(&key_event, AIRO_KEY_CAPTOUCH);
}
/*
#ifdef AIRO_CAPTOUCH_DEBUG
uint8_t t_data1[256];
uint8_t t_data2[256];
void print_arry(uint8_t *data, uint32_t size, uint8_t index)
{

    uint8_t *pdata;
    uint32_t ret_len, i;
    uint32_t print_len;
    uint32_t data_len;

    data_len = size;

    memcpy(t_data1, data, size);

    ret_len = 0;
    print_len = 0;

    pdata = (uint8_t *)t_data1;

    ret_len = sprintf((char *)(t_data2 + print_len), "NO.%d-len[%d]:", (int)index, (int)data_len);
    print_len = print_len + ret_len;

    for (i = 0; i < data_len; i++) {
        ret_len = sprintf((char *)(t_data2 + print_len), "%x ", *(pdata++));
        print_len = print_len + ret_len;
    }

    LOG_I(common, "%s\r\n", (char *)t_data2);
}
#endif
*/
#ifdef HAL_CPT_FEATURE_4CH
bool airo_captouch_init(void)
{
    uint32_t i, temp = 0;
    hal_captouch_config_t config;
    //hal_captouch_tune_data_t tune_data;
    hal_captouch_nvdm_data captouch_data;
    nvdm_status_t ret = NVDM_STATUS_ERROR;
    hal_captouch_status_t cap_ret;

    memset(&config, 0, sizeof(hal_captouch_config_t));

    config.callback.callback  = airo_captouch_callback;
    config.callback.user_data = NULL;

    //get nvdm data
    nvdm_init();
    memset(&captouch_data, 0, sizeof(hal_captouch_nvdm_data));
    temp = sizeof(hal_captouch_nvdm_data);
    ret = nvkey_read_data(NVKEYID_CAPTOUCH_CALI_4CHDATA, (uint8_t *)(&captouch_data), &temp);
    //if no nvdm data,set default value; else set the nvdm channel map;
    captouch_data.hw_ch_map = DRV_KBD_CAPTOUCH_SEL;
    if (ret == NVDM_STATUS_OK) {
        config.channel_bit_map = captouch_data.en_ch_map;
        config.swtune_en       = captouch_data.swtune_en;
        config.swDebounceTime  = captouch_data.swDebounceTime;

    } else {
        config.channel_bit_map  = captouch_data.hw_ch_map;
        captouch_data.en_ch_map = captouch_data.hw_ch_map;
    }

    LOG_MSGID_I(common,"captouch read nvdm status = %d",1,ret);

    for (i = 0; i < CAPTOUCH_MAX_CHANNEL; i++) {
        LOG_MSGID_I(common, "read nvdm:ch:%d, is_key=%d, crs_cap=%d, f_cap=%d, is_thr=%d,h=%d,l=%d is_rs=%d mavg_r=%d avg_s=%d\r\n",10, \
                    i, captouch_data.is_key[i], captouch_data.coarse_cap[i], captouch_data.fine_cap[i], \
                    captouch_data.is_thr[i], captouch_data.thr_h[i], captouch_data.thr_l[i], captouch_data.is_rs[i], captouch_data.mavg_r[i], captouch_data.avg_s[i]);
    }
    LOG_MSGID_I(common, "read nvdm: hw_map=0x%x,en_map=0x%x\r\n",2, captouch_data.hw_ch_map, captouch_data.en_ch_map);


    //set the coarse cap,threshold according to the enable channel map. if no nvdm data,set the default value
    for (i = 0; i < CAPTOUCH_MAX_CHANNEL; i++) {
        if ((1 << i)&config.channel_bit_map) {
            //if no nvdm data,set the mavg_r and avg_s default value, else set the nvdm data
            if ((captouch_data.is_key[i] == true) && (ret == NVDM_STATUS_OK)) {
                config.coarse_cap[i] = captouch_data.coarse_cap[i];
                config.mavg_r[i]     = captouch_data.mavg_r[i];
                config.avg_s[i]      = captouch_data.avg_s[i];
                config.fine_cap[i]   = captouch_data.fine_cap[i];

            } else {
                config.coarse_cap[i] = default_coarse_cap[i];
                config.mavg_r[i] = default_mavg_r[i];
                config.avg_s[i]  = default_avg_s[i];
                captouch_data.mavg_r[i] = config.mavg_r[i];
                captouch_data.avg_s[i]  = config.avg_s[i];
                captouch_data.coarse_cap[i] = config.coarse_cap[i];

            }

            if ((captouch_data.is_thr[i] == true) && (ret == NVDM_STATUS_OK)) {
                config.high_thr[i] = captouch_data.thr_h[i];
                config.low_thr[i]  = captouch_data.thr_l[i];
            } else {
                config.high_thr[i] = default_high_thr[i];
                config.low_thr[i]  = default_low_thr[i];
                captouch_data.thr_h[i] = config.high_thr[i];
                captouch_data.thr_l[i] = config.low_thr[i];
            }
        }
    }

    cap_ret = hal_captouch_init(&config);
    if (cap_ret != HAL_CAPTOUCH_STATUS_OK) {
        LOG_MSGID_I(common, " hal_captouch_init fail=%d\r\n",1, cap_ret);
    }
#if 0
    //according to the enable channel map to tune the coarse cap value automatically.
    //if the coarse value has been tuned, skip
    for (i = 0; i < CAPTOUCH_MAX_CHANNEL; i++) {
        if ((1 << i)&config.channel_bit_map) {
            if ((captouch_data.is_key[i] == false) || (ret != NVDM_STATUS_OK)) {
                cap_ret = hal_captouch_tune_control(i, HAL_CAPTOUCH_TUNE_HW_AUTO, &tune_data);
                if (cap_ret == HAL_CAPTOUCH_STATUS_OK) {
                    captouch_data.is_key[i]     = true;
                    captouch_data.coarse_cap[i] = tune_data.coarse_cap;
                    captouch_data.fine_cap[i]   = tune_data.fine_cap;
                    LOG_MSGID_I(common, "tune: ch:%d, crs_cap=%d, f_cap=%d, man_cap=%d, avg=%d,vadc=%d\r\n",6, \
                                i, tune_data.coarse_cap, tune_data.fine_cap, tune_data.man, tune_data.avg_adc, tune_data.vadc);
                } else {
                    captouch_data.is_key[i]     = false;
                    captouch_data.coarse_cap[i] = tune_data.coarse_cap;
                    captouch_data.fine_cap[i]   = tune_data.fine_cap;
                    LOG_MSGID_I(common, "tune: ch:%d, crs_cap=%d, f_cap=%d, man_cap=%d, avg=%d,vadc=%d fail\r\n",6, \
                                i, tune_data.coarse_cap, tune_data.fine_cap, tune_data.man, tune_data.avg_adc, tune_data.vadc);
                }
            }
        }
    }
    LOG_MSGID_I(common, "write nvdm: hw_map=%x,en_map=%x,is_rs=%d,r=%d,s=%d\r\n",2, captouch_data.hw_ch_map, captouch_data.en_ch_map);
    for (i = 0; i < CAPTOUCH_MAX_CHANNEL; i++) {
        LOG_MSGID_I(common, "write nvdm:ch:%d, is_key=%d, crs_cap=%d, f_cap=%d, is_thr=%x,h=%d,l=%d is_rs=%d mavg_r=%d avg_s=%d\r\n",10, \
                    i, captouch_data.is_key[i], captouch_data.coarse_cap[i], captouch_data.fine_cap[i], \
                    captouch_data.is_thr[i], captouch_data.thr_h[i], captouch_data.thr_l[i], captouch_data.is_rs[i], captouch_data.mavg_r[i], captouch_data.avg_s[i]);
    }

#ifdef AIRO_CAPTOUCH_DEBUG
    print_arry((uint8_t *)(&captouch_data), sizeof(hal_captouch_nvdm_data), 1);
#endif
    nvkey_write_data(NVKEYID_CAPTOUCH_CALI_DATA, (uint8_t *)(&captouch_data), sizeof(hal_captouch_nvdm_data));
#endif

    return true;
}

#else

bool airo_captouch_init(void)
{
    uint32_t i, temp;
    hal_captouch_config_t config;
    hal_captouch_tune_data_t tune_data;
    hal_captouch_nvdm_data captouch_data;
    nvdm_status_t ret;
    hal_captouch_status_t cap_ret;

    config.callback.callback  = airo_captouch_callback;
    config.callback.user_data = NULL;

    //get nvdm data
    nvdm_init();
    memset(&captouch_data, 0, sizeof(hal_captouch_nvdm_data));
    temp = sizeof(hal_captouch_nvdm_data);
    ret = nvkey_read_data(NVKEYID_CAPTOUCH_CALI_DATA, (uint8_t *)(&captouch_data), &temp);

    //if no nvdm data,set default value; else set the nvdm channel map;
    captouch_data.hw_ch_map = DRV_KBD_CAPTOUCH_SEL;
    if (ret == NVDM_STATUS_OK) {
        config.channel_bit_map = captouch_data.en_ch_map;
    } else {
        config.channel_bit_map  = captouch_data.hw_ch_map;
        captouch_data.en_ch_map = captouch_data.hw_ch_map;
    }

    for (i = 0; i < 8; i++) {
        LOG_MSGID_I(common, "read nvdm:ch:%d, is_key=%d, crs_cap=%d, f_cap=%d, is_thr=%x,h=%d,l=%d\r\n",7, \
                     i, captouch_data.is_key[i], captouch_data.coarse_cap[i], captouch_data.fine_cap[i], \
                     captouch_data.is_thr[i], captouch_data.thr_h[i], captouch_data.thr_l[i]);
    }

    LOG_MSGID_I(common, "read nvdm: hw_map=%x,en_map=%x,is_rs=%d,r=%d,s=%d\r\n",5, \
                 captouch_data.hw_ch_map, captouch_data.en_ch_map, \
                 captouch_data.is_rs, captouch_data.mavg_r, captouch_data.avg_s);

    //set the coarse cap,threshold according to the enable channel map. if no nvdm data,set the default value
    for (i = 0; i < CAPTOUCH_MAX_CHANNEL; i++) {
        if ((1 << i)&config.channel_bit_map) {
            if ((captouch_data.is_key[i] == true) && (ret == NVDM_STATUS_OK)) {
                config.coarse_cap[i] = captouch_data.coarse_cap[i];
            } else {
                config.coarse_cap[i] = 7;
            }

            if ((captouch_data.is_thr[i] == true) && (ret == NVDM_STATUS_OK)) {
                config.high_thr[i] = (int32_t)captouch_data.thr_h[i];
                config.low_thr[i]  = (int32_t)captouch_data.thr_l[i];
            } else {
                config.high_thr[i] = 200;
                config.low_thr[i]  = 50;
                captouch_data.thr_h[i] = (int16_t)config.high_thr[i];
                captouch_data.thr_l[i] = (int16_t)config.low_thr[i];
            }
        }
    }

    //if no nvdm data,set the mavg_r and avg_s default value, else set the nvdm data
    if ((captouch_data.is_rs == true) && (ret == NVDM_STATUS_OK)) {
        config.mavg_r        = captouch_data.mavg_r;
        config.avg_s         = captouch_data.avg_s;
    } else {
        config.mavg_r = 10;  //use the default value 10
        config.avg_s  = 5;  //use the default value 5
        captouch_data.mavg_r = config.mavg_r;
        captouch_data.avg_s  = config.avg_s;
    }
    cap_ret = hal_captouch_init(&config);

    if (cap_ret != HAL_CAPTOUCH_STATUS_OK) {
        LOG_MSGID_I(common, " hal_captouch_init fail=%d\r\n",1, cap_ret);
    }

    //according to the enable channel map to tune the coarse cap value automatically.
    //if the coarse value has been tuned, skip
    for (i = 0; i < CAPTOUCH_MAX_CHANNEL; i++) {
        if ((1 << i)&config.channel_bit_map) {
            if ((captouch_data.is_key[i] == false) || (ret != NVDM_STATUS_OK)) {
                cap_ret = hal_captouch_tune_control(i, HAL_CAPTOUCH_TUNE_HW_AUTO, &tune_data);
                if (cap_ret == HAL_CAPTOUCH_STATUS_OK) {
                    captouch_data.is_key[i]     = true;
                    captouch_data.coarse_cap[i] = tune_data.coarse_cap;
                    captouch_data.fine_cap[i]   = tune_data.fine_cap;
                    LOG_MSGID_I(common, "tune: ch:%d, crs_cap=%d, f_cap=%d, man_cap=%d, avg=%d,vadc=%d\r\n",6, \
                                 i, tune_data.coarse_cap, tune_data.fine_cap, tune_data.man, tune_data.avg_adc, tune_data.vadc);
                } else {
                    captouch_data.is_key[i]     = false;
                    captouch_data.coarse_cap[i] = tune_data.coarse_cap;
                    captouch_data.fine_cap[i]   = tune_data.fine_cap;
                    LOG_MSGID_I(common, "tune: ch:%d, crs_cap=%d, f_cap=%d, man_cap=%d, avg=%d,vadc=%d fail\r\n",6, \
                                 i, tune_data.coarse_cap, tune_data.fine_cap, tune_data.man, tune_data.avg_adc, tune_data.vadc);
                }
            }
        }
    }

    LOG_MSGID_I(common, "write nvdm: hw_map=%x,en_map=%x,is_rs=%d,r=%d,s=%d",5, \
                 captouch_data.hw_ch_map, captouch_data.en_ch_map, \
                 captouch_data.is_rs, captouch_data.mavg_r, captouch_data.avg_s);

    for (i = 0; i < CAPTOUCH_MAX_CHANNEL; i++) {
        LOG_MSGID_I(common, "read nvdm:ch:%d, is_key=%d, crs_cap=%d, f_cap=%d, is_thr=%x,h=%d,l=%d\r\n",7, \
                     i, captouch_data.is_key[i], captouch_data.coarse_cap[i], captouch_data.fine_cap[i], \
                     captouch_data.is_thr[i], captouch_data.thr_h[i], captouch_data.thr_l[i]);
    }
/*
#ifdef AIRO_CAPTOUCH_DEBUG
    print_arry((uint8_t *)(&captouch_data), sizeof(hal_captouch_nvdm_data), 1);
#endif
*/
    nvkey_write_data(NVKEYID_CAPTOUCH_CALI_DATA, (uint8_t *)(&captouch_data), sizeof(hal_captouch_nvdm_data));

    return true;
}
#endif

#endif /*MTK_KEYPAD_ENABLE*/

