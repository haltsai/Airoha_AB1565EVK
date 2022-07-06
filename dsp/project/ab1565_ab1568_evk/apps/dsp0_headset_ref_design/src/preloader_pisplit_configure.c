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
 


#include <stdlib.h>
#include <stdio.h>
#include "elf.h"
#ifdef __XTENSA__
#include <xtensa/hal.h>  /* xthal_memcpy */
#endif
#include <string.h>
#include "assert.h"

#ifdef PRELOADER_ENABLE

#include"preloader_pisplit.h"
#include "preloader_pisplit_log.h"
#include"preloader_pisplit_configure.h"
#include "multi_pool_heap.h"
#include "xt_library_loader.h"
#include "hal_resource_assignment.h"

#include "hal.h"
#include "assert.h"
//#include "ccci.h"
#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t _preloader_pic_iram_start[];    //this is DSP0 IRAM free start, defined in memory layout link script
extern uint32_t _preloader_pic_iram_end[];      //this is DSP0 IRAM free end, defined in memory layout link script

extern uint32_t _preloader_pic_dram_start[];    //this is DSP0 DRAM free start, defined in memory layout link script
extern uint32_t _preloader_pic_dram_end[];      //this is DSP0 DRAM free end, defined in memory layout link script

/* just a sample
extern uint32_t _preloader_pic_sysram_start[];
extern uint32_t _preloader_pic_sysram_end[];

extern uint32_t _preloader_pic_psram_start[];
extern uint32_t _preloader_pic_psram_end[];
*/

/*This structure for static PIC memory pools.
If you want to assign some static memory as PIC memory pools for PIC library.
User can register your pools information to here!!!
If user have no any change, please clone this structure to your project.
User must be pay attention to the parameter of preloader_pisplit_memory_type_t, this means the type of memory pool.
When you load your PIC load, should choose which memory type.
*/
preloader_pisplit_pool_info_t preloader_pisplit_customer_static_pools[] =
{
  /*{//SYSRAM   just a sample
        (void *)_preloader_pic_sysram_start,
        (uint32_t)_preloader_pic_sysram_end,
        PRELOADER_I_LOW_PERFORMANCE|PRELOADER_D_LOW_PERFORMANCE
    },
    {//PSRAM
        (void *)_preloader_pic_psram_start,
        (uint32_t)_preloader_pic_psram_end,
        PRELOADER_I_LOW_PERFORMANCE|PRELOADER_D_LOW_PERFORMANCE
    },*/
    {//IRAM
        (void *)_preloader_pic_iram_start,
        (uint32_t)_preloader_pic_iram_end,
        PRELOADER_I_HIGH_PERFORMANCE
    },
    {//DRAM
        (void *)_preloader_pic_dram_start,
        (uint32_t)_preloader_pic_dram_end,
        PRELOADER_D_HIGH_PERFORMANCE
    },
};

#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
/*This structure for dynamic PIC memory pools.
If you want to assign some dynamic memory as PIC memory pools for PIC library.
User can register your pools information to here!!!
If user have no any change, please clone this structure to your project.
On this example, Airoha reference project just use the dynamic pools for the feature of DSP1 PIC solution.
    On the feature of DSP0 will help DSP1 to load its PIC library. So the PIC memory pools of DSP1 I/DRAM will controlled by DSP1.
    Because of DSP1 be aware of the DSP1 I/DRAM unlti DSP1 booting up, so DSP0 should request the information from DSP1 via CCCI,
    so the memory pools of DSP1 will be dynamic register for DSP1.

    User must be pay attention to the parameter of preloader_pisplit_memory_type_t, this means the type of memory pool.
    When you load your PIC load, should choose which memory type.
    As the feature of DSP0 help DSP1 to load, will use the type of PRELOADER_EXT_IRAM and PRELOADER_EXT_DRAM.
 When DSP0 want to load DSP1 library or want to allocate extra memory from DSP1 I/DRAM, need the parameters of PRELOADER_EXT_IRAM and PRELOADER_EXT_DRAM.
*/
//User must pay attention to this key point of the structure of preloader_pisplit_customer_dynamic_pools.
//Because of it defined and read on DSP0 side, and write on DSP1 side. To void will be have data coherency issue, 
//user should guarantee the structure must be non-cacheable for DSP0 and DSP1.
//On this example project, RW data default put on DSP0 DRAM, should it is non-cacheable for DSP0.
//Because DSP1 have no cache, so DSP0 DRAM also non-cacheable for DSP1.

preloader_pisplit_pool_info_t preloader_pisplit_customer_dynamic_pools[] =
{
    {//dsp1 iram
        NULL,
        0,
        PRELOADER_EXT_IRAM
    },
    {//dsp1 dram
        NULL,
        0,
        PRELOADER_EXT_DRAM
    },
};
#endif

/*This function implement on by project level, for register the static PIC memory pools to Preloader_pisplit driver.
User must need this step, after this step, user can do PIC library load or use preloader_pisplit_malloc_memory()  to
allocate extra memory from PIC memory heap.
Please clone this function to your project!!!
And make sure this API called on your init flow after preloader_pisplit_init()

example code on init flow:
    preloader_pisplit_init();
    pisplit_configure_static_pool();
*/
void pisplit_configure_static_pool()
{
    uint32_t count;
    count = sizeof(preloader_pisplit_customer_static_pools)/sizeof(preloader_pisplit_pool_info_t);
    preloader_pisplit_add_pools(preloader_pisplit_customer_static_pools,count);

}

#ifdef PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1
/*This function implement on by project level, for register the dynamic PIC memory pools to Preloader_pisplit driver.
For the feature of DSP0 load DSP1 PIC library, also need power on and let DSP1 running firstly, then can call pisplit_configure_dynamic_pool() to
register DSP1 I/DRAM to PIC pools, because need power on the memory.
Please clone this function to your project!!!

example code on init flow:
    preloader_pisplit_init();
    pisplit_configure_static_pool();
    PRELOADER_LOG_I(preloader,"\r\n#### DSP0 Begin to Reset DSP1... ####\r\n",0);
    hal_dsp_core_reset(HAL_CORE_DSP1, DSP1_BASE);
    pisplit_configure_dynamic_pool();
*/
void pisplit_configure_dynamic_pool()
{
    ccci_msg_t ccci_msg;
    uint32_t pools;
    ccci_msg.event = CCCI_EVENT_DSP0_TO_DSP1_PIC_MEMORY_REQUEST;
    ccci_msg.data = hal_memview_dsp0_to_infrasys((uint32_t)preloader_pisplit_customer_dynamic_pools);
    //send CCCI to DSP1 to request DSP1 I/DRAM free memory,must be wait for got this information
    PRELOADER_LOG_I(preloader,"DSP0 to reuqest DSP1 free I/DRAM info...",0);
    if(CCCI_STATUS_OK != ccci_send_msg(HAL_CORE_DSP1,ccci_msg,CCCI_SEND_MSG_WAIT_FOR_SEND_RECEIVE_DONE))
    {
        assert(0);
    }
    if( (preloader_pisplit_customer_dynamic_pools[0].pool_start == NULL) || (preloader_pisplit_customer_dynamic_pools[1].pool_start == NULL) )
    {
        PRELOADER_LOG_I(preloader,"pisplit_configure_dynamic_pool get DSP1 fre I/DRAM fail!!!",0);
        assert(0);
    }
    pools = sizeof(preloader_pisplit_customer_dynamic_pools)/sizeof(preloader_pisplit_pool_info_t);
    preloader_pisplit_add_pools(preloader_pisplit_customer_dynamic_pools,pools);
}
#endif

#ifdef __cplusplus
}
#endif

#endif

