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

#include "hal_i2c_master.h"
#if defined(HAL_I2C_MASTER_MODULE_ENABLED)
#include "string.h"
#include "stdlib.h"
#include "hal_i2c_master_internal.h"
#include "hal_nvic.h"
#include "hal_pdma_internal.h"
#include "hal_log.h"
#include "hal_clock.h"
#include "hal_gpt.h"
#include <assert.h>


/***************************************************************************************
*
*                        Private Variable Declare
*
***************************************************************************************/

I2C_REGISTER_T *const i2c_master_register[(uint32_t)HAL_I2C_MASTER_MAX] = {(I2C_REGISTER_T *)(I2C_0_BASE), (I2C_REGISTER_T *)(I2C_1_BASE),
                                                                          (I2C_REGISTER_T *)(I2C_2_BASE), (I2C_REGISTER_T *)(I2C_AO_BASE)};
const pdma_channel_t g_i2c_tx_dma_channel[(uint32_t)HAL_I2C_MASTER_MAX] = {HAL_I2C_TX_DMA_CHANNEL};
const pdma_channel_t g_i2c_rx_dma_channel[(uint32_t)HAL_I2C_MASTER_MAX] = {HAL_I2C_RX_DMA_CHANNEL};
const uint32_t       g_i2c_source_clock_table[(uint32_t)HAL_I2C_MASTER_MAX] = {HAL_I2C_SOURCE_CLOCK_TABLE};
#ifdef HAL_I2C_MASTER_FRATURE_SW_FIFO
hal_i2c_sw_fifo_t i2c_sw_fifo[HAL_I2C_MASTER_MAX];
#endif

#ifdef HAL_CLOCK_MODULE_ENABLED
const hal_clock_cg_id g_i2c_pdn_table[(uint32_t)HAL_I2C_MASTER_MAX] = {HAL_I2C_PDN_TABLE};
#else
    #define     RG_XO_PDN_CLRD0         *((volatile uint32_t *)0xA2030B20)
    #define     RG_XO_PDN_SETD0         *((volatile uint32_t *)0xA2030B10)
    #define     XO_PDN_I2C0_OFFSET      (2)
    #define     XO_PDN_I2C1_OFFSET      (3)
    #define     XO_PDN_I2C2_OFFSET      (4)

    uint8_t     g_xo_pdn_i2c_cg_ofs[] = {XO_PDN_I2C0_OFFSET, XO_PDN_I2C1_OFFSET, XO_PDN_I2C2_OFFSET};
#endif



/***************************************************************************************
*
*                        Public Function Declare
*
***************************************************************************************/

void    i2c_clock_control(hal_i2c_port_t i2c_port, bool enable)
{
#ifdef HAL_CLOCK_MODULE_ENABLED
    if(enable) {
        hal_clock_enable(g_i2c_pdn_table[i2c_port]);
    } else {
        hal_clock_disable(g_i2c_pdn_table[i2c_port]);
    }
#else
    uint32_t mask = 0;
    if(i2c_port > HAL_I2C_MASTER_2) {
        return;
    }
    mask = 1<<g_xo_pdn_i2c_cg_ofs[i2c_port];
    if(enable) {
        RG_XO_PDN_CLRD0 = mask;
    } else {
        RG_XO_PDN_SETD0 = mask;
    }
#endif
}




/*
 * To record the call flow for debuging.
 *
 */
#ifdef I2C_ENABLE_DEBUG_LOG
uint32_t i2c_debug_flow_array[HAL_I2C_MASTER_MAX][I2C_DEBUG_FLOW_ARRAY_COUNT] = {0};
static uint32_t debug_loop[HAL_I2C_MASTER_MAX] = {0};
void i2c_debug_record_flow(hal_i2c_port_t i2c_port, uint32_t type)
{
    I2C_DEBUG_LOG(i2c_port, type);
    i2c_debug_flow_array[i2c_port][debug_loop[i2c_port]] = type;
    debug_loop[i2c_port] = (debug_loop[i2c_port] + 1) % I2C_DEBUG_FLOW_ARRAY_COUNT;
}
#endif /* I2C_ENABLE_DEBUG_LOG */

#ifdef HAL_I2C_MASTER_FRATURE_SW_FIFO
void i2c_sw_fifo_init(uint32_t i2c_port)
{
    i2c_sw_fifo[i2c_port].i2c_sw_fifo_len = HAL_I2C_SW_FIFO_LEN;

   i2c_sw_fifo[i2c_port].i2c_sw_fifo_start = (hal_i2c_sw_fifo_base *)malloc(HAL_I2C_SW_FIFO_LEN * sizeof(hal_i2c_sw_fifo_base));
    if (NULL == i2c_sw_fifo[i2c_port].i2c_sw_fifo_start) {
        log_hal_msgid_error("[I2C%d][init]:i2c_sw_fifo_init failed!\r\n", 1, i2c_port);
    }
    
    i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index = 0;
    i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index = 0;
}


uint32_t i2c_push_sw_fifo(uint32_t i2c_port, hal_i2c_sw_fifo_node_t *tansfer_config, uint32_t transfer_cnt)
{
    uint32_t temp_cnt0 = 0;
    uint32_t temp_cnt1 = 0;
    uint32_t temp_cnt2 = 0;
    uint32_t saved_mask;
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    hal_i2c_sw_fifo_node_t *i2c_sw_fifo_p =  i2c_sw_fifo[i2c_port].i2c_sw_fifo_start +  i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index;

    /* Check if fifo is full */
    if ((i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index + 1) % HAL_I2C_SW_FIFO_LEN == (i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index % HAL_I2C_SW_FIFO_LEN)) {
        //log_hal_msgid_warning("[I2C[%d]:]i2c sw fifo is full,please pop data\r\n", 2, i2c_port);
        hal_nvic_restore_interrupt_mask(saved_mask);
        return 0;
    }

    /* read index > write index */
    if (i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index > i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index) {
        /* Get min of frame cnt remained in fifo and the frame cnt of user wanted to push in fifo */
        temp_cnt0 = MIN(i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index - i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index - 1, transfer_cnt);

        /* push frame in fifo */
        memcpy(i2c_sw_fifo_p, tansfer_config, temp_cnt0 * sizeof(hal_i2c_sw_fifo_node_t));

        i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index += temp_cnt0;

    } else if (i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index == 0) {
        /* Get min of frame cnt remained in fifo and the frame cnt of user wanted to push in fifo */
        temp_cnt0 = MIN(i2c_sw_fifo[i2c_port].i2c_sw_fifo_len - i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index - 1, transfer_cnt);

        /* push frame in fifo */
        memcpy(i2c_sw_fifo_p, tansfer_config, temp_cnt0 * sizeof(hal_i2c_sw_fifo_node_t));

        i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index += temp_cnt0;
    } else {
        temp_cnt1 = MIN(i2c_sw_fifo[i2c_port].i2c_sw_fifo_len - i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index, transfer_cnt);
        memcpy(i2c_sw_fifo_p, tansfer_config, temp_cnt1 * sizeof(hal_i2c_sw_fifo_node_t));
        if (temp_cnt1 < transfer_cnt) {
            temp_cnt2 = MIN(i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index, transfer_cnt - temp_cnt1);
            memcpy(i2c_sw_fifo_p, &tansfer_config[temp_cnt2] , temp_cnt2 * sizeof(hal_i2c_sw_fifo_node_t));
            i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index += temp_cnt0;
            temp_cnt0 =  temp_cnt1 + temp_cnt2;
        } else {
            i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index = (i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index + temp_cnt1) % i2c_sw_fifo[i2c_port].i2c_sw_fifo_len ;
            temp_cnt0 =  temp_cnt1;
        }
    }

    hal_nvic_restore_interrupt_mask(saved_mask);
    return temp_cnt0;
}

uint32_t  i2c_pop_sw_fifo(uint32_t i2c_port, hal_i2c_sw_fifo_node_t *transfer_config, uint32_t transfer_cnt)
{
    uint32_t temp_cnt0 = 0;
    uint32_t temp_cnt1 = 0;
    uint32_t temp_cnt2 = 0;
    uint32_t saved_mask;
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    hal_i2c_sw_fifo_node_t *i2c_sw_fifo_p =   i2c_sw_fifo[i2c_port].i2c_sw_fifo_start +  i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index;

    /* fifo empty */
    if (i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index ==  i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index) {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return 0;
    } else if (i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index >  i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index) {
        temp_cnt0 = MIN(i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index - i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index, transfer_cnt);

        memcpy(transfer_config, i2c_sw_fifo_p, temp_cnt0 * sizeof(hal_i2c_sw_fifo_node_t));

        i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index += temp_cnt0;
    } else {
        temp_cnt1 = MIN(i2c_sw_fifo[i2c_port].i2c_sw_fifo_len - i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index, transfer_cnt);
        memcpy(transfer_config, i2c_sw_fifo_p, temp_cnt1 * sizeof(hal_i2c_sw_fifo_node_t));
        if (temp_cnt1 < transfer_cnt) {
            temp_cnt2 = MIN(i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index, transfer_cnt - temp_cnt1);
            memcpy(transfer_config, i2c_sw_fifo[i2c_port].i2c_sw_fifo_start , temp_cnt2 * sizeof(hal_i2c_sw_fifo_node_t));
            i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index += temp_cnt0;
            temp_cnt0 =  temp_cnt1 + temp_cnt2;
        } else {
            i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index = (i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index + temp_cnt1) %  i2c_sw_fifo[i2c_port].i2c_sw_fifo_len ;
            temp_cnt0 =  temp_cnt1;
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    return temp_cnt0;
}

int32_t i2c_get_sw_fifo_remain_transfer_cnt(uint32_t i2c_port)
{
    uint32_t saved_mask;
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    int32_t cnt =  i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index -  i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index - 1;
    if (cnt < 0) {
        cnt +=  i2c_sw_fifo[i2c_port].i2c_sw_fifo_len;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    return cnt;
}

int32_t i2c_get_sw_fifo_available_transfer_cnt(uint32_t i2c_port)
{
    uint32_t saved_mask;
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    int32_t cnt =  i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index -  i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index ;
    if (cnt < 0) {
        cnt +=  i2c_sw_fifo[i2c_port].i2c_sw_fifo_len ;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    return cnt;
}


void i2c_sw_fifo_deinit(uint32_t i2c_port)
{
    i2c_sw_fifo[i2c_port].i2c_sw_fifo_len = 0;

    if (0 != i2c_get_sw_fifo_available_transfer_cnt(i2c_port)) {
        log_hal_msgid_error("[I2C%d][deinit]:i2c_sw_fifo_deinit failed!\r\n", 1, i2c_port);
    } else {
        if (NULL != i2c_sw_fifo[i2c_port].i2c_sw_fifo_start) {
            free(i2c_sw_fifo[i2c_port].i2c_sw_fifo_start);
            i2c_sw_fifo[i2c_port].i2c_sw_fifo_r_index = 0;
            i2c_sw_fifo[i2c_port].i2c_sw_fifo_w_index = 0;
        } else {
            log_hal_msgid_error("[I2C%d][deinit]:i2c_sw_fifo_deinit failed!\r\n", 1, i2c_port);
        }
    }
}

#endif

/*
    Note: 1. I2C source clock fixed 26Mhz.
*/
void i2c_set_frequency(uint32_t i2c_port, uint32_t transfer_frequency)
{
    uint32_t    frequency = 0;
    uint32_t    step_cnt_div;
    uint32_t    sample_cnt_div;
    uint32_t    temp;

    /* set clock div to 0 */
    i2c_master_register[i2c_port]->CLOCK_DIV = 0;
    switch (transfer_frequency) {
        case (HAL_I2C_FREQUENCY_50K) :
            /* set clock div to 1 to acheive 50KHz frequency */
            i2c_master_register[i2c_port]->CLOCK_DIV = 1;
            frequency = I2C_FREQUENCY_100K;
            break;
        case (HAL_I2C_FREQUENCY_100K) :
            frequency = I2C_FREQUENCY_100K;
            break;
        case (HAL_I2C_FREQUENCY_200K) :
            frequency = I2C_FREQUENCY_200K;
            break;
        case (HAL_I2C_FREQUENCY_300K) :
            frequency = I2C_FREQUENCY_300K;
            break;
        case (HAL_I2C_FREQUENCY_400K) :
            frequency = I2C_FREQUENCY_400K;
            break;
        case (HAL_I2C_FREQUENCY_1M) :
            frequency = I2C_FREQUENCY_1M;
            break;
        default :
            /* this branch is special for pmu */
            /* pmu driver will use real i2c frequency as transfer_frquency */
            frequency = transfer_frequency;
            break;
    }

    /***********************************************************
    * Note: according to datasheet
    *  speed = source_clock/(clock_div+1)/(2*(step_cnt_div+1)*(sample_cnt_div+1))
    ************************************************************/
    /* Fast Mode Speed */
    for (sample_cnt_div = 1; sample_cnt_div <= 8; sample_cnt_div++) {

        temp = (frequency * 2 * sample_cnt_div);
        step_cnt_div = (g_i2c_source_clock_table[i2c_port] + temp - 1) / temp;  /* cast the <1 part */

        if (step_cnt_div <= 64) {
            break;
        }
    }

    sample_cnt_div--;
    step_cnt_div--;

    /* write timing register */
    i2c_master_register[i2c_port]->TIMING_UNION.TIMING_CELLS.SAMPLE_CNT_DIV = sample_cnt_div;
    i2c_master_register[i2c_port]->TIMING_UNION.TIMING_CELLS.STEP_CNT_DIV = step_cnt_div;
}

void i2c_set_io_config(uint32_t i2c_port, uint32_t io_config)
{
    i2c_master_register[i2c_port]->IO_CONFIG = io_config;
    if(io_config == HAL_I2C_IO_OPEN_DRAIN){
        i2c_master_register[i2c_port]->CONTROL2_UNION.CONTROL2_CELLS.CLK_EXT_EN = 1;
    }else{
        i2c_master_register[i2c_port]->CONTROL2_UNION.CONTROL2_CELLS.CLK_EXT_EN = 0;
    }
}

/***********************************************************
 * Note: currently, the DMA maximum length is 15Bytes.
 * To let most of the sensors fit to the transfer format, we chose to default
 * set transaction length to 1. If customer want to transaction data
 * more than 15 once, we need to add a new function or new featurn to
 * let the customer give the transfer and transaction length.
 ************************************************************/
void i2c_config_hardware(uint32_t i2c_port, i2c_transaction_type_t transaction_type, uint8_t slave_address, i2c_transaction_size_t *config_size)
{
    /* set slave address */
    i2c_master_register[i2c_port]->SLAVE_ADDR = slave_address;
    /* clear fifo */
    I2C_CLEAR_FIFO(i2c_port);
    /* clear interrupt status */
    I2C_CLEAR_INTERRUPT_STATUS(i2c_port);

    /* set control register */
    i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL = 0;
    //i2c_master_register[i2c_port]->CONTROL2_UNION.CONTROL2 = 0;
    switch (transaction_type) {
        case I2C_TRANSACTION_PUSH_PULL_SEND_POLLING:
        case I2C_TRANSACTION_OPEN_DRAIN_SEND_POLLING:
            i2c_master_register[i2c_port]->CONTROL2_UNION.CONTROL2_CELLS.ACKERR_DET_EN = 1;
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->send_size;
            i2c_master_register[i2c_port]->TRANSAC_LEN = 1;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = 1;
            break;
        case I2C_TRANSACTION_PUSH_PULL_SEND_DMA:
        case I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA:
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.DMA_EN = 1;
            i2c_master_register[i2c_port]->CONTROL2_UNION.CONTROL2_CELLS.ACKERR_DET_EN = 1;
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->send_size;
            i2c_master_register[i2c_port]->TRANSAC_LEN = 1;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = 1;
            break;
        case I2C_TRANSACTION_PUSH_PULL_RECEIVE_POLLING:
        case I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_POLLING:
            i2c_master_register[i2c_port]->CONTROL2_UNION.CONTROL2_CELLS.ACKERR_DET_EN = 1;
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->receive_size;
            i2c_master_register[i2c_port]->TRANSAC_LEN = 1;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = 1;
            break;
        case I2C_TRANSACTION_PUSH_PULL_RECEIVE_DMA:
        case I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA:
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.DMA_EN = 1;
            i2c_master_register[i2c_port]->CONTROL2_UNION.CONTROL2_CELLS.ACKERR_DET_EN = 1;
            i2c_master_register[i2c_port]->TRANSFER_LEN = 1;
            i2c_master_register[i2c_port]->TRANSAC_LEN = config_size->receive_size;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = 1;
            break;
        case I2C_TRANSACTION_PUSH_PULL_SEND_TO_RECEIVE_POLLING:
        case I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_POLLING:
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.RS_STOP = 1;
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.DIR_CHANGE = 1;
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.TRANSFER_LEN_CHANGE = 1;
            i2c_master_register[i2c_port]->CONTROL2_UNION.CONTROL2_CELLS.ACKERR_DET_EN = 1;
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->send_size;
            i2c_master_register[i2c_port]->TRANSAC_LEN =  2;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = config_size->receive_size;
            break;
        case I2C_TRANSACTION_PUSH_PULL_SEND_TO_RECEIVE_DMA:
        case I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA:
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.DMA_EN = 1;
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.RS_STOP = 1;
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.DIR_CHANGE = 1;
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.TRANSFER_LEN_CHANGE = 1;
            i2c_master_register[i2c_port]->CONTROL2_UNION.CONTROL2_CELLS.ACKERR_DET_EN = 1;
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->send_size;
            i2c_master_register[i2c_port]->TRANSAC_LEN = 1 + config_size->receive_size;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = 1;
            break;

        case I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA_EXTEND:
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.DMA_EN = 1;
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.RS_STOP = 1;
            i2c_master_register[i2c_port]->CONTROL2_UNION.CONTROL2_CELLS.ACKERR_DET_EN = 1;
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->transfer_length;
            i2c_master_register[i2c_port]->TRANSAC_LEN = config_size->transaction_length;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = 1;
            break;
        case I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA_EXTEND:
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.DMA_EN = 1;
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.RS_STOP = 1;
            i2c_master_register[i2c_port]->CONTROL2_UNION.CONTROL2_CELLS.ACKERR_DET_EN = 1;
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->transfer_length;
            i2c_master_register[i2c_port]->TRANSAC_LEN = config_size->transaction_length;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = 1;
            break;
        case I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND:
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.DMA_EN = 1;
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.RS_STOP = 1;
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.DIR_CHANGE = 1;
            i2c_master_register[i2c_port]->CONTROL_UNION.CONTROL_CELLS.TRANSFER_LEN_CHANGE = 1;
            i2c_master_register[i2c_port]->CONTROL2_UNION.CONTROL2_CELLS.ACKERR_DET_EN = 1;
            i2c_master_register[i2c_port]->TRANSFER_LEN = config_size->transfer_length;
            i2c_master_register[i2c_port]->TRANSAC_LEN = config_size->transaction_length;
            i2c_master_register[i2c_port]->TRANSFER_LEN_AUX = config_size->transfer_length_aux;
            break;
        default :
            break;

    }

}


void i2c_write_fifo(uint32_t i2c_port, const uint8_t *data, uint32_t size)
{
    uint32_t i;
    for (i = 0; i < size; i++) {
        i2c_master_register[i2c_port]->DATA_PORT = *(data + i);
    }
}


void i2c_read_fifo(uint32_t i2c_port, uint8_t *buffer, uint32_t size)
{
    uint32_t i;
    for (i = 0; i < size; i++) {
        *(buffer + i) = i2c_master_register[i2c_port]->DATA_PORT;
    }
}


i2c_transaction_status_t i2c_get_transaction_status(uint32_t i2c_port)
{
    uint32_t register_value;
    i2c_transaction_status_t return_status = I2C_TRANSACTION_UNKNOWN_ERROR;

    register_value = i2c_master_register[i2c_port]->INTR_STAT;

    if (0 != (register_value & (0x1 << I2C_ACKERR_STATUS_BIT))) {
        return_status = I2C_TRANSACTION_ACK_ERROR;
    } else if (0 != (register_value & (0x1 << I2C_HS_NACKERR_STATUS_BIT))) {
        return_status = I2C_TRANSACTION_NACK_ERROR;
    } else if (0x1 == register_value) {
        return_status = I2C_TRANSACTION_SUCCESS;
    }

#ifdef I2C_WAIT_TRANSACTION_COMPLETE
    uint32_t gpt_start_count, gpt_current_count, gpt_duration_count;
    if(return_status != I2C_TRANSACTION_SUCCESS){
        /*wait for transcation_complete flag is set*/
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
        while(!(i2c_master_register[i2c_port]->INTR_STAT & (0x1 << I2C_COMPLETE_STATUS_BIT)))
        {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_current_count);
            hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);
            if (I2C_INTERRUPT_TIMEOUT_VALUE < gpt_duration_count)
            {
                log_hal_error("[I2C%d][isr]:trans_comp timeout!\r\n", i2c_port);
                break;
            }
        }
    }
#endif

    I2C_CLEAR_INTERRUPT_STATUS(i2c_port);
    return return_status;
}

i2c_transaction_status_t    i2c_get_irq_status(uint32_t i2c_port)
{
    uint32_t                 register_value;
    i2c_transaction_status_t return_status;

    return_status = I2C_TRANSACTION_UNKNOWN_ERROR;
    register_value = i2c_master_register[i2c_port]->INTR_STAT;
    if (0 != (register_value & (0x1 << I2C_ACKERR_STATUS_BIT))) {
        return_status = I2C_TRANSACTION_ACK_ERROR;
    } else if (0 != (register_value & (0x1 << I2C_HS_NACKERR_STATUS_BIT))) {
        return_status = I2C_TRANSACTION_NACK_ERROR;
    } else if (0x1 == register_value) {
        I2C_CLEAR_INTERRUPT_STATUS(i2c_port);
        return_status = I2C_TRANSACTION_SUCCESS;
    }
    I2C_CLEAR_INTERRUPT_STATUS(i2c_port);
    return return_status;
}


pdma_status_t   i2c_dma_init(uint32_t i2c_port, bool is_tx, uint32_t size)
{
    pdma_config_t           i2c_dma_config;
    pdma_status_t           ret_dma;
    pdma_channel_t          pdma_ch;

    i2c_dma_config.burst_mode   = false;
    i2c_dma_config.count        = size;
    i2c_dma_config.size     = PDMA_BYTE;
    if(is_tx) {
        i2c_dma_config.master_type  = PDMA_TX;
        pdma_ch = g_i2c_tx_dma_channel[i2c_port];
    } else {
        i2c_dma_config.master_type  = PDMA_RX;
        pdma_ch = g_i2c_rx_dma_channel[i2c_port];
    }
    ret_dma = pdma_init(pdma_ch);
    if(PDMA_OK != ret_dma){
        return ret_dma;
    }
    ret_dma = pdma_configure(pdma_ch, &i2c_dma_config);
    return ret_dma;
}

pdma_status_t   i2c_dma_deinit(uint32_t i2c_port, bool is_tx)
{
    pdma_channel_t          rx_dma_ch,tx_dma_ch;

    rx_dma_ch = g_i2c_rx_dma_channel[i2c_port];
    tx_dma_ch = g_i2c_tx_dma_channel[i2c_port];

    if(is_tx) {
        pdma_stop(tx_dma_ch);
        pdma_deinit(tx_dma_ch);
    } else {
        pdma_stop(rx_dma_ch);
        pdma_deinit(rx_dma_ch);
    }
    return PDMA_OK;
}



#endif /* #if defined(HAL_I2C_MASTER_MODULE_ENABLED) */

