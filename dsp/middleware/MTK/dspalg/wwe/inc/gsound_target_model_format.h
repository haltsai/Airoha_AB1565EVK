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

typedef enum {
  /**
   * An mmap of this type is Read-Only data
   */
  GSOUND_HOTWORD_MMAP_TEXT,
  /*
   * An mmap of this type is preinitialized Read-Write data
   */
  GSOUND_HOTWORD_MMAP_DATA,
  /*
   * An mmap of this type is uninitialized Read-Write data
   */
  GSOUND_HOTWORD_MMAP_BSS
} GSoundHotwordMmapType;
/**
 * This is a representation of the model stored in flash. It consists of a header 
 * followed by a byte array for each mmap. The header specifies any padding between
 * the mmaps.
 */
#if 0
hw_model {
  GSoundHotwordModelHeader model_header;
  uint8 model_data[];
}
#endif
typedef struct {
  /**
   * RO/RW/RW uninitialized memory. This is 32-bits big endian
   */
  GSoundHotwordMmapType type;
  /**
   * Bytes from the beginning of the model to start of mmap section
   */
  uint32_t offset_bytes;
  /*
   * Number of bytes this memory occupies. For the TEXT and DATA types this
   * length indicates the size of initialized data. For the BSS type this length
   * indicates the memory that must be allocated.
   */
  uint32_t length_bytes;
} GSoundHotwordMmapHeader;

/**
 * This is a representation of the header stored in flash. Assume no padding in
 * the structure. All fields are 32-bits stored in big-endian format.
 * This struct will be at the beginning of a hotword model OTA'd file. Use it to
 * parse and load the models on GSoundTargetSetModel
 */
typedef struct {
  /**
   * Version of the format for this header. This format is version 0
   */
  uint32_t header_version;
  /** Version of the compatible hotword library. Note: int not uint */
  int32_t library_version;
  /** Length of the architecture string */
  uint32_t architecture_length;
  /** Architecture string. Only the first architecture_length bytes are valid */
  uint8_t architecture[16];
  /** Count of valid mmap_headers */
  uint32_t mmap_count;
  /** Array of headers for parsing the mmaps for a model */
  GSoundHotwordMmapHeader mmap_headers[2];
} GSoundHotwordModelHeader;

/**
 * Each mmap is a simple byte array. For a BSS mmap the array will be empty and the
 * length should be allocated in RAM when the model is loaded. 
 */
#if 0
struct {
  uint8 data[length_bytes];
} GSoundHotwordMmap;
#endif

#define GSOUND_HOTWORD_RW_BASE(hotwordbase)\
((((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[0].offset_bytes & 0x000000FF) << 24|\
(((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[0].offset_bytes & 0x0000FF00) << 8 |\
(((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[0].offset_bytes & 0x00FF0000) >> 8 |\
(((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[0].offset_bytes & 0xFF000000) >>24)\
+(uint32_t)hotwordbase

#define GSOUND_HOTWORD_RW_LENGTH(hotwordbase)\
((((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[0].length_bytes & 0x000000FF) << 24|\
(((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[0].length_bytes & 0x0000FF00) << 8 |\
(((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[0].length_bytes & 0x00FF0000) >> 8 |\
(((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[0].length_bytes & 0xFF000000) >>24)

#define GSOUND_HOTWORD_RO_BASE(hotwordbase)\
((((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[1].offset_bytes & 0x000000FF) << 24|\
(((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[1].offset_bytes & 0x0000FF00) << 8 |\
(((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[1].offset_bytes & 0x00FF0000) >> 8 |\
(((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[1].offset_bytes & 0xFF000000) >>24)\
+(uint32_t)hotwordbase

#define GSOUND_HOTWORD_RO_LENGTH(hotwordbase)\
((((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[1].length_bytes & 0x000000FF) << 24|\
(((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[1].length_bytes & 0x0000FF00) << 8 |\
(((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[1].length_bytes & 0x00FF0000) >> 8 |\
(((GSoundHotwordModelHeader *)hotwordbase)->mmap_headers[1].length_bytes & 0xFF000000) >>24)

