/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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

#include "sy3088.h"
#include "assert.h"

static sy3088_context_t context = {.is_initalized = false};

static sy3088_write_ptr sy3088_write;
static sy3088_write_ptr sy3088_read;

static int sy3088_write_reg(uint8_t reg, uint8_t val)
{
    return sy3088_write(SY3088PXS_I2C_ADDRESS, reg, &val, 1);
}
static int sy3088_read_reg(uint8_t reg, uint8_t *val)
{
    return sy3088_read(SY3088PXS_I2C_ADDRESS, reg, val, 1);
}

static int sy3088_clear_pxs_interrupt(void)
{
    return sy3088_write_reg(CLEAR_PXS_FLAG, 0);
}

static int sy3088_software_reset(void)
{
    return sy3088_write_reg(SOFTWARE_RESET, 0);
}

static int sy3088_set_pxs_low_threshold(uint8_t val)
{
    return sy3088_write_reg(REG_PXS_TL, val);
}

static int sy3088_set_pxs_high_threshold(uint8_t val)
{
    return sy3088_write_reg(REG_PXS_TH, val);
}

static int sy3088_enable_pxs_sensing(void)
{
    return sy3088_write_reg(REG_CON0, AP_EN_PXS);
}
static int sy3088_disable_pxs_sensing(void)
{
    return sy3088_write_reg(REG_CON0, 0);
}

static int sy3088_read_pxs_data(uint8_t *pxs_data)
{
    return sy3088_read_reg(REG_PXS_DATA, pxs_data);
}


static int sy3088_set_default_configuration(void)
{
    int status = 0;

    status = sy3088_write_reg(REG_CON1, PXS_PITYPE_WINDOW | PXS_WAIT | PXS_IR_DRV | PXS_PRST);

    context.high_threshold = PXS_OFFSET_MAX;
    context.low_threshold = PXS_OFFSET_MAX-1;
    context.baseline = PXS_OFFSET_MAX;
    context.status = PXS_STATUS_INIT;

    sy3088_set_pxs_low_threshold(context.low_threshold);
    sy3088_set_pxs_high_threshold(context.high_threshold);
    sy3088_clear_pxs_interrupt();
    return status;
}

static sy3088_pxs_sample_t sy3088_pxs_data =
{
    255,                       //Samples avg
    false,
    0,                         //Current Index
    {255, 255, 255, 255, 255}, //PS samples
};


static void sy3088_reset_pxs_samples(void)
{
    int i = 0;

    for (i = 0; i < PXS_DATA_BUFFER_SIZE; i++) {
        sy3088_pxs_data.data[i] = 255;
    }

    sy3088_pxs_data.is_stable = false;
    sy3088_pxs_data.average = 255;
    sy3088_pxs_data.index = 0;
}

int sy3088_init(sy3088_write_ptr write_ptr, sy3088_read_ptr read_ptr)
{
    uint8_t vendor_id = 0;
    if (!write_ptr || !read_ptr) {
        return -1;
    }

    sy3088_write = write_ptr;
    sy3088_read = read_ptr;

    sy3088_reset_pxs_samples();

    sy3088_set_default_configuration();
    sy3088_enable_pxs_sensing();

    int ret = sy3088_read_reg(REG_ID, &vendor_id);
    if (ret != 0) {
        log_hal_msgid_info("[bsp][irsensor] the damaged ir component, ret=%d\r\n", 1, ret);
        return ret;
    } else {
        log_hal_msgid_info("[bsp][irsensor] success, vendor_id=0x%x\r\n", 1, vendor_id);
    }
    context.is_initalized = true;
    return 0;
}

void sy3088_deinit(void)
{
    sy3088_software_reset();
    sy3088_disable_pxs_sensing();
}


static bool sy3088_process_sample_data(uint8_t pxs_data)
{
    sy3088_pxs_sample_t *ps_sample = &sy3088_pxs_data;

    uint8_t j = 0;
    uint32_t sum = 0;
    uint32_t slope_sum = 0;

    ps_sample->data[ps_sample->index++] = pxs_data;
    ps_sample->index %= PXS_DATA_BUFFER_SIZE;

    for (j = 0; j < PXS_DATA_BUFFER_SIZE; j++) {
        sum += ps_sample->data[j];
    }
    for (j = 0; j < PXS_DATA_BUFFER_SIZE - 1; j++) {
        slope_sum +=  PXS_ABS(ps_sample->data[j] - ps_sample->data[j+1]);
    }
    ps_sample->average = sum / PXS_DATA_BUFFER_SIZE;

    log_hal_msgid_info("[bsp][psensor] ps_data[%d,%d,%d,%d,%d]\r\n", 5, \
        ps_sample->data[0], ps_sample->data[1], ps_sample->data[2], ps_sample->data[3], ps_sample->data[4]);
    log_hal_msgid_info("[bsp][psensor] sum=%d ave=%d slope_sum=%d\r\n", 3, sum, ps_sample->average, slope_sum);

    if(slope_sum < PXS_STABLE_SLOPE) {
        ps_sample->is_stable = true;
        return true;
    } else {
        ps_sample->is_stable = false;
        return false;
    }
}

static void sy3088_update_threshold(sy3088_status_t new_status, uint8_t *low_threshold, uint8_t *high_threshold)
{
    if (!low_threshold || !high_threshold) {
        assert(0);
        return;
    }

    context.status = new_status;
    if (new_status == PXS_STATUS_FAR_AWAY) {
        /*set high/low threshold*/
        uint8_t max_count = (context.baseline + PXS_NEAR_DIFF);

        /* high threshold can not set to 255, or, near event will not be triggered. */
        *high_threshold = (max_count <=254) ? max_count : 254;
        /* low threshold is less than crosstalk value, far away event will not always be triggered in stable status. */
        if (!context.is_init_saturated) {
            *low_threshold = /*context.baseline > PXS_LOW_DIFF ? (context.baseline - PXS_LOW_DIFF) :*/ 0;
        }

    } else if (new_status == PXS_STATUS_NEAR_BY) {
        *low_threshold  = context.baseline + PXS_FAR_DIFF;
        *high_threshold = PXS_FAR_MAX;
    }
}

bool sy3088_get_sensor_status(sy3088_status_t *status)
{
    uint8_t high_threshold = 0;
    uint8_t low_threshold = 0;
    uint8_t pxs_data = 0;
    static sy3088_status_t last_status = PXS_STATUS_INIT;
#ifdef BSP_PSENSOR_KEY_TIMING_DEBUG_ENABLE
    uint32_t count;
    static uint32_t last_count = 0;
#endif

    if (!context.is_initalized) {
        log_hal_msgid_error("[bsp][psensor] sensor is not initialized.\r\n", 0);
        return false;
    }
#ifdef BSP_PSENSOR_KEY_TIMING_DEBUG_ENABLE
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &count);
    count /= 1000;
#endif
    if (sy3088_read_pxs_data(&pxs_data) < 0) {
        log_hal_msgid_error("[bsp][psensor] sy3088_read_pxs_data error\r\n", 0);
        return false;
    }
#ifdef BSP_PSENSOR_KEY_TIMING_DEBUG_ENABLE
    log_hal_msgid_info("[bsp][psensor] %d,%d,%d,\r\n", 3, count, (count - last_count), pxs_data);
    last_count = count;
#endif
    sy3088_clear_pxs_interrupt();

    high_threshold = context.high_threshold;
    low_threshold = context.low_threshold;
    log_hal_msgid_info("[bsp][psensor] current low_threshold=%d high_threshold=%d baseline=%d context.status=%d pxs_data=%d\r\n", 5,\
        low_threshold, high_threshold, context.baseline, context.status, pxs_data);

    sy3088_process_sample_data(pxs_data);

    switch (context.status) {
        case PXS_STATUS_INIT:
            if (pxs_data < low_threshold) {
                /* in the Far Away state，and the state is stable，sampling the PXS data and calculating the average as the background noise(also called optical crosstalk) */
                if (sy3088_pxs_data.is_stable) {
                    context.baseline = sy3088_pxs_data.average;

                    sy3088_update_threshold(PXS_STATUS_FAR_AWAY, &low_threshold, &high_threshold);

                    log_hal_msgid_info("[bsp][psensor] init update new baseline=%d \r\n", 1, context.baseline);
                }
            } else if (pxs_data > high_threshold) {
                    /*in this case, there may be some problems in the mechanical structure or the sensor is occluded by the sensing target. */
                    context.is_init_saturated = true;

                    sy3088_update_threshold(PXS_STATUS_NEAR_BY, &low_threshold, &high_threshold);

                    log_hal_msgid_info("[bsp][psensor] sensor is initialed with something on surface. This is considered to be near event. update low_threshold=%d high_threshold=%d \n", 2, \
                        low_threshold, high_threshold);
            } else {
                log_hal_msgid_info("[bsp][psensor] middle value pxs_data=%d", 1, pxs_data);
            }
            break;
        case PXS_STATUS_NEAR_BY:
            if (pxs_data < low_threshold) {

                sy3088_update_threshold(PXS_STATUS_FAR_AWAY, &low_threshold, &high_threshold);

                log_hal_msgid_info("[bsp][psensor] FAR state\n", 0);
            } else {
                log_hal_msgid_warning("[bsp][psensor] NEAR to FAR state\r\n", 0);
                goto exit_process;
            }

            break;
        case PXS_STATUS_FAR_AWAY:
            if (pxs_data > high_threshold) {

                if (pxs_data > PXS_FAR_MAX - 1) {
                    //to do something?
                }

                sy3088_update_threshold(PXS_STATUS_NEAR_BY, &low_threshold, &high_threshold);

                log_hal_msgid_info("[bsp][psensor] NEAR state\r\n", 0);
            } else if (pxs_data < low_threshold) {
                if (context.is_init_saturated) {
                    log_hal_msgid_warning("[bsp][psensor] init_saturated\r\n", 0);
                    if (sy3088_pxs_data.is_stable) {
                        context.is_init_saturated = false;
                        context.baseline = sy3088_pxs_data.average;

                        sy3088_update_threshold(PXS_STATUS_FAR_AWAY, &low_threshold, &high_threshold);
                        log_hal_msgid_info("[bsp][psensor] far away update new baseline=%d \r\n", 1, context.baseline);
                    } else {
                        log_hal_msgid_warning("[bsp][psensor] pxs is not stable\r\n", 0);
                        goto exit_process;
                    }
                } else {
                    log_hal_msgid_warning("[bsp][psensor] FAR to NEAR state, pxs_data < low_threshold\r\n", 0);
                    goto exit_process;
                }
            } else {
                log_hal_msgid_warning("[bsp][psensor] FAR to NEAR state, high_threshold > pxs_data > low_threshold\r\n", 0);
                goto exit_process;
            }

            break;
        default:
            log_hal_msgid_warning("[bsp][psensor] event unknown\r\n", 0);
            goto exit_process;
    }

    context.high_threshold = high_threshold & 0xFF;
    context.low_threshold = (low_threshold < PXS_FAR_MAX) ? low_threshold : PXS_FAR_MAX - PXS_FAR_DIFF; 

    sy3088_set_pxs_high_threshold(context.high_threshold);
    sy3088_set_pxs_low_threshold(context.low_threshold);

exit_process:
    log_hal_msgid_info("[bsp][psensor] new low_threshold=%d high_threshold=%d baseline=%d context.status=%d\r\n", 4\
        , context.low_threshold, context.high_threshold, context.baseline, context.status);

    if (context.status != PXS_STATUS_INIT) {
        if (last_status != context.status) {
            *status = context.status;
            last_status = context.status;
            return true;
        } else {
            log_hal_msgid_warning("[bsp][psensor] repeat event=%d \r\n", 1, context.status);
            return false;
        }
    } else {
        return false;
    }
}

