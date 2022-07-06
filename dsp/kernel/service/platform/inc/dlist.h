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
 
#ifndef _DLIST_H_
#define _DLIST_H_

/*!
 *@file   dlist.h
 *@brief  Defines the interface of double linkin list
 *
 @verbatim
 @endverbatim
 */


#include "types.h"
////////////////////////////////////////////////////////////////////////////////
// Type Definitions ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief double link list structure
 */
typedef struct dlist_stru_t
{
    struct dlist_stru_t * prev;
    struct dlist_stru_t * next;

} DLIST, *DLIST_PTR;

typedef DLIST DLIST_HEAD;
typedef DLIST_PTR DLIST_HEAD_PTR;
/*
 * DLIST_HEAD - list head of double link list structure.
 * This structure is a structure of double link list.
 */



////////////////////////////////////////////////////////////////////////////////
// Function Prototypes /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * @brief       get the container structure of given node
 *
 * @param ptr        the linkin list pointer.
 * @param type       the data structure of container
 * @param member     member of container structure
 * @return           pointer of the contanier structure
 */
#define dlist_OFFSET_OF(type,member) ((SIZE)&(((type*)0)->member))
#define dlist_entry(ptr,type,member) ((type*)((U8*)(ptr) - dlist_OFFSET_OF(type,member)))


/**
 * @breif init a list entry
 *
 * @param list_ptr  list need to initialize
 */
STATIC INLINE VOID dlist_init(
          DLIST_PTR plist)
{
    plist->next = plist->prev = plist;
}


/**
 * @brief check whether a list is empty
 *
 * @param plist pointer of list
 * @return TURE means empty , FALSE means not empty
 */
STATIC INLINE BOOL dlist_is_empty(
            DLIST_PTR plist)
{
    return (plist == plist->next);
}

/**
 * @brief insert a node into a list
 *
 * @param list            pointer of list
 * @param prev_list       pointer of prev list
 * @param next_list       pointer of next list
 */
STATIC INLINE VOID dlist_insert(
                DLIST_PTR list,
                DLIST_PTR prev_list,
                DLIST_PTR next_list)
{
        list->prev = prev_list;
        list->next = next_list;
        prev_list->next = list;
        next_list->prev = list;
}

/**
 * @brief remove node from a list
 *
 * @param  list pointer of list
 */
STATIC INLINE VOID dlist_remove(
          DLIST_PTR list)
{
    list->prev->next = list->next;
    list->next->prev = list->prev;
    dlist_init(list);
}

/**
 * @brief add a node into list front
 * @param newptr             new node to be added
 * @param head            list head to add it after
 */
STATIC INLINE VOID dlist_prepend(
          DLIST_PTR newptr,
          DLIST_PTR head)
{
    dlist_insert(newptr,head,head->next);    
}

/**
 * @brief  add a node into list back
 * @param newptr            new node to be added
 * @param head           list head to add it before
 *
 * Insert a new node just before the specified head.
 */
STATIC INLINE VOID dlist_append(
          DLIST_PTR newptr,
          DLIST_PTR head)
{
    dlist_insert(newptr,head->prev,head);
}

/**
 * @brief get first node from double link list
 *
 * @param head   head of a double list.
 * @return first node of list , null if no node in list
 */
EXTERN DLIST_PTR dlist_getfront(
    IN      DLIST_PTR head);

#endif /* _OS_LIST_H_ */

