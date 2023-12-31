/*
 * Wrapper to scb rate selection algorithm of Broadcom
 * algorithm of Broadcom 802.11b DCF-only Networking Adapter.
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
 *
 * $Id: wlc_scb_ratesel.h 795829 2021-02-17 00:10:40Z $
 */

#ifndef	_WLC_SCB_RATESEL_H_
#define	_WLC_SCB_RATESEL_H_

#include <wlc_rate_sel.h>

#define WME_MAX_AC(wlc, scb) ((WME_PER_AC_MAXRATE_ENAB((wlc)->pub) && SCB_WME(scb)) ? \
				AC_COUNT : 1)

extern wlc_ratesel_info_t *wlc_scb_ratesel_attach(wlc_info_t *wlc);
extern void wlc_scb_ratesel_detach(wlc_ratesel_info_t *wrsi);

#if defined(BCMDBG)
extern int wlc_scb_ratesel_scbdump(void *ctx, struct scb *scb, struct bcmstrbuf *b);
extern int wlc_scb_ratesel_get_fixrate(void *ctx, struct scb *scb, struct bcmstrbuf *b);
extern int wlc_scb_ratesel_set_fixrate(void *ctx, struct scb *scb, int ac, uint8 val);
#endif

/* get primary rate */
extern ratespec_t wlc_scb_ratesel_get_primary(wlc_info_t *wlc, struct scb *scb, void *p);

/* Cleanup raw ratespec from rate module */
extern ratespec_t wlc_scb_ratesel_rspec_cleanup(wlc_info_t *wlc, struct scb *scb,
	wlcband_t *scbband, ratespec_t rspec, bool rspec_overide);

/* select transmit rate given per-scb state */
extern bool wlc_scb_ratesel_gettxrate(wlc_ratesel_info_t *wrsi, struct scb *scb,
	uint16 *frameid, ratesel_txparams_t *cur_rate, uint16 *flags);
extern ratespec_t wlc_scb_ratesel_get_opstats(wlc_ratesel_info_t *wrsi,
	struct scb *scb, uint8 ac, uint16 flags);

/* update per-scb state upon received tx status */
extern void wlc_scb_ratesel_upd_txstatus_normalack(wlc_ratesel_info_t *wrsi, struct scb *scb,
	tx_status_t *txs, uint16 sfbl, uint16 lfbl, uint8 mcs,
	bool sgi, bool fbr, uint8 ac);

/* change the throughput-based algo parameters upon ACI mitigation state change */
extern void wlc_scb_ratesel_aci_change(wlc_ratesel_info_t *wrsi, bool aci_state);

/* update per-scb state upon received tx status for ampdu */
extern void wlc_scb_ratesel_upd_txs_blockack(wlc_ratesel_info_t *wrsi, struct scb *scb,
	tx_status_t *txs, uint8 suc_mpdu, uint8 tot_mpdu,
	bool ba_lost, uint8 retry, uint8 fb_lim, bool tx_error,
	uint8 mcs, bool sgi, uint8 ac);

extern void wlc_scb_ratesel_upd_txs_ampdu(wlc_ratesel_info_t *wrsi, struct scb *scb,
	ratesel_txs_t *rs_txs, tx_status_t *txs, uint16 flags);

#if defined(WL_MU_TX)
extern void wlc_scb_ratesel_init_rcb_itxs(wlc_ratesel_info_t *wrsi, struct scb *scb, uint8 ac);
extern void wlc_scb_ratesel_upd_itxs(wlc_ratesel_info_t *wrsi, struct scb *scb,
	ratesel_txs_t *rs_txs, tx_status_t *txs, uint16 flags);
extern void wlc_scb_ratesel_upd_txs_trig(wlc_ratesel_info_t *wrsi, struct scb *scb,
	ratesel_tgtxs_t *tgtxs, tx_status_t *txs);
extern ratespec_t wlc_scb_ratesel_get_ulrt_rspec(wlc_ratesel_info_t *wrsi,
	struct scb *scb, uint8 rtidx);
extern uint8 wlc_scb_ratesel_get_ulrt_trssi(wlc_ratesel_info_t *wrsi, struct scb *scb);
extern void wlc_scb_ratesel_restore_txrspec(wlc_ratesel_info_t *wrsi, struct scb *scb);
extern bool wlc_scb_ratesel_is_start_sp(wlc_ratesel_info_t *wrsi, struct scb *scb);
extern uint32 wlc_scb_ratesel_get_muboost(wlc_ratesel_info_t *wrsi, struct scb *scb, uint8 ac,
	uint8* num_users);
#if defined(WL_ULRT_DRVR)
extern uint16 wlc_scb_ratesel_get_ulrt_txr(wlc_ratesel_info_t *wrsi, struct scb *scb, uint8 rtidx);
#if defined(BCMDBG)
extern void wlc_scb_ratesel_ulrt_dump_clr(wlc_ratesel_info_t *wrsi, struct scb *scb);
#else
#define wlc_scb_ratesel_ulrt_dump_clr(a, b)     do {} while (0)
#endif
#endif /* WL_ULRT_DRVR */
#endif /* defined(WL_MU_TX) */

/* update rate_sel if a PPDU (ampdu or a reg pkt) is created with probe values */
extern void wlc_scb_ratesel_probe_ready(wlc_ratesel_info_t *wrsi, struct scb *scb,
	uint16 frameid, bool is_ampdu, uint8 ampdu_txretry, uint8 ac);

/* get the fallback rate of the specified mcs rate */
extern ratespec_t wlc_scb_ratesel_getmcsfbr(wlc_ratesel_info_t *wrsi, struct scb *scb,
	uint8 ac, uint8 mcs);
extern bool wlc_scb_ratesel_sync(wlc_ratesel_info_t *wrsi, struct scb *scb, uint8 ac,
	uint now, int rssi);
extern bool wlc_scb_ratesel_minrate(wlc_ratesel_info_t *wrsi, struct scb *scb, tx_status_t *txs,
	uint8 ac);

extern void wlc_scb_ratesel_init(wlc_info_t *wlc, struct scb *scb);
extern void wlc_scb_ratesel_init_all(struct wlc_info *wlc);
extern void wlc_scb_ratesel_init_bss(wlc_info_t *wlc, wlc_bsscfg_t *cfg);
extern void wlc_scb_ratesel_rfbr(wlc_ratesel_info_t *wrsi, struct scb *scb, uint8 ac);
extern void wlc_scb_ratesel_rfbr_bss(wlc_info_t *wlc, wlc_bsscfg_t *cfg);

#ifdef WL_LPC
/* External functions used by wlc_power_sel.c */
void wlc_scb_ratesel_get_info(wlc_ratesel_info_t *wrsi, struct scb *scb, uint8 ac,
	uint8 rate_stab_thresh, uint32 *new_rate_kbps, bool *rate_stable,
	rate_lcb_info_t *lcb_info);
void wlc_scb_ratesel_reset_vals(wlc_ratesel_info_t *wrsi, struct scb *scb, uint8 ac);
#endif /* WL_LPC */
void wlc_scb_ratesel_clr_cache(wlc_ratesel_info_t *wrsi, struct scb *scb,  uint8 ac);

#ifdef WLC_DTPC
extern uint8 wlc_scb_ratesel_change_epoch(wlc_ratesel_info_t *wrsi, struct scb *scb);
#endif /* WLC_DTPC */

/* Update CLM enabled rates bitmap if condition has changed (e.g. OLPC kicked in) */
extern void wlc_scb_ratesel_ppr_upd(wlc_info_t *wlc);

#define WLC_RATE_FRAG_G_PROTECT		WLC_RATE_11M	/* for fragments in g mode protection */
#define WLC_RATE_FRAG_N_PROTECT		WLC_RATE_24M	/* for fragments in n mode protection */

#if defined(WLC_DTPC)
/* get ratesel control block pointer */
extern rcb_t *wlc_scb_ratesel_getrcb(wlc_info_t *wlc, struct scb *scb, uint ac);
#endif /* WLC_DTPC */

extern void wlc_scb_ratesel_get_ratecap(wlc_ratesel_info_t * wrsi, struct scb *scb, uint8 *sgi,
	uint16 mcs_bitmap[], uint8 ac);
extern uint8 wlc_scb_ratesel_get_link_bw(wlc_info_t *wlc, struct scb *scb);

extern uint8 wlc_scb_ratesel_get_ulmu_link_bw(wlc_info_t *wlc, struct scb *scb);
#endif	/* _WLC_SCB_RATESEL_H_ */
