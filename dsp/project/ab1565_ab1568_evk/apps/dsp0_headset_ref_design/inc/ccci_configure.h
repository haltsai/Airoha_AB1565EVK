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
 
#ifndef __CCCI_CONFIGURE_H__
#define __CCCI_CONFIGURE_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef AB155X
/*
sample code:
//if user want to add DSP1 to DSP0 event,please add to here!!!
//this event is a enum of type of ccci_event_t, the event name define by user
//better follow the naming rule of CCCI_EVENT_DSP1_TO_DSP0_{user name}
//please strict follow the format, don't assignment value by user
//like:"CCCI_EVENT_DSP1_TO_DSP0_111 = 1," is forbidden!!!
#define DSP1_TO_DSP0_CCCI_EVENT_LIST    \
                                        CCCI_EVENT_DSP1_TO_DSP0_111,\
                                        CCCI_EVENT_DSP1_TO_DSP0_222,

//if user want to add DSP0 to DSP1 event,please add to here!!!
//this event is a enum of type of ccci_event_t, the event name define by user
//better follow the naming rule of CCCI_EVENT_DSP0_TO_DSP1_{user name}
//please strict follow the format, don't assignment value by user
//like:"CCCI_EVENT_DSP0_TO_DSP1_333 = 3," is forbidden!!!
#define DSP0_TO_DSP1_CCCI_EVENT_LIST    \
                                        CCCI_EVENT_DSP0_TO_DSP1_333,\
                                        CCCI_EVENT_DSP0_TO_DSP1_444,

//if user had add event to "DSP1_TO_DSP0_CCCI_EVENT_LIST". DSP0 as receiver, must add the event callback to here!!!
//user must follow the format, the event sequence must be follow the event of "DSP1_TO_DSP0_CCCI_EVENT_LIST"!!!
#define DSP0_RECEIVE_CCCI_FROM_DSP1_USER_ARRAY  \
        {CCCI_EVENT_DSP1_TO_DSP0_111, ccci_dsp1_to_dsp0_111_callback},\
        {CCCI_EVENT_DSP1_TO_DSP0_222, ccci_dsp1_to_dsp0_222_callback},
*/




/*******************************************************************************/
/*Start*****************CCCI define part 1:DSP0<-->DSP1 ************************/
/*******************************************************************************/
    /*note: For CCCI of DSP0<-->DSP1:
        1. DSP1_TO_DSP0_CCCI_EVENT_LIST and DSP0_TO_DSP1_CCCI_EVENT_LIST must be defined on both DSP0 and DSP1 project file of ccci_configure.h
        2. DSP0_RECEIVE_CCCI_FROM_DSP1_USER_ARRAY just defined on DSP0 project file of ccci_configure.h
        3. DSP1_RECEIVE_CCCI_FROM_DSP0_USER_ARRAY just defined on DSP1 project file of ccci_configure.h
    */
#if defined(PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1) && defined(DSP1_PISPLIT_DEMO_LIBRARY)
extern void dsp1_pic_demo_lib_execute_done();
/*User should add your event which DSP1 send to DSP0 */
#define DSP1_TO_DSP0_CCCI_EVENT_LIST   \
                                        CCCI_EVENT_DSP1_TO_DSP0_PIC_PIC_DEMO_LIBRARY_EXECUTE_DONE,

/* User should add your event which DSP0 send to DSP1
    The event of CCCI_EVENT_DSP0_TO_DSP1_PIC_MEMORY_REQUEST is mandatory for DSP0 request the information of DSP1 pools, please clone to you project!!!
    The last two events is mandatory for the library of demo library, please clone to you project!!!
    User also can add more events, like: "data is ready" and "process is done".*/
#define DSP0_TO_DSP1_CCCI_EVENT_LIST    \
                                        CCCI_EVENT_DSP0_TO_DSP1_PIC_MEMORY_REQUEST,\
                                        CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_LOAD_DONE,\
                                        CCCI_EVENT_DSP0_TO_DSP1_PIC_DEMO_LIBRARY_UNLOAD_DONE,

/*User should add your event and callback to here*/
#define DSP0_RECEIVE_CCCI_FROM_DSP1_USER_ARRAY  \
             {CCCI_EVENT_DSP1_TO_DSP0_PIC_PIC_DEMO_LIBRARY_EXECUTE_DONE, dsp1_pic_demo_lib_execute_done},


//#define DSP1_RECEIVE_CCCI_FROM_DSP0_USER_ARRAY


      
#elif defined(PRELOADER_ENABLE_DSP0_LOAD_FOR_DSP1)

#define DSP1_TO_DSP0_CCCI_EVENT_LIST

#define DSP0_TO_DSP1_CCCI_EVENT_LIST    \
                                        CCCI_EVENT_DSP0_TO_DSP1_PIC_MEMORY_REQUEST,

#define DSP0_RECEIVE_CCCI_FROM_DSP1_USER_ARRAY
//#define DSP1_RECEIVE_CCCI_FROM_DSP0_USER_ARRAY

#else
#define DSP1_TO_DSP0_CCCI_EVENT_LIST

#define DSP0_TO_DSP1_CCCI_EVENT_LIST

#define DSP0_RECEIVE_CCCI_FROM_DSP1_USER_ARRAY
//#define DSP1_RECEIVE_CCCI_FROM_DSP0_USER_ARRAY
#endif
/*******************************************************************************/
/*End*****************CCCI define part 1:DSP0<-->DSP1 **************************/
/*******************************************************************************/



/*******************************************************************************/
/*Start*****************CCCI define part 2:CM4<-->DSP0 *************************/
/*******************************************************************************/
    /*note: For CCCI of CM4<-->DSP0:
        1. CM4_TO_DSP0_CCCI_EVENT_LIST and DSP0_TO_CM4_CCCI_EVENT_LIST must be defined on both CM4 and DSP0 project file of ccci_configure.h
        2. DSP0_RECEIVE_CCCI_FROM_CM4_USER_ARRAY just defined on DSP0 project file of ccci_configure.h
        3. CM4_RECEIVE_CCCI_FROM_DSP0_USER_ARRAY just defined on CM4 project file of ccci_configure.h
    */
#define CM4_TO_DSP0_CCCI_EVENT_LIST

#define DSP0_TO_CM4_CCCI_EVENT_LIST

#define DSP0_RECEIVE_CCCI_FROM_CM4_USER_ARRAY

//#define CM4_RECEIVE_CCCI_FROM_DSP0_USER_ARRAY
/*******************************************************************************/
/*End*****************CCCI define part 2:CM4<-->DSP0 ***************************/
/*******************************************************************************/




/*******************************************************************************/
/*Start*****************CCCI define part 3:CM4<-->DSP1 *************************/
/*******************************************************************************/
    /*note: For CCCI of CM4<-->DSP1:
        1. CM4_TO_DSP1_CCCI_EVENT_LIST and DSP1_TO_CM4_CCCI_EVENT_LIST must be defined on both CM4 and DSP1 project file of ccci_configure.h
        2. DSP1_RECEIVE_CCCI_FROM_CM4_USER_ARRAY just defined on DSP1 project file of ccci_configure.h
        3. CM4_RECEIVE_CCCI_FROM_DSP1_USER_ARRAY just defined on CM4 project file of ccci_configure.h
    */
//#define CM4_TO_DSP1_CCCI_EVENT_LIST

//#define DSP1_TO_CM4_CCCI_EVENT_LIST

//#define DSP1_RECEIVE_CCCI_FROM_CM4_USER_ARRAY

//#define CM4_RECEIVE_CCCI_FROM_DSP1_USER_ARRAY

/*******************************************************************************/
/*End*****************CCCI define part 3:CM4<-->DSP1 ***************************/
/*******************************************************************************/

#elif defined(AB156X)

/*
sample code:
//if user want to add CM4 to DSP0 event,please add to here!!!
//this event is a enum of type of ccci_event_t, the event name define by user
//better follow the naming rule of CCCI_EVENT_CM4_TO_DSP0_{user name}
//please strictly follow the format, don't assignment value by user
//like:"CCCI_EVENT_CM4_TO_DSP0_111 = 1," is forbidden!!!
#define CM4_TO_DSP0_CCCI_EVENT_LIST    \
                                        CCCI_EVENT_CM4_TO_DSP0_111,\
                                        CCCI_EVENT_CM4_TO_DSP0_222,

//if user want to add DSP0 to CM4 event,please add to here!!!
//this event is a enum of type of ccci_event_t, the event name define by user
//better follow the naming rule of CCCI_EVENT_DSP0_TO_CM4_{user name}
//please strict follow the format, don't assignment value by user
//like:"CCCI_EVENT_DSP0_TO_CM4_333 = 3," is forbidden!!!
#define DSP0_TO_CM4_CCCI_EVENT_LIST    \
                                        CCCI_EVENT_DSP0_TO_CM4_333,\
                                        CCCI_EVENT_DSP0_TO_CM4_444,

//if user had add event to "CM4_TO_DSP0_CCCI_EVENT_LIST". DSP0 as receiver, must add the event callback to here!!!
//user must follow the format, the event sequence must be follow the event of "CM4_TO_DSP0_CCCI_EVENT_LIST"!!!
#define DSP0_RECEIVE_CCCI_FROM_CM4_USER_ARRAY  \
        {CCCI_EVENT_CM4_TO_DSP0_111, ccci_cm4_to_dsp0_111_callback},\
        {CCCI_EVENT_CM4_TO_DSP0_222, ccci_cm4_to_dsp0_222_callback},
*/


/*******************************************************************************/
/*Start*****************CCCI define part 1:CM4<-->DSP0 *************************/
/*******************************************************************************/
    /*note: For CCCI of CM4<-->DSP0:
        1. CM4_TO_DSP0_CCCI_EVENT_LIST and DSP0_TO_CM4_CCCI_EVENT_LIST must be defined on both CM4 and DSP0 project file of ccci_configure.h
        2. DSP0_RECEIVE_CCCI_FROM_CM4_USER_ARRAY just defined on DSP0 project file of ccci_configure.h
        3. CM4_RECEIVE_CCCI_FROM_DSP0_USER_ARRAY just defined on CM4 project file of ccci_configure.h
    */

#define CM4_TO_DSP0_CCCI_EVENT_LIST  \
                                        CCCI_EVENT_CM4_TO_DSP0_EXAMPLE_EVENT, \
                                        CCCI_EVENT_CM4_TO_DSP0_EXAMPLE_EVENT_1,

#define DSP0_TO_CM4_CCCI_EVENT_LIST  \
                                        CCCI_EVENT_DSP0_TO_CM4_EXAMPLE_EVENT, \
                                        CCCI_EVENT_DSP0_TO_CM4_EXAMPLE_EVENT_1,

//#define DSP0_RECEIVE_CCCI_FROM_CM4_USER_ARRAY

#define DSP0_RECEIVE_CCCI_FROM_CM4_USER_ARRAY  \
                          {CCCI_EVENT_CM4_TO_DSP0_EXAMPLE_EVENT, ccci_cm4_to_dsp0_example_callback}, \
                          {CCCI_EVENT_CM4_TO_DSP0_EXAMPLE_EVENT_1, ccci_cm4_to_dsp0_example_callback},

/*******************************************************************************/
/*End*****************CCCI define part 1:CM4<-->DSP0 ***************************/
/*******************************************************************************/

#endif

#ifdef __cplusplus
}
#endif

#endif /* __CCCI_CONFIGURE_H__ */

