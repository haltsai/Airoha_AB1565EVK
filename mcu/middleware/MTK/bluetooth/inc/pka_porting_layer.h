
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

#ifndef __BT_CON_PORTING_H__
#define __BT_CON_PORTING_H__

#include <stdint.h>


#define ATTR_LOG_STRING      __attribute__ ((__section__(".log_string"))) static const char




typedef enum
{
    LOG_DM2L,
    LOG_LC,
    LOG_COMMON,
    LOG_NUM
}pka_log_type_t;


void bt_pka_log_msgid_i(pka_log_type_t type, const char *msg, uint32_t arg_cnt, ...);
void bt_pka_log_msgid_d(pka_log_type_t type, const char *msg, uint32_t arg_cnt, ...);
void bt_pka_log_msgid_w(pka_log_type_t type, const char *msg, uint32_t arg_cnt, ...);
void bt_pka_log_msgid_e(pka_log_type_t type, const char *msg, uint32_t arg_cnt, ...);
void bt_pka_dump_msgid_i(pka_log_type_t type , const void *data, uint32_t length, const char *message, ...);


#define LOG_MSG_I(log_type, msg, argc, ...)    \
do{ \
    ATTR_LOG_STRING msg_id_string[] = "[M:PKA_"#log_type" C:info F: L: ]: "msg;\
    bt_pka_log_msgid_i(log_type, msg_id_string, argc, ## __VA_ARGS__); \
}while(0)

#define LOG_MSG_D(log_type, msg, argc, ...)    \
do{ \
    ATTR_LOG_STRING msg_id_string[] = "[M:PKA_"#log_type" C:debug F: L: ]: "msg;\
    bt_pka_log_msgid_d(log_type, msg_id_string, argc, ## __VA_ARGS__); \
}while(0)

#define LOG_MSG_W(log_type, msg, argc, ...)    \
do{ \
    ATTR_LOG_STRING msg_id_string[] = "[M:PKA_"#log_type" C:warning F: L: ]: "msg;\
    bt_pka_log_msgid_w(log_type, msg_id_string, argc, ## __VA_ARGS__); \
}while(0)

#define LOG_MSG_E(log_type, msg, argc, ...)    \
do{ \
    ATTR_LOG_STRING msg_id_string[] = "[M:PKA_"#log_type" C:error F: L: ]: "msg;\
    bt_pka_log_msgid_e(log_type, msg_id_string, argc, ## __VA_ARGS__); \
}while(0)
#define DUMP_MSG_I(log_type, data, length, msg, ...)    \
do{ \
    bt_pka_dump_msgid_i(log_type, data, length, msg, ## __VA_ARGS__); \
}while(0)
typedef enum
{
	xNoAction = 0,				/* Notify the task without updating its notify value. */
	xSetBits,					/* Set bits in the task's notification value. */
	xIncrement,					/* Increment the task's notification value. */
	xSetValueWithOverwrite,		/* Set the task's notification value to a specific value even if the previous value has not yet been read by the task. */
	xSetValueWithoutOverwrite	/* Set the task's notification value if the previous value has been read by the task. */
} xNotifyAction;

typedef void (*pTimerCallBack)( void* ExpiredTimer);
typedef void (*pTaskFun)(void);
typedef void (*bt_isr_t)(uint32_t irq_number);

#define STATIC_ASSERT(COND,MSG) typedef char static_assertion_##MSG[(!!(COND))*2-1]
#define COMPILE_TIME_ASSERT4(X,MSG,L) STATIC_ASSERT(X,MSG##L)
#define COMPILE_TIME_ASSERT3(X,MSG,L) COMPILE_TIME_ASSERT4(X,MSG,_at_line_##L)
#define COMPILE_TIME_ASSERT2(X,MSG,L) COMPILE_TIME_ASSERT3(X,MSG,L)
#define COMPILE_TIME_ASSERT(X,MSG)    COMPILE_TIME_ASSERT2(X,MSG,__LINE__)

char* pka_os_get_task_name(void);

void pka_os_task_resume(void* taskHandle);

uint32_t  pka_os_task_resume_from_isr(void* taskHandle);

void *pka_os_get_timer_id( void * xTimer );

void pka_os_cancel_timer( void * xTimer );

//void* pka_os_create_timer( const char * const pcTimerName, const uint32_t  xTimerPeriodInTicks, const uint32_t  uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction );

void pka_os_start_timer( void * xTimer );

void pka_os_task_suspend(void* taskHandle);

void pka_os_task_notify_wait(uint32_t  ulBitsToClearOnEntry, uint32_t  ulBitsToClearOnExit, uint32_t  *pulNotificationValue);

//void pka_os_task_notify_from_isr( void* xTaskToNotify, uint32_t  ulValue, xNotifyAction eAction, BaseType_t *pxHigherPriorityTaskWoken );

void pka_os_task_notify( void* xTaskToNotify, uint32_t  ulValue, xNotifyAction eAction);

void* pka_os_semaphore_init();

void pka_os_semaphore_take(void* pSemaphore);

void pka_os_semaphore_give(void* pSemaphore);

//void pka_os_task_create(TaskFunction_t fTaskEntry, const char * const taskName, uint32_t  stackSize, void * const pParameters, uint8_t  taskPriority, void* taskHandle);

uint32_t  pka_os_get_interrupt_mask(void);

void pka_os_restore_interrupt_mask(uint32_t  nvic_mask);

uint32_t pka_gpt_get_current_time(void);

void pka_os_register_bt_interrupt(bt_isr_t intr_handler);

void pka_os_register_bt_timer_interrupt(bt_isr_t intr_handler);

void pka_os_enable_bt_and_timer_interrupt();

void pka_os_disable_bt_and_timer_interrupt();

void pka_os_yield_from_isr(void);

uint32_t pka_os_is_run_in_isr(void);


uint32_t pka_os_get_lm_task_priority(void);

uint32_t pka_os_get_lc_task_priority(void);

uint32_t pka_os_get_lc_process_task_priority(void);


void pka_disable_all_irq_except_bt(void);


void pka_restore_all_irq_except_bt(void);

/* big endian input, output */
void pka_uECC_p192_compute_public_key(uint8_t *privatekey, uint8_t *publicKey);

void pka_uECC_p192_shared_secret(uint8_t * remotePublicKey, uint8_t *privatekey, uint8_t *DHKey);

void pka_uECC_p256_compute_public_key(uint8_t *privatekey, uint8_t *publicKey);

void pka_uECC_p256_shared_secret(uint8_t * remotePublicKey, uint8_t *privatekey, uint8_t *DHKey);

int8_t pka_hal_gpt_delay_us( uint32_t us);

void pka_external_hmac_sha256(uint8_t *input, uint8_t ilen, uint8_t *key, uint8_t keylen, uint8_t *output);

void pka_external_sha256(uint8_t *input, uint8_t ilen, uint8_t *output);

void pka_hal_nvic_save_and_set_interrupt_mask_special(uint32_t* mask);

void pka_hal_nvic_restore_interrupt_mask_special(uint32_t mask);

typedef struct LEAudio_DLUL_play_info {
    uint32_t  ISO_interval;//Unit:CLK
    uint32_t  DL_TimeStampClk; //Unit: CLK(312.5us), indicate the first anchor of this DL
    uint32_t  DL_RetransmissionWindowClk;//Unit: CLK(312.5us), valid bit[27:2]
    uint16_t  DL_TimeStampPhase; //Unit: 0.5us, indicate the first anchor of this DL
    uint16_t  DL_RetransmissionWindowPhase;//Unit:0.5us, valid value : 0~2499
    uint8_t   DL_FT;
    uint8_t   DL_PacketCounter;// =ISO DL packetCounter & 0xFF
    uint8_t   UL_FT;
    uint8_t   UL_PacketCounter;// = ISO UL packetCounter & 0xFF
    uint32_t  UL_TimeStamp;//Unit: CLK(312.5us), indicate the first anchor of this UL
    uint32_t  ISOGAnchorClk;
    uint16_t  ISOGAnchorPhase;
}LEAUDIO_DLUL_PLAY_INFO, *LEAUDIO_DLUL_PLAY_INFO_PTR;

uint32_t pka_Send_LEAudio_PlayInfo_ToDSP(uint32_t Address);
void pka_dwt_requsest_watchpoint(uint32_t addr_base);

#endif

