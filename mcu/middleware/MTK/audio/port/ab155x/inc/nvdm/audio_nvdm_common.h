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
#ifndef __AUDIO_NVDM_COMMON_H__
#define __AUDIO_NVDM_COMMON_H__

#define CM4_DSP_FEATURE

#include "types.h"

#ifdef CM4_DSP_FEATURE
	#include "audio_nvdm_coef.h"
#endif

#ifndef WIN32
    #include "nvdm.h"
#endif

#define DSP0_FEATURE_MAX_NUM		0x70
#define DSP0_NVDM_ITEM_MAX			256

#define NAT_NVDM_ITEM_MAX			16
#define NAT_Data_Begin_Ptr			NAT_NVDM_ITEM_MAX * sizeof(nat_nvdm_info_t)
#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
#define NAT_MEM_Size                (1024*3+512)
#else
#define NAT_MEM_Size                (1024*3)
#endif

typedef struct {					/* the Nvdm Allocation Table(NAT) structure */
	uint8_t		*base_addr;			/* The storage base address of NAT is allocated to SYSRAM. */
	uint16_t    total_size;			/* The total size of NAT storage data */
	uint16_t	used_size_in_byte;	/* The used_size_in_byte is the used size of NAT storage data */
	uint8_t		nvdm_num;			/* The number of nvdm existing item on the NAT */
} sysram_info_t;

typedef struct {
	uint16_t	nvdm_id;			/* NVDM ID for NAT */
	uint16_t	length;				/* Length of nvdm data */
	uint16_t	chksum; 			/* checksum of NVDM data */
	uint16_t	offset;				/* nvdm offset address based on the SYSRAM base_addr address */
} nat_nvdm_info_t;

typedef struct {
	uint16_t	nvdm_id;			/* NVDM ID for FLASH/SRAM memory */
	uint32_t	length;				/* length of nvdm data */
    const void	*mem_pt;			/* FLASH or SRAM memory pointer stores the nvdm data */
} mem_nvdm_info_t;

typedef struct {
    uint8_t		feature_type;					/* DSP feature(ex: DRC, EC_NR, PEQ, ..) */
	uint16_t	nvkey_id[NAT_NVDM_ITEM_MAX];	/* DSP feature related NvKeyID item */
} dsp_feature_t;

typedef enum {
    c_flash_mode = 0x00,					/* FLASH memory read/write nvdm data from/to NAT */
    c_sram_mode  = 0x01,					/* SRAM  memory read/write nvdm data from/to NAT */
    c_dsp0_mode  = 0x02						/* DSP0  memory read/write nvdm data from/to NAT */
} nvdm_access_mode_t;

#ifdef WIN32
typedef enum {
    NVDM_DATA_ITEM_TYPE_RAW_DATA = 0x01,	/* Defines the display type with raw data. */
    NVDM_DATA_ITEM_TYPE_STRING   = 0x02,	/* Defines the display type with string. */
} nvdm_data_item_type_t;
#endif

typedef enum{
    ECNR_SINGLE_MIC  = 0,
    ECNR_DUAL_MIC    = 1,
    ECNR_UNKOWN_TYPE = 2,
} dsp_ecnr_alg_mic_type_t;

typedef enum {
#ifdef WIN32
	NVDM_STATUS_OK						=  0,	/* The operation was successful.                                                        */
	NVDM_STATUS_ERROR					= -1,	/* An unknown error occurred.                                                           */
	NVDM_STATUS_INCORRECT_CHECKSUM		= -2,	/* The NVDM found a checksum error when reading the data item.                          */
	NVDM_STATUS_INSUFFICIENT_SPACE		= -3,	/* No space is available in the flash.                                                  */
	NVDM_STATUS_ITEM_NOT_FOUND			= -4,	/* The data item wasn't found by the NVDM.                                              */
	NVDM_STATUS_INVALID_PARAMETER		= -5,	/* The user parameter is invalid.                                                       */
#endif
	NVDM_STATUS_NAT_OK					=   0,  /* The operation was successful.                                                        */
	NVDM_STATUS_NAT_INCORRECT_CHECKSUM	= -11,	/* The NVDM found a checksum error when flash/sram reads the NAT data item.   			*/
	NVDM_STATUS_NAT_ITEM_NOT_FOUND		= -12,	/* The data item wasn't found on the g_nvdm_default_table.                                */
	NVDM_STATUS_NAT_INVALID_LENGTH		= -13,	/* The length of nvdm updated data is not same as the ones of NAT existing NVDM data.	*/
	NVDM_STATUS_NAT_DATA_FULL			= -14,	/* No space is available to store NVDM data on NAT table.                               */
	NVDM_STATUS_NAT_ITEM_FULL			= -15 	/* The number of NVDM item is up to NAT_NVDM_ITEM_MAX.		                            */
} sysram_status_t;

// nvdm_data_VC.c for WIN32 only
#ifdef WIN32
sysram_status_t nvdm_write_data_item(const char *group_name, const char *data_item_name, nvdm_data_item_type_t type, const uint8_t *buffer, uint32_t length);
sysram_status_t nvdm_read_data_item(const char *group_name, const char *data_item_name, uint8_t  *buffer, uint32_t *length);
sysram_status_t nvdm_query_data_item_length(const char *group_name, const char *data_item_name, uint32_t *size);
#endif

/**
 * @addtogroup SYSRAM NVDM
 * @{
 * This section introduces the NVDM APIs including supported features, software architecture, details on how to use the NVDM, enums, structures, typedefs and functions
 *
 * @section NVDM_APIs_Usage_Chapter How to use the NVDM SYSRAM APIs
 *
 *  The audio parameter is named as "NVDM" and "NvKey" on the CM4 and DSP0 platform, respectively.
 *  The content of NVDM's id and data is the same as the NvKey's ones.
 *
 *  The Nvdm Allocation Table(NAT) stores the DSP feature related nvdm data described as follows.
 *    a. The NAT uses memory space size(=NAT_MEM_Size) on the SYSRAM.
 *    b. Each nvdm information will store on the NAT header, and the number of NVDM item is up to NAT_NVDM_ITEM_MAX.
 *	  c. According to the DSP feature, the nvdm related information(nvdm id, length, chksum and offset) is written header in sequence.
 *       The nvdm_id1 is the ID of first nvdm item. The nvdm's data is allocated to the offset address stored on offset1 variable.
 *       The physical SYSRAM address of nvdm iterm1 data is base_addr + offset1.
 *       The length1 and chksum1 of nvdm iterm1 header will record the length and check-sum of the fisrt nvdm data, respectively.
 *
 *                                         Nvdm Allocation Table(NAT)
 *                      base_addr  _______________________________________
 *               	     	      |nvdm_id1 | length1 | chksum1 | offset1 | nvdm iterm1
 *               		          |nvdm_id2 | length2 | chksum2 | offset2 | nvdm iterm2
 *               		          |			        .....			      |		...
 *               		          |nvdm_idN | lengthN | chksumN | offsetN | nvdm itermN
 *	NAT_Data_Begin_Ptr = offset1  |_______________________________________|
 *		                          |              NVDM1 data               |
 *	                     offset2  |---------------------------------------|
 *		                          |				 NVDM2 data				  |
 *		                          |---------------------------------------|
 *		                          |					.....   			  |
 *		                 offsetN  |---------------------------------------|
 *		                          |              NVDMN data               |
 *		                          |_______________________________________|
 *
 *
 *  - CM4 part initialize and setup NVDM procedure as follows. \n
 *  - Step 1. Call #audio_dsp0_nvdm_init() to create nvdm default value as nvdm_audio_coef_default.h and write to FLASH memory.
 *  - Step 2. Call #audio_nvdm_set_feature() to initialize the NAT @ SYSRAM according to the dsp feature list.
 *  - Step 3. Call #nvdm_get_default_info() to obtain the corresponding nvdm information such as nvdm ID, length and memory pointer.
 *  - Step 4. Call #nat_table_write_audio_nvdm_data() to write FLASH/SRAM nvdm data to the NAT @ SYSRAM.
 *
 *  - DSP0 part initialize NvKey procedure as follows. \n
 *  - Step 1. Call #dsp0_nvkey_init() to initialize the NAT @ SYSRAM.
 *  - Step 2. Call #nvkey_read_full_key() to read the nvdm data of NAT.

 *  - sample code:
 *    @code
 *
 *       #include "nvdm_common.h"
 *       #include "nvdm_audio_coef_default.h"
 *
 *       uint8_t        g_SYS_SRAM[NAT_MEM_Size];
 *       sysram_info_t	g_sysram_info;
 *
 *       const DSP_FEATURE_TYPE_LIST audio_feature_list[] =
 *       {
 *           FUNC_VC,
 *       	 FUNC_VP_CPD
 *       };
 *
 *		 const DSP_ALG_NVKEY_e array_of_nvkeys[] =
 *		 {
 *		 	 NVKEY_DSP_PARA_AEC_NR
 *		 };
 *
 *       // CM4 part
 *		 void CM4_application(void)
 *       {
 *          uint8_t				*syaram_baddr = g_SYS_SRAM;
 *          mem_nvdm_info_t		flash_nvdm;
 *
 *			// CM4 initialize the NAT and write the default nvdm data to flash memory.
 *       	audio_dsp0_nvdm_init();
 *
 *          // According to the FUNC_VC and FUNC_VP_CPD featue from the above audio_feature_list, which corresponds to the NvKey data(same as nvdm data) with
 *          // NVKEY_DSP_PARA_VC and NVKEY_DSP_PARA_PLC stored on the NAT.
 *       	audio_nvdm_set_feature(num_of_features, audio_feature_list);
 *
 *			// According to the array_of_nvkeys table, the related nvdm data is written from flash memory to the NAT.
 *			audio_nvdm_set_nvkeys(1, array_of_nvkeys);
 *
 *       }
 *
 *       // DSP0 part
 *		 void DSP0_application(void)
 *       {
 *          uint8_t *syaram_baddr = g_SYS_SRAM;
 *          uint8_t	 buff_vc[sizeof(DSP_PARA_VC_STRU_c)];
 *
 *			// DSP0 initialize the NAT.
 *	        dsp0_nvkey_init(syaram_baddr, NAT_MEM_Size);
 *
 *          // DSP0 reads the nvkey data of NAT
 *  		nvkey_read_full_key(NVKEY_DSP_PARA_VC, &buff_vc, sizeof(buff_vc));
 *
 *          // DSP0 write the nvkey data to NAT
 *         	nvkey_write_full_key(NVKEY_DSP_PARA_VC, buff_vc, sizeof(buff_vc));
 *       }
 *
 *    @endcode
 */

// audio_nvdm.c
/**
 * @brief     During the CM4 initialization, if the flash memory has no nvdm data, then this function needs to be used to
 *            create nvdm default value as nvdm_audio_coef_default.h and write to flash memory.
 *            This function returns after the data is completely written to the flash.
 *            User should check the return status to verify whether the data is written to flash successfully.
 *            Call #audio_dsp0_nvdm_init() only once during the initialization.
  * @return
 *                #NVDM_STATUS_OK						, if the operation completed successfully. \n
 *                #NVDM_STATUS_ERROR					, if an unknown error occurred. \n
 *                #NVDM_STATUS_INCORRECT_CHECKSUM		, if the checksum of data item is invalid. \n
 *                #NVDM_STATUS_INSUFFICIENT_SPACE		, if the storage space is not enough. \n
 *                #NVDM_STATUS_ITEM_NOT_FOUND			, if the data item is not found. \n
 *                #NVDM_STATUS_INVALID_PARAMETER		, if the parameter is invalid. \n
 *                #NVDM_STATUS_NAT_INCORRECT_CHECKSUM	, if the checksum of flash/sram data item is not correct. \n
 *                #NVDM_STATUS_NAT_ITEM_NOT_FOUND	    , if nvdm item wasn't found on the g_nvdm_default_table. \n
 *                #NVDM_STATUS_NAT_INVALID_LENGTH	    , if the length of nvdm updated data is not same as the ones of NAT existing NVDM data. \n
 *                #NVDM_STATUS_NAT_DATA_FULL			, if No space is available to store nvdm data on the NAT. \n
 *                #NVDM_STATUS_NAT_ITEM_FULL			, if the number of nvdm item is up to NAT_NVDM_ITEM_MAX. \n
 */
sysram_status_t audio_dsp0_nvdm_init(void);

/**
 * @brief     This function is used to setup the SNL table @ SYSRAM according to the featurerlist array pointer.
 *            After the scenario(feature) is initialized or changed, the DSP feature related nvdm data from flash memory writes to the NAT.
 * @param[in] num_of_features is the number of feature.
 * @param[in] featurerlist is DSP feature list.
 * @return
 *                Refer to the return of audio_dsp0_nvdm_init function.
 */
#ifdef CM4_DSP_FEATURE
sysram_status_t audio_nvdm_set_feature(uint16_t num_of_features, const DSP_FEATURE_TYPE_LIST *featurerlist);
#endif
/**
 * @brief     This function is used to write the related nvdm data of flash memory to the NAT according to the array_of_nvkeys table.
 * @param[in] num_of_nvkeys is the number of nvkey.
 * @param[in] array_of_nvkeys is NvKey array list.
 * @return
 *                Refer to the return of audio_dsp0_nvdm_init function.
 */
#ifdef CM4_DSP_FEATURE
sysram_status_t audio_nvdm_set_nvkeys(uint16_t num_of_nvkeys, const DSP_ALG_NVKEY_e *array_of_nvkeys);
#endif
/**
 * @brief     This function is used to read nvdm data item from FLASH memory. It includes nvdm_read_data_item function, whose group_name is fixed as "DSP0",
 *			  and id number transfer to string as data_item_name parameter input.
 * @param[in] id   is mapped to the data_item_name of nvdm_read_data_item.
 * @param[in] data is a pointer to the data item's content.
 * @param[in] length is the size of the data item's content.
 * @return
 *                Refer to the return of nvdm_read_data_item function.
 */
sysram_status_t flash_memory_read_nvdm_data(uint16_t id, uint8_t *data, uint32_t *length);

/**
 * @brief     This function is used to write or update nvdm data item to FLASH memory. It includes nvdm_write_data_item function, whose group_name is fixed as "DSP0",
 *			  and id number transfer to string as data_item_name parameter input.
 * @param[in] id   is mapped to the data_item_name of nvdm_read_data_item.
 * @param[in] length is the size of the data item's content.
 * @return
 *                Refer to the return of nvdm_write_data_item function.
 */
sysram_status_t flash_memory_write_nvdm_data(uint16_t id, uint8_t *data, uint32_t length);

/**
 * @brief     This function is used to query nvdm data item length. It includes nvdm_query_data_item_length function, whose group_name is fixed as "DSP0",
 *			  and id number transfer to string as data_item_name parameter input.
 * @param[in] id   is mapped to the data_item_name of nvdm_read_data_item.
 * @param[in] data is a pointer to the data item's content.
 * @param[in] length is the size of the data item's content.
 * @return
 *                Refer to the return of nvdm_query_data_item_length function.
 */
sysram_status_t flash_memory_query_nvdm_data_length(uint16_t id, uint32_t *length);

/**
 * @brief     This function initializes the NAT to enable the NVDM services.
 *            User should not call #nat_table_read_audio_nvdm_data() or #nat_table_write_audio_nvdm_data() before #audio_nvdm_init_sysram() is called.
 *            Call #audio_nvdm_init_sysram() only once during the initialization.
 * @param[in] syaram_baddr is the based address of the storage NAT on SYSRAM.
 * @param[in] total_size is the total size of the storage NAT on SYSRAM.
 * @return
 *                Refer to the return of audio_dsp0_nvdm_init function.
 */
sysram_status_t audio_nvdm_init_sysram(uint32_t sysram_baddr, uint16_t total_size);

/**
 * @brief     This function initializes the NAT whose all of nvdm data are cleared to zero.
 * @return
 *                Refer to the return of audio_dsp0_nvdm_init function.
 */
sysram_status_t audio_nvdm_reset_sysram(void);

/**
 * @brief     This function uses the 3rd parameter "get_id" as reference to obtain the corresponding nvdm information
 *            such as nvdm ID, nvdm length and nvdm memory pointer, which is stored to the 2nd parameter "mem_nvdm" structure.
 * @param[in] mem_nvdm   is a pointer of the mem_nvdm_info_t structure.
 * @param[in] get_id     is nvdm id, which employs nvdm_table table to obtain the corresponding nvdm information.
 * @return
 *                Refer to the return of audio_dsp0_nvdm_init function.
 */
sysram_status_t nvdm_get_default_info(mem_nvdm_info_t *mem_nvdm, uint16_t get_id);

/**
 * @brief     This function is used to read the nvdm data from FLASH/SRAM meory, which writes to the NAT.
 *            The source of nvdm data can be selected as FLASH or SRAM memory according to the 3rd parameter "mode".
 *            If there are existing nvdm data items on the NAT, the nvdm invalidates old values and replaces with new ones.
 * @param[in] mem_nvdm  is a pointer of the mem_nvdm_info_t structure.
 * @param[in] mode      is the selected selection of reading nvdm data.
 *
 *			  mode = c_flash_mode:
 *				        The flash_nvdm structure should use #nvdm_get_default_info() function to get the nvdm default parameter before #nat_table_write_audio_nvdm_data().
 *						This function can read the assigned nvdm data of FLASH memory, which writes to the NAT.
 *						sample code:
 *							mem_nvdm_info_t		flash_nvdm;
 *							nvdm_get_default_info(&flash_nvdm, NVKEY_DSP_PARA_PLC);
 *							status = nat_table_write_audio_nvdm_data(flash_nvdm, c_flash_mode);
 *
 *			  mode =  c_sram_mode:
 *						The parameters of sram_nvdm structure ,nvdm_id, length and mem_pt, should be set before #nat_table_write_audio_nvdm_data() is called.
 *						This function can read the assigned nvdm data of SRAM memory, which writes to the NAT.
 *						sample code:
 *							DSP_PARA_PLC_STRU	buff_plc;
 *							mem_nvdm_info_t		sram_nvdm;
 *							sram_nvdm.nvdm_id	= NVKEY_DSP_PARA_PLC;
 *							sram_nvdm.length	= sizeof(buff_plc);
 *							sram_nvdm.mem_pt	= &DSP_PARA_PLC_STRU_c;
 *							status = nat_table_write_audio_nvdm_data(sram_nvdm, c_sram_mode);
 * @return
 *                Refer to the return of audio_dsp0_nvdm_init function.
 */
sysram_status_t nat_table_write_audio_nvdm_data(mem_nvdm_info_t mem_nvdm, nvdm_access_mode_t mode);

/**
 * @brief     This function is used to read nvdm data from the NAT, and then write to FLASH or SRAM memory according to the 3rd parameter "mode".
 *            If there is no such nvdm item, it returns #NVDM_STATUS_ITEM_NOT_FOUND.
 * @param[in] mem_nvdm  is a pointer of the mem_nvdm_info_t structure.
 * @param[in] mode      is the selected destination of writting nvdm data.
 *
 *			  mode = c_flash_mode:
 *				        The flash_nvdm structure should use #nvdm_get_default_info() function to get the nvdm default parameter before #nat_table_read_audio_nvdm_data().
 *						This function can read the assigned nvdm data of NAT, which writes to FLASH memory.
 *						sample code:
 *							mem_nvdm_info_t		flash_nvdm;
 *							status = nvdm_get_default_info(&flash_nvdm, NVKEY_DSP_PARA_PLC);
 *							status = nat_table_read_audio_nvdm_data(flash_nvdm, c_flash_mode);
 *
 *			  mode =  c_sram_mode:
 *						The parameters of sram_nvdm structure ,nvdm_id, length and mem_pt, should be set before #nat_table_read_audio_nvdm_data() is called.
 *						This function can read the assigned nvdm data of NAT, which writes to the assigned address of SRAM memory.
 *						sample code:
 *							mem_nvdm_info_t		sram_nvdm;
 *							uint8_t				buff_ecnr[sizeof(DSP_PARA_AEC_NR_STRU)];
 *							sram_nvdm.nvdm_id	= NVKEY_DSP_PARA_AEC_NR;
 *							sram_nvdm.length	= sizeof(buff_ecnr);
 *							sram_nvdm.mem_pt	= buff_ecnr;
 *							status = nat_table_read_audio_nvdm_data(sram_nvdm, c_sram_mode);
 * @return
 *                Refer to the return of audio_dsp0_nvdm_init function.
 */
sysram_status_t nat_table_read_audio_nvdm_data(mem_nvdm_info_t mem_nvdm, nvdm_access_mode_t mode);

/**
 * @brief     This function is used to setup the NAT @ SYSRAM according to the dsp feature.
 *            When the scenario is changed, user should call #audio_nvdm_init_sysram() and #dsp_feature_nvdm_write_sysram() again sequentially.
 *            According to the new dsp feature, the NAT reloads and updates the nvdm data.
 * @param[in] sysram_info  is the status of NAT, such as a basedaddr of SYSRAM, used data size and the number of nvdm item.
 * @param[in] num_of_features is the number of feature.
 * @param[in] featurerlist is DSP feature list.
 * @return
 *                Refer to the return of audio_dsp0_nvdm_init function.
 */
#ifdef CM4_DSP_FEATURE
sysram_status_t dsp_feature_nvdm_write_sysram(sysram_info_t *sysram_info, uint16_t num_of_features, const DSP_FEATURE_TYPE_LIST *featurerlist);
#endif

// dsp0_nvkey.c
/**
 * @brief     This function setups the based address of NAT for DSP0.
 *            User should not call #nvkey_read_full_key() before #dsp0_nvkey_init() is called.
 *            Call #dsp0_nvkey_init() only once during the DSP0 initialization.
 * @param[in] syaram_baddr is the based address of the storage NAT on SYSRAM.
 * @param[in] total_size is the total size of the storage NAT on SYSRAM.
 * @return
 */
void dsp0_nvkey_init(uint32_t syaram_baddr, uint16_t total_size);

sysram_status_t nvkey_read_full_key(uint16_t key_id, void *ptr, uint16_t length);
sysram_status_t nvkey_write_full_key(uint16_t key_id, void *ptr, uint16_t length);
dsp_ecnr_alg_mic_type_t nat_table_get_dsp_ecnr_algorithm_type(uint8_t * dst_pt);

extern const mem_nvdm_info_t g_nvdm_default_table[];
extern const dsp_feature_t   g_dsp_feature_table[];
extern       sysram_info_t   g_sysram_info;
extern       dsp_ecnr_alg_mic_type_t gDspEcnrAlg_MicType;

#ifdef WIN32
extern uint8_t g_SYS_SRAM[];
#endif

#endif /*__AUDIO_NVDM_COMMON_H__*/
