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
#include "mips.h"
#include "types.h"
#include "dsp_para_cpd.h"
#if 0
#define ENDIAN_RVRS(A)                  (A)

AT_DRAM0_assign  CPD_AU_NVKEY_STATE AU_NvKey =
{
		// Compander common parameter
		ENDIAN_RVRS(0x01),			 /* ENABLE;		      		   	 		 		 */
		ENDIAN_RVRS(0x01),			 /* REVISION; 		   		   	 		 		 */
		ENDIAN_RVRS(0x0000),		 /* CPD_AU_MODE: b1: 0(Disable Audio INS, default), 1(Enable Audio INS) */
		                             /*              b0: 0(fullband, default), 1(multiband)					*/
		ENDIAN_RVRS(0x0060),		 /* CPD_AU_DELAY_SIZE;      		   			 */
		ENDIAN_RVRS(0x08000000),     /* CPD_PACKET_MAX;      		   	 			 */

		// AU_H(Multi-band compander or Full-band compnader)
		ENDIAN_RVRS(0x2000),		 /* AUH_alpha_release1_44K;      	   	 		*/
		ENDIAN_RVRS(0x0666),		 /* AUH_alpha_release2_44K;      	   	 		*/
		ENDIAN_RVRS(0x0CCD),		 /* AUH_alpha_release3_44K;      	   	 		*/
		ENDIAN_RVRS(0x2000),		 /* AUH_alpha_attack_44K;        	   	 		*/
		ENDIAN_RVRS(0x1DFC),		 /* AUH_alpha_release1_48K;      	   	 		*/
		ENDIAN_RVRS(0x05D5),		 /* AUH_alpha_release2_48K;      	   	 		*/
		ENDIAN_RVRS(0x0B95),		 /* AUH_alpha_release3_48K;      	   	 		*/
		ENDIAN_RVRS(0x1DFC),		 /* AUH_alpha_attack_48K;        	   	 		*/
		ENDIAN_RVRS(0x1DFC),		 /* AUH_alpha_release1_96K;      	   	 		*/
		ENDIAN_RVRS(0x05D5),		 /* AUH_alpha_release2_96K;      	   	 		*/
		ENDIAN_RVRS(0x0B95),		 /* AUH_alpha_release3_96K;      	   	 		*/
		ENDIAN_RVRS(0x1DFC),		 /* AUH_alpha_attack_96K;        	   	 		*/
		ENDIAN_RVRS(0x047F0000),	 /* AUH_CPD_THRD;      							*/
		ENDIAN_RVRS(0x00A40000),	 /* AUH_level_two;     							*/
		ENDIAN_RVRS(0x00520000),	 /* AUH_level_three;   							*/
		ENDIAN_RVRS(0x06E60000),	 /* AUH_out_max;       							*/
		ENDIAN_RVRS(0x000A),		 /* AUH_period;              		   			*/

		// AU_L(Multi-band compander)
		ENDIAN_RVRS(0x028F),		/* AUL_alpha_release1_44K;     		   			*/
		ENDIAN_RVRS(0x0666),		/* AUL_alpha_release2_44K;     		   	 		*/
		ENDIAN_RVRS(0x0CCD),		/* AUL_alpha_release3_44K;     		   	 		*/
		ENDIAN_RVRS(0x399A),		/* AUL_alpha_attack_44K;       		   	 		*/
		ENDIAN_RVRS(0x024D),		/* AUL_alpha_release1_48K;     		   			*/
		ENDIAN_RVRS(0x0594),		/* AUL_alpha_release2_48K;     		   	 		*/
		ENDIAN_RVRS(0x0A9C),		/* AUL_alpha_release3_48K;     		   	 		*/
		ENDIAN_RVRS(0x36C6),		/* AUL_alpha_attack_48K;       		   	 		*/
		ENDIAN_RVRS(0x047F0000),	/* AUL_CPD_THRD;      							*/
		ENDIAN_RVRS(0x00A40000),	/* AUL_level_two;     							*/
		ENDIAN_RVRS(0x00520000),	/* AUL_level_three;   							*/
		ENDIAN_RVRS(0x04800000),	/* AUL_out_max;       							*/
		ENDIAN_RVRS(0x000A),		/* AUL_period;              		   	 		*/

		// AU_F(Multi-band compander)
		ENDIAN_RVRS(0x2000),		 /* AUF_alpha_release1_44K;    		   	 		*/
		ENDIAN_RVRS(0x399A),		 /* AUF_alpha_attack_44K;      		   	 		*/
		ENDIAN_RVRS(0x1D28),		 /* AUF_alpha_release1_48K;    		   	 		*/
		ENDIAN_RVRS(0x36F0),		 /* AUF_alpha_attack_48K;      		   	 		*/
		ENDIAN_RVRS(0x40000000),     /* AUF_CPD_THRD;      							*/
		ENDIAN_RVRS(0x40000000),     /* AUF_out_max;       							*/

		// SET ins_pr_thrd = 2 and ins_pr.atten = 0 to supress +/- 1 PEQ color noise @ 24-bit
		ENDIAN_RVRS(0x00000080),     /* AUX_INS_thrd,  0x00000080(=2*2^6) hhhh      */
		ENDIAN_RVRS(0x00000000),     /* AUX_INS_atten, 0x00000000                   */
		ENDIAN_RVRS(0x2000),         /* AUX_INS_alpha, 0x2000                       */

		//DBB (Multi-band compander)
		ENDIAN_RVRS(0x10000000),	/* thrd; 							 		 	*/
		ENDIAN_RVRS(0x20000000),	/* max_gain; 							 		*/
		ENDIAN_RVRS(0x00000000),	/* band_sw: 0(48K), 1(44K), 2(96K)				*/
		ENDIAN_RVRS(0x00050000),	/* silence_th;      				 		 	*/

		//Filter coefficient (Reserve)
		ENDIAN_RVRS(-2077051), /* HP_COFEB24_3rd_48K[0]; 							 */
		ENDIAN_RVRS( 2097151), /* HP_COFEB24_3rd_48K[1]; 							 */
		ENDIAN_RVRS( 2057855), /* HP_COFEB24_3rd_48K[2]; 							 */
		ENDIAN_RVRS(-2077051), /* HP_COFEB24_3rd_48K[3]; 							 */
		ENDIAN_RVRS(8308203), /* HP_COFEB24_3rd_48K[4]; 							 */
		ENDIAN_RVRS(-8231420), /* HP_COFEB24_3rd_48K[5]; 							 */
		ENDIAN_RVRS(-2037872), /* HP_COFEB24_3rd_48K[6]; 							 */
		ENDIAN_RVRS( 1048576), /* HP_COFEB24_3rd_48K[7]; 							 */
		ENDIAN_RVRS( 1048576), /* HP_COFEB24_3rd_48K[8]; 							 */
		ENDIAN_RVRS(4075745), /* HP_COFEB24_3rd_48K[9]; 							 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_48K[10]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_48K[11]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_48K[12]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_48K[13]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_48K[14]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_48K[15]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_48K[16]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_48K[17]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_48K[18]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_48K[19]; 						 */

		ENDIAN_RVRS(-2054778), /* HP_COFEB24_4rd_48K[0]; 							 */
		ENDIAN_RVRS( 2097151), /* HP_COFEB24_4rd_48K[1]; 							 */
		ENDIAN_RVRS( 2013732), /* HP_COFEB24_4rd_48K[2]; 							 */
		ENDIAN_RVRS(-2054778), /* HP_COFEB24_4rd_48K[3]; 							 */
		ENDIAN_RVRS(8219113), /* HP_COFEB24_4rd_48K[4]; 							 */
		ENDIAN_RVRS(-8054930), /* HP_COFEB24_4rd_48K[5]; 							 */
		ENDIAN_RVRS(-2091219), /* HP_COFEB24_4rd_48K[6]; 							 */
		ENDIAN_RVRS( 2097151), /* HP_COFEB24_4rd_48K[7]; 							 */
		ENDIAN_RVRS( 2086006), /* HP_COFEB24_4rd_48K[8]; 							 */
		ENDIAN_RVRS(-2091219), /* HP_COFEB24_4rd_48K[9]; 							 */
		ENDIAN_RVRS(8364874), /* HP_COFEB24_4rd_48K[10]; 						 */
		ENDIAN_RVRS(-8344025), /* HP_COFEB24_4rd_48K[11]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_48K[12]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_48K[13]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_48K[14]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_48K[15]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_48K[16]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_48K[17]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_48K[18]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_48K[19]; 						 */

		ENDIAN_RVRS(-2075137), /* HP_COFEB24_3rd_44K[0]; 							 */
		ENDIAN_RVRS( 2097151), /* HP_COFEB24_3rd_44K[1]; 							 */
		ENDIAN_RVRS( 2054195), /* HP_COFEB24_3rd_44K[2]; 							 */
		ENDIAN_RVRS(-2075137), /* HP_COFEB24_3rd_44K[3]; 							 */
		ENDIAN_RVRS(8300547), /* HP_COFEB24_3rd_44K[4]; 							 */
		ENDIAN_RVRS(-8216782), /* HP_COFEB24_3rd_44K[5]; 							 */
		ENDIAN_RVRS(-2032694), /* HP_COFEB24_3rd_44K[6]; 							 */
		ENDIAN_RVRS( 1048576), /* HP_COFEB24_3rd_44K[7]; 							 */
		ENDIAN_RVRS( 1048576), /* HP_COFEB24_3rd_44K[8]; 							 */
		ENDIAN_RVRS(4065388), /* HP_COFEB24_3rd_44K[9]; 							 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_44K[10]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_44K[11]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_44K[12]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_44K[13]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_44K[14]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_44K[15]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_44K[16]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_44K[17]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_44K[18]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_3rd_44K[19]; 						 */

		ENDIAN_RVRS(-2051014), /* HP_COFEB24_4rd_44K[0]; 							 */
		ENDIAN_RVRS( 2097151), /* HP_COFEB24_4rd_44K[1]; 							 */
		ENDIAN_RVRS( 2006448), /* HP_COFEB24_4rd_44K[2]; 							 */
		ENDIAN_RVRS(-2051014), /* HP_COFEB24_4rd_44K[3]; 							 */
		ENDIAN_RVRS(8204058), /* HP_COFEB24_4rd_44K[4]; 							 */
		ENDIAN_RVRS(-8025791), /* HP_COFEB24_4rd_44K[5]; 							 */
		ENDIAN_RVRS(-2090573), /* HP_COFEB24_4rd_44K[6]; 							 */
		ENDIAN_RVRS( 2097151), /* HP_COFEB24_4rd_44K[7]; 							 */
		ENDIAN_RVRS( 2084854), /* HP_COFEB24_4rd_44K[8]; 							 */
		ENDIAN_RVRS(-2090573), /* HP_COFEB24_4rd_44K[9]; 							 */
		ENDIAN_RVRS(8362292), /* HP_COFEB24_4rd_44K[10]; 						 */
		ENDIAN_RVRS(-8339416), /* HP_COFEB24_4rd_44K[11]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_44K[12]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_44K[13]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_44K[14]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_44K[15]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_44K[16]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_44K[17]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_44K[18]; 						 */
		ENDIAN_RVRS(0x00000000), /* HP_COFEB24_4rd_44K[19]; 						 */
        //Reserve
        ENDIAN_RVRS(0x00000000),    /* RESERVE_0				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_1				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_2				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_3				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_4				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_5				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_6				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_7				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_8				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_9				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_10				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_11				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_12				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_13				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_14				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_15				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_16				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_17				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_18				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_19				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_20				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_21				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_22				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_23				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_24				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_25				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_26				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_27				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_28				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_29				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_30				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_31				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_32				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_33				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_34				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_35				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_36				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_37				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_38				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_39				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_40				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_41				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_42				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_43				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_44				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_45				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_46				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_47				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_48				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_49				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_50				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_51				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_52				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_53				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_54				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_55				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_56				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_57				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_58				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_59				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_60				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_61				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_62				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_63				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_64				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_65				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_66				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_67				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_68				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_69				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_70				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_71				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_72				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_73				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_74				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_75				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_76				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_77				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_78				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_79				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_80				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_81				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_82				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_83				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_84				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_85				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_86				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_87				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_88				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_89				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_90				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_91				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_92				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_93				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_94				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_95				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_96				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_97				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_98				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_99				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_100			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_101			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_102			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_103			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_104			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_105			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_106			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_107			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_108			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_109			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_110			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_111			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_112			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_113			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_114			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_115			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_116			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_117			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_118			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_119			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_120			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_121			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_122			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_123			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_124			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_125			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_126			          	*/
        ENDIAN_RVRS(0x00000000),    /* RESERVE_127			          	*/
};

AT_DRAM0_assign CPD_VO_NVKEY_STATE VO_NvKey =
{
		// Compander common parameter
		ENDIAN_RVRS(0x01),		 	/* ENABLE;    		 				 	*/
		ENDIAN_RVRS(0x01),		 	/* REVISION;  		 				 	*/
		ENDIAN_RVRS(0x0028),		/* CPD_VO_DELAY_SIZE; 	 		 	 	*/
		ENDIAN_RVRS(0x7F1B),		/* alpha_release;						*/
		ENDIAN_RVRS(0x3B6A),		/* alpha_attack;						*/
		ENDIAN_RVRS(87241523 ),		/* CPD_THRD1; 						  */
		ENDIAN_RVRS(87241523 ),		/* CPD_THRD2; 						  */
		ENDIAN_RVRS(87241523 ),		/* CPD_IN2; 						  */
		ENDIAN_RVRS(134217728),		/* out_max;							  */
		ENDIAN_RVRS(134217728),		/* CPD_PACKET_MAX;  		 		  */
		ENDIAN_RVRS(0x0005),		/* period;                              */
		ENDIAN_RVRS(0x000D), 		/* INS_thrd;                            */
		ENDIAN_RVRS(0x2879), 		/* INS_atten;                           */
		ENDIAN_RVRS(0x0CCD),  		/* INS_alpha_rx							*/
		ENDIAN_RVRS(0x70A4), 		/* INS_alpha_tx_vad                     */
		ENDIAN_RVRS(0x7C29),		/* INS_alpha_tx_novad                   */
		ENDIAN_RVRS(0x0000),		/* Offset_volume                   */
		//Reserve
        ENDIAN_RVRS(0x0000),    /* RESERVE_0				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_1				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_2				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_3				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_4				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_5				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_6				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_7				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_8				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_9				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_10				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_11				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_12				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_13				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_14				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_15				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_16				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_17				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_18				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_19				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_20				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_21				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_22				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_23				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_24				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_25				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_26				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_27				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_28				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_29				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_30				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_31				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_32				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_33				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_34				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_35				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_36				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_37				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_38				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_39				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_40				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_41				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_42				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_43				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_44				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_45				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_46				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_47				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_48				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_49				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_50				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_51				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_52				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_53				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_54				          */
        ENDIAN_RVRS(0x00000000),    /* RESERVE_55				          */
};
#endif
