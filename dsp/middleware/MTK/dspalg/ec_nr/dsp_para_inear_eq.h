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
#ifndef _DSP_PARA_InEarEQ_H_
#define _DSP_PARA_InEarEQ_H_

//#include "types_inear.h"

/**
 * @brief Parameter for DSP WB TX InEar EQ
 * @KeyID 0xE167
 */
 
#ifdef WIN32
	#pragma pack(1)
#endif 

typedef struct stru_dsp_ineareq_para_s
{
	U8 ENABLE;				            /**< @Value 0x01 @Desc 1 */
	U8 REVISION;				        /**< @Value 0x01 @Desc 1 */	
	U16 InEar_EQ_COF_0;					/**< @Value 0x03F6 @Desc 1 */
	U16 InEar_EQ_COF_1;					/**< @Value 0x039B @Desc 1 */
	U16 InEar_EQ_COF_2;					/**< @Value 0x03A7 @Desc 1 */
	U16 InEar_EQ_COF_3;					/**< @Value 0x0398 @Desc 1 */
	U16 InEar_EQ_COF_4;					/**< @Value 0x0392 @Desc 1 */
	U16 InEar_EQ_COF_5;					/**< @Value 0x03B4 @Desc 1 */
	U16 InEar_EQ_COF_6;					/**< @Value 0x0406 @Desc 1 */
	U16 InEar_EQ_COF_7;					/**< @Value 0x048A @Desc 1 */
	U16 InEar_EQ_COF_8;					/**< @Value 0x0547 @Desc 1 */
	U16 InEar_EQ_COF_9;					/**< @Value 0x0641 @Desc 1 */
	U16 InEar_EQ_COF_10;				/**< @Value 0x0780 @Desc 1 */
	U16 InEar_EQ_COF_11;				/**< @Value 0x0902 @Desc 1 */
	U16 InEar_EQ_COF_12;				/**< @Value 0x0ABF @Desc 1 */
	U16 InEar_EQ_COF_13;				/**< @Value 0x0C9E @Desc 1 */
	U16 InEar_EQ_COF_14;				/**< @Value 0x0E74 @Desc 1 */
	U16 InEar_EQ_COF_15;				/**< @Value 0x1000 @Desc 1 */
	U16 InEar_EQ_COF_16;				/**< @Value 0x1108 @Desc 1 */
	U16 InEar_EQ_COF_17;				/**< @Value 0x1193 @Desc 1 */
	U16 InEar_EQ_COF_18;				/**< @Value 0x11CD @Desc 1 */
	U16 InEar_EQ_COF_19;				/**< @Value 0x11E9 @Desc 1 */
	U16 InEar_EQ_COF_20;				/**< @Value 0x121B @Desc 1 */
	U16 InEar_EQ_COF_21;				/**< @Value 0x1272 @Desc 1 */
	U16 InEar_EQ_COF_22;				/**< @Value 0x12E9 @Desc 1 */
	U16 InEar_EQ_COF_23;				/**< @Value 0x137A @Desc 1 */
	U16 InEar_EQ_COF_24;				/**< @Value 0x141F @Desc 1 */
	U16 InEar_EQ_COF_25;				/**< @Value 0x14D1 @Desc 1 */
	U16 InEar_EQ_COF_26;				/**< @Value 0x1589 @Desc 1 */
	U16 InEar_EQ_COF_27;				/**< @Value 0x1640 @Desc 1 */
	U16 InEar_EQ_COF_28;				/**< @Value 0x16F5 @Desc 1 */
	U16 InEar_EQ_COF_29;				/**< @Value 0x17A7 @Desc 1 */
	U16 InEar_EQ_COF_30;				/**< @Value 0x1855 @Desc 1 */
	U16 InEar_EQ_COF_31;				/**< @Value 0x18FF @Desc 1 */
	U16 InEar_EQ_COF_32;				/**< @Value 0x19A4 @Desc 1 */
	U16 InEar_EQ_COF_33;				/**< @Value 0x1A43 @Desc 1 */
	U16 InEar_EQ_COF_34;				/**< @Value 0x1ADC @Desc 1 */
	U16 InEar_EQ_COF_35;				/**< @Value 0x1B6C @Desc 1 */
	U16 InEar_EQ_COF_36;				/**< @Value 0x1BF4 @Desc 1 */
	U16 InEar_EQ_COF_37;				/**< @Value 0x1C72 @Desc 1 */
	U16 InEar_EQ_COF_38;				/**< @Value 0x1CE6 @Desc 1 */
	U16 InEar_EQ_COF_39;				/**< @Value 0x1D51 @Desc 1 */
	U16 InEar_EQ_COF_40;				/**< @Value 0x1DB2 @Desc 1 */
	U16 InEar_EQ_COF_41;				/**< @Value 0x1E0A @Desc 1 */
	U16 InEar_EQ_COF_42;				/**< @Value 0x1E59 @Desc 1 */
	U16 InEar_EQ_COF_43;				/**< @Value 0x1E9F @Desc 1 */
	U16 InEar_EQ_COF_44;				/**< @Value 0x1EDD @Desc 1 */
	U16 InEar_EQ_COF_45;				/**< @Value 0x1F12 @Desc 1 */
	U16 InEar_EQ_COF_46;				/**< @Value 0x1F41 @Desc 1 */
	U16 InEar_EQ_COF_47;				/**< @Value 0x1F68 @Desc 1 */
	U16 InEar_EQ_COF_48;				/**< @Value 0x1F88 @Desc 1 */
	U16 InEar_EQ_COF_49;				/**< @Value 0x1FA3 @Desc 1 */
	U16 InEar_EQ_COF_50;				/**< @Value 0x1FB9 @Desc 1 */
	U16 InEar_EQ_COF_51;				/**< @Value 0x1FCA @Desc 1 */
	U16 InEar_EQ_COF_52;				/**< @Value 0x1FD6 @Desc 1 */
	U16 InEar_EQ_COF_53;				/**< @Value 0x1FDF @Desc 1 */
	U16 InEar_EQ_COF_54;				/**< @Value 0x1FE6 @Desc 1 */
	U16 InEar_EQ_COF_55;				/**< @Value 0x1FEA @Desc 1 */
	U16 InEar_EQ_COF_56;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_57;				/**< @Value 0x1FEE @Desc 1 */
	U16 InEar_EQ_COF_58;				/**< @Value 0x1FEF @Desc 1 */
	U16 InEar_EQ_COF_59;				/**< @Value 0x1FEF @Desc 1 */
	U16 InEar_EQ_COF_60;				/**< @Value 0x1FEF @Desc 1 */
	U16 InEar_EQ_COF_61;				/**< @Value 0x1FEF @Desc 1 */
	U16 InEar_EQ_COF_62;				/**< @Value 0x1FEE @Desc 1 */
	U16 InEar_EQ_COF_63;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_64;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_65;				/**< @Value 0x1FEC @Desc 1 */
	U16 InEar_EQ_COF_66;				/**< @Value 0x1FEC @Desc 1 */
	U16 InEar_EQ_COF_67;				/**< @Value 0x1FEC @Desc 1 */
	U16 InEar_EQ_COF_68;				/**< @Value 0x1FEC @Desc 1 */
	U16 InEar_EQ_COF_69;				/**< @Value 0x1FEC @Desc 1 */
	U16 InEar_EQ_COF_70;				/**< @Value 0x1FEC @Desc 1 */
	U16 InEar_EQ_COF_71;				/**< @Value 0x1FEC @Desc 1 */
	U16 InEar_EQ_COF_72;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_73;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_74;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_75;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_76;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_77;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_78;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_79;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_80;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_81;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_82;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_83;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_84;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_85;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_86;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_87;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_88;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_89;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_90;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_91;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_92;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_93;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_94;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_95;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_96;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_97;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_98;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_99;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_100;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_101;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_102;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_103;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_104;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_105;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_106;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_107;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_108;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_109;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_110;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_111;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_112;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_113;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_114;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_115;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_116;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_117;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_118;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_119;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_120;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_121;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_122;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_123;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_124;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_125;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_126;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_127;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_128;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_129;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_130;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_131;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_132;				/**< @Value 0x1FEC @Desc 1 */
	U16 InEar_EQ_COF_133;				/**< @Value 0x1FEC @Desc 1 */
	U16 InEar_EQ_COF_134;				/**< @Value 0x1FEC @Desc 1 */
	U16 InEar_EQ_COF_135;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_136;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_137;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_138;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_139;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_140;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_141;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_142;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_143;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_144;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_145;				/**< @Value 0x1FEC @Desc 1 */
	U16 InEar_EQ_COF_146;				/**< @Value 0x1FEC @Desc 1 */
	U16 InEar_EQ_COF_147;				/**< @Value 0x1FEB @Desc 1 */
	U16 InEar_EQ_COF_148;				/**< @Value 0x1FEB @Desc 1 */
	U16 InEar_EQ_COF_149;				/**< @Value 0x1FEB @Desc 1 */
	U16 InEar_EQ_COF_150;				/**< @Value 0x1FEB @Desc 1 */
	U16 InEar_EQ_COF_151;				/**< @Value 0x1FEC @Desc 1 */
	U16 InEar_EQ_COF_152;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_153;				/**< @Value 0x1FEE @Desc 1 */
	U16 InEar_EQ_COF_154;				/**< @Value 0x1FEF @Desc 1 */
	U16 InEar_EQ_COF_155;				/**< @Value 0x1FF1 @Desc 1 */
	U16 InEar_EQ_COF_156;				/**< @Value 0x1FF2 @Desc 1 */
	U16 InEar_EQ_COF_157;				/**< @Value 0x1FF2 @Desc 1 */
	U16 InEar_EQ_COF_158;				/**< @Value 0x1FF2 @Desc 1 */
	U16 InEar_EQ_COF_159;				/**< @Value 0x1FF0 @Desc 1 */
	U16 InEar_EQ_COF_160;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_161;				/**< @Value 0x1FE8 @Desc 1 */
	U16 InEar_EQ_COF_162;				/**< @Value 0x1FE2 @Desc 1 */
	U16 InEar_EQ_COF_163;				/**< @Value 0x1FDD @Desc 1 */
	U16 InEar_EQ_COF_164;				/**< @Value 0x1FD9 @Desc 1 */
	U16 InEar_EQ_COF_165;				/**< @Value 0x1FD8 @Desc 1 */
	U16 InEar_EQ_COF_166;				/**< @Value 0x1FDA @Desc 1 */
	U16 InEar_EQ_COF_167;				/**< @Value 0x1FE0 @Desc 1 */
	U16 InEar_EQ_COF_168;				/**< @Value 0x1FED @Desc 1 */
	U16 InEar_EQ_COF_169;				/**< @Value 0x1FFF @Desc 1 */
	U16 InEar_EQ_COF_170;				/**< @Value 0x2014 @Desc 1 */
	U16 InEar_EQ_COF_171;				/**< @Value 0x2027 @Desc 1 */
	U16 InEar_EQ_COF_172;				/**< @Value 0x2036 @Desc 1 */
	U16 InEar_EQ_COF_173;				/**< @Value 0x203B @Desc 1 */
	U16 InEar_EQ_COF_174;				/**< @Value 0x2033 @Desc 1 */
	U16 InEar_EQ_COF_175;				/**< @Value 0x201A @Desc 1 */
	U16 InEar_EQ_COF_176;				/**< @Value 0x1FEC @Desc 1 */
	U16 InEar_EQ_COF_177;				/**< @Value 0x1FA7 @Desc 1 */
	U16 InEar_EQ_COF_178;				/**< @Value 0x1F4E @Desc 1 */
	U16 InEar_EQ_COF_179;				/**< @Value 0x1EE4 @Desc 1 */
	U16 InEar_EQ_COF_180;				/**< @Value 0x1E6F @Desc 1 */
	U16 InEar_EQ_COF_181;				/**< @Value 0x1DF2 @Desc 1 */
	U16 InEar_EQ_COF_182;				/**< @Value 0x1D72 @Desc 1 */
	U16 InEar_EQ_COF_183;				/**< @Value 0x1CF1 @Desc 1 */
	U16 InEar_EQ_COF_184;				/**< @Value 0x1C73 @Desc 1 */
	U16 InEar_EQ_COF_185;				/**< @Value 0x1BFB @Desc 1 */
	U16 InEar_EQ_COF_186;				/**< @Value 0x1B89 @Desc 1 */
	U16 InEar_EQ_COF_187;				/**< @Value 0x1B1D @Desc 1 */
	U16 InEar_EQ_COF_188;				/**< @Value 0x1AB7 @Desc 1 */
	U16 InEar_EQ_COF_189;				/**< @Value 0x1A58 @Desc 1 */
	U16 InEar_EQ_COF_190;				/**< @Value 0x19FE @Desc 1 */
	U16 InEar_EQ_COF_191;				/**< @Value 0x19A9 @Desc 1 */
	U16 InEar_EQ_COF_192;				/**< @Value 0x195B @Desc 1 */
	U16 InEar_EQ_COF_193;				/**< @Value 0x1912 @Desc 1 */
	U16 InEar_EQ_COF_194;				/**< @Value 0x18CB @Desc 1 */
	U16 InEar_EQ_COF_195;				/**< @Value 0x1882 @Desc 1 */
	U16 InEar_EQ_COF_196;				/**< @Value 0x1835 @Desc 1 */
	U16 InEar_EQ_COF_197;				/**< @Value 0x17E1 @Desc 1 */
	U16 InEar_EQ_COF_198;				/**< @Value 0x1781 @Desc 1 */
	U16 InEar_EQ_COF_199;				/**< @Value 0x1715 @Desc 1 */
	U16 InEar_EQ_COF_200;				/**< @Value 0x1699 @Desc 1 */
	U16 InEar_EQ_COF_201;				/**< @Value 0x160C @Desc 1 */
	U16 InEar_EQ_COF_202;				/**< @Value 0x1574 @Desc 1 */
	U16 InEar_EQ_COF_203;				/**< @Value 0x14D5 @Desc 1 */
	U16 InEar_EQ_COF_204;				/**< @Value 0x1435 @Desc 1 */
	U16 InEar_EQ_COF_205;				/**< @Value 0x1397 @Desc 1 */
	U16 InEar_EQ_COF_206;				/**< @Value 0x1300 @Desc 1 */
	U16 InEar_EQ_COF_207;				/**< @Value 0x1273 @Desc 1 */
	U16 InEar_EQ_COF_208;				/**< @Value 0x11F3 @Desc 1 */
	U16 InEar_EQ_COF_209;				/**< @Value 0x1180 @Desc 1 */
	U16 InEar_EQ_COF_210;				/**< @Value 0x1110 @Desc 1 */
	U16 InEar_EQ_COF_211;				/**< @Value 0x1098 @Desc 1 */
	U16 InEar_EQ_COF_212;				/**< @Value 0x100E @Desc 1 */
	U16 InEar_EQ_COF_213;				/**< @Value 0x0F6B @Desc 1 */
	U16 InEar_EQ_COF_214;				/**< @Value 0x0EA7 @Desc 1 */
	U16 InEar_EQ_COF_215;				/**< @Value 0x0DBF @Desc 1 */
	U16 InEar_EQ_COF_216;				/**< @Value 0x0CB3 @Desc 1 */
	U16 InEar_EQ_COF_217;				/**< @Value 0x0B89 @Desc 1 */
	U16 InEar_EQ_COF_218;				/**< @Value 0x0A56 @Desc 1 */
	U16 InEar_EQ_COF_219;				/**< @Value 0x092C @Desc 1 */
	U16 InEar_EQ_COF_220;				/**< @Value 0x0818 @Desc 1 */
	U16 InEar_EQ_COF_221;				/**< @Value 0x0722 @Desc 1 */
	U16 InEar_EQ_COF_222;				/**< @Value 0x064F @Desc 1 */
	U16 InEar_EQ_COF_223;				/**< @Value 0x059D @Desc 1 */
	U16 InEar_EQ_COF_224;				/**< @Value 0x050E @Desc 1 */
	U16 InEar_EQ_COF_225;				/**< @Value 0x049F @Desc 1 */
	U16 InEar_EQ_COF_226;				/**< @Value 0x0449 @Desc 1 */
	U16 InEar_EQ_COF_227;				/**< @Value 0x0407 @Desc 1 */
	U16 InEar_EQ_COF_228;				/**< @Value 0x03D7 @Desc 1 */
	U16 InEar_EQ_COF_229;				/**< @Value 0x03B5 @Desc 1 */
	U16 InEar_EQ_COF_230;				/**< @Value 0x03A0 @Desc 1 */
	U16 InEar_EQ_COF_231;				/**< @Value 0x0395 @Desc 1 */
	U16 InEar_EQ_COF_232;				/**< @Value 0x0395 @Desc 1 */
	U16 InEar_EQ_COF_233;				/**< @Value 0x039E @Desc 1 */
	U16 InEar_EQ_COF_234;				/**< @Value 0x03B0 @Desc 1 */
	U16 InEar_EQ_COF_235;				/**< @Value 0x03CB @Desc 1 */
	U16 InEar_EQ_COF_236;				/**< @Value 0x03EF @Desc 1 */
	U16 InEar_EQ_COF_237;				/**< @Value 0x041D @Desc 1 */
	U16 InEar_EQ_COF_238;				/**< @Value 0x0454 @Desc 1 */
	U16 InEar_EQ_COF_239;				/**< @Value 0x0496 @Desc 1 */
	U16 InEar_EQ_COF_240;				/**< @Value 0x04E2 @Desc 1 */
	U16 InEar_EQ_COF_241;				/**< @Value 0x053B @Desc 1 */
	U16 InEar_EQ_COF_242;				/**< @Value 0x059F @Desc 1 */
	U16 InEar_EQ_COF_243;				/**< @Value 0x0611 @Desc 1 */
	U16 InEar_EQ_COF_244;				/**< @Value 0x0692 @Desc 1 */
	U16 InEar_EQ_COF_245;				/**< @Value 0x0721 @Desc 1 */
	U16 InEar_EQ_COF_246;				/**< @Value 0x07C0 @Desc 1 */
	U16 InEar_EQ_COF_247;				/**< @Value 0x086F @Desc 1 */
	U16 InEar_EQ_COF_248;				/**< @Value 0x092F @Desc 1 */
	U16 InEar_EQ_COF_249;				/**< @Value 0x09FF @Desc 1 */
	U16 InEar_EQ_COF_250;				/**< @Value 0x0AE0 @Desc 1 */
	U16 InEar_EQ_COF_251;				/**< @Value 0x0BCF @Desc 1 */
	U16 InEar_EQ_COF_252;				/**< @Value 0x0CCD @Desc 1 */
	U16 InEar_EQ_COF_253;				/**< @Value 0x0DD5 @Desc 1 */
	U16 InEar_EQ_COF_254;				/**< @Value 0x0EE6 @Desc 1 */
	U16 InEar_EQ_COF_255;				/**< @Value 0x0FFC @Desc 1 */
} PACKED DSP_PARA_InEarEQ_STRU;

#ifdef WIN32
	#pragma pack()
#endif

#endif /* _DSP_PARA_InEarEQ_H_ */
