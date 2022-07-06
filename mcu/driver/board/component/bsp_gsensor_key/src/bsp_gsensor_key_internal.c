/*
 ******************************************************************************
 * @file    gseneor_key_internal.c
 * @author  MEMS Software Solution Team
 * @brief   gseneor tap driver file
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <string.h>
#include "hal.h"


#ifdef GSENSOR_LIS2DW12_EANBLE

#include "lis2dw12_reg.h"
static lis2dw12_ctx_t g_dev_ctx;
#define BSP_SENSOR_TYPE "lisdw12"

#elif defined(GSENSOR_LIS2DS12_EANBLE)

#include "lis2ds12_reg.h"
static lis2ds12_ctx_t g_dev_ctx;
#define BSP_SENSOR_TYPE "lisds12"

#else
#error "Error: No sensor is defined!"
#endif

#include "bsp_gsensor_key_internal.h"

#if 0
void gsensor_goldem_sample(void)
{
    lis2dw12_ctx_t dev_ctx;

    uint8_t temp;


    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;

    gsensor_platform_i2c_init();
    gsensor_platform_eint_init();

    // single tap
#if 1
    temp = 0x4;
    lis2dw12_write_reg(&dev_ctx, 0x25, &temp, 1);

    temp = 0x40;
    lis2dw12_write_reg(&dev_ctx, 0x23, &temp, 1);

    temp = 0x09;
    lis2dw12_write_reg(&dev_ctx, 0x30, &temp, 1);

    temp = 0x09;
    lis2dw12_write_reg(&dev_ctx, 0x31, &temp, 1);

    temp = 0xe9;
    lis2dw12_write_reg(&dev_ctx, 0x32, &temp, 1);

    temp = 0x02;
    lis2dw12_write_reg(&dev_ctx, 0x33, &temp, 1);

    temp = 0x74;
    lis2dw12_write_reg(&dev_ctx, 0x20, &temp, 1);

    temp = 0x20;
    lis2dw12_write_reg(&dev_ctx, 0x3f, &temp, 1);
#endif

    //double tap + single tap. the single tap event will be reported before double tap
#if 0
    temp = 0x4;
    lis2dw12_write_reg(&dev_ctx, 0x25, &temp, 1);

    temp = 0x48;
    lis2dw12_write_reg(&dev_ctx, 0x23, &temp, 1);

    temp = 0x0c;
    lis2dw12_write_reg(&dev_ctx, 0x30, &temp, 1);

    temp = 0x0c;
    lis2dw12_write_reg(&dev_ctx, 0x31, &temp, 1);

    temp = 0xec;
    lis2dw12_write_reg(&dev_ctx, 0x32, &temp, 1);

    temp = 0x7f;
    lis2dw12_write_reg(&dev_ctx, 0x33, &temp, 1);

    temp = 0x80;
    lis2dw12_write_reg(&dev_ctx, 0x34, &temp, 1);

    temp = 0x74;
    lis2dw12_write_reg(&dev_ctx, 0x20, &temp, 1);
    hal_gpt_delay_us(3);

    temp = 0x20;
    lis2dw12_write_reg(&dev_ctx, 0x3f, &temp, 1);
#endif

    hal_eint_unmask(HAL_EINT_NUMBER_9);

    while (1);



}

/* Main Example --------------------------------------------------------------*/
void gsenosr_key_test(void)
{

    lis2dw12_ctx_t dev_ctx;
    lis2dw12_ctrl4_int1_pad_ctrl_t pad_ctrl;
    uint8_t  id_check;

    dev_ctx.write_reg = platform_write;
    dev_ctx.read_reg = platform_read;

    gsensor_platform_i2c_init();
    gsensor_platform_eint_init();

    //check Gsensor ID
    id_check = 0;
    lis2dw12_device_id_get(&dev_ctx, &id_check);

    if (id_check != LIS2DW12_ID) {
        log_hal_msgid_info("check fail! ID: 0x%x\r\n",1, id_check);
    } else {
        log_hal_msgid_info("check done! ID: 0x%x\r\n",1, id_check);
    }


    lis2dw12_full_scale_set(&dev_ctx, LIS2DW12_2g);
    lis2dw12_power_mode_set(&dev_ctx, LIS2DW12_HIGH_PERFORMANCE_LOW_NOISE);
    lis2dw12_data_rate_set(&dev_ctx, LIS2DW12_XL_ODR_400Hz);
    hal_gpt_delay_us(3);

    lis2dw12_tap_threshold_x_set(&dev_ctx, 0x9); //golden: single 0x9, double 0xc
    lis2dw12_tap_threshold_y_set(&dev_ctx, 0x9); //golden: single 0x9, double 0xc
    lis2dw12_tap_threshold_z_set(&dev_ctx, 0x9); //golden: single 0x9, double 0xc
    lis2dw12_tap_detection_on_x_set(&dev_ctx, 1);
    lis2dw12_tap_detection_on_y_set(&dev_ctx, 1);
    lis2dw12_tap_detection_on_z_set(&dev_ctx, 1);

    lis2dw12_tap_shock_set(&dev_ctx, 0x2);      //golden: single 0x2, double 0x3
    //lis2dw12_tap_quiet_set(&dev_ctx, 0x3);    //for double
    //lis2dw12_tap_dur_set(&dev_ctx, 0x7);      //for double

    //lis2dw12_tap_mode_set(&dev_ctx,LIS2DW12_BOTH_SINGLE_DOUBLE); // for double

    //pad_ctrl.int1_tap        = 1;             //for double
    pad_ctrl.int1_single_tap = 1;
    lis2dw12_pin_int1_route_set(&dev_ctx, &pad_ctrl);

    hal_eint_unmask(HAL_EINT_NUMBER_9);


    while (1);
}
#endif

#ifdef GSENSOR_LIS2DS12_EANBLE
void lis2ds12_init(bsp_gsensor_write_ptr  write_reg, bsp_gsensor_read_ptr read_reg, void* handle)
{
    lis2ds12_pin_int1_route_t pad_ctrl = {0};
    bsp_gsensor_t *gsensor = (bsp_gsensor_t*)handle;
    uint8_t  id_check;
    uint8_t  rst;
    uint8_t temp;

    if ((write_reg == NULL) || (read_reg == NULL) || (handle == NULL)) {
        log_hal_msgid_info("[%s] init error\r\n",1, __func__);
        return;
    }

    g_dev_ctx.write_reg = (lis2ds12_write_ptr)write_reg;
    g_dev_ctx.read_reg  = (lis2ds12_read_ptr)read_reg;
    g_dev_ctx.handle    = handle;


    //check Gsensor ID
    gsensor->i2c_addr = LIS2DS12_I2C_ADD_L;
    id_check = 0;
    lis2ds12_device_id_get(&g_dev_ctx, &id_check);

    if (id_check != LIS2DS12_ID) {
        log_hal_msgid_info("slave addr %d check fail!!! ID: 0x%x\r\n",2, gsensor->i2c_addr, id_check);

        gsensor->i2c_addr = LIS2DS12_I2C_ADD_H;
        id_check = 0;
        lis2ds12_device_id_get(&g_dev_ctx, &id_check);
        if (id_check != LIS2DS12_ID) {
            log_hal_msgid_info("slave addr %d check fail!!! ID: 0x%x\r\n",2, gsensor->i2c_addr, id_check);
            return;
        }
    }

    log_hal_msgid_info("slave addr %d check done!ID: 0x%x\r\n",2, gsensor->i2c_addr, id_check);

    /*
    * Restore default configuration
    */
    lis2ds12_reset_set(&g_dev_ctx, PROPERTY_ENABLE);
    do {
        lis2ds12_reset_get(&g_dev_ctx, &rst);
    } while (rst);

    /*
     * Set XL Output Data Rate
     */
    lis2ds12_xl_data_rate_set(&g_dev_ctx, LIS2DS12_XL_ODR_400Hz_HR);

    /*
    * Set 2g full XL scale
    */
    lis2ds12_xl_full_scale_set(&g_dev_ctx, LIS2DS12_2g);

    /*
    * Enable Tap detection on X, Y, Z
    */
    lis2ds12_tap_detection_on_z_set(&g_dev_ctx, PROPERTY_ENABLE);
    lis2ds12_tap_detection_on_y_set(&g_dev_ctx, PROPERTY_ENABLE);
    lis2ds12_tap_detection_on_x_set(&g_dev_ctx, PROPERTY_ENABLE);
    lis2ds12_4d_mode_set(&g_dev_ctx, PROPERTY_ENABLE);

    /*
    * Set Tap threshold to 01001b, therefore the tap threshold is
    * 562.5 mg (= 9 * FS_XL / 2 5 )
    */
    lis2ds12_tap_threshold_set(&g_dev_ctx, 0x09);

    /*
    * Configure Single Tap parameter
    */
    //lis2ds12_tap_dur_set(&g_dev_ctx, 0x0);
    lis2ds12_tap_quiet_set(&g_dev_ctx, 0x01);
    lis2ds12_tap_shock_set(&g_dev_ctx, 0x02);

    /*
    * Enable Single Tap detection only
    */
    lis2ds12_tap_mode_set(&g_dev_ctx, LIS2DS12_ONLY_SINGLE);


    pad_ctrl.int1_s_tap = 1;
    lis2ds12_pin_int1_route_set(&g_dev_ctx, pad_ctrl);



    lis2ds12_read_reg(&g_dev_ctx, LIS2DS12_CTRL4, &temp, 1);

    log_hal_msgid_info("temp=%x\r\n",1, temp);

}
#endif

#ifdef GSENSOR_LIS2DW12_EANBLE
void lis2dw12_init(bsp_gsensor_write_ptr  write_reg, bsp_gsensor_read_ptr read_reg, void* handle)
{
    lis2dw12_ctrl4_int1_pad_ctrl_t pad_ctrl = {0};
    bsp_gsensor_t *gsensor = (bsp_gsensor_t*)handle;
    uint8_t  id_check;

    g_dev_ctx.write_reg = (lis2dw12_write_ptr)write_reg;
    g_dev_ctx.read_reg  = (lis2dw12_read_ptr)read_reg;
    g_dev_ctx.handle    = gsensor->handle;

    if ((write_reg == NULL) || (read_reg == NULL) || (handle == NULL)) {
        log_hal_msgid_info("[%s] init error\r\n",1, __func__);
        return;
    }

    //check Gsensor ID
    gsensor->i2c_addr = LIS2DW12_I2C_ADD_L;
    id_check = 0;
    lis2dw12_device_id_get(&g_dev_ctx, &id_check);

    if (id_check != LIS2DW12_ID) {
        log_hal_msgid_info("slave addr %d check fail!!! ID: 0x%x\r\n",2, gsensor->i2c_addr, id_check);

        gsensor->i2c_addr = LIS2DW12_I2C_ADD_H;
        id_check = 0;
        lis2dw12_device_id_get(&g_dev_ctx, &id_check);
        if (id_check != LIS2DW12_ID) {
            log_hal_msgid_info("slave addr %d check fail!!! ID: 0x%x\r\n",2, gsensor->i2c_addr, id_check);
            return;
        }
    }

    log_hal_msgid_info("slave addr %d check done!ID: 0x%x\r\n",2, gsensor->i2c_addr, id_check);

    lis2dw12_full_scale_set(&g_dev_ctx, LIS2DW12_2g);
    lis2dw12_power_mode_set(&g_dev_ctx, LIS2DW12_HIGH_PERFORMANCE_LOW_NOISE);
    lis2dw12_data_rate_set(&g_dev_ctx, LIS2DW12_XL_ODR_400Hz);
    hal_gpt_delay_us(3);

    lis2dw12_tap_threshold_x_set(&g_dev_ctx, 0x9); //golden: single 0x9, double 0xc
    lis2dw12_tap_threshold_y_set(&g_dev_ctx, 0x9); //golden: single 0x9, double 0xc
    lis2dw12_tap_threshold_z_set(&g_dev_ctx, 0x9); //golden: single 0x9, double 0xc
    lis2dw12_tap_detection_on_x_set(&g_dev_ctx, 1);
    lis2dw12_tap_detection_on_y_set(&g_dev_ctx, 1);
    lis2dw12_tap_detection_on_z_set(&g_dev_ctx, 1);

    lis2dw12_tap_shock_set(&g_dev_ctx, 0x2);      //golden: single 0x2, double 0x3
    //lis2dw12_tap_quiet_set(&g_dev_ctx, 0x3);    //for double
    //lis2dw12_tap_dur_set(&g_dev_ctx, 0x7);      //for double

    //lis2dw12_tap_mode_set(&g_dev_ctx,LIS2DW12_BOTH_SINGLE_DOUBLE); // for double

    //pad_ctrl.int1_tap        = 1;             //for double
    pad_ctrl.int1_single_tap = 1;
    lis2dw12_pin_int1_route_set(&g_dev_ctx, &pad_ctrl);

    uint8_t temp;

    lis2dw12_read_reg(&g_dev_ctx, LIS2DW12_CTRL4_INT1_PAD_CTRL, &temp, 1);

    log_hal_msgid_info("temp=%x\r\n",1, temp);

}


#endif

bsp_gsensor_status_t* bsp_gsensor_get_status(void)
{
    static bsp_gsensor_status_t gsensor_status = {{0}};

#ifdef GSENSOR_LIS2DS12_EANBLE
    lis2ds12_all_sources_t all_source;
    lis2ds12_all_sources_get(&g_dev_ctx, &all_source);
    gsensor_status.tap_type.single_tap = all_source.reg.tap_src.single_tap;
    gsensor_status.tap_type.tap_level = BSP_GSENSOR_TAP_LEVEL_1;
#endif

#ifdef GSENSOR_LIS2DW12_EANBLE
    lis2dw12_status_t status;
    lis2dw12_status_reg_get(&g_dev_ctx, &status);

    gsensor_status.tap_type.single_tap = status.single_tap;
    gsensor_status.tap_type.double_tap = status.double_tap;
    gsensor_status.tap_type.tap_level = BSP_GSENSOR_TAP_LEVEL_2;
#endif
    return &gsensor_status;
}

void bsp_gsensor_init(bsp_gsensor_write_ptr write_reg, bsp_gsensor_read_ptr read_reg, void* handle)
{
#ifdef GSENSOR_LIS2DS12_EANBLE
    lis2ds12_init(write_reg, read_reg, handle);
#endif

#ifdef GSENSOR_LIS2DW12_EANBLE
    lis2dw12_init(write_reg, read_reg, handle);
#endif

}
const char* bsp_gsensor_get_type(void)
{
    return BSP_SENSOR_TYPE;
}
