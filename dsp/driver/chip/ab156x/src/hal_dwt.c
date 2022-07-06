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

#include "hal_dwt.h"
#include "hal_nvic.h"
#include <string.h>
#ifdef HAL_DWT_MODULE_ENABLED
#include <xtensa/tie/xt_debug.h>

typedef struct{
    uint32_t base_address[HAL_DWT_MAX];
    uint8_t  addr_mask[HAL_DWT_MAX];
    hal_dwt_function_t  func[HAL_DWT_MAX];
    bool address_flag[HAL_DWT_MAX];   
} dwt_save_last_data_t ;
uint32_t hal_dwt_resource_count;

static dwt_save_last_data_t dwt_save_last_data; 
void hal_dwt_reset(void)
{
    memset(&dwt_save_last_data,0,sizeof(dwt_save_last_data));
    _TIE_xt_debug_WSR_DBREAKA0(0x0);
    _TIE_xt_debug_WSR_DBREAKC0(0x0);

    _TIE_xt_debug_WSR_DBREAKA1(0x0);
    _TIE_xt_debug_WSR_DBREAKC1(0x0);
}

void hal_dwt_init(void)
{   
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if(hal_dwt_resource_count==0)
    { 
    hal_dwt_resource_count++;
    memset(&dwt_save_last_data,0,sizeof(dwt_save_last_data));
    _TIE_xt_debug_WSR_DBREAKA0(0x0);
    _TIE_xt_debug_WSR_DBREAKC0(0x0);

    _TIE_xt_debug_WSR_DBREAKA1(0x0);
    _TIE_xt_debug_WSR_DBREAKC1(0x0);
    }
    hal_nvic_restore_interrupt_mask(mask);
}

int32_t hal_dwt_request_watchpoint(hal_dwt_channel_t index, uint32_t addr_base, uint32_t addr_mask, hal_dwt_function_t func)
{
    uint32_t mask;
    if( (index >= HAL_DWT_MAX) ||
        (func > WDE_DATA_RW)   ||
        (addr_mask > 0x6) )
    {
        return -1;
    }

    if( (addr_mask != 0) &&
        ((addr_base & (~(0xffffffff << addr_mask))) != 0) )
    {
        return -1;
    }
    hal_nvic_save_and_set_interrupt_mask(&mask);
    dwt_save_last_data.addr_mask[index] = addr_mask;
    dwt_save_last_data.base_address[index]=addr_base;
    dwt_save_last_data.func[index] = func;
    dwt_save_last_data.address_flag[index] = true;
    hal_nvic_restore_interrupt_mask(mask);
    if(index == HAL_DWT_0)
    {
        _TIE_xt_debug_WSR_DBREAKA0(addr_base);
        _TIE_xt_debug_WSR_DBREAKC0(((func << 30) + 0x3f) & (0xffffffff << addr_mask));
    }
    else if(index == HAL_DWT_1)
    {
        _TIE_xt_debug_WSR_DBREAKA1(addr_base);
        _TIE_xt_debug_WSR_DBREAKC1(((func << 30) + 0x3f) & (0xffffffff << addr_mask));
    }

    return 0;
}

int32_t hal_dwt_control_watchpoint(hal_dwt_channel_t index, hal_dwt_control_type_t is_enable)
{
    if((index > HAL_DWT_MAX )||(dwt_save_last_data.address_flag[index]!=true))
    {
        return -1 ;
    }
    if(is_enable == HAL_DWT_ENABLE){
       if(index == HAL_DWT_0){
        _TIE_xt_debug_WSR_DBREAKA0(dwt_save_last_data.base_address[HAL_DWT_0]);
        _TIE_xt_debug_WSR_DBREAKC0((((dwt_save_last_data.func[HAL_DWT_0]) << 30) + 0x3f) & (0xffffffff << (dwt_save_last_data.addr_mask[HAL_DWT_0])));
       }else{
        _TIE_xt_debug_WSR_DBREAKA1(dwt_save_last_data.base_address[HAL_DWT_1]);
        _TIE_xt_debug_WSR_DBREAKC1((((dwt_save_last_data.func[HAL_DWT_1]) << 30) + 0x3f) & (0xffffffff << (dwt_save_last_data.addr_mask[HAL_DWT_1])));
       }
    }else{
      if(index == HAL_DWT_0){
        _TIE_xt_debug_WSR_DBREAKA0(0x0);
        _TIE_xt_debug_WSR_DBREAKC0(0x0); 
      }else{
        _TIE_xt_debug_WSR_DBREAKA1(0x0);
        _TIE_xt_debug_WSR_DBREAKC1(0x0);
      }
    }
    return 0;
}
#endif /* HAL_DWT_MODULE_ENABLED */
