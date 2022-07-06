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
 
#ifdef MTK_SWLA_ENABLE
#include "swla.h"
#include "FreeRTOS.h"
#include "memory_attribute.h"
#include "exception_handler.h"
#include "hal.h"
#include "verno.h"
#include "hal_gpt_internal.h"
#include <stdio.h>
#include <string.h>

/* !!!please disable sleep mode to profiling the SWLA overhead to avoid the issue of dwt not work after sleep wakeup
    set the valude of configCHECK_FOR_STACK_OVERFLOW marco to be zero in project's inc/freeRTOSConfig.h to disable sleep mode
    #define configCHECK_FOR_STACK_OVERFLOW	0
*/
//#define SWLA_OVERHEAD_MEASURE

#ifdef SWLA_OVERHEAD_MEASURE
#include "hal_dwt.h"
#endif /* SWLA_OVERHEAD_MEASURE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define SLA_STREAM_MODE 0xa0 /* swla data will be output by log service */
#define SLA_DUMP_MODE 0xb0  /* swla data will be dumped when exception occure, only support dump mode on IOT */
#define SLA_CORE_ID 0x0     /* only single core, cm4 core */

#define SLA_NODE_SIZE 0x8   /* SWLA node size, currently is 8byte, include [context, time stamp] */

#define MAIN_VER '1'
#define SUB_VER '0'        /* SWLA implementation on IOT */

/* Private variables ---------------------------------------------------------*/
const SA_IMAGE_HEADER1 gSLA_Header1 = {
    MAIN_VER,
    SUB_VER,
    sizeof(SA_IMAGE_HEADER1) + sizeof(SA_IMAGE_HEADER2),
    0, //? main part desc len
    0, //? addon desc len
    SLA_DUMP_MODE,
    SLA_CORE_ID,
    {0, 0}, //res[2]
    0, //? MDSys US
#ifdef __GNUC__
    PRODUCT_VERSION_STR,
    {0},//MTK_FW_VERSION,
#else
    {0x49,0x4E,0x56,0x41,0x4C,0x49,0x44}, //"INVALID"
    {0x49,0x4E,0x56,0x41,0x4C,0x49,0x44}, //"INVALID"
#endif
};

ATTR_ZIDATA_IN_TCM static SA_CONTROL_t xSLA_EnableFlag = SA_DISABLE;
ATTR_ZIDATA_IN_TCM static SA_NODE_t *pxSLA_Base;
ATTR_ZIDATA_IN_TCM static uint32_t xSLA_CurIndex;
ATTR_ZIDATA_IN_TCM static uint32_t xSLA_MaxIndex;
ATTR_ZIDATA_IN_TCM static uint32_t xSLA_WrapFlag;

/* Private functions ---------------------------------------------------------*/
extern void SLA_get_region(uint32_t *pxBase, uint32_t *pxLen);
static void SLA_Reinit(void)
{
    /* this function must to run under critical section. */
    /* get swla region location and length according to layout */
    uint32_t xBase, xLen;

    SLA_get_region(&xBase, &xLen);

    /* check the length of swla memory, ensure the buffer can record at least 32's data */
    if(xLen <= sizeof(SA_IMAGE_HEADER1) + sizeof(SA_IMAGE_HEADER2) + sizeof(SA_NODE_t) * 32) {
        /* the swla cannot be enabled, keep xSLA_EnableFlag is 0 */
        LOG_MSGID_E(common, "SWLA working memory is not enough to start SWLA.\r\n",0);
        xSLA_EnableFlag = SA_DISABLE;
        return;
    }

    xBase += sizeof(SA_IMAGE_HEADER1);
    *((uint32_t *)(xBase + 4)) = xBase + sizeof(SA_IMAGE_HEADER2); //reset curr position
    *((uint32_t *)(xBase + 12)) = 0; //reset wrap count

    xSLA_CurIndex = 0;
    xSLA_WrapFlag = 0;
}

void SLA_MemoryCallbackInit(void)
{
    /* get swla region location and length according to layout */
    uint32_t xBase, xLen;
    unsigned int *pxBase, *pxEnd;

    SLA_get_region(&xBase, &xLen);

    /* check the length of swla memory, ensure the buffer can record at least 128's data */
    if(xLen <= sizeof(SA_IMAGE_HEADER1) + sizeof(SA_IMAGE_HEADER2) + sizeof(SA_NODE_t) * 128) {
        /* the swla cannot be enabled, keep xSLA_EnableFlag is 0 */
        return;
    }

    pxBase = (unsigned int *)xBase;

#if (PRODUCT_VERSION == 2523)||(PRODUCT_VERSION == 7687)||(PRODUCT_VERSION == 7697)||(PRODUCT_VERSION == 7686)||(PRODUCT_VERSION == 7682)||(PRODUCT_VERSION == 5932)||(PRODUCT_VERSION == 7698)
    printf("####SWLA enabled[0x%08X,0x%08X]####\r\n", (unsigned int)xBase, (unsigned int)xLen);
#else
    exception_printf("####SWLA enabled[0x%08X,0x%08X]####\r\n", (unsigned int)xBase, (unsigned int)xLen);
#endif

    /* update SWLA header */
    xBase += sizeof(SA_IMAGE_HEADER1);
    *((uint32_t *)(xBase + 4)) = (xSLA_CurIndex == 0) ? ((uint32_t)(pxSLA_Base + xSLA_CurIndex)) : ((uint32_t)(pxSLA_Base + xSLA_CurIndex - 1)); // Curr Position
    *((uint32_t *)(xBase + 12)) = xSLA_WrapFlag; /* wrap count */

    /* SWLA buffer valid length */
    if (xSLA_WrapFlag) {
        pxEnd = (unsigned int *)(xBase - sizeof(SA_IMAGE_HEADER1) + xLen);
    } else {
        pxEnd = (unsigned int *)(pxSLA_Base + xSLA_CurIndex);
        *((uint32_t *)(xBase + 8)) = xSLA_CurIndex * sizeof(SA_NODE_t); /* raw data length */
    }

#if (PRODUCT_VERSION == 2523)||(PRODUCT_VERSION == 7687)||(PRODUCT_VERSION == 7697)||(PRODUCT_VERSION == 7686)||(PRODUCT_VERSION == 7682)||(PRODUCT_VERSION == 5932)||(PRODUCT_VERSION == 7698)
    unsigned int *pxCurrent;
    /* output swla region content */
    for (pxCurrent = pxBase; pxCurrent < pxEnd; pxCurrent += 4) {
        if (*(pxCurrent + 0) == 0 &&
                *(pxCurrent + 1) == 0 &&
                *(pxCurrent + 2) == 0 &&
                *(pxCurrent + 3) == 0) {
            continue;
        }

        platform_printf("0x%08x: %08x %08x %08x %08x\n\r",
               (unsigned int)pxCurrent,
               *(pxCurrent + 0),
               *(pxCurrent + 1),
               *(pxCurrent + 2),
               *(pxCurrent + 3));
    }
#else
    memory_region_type region_config;
    /* register exception region to exception flow to dump swla raw data */
    region_config.region_name = "swla";
    region_config.start_address = pxBase;
    region_config.end_address = (xSLA_EnableFlag == SA_DISABLE)? pxBase : pxEnd;
    region_config.is_dumped = 1;
    exception_register_regions(&region_config);
#endif
}

static void SLA_MemoryDumpInit(void)
{
    uint32_t ret;
    exception_config_type callback_config;

    /* register callback to exception flow */
    callback_config.init_cb = SLA_MemoryCallbackInit;
    callback_config.dump_cb = NULL;

    ret = exception_register_callbacks(&callback_config);
    if (!ret) {
        configASSERT(0);
    }
}

/* Public functions ---------------------------------------------------------*/

/**
 * @brief  swla enable
 * @param[in]  none.
 * @return none
 */
void SLA_Enable(void)
{
    /* get swla region location and length according to layout */
    uint32_t xBase, xLen;
    //uint32_t xSleepMode = 0;
    uint32_t xTimerStamp;

    SLA_get_region(&xBase, &xLen);

    /* check the length of swla memory, ensure the buffer can record at least the header (64B + 16B), and the 20's user data 
    ** that's total need at least 80 + 20 * 8 = 240Byte working buffer
    */
    if(xLen <= sizeof(SA_IMAGE_HEADER1) + sizeof(SA_IMAGE_HEADER2) + sizeof(SA_NODE_t) * 20) {
        /* the swla cannot be enabled, keep xSLA_EnableFlag is 0 */
        LOG_MSGID_I(common, "swla cannot enable because of the lack of working buffer[0x%x, 0x%x]\n",2, xBase, xLen);
        return;
    }

    /* copy SA_IMAGE_HEADER1 to the begin of the SWLA buffer */
    //memset((void *)xBase, 0x0, xLen);
    memcpy((void *)xBase, &gSLA_Header1, sizeof(SA_IMAGE_HEADER1));
    /* only copy the front 32byte of the firmware version */
    strncpy((char *)(((SA_IMAGE_HEADER1 *)xBase)->xFlavormName), (const char *)MTK_FW_VERSION, sizeof(((SA_IMAGE_HEADER1 *)0)->xFlavormName));
	(((SA_IMAGE_HEADER1 *)xBase)->xFlavormName)[31] = '\0';
	
    /* to config the SWLA global variable through the SWLA area in layout */
    xBase += sizeof(SA_IMAGE_HEADER1);
    *((uint32_t *)xBase) = xBase + sizeof(SA_IMAGE_HEADER2); // Start Position
    *((uint32_t *)(xBase + 4)) = xBase + sizeof(SA_IMAGE_HEADER2); // Curr Position
    *((uint32_t *)(xBase + 8)) = xLen - gSLA_Header1.xImageHeaderLen; //Raw data length
    *((uint32_t *)(xBase + 12)) = 0; //wrap count

    /* point to the raw data area */
    xBase += sizeof(SA_IMAGE_HEADER2);
    pxSLA_Base = (SA_NODE_t *)xBase;
    xSLA_MaxIndex = ((xLen - gSLA_Header1.xImageHeaderLen) / sizeof(SA_NODE_t)) - 1;
    //xSLA_CurIndex = 0;
    //xSLA_WrapFlag = 0; //global zi will be cleared by region init

    /* dummy read to make gpt is enabled */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &xTimerStamp);

    /* register callback in exception handling flow to dump SWLA region */
    SLA_MemoryDumpInit();

    /* SWLA enable done */
    xSLA_EnableFlag = SA_ENABLE;

#ifdef SWLA_OVERHEAD_MEASURE
    DWT_RESET_CYCLECOUNTER(xTimerStamp);
#endif /* SWLA_OVERHEAD_MEASURE */
}

/**
 * @brief  swla logging
 * @param[in]       *Context points to the input buffer, include swla label and action
 * @return none
 */
ATTR_TEXT_IN_TCM void SLA_RamLogging(uint32_t xContext)
{
    uint32_t xTimerStamp, xSavedMask;
#ifdef SWLA_OVERHEAD_MEASURE
    uint32_t xTimeStart, xTimeEnd;
    uint32_t xTimeGptStart, xTimeGptEnd, xOverheadUpdateFlag = 0;
    static uint32_t xTimeRamLogging = 0;
    DWT_BENCHMARK_START(xTimeStart);
#endif /* SWLA_OVERHEAD_MEASURE */
    hal_nvic_save_and_set_interrupt_mask(&xSavedMask);

    /* if SWLA is not enabled, do nothing */
    if(!xSLA_EnableFlag) {
        hal_nvic_restore_interrupt_mask(xSavedMask);
        return;
    }
    /* get time stamp */
#if ((PRODUCT_VERSION == 7687)|| (PRODUCT_VERSION == 7697))
    xTimerStamp = (*(volatile uint32_t *)(0x83050000 + 0x34)); //GPT2_CNT 32k
#elif (PRODUCT_VERSION == 2523) || (PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7682) || (PRODUCT_VERSION == 5932) || (PRODUCT_VERSION == 7698)
    xTimerStamp = GPT5->GPT_COUNT;
#elif (PRODUCT_VERSION == 1552)  || (PRODUCT_VERSION == 2552)
    xTimerStamp = GPT1->GPT_COUNT;
#elif (PRODUCT_VERSION == 3335)
    xTimerStamp = GPT2->GPT_COUNT;
#elif (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1568) || (PRODUCT_VERSION == 1565)
    #ifdef FPGA_ENV
    xTimerStamp = (GPT2->GPT_COUNT) / 10000;
    #else
    xTimerStamp = ((GPT_REGISTER_T *)GPT(HAL_GPT_MS_PORT))->GPT_COUNT;
    #endif
#else
    #error "not support chip, please notify swla owner to porting "
#endif
    pxSLA_Base[xSLA_CurIndex].xContext = xContext;
    pxSLA_Base[xSLA_CurIndex].xTimeStamp = xTimerStamp;

    if (xSLA_CurIndex == xSLA_MaxIndex) {
        xSLA_WrapFlag ++;
        xSLA_CurIndex = 0;
    } else {
        xSLA_CurIndex++;
    }

#ifdef SWLA_OVERHEAD_MEASURE
    DWT_BENCHMARK_END(xTimeEnd);
    if ((xTimeEnd - xTimeStart) > xTimeRamLogging) {
        xTimeRamLogging = xTimeEnd - xTimeStart;
        xOverheadUpdateFlag = 1;
    }
#endif /* SWLA_OVERHEAD_MEASURE */

    hal_nvic_restore_interrupt_mask(xSavedMask);

#ifdef SWLA_OVERHEAD_MEASURE
    if (xOverheadUpdateFlag) {
        xOverheadUpdateFlag = 0;
        DWT_BENCHMARK_PRINTF("##SLA_Ram", xTimeStart, xTimeEnd);
    }
#endif /* SWLA_OVERHEAD_MEASURE */
}

/**
 * @brief customer swla logging
 * @param[in]       *customLabel points to the input buffer
 * @param[in]       saAction swla operation, include start, stop and one-shot mode
 * @return none
 */
ATTR_TEXT_IN_TCM void SLA_CustomLogging(const char *pxCustomLabel, SA_ACTION_t xAction)
{
    uint32_t xSavedMask, xContext;
    uint8_t *pxCustomContext;

#ifdef SWLA_OVERHEAD_MEASURE
    uint32_t xTimeStart, xTimeEnd;
    uint32_t xTimeGptStart, xTimeGptEnd, xOverheadUpdateFlag = 0;
    static uint32_t xTimeCusLogging = 0;
#endif /* SWLA_OVERHEAD_MEASURE */

    hal_nvic_save_and_set_interrupt_mask(&xSavedMask);

#ifdef SWLA_OVERHEAD_MEASURE
    DWT_BENCHMARK_START(xTimeStart);
#endif /* SWLA_OVERHEAD_MEASURE */

    /* if SWLA is not enabled, do nothing */
    if(!xSLA_EnableFlag) {
        hal_nvic_restore_interrupt_mask(xSavedMask);
        return;
    }

    pxCustomContext = (uint8_t *)&xContext;

    /* check action */
    if ((xAction != SA_START) && (xAction != SA_STOP) && (xAction != SA_LABEL)) {
        LOG_MSGID_I(common, "[parameter error]invalid xAction:%d.\r\n", 1, (unsigned int)xAction);
        configASSERT(0);
    }

    /* update action */
    pxCustomContext[0] = (uint8_t)xAction;

    /* only support 3-characters for customer label */
    pxCustomContext[1] = (uint8_t)pxCustomLabel[0];
    pxCustomContext[2] = (uint8_t)pxCustomLabel[1];
    pxCustomContext[3] = (uint8_t)pxCustomLabel[2];

    /* write one record to SWLA buffer */
    SLA_RamLogging((uint32_t)xContext);

#ifdef SWLA_OVERHEAD_MEASURE
    DWT_BENCHMARK_END(xTimeEnd);
    if ((xTimeEnd - xTimeStart) > xTimeCusLogging) {
        xTimeCusLogging = xTimeEnd - xTimeStart;
        xOverheadUpdateFlag = 1;
    }
#endif /* SWLA_OVERHEAD_MEASURE */

    hal_nvic_restore_interrupt_mask(xSavedMask);

#ifdef SWLA_OVERHEAD_MEASURE
    if (xOverheadUpdateFlag) {
        xOverheadUpdateFlag = 0;
        DWT_BENCHMARK_PRINTF("##SLA_Cus", xTimeStart, xTimeEnd);
    }
#endif /* SWLA_OVERHEAD_MEASURE */
}

/**
 * @brief swla control
 * @param[in]  xOperation enable or disable swla
 * @return[out] ret return control ok or not
 */
void SLA_Control(SA_CONTROL_t xOperation)
{
    uint32_t xSavedMask;

    if((xOperation != SA_ENABLE) && (xOperation != SA_DISABLE)){
        LOG_MSGID_E(common, "[parameter error]invalid xOperation:%d.\r\n", 1, (unsigned int)xOperation);
        return;
    }else{
        LOG_MSGID_I(common, "receive swla xOperation:%d.\r\n", 1, (unsigned int)xOperation);
    }

    hal_nvic_save_and_set_interrupt_mask(&xSavedMask);
    xSLA_EnableFlag = xOperation;
    if(xOperation == SA_ENABLE){
        SLA_Reinit();
    }

    hal_nvic_restore_interrupt_mask(xSavedMask);
}


#ifdef MTK_SWLA_ENABLE_O2


#if (PRODUCT_VERSION == 1552) || (PRODUCT_VERSION == 2552) || (PRODUCT_VERSION == 2811)
volatile uint32_t UART_PORT[(hal_uart_port_t)3]={0xA00F0000,0xA0100000,0xA0110000};
#elif (PRODUCT_VERSION == 3335)
volatile uint32_t UART_PORT[(hal_uart_port_t)3]={0xA00B0000,0xA00C0000,0xA00D0000};
#elif (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565) || (PRODUCT_VERSION == 1568)
volatile uint32_t UART_PORT[(hal_uart_port_t)3]={0xA0030000,0xA3070000,0xA3080000};
#elif (PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7682)
volatile uint32_t UART_PORT[(hal_uart_port_t)3]={0xA00C0000,0xA00D0000,0xA00E0000};
#endif

void uart_debug_init(hal_uart_port_t port, uint32_t baudrate)
{
    uint32_t BaseAddr = UART_PORT[port];

    /*enable clock*/
#if (PRODUCT_VERSION == 1552) || (PRODUCT_VERSION == 2552) || (PRODUCT_VERSION == 2811)
    /*bit[20]-UART0, bit[11]-UART1, bit[12]-UART2*/
    *(volatile uint32_t*)(0xA2270320) |= 0x101800;
#elif (PRODUCT_VERSION == 3335)
    /*bit[16]-UART0, bit[0]-UART1, bit[1]-UART2*/
    *(volatile uint32_t*)(0xA2030B20) |= 0x10003;
#elif (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565) || (PRODUCT_VERSION == 1568)
    /*bit[16]-UART0, bit[0]-UART1, bit[1]-UART2*/
    *(volatile uint32_t*)(0xA2030B20) |= 0x10003;
#elif (PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7682)
    /*bit[20]-UART0, bit[9]-UART1, bit[10]-UART2*/
    *(volatile uint32_t*)(0xA2030B20) |= 0x100300;
#endif

    /*set baudrate*/
    *(volatile uint32_t*)(BaseAddr+0x8) = 0x1;

    if(baudrate == 115200){
        /*115200: 0x1- 0x6fe0- 0x16c*/
        *(volatile uint32_t*)(BaseAddr+0x38) = 0x6fe0;
        *(volatile uint32_t*)(BaseAddr+0x58) = 0x16c;
    }else if(baudrate == 921600){
        /*921600: 0x1- 0xd1b--0x20 */
        *(volatile uint32_t*)(BaseAddr+0x38) = 0xd1b;
        *(volatile uint32_t*)(BaseAddr+0x58) = 0x20;
    }else if(baudrate == 3000000){
        /*3M:     0x1- 0x307--0x16c */
        *(volatile uint32_t*)(BaseAddr+0x38) = 0x307;
        *(volatile uint32_t*)(BaseAddr+0x58) = 0x16C;
    }else {
        return;
    }
    /*set high speed*/
    *(volatile uint32_t*)(BaseAddr+0x34) = 0x3;
    /*set format start_1+8 bits+stop_1*/
    *(volatile uint32_t*)(BaseAddr+0x1C) = 0x23;
    /*clear fifo and set tx threshold=1, rx threshold=12 */
    *(volatile uint32_t*)(BaseAddr+0x14) = 0x801;
}

void uart_debug_deinit(hal_uart_port_t port)
{
    uint32_t BaseAddr = UART_PORT[port];
    /*reset to default value*/
    *(volatile uint32_t*)(BaseAddr+0x8) = 0x01;
    *(volatile uint32_t*)(BaseAddr+0xC) = 0x00;
    *(volatile uint32_t*)(BaseAddr+0x14) = 0xFFFFF000;
    *(volatile uint32_t*)(BaseAddr+0x18) = 0x00;
    *(volatile uint32_t*)(BaseAddr+0x1C) = 0x20;
    *(volatile uint32_t*)(BaseAddr+0x24) = 0x00;
    *(volatile uint32_t*)(BaseAddr+0x28) = 0x60;
    *(volatile uint32_t*)(BaseAddr+0x2C) = 0x00;
    *(volatile uint32_t*)(BaseAddr+0x30) = 0x00;
    *(volatile uint32_t*)(BaseAddr+0x34) = 0x00;
    *(volatile uint32_t*)(BaseAddr+0x38) = 0xFF00;
    *(volatile uint32_t*)(BaseAddr+0x3C) = 0x00;
    *(volatile uint32_t*)(BaseAddr+0x40) = 0x0D;
    *(volatile uint32_t*)(BaseAddr+0x44) = 0x0F;
    *(volatile uint32_t*)(BaseAddr+0x48) = 0xFF;
    *(volatile uint32_t*)(BaseAddr+0x4C) = 0x00;
    *(volatile uint32_t*)(BaseAddr+0x50) = 0x00;
    *(volatile uint32_t*)(BaseAddr+0x54) = 0x00;
    *(volatile uint32_t*)(BaseAddr+0x58) = 0x00;
    *(volatile uint32_t*)(BaseAddr+0x5C) = 0x00;
    *(volatile uint32_t*)(BaseAddr+0x60) = 0xFFFF;
}

void uart_debug_disable_tx_irq(hal_uart_port_t port,char byte)
{
    uint32_t BaseAddr = UART_PORT[port];
    /*disable UART TX interrupt, reserve UART RX interrupt*/
	*(volatile uint32_t*)(BaseAddr+0xC) = 0x300;
    /*disable UART DMA_mode TX channel, reserve UART DMA_mode RX channel*/
	*(volatile uint32_t*)(BaseAddr+0x50) = 0x1;
}

void uart_debug_set_software_flowcontrol(hal_uart_port_t port)
{
    uint32_t BaseAddr = UART_PORT[port];
    /*sw flow control*/
    *(volatile uint32_t*)(BaseAddr+0x18) = 0x03;
    /*set flow control char 0x11 0x13*/
    *(volatile uint32_t*)(BaseAddr+0x24) = 0x1113;
    /*set excape char 0x77 and enable*/
    *(volatile uint32_t*)(BaseAddr+0x48) = 0x177;
}
/*if uart_port = syslog port, we might need disable flow control to enable us to use character 0x11 & 0x13 & 0x77*/
void uart_debug_disable_software_flowcontrol(hal_uart_port_t port)
{
    uint32_t BaseAddr = UART_PORT[port];
    /*sw flow control*/
    *(volatile uint32_t*)(BaseAddr+0x18) = 0x00;
    /*set flow control char 0x11 0x13*/
    *(volatile uint32_t*)(BaseAddr+0x24) = 0x00;
    /*set excape char 0x77 and enable*/
    *(volatile uint32_t*)(BaseAddr+0x48) = 0x00;
}
/*send char*/
ATTR_TEXT_IN_TCM void uart_debug_print_byte(hal_uart_port_t port,char byte)
{
    uint32_t BaseAddr = UART_PORT[port];
    /*disable UART TX interrupt, reserve UART RX interrupt*/
	*(volatile uint32_t*)(BaseAddr+0xC) = 0x300;
    /*disable UART DMA_mode TX channel, reserve UART DMA_mode RX channel*/
	*(volatile uint32_t*)(BaseAddr+0x50) = 0x1;
    while (1) {
        /*(0x1<<6) <--- <6> tx empty,<5> below tx fifo threashold,(but now threashold=1).*/
        if (*(volatile uint32_t*)(BaseAddr+0x28) & (0x1<<6)) {
            *(volatile uint32_t*)(BaseAddr+0x04) = byte;
            break;
        }
    }
}

/*send word*/
ATTR_TEXT_IN_TCM void  uart_debug_print_word(hal_uart_port_t port,uint32_t word)
{
    char byte[4] = {0};
    for(uint32_t i = 4; i > 0; i--){
        byte[i] = (char)(word >> ((i-1)*8));
        uart_debug_print_byte(port,byte[i]);
    }
}

/*dump one consequent memory using address and length*/
ATTR_TEXT_IN_TCM void  uart_debug_dump_memory(hal_uart_port_t port,uint32_t address,uint32_t len)
{
    uint32_t i;
    uint32_t value = 0x0;

    for(i = 0; i < (len / 4); i++){
        uart_debug_print_byte(port,'\r');
        uart_debug_print_byte(port,'\n');
        uart_debug_print_byte(port,'\r');
        uart_debug_print_byte(port,'\n');
        value = *(volatile uint32_t*)(address + (i*4));
        uart_debug_print_word(port,value);
    }
}
/*send string*/
ATTR_TEXT_IN_TCM uint32_t uart_debug_send_polling(hal_uart_port_t port,const char *data, uint32_t size)
{
    uint32_t i, len;
    len = strlen(data);
    for (i = 0; i < len; i++) {
        uart_debug_print_byte(port,*data);
        data++;
    }
    return size;
}

/*
    if need clear DMA and UART fifo simultaneously,
    please call " hal_uart_status_t  uart_clear_vfifo_and_fifo(hal_uart_port_t uart_port).
*/
ATTR_TEXT_IN_TCM void uart_debug_clear_fifo(hal_uart_port_t port, bool is_rx)
{
    uint32_t BaseAddr = UART_PORT[port];
    if(is_rx){
        /*clear UART RX hardware fifo*/
        *(volatile uint32_t*)(BaseAddr+0x14) |= (1 << 16);
    }else {
        /*clear UART TX hardware fifo*/
        *(volatile uint32_t*)(BaseAddr+0x14) |= (1 << 24);
    }
}
#endif /*MTK_SWLA_ENABLE_O2*/

#endif /* MTK_SWLA_ENABLE */
