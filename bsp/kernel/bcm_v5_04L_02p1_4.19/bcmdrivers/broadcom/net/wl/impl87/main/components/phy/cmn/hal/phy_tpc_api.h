/*
 * TxPowerCtrl module public interface (to MAC driver).
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
 * $Id: phy_tpc_api.h 793449 2020-11-27 04:09:11Z $
 */

#ifndef _phy_tpc_api_h_
#define _phy_tpc_api_h_

#include <typedefs.h>
#include <phy_api.h>

int32 phy_tpc_get_min_power_limit(phy_info_t *pi);
uint8 phy_tpc_get_target_min(phy_info_t *pi);
uint8 phy_tpc_get_target_max(phy_info_t *pi);
uint8 phy_tpc_get_power_backoff(phy_info_t *pi);
bool phy_tpc_ipa_ison(phy_info_t *pi);
#ifdef BCMDBG
void phy_tpc_dump_txpower_limits(phy_info_t *pi, ppr_t* txpwr);
#endif /* BCM_DBG */
void phy_tpc_set_txpower_hw_ctrl(phy_info_t *pi, bool hwpwrctrl);

/*
 * Compute power control cap.
 */
#ifdef WLOLPC
int8 wlc_phy_calc_ppr_pwr_cap(phy_info_t *pi, uint8 core);
void wlc_phy_update_olpc_cal(phy_info_t *pi, bool set, bool dbg);
#endif
void wlc_phy_txpower_limit_set(phy_info_t *pi, ppr_t* txpwr, chanspec_t chanspec);
#ifdef FCC_PWR_LIMIT_2G
void wlc_phy_prev_chanspec_set(phy_info_t *pi, chanspec_t prev_chanspec);
#endif /* FCC_PWR_LIMIT_2G */
int wlc_phy_txpower_set(phy_info_t *pi, int8 qdbm, bool override, ppr_t *reg_pwr);
void wlc_phy_txpower_recalc_target(phy_info_t *pi, ppr_t *txpwr_reg, ppr_t *txpwr_targets);
int wlc_phy_txpower_get_current(phy_info_t *pi, ppr_t *reg_pwr, phy_tx_power_t *power);
#ifdef WL11AX
int wlc_phy_txpower_get_txctrl(phy_info_t *pi, phy_tx_power_t *power);
void wlc_phy_set_ru_power_limits(phy_info_t *pi, ppr_ru_t *ru_reg_pwr);
#ifdef WL_EXPORT_CURPOWER
void phy_tpc_get_he_tb_board_limits(phy_info_t *pi, ppr_ru_t **board_he_txpwr_limit);
void phy_tpc_get_he_tb_clm_limits(phy_info_t *pi, ppr_ru_t **clm_he_txpwr_limit);
void phy_tpc_get_he_tb_tgt_limits(phy_info_t *pi, ppr_ru_t **final_he_txpwr_limit);
#endif /* WL_EXPORT_CURPOWER */
#endif /* WL11AX */
void wlc_phy_get_tx_power_offset(phy_info_t *pi, ppr_t *txpwr_offset);
void wlc_phy_get_ppr_board_limits(phy_info_t *pi, ppr_t *ppr_board_limits);
void wlc_phy_txpower_sromlimit(phy_info_t *pi, chanspec_t chanspec, uint8 *min_pwr,
	ppr_t *max_pwr, uint8 core);
/* set/get FCC power limit */
#ifdef FCC_PWR_LIMIT_2G
bool wlc_phy_fcc_pwr_limit_get(phy_info_t *pi);
void wlc_phy_fcc_pwr_limit_set(phy_info_t *pi, bool enable);
#endif /* FCC_PWR_LIMIT_2G */
#ifdef WL_SAR_SIMPLE_CONTROL
void wlc_phy_dynamic_sarctrl_set(phy_info_t *pi, bool isctrlon);
#if (MAX_RSDB_MAC_NUM > 1)
int wlc_phy_is_rsdb_mode(phy_info_t *pi);
#endif /* MAX_RSDB_MAC_NUM */
#endif /* WL_SAR_SIMPLE_CONTROL */
#ifdef WL_SARLIMIT
void wlc_phy_sar_limit_set(phy_info_t *pi, uint32 int_val);
#endif /* WL_SARLIMIT */
bool wlc_phy_txpower_hw_ctrl_get(phy_info_t *pi);

int wlc_phy_txpower_core_offset_set(phy_info_t *pi,
	struct phy_txcore_pwr_offsets *offsets);
int wlc_phy_txpower_core_offset_get(phy_info_t *pi,
	struct phy_txcore_pwr_offsets *offsets);
void wlc_phy_txpwr_percent_set(phy_info_t *pi, uint8 txpwr_percent);
void wlc_phy_txpwr_degrade(phy_info_t *pi, uint8 txpwr_degrade);

int wlc_phy_txpower_get(phy_info_t *pi, int8 *qdbm, bool *override);
int	wlc_phy_neg_txpower_set(phy_info_t *pi, uint qdbm);
extern void wlc_phy_set_country(phy_info_t *pi, const char* ccode_ptr);
extern int8 wlc_phy_maxtxpwr_lowlimit(phy_info_t *pi);

#ifdef WLTXPWR_CACHE
tx_pwr_cache_entry_t* phy_tpc_get_txpwr_cache(phy_info_t *pi);
#if !defined(WLTXPWR_CACHE_PHY_ONLY)
void phy_tpc_set_txpwr_cache(phy_info_t *pi, void* cacheptr);
#endif
#endif	/* WLTXPWR_CACHE */

/* Tx backoff config */
#define TXPWRBACKOFF_RANGE_LO		0
#define TXPWRBACKOFF_RANGE_HI		20

void phy_tpc_get_target_max_per_core(phy_info_t *pi, phy_tx_targets_per_core_t *max_per_core);

typedef struct phy_txpwrbackoff_tvpm_info {
	bool  enable; /* Dynamic txbackoff control */
	int16 vbat_tempsense_pwrbackoff;
} phy_txpwrbackoff_info_t;

int phy_tpc_txbackoff_set(phy_info_t *pi, int16 txpwr_offset);
bool phy_tpc_txbackoff_enable(phy_info_t *pi, bool enable);
bool phy_tpc_txbackoff_is_enabled(phy_info_t *pi);
int16 phy_tpc_get_vtherm_pwrbackoff(phy_info_t *pi);
int phy_tpc_txbackoff_dump(phy_info_t *pi, phy_txpwrbackoff_info_t *pdata);

#endif /* _phy_tpc_api_h_ */
