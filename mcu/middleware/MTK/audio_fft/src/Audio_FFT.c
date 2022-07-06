/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

//#include "common.h"
#include "Audio_FFT.h"
#include "audio_log.h"
#include "exception_handler.h"
#include "FreeRTOS.h"
#include <math.h>
#include <string.h>
#define SINE_NOISE (312)
//#define Calc_Boundary

/*******************************************************************************
 * Type definition
 *******************************************************************************/
#define MAX_SAMPLE_NUM 256
//static Complex_Fixed rComData[MAX_SAMPLE_NUM] ={{0,0}};
//static const float HanningWindow[256];

/*******************************************************************************
 * Private Function
 *******************************************************************************/
static kal_uint32 CalSquareRS8(kal_int32 i2Real, kal_int32 i2Image);
extern void vPortFree(void*);
extern void *pvPortMalloc(size_t);
/*******************************************************************************
 * Public Function
 *******************************************************************************/
//256-pt FFT
/*----------------------------------------------------------------------------
 * CalFFT256
 *----------------------------------------------------------------------------
 * Cosim Test Cases - IRQ1 & IRQ2 interrupt interval test
 *----------------------------------------------------------------------------
 * Purpose: calculate 256-pt FFT
 *
 * Inputs:
 *   short *pDat0
 *   short *pDat1
 *   kal_uint16 u2DataStart    :the start idx of data
 * Outputs:
 * kal_uint32* u4FreqData
 * kal_uint32* u4MaxData
 *----------------------------------------------------------------------------
*/

void DIF_FFT(Complex *x, unsigned int Nu)
{
    unsigned int i, j, k, ip, I;
    unsigned int N, LE, LE1, Nv2;
    Complex Wn, W, t, temp;
    N = 1 << Nu;
    LE = N << 1;
    for (i = 1; i <= Nu; i++) // the butterfly part
    {
        LE >>= 1;
        LE1 = LE >> 1;
        Wn.real = 1.0;
        Wn.image = 0; // Wn(0)
        W.real = (float)cos(PI / LE1);
        W.image = (float) - sin(PI / LE1); // Step of Wn increment
        for (j = 1; j <= LE1; j++)
        {
            for (k = j; k <= N; k += LE)
            {
                I = k - 1; // index of upper part of butterfly
                ip = I + LE1; // index of lower part of butterfly
                t.real = x[I].real + x[ip].real;
                t.image = x[I].image + x[ip].image; // the output of butterfly upper part
                temp.real = x[I].real - x[ip].real;
                temp.image = x[I].image - x[ip].image; // the output of butterfly lower part
                x[ip].real = temp.real * Wn.real - temp.image * Wn.image; // lower part has to multiply with Wn(k)
                x[ip].image = temp.real * Wn.image + temp.image * Wn.real;
                x[I].real = t.real;
                x[I].image = t.image; // copy t to x[i] directly
            }
            temp.real = W.real * Wn.real - W.image * Wn.image; // Increment Wn(j) to Wn(j+LE)
            Wn.image = W.real * Wn.image + W.image * Wn.real;
            Wn.real = temp.real;
        }
    }
    Nv2 = N / 2;
    j = 1;
    for (i = 1; i <= N - 1; i++) // bit-reverse
    {
        if (i < j)
        {
            t.real = x[j - 1].real;
            x[j - 1].real = x[i - 1].real;
            x[i - 1].real = t.real;
            t.image = x[j - 1].image;
            x[j - 1].image = x[i - 1].image;
            x[i - 1].image = t.image;
        }
        k = Nv2;
        while (k < j)
        {
            j -= k;
            k /= 2;
        }
        j += k;
    }
}

kal_bool ApplyFFT256(int16_t *pData, kal_uint16 u2DataStart, kal_uint32 *u4FreqData, kal_uint32 *u4MaxData, UINT32 dSR)
{
    kal_uint32 i, u4TmpData;
    //u2IdxStart: skip first 5 frequencies 0~312.5Hz
    kal_uint16 u2IdxData, u2IdxStart = 5;
    int i4AvgData = 0;
    float dFreq_Idx = (float)dSR / 256; //16000/256
    Complex *rComData = (Complex*)pvPortMalloc(sizeof(Complex)*MAX_SAMPLE_NUM);
    if(rComData == NULL) {
        AUD_LOG_E("ApplyFFT256 pvPortMalloc failed, assert\t,", 0);
        configASSERT(0);
        return 1;
    }

    bzero(rComData,sizeof(Complex)*MAX_SAMPLE_NUM);
    //1. calculate average value
    for (i = 0; i < 256; i++)
    {
        i4AvgData = i4AvgData + pData[u2DataStart + i];
    }
    i4AvgData = i4AvgData >> 8;
    //2. apply Hanning window
    for (i = 0; i < 256; i++)
    {
        rComData[i].real = (float)(pData[u2DataStart + i] - i4AvgData);
        rComData[i].image = 0.0;
    }

    //3. do 256-pt FFT
    DIF_FFT(rComData, 8);
    //4. calculate magnatude
    kal_uint32 u4MagData[256];//remove from local because of stack size issue
    for (i = 0; i < 128; i++)
    {
        //Avoid overflow calculate the square after right shift 8 bit
        u4MagData[i] = CalSquareRS8((kal_int32)round(rComData[i].real), (kal_int32)round(rComData[i].image));
    }
    vPortFree(rComData);
    //5. calculate the frequency
    u2IdxData = u2IdxStart;
    u4MaxData[0] = u4MagData[u2IdxStart];
    for (i = u2IdxStart + 1; i < 128; i++)
    {
        u4TmpData = u4MagData[i];
        if (u4TmpData > u4MaxData[0])
        {
            if(i*dFreq_Idx == SINE_NOISE)
                continue;

            u2IdxData = i;
            u4MaxData[0] = u4TmpData;
        }
    }
    u4FreqData[0] = u2IdxData * dFreq_Idx;
    return 0;
}


kal_bool FreqCheck(kal_uint32 u4TargetFreq, kal_uint32 u4FreqData)
{
    double dDiviRange = 0.1;
    kal_uint32 u4LboundData, u4UboundData;

    u4LboundData = u4TargetFreq * (1 - dDiviRange);
    u4UboundData = u4TargetFreq * (1 + dDiviRange);
    return ((u4FreqData < u4UboundData) && (u4LboundData < u4FreqData)) ? 1:0;
}

kal_bool MagnitudeCheck(kal_uint32 u4LboundMag, kal_uint32 u4UboundMag, kal_uint32 *u4MagData)
{
    kal_bool bResult = true;
    float fTHD2 = 0.0, fTHD3 = 0.0;
#ifdef Calc_Boundary
    AUD_LOG_I("Magnitude\t %d\t,", 1, u4MagData);
#else
    //Main magnitude
    if ((u4LboundMag < u4MagData[0]) && (u4MagData[0] < u4UboundMag))
    {
        AUD_LOG_I("Main Magnitude Pass!! %d < %d < %d\n", 3, u4LboundMag, u4MagData[0], u4UboundMag);
    }
    else
    {
        AUD_LOG_I("Main Magnitude Fail!! %d X %d X %d !!!\n", 3, u4LboundMag, u4MagData[0], u4UboundMag);
        bResult = false;
    }
    fTHD2 = (u4MagData[1] == 0) ? 100000 : ((float)u4MagData[0] / u4MagData[1]);
    fTHD3 = (u4MagData[2] == 0) ? 100000 : ((float)u4MagData[0] / u4MagData[2]);
    if (fTHD2 > 10000)
    {
        AUD_LOG_I("2nd Magnitude Pass!! THD2 = %d\n", 1, u4MagData[1]);
    }
    else
    {
        AUD_LOG_I("2nd Magnitude Fail!! THD2 = %d\n", 1, u4MagData[1]);
        bResult = false;
    }
    if (fTHD3 > 10000)
    {
        AUD_LOG_I("3rd Magnitude Pass!! THD3 = %d\n", 1, u4MagData[2]);
    }
    else
    {
        AUD_LOG_I("3rd Magnitude Fail!! THD3 = %d\n", 1, u4MagData[2]);
        bResult = false;
    }
#endif

    return bResult;

}

static kal_uint32 CalSquareRS8(kal_int32 i2Real, kal_int32 i2Image)
{
    kal_uint32 u4Sqr;
    kal_int32 i2RealRS, i2ImageRS;
    i2RealRS = i2Real >> 8;
    i2ImageRS = i2Image >> 8;
    u4Sqr = i2RealRS * i2RealRS + i2ImageRS * i2ImageRS;
    return u4Sqr;
}

