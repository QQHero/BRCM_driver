/*
   Copyright (c) 2015 Broadcom
   All Rights Reserved

    <:label-BRCM:2015:DUAL/GPL:standard

    Unless you and Broadcom execute a separate written software license
    agreement governing use of this software, this software is licensed
    to you under the terms of the GNU General Public License version 2
    (the "GPL"), available at http://www.broadcom.com/licenses/GPLv2.php,
    with the following added to such license:

       As a special exception, the copyright holders of this software give
       you permission to link this software with independent modules, and
       to copy and distribute the resulting executable under terms of your
       choice, provided that you also meet, for each linked independent
       module, the terms and conditions of the license of that module.
       An independent module is a module which is not derived from this
       software.  The special exception does not apply to any modifications
       of the software.

    Not withstanding the above, under no circumstances may you combine
    this software in any way with any other Broadcom software provided
    under a license other than the GPL, without Broadcom's express prior
    written consent.

:>
*/

#include "ru.h"

#if RU_INCLUDE_FIELD_DB
/******************************************************************************
 * Field: EARLY_TXEN_CFG_HOLD_TIME
 ******************************************************************************/
const ru_field_rec EARLY_TXEN_CFG_HOLD_TIME_FIELD =
{
    "HOLD_TIME",
#if RU_INCLUDE_DESC
    "HOLD_TIME",
    "Early TXEN Hold Time",
#endif
    EARLY_TXEN_CFG_HOLD_TIME_FIELD_MASK,
    0,
    EARLY_TXEN_CFG_HOLD_TIME_FIELD_WIDTH,
    EARLY_TXEN_CFG_HOLD_TIME_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: EARLY_TXEN_CFG_SETUP_TIME
 ******************************************************************************/
const ru_field_rec EARLY_TXEN_CFG_SETUP_TIME_FIELD =
{
    "SETUP_TIME",
#if RU_INCLUDE_DESC
    "SETUP_TIME",
    "Early TXEN Setup Time",
#endif
    EARLY_TXEN_CFG_SETUP_TIME_FIELD_MASK,
    0,
    EARLY_TXEN_CFG_SETUP_TIME_FIELD_WIDTH,
    EARLY_TXEN_CFG_SETUP_TIME_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: EARLY_TXEN_CFG_TOFF_TIME
 ******************************************************************************/
const ru_field_rec EARLY_TXEN_CFG_TOFF_TIME_FIELD =
{
    "TOFF_TIME",
#if RU_INCLUDE_DESC
    "TOFF_TIME",
    "Early TXEN Toff Time",
#endif
    EARLY_TXEN_CFG_TOFF_TIME_FIELD_MASK,
    0,
    EARLY_TXEN_CFG_TOFF_TIME_FIELD_WIDTH,
    EARLY_TXEN_CFG_TOFF_TIME_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: EARLY_TXEN_CFG_OUTPUT_TXEN_POLARITY
 ******************************************************************************/
const ru_field_rec EARLY_TXEN_CFG_OUTPUT_TXEN_POLARITY_FIELD =
{
    "OUTPUT_TXEN_POLARITY",
#if RU_INCLUDE_DESC
    "OUTPUT_TXEN_POLARITY",
    "Early TXEN output polarity."
    "0 - Active Low"
    "1 - Active High",
#endif
    EARLY_TXEN_CFG_OUTPUT_TXEN_POLARITY_FIELD_MASK,
    0,
    EARLY_TXEN_CFG_OUTPUT_TXEN_POLARITY_FIELD_WIDTH,
    EARLY_TXEN_CFG_OUTPUT_TXEN_POLARITY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: EARLY_TXEN_CFG_INPUT_TXEN_POLARITY
 ******************************************************************************/
const ru_field_rec EARLY_TXEN_CFG_INPUT_TXEN_POLARITY_FIELD =
{
    "INPUT_TXEN_POLARITY",
#if RU_INCLUDE_DESC
    "INPUT_TXEN_POLARITY",
    "Mac TXEN input polarity."
    "0 - Active Low"
    "1 - Active High",
#endif
    EARLY_TXEN_CFG_INPUT_TXEN_POLARITY_FIELD_MASK,
    0,
    EARLY_TXEN_CFG_INPUT_TXEN_POLARITY_FIELD_WIDTH,
    EARLY_TXEN_CFG_INPUT_TXEN_POLARITY_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: EARLY_TXEN_CFG_EARLY_TXEN_BYPASS
 ******************************************************************************/
const ru_field_rec EARLY_TXEN_CFG_EARLY_TXEN_BYPASS_FIELD =
{
    "EARLY_TXEN_BYPASS",
#if RU_INCLUDE_DESC
    "EARLY_TXEN_BYPASS",
    "Early TXEN Enable Logic Bypass.",
#endif
    EARLY_TXEN_CFG_EARLY_TXEN_BYPASS_FIELD_MASK,
    0,
    EARLY_TXEN_CFG_EARLY_TXEN_BYPASS_FIELD_WIDTH,
    EARLY_TXEN_CFG_EARLY_TXEN_BYPASS_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

/******************************************************************************
 * Field: EARLY_TXEN_CFG_RESERVED0
 ******************************************************************************/
const ru_field_rec EARLY_TXEN_CFG_RESERVED0_FIELD =
{
    "RESERVED0",
#if RU_INCLUDE_DESC
    "",
    "",
#endif
    EARLY_TXEN_CFG_RESERVED0_FIELD_MASK,
    0,
    EARLY_TXEN_CFG_RESERVED0_FIELD_WIDTH,
    EARLY_TXEN_CFG_RESERVED0_FIELD_SHIFT,
#if RU_INCLUDE_ACCESS
    ru_access_rw
#endif
};

#endif /* RU_INCLUDE_FIELD_DB */

/******************************************************************************
 * Register: EARLY_TXEN_CFG
 ******************************************************************************/
#if RU_INCLUDE_FIELD_DB
static const ru_field_rec *EARLY_TXEN_CFG_FIELDS[] =
{
    &EARLY_TXEN_CFG_HOLD_TIME_FIELD,
    &EARLY_TXEN_CFG_SETUP_TIME_FIELD,
    &EARLY_TXEN_CFG_TOFF_TIME_FIELD,
    &EARLY_TXEN_CFG_OUTPUT_TXEN_POLARITY_FIELD,
    &EARLY_TXEN_CFG_INPUT_TXEN_POLARITY_FIELD,
    &EARLY_TXEN_CFG_EARLY_TXEN_BYPASS_FIELD,
    &EARLY_TXEN_CFG_RESERVED0_FIELD,
};

#endif /* RU_INCLUDE_FIELD_DB */

const ru_reg_rec EARLY_TXEN_CFG_REG = 
{
    "CFG",
#if RU_INCLUDE_DESC
    "EARLY_TXEN_CFG Register",
    "Holds configuration registers of the Early TXEN for PMD",
#endif
    EARLY_TXEN_CFG_REG_OFFSET,
    0,
    0,
    1,
#if RU_INCLUDE_ACCESS
    ru_access_rw,
#endif
#if RU_INCLUDE_FIELD_DB
    7,
    EARLY_TXEN_CFG_FIELDS
#endif /* RU_INCLUDE_FIELD_DB */
};

/******************************************************************************
 * Block: EARLY_TXEN
 ******************************************************************************/
static const ru_reg_rec *EARLY_TXEN_REGS[] =
{
    &EARLY_TXEN_CFG_REG,
};

unsigned long EARLY_TXEN_ADDRS[] =
{
    0x82db2030,
};

const ru_block_rec EARLY_TXEN_BLOCK = 
{
    "EARLY_TXEN",
    EARLY_TXEN_ADDRS,
    1,
    1,
    EARLY_TXEN_REGS
};

/* End of file WAN_TOP_EARLY_TXEN.c */
