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

#ifndef _BSP_EINT_KEY_COMMON_H_
#define _BSP_EINT_KEY_COMMON_H_

#define DEFAULT_DEBOUNCE_TIME_MSEC (6)

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    MASK,
    UNMASK,
} bsp_eint_key_mask_control_t;

 /** @brief Common GPIO driver interface. */
typedef struct bsp_eint_key_gpio_driver {
    bool (*init)(struct bsp_eint_key_gpio_driver *driver);
    bool (*enable)(struct bsp_eint_key_gpio_driver *driver);
    int (*is_active)(struct bsp_eint_key_gpio_driver *driver, bool *is_active);
    int (*set_debounce_time_msec)(struct bsp_eint_key_gpio_driver *driver, uint32_t debounce_time_msec);
    uint8_t key_data; /**< keydata define by user*/
#ifdef AIR_BSP_EINT_KEY_ACTIVE_LEVEL_CONFIGURABLE
    uint8_t active_level;
#endif
    bsp_eint_key_event_t key_state; /**< key state*/
} bsp_eint_key_gpio_driver_t;

 /** @brief Change the key state.
  *
  * @param [in] gpio_driver detected the status is changed
  **/
void bsp_eint_key_change_key_state(bsp_eint_key_gpio_driver_t *gpio_driver);

 /** @brief The function to register the gpio driver instance
  *
  * @param [in] new_driver will be registered to bsp_eint_key.
  * @return >=0 the number of the drivers registered.
  * @return <0 failed to register the driver.
  **/
int bsp_eint_key_register_gpio_driver(bsp_eint_key_gpio_driver_t *new_driver);

#ifdef __cplusplus
}
#endif

#endif /* _BSP_EINT_KEY_COMMON_H_ */

