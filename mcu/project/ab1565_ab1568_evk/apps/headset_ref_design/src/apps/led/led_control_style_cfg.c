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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nvkey.h"
#include "hal.h"
#include "led_control_style_cfg.h"
#include "led_control_internal.h"
#include "nvkey_id_list.h"
#include "syslog.h"
#include "FreeRTOS.h"


//#define     LED_CONFIG_DEBUG_EN
#define     APP_LED_RD_BUFF_SZ  1024

#ifdef     LED_CONFIG_DEBUG_EN
#define     log_led_cfg_info(fmt, cnt, ...)   log_led_info(fmt, cnt, ##__VA_ARGS__)
#else
#define     log_led_cfg_info(...)
#endif
#define     log_led_cfg_error(fmt, cnt, ...)  log_led_error(fmt, cnt, ##__VA_ARGS__)


log_create_module(led_log, PRINT_LEVEL_INFO);
/*****************************************Private variable********************************************************/
static bool                 g_ledcfg_in_nvdm = false;
static led_style_config_t   g_current_style;
static uint8_t             *g_pnvdm_mem  = NULL;
static uint8_t              g_sys_mode   = LED_SYSTEM_MODE_NORMAL;

/* default value for app led, when no configure data in NVDM */
static one_led_style_t   led_default_pattern_setting[] = {
    /* unit,  t0   t1     t2     t3  t1t1rp   ext     br        onoff */
    /* no.0 */
    { 10,   0,  100,   100,    0,     0,    0,      10,    LED_OFF },/*l ed 0 */
    { 10,   0,   50,    50,    0,     1,    2,      10,    LED_OFF },/* led 1 */
    /* no.1 */
    { 10,   0,  255,     0,    0,     0,    0,      10,    LED_ON },/* led 0 */
    { 10,   0,  255,     0,    0,     0,    0,      10,    LED_ON },/* led 1 */
    /* no.2 */
    { 10,   0,  200,   200,    0,     0,    0,      10,    LED_ON }, /* led 0 */
    { 10,   0,  200,   200,    0,     0,    0,      10,    LED_OFF },/* led 1 */
    /* no.3 */
    { 10,   0,   25,    25,    0,     0,    0,      10,    LED_ON }, /* led 0 */
    { 10,   0,  200,   200,    0,     0,    0,      10,    LED_OFF },/* led 1 */
    /* no.4 */
    { 10,   0,  100,     0,    0,     0,    0,      10,    LED_ON }, /* led 0 */
    { 10,   0,  200,   200,    0,     0,    0,      10,    LED_OFF },/* led 1 */
    /* no.5 */
    { 10,   0,  100,     0,    0,     0,    0,      10,    LED_OFF },/* led 0 */
    { 10,   0,  100,   100,    0,     0,    0,      10,    LED_ON }, /* led 1 */
    /* no.6 */
    { 10,   0,  100,     0,    0,     0,    0,      10,    LED_OFF },/* led 0 */
    { 10,   0,  255,   100,    0,     0,    0,      10,    LED_ON }, /* led 1 */
    /* no.7 */
    { 10,   0,  100,     0,    0,     0,    0,      10,    LED_OFF },/* led 0 */
    { 10,   0,   30,    30,    0,     0,    0,      10,    LED_ON }, /* led 1 */
    /* no.8 */
    { 20,   0,  100,   100,    0,     0,    0,      10,    LED_ON }, /* led 0 */
    { 20,   0,  100,   100,    0,     0,    0,      10,    LED_ON }, /* led 1 */
    /* no.9 */
    { 10,   0,    5,     5,   50,     2,    0,      10,    LED_ON }, /* led 0 */
    { 10,   0,    5,     5,   50,     2,    0,      10,    LED_ON }, /* led 1 */
    /* no.10 */
    { 10,   0,    5,     5,   50,     2,    0,      10,    LED_ON }, /* led 0 */
    { 10,   0,    5,     5,   50,     2,    0,      10,    LED_OFF },/* led 1 */
    /* no.11 */
    { 10,   0,    5,     5,   50,     2,    0,      10,    LED_OFF },/* led 0 */
    { 10,   0,    5,     5,   50,     2,    0,      10,    LED_OFF },/* led 1 */
};

/******************************************Public Function**********************************************/

void    led_style_config_deinit()
{
    if (g_ledcfg_in_nvdm) {
        vPortFree(g_pnvdm_mem);
        g_ledcfg_in_nvdm = false;
    }
}


void    led_style_config_init()
{
    nvkey_status_t  status;
    uint32_t        size    = APP_LED_RD_BUFF_SZ;
    uint32_t        unit_sz = 0;
    uint8_t        *temp  = NULL;
    uint32_t        tsize = 1;
    /* If have initialized, then return */
    if (g_ledcfg_in_nvdm) {
        return;
    }
    nvdm_init();
    memset(&g_current_style, 0, sizeof(g_current_style));
    temp = (uint8_t *)pvPortMalloc(APP_LED_RD_BUFF_SZ);
    if (temp == NULL) {
        log_led_cfg_error("[led][cfg] led style config init,malloc read buf fail\r\n", 0);
        goto error_2;
    }
    /* Read led parttern */
    status = nvkey_read_data(NVKEYID_APP_LED_PATTERN, temp, &size);
    if (status != NVKEY_STATUS_OK) {
        log_led_cfg_error("[led][cfg] read led setting from nvdm failed(%d)\r\n", 1, status);
        goto error_2;
    }
    /* Read system mode */
    status = nvkey_read_data(NVKEYID_SYSTEM_MODE_SETTING, &g_sys_mode, &tsize);
    if (status != NVKEY_STATUS_OK) {
        log_led_cfg_error("[led][cfg] read system mode from nvdm failed(%d)\r\n", 1, status);
        g_sys_mode = LED_SYSTEM_MODE_NORMAL;
    }
    /* Check data is valid */
    unit_sz = sizeof(one_led_style_t);
    if (size == 0 || (size - 1) % unit_sz != 0) {
        log_led_cfg_error("[led][cfg] read nvdm data size invalid(%d,expect %d*n)\r\n", 2, (int)size, (int)unit_sz);
        goto error_2;;
    }
    /* Adapter buffer to smaller one */
    g_pnvdm_mem = (uint8_t *)pvPortMalloc(size + 5);
    if (g_pnvdm_mem == NULL) {
        log_led_cfg_error("[led][cfg] led style config init,malloc cfg buf fail\r\n", 0);
        goto error_1;
    }
    memcpy(g_pnvdm_mem, temp, size);
    vPortFree(temp);
    g_ledcfg_in_nvdm = true;
    log_led_cfg_error("[led][cfg] read nvdm succ, size %d\r\n", 1, size);
    return;

error_1:
    vPortFree(g_pnvdm_mem);
error_2:
    vPortFree(temp);
}


led_style_config_t    *led_style_config_read(led_pattern_type_t    pattern, uint8_t  style_no)
{
    uint32_t    led_num = 0;

    one_led_style_t *pstyle = NULL;

    if (g_ledcfg_in_nvdm == true) {
        led_num = g_pnvdm_mem[0];
        pstyle  = (one_led_style_t *)&g_pnvdm_mem[1];
        pstyle += (led_num * style_no);
        g_current_style.led_num     = led_num;
        g_current_style.led_setting = pstyle;
    } else {
        uint32_t led_cfg_sz = 0;

        g_current_style.led_num = 2;
        led_cfg_sz  = sizeof(led_default_pattern_setting) / (sizeof(one_led_style_t) * (g_current_style.led_num));
        if (style_no < led_cfg_sz) {
            g_current_style.led_setting = &led_default_pattern_setting[style_no * g_current_style.led_num];
        } else {
            log_led_cfg_error("[led][cfg] style_no exceed table size(%d)\r\n", 1, led_cfg_sz);
            return NULL;
        }
        log_led_cfg_error("[led][cfg] no led cfg in nvdm, using default\r\n", 0);
    }
    return &g_current_style;
}



uint8_t led_style_config_sys_mode()
{
    return g_sys_mode;
}
