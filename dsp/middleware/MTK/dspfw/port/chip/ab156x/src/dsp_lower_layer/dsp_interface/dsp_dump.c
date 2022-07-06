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

#include "types.h"
#include "syslog.h"
#include "dsp_dump.h"
#include "audio_nvdm_common.h"
#include "string.h"
#include "dsp_temp.h"

//#define MTK_AUDIO_DUMP_BY_SPDIF_ENABLE
//#define MTK_AUDIO_DUMP_BY_SPDIF_DEBUG_ENABLE

/******************************************************************************
* Constant Definitions
******************************************************************************/
#define MAX_DUMP_SIZE 2048
#define SUB_HEADER_SIZE 2
#define DUMP_BY_TUNING_TOOL 0

#define AUDIO_DUMP_DEVICE_NONE         0
#define AUDIO_DUMP_DEVICE_LOGGING      1
#define AUDIO_DUMP_DEVICE_MUX          2
#define AUDIO_DUMP_DEVICE_SPDIF        3
#define AUDIO_DUMP_DEVICE_I2S_MASTER   4
#define AUDIO_DUMP_DEVICE_I2S_SLAVE    5
#define AUDIO_DUMP_DEVICE_TYPE(device) ((device >> 8) & 0xFF)
#define AUDIO_DUMP_DEVICE_NUM(device)  (device & 0xFF)

/******************************************************************************
 * Variables
 ******************************************************************************/
#ifdef MTK_AUDIO_DUMP_BY_SPDIF_ENABLE
/*
 * 10ms: (96000 * 2 * 24) / (8 * 100) = 5760
 * MEMIF buffer requirement: 384 * 3 * 2 = 2304
 */
#define SDPIF_IRQ_COUNTER 384 /* 192 for each 2ms */
U8 g_audio_dump_buffer[16 * 1024 + 32];
#endif
U32 AudioDumpMask[DUMPDATA_MASK_WORD_NO];
U32 AudioDumpDevice; /**< @Value 0x00000002 @Desc 0:Disable, 1:LOGGING, 2:MUX, 3:SPDIF, 4:I2S_MASTER, 5:I2S_SLAVE, */

/******************************************************************************
 * Function Declaration
 ******************************************************************************/
void LOG_AUDIO_DUMP(U8 *audio, U32 audio_size, DSP_DATADUMP_MASK_BIT dumpID);


/******************************************************************************
 * Functions
 ******************************************************************************/
log_create_module(audio_module, PRINT_LEVEL_INFO);

#ifdef MTK_AUDIO_DUMP_BY_SPDIF_ENABLE

#include "FreeRTOS.h"
#include "semphr.h"
#include "dtm.h"
#include "hal_audio.h"
#include "dsp_buffer.h"
#include "common.h"

typedef union {
    struct DSP_DATADUMP_HEADER_stru
    {
        U8  _reserved1;
        U8  sync_word;
        U32 length:12;
        U32 check:4;

        U8  _reserved2;
        U8  source_type;
        U8  sink_type;
        U8  feature_number;
    } field;
    struct DSP_DATADUMP_STORE_stru
    {
        U32 length;

        U8  _reserved;
        U8  source_type;
        U8  sink_type;
        U8  feature_number;
    } store;
    U32  reg[2];
} DSP_DATADUMP_HEADER, *DSP_DATADUMP_HEADER_PTR;

typedef struct
{
    U32 BufSize;

    hal_audio_memory_parameter_t memory;
    hal_audio_device_parameter_t device;
    hal_audio_path_parameter_t   path;
    U8  TempBuf[8];
    U8* BufPtr;
    U16 BufCnt;
    U16 BufWo;
    U16 BufRo;

    U16 _reserved;

    DSP_DATADUMP_HEADER header;
    QueueHandle_t DataDump_Semaphore;
} DSP_DATADUMP_CTRL_STRU, *DSP_DATADUMP_CTRL_STRU_ptr;

#define DSP_DUMP_HEADER_STRUCT_SIZE         (sizeof(DSP_DATADUMP_HEADER))
#define DSP_DATADUMP_SYNC_WORD      (0x5A)
#define DSP_DATADUMP_MASK_ID_TYPE   (0xFE)
#define DUMPDATA_MASK_WORD_NO       (2)
#define NO_OF_BITS_IN_A_WORD        (32)

static DSP_DATADUMP_CTRL_STRU DSP_DumpData_Ctrl;

#ifdef MTK_AUDIO_DUMP_BY_SPDIF_DEBUG_ENABLE
static uint8_t g_test_buffer[512];
#endif

VOID DSP_DataDump_UpdateBufferWriteOffset(VOID *sizePtr)
{
    U16 *size;
    size = (U16*)sizePtr;
    DSP_DumpData_Ctrl.BufWo  = (DSP_DumpData_Ctrl.BufWo + *size)%DSP_DumpData_Ctrl.BufSize;
    DSP_DumpData_Ctrl.BufCnt += *size;
}

VOID DSP_DataDump_UpdateBufferReadOffset(VOID *sizePtr)
{
    U16 *size;
    size = (U16*)sizePtr;
    DSP_DumpData_Ctrl.BufRo  = (DSP_DumpData_Ctrl.BufRo + *size)%DSP_DumpData_Ctrl.BufSize;
    DSP_DumpData_Ctrl.BufCnt -= *size;
}

static void DSP_DataDump_DataOut(void)
{
    uint32_t dl_read_pointer, frame_size, write_offset, readSize;
    int32_t out_remain;
    uint32_t out_ptr, save_out_ptr;
    hal_audio_get_value_parameter_t get_value_parameter;
    DSP_DATADUMP_HEADER_PTR outHeader;

    xSemaphoreTake(DSP_DumpData_Ctrl.DataDump_Semaphore, portMAX_DELAY);

    get_value_parameter.get_current_offset.memory_select = HAL_AUDIO_MEMORY_DL_DL12;
    get_value_parameter.get_current_offset.pure_agent_with_src = false;
    dl_read_pointer = hal_audio_get_value(&get_value_parameter, HAL_AUDIO_GET_MEMORY_OUTPUT_CURRENT_OFFSET);

    frame_size = DSP_DumpData_Ctrl.memory.irq_counter * sizeof(uint32_t) * 2;// Interleaved buffer

    write_offset = ((1+((dl_read_pointer - (U32)DSP_DumpData_Ctrl.memory.buffer_addr)/frame_size))*frame_size)%DSP_DumpData_Ctrl.memory.buffer_length;
    out_remain = frame_size;

    out_ptr = (DSP_DumpData_Ctrl.memory.buffer_addr + write_offset);
    save_out_ptr = out_ptr;
    if ((write_offset + frame_size)<=DSP_DumpData_Ctrl.memory.buffer_length) {
        memset((VOID*)out_ptr, 0, frame_size);
    } else {
        memset((VOID*)out_ptr, 0, DSP_DumpData_Ctrl.memory.buffer_length-write_offset);
        memset((VOID*)(DSP_DumpData_Ctrl.memory.buffer_addr), 0, frame_size-(DSP_DumpData_Ctrl.memory.buffer_length-write_offset));
    }

    out_remain = frame_size;
    while ((out_remain > 0 ) && (DSP_DumpData_Ctrl.BufCnt != 0)) {
        if((out_remain == (int32_t)frame_size) || (DSP_DumpData_Ctrl.header.store.length==0)) {

            if (out_remain <= (int32_t)DSP_DUMP_HEADER_STRUCT_SIZE)
            {
                break;
            }

            if (DSP_DumpData_Ctrl.header.store.length == 0)
            {
                readSize = DSP_DUMP_HEADER_STRUCT_SIZE;
                DSP_C2D_BufferCopy((VOID*)&DSP_DumpData_Ctrl.header.reg[0],
                                   (VOID*)((U8*)DSP_DumpData_Ctrl.BufPtr + DSP_DumpData_Ctrl.BufRo),
                                   readSize,
                                   (VOID*)DSP_DumpData_Ctrl.BufPtr,
                                   DSP_DumpData_Ctrl.BufSize);

#ifdef MTK_AUDIO_DUMP_BY_SPDIF_DEBUG_ENABLE
                DSP_MW_LOG_I("[SPDIF_DUMP][read] detect new package: BufWo = 0x%08x, BufRo = 0x%08x, header[0] = 0x%08x, header[1] = 0x%08x", 4,
                                DSP_DumpData_Ctrl.BufWo, DSP_DumpData_Ctrl.BufRo,
                                DSP_DumpData_Ctrl.header.reg[0], DSP_DumpData_Ctrl.header.reg[1]);
#endif

                DSP_DataDump_UpdateBufferReadOffset((VOID*)&readSize);
            }
            outHeader = (DSP_DATADUMP_HEADER_PTR)out_ptr;
            outHeader->field.sync_word = DSP_DATADUMP_SYNC_WORD;
            outHeader->field.length = MIN((((out_remain-DSP_DUMP_HEADER_STRUCT_SIZE)*3)>>2), DSP_DumpData_Ctrl.header.store.length);
            outHeader->field.check = ~((outHeader->field.sync_word + outHeader->field.length)&0xF);

            outHeader->reg[1] = DSP_DumpData_Ctrl.header.reg[1];

#ifdef MTK_AUDIO_DUMP_BY_SPDIF_DEBUG_ENABLE
            DSP_MW_LOG_I("[SPDIF_DUMP][read] assembly new package: write_offset = 0x%08x, BufWo = 0x%08x, BufRo = 0x%08x, length %d, header[0] = 0x%08x, header[1] = 0x%08x", 6,
                            write_offset, DSP_DumpData_Ctrl.BufWo, DSP_DumpData_Ctrl.BufRo,
                            outHeader->field.length, outHeader->reg[0], outHeader->reg[1]);
#endif
        } else {

            readSize = MIN(DSP_DumpData_Ctrl.header.store.length, 6);
            memset(DSP_DumpData_Ctrl.TempBuf, 0, 6);
            DSP_C2D_BufferCopy((VOID*)DSP_DumpData_Ctrl.TempBuf,
                               (VOID*)((U8*)DSP_DumpData_Ctrl.BufPtr + DSP_DumpData_Ctrl.BufRo),
                               readSize,
                               (VOID*)DSP_DumpData_Ctrl.BufPtr,
                               DSP_DumpData_Ctrl.BufSize);


            memcpy((VOID*)(out_ptr + 1), (VOID*)&DSP_DumpData_Ctrl.TempBuf[0], 3);
            memcpy((VOID*)(out_ptr + 5), (VOID*)&DSP_DumpData_Ctrl.TempBuf[3], 3);

            DSP_DumpData_Ctrl.header.store.length -= readSize;
            DSP_DataDump_UpdateBufferReadOffset((VOID*)&readSize);

        }
        out_remain -= 8;
        write_offset += 8;
        write_offset %= DSP_DumpData_Ctrl.memory.buffer_length;
        out_ptr = (DSP_DumpData_Ctrl.memory.buffer_addr + write_offset);
    }

    xSemaphoreGive(DSP_DumpData_Ctrl.DataDump_Semaphore);
}





/**
 * @brief Data in for dump.
 *
 * @param inBuf         Pointer to the source of data to be dump.
 * @param dataLength    Pointer to the source of data to be dump.
 * @param dumpID        dump ID.
 */
static U32 DSP_DataDump_DataIn (U8* inBuf, U16 dataLength, DSP_DATADUMP_MASK_BIT dumpID)
{
    U16 i;
    U16 dumpIDinWord;
    U32 dumpWordOffset, irq_mask;
    U16 storeSize = 0, readSize = 0, remainSize;
    DSP_DATADUMP_HEADER_PTR inBufPtr;
    DSP_DATADUMP_HEADER inTempBuf;

    //DSP_MW_LOG_I("[SPDIF_DUMP] data in begin", 0);

    dumpWordOffset   = dumpID / NO_OF_BITS_IN_A_WORD;
    dumpIDinWord     = dumpID % NO_OF_BITS_IN_A_WORD;
    if (inBuf == NULL)
        return 0;

    xSemaphoreTake(DSP_DumpData_Ctrl.DataDump_Semaphore, portMAX_DELAY);

    for (i=0 ; i<dataLength ; i+=readSize)
    {
        remainSize
            = (DSP_DumpData_Ctrl.BufSize-DSP_DumpData_Ctrl.BufCnt > DSP_DUMP_HEADER_STRUCT_SIZE)
                ? (DSP_DumpData_Ctrl.BufSize-DSP_DumpData_Ctrl.BufCnt-DSP_DUMP_HEADER_STRUCT_SIZE)
                    : 0;

        if ((DSP_DumpData_Ctrl.BufSize-DSP_DumpData_Ctrl.BufCnt) > DSP_DUMP_HEADER_STRUCT_SIZE)
        {
            readSize = MIN(remainSize, dataLength-i);
        }
        else
        {
            xSemaphoreGive(DSP_DumpData_Ctrl.DataDump_Semaphore);
            return i;
        }

        inBufPtr = &inTempBuf;
        inBufPtr->store.length = readSize;
        if (dumpWordOffset<DUMPDATA_MASK_WORD_NO)
        {
            inBufPtr->store._reserved = 0;
            inBufPtr->store.source_type = DSP_DATADUMP_MASK_ID_TYPE;
            inBufPtr->store.sink_type = DSP_DATADUMP_MASK_ID_TYPE;
            inBufPtr->store.feature_number = dumpID;
        }
        else
        {
            inBufPtr->reg[1] = dumpID;
        }

        DSP_D2C_BufferCopy((U8*)((U32)DSP_DumpData_Ctrl.BufPtr + DSP_DumpData_Ctrl.BufWo),
                           inBufPtr,
                           DSP_DUMP_HEADER_STRUCT_SIZE,
                           DSP_DumpData_Ctrl.BufPtr,
                           (U16)DSP_DumpData_Ctrl.BufSize);

        DSP_D2C_BufferCopy((U8*)((U32)DSP_DumpData_Ctrl.BufPtr + ((DSP_DumpData_Ctrl.BufWo + DSP_DUMP_HEADER_STRUCT_SIZE) % DSP_DumpData_Ctrl.BufSize)),
                           inBuf,
                           readSize,
                           DSP_DumpData_Ctrl.BufPtr,
                           (U16)DSP_DumpData_Ctrl.BufSize);

#ifdef MTK_AUDIO_DUMP_BY_SPDIF_DEBUG_ENABLE
        DSP_MW_LOG_I("[SPDIF_DUMP][write] dumpID = %d, BufWo = 0x%08x, BufRo = 0x%08x, WriteSize = %d", 4, dumpID, DSP_DumpData_Ctrl.BufWo, DSP_DumpData_Ctrl.BufRo, readSize);
#endif

        inBuf += readSize;

        storeSize = readSize + DSP_DUMP_HEADER_STRUCT_SIZE;

        hal_nvic_save_and_set_interrupt_mask(&irq_mask);
        DSP_DataDump_UpdateBufferWriteOffset(&storeSize);
        hal_nvic_restore_interrupt_mask(irq_mask);

    }

    xSemaphoreGive(DSP_DumpData_Ctrl.DataDump_Semaphore);

    //DSP_MW_LOG_I("[SPDIF_DUMP] data in end", 0);

    return dataLength;
}


void audio_dump_task_handler(uint32_t arg)
{
    arg = arg;

    DSP_DataDump_DataOut();
}

static void spdif_dump_isr_handler(void)
{
    /* wake up DTM task to do MEMIF buffer fill */
    DTM_enqueue(DTM_EVENT_ID_AUDIO_DUMP, 0, true);
}

static void spdif_dump_init(void)
{
    hal_audio_device_parameter_t *device_handle = &(DSP_DumpData_Ctrl.device);
    hal_audio_path_parameter_t *path_handle = &(DSP_DumpData_Ctrl.path);
    hal_audio_memory_parameter_t *memory_handle = &(DSP_DumpData_Ctrl.memory);
    hal_audio_irq_parameter_t   register_irq_handler;

    memset(device_handle, 0, sizeof(hal_audio_device_parameter_t));
    memset(path_handle, 0, sizeof(hal_audio_path_parameter_t));
    memset(memory_handle, 0, sizeof(hal_audio_memory_parameter_t));

    //Set device parameters
    device_handle->spdif.i2s_setting.audio_device           = HAL_AUDIO_CONTROL_DEVICE_SPDIF;
    device_handle->spdif.i2s_setting.rate                   = 96000;
    device_handle->spdif.i2s_setting.i2s_interface          = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
    device_handle->spdif.i2s_setting.i2s_format             = HAL_AUDIO_I2S_I2S;
    device_handle->spdif.i2s_setting.word_length            = HAL_AUDIO_I2S_WORD_LENGTH_32BIT;
    device_handle->spdif.i2s_setting.mclk_divider           = 2;
    device_handle->spdif.i2s_setting.with_mclk              = false;
    device_handle->spdif.i2s_setting.is_low_jitter          = true; //Need to open PLL
    device_handle->spdif.i2s_setting.is_rx_swap             = false;
    device_handle->spdif.i2s_setting.is_tx_swap             = false;
    device_handle->spdif.i2s_setting.is_internal_loopback   = false;
    device_handle->spdif.i2s_setting.is_recombinant         = false;

    //Set memory parameters
    memory_handle->memory_select            = HAL_AUDIO_MEMORY_DL_DL12;
    memory_handle->pcm_format               = HAL_AUDIO_PCM_FORMAT_S32_LE;
    memory_handle->sync_status              = HAL_AUDIO_MEMORY_SYNC_NONE;
    memory_handle->audio_path_rate          = device_handle->spdif.i2s_setting.rate;
    memory_handle->buffer_addr              = 0;
    memory_handle->irq_counter              = SDPIF_IRQ_COUNTER; //Units:sample
    memory_handle->buffer_length            = memory_handle->irq_counter * sizeof(uint32_t) * 2 * 2; /* store 2 frame */
    memory_handle->initial_buffer_offset    = 32;
    memory_handle->with_mono_channel        = false;
    memory_handle->pure_agent_with_src      = false;
    memory_handle->src_tracking_clock_source= HAL_AUDIO_SRC_TRACKING_DISABLE;

    //Set path parameters
    path_handle->input.interconn_sequence[0]    = HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL12_CH1;
    path_handle->input.interconn_sequence[1]    = HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL12_CH2;
    path_handle->output.interconn_sequence[0]   = HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S0_CH1; //Todo : Convert audio device 
    path_handle->output.interconn_sequence[1]   = HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S0_CH2; //Todo : Convert audio device
    path_handle->connection_number              = 2;
    path_handle->audio_path_input_rate          = memory_handle->audio_path_rate;
    path_handle->audio_path_output_rate         = device_handle->spdif.i2s_setting.rate;
    path_handle->with_hw_gain                   = false;
    path_handle->with_upwdown_sampler           = false;
    path_handle->with_dl_deq_mixer              = false;

    //Register ISR handler
    register_irq_handler.audio_irq = HAL_AUDIO_IRQ_AUDIOSYS;
    register_irq_handler.memory_select = memory_handle->memory_select;
    register_irq_handler.entry = spdif_dump_isr_handler; //ISR entry
    hal_audio_set_value((hal_audio_set_value_parameter_t *)&register_irq_handler, HAL_AUDIO_SET_IRQ_HANDLER);

    //Enable
    hal_audio_set_device(device_handle, device_handle->common.audio_device, HAL_AUDIO_CONTROL_ON);
    hal_audio_set_path(path_handle, HAL_AUDIO_CONTROL_ON);
    hal_audio_set_memory(memory_handle, HAL_AUDIO_CONTROL_MEMORY_INTERFACE, HAL_AUDIO_CONTROL_ON);

    DSP_DumpData_Ctrl.BufPtr = &g_audio_dump_buffer[16];
    DSP_DumpData_Ctrl.BufSize = sizeof(g_audio_dump_buffer) - 16;
    DSP_DumpData_Ctrl.BufCnt = 0;
    DSP_DumpData_Ctrl.BufWo  = 0;
    DSP_DumpData_Ctrl.BufRo  = 0;
    DSP_DumpData_Ctrl.header.reg[0] = 0;
    DSP_DumpData_Ctrl.header.reg[1] = 0;

    DSP_DumpData_Ctrl.DataDump_Semaphore = xSemaphoreCreateMutex();
    assert(DSP_DumpData_Ctrl.DataDump_Semaphore != NULL);

#ifdef MTK_AUDIO_DUMP_BY_SPDIF_DEBUG_ENABLE

{
    uint32_t i, seq;
    uint8_t *p_buf;

#if 1
    for (i = 0; i < sizeof(g_test_buffer); i++) {
        g_test_buffer[i] = i % 256;
    }
#endif
#if 0
    seq = 0;
    p_buf = (uint8_t *)(memory_handle->buffer_addr);
    for (i = 0; i < memory_handle->buffer_length; i += 4) {
        p_buf[i] = 0;
        p_buf[i+1] = seq++ % 256;
        p_buf[i+2] = seq++ % 256;
        p_buf[i+3] = seq++ % 256;
    }
#endif
}
    DSP_MW_LOG_I("[SPDIF_DUMP] init 0x%08x, 0x%08x, 0x%08x, 0x%08x", 4, *(volatile uint32_t *)(0xC00003E4), *(volatile uint32_t *)(0xC00003A0), *(volatile uint32_t *)(0xC00002E4), *(volatile uint32_t *)(0xC000030C));
#endif

    DSP_MW_LOG_I("[SPDIF_DUMP] init done", 0);
}

void audio_dump_init(void)
{
    if (AUDIO_DUMP_DEVICE_TYPE(AudioDumpDevice) == AUDIO_DUMP_DEVICE_SPDIF) {
        spdif_dump_init();
    }
}

#endif

void LOG_AUDIO_DUMP(U8 *audio, U32 audio_size, DSP_DATADUMP_MASK_BIT dumpID)
{
    U32 left_size, curr_size, left_send_size, curr_send_size = 0;
    U8 *p_curr_audio;
    U8 dump_header[SUB_HEADER_SIZE];

    dump_header[0] = dumpID;
    dump_header[1] = 0;

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    U16 dumpIDinWord;
    U32 dumpWordOffset;
    //DSP_MW_LOG_I("DSP_Dump_NvKey.Mask[0]: %d", 1, DSP_Dump_NvKey.Mask[0]);
    //DSP_MW_LOG_I("DSP_Dump_NvKey.Mask[1]: %d", 1, DSP_Dump_NvKey.Mask[1]);

    dumpWordOffset   = dumpID / NO_OF_BITS_IN_A_WORD;
    dumpIDinWord     = dumpID % NO_OF_BITS_IN_A_WORD;

    /*DSP_MW_LOG_I("dumpID: %d, dumpWordOffset:%d, dumpIDinWord:%d", 3, dumpID,dumpWordOffset,dumpIDinWord);
    DSP_MW_LOG_I("DSP_Dump_NvKey.Mask[dumpWordOffset]: %x", 1, DSP_Dump_NvKey.Mask[dumpWordOffset]);
    DSP_MW_LOG_I("1 << dumpIDinWord: %x", 1, (1 << dumpIDinWord));*/

    if ((dumpWordOffset>=DUMPDATA_MASK_WORD_NO) ||
       ((AudioDumpMask[dumpWordOffset] & (1 << dumpIDinWord)) != 0)){
#endif
        if (AUDIO_DUMP_DEVICE_TYPE(AudioDumpDevice) == AUDIO_DUMP_DEVICE_LOGGING) {
            left_size    = audio_size;
            p_curr_audio = audio;
            while(left_size) {
                if (left_size <= (MAX_DUMP_SIZE - SUB_HEADER_SIZE)) {
                    curr_size = left_size;
                } else {
                    curr_size = MAX_DUMP_SIZE - SUB_HEADER_SIZE;
                }
                uint8_t *audio_buffer_array[] = {dump_header, p_curr_audio, NULL};
                uint32_t audio_buffer_length_array[] = {SUB_HEADER_SIZE, curr_size};
                left_send_size = curr_size + SUB_HEADER_SIZE;
                LOG_TLVDUMP_I(audio_module, LOG_TYPE_AUDIO_DATA, audio_buffer_array, audio_buffer_length_array, curr_send_size);
                if (curr_send_size != left_send_size) {
                    DSP_MW_LOG_I("audio dump drop happen, total %d, send %d", 2, left_send_size, curr_send_size);
                    return;
                }
                p_curr_audio += curr_size;
                left_size -= curr_size;
            }
        }
#ifdef MTK_AUDIO_DUMP_BY_SPDIF_ENABLE
        else if (AUDIO_DUMP_DEVICE_TYPE(AudioDumpDevice) == AUDIO_DUMP_DEVICE_SPDIF) {
            U32 miss_cnt = 0;
#ifdef MTK_AUDIO_DUMP_BY_SPDIF_DEBUG_ENABLE
            left_size = sizeof(g_test_buffer);
            p_curr_audio = g_test_buffer;
#else
            left_size = audio_size;
            p_curr_audio = audio;
#endif
            do {
                curr_size = DSP_DataDump_DataIn(p_curr_audio, left_size, dumpID);
                if ((curr_size == 0) && (miss_cnt++ >= 3)) {
                    miss_cnt--;
                    DSP_MW_LOG_E("audio dump drop happen, miss_cnt:%d", 1, miss_cnt);
                    break;
                }
                left_size -= curr_size;
                p_curr_audio += curr_size;
            } while (left_size);
        }
#endif
        else {
            DSP_MW_LOG_E("audio dump, device %d not support", 1, AudioDumpDevice);
        }
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    }
#endif
}

