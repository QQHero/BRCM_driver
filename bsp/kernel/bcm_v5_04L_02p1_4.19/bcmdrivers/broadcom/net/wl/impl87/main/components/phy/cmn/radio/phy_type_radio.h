/*
 * RADIO control module internal interface (to PHY specific implementations).
 *
 * Copyright 2022 Broadcom
 *
 * This program is the proprietary software of Broadcom and/or
 * its licensors, and may only be used, duplicated, modified or distributed
 * pursuant to the terms and conditions of a separate, written license
 * agreement executed between you and Broadcom (an "Authorized License").
 * Except as set forth in an Authorized License, Broadcom grants no license
 * (express or implied), right to use, or waiver of any kind with respect to
 * the Software, and Broadcom expressly reserves all rights in and to the
 * Software and all intellectual property rights therein.  IF YOU HAVE NO
 * AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY
 * WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF
 * THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1. This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof, and to
 * use this information only in connection with your use of Broadcom
 * integrated circuit products.
 *
 * 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
 * OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
 * SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
 * IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
 * IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
 * ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
 * OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
 * NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
 *
 *
 * <<Broadcom-WL-IPTag/Proprietary:>>
 *
 * $Id: phy_type_radio.h 782142 2019-12-11 20:57:38Z $
 */

#ifndef _phy_type_radio_h_
#define _phy_type_radio_h_

#include <typedefs.h>
#include <bcmutils.h>
#include <phy_radio.h>

/*
 * PHY type implementation interface.
 *
 * Each PHY type implements the following functionality and registers the functions
 * via a vtbl/ftbl defined below, along with a context 'ctx' pointer.
 */
typedef void phy_type_radio_ctx_t;

typedef void (*phy_type_radio_switch_fn_t)(phy_type_radio_ctx_t *ctx, bool on);
typedef void (*phy_type_radio_on_fn_t)(phy_type_radio_ctx_t *ctx);
typedef void (*phy_type_radio_bandx_fn_t)(phy_type_radio_ctx_t *ctx);
typedef void (*phy_type_radio_init_fn_t)(phy_type_radio_ctx_t *ctx);
typedef uint32 (*phy_type_radio_id_fn_t)(phy_type_radio_ctx_t *ctx);
typedef int (*phy_type_radio_getlistandsize_fn_t)(phy_type_radio_ctx_t *ctx,
	phyradregs_list_t **phyreglist, uint16 *phyreglist_sz);
typedef int (*phy_type_radio_dump_fn_t)(phy_type_radio_ctx_t *ctx, struct bcmstrbuf *b);
typedef bool (*phy_type_radio_check_pll_lock)(phy_type_radio_ctx_t *ctx);
typedef int (*phy_type_radio_get_valid_chanvec_fn_t)(phy_type_radio_ctx_t *ctx,
	chanspec_band_t band, chanvec_t *valid_chans);

typedef struct {
	/* switch radio on/off */
	phy_type_radio_switch_fn_t ctrl;
	/* turn radio on */
	phy_type_radio_on_fn_t on;
	/* turn radio off when switching band */
	phy_type_radio_bandx_fn_t bandx;
	/* turn radio off when initializing band */
	phy_type_radio_init_fn_t init;
	/* query radioid */
	phy_type_radio_id_fn_t id;
	/* get register list and size */
	phy_type_radio_getlistandsize_fn_t getlistandsize;
	/* dump */
	phy_type_radio_dump_fn_t dump;
	/* check pll lock */
	phy_type_radio_check_pll_lock pll_lock;
	/* get valid channels */
	phy_type_radio_get_valid_chanvec_fn_t get_valid_chanvec;
	/* context */
	phy_type_radio_ctx_t *ctx;
} phy_type_radio_fns_t;

/*
 * Register/unregister PHY type implementation to the RADIO control module.
 *
 * It returns BCME_XXXX.
 */
int phy_radio_register_impl(phy_radio_info_t *ri, phy_type_radio_fns_t *fns);
void phy_radio_unregister_impl(phy_radio_info_t *ri);

int phy_radio_add_valid_20MHz_chan(phy_radio_info_t *ri, uint16 perband_radiorev,
	chanspec_band_t band, uint16 freq, uint8 chan, chanvec_t *valid_chans);

#endif /* _phy_type_radio_h_ */
