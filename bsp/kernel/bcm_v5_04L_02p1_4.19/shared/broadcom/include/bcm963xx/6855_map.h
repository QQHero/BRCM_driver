/*
<:copyright-BRCM:2020:proprietary:standard

   Copyright (c) 2020 Broadcom 
   All Rights Reserved

 This program is the proprietary software of Broadcom and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").  Except as set forth in
 an Authorized License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and Broadcom
 expressly reserves all rights in and to the Software and all intellectual
 property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE
 NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY
 BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

 Except as expressly set forth in the Authorized License,

 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use
    all reasonable efforts to protect the confidentiality thereof, and to
    use this information only in connection with your use of Broadcom
    integrated circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
    RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND
    ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT,
    FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR
    COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE
    TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR
    PERFORMANCE OF THE SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR
    ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL,
    INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY
    WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
    IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES;
    OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE
    SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS
    SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY
    LIMITED REMEDY.
:>
*/
/***********************************************************************/
/*                                                                     */
/*   MODULE:  6855_map.h                                               */
/*   PURPOSE: Define the proprietary hardware blocks/subblocks for     */
/*            BCM6855                                                  */
/*                                                                     */
/***********************************************************************/
#ifndef __BCM6855_MAP_H
#define __BCM6855_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bcmtypes.h"
#include "6855_common.h"
#include "6855_map_part.h"

/* For the proprietary blocks that needs be mapped in the linux, the base 
   address still define in the map_part.h. The detail block register definition
   must be defined in this file with the C structure below */
#define BROM_BASE                   (PERF_BASE + 0x00000600)  /* bootrom registers */
#define BROM_GEN_BASE               BROM_BASE
#define BROM_SEC_BASE               (PERF_BASE + 0x00000620)  /* bootrom secure registers */
#define BROM_SEC1_BASE              BROM_SEC_BASE

/* For the proprietary blocks does not need be mapped in linux at all */
#define BOOTROM_BASE                0xfff00000
#define BOOTROM_SIZE                0x20000
#define RDP_SRAM_BASE               0x82000000
#define RDP_SRAM_SIZE               0x10000     /* 256KB */

#define BOOTLUT_SIZE                0x1000

#define SOTP_OFFSET                 0x4000
#define SOTP_BASE                   (PERF_PHYS_BASE+SOTP_OFFSET)
/*
#####################################################################
# BIU config Registers
#####################################################################
*/
#define TS0_CTRL_CNTCR             0x1000

#ifndef __ASSEMBLER__

#if defined(__KERNEL__) && !defined(MODULE)
#error "PRIVATE FILE INCLUDED IN KERNEL"
#endif


typedef struct BootBase {
    uint32 general_secbootcfg;
#define BOOTROM_CRC_DONE                (1 << 31)
#define BOOTROM_CRC_FAIL                (1 << 30)
    uint32 general_boot_crc_low;
    uint32 general_boot_crc_high;
} BootBase;

#define BOOTBASE ((volatile BootBase * const) BROM_BASE)

typedef struct BootSec {
    uint32 AccessCtrl;
    uint32 AccessRangeChk[4];
} BootSec;

#define BOOTSECURE ((volatile BootSec * const) BROM_SEC_BASE)

/* SOTP defs */
typedef struct SecKeyObj {
#define SECURE_KEY_KEY0_PAC_CNTRL_SHIFT        0x0
#define SECURE_KEY_KEY0_PAC_CNTRL_MASK         0xffffffff
   uint32 key0_pac_ctrl;                   /* 0x00       */
#define SECURE_KEY_KEY0_STATUS_SHIFT           30
#define SECURE_KEY_KEY0_STATUS_MASK            (1<<30)
   uint32 key0_status;                     /* 0x04       */
   uint32 rsvd1;                           /* 0x08       */
#define SECURE_KEY_KEY0_CONTROL_SHIFT           30
#define SECURE_KEY_KEY0_CONTROL_MASK           (1<<30)
   uint32 key0_ctrl;                       /* 0x0C       */
#define SECURE_KEY_KEY0_DATA_SHIFT             0x0
#define SECURE_KEY_KEY0_DATA_MASK              0xffffffff 
   uint32 key0_data[8];                    /* 0x010-0x2c */
   uint32 rsvd2[4];
#define SECURE_KEY_KEY1_PAC_CNTRL_SHIFT        0x0
#define SECURE_KEY_KEY1_PAC_CNTRL_MASK         0xffffffff
   uint32 key1_pac_ctrl;                   /* 0x40       */
#define SECURE_KEY_KEY1_STATUS_SHIFT       30
#define SECURE_KEY_KEY1_STATUS_MASK        (1<<30)
   uint32 key1_status;                     /* 0x44       */
   uint32 rsvd3;                           /* 0x48       */
#define SECURE_KEY_KEY1_CONTROL_SHIFT           30
#define SECURE_KEY_KEY1_CONTROL_MASK           (1<<30)
   uint32 key1_ctrl;                       /* 0x4C       */
#define SECURE_KEY_KEY1_DATA_SHIFT             0x0
#define SECURE_KEY_KEY1_DATA_MASK              0xffffffff 
   uint32 key1_data[8];                    /* 0x50-0x6c  */
} SecKeyObj,Sotp;

#define SOTP_OBJ ((volatile Sotp * const) SOTP_BASE)

#endif


#ifdef __cplusplus
}
#endif

#endif

