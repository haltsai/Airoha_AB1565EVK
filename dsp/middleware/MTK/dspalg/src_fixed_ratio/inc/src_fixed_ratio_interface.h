/* Copyright Statement:
*
* (C) 2020 Airoha Technology Corp. All rights reserved.
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

#ifndef _SRC_FIXED_RATIO_INTERFACE_H_
#define _SRC_FIXED_RATIO_INTERFACE_H_
/* Includes ------------------------------------------------------------------*/
#include "dsp_utilities.h"
//#include "dsp_buffer.h"
#include "dsp_feature_interface.h"
#include "dsp_callback.h"
#include "dsp_sdk.h"

/* Public define -------------------------------------------------------------*/
#define DSP_SMP_SIZE    (0)
#define SMP_MAX_CH_NUM  (8)
/* Public typedef ------------------------------------------------------------*/
typedef enum
{
    SRC_FIXED_RATIO_PORT_STAT_DEINIT,
    SRC_FIXED_RATIO_PORT_STAT_INIT,
    SRC_FIXED_RATIO_PORT_STAT_RUNNING,
} src_fixed_ratio_port_status_e;

typedef enum
{
    SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_SINGLE,
    SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_CONSECUTIVE,
    SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_ALTERNATE,
    SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_MAX,
    SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_DUMMY = 0xFFFF,
} src_fixed_ratio_port_multi_cvt_mode_e;

typedef enum
{
    SRC_FIXED_RATIO_CH_NUM_UPDATE_LATER = 0,
    SRC_FIXED_RATIO_CH_NUM_EXCLUDE_REF = 0xFFFF,
} src_fixed_ratio_channel_num_e;

typedef struct
{
    src_fixed_ratio_port_status_e status;
    void* owner;
    uint32_t channel_number;
    uint32_t in_sampling_rate;
    uint32_t out_sampling_rate;/*only support 2x or 3x up/dn sampling*/
    stream_resolution_t resolution;
    int32_t is_upsample;
    int32_t factor;
    void* smp_instance_ptr[SMP_MAX_CH_NUM];
    uint16_t cvt_num;
    src_fixed_ratio_port_multi_cvt_mode_e multi_cvt_mode;
    /*===============================No need to cofing by user===================================*/
    uint32_t tmp_buff_size;
    uint8_t* tmp_buff;
} src_fixed_ratio_port_t;

typedef struct
{
    uint32_t channel_number;
    uint32_t in_sampling_rate;
    uint32_t out_sampling_rate;/*only support 2x or 3x up/dn sampling*/
    stream_resolution_t resolution;
    uint16_t cvt_num;
    src_fixed_ratio_port_multi_cvt_mode_e multi_cvt_mode;
} src_fixed_ratio_config_t;

typedef struct
{
	S16 LP2_COFEB16_00; /**< @Value 0x22D5 @Desc 1 */
	S16 LP2_COEFB16_01; /**< @Value 0x2923 @Desc 1 */
	S16 LP2_COFEB16_02; /**< @Value 0x34D6 @Desc 1 */
	S16 LP2_COEFB16_03; /**< @Value 0xDE52 @Desc 1 */	 
	S16 LP2_COFEB16_04; /**< @Value 0x2923 @Desc 1 */
	S16 LP2_COEFB16_05; /**< @Value 0x2923 @Desc 1 */	 
	S16 LP2_COFEB16_06; /**< @Value 0x0D09 @Desc 1 */
	S16 LP2_COEFB16_07; /**< @Value 0x2923 @Desc 1 */		 
	S16 LP2_COFEB16_08; /**< @Value 0x2200 @Desc 1 */
	S16 LP2_COEFB16_09; /**< @Value 0xB25B @Desc 1 */		 
	S16 LP2_COFEB16_10; /**< @Value 0x03FB @Desc 1 */
	S16 LP2_COEFB16_11; /**< @Value 0x2923 @Desc 1 */		 
	S16 LP2_COFEB16_12; /**< @Value 0x1657 @Desc 1 */
	S16 LP2_COEFB16_13; /**< @Value 0x8E27 @Desc 1 */
	S16 LP2_COFEB16_14; /**< @Value 0x2923 @Desc 1 */
	S16 LP2_Reserved  ;  

	S16 LP3_COFEB16_00; /**< @Value 0x12CC @Desc 1 */
	S16 LP3_COEFB16_01; /**< @Value 0x1D43 @Desc 1 */
	S16 LP3_COFEB16_02; /**< @Value 0x4B0B @Desc 1 */
	S16 LP3_COEFB16_03; /**< @Value 0xCDAF @Desc 1 */	 
	S16 LP3_COFEB16_04; /**< @Value 0x1D43 @Desc 1 */
	S16 LP3_COEFB16_05; /**< @Value 0x1D43 @Desc 1 */	 
	S16 LP3_COFEB16_06; /**< @Value 0xFB40 @Desc 1 */
	S16 LP3_COEFB16_07; /**< @Value 0x1D43 @Desc 1 */		 
	S16 LP3_COFEB16_08; /**< @Value 0x4934 @Desc 1 */
	S16 LP3_COEFB16_09; /**< @Value 0xACFC @Desc 1 */		 
	S16 LP3_COFEB16_10; /**< @Value 0xF4CE @Desc 1 */
	S16 LP3_COEFB16_11; /**< @Value 0x1D43 @Desc 1 */		 
	S16 LP3_COFEB16_12; /**< @Value 0x4A0F @Desc 1 */
	S16 LP3_COEFB16_13; /**< @Value 0x8D79 @Desc 1 */
	S16 LP3_COFEB16_14; /**< @Value 0x1D43 @Desc 1 */
	S16 LP3_Reserved  ;  
} PACKED SMP_BYN_NVKEY;

typedef struct {		
	ALIGN(8) S32 Xstate32[2];   //For IIR filte  X[n-1]!BX[n-2] state temporary
	ALIGN(8) S32 Ystate32[6];	//For IIR filter Y1[n-1]!BY1[n-2]!BY2[n-1]!BY2[n-2]!BY3[n-1]!BY3[n-2] state temporary
} SMP_STATE;

//VO:
typedef struct SMP_INSTANCE_s  
{
  ALIGN(2)   SMP_BYN_NVKEY NvKey;	// Coef 
  DSP_ALIGN8 SMP_STATE State;		// Tmp
} SMP_INSTANCE_t;

/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
extern int get_sampler_by_n_version(void);
extern void updn_sampling_Init(SMP_INSTANCE_t *smp_all, void *nvkey);
extern S16 updn_sampling_16bit_Proc(SMP_INSTANCE_t *smp_all, int is_upsample, int factor, S16 *InBuf, S16 *OuBuf, S16 sample_cnt);
extern S16 updn_sampling_32bit_Proc(SMP_INSTANCE_t *smp_all, int is_upsample, int factor, S32 *InBuf, S32 *OuBuf, S16 sample_cnt);

src_fixed_ratio_port_t* stream_function_src_fixed_ratio_get_port(void* owner);
src_fixed_ratio_port_t* stream_function_src_fixed_ratio_get_2nd_port(void* owner);
src_fixed_ratio_port_t* stream_function_src_fixed_ratio_get_3rd_port(void* owner);
void stream_function_src_fixed_ratio_init(src_fixed_ratio_port_t* port, src_fixed_ratio_config_t* config);
void stream_function_src_fixed_ratio_deinit(src_fixed_ratio_port_t* port);

/**
This section introduces up/dn sampler API:
    1. Before stream is created, after source/sink are opened:
        step 1: Call #stream_function_src_fixed_ratio_get_port() to get a sampler port
        step 2: Call #stream_function_src_fixed_ratio_init() to config sampler port

    2. Before source/sink are closed :
        step 1: Call #stream_function_src_fixed_ratio_deinit() to free sampler port

    3. sample code:
        #include "src_fixed_ratio_interface.h"

        // open stream
        record_smp_port = stream_function_src_fixed_ratio_get_port(record_if.source);

        src_fixed_ratio_config_t smp_config;
        smp_config.channel_number = 2;
        smp_config.in_sampling_rate = 48000;
        smp_config.out_sampling_rate = 16000;
        smp_config.resolution = RESOLUTION_16BIT;

        stream_function_src_fixed_ratio_init(record_smp_port, &smp_config);
        
       //close stream
       stream_function_src_fixed_ratio_deinit(record_smp_port);
       
**/
extern bool stream_function_src_fixed_ratio_initialize(void *para);
extern bool stream_function_src_fixed_ratio_process(void *para);


#endif /* _SRC_FIXED_RATIO_INTERFACE_H_ */

