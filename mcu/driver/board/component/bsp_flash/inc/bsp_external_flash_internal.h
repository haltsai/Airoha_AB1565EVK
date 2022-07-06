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

/* Includes ------------------------------------------------------------------*/
#ifndef _BSP_SPI_SERIAL_FLASH_PORT_H_
#define _BSP_SPI_SERIAL_FLASH_PORT_H_

#ifdef BSP_EXTERNAL_SERIAL_FLASH_ENABLED

//Quad program should be 256 byte one time
#define WRITE_BUFFER_SIZE     (0x100)
#define FLASH_DADA_MAX_LENGTH (0x1000)

#define FLASH_BUSY      (0x1)
#define FLASH_INIT      (2)
#define FLASH_NOT_INIT  (-1)

//only support 3bytes address
#define ADDRESS_MASK        (0x00FFFFFF)
#define ADDRESS_CHECK_MASK  (0xFF000000)

#define COMMAND_LENGTH    (0x8)

typedef enum {
    FLASH_STATUS_IDLE = 0,
    FLASH_STATUS_BUSY = 1,
} flash_status_t;

typedef struct {
    char *name;
    uint32_t jedec_id;
    uint32_t page_size;
    uint32_t n_pages;
    uint8_t frd_cmd;     //fast read
    uint8_t frd_delay;   //fast read delay
    uint8_t qfrd_cmd;    //quad read
    uint8_t qfrd_delay;  //quad read delay
    uint8_t wd_cmd;      //write
    uint8_t wd_delay;    //write delay
    uint8_t qfwd_cmd;    //quad write
    uint8_t qfwd_delay;  //quad write delay
    uint8_t rqbit_cmd;   //read qbit command
    uint8_t wqbit_cmd;   //write qbit commnad
    uint8_t qbit;        //Qbit position
    uint8_t offset;      //Delay cycles(includes command) for polling read
    uint8_t q_offset;    //Delay cycles(includes command) for DAM read
} flsh_device_info_t;


/* the next is command set defination, please make sure the  */


//W25Q64 & 25WP064A command is same
#define READ_SR          (0x5)
#define WRITE_SR         (0x1)
#define QE_ENABLE        (0x40)


#define WRITE_ENABLE     (0x6)
#define READ_RDID        (0x9F)
#define FAST_READ        (0x0B)
//#define QUAD_READ      (0xEB)  //don't use 1.4.4 format as SPI doesn't support
#define SPIQ_READ        (0x6B)  //should use 0x6b as it is 1.1.4 foramt 
#define SPI_WRITE        (0x02)
#define SPIQ_WRITE       (0x32)
#define SECTOR_4K_ERASE  (0x20)
#define BLOCK_32K_ERASE  (0x52)
#define BLOCK_64K_ERASE  (0xD8)
#define CHIP_ERASE       (0xC7)
#define EANBLE_RESET     (0x66)
#define RESET_DEVICE     (0x99)

// QE bit operation is difference
// W25Q64: S9(BIT1_SR2) is QE bit for W25Q64FW
#define READ_SR_2        (0x35)
#define WRITE_SR_2       (0x31)
#define QE_ENABLE_WB     (0x2)


#endif //_BSP_SPI_SERIAL_FLASH_PORT_H_
#endif //BSP_EXTERNAL_SERIAL_FLASH_ENABLED

