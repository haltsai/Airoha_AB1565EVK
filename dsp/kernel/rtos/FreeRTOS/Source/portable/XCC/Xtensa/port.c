
/*
 * FreeRTOS Kernel V10.0.0
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software. If you wish to use our Amazon
 * FreeRTOS name, please do so in a fair use way that does not cause confusion.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * Copyright (c) 2015-2019 Cadence Design Systems, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <xtensa/config/core.h>

#include "xtensa_rtos.h"

#include "FreeRTOS.h"
#include "task.h"

#include "hal_dwt.h"
#include "hal_gpt.h"

#ifdef MTK_SWLA_ENABLE
#include "swla.h"
#endif /* MTK_SWLA_ENABLE */

#if configUSE_TICKLESS_IDLE == 2
#include "hal_gpt_internal.h"
#endif

#include "syslog.h"
log_create_module(freertos, PRINT_LEVEL_INFO);

/* Defined in portasm.h */
extern void _frxt_tick_timer_init(void);

/* Defined in xtensa_context.S */
extern void _xt_coproc_init(void);


/*-----------------------------------------------------------*/

/* We require the address of the pxCurrentTCB variable, but don't want to know
any details of its type. */
typedef void TCB_t;
extern volatile TCB_t * volatile pxCurrentTCB;

unsigned port_xSchedulerRunning = 0; // Duplicate of inaccessible xSchedulerRunning; needed at startup to avoid counting nesting
unsigned port_interruptNesting = 0;  // Interrupt nesting level

/*-----------------------------------------------------------*/
#ifdef AIR_CPU_MCPS_PRIORING_ENABLE
task_profiling_type_t port_xTaskRuntimeCounter[config_MAX_NUM_OF_TASK]; //task run counter for MCPS profiling

void vPortSetupTcb(TaskHandle_t pxTCB)
{
	uint32_t index = 0;

	for(index=0; index < config_MAX_NUM_OF_TASK; index++){
		if(port_xTaskRuntimeCounter[index].xTaskHandle == NULL){
			port_xTaskRuntimeCounter[index].xTaskHandle = pxTCB;
			break;
		}
	}

	if(index == config_MAX_NUM_OF_TASK){
		LOG_MSGID_E(common,"task counts is bigger than the max=%d, cannot record the task 0x%x\n", 2, config_MAX_NUM_OF_TASK, (uint32_t)pxTCB);
	}
}

void vPortCleanUpTcb(TaskHandle_t pxTCB)
{
	uint32_t index = 0;

	for(index=0; index < config_MAX_NUM_OF_TASK; index++){
		if(port_xTaskRuntimeCounter[index].xTaskHandle == pxTCB){
			port_xTaskRuntimeCounter[index].xTaskHandle = NULL;
			break;
		}
	}
}
#endif /* AIR_CPU_MCPS_PRIORING_ENABLE */

// User exception dispatcher when exiting
void _xt_user_exit(void);

/* 
 * Stack initialization
 */
#if portUSING_MPU_WRAPPERS
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters, BaseType_t xRunPrivileged )
#else
StackType_t *pxPortInitialiseStack( StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters )
#endif
{
	#if portUSING_MPU_WRAPPERS
	(void)xRunPrivileged;
	#endif /* portUSING_MPU_WRAPPERS */

	StackType_t *sp, *tp;
	XtExcFrame  *frame;
	#if XCHAL_CP_NUM > 0
	uint32_t *p;
	#endif

	/* Create interrupt stack frame aligned to 16 byte boundary */
	sp = (StackType_t *) (((UBaseType_t)(pxTopOfStack + 1) - XT_CP_SIZE - XT_STK_FRMSZ) & ~0xf);

	/* Clear the entire frame (do not use memset() because we don't depend on C library) */
	for (tp = sp; tp <= pxTopOfStack; ++tp)
		*tp = 0;

	frame = (XtExcFrame *) sp;

	/* Explicitly initialize certain saved registers */
	frame->pc   = (UBaseType_t) pxCode;             /* task entrypoint                */
	frame->a0   = 0;                                /* to terminate GDB backtrace     */
	frame->a1   = (UBaseType_t) sp + XT_STK_FRMSZ;  /* physical top of stack frame    */
	frame->exit = (UBaseType_t) _xt_user_exit;      /* user exception exit dispatcher */

	/* Set initial PS to int level 0, EXCM disabled ('rfe' will enable), user mode. */
	/* Also set entry point argument parameter. */
	#ifdef __XTENSA_CALL0_ABI__
	frame->a2 = (UBaseType_t) pvParameters;
	frame->ps = PS_UM | PS_EXCM;
	#else
	/* + for windowed ABI also set WOE and CALLINC (pretend task was 'call4'd). */
	frame->a6 = (UBaseType_t) pvParameters;
	frame->ps = PS_UM | PS_EXCM | PS_WOE | PS_CALLINC(1);
	#endif

	#ifdef XT_USE_SWPRI
	/* Set the initial virtual priority mask value to all 1's. */
	frame->vpri = 0xFFFFFFFF;
	#endif

	#if XCHAL_CP_NUM > 0
	/* Init the coprocessor save area (see xtensa_context.h) */
	/* No access to TCB here, so derive indirectly. Stack growth is top to bottom.
         * //p = (uint32_t *) xMPUSettings->coproc_area;
	 */
	p = (uint32_t *)(((uint32_t) pxTopOfStack - XT_CP_SIZE) & ~0xf);
	p[0] = 0;
	p[1] = 0;
	p[2] = (((uint32_t) p) + 12 + XCHAL_TOTAL_SA_ALIGN - 1) & -XCHAL_TOTAL_SA_ALIGN;
	#endif

	return sp;
}

/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
	/* It is unlikely that the Xtensa port will get stopped.  If required simply
	disable the tick interrupt here. */
}

/*-----------------------------------------------------------*/
#if (configCHECK_FOR_STACK_OVERFLOW > 0)
#ifdef HAL_DWT_MODULE_ENABLED
ATTR_TEXT_IN_IRAM void vPortCurrentTaskStackOverflowCheck(void)
{
	uint32_t stack_start_address;
	int32_t ret;

	stack_start_address = uxTaskGetBottomOfStack(NULL);

	/* check the last 4Byte because the stack is 4 Byte aligned*/
	ret = hal_dwt_request_watchpoint(HAL_DWT_0, stack_start_address, 0x2, WDE_DATA_WO);
	// printf("comparator:%d, check address: 0x%x\r\n",HAL_DWT_0,stack_start_address);

	/* Just to avoid compiler warnings. */
	( void ) ret;
}
#else
	#error please enable HAL_DWT_MODULE_ENABLED in project inc/hal_feature_config.h for task stack overflow check.
#endif /* HAL_DWT_MODULE_ENABLED */
#endif /* (configCHECK_FOR_STACK_OVERFLOW > 0) */

#ifdef AIR_CPU_MCPS_PRIORING_ENABLE
uint32_t os_profiling_gpt_handle = 0;
#endif /* AIR_CPU_MCPS_PRIORING_ENABLE */

BaseType_t xPortStartScheduler( void )
{
	#ifdef AIR_CPU_MCPS_PRIORING_ENABLE
	hal_gpt_status_t gpt_ret1, gpt_ret2;
	#endif /* AIR_CPU_MCPS_PRIORING_ENABLE */

	// Interrupts are disabled at this point and stack contains PS with enabled interrupts when task context is restored

	#if XCHAL_CP_NUM > 0
	/* Initialize co-processor management for tasks. Leave CPENABLE alone. */
	_xt_coproc_init();
	#endif

	/* Init the tick divisor value */
	//_xt_tick_divisor_init();
	/* Setup the hardware to generate the tick. */
	_frxt_tick_timer_init();

	#if XT_USE_THREAD_SAFE_CLIB
	// Init C library
	vPortClibInit();
	#endif

    #if defined(MTK_SWLA_ENABLE)
    SLA_Enable();
    #endif /* MTK_SWLA_ENABLE */

#if (configCHECK_FOR_STACK_OVERFLOW > 0)
#ifdef HAL_DWT_MODULE_ENABLED
	hal_dwt_init();
	vPortCurrentTaskStackOverflowCheck();

    /* 0x0 is not available for DSP, add dwt channel to monitor the illegal access, such as load/store data.
        note1: the illegal access will corrupt CM4 RAM code area on the chip with PSRAM.
        note2: monitor area [0x0, 0x63], the max-length of DWT.
    */
    hal_dwt_request_watchpoint(HAL_DWT_1, 0x0, 0x6, WDE_DATA_RW);
#endif /* HAL_DWT_MODULE_ENABLED */
#endif /* (configCHECK_FOR_STACK_OVERFLOW > 0) */

#ifdef AIR_CPU_MCPS_PRIORING_ENABLE
    /* register and start os profiling timer, the interval is 1s */
	extern VOID osDumpTaskCounter(VOID);
	gpt_ret1 = hal_gpt_sw_get_timer(&os_profiling_gpt_handle);
	gpt_ret2 = hal_gpt_sw_start_timer_ms(os_profiling_gpt_handle, OS_PROFILING_GPT_PERIOD_MS, (hal_gpt_callback_t)osDumpTaskCounter, NULL);
	if ((HAL_GPT_STATUS_OK != gpt_ret1) || (HAL_GPT_STATUS_OK != gpt_ret2) ){
		LOG_MSGID_E(common, "os profiling timer is not started:%d, %d", 2, gpt_ret1, gpt_ret2);
	}
#endif /* AIR_CPU_MCPS_PRIORING_ENABLE */

	port_xSchedulerRunning = 1;

	// Cannot be directly called from C; never returns
	__asm__ volatile ("call0    _frxt_dispatch\n");

	/* Should not get here. */
	return pdTRUE;
}
/*-----------------------------------------------------------*/

ATTR_TEXT_IN_IRAM BaseType_t xPortSysTickHandler( void )
{
	BaseType_t ret;
	UBaseType_t uxSavedInterruptStatus;

#if defined(MTK_SWLA_ENABLE)
    //SLA_CustomLogging("tik",SA_LABEL);
#endif /* MTK_SWLA_ENABLE */

	portbenchmarkIntLatency();

	/* Interrupts upto configMAX_SYSCALL_INTERRUPT_PRIORITY must be
	 * disabled before calling xTaskIncrementTick as it access the
	 * kernel lists. */
	uxSavedInterruptStatus = portSET_INTERRUPT_MASK_FROM_ISR();

#if configUSE_TICKLESS_IDLE == 2
	extern hal_gpt_status_t hal_gpt_stop_timer(hal_gpt_port_t gpt_port);
	extern hal_gpt_status_t hal_gpt_start_timer_us(hal_gpt_port_t gpt_port, uint32_t timeout_time_us, hal_gpt_timer_type_t timer_type);
	extern volatile uint32_t systick_change_period;
	if(systick_change_period == 1)
	{
		systick_change_period = 0;
		hal_gpt_stop_timer(HAL_GPT_7);
		hal_gpt_start_timer_us(HAL_GPT_7,portTICK_PERIOD_MS*1000,HAL_GPT_TIMER_TYPE_REPEAT);
	}
 #endif

	{
		ret = xTaskIncrementTick();
		portYIELD_FROM_ISR( ret );     /* only set a global fag to mark a task context switch is needed, and the task context switched issued when interrupt return */
	}
	portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedInterruptStatus );



	return ret;
}
/*-----------------------------------------------------------*/

/*
 * Used to set coprocessor area in stack. Current hack is to reuse MPU pointer for coprocessor area.
 */
#if portUSING_MPU_WRAPPERS
void vPortStoreTaskMPUSettings( xMPU_SETTINGS *xMPUSettings, const struct xMEMORY_REGION * const xRegions, StackType_t *pxBottomOfStack, uint32_t ulStackDepth )
{
	(void)xRegions;
	#if XCHAL_CP_NUM > 0
	//xMPUSettings->coproc_area = (StackType_t*)((((uint32_t)(pxBottomOfStack + ulStackDepth - 1)) - XT_CP_SIZE ) & ~0xf);

	uint32_t tmp = ((uint32_t)(pxBottomOfStack + ulStackDepth - 1)) & (~((portPOINTER_SIZE_TYPE) portBYTE_ALIGNMENT_MASK)); //pxTopOfStack
	tmp = ((tmp - XT_CP_SIZE) & ~0xf);
	xMPUSettings->coproc_area = (StackType_t*) tmp;

	/* NOTE: we cannot initialize the coprocessor save area here because FreeRTOS is going to
         * clear the stack area after we return. This is done in pxPortInitialiseStack().
	 */
	#endif
}
#endif

#if( configCHECK_FOR_STACK_OVERFLOW > 0 )
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    printf("stack overflow: %x %s\r\n",(unsigned int)xTask,(portCHAR *)pcTaskName);
    configASSERT(0);
}
#endif

