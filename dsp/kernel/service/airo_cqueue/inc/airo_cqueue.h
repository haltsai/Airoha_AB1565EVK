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

#ifndef _AIRO_CQUEUE_H
#define _AIRO_CQUEUE_H
#include <stdint.h>
#include <stdbool.h>

/**
 * @addtogroup kernel_service
 * @{
 * @addtogroup Airo_cqueue
 * @{
 * This section introduces the airo_cqueue APIs including supported features, software architecture, and details on how to use airo_cqueue structures and functions.
 * Airo_cqueue is a lightweight fixed size queue service. It can be used to send messages between tasks, and between interrupts and tasks.
 * It is used as a thread-safe First In First Out (FIFO) buffer with new data being sent to the back of the queue.
 * Suggest Airo_cqueue is used only when the system doesn't have operation system, such as FreeRTOS, because of the limited function.
 *
 * @section Airo_cqueue_Usage_Chapter How to use airo_cqueue
 *  Use airo_cqueue to create a queue. Then, send to and receive from the queue. For example:
 *  - \b Step 1. Include airo_cqueue module.mk in your project makefile.
 *  @code
 *  //file:dsp\project\ab1558_evk\templates\dsp1_create_message_queue\XT-XCC\Makefile
 *   ################################################################################
 *   # Include Module Configuration
 *   ################################################################################
 *   include $(ROOTDIR)/kernel/service/airo_cqueue/module.mk
 *  @endcode
 *  - \b Step 2. Define the queue message structure. Queue item counts as your request, and statically allocates memory for the queue.
 *       Note: The buffer must be allocated at the word aligned address, and must also include the queue ctrl block size size(airo_cqueue_t).
 *  @code
 *  //file:dsp\project\ab1558_evk\templates\dsp1_create_message_queue\inc\message_queue_configure.h
 *  typedef struct {
 *      message_event_id_t event_id;
 *  } message_queue_t;
 *  //file:dsp\project\ab1558_evk\templates\dsp1_create_message_queue\src\main.c
 *  #define MESSAGE_QUEUE_ITEMS 16
 *  void *p_message_queue = NULL;
 *  __attribute__((__aligned__(4))) char queue_buffer[sizeof(airo_cqueue_t) + MESSAGE_QUEUE_ITEMS * sizeof(message_queue_t)];
 *  @endcode
 *  - \b Step 3. Call airo_cqueue_create() to create a queue.
 *  @code
 *  //file:dsp\project\ab1558_evk\templates\dsp1_create_message_queue\src\main.c
 *  p_message_queue = airo_cqueue_create(MESSAGE_QUEUE_ITEMS, sizeof(message_queue_t), (void *)queue_buffer);
 *  assert(p_message_queue);
 *  @endcode
 *  - \b Step 4. Package a message and call airo_cqueue_send() to send the messgae to the queue.
 *  @code
 *  //file:dsp\kernel\service\pre_libloader\dsp1\dsp1_pic_demo_portable\dsp1_pisplit_demo_library_portable.c
 *  bool ret = false;
 *  message_queue_t dsp1_lib_demo_message;
 *  dsp1_lib_demo_message.event_id = MESSAGE_DSP1_PISPLIT_LIBRARY_DEMO_LOADING_DONE;
 *  ret = airo_cqueue_send(p_message_queue, (void *)&dsp1_lib_demo_message);
 *  assert(ret);
 *  @endcode
 *  - \b Step 5. Call airo_cqueue_receive() to receive a message from the queue.
 *  @code
 *  //file:dsp\project\ab1558_evk\templates\dsp1_create_message_queue\src\main.c
 *  //Main loop to handle message
 *  while (airo_cqueue_message_waiting(p_message_queue)) {
 *      //Receive a message from preloader queue
 *      ret = airo_cqueue_receive(p_message_queue, (void *)&message);
 *      if (ret) {
 *          //......
 *      }
 *  }
 *  @endcode
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Structures
 *****************************************************************************/
/** @defgroup airo_cqueue Struct
 *  @{
 */
/** @brief airo_cqueue structure */
typedef struct {
    uint32_t    head;       /**< The next message to read */
    uint32_t    tail;       /**< The next space to write */
    uint32_t    length;     /**< The queue lenth */
    uint32_t    item_size;  /**< The size of the queue item */
    uint32_t    used_item;  /**< The count of available messages */
    bool        is_full;    /**< The flag to mark whether the queue is full */
    char        res[3];     /**< Reserved for alignement */
} airo_cqueue_t;
/**
 * @}
 */

/*****************************************************************************
 * Functions
 *****************************************************************************/

/**
 * @brief  This function creates a queue.
 * @param[in] queue_len is the maximum number of items that the queue can contain.
 * @param[in] item_size is the number of bytes each item in the queue will require.
 * @param[in] buffer is the queue space. It must be allocated at the word aligned address,
 * and must also include the queue ctrl block size, which is sizeof(airo_cqueue_t).
 * @return
 * the queue handle if the queue is successfully created.\n
 * NULL if the queue creation fails.
 */
void *airo_cqueue_create(uint32_t queue_len, uint32_t item_size, void *buffer);

/**
 * @brief  This function sends a message to the queue.
 * @param[in] qHandle is the handle to the queue on which the item is to be posted.
 * @param[in] pItem is a pointer to the item that is to be put on the queue.
 * The size of the items the queue will hold was defined when the queue was created,
 * so this number of bytes is copied from pItem into the queue storage area.
 * @return
 * return true, the message is successfully sent to the queue.\n
 * return false, the message failed to be sent to the queue.
 */
bool airo_cqueue_send(void *qHandle, const void *const pItem);

/**
 * @brief  This function receives a message from the queue.
 * @param[in] qHandle is the handle to the queue on which the item is to be posted.
 * @param[in] pUser_buffer is the buffer into which the received item will be copied.
 * @return
 * return true, a message is successfully received from the queue. \n
 * return false, a message failed to be received from the queue.
 */
bool airo_cqueue_receive(void *qHandle, void *const pUser_buffer);

/**
 * @brief  This function checks whether the queue is full.
 * @param[in] qHandle is the handle to the queue on which the item is to be posted.
 * @return
 * return true, the queue is full. \n
 * return false, the queue is not full.
 */
bool airo_cqueue_is_full(void *qHandle);

/**
 * @brief  This function checks whether the queue is empty.
 * @param[in] qHandle is the handle to the queue on which the item is to be posted.
 * @return
 * return true, the queue is empty. \n
 * return false, the queue is not empty.
 */
bool airo_cqueue_is_empty(void *qHandle);

/**
 * @brief  This function queries the message count in the queue. It is equal to the number of the messages waiting to be received in the queue.
 * @param[in] qHandle is the handle to the queue on which the item is to be posted.
 * @return
 * return the number of messages in the queue \n
 */
uint32_t airo_cqueue_message_waiting(void *qHandle);

#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
*/


#endif /* _AIRO_CQUEUE_H */
