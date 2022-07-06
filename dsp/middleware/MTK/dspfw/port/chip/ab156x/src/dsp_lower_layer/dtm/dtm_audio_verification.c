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
 
#include "config.h"
#if (FEA_SUPP_DSP_AUDIO_VERIFICATION)
//#include "os.h"
//#include "rc.h"
#include "dsp_task.h"
//#include "os_memory.h"
#include "dsp_buffer.h"
#if (FEA_SUPP_DSP_VAD_DEMO)
#include "mDSP_VadDemo.h"
#endif
#include "drv_usb_cdc.h"
#include "usb_verify_interface.h"


/**
 *
 *  Definition
 *
 */
#define USB_INPUT_FRAME_SIZE    ((U32)(512))
#define USB_CBUF_SIZE           ((U32)(4096+USB_INPUT_FRAME_SIZE))


/**
 *
 *  Type Definition
 *
 */
 typedef struct USB_DUMP_BUF_s
{
    U16 wo;
    U16 ro;
    U16 count;
} USB_DUMP_BUF_t;


/**
 *
 *  Function Prototype
 *
 */
VOID DSP_FakeAudioProcess(VOID);
STATIC VOID DSP_AuVerInit(VOID);
STATIC VOID DSP_AuVerProc(VOID);
STATIC BOOL DSP_DataComparison(U8* Stream1, U8* Stream2, U32 Len);


/**
 *
 *  Variables
 *
 */
USB_DUMP_BUF_t UsbDumpCtrl;
U8 USB_IN_BUF[USB_INPUT_FRAME_SIZE];
U8 USB_CBUF[USB_CBUF_SIZE];
U8 USB_OUT_BUF[4096];


/**
 * DSP_FakeAudioProcess
 *
 * This function is a virtual entry designed for algorithm bit-true verification
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 *
 */
VOID DSP_FakeAudioProcess(VOID)
{
	DSP_AuVerInit();

	while (1)
	{
		#if (FEA_SUPP_DSP_VAD_DEMO)
		DSP_VadCheckProc();
		#else
		//if (DSP_UsbPureDataInput(USB_OUT_BUF, USB_INPUT_FRAME_SIZE))
        {
            DSP_AuVerProc();

            //DRV_USB_CDC_ACM1_Send(USB_OUT_BUF, USB_INPUT_FRAME_SIZE);
        }

        DSP_AuVerProc();

        vTaskSuspend(DSP_TASK_ID);
        portYIELD();
		#endif
	}
}


/**
 * DSP_AuVerInit
 *
 * This function is used to insert DSP init function
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 *
 */
VOID DSP_AuVerInit(VOID)
{
    #if (FEA_SUPP_DSP_VAD_DEMO)
    DSP_VadDemoInit();
    #endif

    DSP_UsbDataInit();
}


/**
 * DSP_AuVerProc
 *
 * This function is used to insert DSP processing function
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_AuVerProc(VOID)
{
	U32 Length;

	/* For USB I/O verification */
	if ((Length = DSP_UsbPureDataInput(USB_OUT_BUF, USB_INPUT_FRAME_SIZE)))
	{
		DRV_USB_CDC_ACM1_Send(USB_OUT_BUF, USB_INPUT_FRAME_SIZE);
	}
}


/**
 * DSP_DataComparison
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
BOOL DSP_DataComparison(U8* Stream1, U8* Stream2, U32 Len)
{
    U32 CheckNo = 0;

    while (CheckNo < Len)
    {
        if (Stream1[Len] != Stream2[Len])
        {
            return FALSE;
        }
        CheckNo++;
    }

    return TRUE;
}


#endif
