/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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

#ifdef MTK_AUDIO_BT_COMMON_ENABLE

/* Includes ------------------------------------------------------------------*/
#include "types.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "dsp_buffer.h"
#include "dsp_memory.h"
#include "dsp_callback.h"
#include "dsp_temp.h"
#include "dsp_dump.h"
#include "dsp_scenario.h"
#include "stream_bt_common.h"
#include "hal_gpt.h"
#include "hal_resource_assignment.h"
#include "bt_types.h"
#include "memory_attribute.h"
#ifdef AIR_GAMING_MODE_DONGLE_ENABLE
#include "scenario_ull_audio.h"
#endif /* AIR_GAMING_MODE_DONGLE_ENABLE */
#ifdef AIR_BLE_AUDIO_DONGLE_ENABLE
#include "scenario_ble_audio.h"
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

/* Private define ------------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Public macro --------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ATTR_ZIDATA_IN_DRAM static f_bt_common_ccni_callback_t bt_common_ccni_callback;

/* Public variables ----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/******************************************************************************/
/*                Share Information Operations Private Functions              */
/******************************************************************************/
EXTERN VOID StreamDSP_HWSemaphoreTake(VOID);
EXTERN VOID StreamDSP_HWSemaphoreGive(VOID);

/**
 * @brief This function is used to reset the read offset and write offest of the share buffer.
 *
 * @param source is the instance whose the share buffer is reset. It can be NULL.
 * @param sink is the instance whose the share buffer is reset. It can be NULL.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 void bt_common_share_information_reset_read_write_offset(SOURCE source, SINK sink)
{
    StreamDSP_HWSemaphoreTake();

    if(source != NULL)
    {
        switch(source->param.bt_dl.share_info_type)
        {
            case BUFFER_INFO_TYPE:
                configASSERT(0);
                break;

            case SHARE_BUFFER_INFO_TYPE:
                ((SHARE_BUFFER_INFO_PTR)(source->param.bt_dl.share_info_base_addr))->WriteOffset = 0;
                ((SHARE_BUFFER_INFO_PTR)(source->param.bt_dl.share_info_base_addr))->ReadOffset = 0;
                ((SHARE_BUFFER_INFO_PTR)(source->param.bt_dl.share_info_base_addr))->bBufferIsFull = false;
                break;

            case AVM_SHARE_BUF_INFO_TYPE:
                ((AVM_SHARE_BUF_INFO_PTR)(source->param.bt_dl.share_info_base_addr))->WriteIndex = 0;
                ((AVM_SHARE_BUF_INFO_PTR)(source->param.bt_dl.share_info_base_addr))->ReadIndex = 0;
                break;

            default:
                configASSERT(0);
                break;
        }
    }

    if(sink != NULL)
    {
        switch(sink->param.bt_ul.share_info_type)
        {
            case BUFFER_INFO_TYPE:
                configASSERT(0);
                break;

            case SHARE_BUFFER_INFO_TYPE:
                ((SHARE_BUFFER_INFO_PTR)(sink->param.bt_ul.share_info_base_addr))->WriteOffset = 0;
                ((SHARE_BUFFER_INFO_PTR)(sink->param.bt_ul.share_info_base_addr))->ReadOffset = 0;
                ((SHARE_BUFFER_INFO_PTR)(sink->param.bt_ul.share_info_base_addr))->bBufferIsFull = false;
                break;

            case AVM_SHARE_BUF_INFO_TYPE:
                ((AVM_SHARE_BUF_INFO_PTR)(sink->param.bt_ul.share_info_base_addr))->WriteIndex = 0;
                ((AVM_SHARE_BUF_INFO_PTR)(sink->param.bt_ul.share_info_base_addr))->ReadIndex = 0;
                break;

            default:
                configASSERT(0);
                break;
        }
    }

    StreamDSP_HWSemaphoreGive();
}

/**
 * @brief This function is used to fetch the share buffer information into source or sink.
 *
 * @param source is the instance whose the share buffer info is fetched. It can be NULL.
 * @param sink is the instance whose the share buffer info is fetched. It can be NULL.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 void bt_common_share_information_fetch(SOURCE source, SINK sink)
{
    uint32_t i;
    StreamDSP_HWSemaphoreTake();

    if(source != NULL)
    {
        switch(source->param.bt_dl.share_info_type)
        {
            case BUFFER_INFO_TYPE:
                configASSERT(0);
                break;

            case SHARE_BUFFER_INFO_TYPE:
                // memcpy(&(source->streamBuffer.ShareBufferInfo), source->param.bt_dl.share_info_base_addr, sizeof(SHARE_BUFFER_INFO));
                for (i = 0; i < sizeof(SHARE_BUFFER_INFO)/4; i++)
                {
                    /* do not warry memory corruption here, becasue sizeof(SHARE_BUFFER_INFO) must be 4*n */
                    *((uint32_t *)(&(source->streamBuffer.ShareBufferInfo)) + i) = *((uint32_t *)(source->param.bt_dl.share_info_base_addr) + i);
                }
                source->streamBuffer.ShareBufferInfo.startaddr = hal_memview_cm4_to_dsp0(source->streamBuffer.ShareBufferInfo.startaddr);
                break;

            case AVM_SHARE_BUF_INFO_TYPE:
                // memcpy(&(source->streamBuffer.AVMBufferInfo), source->param.bt_dl.share_info_base_addr, sizeof(AVM_SHARE_BUF_INFO));
                for (i = 0; i < sizeof(AVM_SHARE_BUF_INFO)/4; i++)
                {
                    /* do not warry memory corruption here, becasue sizeof(AVM_SHARE_BUF_INFO) must be 4*n */
                    *((uint32_t *)(&(source->streamBuffer.AVMBufferInfo)) + i) = *((uint32_t *)(source->param.bt_dl.share_info_base_addr) + i);
                }
                source->streamBuffer.AVMBufferInfo.StartAddr = hal_memview_cm4_to_dsp0(source->streamBuffer.AVMBufferInfo.StartAddr);
                break;

            default:
                configASSERT(0);
                break;
        }
    }

    if(sink != NULL)
    {
        switch(sink->param.bt_ul.share_info_type)
        {
            case BUFFER_INFO_TYPE:
                configASSERT(0);
                break;

            case SHARE_BUFFER_INFO_TYPE:
                // memcpy(&(sink->streamBuffer.ShareBufferInfo), sink->param.bt_ul.share_info_base_addr, sizeof(SHARE_BUFFER_INFO));
                for (i = 0; i < sizeof(SHARE_BUFFER_INFO)/4; i++)
                {
                    /* do not warry memory corruption here, becasue sizeof(SHARE_BUFFER_INFO) must be 4*n */
                    *((uint32_t *)(&(sink->streamBuffer.ShareBufferInfo)) + i) = *((uint32_t *)(sink->param.bt_ul.share_info_base_addr) + i);
                }
                sink->streamBuffer.ShareBufferInfo.startaddr = hal_memview_cm4_to_dsp0(sink->streamBuffer.ShareBufferInfo.startaddr);
                break;

            case AVM_SHARE_BUF_INFO_TYPE:
                // memcpy(&(sink->streamBuffer.AVMBufferInfo), sink->param.bt_ul.share_info_base_addr, sizeof(AVM_SHARE_BUF_INFO));
                for (i = 0; i < sizeof(AVM_SHARE_BUF_INFO)/4; i++)
                {
                    /* do not warry memory corruption here, becasue sizeof(AVM_SHARE_BUF_INFO) must be 4*n */
                    *((uint32_t *)(&(sink->streamBuffer.AVMBufferInfo)) + i) = *((uint32_t *)(sink->param.bt_ul.share_info_base_addr) + i);
                }
                sink->streamBuffer.AVMBufferInfo.StartAddr = hal_memview_cm4_to_dsp0(sink->streamBuffer.AVMBufferInfo.StartAddr);
                break;

            default:
                configASSERT(0);
                break;
        }
    }

    StreamDSP_HWSemaphoreGive();
}

/**
 * @brief This function is used to update the write offset of the sink's share buffer.
 *
 * @param sink is the instance whose write offset is updated.
 * @param WriteOffset is the new write offset value.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 void bt_common_share_information_update_write_offset(SINK sink, U32 WriteOffset)
{
    StreamDSP_HWSemaphoreTake();

    switch(sink->param.bt_ul.share_info_type)
    {
        case BUFFER_INFO_TYPE:
            configASSERT(0);
            break;

        case SHARE_BUFFER_INFO_TYPE:
            ((SHARE_BUFFER_INFO_PTR)(sink->param.bt_ul.share_info_base_addr))->WriteOffset = WriteOffset;
            if (WriteOffset == ((SHARE_BUFFER_INFO_PTR)(sink->param.bt_ul.share_info_base_addr))->ReadOffset)
            {
                ((SHARE_BUFFER_INFO_PTR)(sink->param.bt_ul.share_info_base_addr))->bBufferIsFull = true;
            }
            break;

        case AVM_SHARE_BUF_INFO_TYPE:
            ((AVM_SHARE_BUF_INFO_PTR)(sink->param.bt_ul.share_info_base_addr))->WriteIndex = (U16)WriteOffset;
            break;

        default:
            configASSERT(0);
            break;
    }

    StreamDSP_HWSemaphoreGive();
}

/**
 * @brief This function is used to update the read offset of the source's share buffer.
 *
 * @param source is the instance whose write offset is updated.
 * @param ReadOffset is the new read offset value.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 void bt_common_share_information_update_read_offset(SOURCE source, U32 ReadOffset)
{
    StreamDSP_HWSemaphoreTake();

    switch(source->param.bt_dl.share_info_type)
    {
        case BUFFER_INFO_TYPE:
            configASSERT(0);
            break;

        case SHARE_BUFFER_INFO_TYPE:
            ((SHARE_BUFFER_INFO_PTR)(source->param.bt_dl.share_info_base_addr))->ReadOffset = ReadOffset;
            if (((SHARE_BUFFER_INFO_PTR)(source->param.bt_dl.share_info_base_addr))->bBufferIsFull == true)
            {
                ((SHARE_BUFFER_INFO_PTR)(source->param.bt_dl.share_info_base_addr))->bBufferIsFull = false;
            }
            break;

        case AVM_SHARE_BUF_INFO_TYPE:
            ((AVM_SHARE_BUF_INFO_PTR)(source->param.bt_dl.share_info_base_addr))->ReadIndex = ReadOffset;
            break;

        default:
            configASSERT(0);
            break;
    }

    StreamDSP_HWSemaphoreGive();
}

/******************************************************************************/
/*                   BT_COMMON Source Private Functions                       */
/******************************************************************************/
/**
 * @brief This function is used to config bt common source.
 *
 * @param source is the instance who is wanted be configured.
 * @param type is the configure type.
 * @param value is the configure paramters.
 * @return true means the configuration is done successfully.
 * @return false means the configuration is done unsuccessfully.
 */
bool port_bt_common_source_config(SOURCE source, stream_config_type type, U32 value)
{
    bool ret = false;

    UNUSED(type);
    UNUSED(value);

    switch(source->param.bt_dl.scenario_type)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            /* Dongle side, voice path, bt source in case */
            if(source->param.bt_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
            {
                ret = gaming_mode_dongle_ul_config(source, type, value);
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            /* Dongle side, voice path, bt source in case */
            if(source->param.bt_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
            {
                ret = ble_audio_dongle_ul_config(source, type, value);
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            configASSERT(0);
            break;
    }

    return ret;
}

/**
 * @brief This function is used to get the available space size of the source based on the speical scenario type.
 *
 * @param source is the instance who is checked its available space size.
 * @param avail_size is the actual available size of the source.
 * @return true means this scenario use special method to get the the available size.
 * @return false means this scenario use common method to get the the available size.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 static bool port_bt_common_source_get_avail_size(SOURCE source, uint32_t *avail_size)
{
    bool ret = false;

    *avail_size = 0;

    switch(source->param.bt_dl.scenario_type)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            /* Dongle side, voice path, bt source in case */
            if(source->param.bt_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
            {
                ret = gaming_mode_dongle_ul_source_get_avail_size(source, avail_size);
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            /* Dongle side, voice path, bt source in case */
            if(source->param.bt_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
            {
                ret = ble_audio_dongle_ul_source_get_avail_size(source, avail_size);
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            configASSERT(0);
            break;
    }

    return ret;
}

/**
 * @brief This function is used to copy the data from the share buffer of the source based on the speical scenario type.
 *
 * @param source is the instance who owns the share buffer.
 * @param dst_buf is the destination address.
 * @param length is the wanted copy size.
 * @return uint32_t is the actual copy size.
 */
static uint32_t port_bt_common_source_copy_payload(SOURCE source, uint8_t *dst_buf, uint32_t length)
{
    UNUSED(dst_buf);

    switch(source->param.bt_dl.scenario_type)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            /* Dongle side, voice path, bt source in case */
            if(source->param.bt_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
            {
                length = gaming_mode_dongle_ul_source_copy_payload(source, dst_buf, length);
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            /* Dongle side, voice path, bt source in case */
            if(source->param.bt_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
            {
                length = ble_audio_dongle_ul_source_copy_payload(source, dst_buf, length);
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            configASSERT(0);
            break;
    }

    return length;
}

/**
 * @brief This function is used to get the new read offset of the source's share buffer based on the speical scenario type.
 *
 * @param source is the instance who owns the share buffer.
 * @param amount is the least offset size of the read offset.
 * @return true means this scenario needs to update read offset.
 * @return false means this scenario does not need to update read offset.
 */
static bool port_bt_common_source_get_new_read_offset(SOURCE source, U32 amount, U32 *ReadOffset)
{
    bool ret = false;
    UNUSED(amount);
    UNUSED(ReadOffset);

    switch(source->param.bt_dl.scenario_type)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            /* Dongle side, voice path, bt source in case */
            if(source->param.bt_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
            {
                ret = gaming_mode_dongle_ul_source_get_new_read_offset(source, amount, ReadOffset);
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            /* Dongle side, voice path, bt source in case */
            if(source->param.bt_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
            {
                ret = ble_audio_dongle_ul_source_get_new_read_offset(source, amount, ReadOffset);
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            configASSERT(0);
    }

    return ret;
}

/**
 * @brief This function is used to do special thing based on the speical scenario type after the drop is done.
 *
 * @param source is the instance who drop the data.
 * @param amount is the total length has been dropped.
 */
static void port_bt_common_source_drop_postprocess(SOURCE source, uint32_t amount)
{
    UNUSED(amount);

    switch(source->param.bt_dl.scenario_type)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            /* Dongle side, voice path, bt source in case */
            if(source->param.bt_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
            {
                gaming_mode_dongle_ul_source_drop_postprocess(source, amount);
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            /* Dongle side, voice path, bt source in case */
            if(source->param.bt_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
            {
                ble_audio_dongle_ul_source_drop_postprocess(source, amount);
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            configASSERT(0);
    }
}

/**
 * @brief This function is used to close the source based on the speical scenario type.
 *
 * @param source is the instance who needs to be closed.
 * @return true means this scenario use special method to close the source.
 * @return false means this scenario use common method to close the source.
 */
static bool port_bt_common_source_close(SOURCE source)
{
    bool ret = false;

    switch(source->param.bt_dl.scenario_type)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            /* Dongle side, voice path, bt source in case */
            if(source->param.bt_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
            {
                ret = gaming_mode_dongle_ul_source_close(source);
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            /* Dongle side, voice path, bt source in case */
            if(source->param.bt_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
            {
                ret = ble_audio_dongle_ul_source_close(source);
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            configASSERT(0);
            break;
    }

    return ret;
}

/******************************************************************************/
/*                     BT_COMMON Sink Private Functions                       */
/******************************************************************************/
/**
 * @brief This function is used to get the residual space size of the sink based on the speical scenario type.
 *
 * @param sink is the instance who is checked its residual space size.
 * @param avail_size is the actual residual size of the sink.
 * @return true means this scenario use special method to get the the residual space size.
 * @return false means this scenario use common method to get the the residual space size.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 static bool port_bt_common_sink_get_avail_size(SINK sink, uint32_t *avail_size)
{
    bool ret = false;

    *avail_size = 0;

    switch(sink->param.bt_ul.scenario_type)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            /* Dongle side, music path, bt sink out case */
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
            {
                ret = gaming_mode_dongle_dl_sink_get_avail_size(sink, avail_size);
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            /* Dongle side, music path, bt sink out case */
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
            {
                ret = ble_audio_dongle_dl_sink_get_avail_size(sink, avail_size);
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            configASSERT(0);
            break;
    }

    return ret;
}

/**
 * @brief This function is used to copy the data into the share buffer of the sink based on the speical scenario type.
 *
 * @param sink is the instance who owns the share buffer.
 * @param src_buf is the source address.
 * @param length is the wanted copy size.
 * @return uint32_t is the actual copy size.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 static uint32_t port_bt_common_sink_copy_payload(SINK sink, uint8_t *src_buf, uint32_t length)
{
    UNUSED(src_buf);

    switch(sink->param.bt_ul.scenario_type)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            /* Dongle side, music path, bt sink out case */
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
            {
                length = gaming_mode_dongle_dl_sink_copy_payload(sink, src_buf, length);
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            /* Dongle side, music path, bt sink out case */
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
            {
                length = ble_audio_dongle_dl_sink_copy_payload(sink, src_buf, length);
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            configASSERT(0);
            break;
    }

    return length;
}

/**
 * @brief This function is used to get the new write offset of the sink's share buffer based on the speical scenario type.
 *
 * @param sink is the instance who owns the share buffer.
 * @param amount is the least offset size of the write offset.
 * @param new_write_offset is the actual new write offset.
 * @return true means needs to update write offset.
 * @return false means does not need to update write offset.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 static bool port_bt_common_sink_get_new_write_offset(SINK sink, U32 amount, uint32_t *new_write_offset)
{
    bool ret = false;
    UNUSED(amount);
    UNUSED(new_write_offset);

    switch(sink->param.bt_ul.scenario_type)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            /* Dongle side, music path, bt sink out case */
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
            {
                ret = gaming_mode_dongle_dl_sink_get_new_write_offset(sink, amount, new_write_offset);
                return ret;
            }
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            /* Dongle side, music path, bt sink out case */
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
            {
                ret = ble_audio_dongle_dl_sink_get_new_write_offset(sink, amount, new_write_offset);
                return ret;
            }
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            configASSERT(0);
            return ret;
    }
}

/**
 * @brief This function is used to query if the notification needs to be sent based on the speical scenario type.
 *
 * @param sink is the instance who owns the notification.
 * @param notification_flag is the flag if needs to send notification. It it is set to true, the notification needs to be sent. Vice versa.
 * @return true means this scenario use special method to query if needs to send the notification.
 * @return false means this scenario use common method to query if needs to send the notification.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 static bool port_bt_common_sink_query_notification(SINK sink, bool *notification_flag)
{
    bool ret = false;

    *notification_flag = false;

    switch(sink->param.bt_ul.scenario_type)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            /* Dongle side, music path, bt sink out case */
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
            {
                ret = gaming_mode_dongle_dl_sink_query_notification(sink, notification_flag);
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            /* Dongle side, music path, bt sink out case */
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
            {
                ret = ble_audio_dongle_dl_sink_query_notification(sink, notification_flag);
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            configASSERT(0);
            break;
    }

    return ret;
}

/**
 * @brief This function is used to send the notification based on the speical scenario type.
 *
 * @param sink is the instance who owns the notification.
 * @return true means this scenario use special method to send the notification.
 * @return false means this scenario use common method to send the notification.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 static bool port_bt_common_sink_send_data_ready_notification(SINK sink)
{
    bool ret = false;

    switch(sink->param.bt_ul.scenario_type)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            /* Dongle side, music path, bt sink out case */
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
            {
                ret = gaming_mode_dongle_dl_sink_send_data_ready_notification(sink);
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            /* Dongle side, music path, bt sink out case */
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
            {
                ret = ble_audio_dongle_dl_sink_send_data_ready_notification(sink);
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            configASSERT(0);
            break;
    }

    return ret;
}

/**
 * @brief This function is used to do special thing based on the speical scenario type after the flush is done.
 *
 * @param sink is the instance who flush the data.
 * @param amount is the total length has been flushed.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 static void port_bt_common_sink_flush_postprocess(SINK sink, uint32_t amount)
{
    UNUSED(amount);

    switch(sink->param.bt_ul.scenario_type)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            /* Dongle side, music path, bt sink out case */
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
            {
                gaming_mode_dongle_dl_sink_flush_postprocess(sink, amount);
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            /* Dongle side, music path, bt sink out case */
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
            {
                ble_audio_dongle_dl_sink_flush_postprocess(sink, amount);
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            break;
    }
}

/**
 * @brief This function is used to close the sink based on the speical scenario type.
 *
 * @param sink is the instance who needs to be closed.
 * @return true means this scenario use special method to close the sink.
 * @return false means this scenario use common method to close the sink.
 */
static bool port_bt_common_sink_close(SINK sink)
{
    bool ret = false;

    switch(sink->param.bt_ul.scenario_type)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            /* Dongle side, music path, bt sink out case */
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
            {
                ret = gaming_mode_dongle_dl_sink_close(sink);
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            /* Dongle side, music path, bt sink out case */
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
            {
                ret = ble_audio_dongle_dl_sink_close(sink);
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            configASSERT(0);
            break;
    }

    return ret;
}


/* Public functions ----------------------------------------------------------*/
/******************************************************************************/
/*                   BT_COMMON Source Functions                               */
/******************************************************************************/
void bt_common_register_isr_handler(f_bt_common_ccni_callback_t callback)
{
    uint32_t saved_mask;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if ((bt_common_ccni_callback == NULL) || (bt_common_ccni_callback == callback))
    {
        bt_common_ccni_callback = callback;
    }
    else
    {
        DSP_MW_LOG_E("[audio transmitter][source]ccni callback is used\r\n", 0);
        configASSERT(0);
    }

    hal_nvic_restore_interrupt_mask(saved_mask);
}

void bt_common_unregister_isr_handler(f_bt_common_ccni_callback_t callback)
{
    uint32_t saved_mask;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if (bt_common_ccni_callback == callback)
    {
        bt_common_ccni_callback = NULL;
    }

    hal_nvic_restore_interrupt_mask(saved_mask);
}

/**
 * @brief This function is the ccni callback for triggering stream handling flow.
 *
 * @param event is ccni event.
 * @param msg is the ccni msg.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 void bt_common_source_ccni_handler(hal_ccni_event_t event, void * msg)
{
    hal_ccni_status_t status;
    UNUSED(event);
    UNUSED(msg);

    status = hal_ccni_mask_event(event);
    if(status != HAL_CCNI_STATUS_OK)
        DSP_MW_LOG_E("[bt common][source]ccni mask event: 0x%x something wrong, return is %d\r\n", 2, event, status);

    /* run registered callback */
    if (bt_common_ccni_callback != NULL)
    {
        bt_common_ccni_callback(event, msg);
    }
    else
    {
        DSP_MW_LOG_E("[bt common][source]ccni callback is NULL\r\n", 0);
    }

    status = hal_ccni_clear_event(event);
    if(status != HAL_CCNI_STATUS_OK)
        DSP_MW_LOG_E("CCNI clear event something wrong, return is %d\r\n", 1, status);

    status = hal_ccni_unmask_event(event);
    if(status != HAL_CCNI_STATUS_OK)
        DSP_MW_LOG_E("CM4 CCNI unmask event: 0x%x something wrong, return is %d\r\n", 2, event, status);
}

/**
 * @brief This function is the implementation about getting the available space size of the source in bt common architecture.
 *
 * @param source the instance who is checked its available space size.
 * @return uint32_t is the actual available size of the source.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 uint32_t SourceSize_bt_common(SOURCE source)
{
    bool ret;
    uint32_t avail_size = 0;

    bt_common_share_information_fetch(source, NULL);

    ret = port_bt_common_source_get_avail_size(source, &avail_size);
    if (ret == false) {
        avail_size = source->param.bt_dl.max_payload_size;
    }

    return avail_size;
}

/**
 * @brief This function is the implementation for management mode of the source in bt common architecture.
 *
 * @param source is the source instance.
 * @return uint8_t* is the return value.
 */
uint8_t* SourceMap_bt_common(SOURCE source)
{
    UNUSED(source);

    return NULL;
}

/**
 * @brief This function is the implementation for dropping data and updating the read offset in bt common architecture.
 *
 * @param source is the source instance.
 * @param amount the least offset size of the read offset.
 */
void SourceDrop_bt_common(SOURCE source, U32 amount)
{
    uint32_t ReadOffset;
    bool ret;

    bt_common_share_information_fetch(source, NULL);
    ret = port_bt_common_source_get_new_read_offset(source, amount, &ReadOffset);
    if (ret)
    {
        bt_common_share_information_update_read_offset(source, ReadOffset);
    }

    /* do special process after the drop done */
    port_bt_common_source_drop_postprocess(source, amount);
}

/**
 * @brief This function is the implementation for configuring the source in bt common architecture.
 *
 * @param source is the source instance.
 * @param type is the configuration type.
 * @param value is the new value of the configuration type.
 * @return true means the configuration is done successfully.
 * @return false means the configuration is done unsuccessfully.
 */
BOOL SourceConfigure_bt_common(SOURCE source, stream_config_type type, U32 value)
{
    bool ret;

    UNUSED(source);
    UNUSED(type);
    UNUSED(value);

    ret = port_bt_common_source_config(source, type, value);

    return ret;
}

/**
 * @brief This function is the implementation to read data from the source's share buffer in bt common architecture.
 *
 * @param source is the source instance.
 * @param dst_addr is the destination address.
 * @param length is the wanted copy size.
 * @return true means the read operation is done successfully.
 * @return false means the read operation is done unsuccessfully.
 */
BOOL SourceReadBuf_bt_common(SOURCE source, U8 *dst_addr, U32 length)
{
    uint32_t avail_size, payload_size;

    bt_common_share_information_fetch(source, NULL);

    /* available size double check */
    port_bt_common_source_get_avail_size(source, &avail_size);
    if(length > avail_size)
    {
        DSP_MW_LOG_E("[bt common][source]: error length %d, avail_size %d", 2, length, avail_size);
        return false;
    }

    /* Copy payload */
    payload_size = port_bt_common_source_copy_payload(  source,
                                                        dst_addr,
                                                        length);
    if (payload_size > source->param.bt_dl.max_payload_size) {
        DSP_MW_LOG_E("[bt common][source]: error payload size %d, max payload size %d", 2, payload_size, source->param.bt_dl.max_payload_size);
        return false;
    }
    source->param.bt_dl.frame_size = payload_size;

    return true;
}

/**
 * @brief This function is the implementation to close the source in bt common architecture.
 *
 * @param source is the source instance.
 * @return true means the close operation is done successfully.
 * @return false means the close operation is done unsuccessfully.
 */
BOOL SourceClose_bt_common(SOURCE source)
{
    UNUSED(source);

    return port_bt_common_source_close(source);
}

/**
 * @brief This function is the implementation to initialize the source in bt common architecture.
 *
 * @param source is the source instance.
 */
void SourceInit_bt_common(SOURCE source)
{
    // bt_common_share_information_reset_read_write_offset(source, NULL);

    /* interface init */
    source->sif.SourceSize          = SourceSize_bt_common;
    source->sif.SourceMap           = SourceMap_bt_common;
    source->sif.SourceDrop          = SourceDrop_bt_common;
    source->sif.SourceConfigure     = SourceConfigure_bt_common;
    source->sif.SourceReadBuf       = SourceReadBuf_bt_common;
    source->sif.SourceClose         = SourceClose_bt_common;
}

/******************************************************************************/
/*                     BT_COMMON Sink Functions                               */
/******************************************************************************/
/**
 * @brief This function is the implementation about getting the residual space size of the sink in bt common architecture.
 *
 * @param sink is the instance who is checked its residual space size.
 * @return uint32_t is the actual residual size of the sink.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 uint32_t SinkSlack_bt_common(SINK sink)
{
    bool ret;
    uint32_t avail_size;

    bt_common_share_information_fetch(NULL, sink);

    ret = port_bt_common_sink_get_avail_size(sink, &avail_size);
    if (ret == false) {
        avail_size = sink->param.bt_ul.max_payload_size;
    }

    return avail_size;
}

/**
 * @brief This function is the implementation for management mode of the sink in bt common architecture.
 *
 * @param sink is the sink instance.
 * @return uint8_t* is the return value.
 */
uint8_t* SinkMap_bt_common(SINK sink)
{
    UNUSED(sink);

    return NULL;
}

/**
 * @brief This function is the implementation for configuring the sink in bt common architecture.
 *
 * @param sink is the sink instance.
 * @param type is the configuration type.
 * @param value is the new value of the configuration type.
 * @return true means the configuration is done successfully.
 * @return false means the configuration is done unsuccessfully.
 */
BOOL SinkConfigure_bt_common(SINK sink, stream_config_type type, U32 value)
{
    UNUSED(sink);
    UNUSED(type);
    UNUSED(value);

    return true;
}

/**
 * @brief This function is the implementation to write data into the source's share buffer in bt common architecture.
 *
 * @param sink is the sink instance.
 * @param src_addr is the source address.
 * @param length is the wanted copy size.
 * @return true means the write operation is done successfully.
 * @return false means the write operation is done successfully.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 BOOL SinkWriteBuf_bt_common(SINK sink, U8 *src_addr, U32 length)
{
    uint32_t avail_size, payload_size;

    bt_common_share_information_fetch(NULL, sink);

    /* available size double check */
    port_bt_common_sink_get_avail_size(sink, &avail_size);
    if(length > avail_size)
    {
        DSP_MW_LOG_E("[bt common][sink]: error length %d, avail_size %d", 2, length, avail_size);
        return false;
    }

    /* Copy payload */
    payload_size = port_bt_common_sink_copy_payload(sink, src_addr, length);
    if (payload_size > sink->param.bt_ul.max_payload_size) {
        DSP_MW_LOG_E("[bt common][sink]: error payload size %d, max payload size %d", 2, payload_size, sink->param.bt_ul.max_payload_size);
        return FALSE;
    }
    sink->param.bt_ul.frame_size = payload_size;

    return TRUE;
}

/**
 * @brief This function is the implementation to flush data and update the write offset in bt common architecture.
 *
 * @param sink is the sink instance.
 * @param amount is the least offset size of the write offset.
 * @return true means the flush operation is done successfully.
 * @return false means the flush operation is done successfully.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 BOOL SinkFlush_bt_common(SINK sink, U32 amount)
{
    bool notification_flag;
    uint32_t WriteOffset;

    UNUSED(amount);

    /* If one frame has not fill done, don't notice host */
    if (sink->param.bt_ul.frame_size == 0) {
        return FALSE;
    }

    /* flush data */
    bt_common_share_information_fetch(NULL, sink);
    if (port_bt_common_sink_get_new_write_offset(sink, amount, &WriteOffset))
    {
        bt_common_share_information_update_write_offset(sink, WriteOffset);
    }

    /* send notification */
    notification_flag = false;
    if (port_bt_common_sink_query_notification(sink, &notification_flag) == false) {
        if (sink->param.bt_ul.current_notification_index >= sink->param.bt_ul.data_notification_frequency) {
            sink->param.bt_ul.current_notification_index = 0;
            notification_flag = true;
        } else {
            sink->param.bt_ul.current_notification_index++;
        }
    }
    if (notification_flag == true) {
        port_bt_common_sink_send_data_ready_notification(sink);
    }

    /* do special process after the flush done */
    port_bt_common_sink_flush_postprocess(sink, amount);

    return true;
}

/**
 * @brief This function is the implementation to close the sink in bt common architecture.
 *
 * @param sink is the sink instance.
 * @return true means the close operation is done successfully.
 * @return false means the close operation is done unsuccessfully.
 */
BOOL SinkClose_bt_common(SINK sink)
{
    UNUSED(sink);

    return port_bt_common_sink_close(sink);
}

/**
 * @brief This function is the implementation to initialize the sink in bt common architecture.
 *
 * @param sink is the source instance.
 */
void SinkInit_bt_common(SINK sink)
{
    bt_common_share_information_reset_read_write_offset(NULL, sink);

    /* interface init */
    sink->sif.SinkSlack        = SinkSlack_bt_common;
    sink->sif.SinkMap          = SinkMap_bt_common;
    sink->sif.SinkConfigure    = SinkConfigure_bt_common;
    sink->sif.SinkFlush        = SinkFlush_bt_common;
    sink->sif.SinkWriteBuf     = SinkWriteBuf_bt_common;
    sink->sif.SinkClose        = SinkClose_bt_common;
}

#endif /* MTK_AUDIO_BT_COMMON_ENABLE */
