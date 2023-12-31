
/*
 * ACPHY TxPowerCtrl module implementation
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
 * $Id: phy_ac_tpc.c 810784 2022-04-13 19:59:25Z $
 */
#include <phy_cfg.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <bcmutils.h>
#include <phy_dbg.h>
#include <phy_mem.h>
#include <phy_wd.h>
#include "phy_type_tpc.h"
#include <phy_ac.h>
#include <phy_ac_tpc.h>
#include <phy_ac_tbl.h>
#include <phy_ac_temp.h>
#include <phy_ac_txiqlocal.h>
#include <phy_utils_reg.h>
#include <phy_ac_info.h>
#include <wlc_phyreg_ac.h>
#include <wlc_phytbl_ac.h>
#include <phy_papdcal.h>
#include <phy_tpc_api.h>
#include <phy_tpc.h>
#ifdef WL_SAR_SIMPLE_CONTROL
#endif /* WL_SAR_SIMPLE_CONTROL */
#include <phy_utils_var.h>
#include <phy_rstr.h>
#ifdef WLC_SW_DIVERSITY
#include "phy_ac_antdiv.h"
#endif
#ifdef WLC_TXPWRCAP
#include <phy_txpwrcap.h>
#include <phy_type_txpwrcap.h>
#endif
#include <phy_stf.h>

#ifdef WL_MU_TX
#include <wlc_phy_shim.h>
#endif

#ifdef WLC_TXSHAPER
#include <phy_ac_txss.h>
#endif

#ifndef ALL_NEW_PHY_MOD
/* < TODO: all these are going away... */
#include <wlc_phy_hal.h>

/* TODO: all these are going away... > */
#endif

#include <bcmdevs.h>

#define PDOFFSET(pi, nvramstrng, core) ((uint16)(PHY_GETINTVAR((pi), \
    (nvramstrng)) >> (5 * (core))) & 0x1f)

#define ESTPWRLUTS_TBL_LEN        128
#define MHF6_HWPPR_EN 0x0004

#if BAND6G
#define PHY_HAS_6G(pi)    (pi->u.pi_acphy->phy_caps & PHY_CAP_SUP_6G)
#else
#define PHY_HAS_6G(pi)    0
#endif

#define HWPPR_PERRATETBL_OFFSET        107

/* This is expected to increase with 320MHz chips */
#define TPC_ESTPWRTBLS_MAX        4

/* #ifdef PREASSOC_PWRCTRL */
typedef struct phy_pwr_ctrl_save_acphy {
    bool status_idx_carry_2g[PHY_CORE_MAX];
    bool status_idx_carry_5g[PHY_CORE_MAX];
    uint8 status_idx_2g[PHY_CORE_MAX];
    uint8 status_idx_5g[PHY_CORE_MAX];
    uint16 last_chan_stored_2g;
    uint16 last_chan_stored_5g;
    int8   pwr_qdbm_2g[PHY_CORE_MAX];
    int8   pwr_qdbm_5g[PHY_CORE_MAX];
    bool   stored_not_restored_2g[PHY_CORE_MAX];
    bool   stored_not_restored_5g[PHY_CORE_MAX];
} phy_pwr_ctrl_s;
/* #endif */  /* PREASSOC_PWRCTRL */

/* Tx Backoff configuration */
typedef struct phy_txpwrbackoff {
    int16 vbat_tempsense_pwrbackoff;
    bool  enable; /* Dynamic txbackoff control */
} phy_txpwrbackoff_t;

#ifdef WL11AX
typedef enum {
    TXCTRL_FRAME_TYPE = 0,
    TXCTRL_HE_FORMAT,
    TXCTRL_NOT_SOUNDING,
    TXCTRL_PREAMBLE,
    TXCTRL_MCS_NSS,
    TXCTRL_STBC,
    TXCTRL_PKTBW,
    TXCTRL_SUBBAND_LOCATION,
    TXCTRL_PARTIAL_OFDMA_SUBBAND,
    TXCTRL_DYNBW_PRESENT,
    TXCTRL_DYNBW_MOD,
    TXCTRL_MU,
    TXCTRL_COREMASK,
    TXCTRL_ANTCFG,
    TXCTRL_SCRAMBLER,
    TXCTRL_CFO_COMP_VAL,
    TXCTRL_CFO_COMP_EN,
    TXCTRL_HAS_TRIGGER,
    TXCTRL_N_PWR,
    TXCTRL_POWER_OFFSET_0
} phy_txctrlwd_common;

typedef enum {
    TXCTRL_BFM_MU,
    TXCTRL_USRIDX_MU,
    TXCTRL_RUIDX_MU,
    TXCTRL_MCS_NSS_MU,
    TXCTRL_LDPC_MU,
    TXCTRL_AID_MU
} phy_txctrlwd_mu;

typedef enum {
    HESIGEXT_CPLTFSIZE,
    HESIGEXT_HELTF_MASK,
    HESIGEXT_NUM_HELTF,
    HESIGEXT_LDPC_EXTRA,
    HESIGEXT_AFACTOR,
    HESIGEXT_DISAMB,
    HESIGEXT_DOPPLER,
    HESIGEXT_CODING,
    HESIGEXT_MCS,
    HESIGEXT_DCM,
    HESIGEXT_NSSSTART,
    HESIGEXT_NSS,
    HESIGEXT_SIZE
} phy_txctrlwd_hesigext;

typedef struct phy_ac_txctrlwd_info {
    uint8 common_info[TXCTRL_POWER_OFFSET_0];
    uint8 pwr_offset[4];
    uint8 n_usr;
    uint8 bfm_mu[16];
    uint8 usridx_mu[16];
    uint8 ruidx_mu[16];
    uint8 mcs_nss_mu[16];
    uint8 ldpc_mu[16];
    uint16 aid_mu[16];
    uint8 rl_backoff;
    uint8 hesig_ext[HESIGEXT_SIZE];
} phy_ac_txctrlwd_info_t;
#endif /* WL11AX */

/* module private states */
struct phy_ac_tpc_info {
    phy_info_t *pi;
    phy_ac_info_t *aci;
    phy_tpc_info_t *ti;
    phy_pwr_ctrl_s *pwr_ctrl_save;
    /* std params */
    uint16    offset_targetpwr; /* target offset power (in qDb) */
    int8    txpwr_offset[PHY_CORE_MAX];            /* qdBm signed offset for tx pwr */
    uint8    txpwrindex_hw_save[PHY_CORE_MAX];    /* txpwr start index for hwpwrctrl */
    uint8    txpwrindex_cck_hw_save[PHY_CORE_MAX]; /* txpwr start index for hwpwrctrl */
    uint8    txpwrindexlimit[NUM_TXPWRINDEX_LIM];
    uint8    txpwrindex_hw_save_chan; /* current chan for which baseindex is saved */
    uint8    txpwr_damping_factor_set;
    bool    olpc_dbg_mode;
    bool    olpc_dbg_mode_caldone;
    bool    olpc_mode_caldone;
    bool _txpwrbackoff;
    phy_txpwrbackoff_t* phy_txpwrbackoff;
#ifdef WL11AX
    phy_ac_txctrlwd_info_t* phy_ac_txctrlwd_info;
#endif /* WL11AX */
    int    dbg_phy_nvram_err;
    const char *dbg_phy_nvram_rstr;
    uint8   txpwr_cap[PHY_CORE_MAX];        /* per-core txpwr cap */
    bool   txpwr_cap_status;        /* txpwr cap status */
/* add other variable size variables here at the end */
};

/* Tx Pwr backoff based on vbat/tempsense Support */
#ifdef TXPWRBACKOFF
#if defined(ROM_ENAB_RUNTIME_CHECK)
    #define TXPWRBACKOFF_ENAB(tpci)   ((tpci)->_txpwrbackoff)
#elif defined(TXPWRBACKOFF_DISABLED)
    #define TXPWRBACKOFF_ENAB(tpci)   0
#else
    #define TXPWRBACKOFF_ENAB(tpci)   ((tpci)->_txpwrbackoff)
#endif
#else
    #define TXPWRBACKOFF_ENAB(tpci)   0
#endif    /* TXPWRBACKOFF */

#define WL_CHAN_FREQ_RANGE_BW160(band) (((band) >= WL_CHAN_FREQ_RANGE_5G_BAND0_160 &&\
                    (band) <= WL_CHAN_FREQ_RANGE_5G_BAND3_160) ||\
                    ((band) >= WL_CHAN_FREQ_RANGE_5G_BAND4_160 &&\
                    (band) <= WL_CHAN_FREQ_RANGE_5G_BAND6_160))

#define WL_CHAN_FREQ_RANGE_BW80(band) (((band) >= WL_CHAN_FREQ_RANGE_5G_BAND0_80 &&\
                    (band) <= WL_CHAN_FREQ_RANGE_5G_BAND4_80) ||\
                    ((band) >= WL_CHAN_FREQ_RANGE_5G_BAND5_80 &&\
                    (band) <= WL_CHAN_FREQ_RANGE_5G_BAND6_80))

#define WL_CHAN_FREQ_RANGE_BW40(band) (((band) >= WL_CHAN_FREQ_RANGE_2G_40 &&\
                    (band) <= WL_CHAN_FREQ_RANGE_5G_BAND4_40) ||\
                    ((band) >= WL_CHAN_FREQ_RANGE_5G_BAND5_40 &&\
                    (band) <= WL_CHAN_FREQ_RANGE_5G_BAND6_40))

#define WL_CHAN_FREQ_RANGE_BW20(band) (((band) == WL_CHAN_FREQ_RANGE_2G) ||\
                    ((band) >= WL_CHAN_FREQ_RANGE_5G_BAND0 &&\
                    (band) <= WL_CHAN_FREQ_RANGE_5G_BAND4) ||\
                    ((band) >= WL_CHAN_FREQ_RANGE_5G_BAND5 &&\
                    (band) <= WL_CHAN_FREQ_RANGE_5G_BAND6))

#define WL_CHAN_FREQ_RANGE_LT_BAND5(range) ((range) < WL_CHAN_FREQ_RANGE_5G_BAND5)

/* PA params struct to replace loose parameters */
typedef struct acphy_paparams {
    int16 a;
    int16 b;
    int16 c;
    int16 d;
} acphy_paparams_t;

/* Addressing multimode tables */
enum {
    BW0 = 0,
    BW1,
    BW2,
    BW3
};

/* local functions */
static void phy_ac_tpc_std_params_attach(phy_ac_tpc_info_t *info);
static void wlc_phy_txpwrctrl_pwr_setup_acphy(phy_info_t *pi);
static void wlc_phy_txpwrctrl_pwr_setup_srom12_acphy(phy_info_t *pi);
int8 wlc_phy_fittype_srom12_acphy(phy_info_t *pi);
#if BAND5G || BAND6G
static uint8 wlc_phy_num_paparams_acphy(phy_info_t *pi, bool force5g6g);
#endif /* BAND5G || BAND6G */
static uint8 wlc_phy_percent_to_qdb(uint8 percent);
#ifdef WL11AX
static uint16 wlc_phy_ten_log_ten(uint16 n_sc_clm, uint16 n_sc_dl);
static uint16 wlc_phy_64bit_shift_parse(uint64 input, uint8 shift, uint32 bitmap);
static void wlc_phy_txctrl_hesigext_parse(phy_info_t *pi,
        uint64 *val, phy_ac_txctrlwd_info_t *txctrlwd);
#endif /* WL11AX */
static void wlc_phy_get_srom12_pdoffset_acphy(phy_info_t *pi, int8 *pdoffs, int8 *pdoffs_160);
static void phy_ac_tpc_get_paparams_percore_srom12(phy_info_t *pi,
        uint8 chan_freq_range, acphy_paparams_t *params, uint8 core,
        uint8 cck);
static void phy_ac_tpc_get_paparams_srom12(phy_info_t *pi,
        uint8 chan_freq_range, acphy_paparams_t *params);
static void phy_ac_tpc_get_cck_paparams_srom18(phy_info_t *pi,
        acphy_paparams_t *params);
static void phy_ac_tpc_get_paparams_80p80_srom12(phy_info_t *pi,
        uint8 *chan_freqs, acphy_paparams_t *params);
static int32 phy_ac_tpc_get_estpwrlut_srom12(acphy_paparams_t *params,
        uint8 pa_fittype, int32 idx);
static void
phy_ac_tpc_get_paparams_percore_multimode_srom12(phy_info_t *pi, uint8 chan_freq_range,
        acphy_paparams_t params[][PHY_CORE_MAX], uint8 core, uint8 cck);
#if BAND6G
static void
phy_ac_tpc_get_5gext_paparams_percore_multimode_srom18(phy_info_t *pi, uint8 chan_freq_range,
        acphy_paparams_t params[][PHY_CORE_MAX], uint8 core);
#endif /* BAND6G */
static void wlc_phy_get_tssi_floor_acphy(phy_info_t *pi, int16 *floor);
static uint32 wlc_phy_pdoffset_cal_acphy(uint32 pdoffs, uint16 pdoffset, uint8 band, uint8 core);
static bool wlc_phy_txpwrctrl_ison_acphy(phy_info_t *pi);
static uint8 wlc_phy_set_txpwr_clamp_acphy(phy_info_t *pi, uint8 core);
static void wlc_phy_txpower_recalc_target_acphy(phy_info_t *pi);
#if defined(WL_MU_TX) && defined(WLVASIP)
static void phy_ac_tpc_offload_ppr_to_svmp(phy_info_t *pi, ppr_t* tx_power_offset, int16 floor_pwr);
#endif
static void phy_ac_tpc_recalc_tgt(phy_type_tpc_ctx_t *ctx);
static void wlc_phy_txpower_recalc_target_ac_big(phy_type_tpc_ctx_t *ctx, ppr_t *tx_pwr_target,
    ppr_t *srom_max_txpwr, ppr_t *reg_txpwr_limit, ppr_t *txpwr_targets);
static void phy_ac_tpc_sromlimit_get(phy_type_tpc_ctx_t *ctx, chanspec_t chanspec, ppr_t *max_pwr,
    uint8 core);
static void wlc_phy_txpwrctrl_setminpwr(phy_info_t *pi);
static bool phy_ac_tpc_hw_ctrl_get(phy_type_tpc_ctx_t *ctx);
static void phy_ac_tpc_set(phy_type_tpc_ctx_t *ctx, ppr_t *reg_pwr);
static void phy_ac_tpc_set_flags(phy_type_tpc_ctx_t *ctx, phy_tx_power_t *power);
static void phy_ac_tpc_set_max(phy_type_tpc_ctx_t *ctx, phy_tx_power_t *power);
static bool phy_ac_tpc_wd(phy_wd_ctx_t *ctx);
static int phy_ac_tpc_init(phy_type_tpc_ctx_t *ctx);
static void phy_ac_fill_estpwrshft_table(phy_info_t *pi, srom12_pwrdet_t *pwrdet, uint8 chan_freq,
        uint8 shfttbl_len, uint32 *shfttblval);
#if defined(PHYCAL_CACHING)
#if (defined(WLOLPC) && !defined(WLOLPC_DISABLED))
static void phy_ac_tpc_update_olpc_cal(phy_type_tpc_ctx_t *tpc_ctx, bool set, bool dbg);
#endif /* WLOLPC && !defined(WLOLPC_DISABLED) */
#endif /* PHYCAL_CACHING */
#ifdef FCC_PWR_LIMIT_2G
static void wlc_phy_fcc_pwr_limit_set_acphy(phy_type_tpc_ctx_t *ctx, bool enable);
#endif /* FCC_PWR_LIMIT_2G */
#if defined(WL_SARLIMIT) || defined(WL_SAR_SIMPLE_CONTROL)
static void wlc_phy_set_sarlimit_acphy(phy_type_tpc_ctx_t *ctx);
#endif /* defined(WL_SARLIMIT) || defined(WL_SAR_SIMPLE_CONTROL) */
#ifdef WL_SAR_SIMPLE_CONTROL
static void wlc_phy_nvram_dynamicsarctrl_read(phy_info_t *pi);
static bool wlc_phy_isenabled_dynamic_sarctrl(phy_info_t *pi);
#endif /* WL_SAR_SIMPLE_CONTROL */
static void wlc_acphy_avvmid_txcal(phy_type_tpc_ctx_t *ctx,
    wlc_phy_avvmid_txcal_t *avvmidinfo, bool set);

#ifdef PREASSOC_PWRCTRL
static void wlc_phy_store_tx_pwrctrl_setting_acphy(phy_type_tpc_ctx_t *ctx,
    chanspec_t previous_channel);
static void phy_ac_tpc_shortwindow_upd(phy_type_tpc_ctx_t *ctx, bool new_channel);
static void wlc_phy_pwrctrl_shortwindow_upd_acphy(phy_info_t *pi, bool shortterm);
#endif /* PREASSOC_PWRCTRL */

static uint8 wlc_phy_txpwrctrl_get_target_acphy(phy_info_t *pi, uint8 core);

#ifdef POWPERCHANNL
static void wlc_phy_tx_target_pwr_per_channel_set_acphy(phy_info_t *pi);
#endif /* POWPERCHANNL */
#if BAND5G
static void wlc_phy_txpwr_srom11_read_5Gsubbands(phy_info_t *pi, srom11_pwrdet_t * pwrdet,
    uint8 subband, bool update_rsdb_core1_params, uint8 ant);
#endif /* BAND5G */
static void wlc_phy_txpwr_srom11_read_pwrdet(phy_info_t *pi, srom11_pwrdet_t * pwrdet,
    uint8 param, uint8 band, uint8 offset,  const char * name);
static bool BCMATTACHFN(wlc_phy_txpwr_srom11_read)(phy_type_tpc_ctx_t *ctx);
static bool BCMATTACHFN(wlc_phy_txpwr_srom12_read)(phy_type_tpc_ctx_t *ctx);
#if BAND6G
static void phy_ac_tpc_get_5gext_paparams_percore_srom18(phy_info_t *pi,
        uint8 chan_freq_range, acphy_paparams_t *params, uint8 core);
static void phy_ac_tpc_get_5gext_paparams_srom18(phy_info_t *pi,
        uint8 chan_freq_range, acphy_paparams_t *params);
static void phy_ac_fill_estpwrshft_table_5gext(phy_info_t *pi, srom18_5gext_pwrdet_t *pwrdet_5gext,
        uint8 chan_freq, uint8 shfttbl_len, uint32 *shfttblval);
static bool BCMATTACHFN(wlc_phy_txpwr_5gext_srom18_read)(phy_type_tpc_ctx_t *ctx);
#endif /* BAND6G */
#if defined(WL_EAP_BOARD_RF_5G_FILTER)
static bool BCMATTACHFN(wl_eap_txpwr_srom12_read_filtered)(phy_info_t *pi, int b, int i, int j);
#endif /* WL_EAP_BOARD_RF_5G_FILTER */

static int phy_ac_tpc_txpower_core_offset_set(phy_type_tpc_ctx_t *ctx,
    struct phy_txcore_pwr_offsets *offsets);
static int phy_ac_tpc_txpower_core_offset_get(phy_type_tpc_ctx_t *ctx,
    struct phy_txcore_pwr_offsets *offsets);

static int phy_ac_tpc_nvram_attach(phy_ac_tpc_info_t *tpci);

static void wlc_mac_register_init_for_hwppr(phy_info_t *pi);
static uint16 wlc_bitor_reg(uint16 reg1, uint16 reg2, int lshft_reg2);

/* External functions */
#ifdef POWPERCHANNL
void wlc_phy_tx_target_pwr_per_channel_decide_run_acphy(phy_info_t *pi);
void BCMATTACHFN(wlc_phy_tx_target_pwr_per_channel_limit_acphy)(phy_info_t *pi);
#endif /* POWPERCHANNL */
uint8 wlc_phy_ac_set_tssi_params_legacy(phy_info_t *pi);
uint8 wlc_phy_ac_set_tssi_params_maj36(phy_info_t *pi);
uint8 wlc_phy_ac_set_tssi_params_majrev128(phy_info_t *pi);

static void phy_ac_tpc_ipa_upd(phy_type_tpc_ctx_t *ctx);
#if defined(WLTEST)
static int phy_ac_tpc_set_pavars(phy_type_tpc_ctx_t *ctx, void* a, void* p);
static int phy_ac_tpc_get_pavars(phy_type_tpc_ctx_t *ctx, void* a, void* p);
static int phy_ac_tpc_txpower_get_instant(phy_type_tpc_ctx_t *ctx, void *pwr);
static int phy_ac_tpc_txpower_get_instant_percore(phy_type_tpc_ctx_t *ctx, void *pwr);
#endif
static int8 phy_ac_tpc_get_maxtxpwr_lowlimit(phy_type_tpc_ctx_t *ctx);
#ifdef RADIO_HEALTH_CHECK
static phy_crash_reason_t phy_ac_tpc_healthcheck_baseindex(phy_type_tpc_ctx_t *ctx);
#endif /* RADIO_HEALTH_CHECK */

static int
phy_ac_tpc_get_est_pout(phy_type_tpc_ctx_t *ctx,
    uint8* est_Pout, uint8* est_Pout_adj, uint8* est_Pout_cck);
#ifdef WL11AX
static int
phy_ac_tpc_get_txctrl_mu(phy_type_tpc_ctx_t *ctx, phy_txctrl_info_t* txctrlwd_info);
#endif /* WL11AX */
#ifdef TXPWRBACKOFF
#if defined(WLTXPWR_CACHE)
static bool
phy_ac_ppr_check_vt_backoff(phy_type_tpc_ctx_t *ctx, chanspec_t chspec);
#endif /* WLTXPWR_CACHE */

int16 phy_ac_tpc_get_vt_pwrbackoff(phy_type_tpc_ctx_t *ctx);
bool phy_ac_tpc_enable_power_backoff(phy_type_tpc_ctx_t *ctx, bool enable);
bool phy_ac_tpc_is_enabled_power_backoff(phy_type_tpc_ctx_t *ctx);
int phy_ac_tpc_dump_power_backoff(phy_type_tpc_ctx_t *ct, phy_txpwrbackoff_info_t *pdata);
int phy_ac_tpc_txbackoff_set(phy_type_tpc_ctx_t *ct, int16 txpwr_offset);
#endif /* TXPWRBACKOFF */

int wlc_phy_set_hwppr_bl_tbl(phy_info_t *pi, ppr_t *board_txpwr_limit,
        int8 max_pwr, chanspec_t chanspec);
int wlc_phy_set_hwppr_rl_tbl(phy_info_t *pi, ppr_t *reg_txpwr_limit,
        int8 max_pwr, chanspec_t chanspec);
void wlc_phy_hwppr_en(phy_info_t *pi, bool en);
bool wlc_phy_hwppr_ison(phy_info_t *pi);
void wlc_phy_dump_hwppr_tbl(phy_info_t *pi);
void wlc_phy_zero_hwppr_tbl(phy_info_t *pi);

static const uint32 lowrate_tssi_delay_set47_1[17][2] = {
    {470,470},  /* pdet_id = 0 */
    {470,470},  /* pdet_id = 1 */
    {470,470},  /* pdet_id = 2 */
    {470,470},  /* pdet_id = 3 */
    {440,470},  /* pdet_id = 4 */
    {440,470},  /* pdet_id = 5 */
    {440,470},  /* pdet_id = 6 */
    {440,440},  /* pdet_id = 7 */
    {470,470},  /* pdet_id = 8 */
    {470,470},  /* pdet_id = 9 */
    {380,380},  /* pdet_id = 10 */
    {350,350},  /* pdet_id = 11 */
    {380,380},  /* pdet_id = 12 */
    {470,470},  /* pdet_id = 13 */
    {420,420},  /* pdet_id = 14 */
    {420,420},  /* pdet_id = 15 */
    {380,380},  /* pdet_id = 16 */
};

static const uint32 lowrate_tssi_delay_set129[8][2] = {
    {750,750},  /* pdet_id = 0 */
    {750,750},  /* pdet_id = 1 */
    {800,750},  /* pdet_id = 2 */
    {750,750},  /* pdet_id = 3 */
    {750,750},  /* pdet_id = 4 */
    {900,820},  /* pdet_id = 5 */
    {750,750},  /* pdet_id = 6 */
    {750,750},  /* pdet_id = 7 */
};

static const uint32 lowrate_tssi_delay_set130[18][3] = {
    /* 2G/5G/5G-80MHz */
    {440,440,400},  /* pdet_id = 0 */
    {440,440,400},  /* pdet_id = 1 */
    {440,440,400},  /* pdet_id = 2 */
    {440,440,400},  /* pdet_id = 3 */
    {440,440,400},  /* pdet_id = 4 */
    {440,440,400},  /* pdet_id = 5 */
    {440,440,400},  /* pdet_id = 6 */
    {440,440,400},  /* pdet_id = 7 */
    {440,440,400},  /* pdet_id = 8 */
    {440,440,400},  /* pdet_id = 9 */
    {440,440,400},  /* pdet_id = 10 */
    {440,440,400},  /* pdet_id = 11 */
    {440,440,400},  /* pdet_id = 12 */
    {440,440,400},  /* pdet_id = 13 */
    {440,440,400},  /* pdet_id = 14 */
    {440,440,400},  /* pdet_id = 15 */
    {440,440,400},  /* pdet_id = 16 */
    {440,440,400},  /* pdet_id = 17 */
};

#ifdef WL11AX
static const  uint32  txctrlwd_tbl[TXCTRL_POWER_OFFSET_0+1][2] = {
    {16, 0x7}, /* frame_type */
    {19, 0x3}, /* he_format */
    {22, 0x1}, /* not_sounding */
    {23, 0x1}, /* preamble */
    {24, 0x7f}, /* mcs_nss */
    {31, 0x1}, /* stbc */
    {32, 0x3}, /* pktbw */
    {34, 0x7}, /* subband_location */
    {37, 0xff}, /* partial ofdma subband */
    {45, 0x1}, /* dynBW present */
    {46, 0x1}, /* dynBW mode */
    {47, 0x1}, /* mu */
    {48, 0xff}, /* coremask */
    {56, 0xff}, /* antcfg */
    {64, 0xff}, /* scrambler */
    {72, 0xfffff}, /* cfo_comp_value */
    {92, 0x1}, /* cfo_comp_en */
    {95, 0x1}, /* has_trigger_info */
    {96, 0xf}, /* n_pwr */
    {112, 0xff}, /* power_offset0 */
};

static const  uint32  txctrlwd_tbl_mu[TXCTRL_AID_MU+1][2] = {
    {16, 0xff}, /* bfm_mu */
    {24, 0xff}, /* usridx_mu */
    {32, 0xff}, /* ruidx_mu */
    {40, 0x7f}, /* mcs_nss_mu */
    {47, 0x1}, /* ldpc_mu */
    {48, 0xffff}, /* AID_mu */
};

static const  uint32  txctrlwd_tbl_hesigext[HESIGEXT_SIZE][3] = {
    {5,  8, 0x3}, /* CPLTFSIZE  */
    {5, 10, 0x1}, /* HELTF_MASK */
    {5, 11, 0x7}, /* NUM_HELTF  */
    {5, 15, 0x1}, /* LDPC_EXTRA */
    {5, 16, 0x3}, /* AFACTOR    */
    {5, 18, 0x1}, /* DISAMB     */
    {5, 19, 0x1}, /* DOPPLER    */
    {5, 24, 0x1}, /* CODING     */
    {5, 25, 0xf}, /* MCS        */
    {5, 29, 0x1}, /* DCM        */
    {5, 30, 0x7}, /* NSSSTART   */
    {5, 33, 0x7}, /* NSS        */
};

static const uint16 ruidx_scnum[69]  = {
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, //RU0~RU36
    52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, //RU37-RU52
    106, 106, 106, 106, 106, 106, 106, 106, //RU53-RU60
    242, 242, 242, 242, //RU61-RU64
    484, 484, //RU65-66
    996, //RU67
    1992, //RU68
};

/* 10log10 LUT in qdb */
static const uint16 log10_tbl[20] = {14, 14, 14, 13, 12, 11,
    10, 9, 8, 7, 6, 5, 5, 4, 3, 2, 2, 1, 1, 0};

/* tone numbers for CLM test cases */
/* BW20, BW40, BW80, BW160 */
/* SU, LUB, UB */
static uint16 tone_sc_clm[4][3] = {{242, 182, 106}, {484, 348, 242},
    {996, 752, 484}, {1992, 1480, 996}};
#endif /* WL11AX */

#ifdef PPR_API
static void wlc_phy_txpwr_ppr_bit_ext_mcs8and9(ppr_vht_mcs_rateset_t* vht,
    uint8 msb);
#endif

/* Register/unregister ACPHY specific implementation to common layer. */
phy_ac_tpc_info_t *
BCMATTACHFN(phy_ac_tpc_register_impl)(phy_info_t *pi, phy_ac_info_t *aci, phy_tpc_info_t *ti)
{
    phy_ac_tpc_info_t *info;
    phy_type_tpc_fns_t fns;

    PHY_TRACE(("%s\n", __FUNCTION__));
    BCM_REFERENCE(rstr_offtgpwr);

    /* allocate all storage in once */
    if ((info = phy_malloc(pi, sizeof(phy_ac_tpc_info_t))) == NULL) {
        PHY_ERROR(("%s: phy_malloc failed\n", __FUNCTION__));
        goto fail;
    }
#ifdef PREASSOC_PWRCTRL
    if ((info->pwr_ctrl_save = phy_malloc(pi, sizeof(phy_pwr_ctrl_s))) == NULL) {
        PHY_ERROR(("%s: pwr_ctrl_save malloc failed\n", __FUNCTION__));
        goto fail;
    }
#endif
    if ((info->phy_txpwrbackoff = phy_malloc(pi, sizeof(phy_txpwrbackoff_t))) == NULL) {
        PHY_ERROR(("%s: phy_txpwrbackoff malloc failed\n", __FUNCTION__));
        goto fail;
    }
#ifdef WL11AX
    if ((info->phy_ac_txctrlwd_info = phy_malloc(pi, sizeof(phy_ac_txctrlwd_info_t))) == NULL) {
        PHY_ERROR(("%s: phy_ac_txctrlwd_info malloc failed\n", __FUNCTION__));
        goto fail;
    }
#endif /* WL11AX */
    info->phy_txpwrbackoff->enable = FALSE;
    info->phy_txpwrbackoff->vbat_tempsense_pwrbackoff = 0;
#ifdef TXPWRBACKOFF
    info->_txpwrbackoff = TRUE;
#else
    info->_txpwrbackoff = FALSE;
#endif /* TXPWRBACKOFF */

    info->pi = pi;
    info->aci = aci;
    info->ti = ti;

    /* register watchdog fn */
    if (phy_wd_add_fn(pi->wdi, phy_ac_tpc_wd, info,
                      PHY_WD_PRD_1TICK, PHY_WD_1TICK_TPC, PHY_WD_FLAG_DEF_SKIP) != BCME_OK) {
        PHY_ERROR(("%s: phy_wd_add_fn failed\n", __FUNCTION__));
        goto fail;
    }

    /* Register PHY type specific implementation */
    bzero(&fns, sizeof(fns));
    fns.init = phy_ac_tpc_init;
    fns.recalc = phy_ac_tpc_recalc_tgt;
    fns.recalc_target = wlc_phy_txpower_recalc_target_ac_big;
#ifdef PREASSOC_PWRCTRL
    fns.store_setting = wlc_phy_store_tx_pwrctrl_setting_acphy;
    fns.shortwindow_upd = phy_ac_tpc_shortwindow_upd;
#endif /* PREASSOC_PWRCTRL */
    fns.get_sromlimit = phy_ac_tpc_sromlimit_get;
#ifdef PHYCAL_CACHING
#ifdef WLOLPC
    fns.update_cal = phy_ac_tpc_update_olpc_cal;
#endif /* WLOLPC */
#endif /* PHYCAL_CACHING */
#ifdef FCC_PWR_LIMIT_2G
    fns.set_pwr_limit = wlc_phy_fcc_pwr_limit_set_acphy;
#endif /* FCC_PWR_LIMIT_2G */
#if defined(WL_SARLIMIT) || defined(WL_SAR_SIMPLE_CONTROL)
    fns.set_sarlimit = wlc_phy_set_sarlimit_acphy;
#endif /* defined(WL_SARLIMIT) || defined(WL_SAR_SIMPLE_CONTROL) */
    fns.set_avvmid = wlc_acphy_avvmid_txcal;
    fns.get_hwctrl = phy_ac_tpc_hw_ctrl_get;
    fns.set = phy_ac_tpc_set;
    fns.setflags = phy_ac_tpc_set_flags;
    fns.setmax = phy_ac_tpc_set_max;
    fns.txcorepwroffsetset = phy_ac_tpc_txpower_core_offset_set;
    fns.txcorepwroffsetget = phy_ac_tpc_txpower_core_offset_get;
    fns.ipa_upd = phy_ac_tpc_ipa_upd;
#if defined(WLTEST)
    fns.set_pavars = phy_ac_tpc_set_pavars;
    fns.get_pavars = phy_ac_tpc_get_pavars;
    fns.txpower_get_instant = phy_ac_tpc_txpower_get_instant;
    fns.txpower_get_instant_percore = phy_ac_tpc_txpower_get_instant_percore;
#endif
    fns.get_maxtxpwr_lowlimit = phy_ac_tpc_get_maxtxpwr_lowlimit;
#ifdef RADIO_HEALTH_CHECK
    fns.baseindex = phy_ac_tpc_healthcheck_baseindex;
#endif /* RADIO_HEALTH_CHECK */
    fns.get_est_pout = phy_ac_tpc_get_est_pout;
#ifdef WL11AX
    fns.get_txctrl_mu = phy_ac_tpc_get_txctrl_mu;
#endif /* WL11AX */
#if defined(TXPWRBACKOFF)
#if defined(WLTXPWR_CACHE)
    fns.check_vt = phy_ac_ppr_check_vt_backoff;
#endif /* WLTXPWR_CACHE */
    fns.tvpm_txbackoff_enable = phy_ac_tpc_enable_power_backoff;
    fns.tvpm_txbackoff_is_enabled = phy_ac_tpc_is_enabled_power_backoff;
    fns.tvpm_txbackoff_dump = phy_ac_tpc_dump_power_backoff;
    fns.txbackoff_set = phy_ac_tpc_txbackoff_set;
    fns.get_vt_pwrbackoff = phy_ac_tpc_get_vt_pwrbackoff;
#endif /* TXPWRBACKOFF */

    fns.ctx = info;
    /* set up srom cfg */
    if (phy_ac_tpc_nvram_attach(info) != BCME_OK) {
        goto fail;
    }

    /* update txpwr settings */
    phy_ac_tpc_ipa_upd(info);

#ifdef WL_SAR_SIMPLE_CONTROL
    wlc_phy_nvram_dynamicsarctrl_read(pi);
#endif

    phy_ac_tpc_std_params_attach(info);

    wlc_phy_txpwrctrl_config_acphy(pi);

    /* read srom txpwr limits */
#if BAND6G
    if (SROMREV(pi->sh->sromrev) >= 18 &&
        ((pi->u.pi_acphy->phy_caps & PHY_CAP_SUP_5G) ||
        (pi->u.pi_acphy->phy_caps & PHY_CAP_SUP_6G))) {
        /* include 5G for subband5gver=6, SUBBANB5GV6(pi) is not set yet */
        if (wlc_phy_txpwr_5gext_srom18_read(info) != TRUE) {
            PHY_ERROR(("%s: wlc_phy_txpwr_5gext_srom18_read failed\n", __FUNCTION__));
            goto fail;
        }
    }
#endif /* BAND6G */
    if (SROMREV(pi->sh->sromrev) >= 12) {
        if (wlc_phy_txpwr_srom12_read(info) != TRUE) {
            PHY_ERROR(("%s: wlc_phy_txpwr_srom12_read failed\n", __FUNCTION__));
            goto fail;
        }
    } else {
        if (wlc_phy_txpwr_srom11_read(info) != TRUE) {
            PHY_ERROR(("%s: wlc_phy_txpwr_srom11_read failed\n", __FUNCTION__));
            goto fail;
        }
    }

    phy_set_feature_flag(pi, PHY_FEATURE_TXPWRCTRL_IDX, TRUE);

    phy_tpc_register_impl(ti, &fns);

    /* Register the scratch buffer size */
    phy_cache_register_reuse_size(pi->cachei, ESTPWRLUTS_TBL_LEN*4);

    return info;
fail:
    phy_ac_tpc_unregister_impl(info);
    return NULL;
}

void
BCMATTACHFN(phy_ac_tpc_unregister_impl)(phy_ac_tpc_info_t *tpci)
{
    PHY_TRACE(("%s\n", __FUNCTION__));

    if (tpci == NULL) {
        return;
    }

    phy_tpc_unregister_impl(tpci->ti);

#ifdef PREASSOC_PWRCTRL
    if (tpci->pwr_ctrl_save != NULL) {
        phy_mfree(tpci->pi, tpci->pwr_ctrl_save, sizeof(phy_pwr_ctrl_s));
    }
#endif /* PREASSOC_PWRCTRL */

    if (tpci->phy_txpwrbackoff) {
        phy_mfree(tpci->pi, tpci->phy_txpwrbackoff,
                sizeof(phy_txpwrbackoff_t));
        tpci->phy_txpwrbackoff = NULL;
    }
#ifdef WL11AX
    if (tpci->phy_ac_txctrlwd_info != NULL) {
        phy_mfree(tpci->pi, tpci->phy_ac_txctrlwd_info, sizeof(phy_ac_txctrlwd_info_t));
    }
#endif /* WL11AX */
    phy_mfree(tpci->pi, tpci, sizeof(phy_ac_tpc_info_t));
}

static void
BCMATTACHFN(phy_ac_tpc_std_params_attach)(phy_ac_tpc_info_t *tpci)
{
    uint8 core;
    /* Read the offset target power var */
    tpci->offset_targetpwr = (uint16)PHY_GETINTVAR_DEFAULT_SLICE(tpci->pi, rstr_offtgpwr, 0);
#ifdef WL_SAR_SIMPLE_CONTROL
    /* user specified sarlimit by nvram. off as a default */
    tpci->ti->data->cfg->dynamic_sarctrl_2g = 0;
    tpci->ti->data->cfg->dynamic_sarctrl_5g = 0;
    tpci->ti->data->cfg->dynamic_sarctrl_2g_2 = 0;
    tpci->ti->data->cfg->dynamic_sarctrl_5g_2 = 0;
#endif /* WL_SAR_CONTROL_LIMIT */
#ifdef FCC_PWR_LIMIT_2G
    tpci->ti->data->fcc_pwr_limit_2g = FALSE;
#endif /* FCC_PWR_LIMIT_2G */
    FOREACH_CORE(tpci->pi, core) {
        tpci->txpwrindex_hw_save[core] = 128;
        tpci->txpwr_cap[core] = 127;
    }
    tpci->txpwr_cap_status = FALSE;
#if defined(WLTEST)
    /* Initialize baseindex override to FALSE */
    tpci->ti->data->ovrinitbaseidx = FALSE;
#endif
    tpci->olpc_mode_caldone = FALSE;
}

static int
WLBANDINITFN(phy_ac_tpc_init)(phy_type_tpc_ctx_t *ctx)
{
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = tpci->pi;

    uint8 core;

    if (ACMAJORREV_130(pi->pubpi->phy_rev))  {
        wlc_mac_register_init_for_hwppr(pi);
    }

    FOREACH_CORE(tpci->pi, core) {
        if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
            tpci->ti->data->base_index_init[core] = 50;
            tpci->ti->data->base_index_cck_init[core] =
                tpci->ti->data->base_index_init[core];
        } else {
            tpci->ti->data->base_index_init[core] = 20;
            tpci->ti->data->base_index_cck_init[core] = 20;
        }
#ifdef PREASSOC_PWRCTRL
        tpci->pwr_ctrl_save->status_idx_carry_2g[core] = FALSE;
        tpci->pwr_ctrl_save->status_idx_carry_5g[core] = FALSE;
#endif
    }

    return BCME_OK;
}

void
chanspec_setup_tpc(phy_info_t *pi)
{
    uint8 core;

#if (defined(WLTEST) && !defined(WLTEST_DISABLED))
    /* Override init base index */
    if ((pi->tpci->data->ovrinitbaseidx) && !ACMAJORREV_32(pi->pubpi->phy_rev) &&
        !ACMAJORREV_33(pi->pubpi->phy_rev) &&
        !(ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        wlc_phy_txpwr_ovrinitbaseidx(pi);
    }
#endif

    if (!ACMAJORREV_32(pi->pubpi->phy_rev) &&
        !ACMAJORREV_33(pi->pubpi->phy_rev) &&
        !(ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        pi->tpci->data->base_index_init_invalid_frame_cnt = PHY_TOTAL_TX_FRAMES(pi);
        pi->u.pi_acphy->tpci->txpwr_damping_factor_set = 0;
        /* This is the damping value to be written to "TxPwrCtrlDamping.DeltaPwrDamp"
         * This value is in 1.6 format. So, 0x20 ie 32 corresponds to factor 0.5.
         * This damping factor is multiplied by the (est pwr - target power) to arrive
         * at the new index to be used.
         */
        pi->tpci->data->deltapwrdamp = 0x20;

        /* Set default damping scale factor - 0x40 corresponds to damping factor 1
         * So, this means default behaviour, no damping.
         */
        if (pi->tpci->data->tx_pwr_ctrl_damping_en) {
                MOD_PHYREG(pi, TxPwrCtrlDamping, DeltaPwrDamp, 0x40);
        }
    }

    if (!ACMAJORREV_32(pi->pubpi->phy_rev) &&
        !ACMAJORREV_33(pi->pubpi->phy_rev) &&
        !(ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        FOREACH_CORE(pi, core) {
            pi->tpci->data->adjusted_pwr_cap[core] = 127;
        }
    }
}

/* Conditonal Backoff Per Rate TX Power Targets */
#if defined(TXPWRBACKOFF)
void
wlc_phy_apply_txpwr_backoff_acphy(phy_ac_tpc_info_t *tpci, ppr_t *tx_pwr_target)
{
    PHY_INFORM(("%s(): applying tx power backoff (%d * .25) dBm on Chan %d\n",
            __FUNCTION__, tpci->phy_txpwrbackoff->vbat_tempsense_pwrbackoff,
            CHSPEC_CHANNEL(tpci->pi->radio_chanspec)));
    ppr_plus_cmn_val(tx_pwr_target, (uint8)tpci->phy_txpwrbackoff->vbat_tempsense_pwrbackoff);
}
#endif /* TXPWRBACKOFF */

/* recalc target txpwr and apply to h/w */
static void
phy_ac_tpc_recalc_tgt(phy_type_tpc_ctx_t *ctx)
{
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = info->pi;

    PHY_TRACE(("%s\n", __FUNCTION__));

    wlc_phy_txpower_recalc_target_acphy(pi);
}

static void
wlc_write_reg_obj(phy_info_t *pi, uint32 addr, uint32 val)
{
    W_REG(pi->sh->osh, D11_objaddr(pi), addr);
    W_REG(pi->sh->osh, D11_objdata(pi), val);
    return;
}

static uint8
offset_calc(int8 pwr, int8 max_pwr)
{
    return max_pwr <= pwr ? 0 : max_pwr - pwr;
}

static uint32
offsets2word(int8 *I, int8 max_pwr)
{
    int k = 0;
    uint32 val = offset_calc(I[3], max_pwr);
    for (k = 2; k >= 0; k--) {
        val = (val << 8) | offset_calc(I[k], max_pwr);
    }
    return val;
}

#ifdef WL11AX
#define RU_BOARD_PWR 60        /* 15 dBm, temporary value for RU board limit */

static void
wlc_phy_set_ru_offst_calc(uint8 txcap, int8 *tx_pwr_max, ppr_ru_t *clm_ru_txpwr_limit,
    ppr_ru_t *board_ru_txpwr_limit, wl_he_rate_type_t rusize,
    ppr_ru_offset_t *ruoffset)
{
    ppr_he_mcs_rateset_t herate;
    uint ss_idx = 1;
    int8 idx = 0, pwr = 0;
    /* RL offset, consider CDD/TxBF for different NTx */
    PHY_TXPWR(("%s: TBPPDU RL offset calc, RU Size = %d\n",
        __FUNCTION__, rusize));

    for (ss_idx = 1; ss_idx <= txcap; ss_idx++) {
        if (ss_idx == 1 && txcap > 1) {
            ppr_get_he_ru_mcs(clm_ru_txpwr_limit, rusize, ss_idx, WL_TX_MODE_CDD,
                txcap, &herate);
        } else {
            ppr_get_he_ru_mcs(clm_ru_txpwr_limit, rusize, ss_idx, WL_TX_MODE_NONE,
                txcap, &herate);
        }
        pwr = *tx_pwr_max - herate.pwr[0];
        if (herate.pwr[0] == WL_RATE_DISABLED || pwr < 0) {
            pwr = 0;
        }
        ppr_set_he_ruoffset_rl(ruoffset, ss_idx, WL_TX_MODE_CDD, rusize, pwr);
        PHY_TXPWR(("Nss=%d: power_offset = %d  ", ss_idx, pwr));
        if (ss_idx != txcap) {
            ppr_get_he_ru_mcs(clm_ru_txpwr_limit, rusize, ss_idx, WL_TX_MODE_TXBF,
                txcap, &herate);
            pwr = (uint8)(*tx_pwr_max - herate.pwr[0]);
            if (herate.pwr[0] == WL_RATE_DISABLED || pwr < 0) {
                pwr = 0;
            }
            ppr_set_he_ruoffset_rl(ruoffset, ss_idx, WL_TX_MODE_TXBF, rusize, pwr);
            PHY_TXPWR(("  %d",  pwr));

        }
        PHY_TXPWR(("\n"));
    }
    /* BL offset, MCS dependent only */
    ppr_get_he_ru_mcs(board_ru_txpwr_limit, rusize, WL_TX_NSS_1, WL_TX_MODE_NONE,
        WL_TX_CHAINS_1, &herate);
    PHY_TXPWR(("%s: TBPPDU BL offset calc, RU Size = %d power_offser=[",
        __FUNCTION__, rusize));
    for (idx = 0; idx < WL_RATESET_SZ_HE_MCS; idx++) {
        pwr = *tx_pwr_max - herate.pwr[idx];
        if (herate.pwr[0] == WL_RATE_DISABLED || pwr < 0) {
            pwr = 0;
        }
        ppr_set_he_ruoffset_bl(ruoffset, rusize, idx, pwr);
        PHY_TXPWR(("%d  ", pwr));
    }
    PHY_TXPWR(("]\n"));
}

static void
wlc_phy_set_shm_ru_offst(phy_ac_tpc_info_t *info, uint8 txcap, ppr_ru_offset_t* ruoffset)
{
    phy_info_t *pi = info->pi;
    int8 runum_idx = 0;
    int8 runum_idx_max = 0;
    int8 mcs_idx = 0;
    uint16 tmp, tmp1, tmp2;
    uint16 ru_blpwr_tbl = M_BRDLMT_BLK(pi);
    uint16 ru_rlpwr_tbl = M_RULMT_BLK(pi);

    //Writing M_BRDLMT_BLK
    for (mcs_idx = 2; mcs_idx < WL_RATESET_SZ_HE_MCS; mcs_idx++) {
        tmp1 = (uint8)(ppr_get_he_ruoffset(ruoffset, WL_HE_RT_RU52,
            mcs_idx, WL_TX_NSS_1, FALSE, FALSE));
        tmp2 = (uint8)(ppr_get_he_ruoffset(ruoffset, WL_HE_RT_RU26,
            mcs_idx, WL_TX_NSS_1, FALSE, FALSE));
        tmp = (tmp1 << 8) + tmp2;
        wlapi_bmac_write_shm(pi->sh->physhim, ru_blpwr_tbl + (0 + 3*(mcs_idx-2))*2, tmp);

        tmp1 = (uint8)(ppr_get_he_ruoffset(ruoffset, WL_HE_RT_RU242,
            mcs_idx, WL_TX_NSS_1, FALSE, FALSE));
        tmp2 = (uint8)(ppr_get_he_ruoffset(ruoffset, WL_HE_RT_RU106,
            mcs_idx, WL_TX_NSS_1, FALSE, FALSE));
        tmp = (tmp1 << 8) + tmp2;
        wlapi_bmac_write_shm(pi->sh->physhim, ru_blpwr_tbl + (1 + 3*(mcs_idx-2))*2, tmp);

        if (CHSPEC_BW_GE(pi->radio_chanspec, WL_CHANSPEC_BW_80)) {
            tmp1 = (uint8)(ppr_get_he_ruoffset(ruoffset, WL_HE_RT_RU996,
                mcs_idx, WL_TX_NSS_1, FALSE, FALSE));
        } else {
            tmp1 = 0;
        }
        if (CHSPEC_BW_GE(pi->radio_chanspec, WL_CHANSPEC_BW_40)) {
            tmp2 = (uint8)(ppr_get_he_ruoffset(ruoffset, WL_HE_RT_RU484,
                mcs_idx, WL_TX_NSS_1, FALSE, FALSE));
        } else {
            tmp2 = 0;
        }
        tmp = (tmp1 << 8) + tmp2;
        wlapi_bmac_write_shm(pi->sh->physhim, ru_blpwr_tbl + (2 + 3*(mcs_idx-2))*2, tmp);
    }

    //Writing M_RULMT_BLK
    if (CHSPEC_BW_GE(pi->radio_chanspec, WL_CHANSPEC_BW_80)) {
        runum_idx_max = WL_HE_RT_RU996;
    } else if (CHSPEC_BW_GE(pi->radio_chanspec, WL_CHANSPEC_BW_40)) {
        runum_idx_max = WL_HE_RT_RU484;
    } else {
        runum_idx_max = WL_HE_RT_RU242;
    }

    for (runum_idx = WL_HE_RT_RU26; runum_idx <= runum_idx_max; runum_idx++) {
        tmp1 = (txcap >= WL_TX_NSS_1) ? (uint8)(ppr_get_he_ruoffset(ruoffset, runum_idx,
            0, WL_TX_NSS_1, FALSE, TRUE)) : 0;
        tmp2 = (txcap >= WL_TX_NSS_2) ? (uint8)(ppr_get_he_ruoffset(ruoffset, runum_idx,
            0, WL_TX_NSS_2, FALSE, TRUE)) : 0;
        tmp = (tmp1 << 8) + tmp2;
        wlapi_bmac_write_shm(pi->sh->physhim, ru_rlpwr_tbl + (0 +
            4*(runum_idx-WL_HE_RT_RU26))*2, tmp);

        tmp1 = (txcap >= WL_TX_NSS_3) ? (uint8)(ppr_get_he_ruoffset(ruoffset, runum_idx,
            0, WL_TX_NSS_3, FALSE, TRUE)) : 0;
        tmp2 = (txcap >= WL_TX_NSS_4) ? (uint8)(ppr_get_he_ruoffset(ruoffset, runum_idx,
            0, WL_TX_NSS_4, FALSE, TRUE)) : 0;
        tmp = (tmp1 << 8) + tmp2;
        wlapi_bmac_write_shm(pi->sh->physhim, ru_rlpwr_tbl + (1 +
            4*(runum_idx-WL_HE_RT_RU26))*2, tmp);

        tmp1 = (txcap > WL_TX_NSS_1) ? (uint8)(ppr_get_he_ruoffset(ruoffset, runum_idx,
            0, WL_TX_NSS_1, FALSE, TRUE)) : 0;
        tmp2 = (txcap > WL_TX_NSS_2) ? (uint8)(ppr_get_he_ruoffset(ruoffset, runum_idx,
            0, WL_TX_NSS_2, FALSE, TRUE)) : 0;
        tmp = (tmp1 << 8) + tmp2;
        wlapi_bmac_write_shm(pi->sh->physhim, ru_rlpwr_tbl + (2 +
            4*(runum_idx-WL_HE_RT_RU26))*2, tmp);

        tmp1 = (txcap > WL_TX_NSS_3) ? (uint8)(ppr_get_he_ruoffset(ruoffset, runum_idx,
            0, WL_TX_NSS_3, FALSE, TRUE)) : 0;
        tmp = (tmp1 << 8);
        wlapi_bmac_write_shm(pi->sh->physhim, ru_rlpwr_tbl + (3 +
            4*(runum_idx-WL_HE_RT_RU26))*2, tmp);
    }
}

static void
wlc_phy_set_ru_txpwr_offset(phy_ac_tpc_info_t *info, uint8 mintxpwr, int8 *tx_pwr_max,
    int8 *tx_pwr_min, uint8 core)
{
    phy_info_t *pi = info->pi;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);
    ppr_ru_t *clm_ru_txpwr_limit;
    ppr_ru_t *board_ru_txpwr_limit;
    ppr_ru_t *tgt_ru_txpwr_limit;
    ppr_ru_offset_t* ru_offset;
    int8 ru_tx_pwr_max = 0;
    int8 ru_tx_pwr_min = 255;
    int8 max_board_limit;
    uint8 txcap = PHY_BITSCNT(stf_shdata->phytxchain);
    uint16 host_flags6;
    BCM_REFERENCE(stf_shdata);

    /* If the chip is HE incapable, don't set HE power limits */
    if ((pi->u.pi_acphy->phy_caps & PHY_CAP_HE) == 0) {
        return;
    }
    host_flags6 = wlapi_bmac_read_shm(pi->sh->physhim, M_HOST_FLAGS6(pi));
    clm_ru_txpwr_limit = pi->tpci->data->clm_ru_txpwr_limit;
    board_ru_txpwr_limit = pi->tpci->data->board_ru_txpwr_limit;
    tgt_ru_txpwr_limit = pi->tpci->data->tgt_ru_txpwr_limit;
    ru_offset = pi->tpci->data->ru_pwroffset;
    ppr_ru_offset_clear(ru_offset);
#if defined(WLTEST) || defined(WL_EXPORT_TXPOWER)
    /* Only allow tx power override for internal or test builds. */
    if (info->ti->data->txpwroverride) {
        ppr_ru_set_same_value(tgt_ru_txpwr_limit, info->ti->data->tx_user_target);
        wlapi_bmac_write_shm(pi->sh->physhim, M_HOST_FLAGS6(pi),
            host_flags6 | MHF6_PWR_OVRD);
    } else
#endif
    {
        wlapi_bmac_write_shm(pi->sh->physhim, M_HOST_FLAGS6(pi),
            host_flags6 & ~MHF6_PWR_OVRD);
        if (info->ti->data->phy_ultpc_ppr_dis) {
            wlapi_bmac_write_shm(pi->sh->physhim, M_HOST_FLAGS6(pi),
            host_flags6 | MHF6_ULTPC_DIS);
        } else {
            wlapi_bmac_write_shm(pi->sh->physhim, M_HOST_FLAGS6(pi),
            host_flags6 & ~MHF6_ULTPC_DIS);
        }
        /* Save the Board Limit for Radio Debugability Radar */
        max_board_limit = ppr_ru_get_max(board_ru_txpwr_limit);
        pi->tpci->data->txpwr_max_boardlim_percore[core] =
            MAX(pi->tpci->data->txpwr_max_boardlim_percore[core], max_board_limit);

        ppr_ru_compare_min(tgt_ru_txpwr_limit, clm_ru_txpwr_limit, board_ru_txpwr_limit);

        /* Subtract 6 (1.5 db) to ensure we don't go over
         * the limit given a noisy power detector.  The result
         * is now a target, not a limit.
         */
        ppr_ru_minus_cmn_val(tgt_ru_txpwr_limit, pi->tx_pwr_backoff);

        /* The user target is the starting point for determining the transmit power,
         * the transmit power should not be over the user target.
         */
        ppr_ru_compare_min_cmn_val(tgt_ru_txpwr_limit, pi->tpci->data->tx_user_target);

        ru_tx_pwr_max = ppr_ru_get_max(tgt_ru_txpwr_limit);
        *tx_pwr_max = MAX(*tx_pwr_max, ru_tx_pwr_max);

        if (CHSPEC_BW_GE(pi->radio_chanspec, WL_CHANSPEC_BW_160)) {
            //ru1992 is not supported yet
            //wlc_phy_set_ru_offst_calc(txcap, tx_pwr_max, clm_ru_txpwr_limit,
            //    board_ru_txpwr_limit, WL_HE_RT_RU1992, ru_offset);
        }
        if (CHSPEC_BW_GE(pi->radio_chanspec, WL_CHANSPEC_BW_80)) {
            wlc_phy_set_ru_offst_calc(txcap, tx_pwr_max, clm_ru_txpwr_limit,
                board_ru_txpwr_limit, WL_HE_RT_RU996, ru_offset);
        }
        if (CHSPEC_BW_GE(pi->radio_chanspec, WL_CHANSPEC_BW_40)) {
            wlc_phy_set_ru_offst_calc(txcap, tx_pwr_max, clm_ru_txpwr_limit,
                board_ru_txpwr_limit, WL_HE_RT_RU484, ru_offset);
        }

        if (CHSPEC_BW_GE(pi->radio_chanspec, WL_CHANSPEC_BW_20)) {
            wlc_phy_set_ru_offst_calc(txcap, tx_pwr_max, clm_ru_txpwr_limit,
                board_ru_txpwr_limit, WL_HE_RT_RU242, ru_offset);
            wlc_phy_set_ru_offst_calc(txcap, tx_pwr_max, clm_ru_txpwr_limit,
                board_ru_txpwr_limit, WL_HE_RT_RU106, ru_offset);
            wlc_phy_set_ru_offst_calc(txcap, tx_pwr_max, clm_ru_txpwr_limit,
                board_ru_txpwr_limit, WL_HE_RT_RU52, ru_offset);
            wlc_phy_set_ru_offst_calc(txcap, tx_pwr_max, clm_ru_txpwr_limit,
                board_ru_txpwr_limit, WL_HE_RT_RU26, ru_offset);
        }
        // write ruppr offset into shm
        wlc_phy_set_shm_ru_offst(info, txcap, ru_offset);

        ru_tx_pwr_min = ppr_ru_get_min(tgt_ru_txpwr_limit, mintxpwr);
        *tx_pwr_min = MIN(*tx_pwr_min, ru_tx_pwr_min);
    }

    /* Calculate new power offset (tx_pwr_max - *rptr_target)
     * Updtae to ru_tx_power_offset
     */
    ppr_ru_update_txpwr_offset(pi->tpci->data->ru_tx_power_offset,
        tgt_ru_txpwr_limit, *tx_pwr_max);
}

static uint32
wlc_read_reg_obj(phy_info_t *pi, uint32 addr)
{
    W_REG(pi->sh->osh, D11_objaddr(pi), addr);
    return R_REG(pi->sh->osh, D11_objdata(pi));
}

/* Function for iovar */
void
wlc_phy_hwpprctrl_enable_acphy(phy_info_t *pi, uint8 ctrl_type)
{
    //phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t *)pi->u.pi_acphy->tpci;

    if (ctrl_type == PHY_HWPPR_OFF) {
        /* FIXME: may need to save current info/status
         * if hwpprctrl was enabled
         */
        wlc_phy_hwppr_en(pi, FALSE);
        PHY_TXPWR(("wl%d: HWPPR Disabled\n", pi->sh->unit));
    } else {
        /* Enable HWPPR for MAC */
        wlc_phy_hwppr_en(pi, TRUE);

        PHY_TXPWR(("wl%d: HWPPR Enabled, phy_hwppr_ison = %d\n",
            pi->sh->unit, wlc_phy_hwppr_ison(pi)));
    }
}

void
wlc_phy_hwppr_en(phy_info_t *pi, bool en)
{
    uint32 host_flags6;
    host_flags6 = wlapi_bmac_read_shm(pi->sh->physhim, M_HOST_FLAGS6(pi));
    if (en) {
        host_flags6 &= ~MHF6_HWPPR_EN;
    } else {
        host_flags6 |= MHF6_HWPPR_EN;
    }
    wlapi_bmac_write_shm(pi->sh->physhim, M_HOST_FLAGS6(pi), host_flags6);

    /* if host_flags6 = 0, then hwppr enabled. If 4, then disabled */
    PHY_TXPWR(("wl%d: host_flags6 = %d\n", pi->sh->unit, host_flags6));
}

bool wlc_phy_hwppr_ison(phy_info_t *pi)
{
    uint32 host_flags6;
    host_flags6 = wlapi_bmac_read_shm(pi->sh->physhim, M_HOST_FLAGS6(pi));
    return !(host_flags6 & MHF6_HWPPR_EN);
}

void
wlc_phy_dump_hwppr_tbl(phy_info_t *pi)
{
    int k, tbl_len = 162;
    if (ACMAJORREV_130(pi->pubpi->phy_rev) && ACMINORREV_1(pi)) {
        tbl_len = 381;
    }

    for (k = 0; k < tbl_len; k++) {
        PHY_TXPWR(("HW-PPR Table[%03d] = %x\n", k,
                wlc_read_reg_obj(pi, 0xf0000 + k)));
    }
    return;
}
void
wlc_phy_zero_hwppr_tbl(phy_info_t *pi)
{
    int k, tbl_len = 162;
    for (k = 0; k < tbl_len; k++) {
        wlc_write_reg_obj(pi, 0xf0000 + k, 0x0);
    }
    return;
}

#endif /* WL11AX */

int
wlc_phy_set_hwppr_bl_tbl(phy_info_t *pi, ppr_t *board_txpwr_limit,
        int8 max_pwr, chanspec_t chanspec)
{
    ppr_dsss_rateset_t dsss;
    ppr_ofdm_rateset_t ofdm;
    ppr_vht_mcs_rateset_t mcs;
#ifdef WL11AX
    ppr_ru_t *board_ru_txpwr_limit = NULL;
    ppr_he_mcs_rateset_t he;
#endif /* WL11AX */
    int bw, max_bw = 1;
    int bw_list[4] = {WL_TX_BW_20, WL_TX_BW_40, WL_TX_BW_80, WL_TX_BW_160};
    uint32 addr, val;
    int idx, k, cnt, err = BCME_OK;
    const uint8 MAX_RATE_GRP = 3; /* 12 mcs / 4 mcs per group */

    /* This is supported for Major REV 130 only */
    if (!ACMAJORREV_130(pi->pubpi->phy_rev)) {
        return BCME_ERROR;
    }

    if (CHSPEC_IS40(chanspec))
        max_bw = 2;
    else if (CHSPEC_IS80(chanspec))
        max_bw = 3;
    else if (CHSPEC_IS160(chanspec))
        max_bw = 4;

    ASSERT(board_txpwr_limit);
#ifdef WL11AX
    board_ru_txpwr_limit = pi->tpci->data->board_ru_txpwr_limit;
    ASSERT(board_ru_txpwr_limit);
#endif /* WL11AX */
    /* DSSS */
    if (CHSPEC_IS2G(chanspec)) {
        addr = 0xf0000;
        cnt = ppr_get_dsss(board_txpwr_limit, WL_TX_BW_20, WL_TX_CHAINS_1, &dsss);
        if (cnt < WL_RATESET_SZ_DSSS) err = BCME_ERROR;
        val = offsets2word(&dsss.pwr[0], max_pwr);
        wlc_write_reg_obj(pi, addr, val);
        PHY_TXPWR(("DSSS TX = %x\n", val));

        /* Assuming 2G channel scan mode only use cck frame */
        if (wlc_phy_is_scan_chan_acphy(pi))
            return err;
    }

     /* DOT11AG */
    for (idx = 0; idx < max_bw; idx++) {
        addr = 0xf0000 + 1 + (idx << 1);
        bw = bw_list[idx];
        cnt = ppr_get_ofdm(board_txpwr_limit, bw,
                WL_TX_MODE_NONE,
                WL_TX_CHAINS_1, &ofdm);
        if (cnt < WL_RATESET_SZ_OFDM) err = BCME_ERROR;
        val = offsets2word(&ofdm.pwr[0], max_pwr);
        wlc_write_reg_obj(pi, addr++, val);
        val = offsets2word(&ofdm.pwr[4], max_pwr);
        wlc_write_reg_obj(pi, addr, val);
        PHY_TXPWR(("OFDM BL bw%d TX = %x\n", idx, val));
    }

    /* Assuming 5G channel scan mode only use dot11ag frame */
    if (wlc_phy_is_scan_chan_acphy(pi))
        return err;

    /* DOT11N-VHT */
    for (idx = 0; idx < max_bw; idx++) {
        bw = bw_list[idx];
        addr = 0xf0000 + 9 + idx * 3;
        cnt = ppr_get_vht_mcs(board_txpwr_limit, bw, WL_TX_NSS_1,
                WL_TX_MODE_NONE, WL_TX_CHAINS_1, &mcs);
        if (cnt < WL_RATESET_SZ_HE_MCS) err = BCME_ERROR;
        for (k = 0; k < MAX_RATE_GRP; k++) {
            val = offsets2word(&mcs.pwr[k<<2], max_pwr);
            wlc_write_reg_obj(pi, addr++, val);
            PHY_TXPWR(("HT-VHT bw%d mcs%d TX = %x\n", idx, k<<2, val));
        }
    }

#ifdef WL11AX
    /* HE SU/MU */
    for (idx = 0; idx < max_bw; idx++) {
        bw = bw_list[idx];
        addr = 0xf0000 + 21 + idx * 3;
        cnt = ppr_get_he_mcs(board_txpwr_limit, bw, WL_TX_NSS_1,
                WL_TX_MODE_NONE, WL_TX_CHAINS_1, &he, WL_HE_RT_SU);
        if (cnt < WL_RATESET_SZ_HE_MCS) err = BCME_ERROR;
        for (k = 0; k < MAX_RATE_GRP; k++) {
            val = offsets2word(&he.pwr[k<<2], max_pwr);
            wlc_write_reg_obj(pi, addr++, val);
            PHY_TXPWR(("HE bw%d mcs%d TX = %x\n", idx, k<<2, val));
        }
    }

    /* HE RU TB-PPDU */
    for (idx = WL_HE_RT_RU26; idx <= WL_HE_RT_RU996; idx++) {
        // 1ss_CDD
        addr = 0xf0000 + 33 + (idx - WL_HE_RT_RU26) * 3;
        cnt = ppr_get_he_ru_mcs(board_ru_txpwr_limit, idx, WL_TX_NSS_1,
                WL_TX_MODE_NONE, WL_TX_CHAINS_1, &he);
        if (cnt < WL_RATESET_SZ_HE_MCS) err = BCME_ERROR;
        for (k = 0; k < MAX_RATE_GRP; k++) {
            val = offsets2word(&he.pwr[k<<2], max_pwr);
            wlc_write_reg_obj(pi, addr++, val);
            PHY_TXPWR(("HE RU bw%d mcs%d TX = %x\n", idx, k<<2, val));
        }
    }

    /* HE RU-1992 TB-PPDU */
    if (max_bw == 4) {
        addr = 0xf0000 + 30;
        for (k = 0; k < MAX_RATE_GRP; k++) {
            val = wlc_read_reg_obj(pi, addr + k);
            /* Copy from SU-160mhz boardlimit offset */
            wlc_write_reg_obj(pi, addr + 21 + k, val);
        }
    }

    /* HE RE */
    addr = 0xf0000;
    /* Copy from SU-160mhz boardlimit offset */
    if (max_bw == 4) {
        val = wlc_read_reg_obj(pi, addr + 21);
        wlc_write_reg_obj(pi, addr + 54, val);
    }
    /* Copy from TB-RU106 boardlimit offset */
    val = wlc_read_reg_obj(pi, addr + 39);
    wlc_write_reg_obj(pi, addr + 55, val);

#endif /* WL11AX */

    if (err == BCME_ERROR)
        PPR_ERROR(("%s failed!\n", __FUNCTION__));

    return err;
}

int
wlc_phy_set_hwppr_rl_tbl(phy_info_t *pi, ppr_t *reg_txpwr_limit, int8 max_pwr, chanspec_t chanspec)
{
    ppr_dsss_rateset_t dsss;
    ppr_ofdm_rateset_t ofdm;
    ppr_vht_mcs_rateset_t mcs;
#ifdef WL11AX
    ppr_ru_t *clm_ru_txpwr_limit;
    ppr_he_mcs_rateset_t he;
    int rt;
#endif /* WL11AX */
    int bw, max_bw = 1;
    int bw_list[4] = {WL_TX_BW_20, WL_TX_BW_40, WL_TX_BW_80, WL_TX_BW_160};
    uint32 addr, val;
    uint32 addr2 = 0;
    uint32 addr3 = 0;
    uint32 val2 = 0;
    uint32 val3 = 0;
    int idx, k, cnt, err = BCME_OK;

    /* This is supported for Major REV 130_1 (6715B0) only */
    if (!(ACMAJORREV_130(pi->pubpi->phy_rev) && ACMINORREV_1(pi))) {
        return BCME_ERROR;
    }

    if (CHSPEC_IS40(chanspec))
        max_bw = 2;
    else if (CHSPEC_IS80(chanspec))
        max_bw = 3;
    else if (CHSPEC_IS160(chanspec))
        max_bw = 4;

    ASSERT(reg_txpwr_limit);
#ifdef WL11AX
    clm_ru_txpwr_limit = pi->tpci->data->clm_ru_txpwr_limit;
    ASSERT(clm_ru_txpwr_limit);
#endif /* WL11AX */

    //hwppr rate2group setting
    addr = 0xf0179;
    val = 0x36C0000;
    wlc_write_reg_obj(pi, addr, val);
    val = 0x801B248;
    wlc_write_reg_obj(pi, ++addr, val);
    val = 0x6DB6C9;
    wlc_write_reg_obj(pi, ++addr, val);
    val = 0x1B692248;
    wlc_write_reg_obj(pi, ++addr, val);

    /* DSSS */
    if (CHSPEC_IS2G(chanspec)) {
        addr = 0xf0038;
        addr2 = addr + HWPPR_PERRATETBL_OFFSET;
        addr3 = addr + (HWPPR_PERRATETBL_OFFSET << 1);
        val = 0;
        val2 = 0; val3 = 0;
        for (k = 1; k <= PPR_MAX_TX_CHAINS; k++) {
            cnt = ppr_get_dsss(reg_txpwr_limit, WL_TX_BW_20, k, &dsss);
            if (cnt == 0) err = BCME_ERROR;
            val |= offset_calc(dsss.pwr[0], max_pwr) << ((k-1) << 3);
        }
        wlc_write_reg_obj(pi, addr, val);
        wlc_write_reg_obj(pi, addr2, val2);
        wlc_write_reg_obj(pi, addr3, val3);
        PHY_TXPWR(("DSSS TX = %x\n", val));

        /* Assuming 2G channel scan mode only use cck frame */
        if (wlc_phy_is_scan_chan_acphy(pi))
            return err;
    }

    /* DOT11AG */
    for (idx = 0; idx < max_bw; idx++) {
        addr = 0xf0038 + 1 + (idx << 1);
        addr2 = addr + HWPPR_PERRATETBL_OFFSET;
        addr3 = addr + (HWPPR_PERRATETBL_OFFSET << 1);
        val = 0;
        val2 = 0; val3 = 0;
        bw = bw_list[idx];
        for (k = 1; k <= PPR_MAX_TX_CHAINS; k++) {
            cnt = ppr_get_ofdm(reg_txpwr_limit, bw,
                    k == 1 ? WL_TX_MODE_NONE : WL_TX_MODE_CDD,
                    k, &ofdm);
            if (cnt == 0) err = BCME_ERROR;
            val |= offset_calc(ofdm.pwr[0], max_pwr) << ((k-1) << 3);
            //group3, 36~54mbps offset in half dB
            val3 |= (offset_calc(ofdm.pwr[5], ofdm.pwr[0])/2) << ((k-1) << 3);
        }
        wlc_write_reg_obj(pi, addr, val);
        wlc_write_reg_obj(pi, addr2, val2);
        wlc_write_reg_obj(pi, addr3, val3);
        PHY_TXPWR(("OFDM CDD REG bw%d TX = %x\n", idx, val));

        /* bypass this part of code as we don't support dot11ag txbf
        addr += 1;
        val = 0xff;
        for (k = 2; k <= PPR_MAX_TX_CHAINS; k++) {
            cnt = ppr_get_ofdm(reg_txpwr_limit, bw, WL_TX_MODE_TXBF, k, &ofdm);
            if (cnt == 0) err = BCME_ERROR;
            val |= offset_calc(ofdm.pwr[0], max_pwr) << ((k-1) << 3);
        }
        wlc_write_reg_obj(pi, addr, val);
        PHY_TXPWR(("OFDM TXBF REG bw%d = %x\n", idx, val));
        */
        wlc_write_reg_obj(pi, ++addr, 0x0); /* write zero */
        wlc_write_reg_obj(pi, ++addr2, val2);
        wlc_write_reg_obj(pi, ++addr3, val3);
    }

    /* Assuming 5G channel scan mode only use dot11ag frame */
    if (wlc_phy_is_scan_chan_acphy(pi))
        return err;

    /* DOT11N-VHT */
    for (idx = 0; idx < max_bw; idx++) {
        bw = bw_list[idx];
        // 1ss_CDD
        addr = 0xf0038 + 9 + idx * 5;
        addr2 = addr + HWPPR_PERRATETBL_OFFSET;
        addr3 = addr + (HWPPR_PERRATETBL_OFFSET << 1);
        val = 0;
        val2 = 0; val3 = 0;
        for (k = 1; k <= PPR_MAX_TX_CHAINS; k++) {
            cnt = ppr_get_vht_mcs(reg_txpwr_limit, bw, WL_TX_NSS_1,
                    k == 1 ? WL_TX_MODE_NONE : WL_TX_MODE_CDD, k, &mcs);
            if (cnt == 0) err = BCME_ERROR;
            val |= offset_calc(mcs.pwr[0], max_pwr) << ((k-1) << 3);
            //mcs3-5, group1 in half dB
            val2 |= (offset_calc(mcs.pwr[3], mcs.pwr[0]))/2 << ((k-1) << 3);
            //mcs6-11, group3 in half dB
            val3 |= (offset_calc(mcs.pwr[6], mcs.pwr[0]))/2 << ((k-1) << 3);
        }
        wlc_write_reg_obj(pi, addr, val);
        wlc_write_reg_obj(pi, addr2, val2);
        wlc_write_reg_obj(pi, addr3, val3);
        PHY_TXPWR(("HT-VHT CDD bw%d 1ss = %x\n", idx, val));

        // 1ss_TXBF/STBC
        addr += 1;
        addr2 += 1; addr3 += 1;
        val = 0;
        val2 = 0; val3 = 0;
        for (k = 1; k <= PPR_MAX_TX_CHAINS; k++) {
            if (k == 1 && PPR_MAX_TX_CHAINS >= WL_TX_CHAINS_2) {
                cnt = ppr_get_vht_mcs(reg_txpwr_limit, bw, WL_TX_NSS_2,
                        WL_TX_MODE_STBC, WL_TX_CHAINS_2, &mcs);
            } else if (k > 1) {
                cnt = ppr_get_vht_mcs(reg_txpwr_limit, bw, WL_TX_NSS_1,
                        WL_TX_MODE_TXBF, k, &mcs);
            } else { /* not used if PPR_MAX_TX_CHAINS < WL_TX_CHAINS_2 */
                cnt = 1; mcs.pwr[0] = 0; mcs.pwr[3] = 0; mcs.pwr[6] = 0;
            }
            if (cnt == 0) err = BCME_ERROR;
            val |= offset_calc(mcs.pwr[0], max_pwr) << ((k-1) << 3);
            //mcs3-5, group1 in half dB
            val2 |= (offset_calc(mcs.pwr[3], mcs.pwr[0]))/2 << ((k-1) << 3);
            //mcs6-11, group3 in half dB
            val3 |= (offset_calc(mcs.pwr[6], mcs.pwr[0]))/2 << ((k-1) << 3);
        }
        wlc_write_reg_obj(pi, addr, val);
        wlc_write_reg_obj(pi, addr2, val2);
        wlc_write_reg_obj(pi, addr3, val3);

        PHY_TXPWR(("HT-VHT 1ss_TXBF/STBC bw%d 1ss = %x\n", idx, val));

        // 2ss_SPEXP/STBC_SPEXP1
        addr += 1;
        addr2 += 1; addr3 += 1;
        val = 0;
        val2 = 0; val3 = 0;
        for (k = 1; k <= PPR_MAX_TX_CHAINS; k++) {
            if (k == 1 && PPR_MAX_TX_CHAINS >= WL_TX_CHAINS_3) {
                cnt = ppr_get_vht_mcs(reg_txpwr_limit, bw, WL_TX_NSS_2,
                        WL_TX_MODE_STBC, WL_TX_CHAINS_3, &mcs);
            } else if (k > 1) {
                cnt = ppr_get_vht_mcs(reg_txpwr_limit, bw, WL_TX_NSS_2,
                        WL_TX_MODE_NONE, k, &mcs);
            } else { /* not used if PPR_MAX_TX_CHAINS < WL_TX_CHAINS_2 */
                cnt = 1; mcs.pwr[0] = 0; mcs.pwr[3] = 0; mcs.pwr[6] = 0;
            }
            if (cnt == 0) err = BCME_ERROR;
            val |= offset_calc(mcs.pwr[0], max_pwr) << ((k-1) << 3);
            //mcs3-5, group1 in half dB
            val2 |= (offset_calc(mcs.pwr[3], mcs.pwr[0]))/2 << ((k-1) << 3);
            //mcs6-11, group3 in half dB
            val3 |= (offset_calc(mcs.pwr[6], mcs.pwr[0]))/2 << ((k-1) << 3);
        }
        wlc_write_reg_obj(pi, addr, val);
        wlc_write_reg_obj(pi, addr2, val2);
        wlc_write_reg_obj(pi, addr3, val3);
        PHY_TXPWR(("HT-VHT 2ss_SPEXP/STBC_SPEXP1 bw%d 1ss = %x\n", idx, val));

        // 2ss_SPEXP/STBC_SPEXP1
        addr += 1;
        addr2 += 1; addr3 += 1;
        val = 0;
        val2 = 0; val3 = 0;
        for (k = 1; k <= PPR_MAX_TX_CHAINS; k++) {
            if (k == 1 && PPR_MAX_TX_CHAINS >= WL_TX_CHAINS_4) {
                cnt = ppr_get_vht_mcs(reg_txpwr_limit, bw, WL_TX_NSS_2,
                        WL_TX_MODE_STBC, WL_TX_CHAINS_4, &mcs);
            } else if (k == 2 && PPR_MAX_TX_CHAINS >= WL_TX_CHAINS_4) {
                cnt = ppr_get_vht_mcs(reg_txpwr_limit, bw, WL_TX_NSS_4,
                        WL_TX_MODE_NONE, WL_TX_CHAINS_4, &mcs);
            } else if (k > 2) {
                cnt = ppr_get_vht_mcs(reg_txpwr_limit, bw, WL_TX_NSS_2,
                        WL_TX_MODE_TXBF, k, &mcs);
            } else {
                cnt = 1; mcs.pwr[0] = 0; mcs.pwr[3] = 0; mcs.pwr[6] = 0;
            }
            if (cnt == 0) err = BCME_ERROR;
            val |= offset_calc(mcs.pwr[0], max_pwr) << ((k-1) << 3);
            //mcs3-5, group1 in half dB
            val2 |= (offset_calc(mcs.pwr[3], mcs.pwr[0]))/2 << ((k-1) << 3);
            //mcs6-11, group3 in half dB
            val3 |= (offset_calc(mcs.pwr[6], mcs.pwr[0]))/2 << ((k-1) << 3);
        }
        wlc_write_reg_obj(pi, addr, val);
        wlc_write_reg_obj(pi, addr2, val2);
        wlc_write_reg_obj(pi, addr3, val3);

        PHY_TXPWR(("HT-VHT 2ss_SPEXP/STBC_SPEXP1 bw%d 1ss = %x\n", idx, val));

        // 3ss_TXBF/SDM/SPEXP1
        val = 0xff; addr += 1;
        addr2 += 1; addr3 += 1;
        val2 = 0; val3 = 0;
        for (k = 2; k <= PPR_MAX_TX_CHAINS; k++) {
            // k = 1 is rsvd
            if (k == 2 && PPR_MAX_TX_CHAINS >= WL_TX_CHAINS_4) {
                cnt = ppr_get_vht_mcs(reg_txpwr_limit, bw, WL_TX_NSS_3,
                        WL_TX_MODE_TXBF, WL_TX_CHAINS_4, &mcs);
            } else if (k == 3 && PPR_MAX_TX_CHAINS >= WL_TX_CHAINS_3) {
                cnt = ppr_get_vht_mcs(reg_txpwr_limit, bw, WL_TX_NSS_3,
                        WL_TX_MODE_NONE, WL_TX_CHAINS_3, &mcs);
            } else if (k == 4) {
                cnt = ppr_get_vht_mcs(reg_txpwr_limit, bw, WL_TX_NSS_3,
                        WL_TX_MODE_NONE, k, &mcs);
            } else {
                cnt = 1; mcs.pwr[0] = 0; mcs.pwr[3] = 0; mcs.pwr[6] = 0;
            }
            if (cnt == 0) err = BCME_ERROR;
            val |= offset_calc(mcs.pwr[0], max_pwr) << ((k-1) << 3);
            //mcs3-5, group1 in half dB
            val2 |= (offset_calc(mcs.pwr[3], mcs.pwr[0]))/2 << ((k-1) << 3);
            //mcs6-11, group3 in half dB
            val3 |= (offset_calc(mcs.pwr[6], mcs.pwr[0]))/2 << ((k-1) << 3);
        }
        wlc_write_reg_obj(pi, addr, val);
        wlc_write_reg_obj(pi, addr2, val2);
        wlc_write_reg_obj(pi, addr3, val3);

        PHY_TXPWR(("HT-VHT 3ss_TXBF/SDM/SPEXP1 bw%d 1ss = %x\n", idx, val));
    }

#ifdef WL11AX
    /* HE SU/MU */
    for (rt = WL_HE_RT_SU; rt <= WL_HE_RT_LUB; rt++) {
        for (idx = 0; idx < max_bw; idx++) {
            bw = bw_list[idx];
            // 1ss_CDD
            addr = 0xf0038 + 29 + idx * 4;
            addr += ((rt == WL_HE_RT_LUB) ? 16 : (rt == WL_HE_RT_UB) ? 32 : 0);
            addr2 = addr + HWPPR_PERRATETBL_OFFSET;
            addr3 = addr + (HWPPR_PERRATETBL_OFFSET << 1);
            /* in HW, addr order is SU=>LUB=>UB */
            val = 0;
            val2 = 0; val3 = 0;
            for (k = 1; k <= PPR_MAX_TX_CHAINS; k++) {
                cnt = ppr_get_he_mcs(reg_txpwr_limit, bw, WL_TX_NSS_1,
                        k == 1 ? WL_TX_MODE_NONE : WL_TX_MODE_CDD,
                        k, &he, rt);
                if (cnt == 0) err = BCME_ERROR;
                val |= offset_calc(he.pwr[0], max_pwr) << ((k-1) << 3);
                //mcs3-5, group1 in half dB, mcs6-8, group2 in half dB
                val2 |= ((offset_calc(he.pwr[3], he.pwr[0])/2) |
                    ((offset_calc(he.pwr[6], he.pwr[0])/2) << 4)) <<
                    ((k-1) << 3);
                //mcs9-11, group3 in half dB
                val3 |= (offset_calc(he.pwr[9], he.pwr[0])/2) << ((k-1) << 3);
            }
            wlc_write_reg_obj(pi, addr, val);
            wlc_write_reg_obj(pi, addr2, val2);
            wlc_write_reg_obj(pi, addr3, val3);
            PHY_TXPWR(("\n%s HE-%d CDD bw%d 1ss = %x\n", __FUNCTION__, rt, idx, val));

            // 1ss_TXBF/3ss_SDM
            val = 0; addr += 1;
            val2 = 0; val3 = 0;
            addr2 += 1; addr3 += 1;
            for (k = 1; k <= PPR_MAX_TX_CHAINS; k++) {
                if (k == 1 && PPR_MAX_TX_CHAINS >= WL_TX_CHAINS_3) {
                    cnt = ppr_get_he_mcs(reg_txpwr_limit, bw, WL_TX_NSS_3,
                            WL_TX_MODE_NONE, WL_TX_CHAINS_3, &he, rt);
                } else if (k > 1) {
                    cnt = ppr_get_he_mcs(reg_txpwr_limit, bw, WL_TX_NSS_1,
                            WL_TX_MODE_TXBF, k, &he, rt);
                } else {
                    cnt = 1; he.pwr[0] = 0;
                }
                if (cnt == 0) err = BCME_ERROR;
                val |= offset_calc(he.pwr[0], max_pwr) << ((k-1) << 3);
                //mcs3-5, group1 in half dB, mcs6-8, group2 in half dB
                val2 |= ((offset_calc(he.pwr[3], he.pwr[0])/2) |
                    ((offset_calc(he.pwr[6], he.pwr[0])/2) << 4)) <<
                    ((k-1) << 3);
                //mcs9-11, group3 in half dB
                val3 |= (offset_calc(he.pwr[9], he.pwr[0])/2) << ((k-1) << 3);
            }
            wlc_write_reg_obj(pi, addr, val);
            wlc_write_reg_obj(pi, addr2, val2);
            wlc_write_reg_obj(pi, addr3, val3);
            PHY_TXPWR(("HE 1ss_TXBF/3ss_SDM bw%d 1ss = %x\n", idx, val));

            // 2ss_SPEXP/SDM
            val = 0; addr += 1;
            val2 = 0; val3 = 0;
            addr2 += 1; addr3 += 1;
            for (k = 1; k <= PPR_MAX_TX_CHAINS; k++) {
                if (k == 1 && PPR_MAX_TX_CHAINS >= WL_TX_CHAINS_4) {
                    cnt = ppr_get_he_mcs(reg_txpwr_limit, bw, WL_TX_NSS_3,
                            WL_TX_MODE_NONE, WL_TX_CHAINS_4, &he, rt);
                } else if (k > 1) {
                    cnt = ppr_get_he_mcs(reg_txpwr_limit, bw, WL_TX_NSS_2,
                            WL_TX_MODE_NONE, k, &he, rt);
                } else {
                    cnt = 1; he.pwr[0] = 0;
                }
                if (cnt == 0) err = BCME_ERROR;
                val |= offset_calc(he.pwr[0], max_pwr) << ((k-1) << 3);
                //mcs3-5, group1 in half dB, mcs6-8, group2 in half dB
                val2 |= ((offset_calc(he.pwr[3], he.pwr[0])/2) |
                    ((offset_calc(he.pwr[6], he.pwr[0])/2) << 4)) <<
                    ((k-1) << 3);
                //mcs9-11, group3 in half dB
                val3 |= (offset_calc(he.pwr[9], he.pwr[0])/2) << ((k-1) << 3);
            }
            wlc_write_reg_obj(pi, addr, val);
            wlc_write_reg_obj(pi, addr2, val2);
            wlc_write_reg_obj(pi, addr3, val3);
            PHY_TXPWR(("HE 2ss_SPEXP/SDM bw%d 1ss = %x\n", idx, val));

            // 2ss_SPEXP/TXBF_SPEXP1
            val = 0; addr += 1;
            val2 = 0; val3 = 0;
            addr2 += 1; addr3 += 1;
            for (k = 1; k <= PPR_MAX_TX_CHAINS; k++) {
                if (k == 1 && PPR_MAX_TX_CHAINS >= WL_TX_CHAINS_4) {
                    cnt = ppr_get_he_mcs(reg_txpwr_limit, bw, WL_TX_NSS_3,
                            WL_TX_MODE_TXBF, WL_TX_CHAINS_4, &he, rt);
                } else if (k == 2 && PPR_MAX_TX_CHAINS >= WL_TX_CHAINS_4) {
                    cnt = ppr_get_he_mcs(reg_txpwr_limit, bw, WL_TX_NSS_4,
                            WL_TX_MODE_NONE, WL_TX_CHAINS_4, &he, rt);
                } else if (k > 2) {
                    cnt = ppr_get_he_mcs(reg_txpwr_limit, bw, WL_TX_NSS_2,
                            WL_TX_MODE_TXBF, k, &he, rt);
                } else {
                    cnt = 1; he.pwr[0] = 0;
                }
                if (cnt == 0) err = BCME_ERROR;
                val |= offset_calc(he.pwr[0], max_pwr) << ((k-1) << 3);
                //mcs3-5, group1 in half dB, mcs6-8, group2 in half dB
                val2 |= ((offset_calc(he.pwr[3], he.pwr[0])/2) |
                    ((offset_calc(he.pwr[6], he.pwr[0])/2) << 4)) <<
                    ((k-1) << 3);
                //mcs9-11, group3 in half dB
                val3 |= (offset_calc(he.pwr[9], he.pwr[0])/2) << ((k-1) << 3);
            }
            wlc_write_reg_obj(pi, addr, val);
            wlc_write_reg_obj(pi, addr2, val2);
            wlc_write_reg_obj(pi, addr3, val3);
            PHY_TXPWR(("HE 2ss_SPEXP/TXBF_SPEXP1 bw%d 1ss = %x\n", idx, val));
        }
    }

    /* HE RU TB-PPDU */
    for (idx = WL_HE_RT_RU26; idx <= WL_HE_RT_RU996; idx++) {
        // 1ss_CDD
        addr = 0xf0038 + 77 + (idx - WL_HE_RT_RU26) * 4;
        val = 0;
        addr2 = addr + HWPPR_PERRATETBL_OFFSET;
        addr3 = addr + (HWPPR_PERRATETBL_OFFSET << 1);
        val2 = 0; val3 = 0;
        for (k = 1; k <= PPR_MAX_TX_CHAINS; k++) {
            cnt = ppr_get_he_ru_mcs(clm_ru_txpwr_limit, idx, WL_TX_NSS_1,
                    k == 1 ? WL_TX_MODE_NONE : WL_TX_MODE_CDD, k, &he);
            if (cnt == 0) err = BCME_ERROR;
            val |= offset_calc(he.pwr[0], max_pwr) << ((k-1) << 3);
            //mcs3-5, group1 in half dB, mcs6-8, group2 in half dB
            val2 |= ((offset_calc(he.pwr[3], he.pwr[0])/2) |
                ((offset_calc(he.pwr[6], he.pwr[0])/2) << 4)) <<
                ((k-1) << 3);
            //mcs9-11, group3 in half dB
            val3 |= (offset_calc(he.pwr[9], he.pwr[0])/2) << ((k-1) << 3);
        }
        wlc_write_reg_obj(pi, addr, val);
        wlc_write_reg_obj(pi, addr2, val2);
        wlc_write_reg_obj(pi, addr3, val3);
        PHY_TXPWR(("%s HE RU%d CDD 1ss = %x\n", __FUNCTION__, idx, val));

        // 1ss_TXBF/3ss_SDM
        val = 0; addr += 1;
        val2 = 0; val3 = 0;
        addr2 += 1; addr3 += 1;
        for (k = 1; k <= PPR_MAX_TX_CHAINS; k++) {
            if (k == 1 && PPR_MAX_TX_CHAINS >= WL_TX_CHAINS_3) {
                cnt = ppr_get_he_ru_mcs(clm_ru_txpwr_limit, idx, WL_TX_NSS_3,
                        WL_TX_MODE_NONE, WL_TX_CHAINS_3, &he);
            } else if (k > 1) {
                cnt = ppr_get_he_ru_mcs(clm_ru_txpwr_limit, idx, WL_TX_NSS_1,
                        WL_TX_MODE_TXBF, k, &he);
            } else {
                cnt = 1; he.pwr[0] = 0;
            }
            if (cnt == 0) err = BCME_ERROR;
            val |= offset_calc(he.pwr[0], max_pwr) << ((k-1) << 3);
            //mcs3-5, group1 in half dB, mcs6-8, group2 in half dB
            val2 |= ((offset_calc(he.pwr[3], he.pwr[0])/2) |
                ((offset_calc(he.pwr[6], he.pwr[0])/2) << 4)) <<
                ((k-1) << 3);
            //mcs9-11, group3 in half dB
            val3 |= (offset_calc(he.pwr[9], he.pwr[0])/2) << ((k-1) << 3);
        }
        wlc_write_reg_obj(pi, addr, val);
        wlc_write_reg_obj(pi, addr2, val2);
        wlc_write_reg_obj(pi, addr3, val3);
        PHY_TXPWR(("HE RU%d 1ss_TXBF/3ss_SDM = %x\n", idx, val));

        // 2ss_SPEXP/SDM
        val = 0; addr += 1;
        val2 = 0; val3 = 0;
        addr2 += 1; addr3 += 1;
        for (k = 1; k <= PPR_MAX_TX_CHAINS; k++) {
            if (k == 1 && PPR_MAX_TX_CHAINS >= WL_TX_CHAINS_4) {
                cnt = ppr_get_he_ru_mcs(clm_ru_txpwr_limit, idx, WL_TX_NSS_3,
                        WL_TX_MODE_NONE, WL_TX_CHAINS_4, &he);
            } else if (k > 1) {
                cnt = ppr_get_he_ru_mcs(clm_ru_txpwr_limit, idx, WL_TX_NSS_2,
                        WL_TX_MODE_NONE, k, &he);
            } else {
                cnt = 1; he.pwr[0] = 0;
            }
            if (cnt == 0) err = BCME_ERROR;
            val |= offset_calc(he.pwr[0], max_pwr) << ((k-1) << 3);
            //mcs3-5, group1 in half dB, mcs6-8, group2 in half dB
            val2 |= ((offset_calc(he.pwr[3], he.pwr[0])/2) |
                ((offset_calc(he.pwr[6], he.pwr[0])/2) << 4)) <<
                ((k-1) << 3);
            //mcs9-11, group3 in half dB
            val3 |= (offset_calc(he.pwr[9], he.pwr[0])/2) << ((k-1) << 3);
        }
        wlc_write_reg_obj(pi, addr, val);
        wlc_write_reg_obj(pi, addr2, val2);
        wlc_write_reg_obj(pi, addr3, val3);
        PHY_TXPWR(("HE RU%d 2ss_SPEXP/SDM = %x\n", idx, val));

        // 2ss_SPEXP/TXBF_SPEXP1
        val = 0; addr += 1;
        val2 = 0; val3 = 0;
        addr2 += 1; addr3 += 1;
        for (k = 1; k <= PPR_MAX_TX_CHAINS; k++) {
            if (k == 1 && PPR_MAX_TX_CHAINS >= WL_TX_CHAINS_4) {
                cnt = ppr_get_he_ru_mcs(clm_ru_txpwr_limit, idx, WL_TX_NSS_3,
                        WL_TX_MODE_TXBF, WL_TX_CHAINS_4, &he);
            } else if (k == 2 && PPR_MAX_TX_CHAINS >= WL_TX_CHAINS_4) {
                cnt = ppr_get_he_ru_mcs(clm_ru_txpwr_limit, idx, WL_TX_NSS_4,
                        WL_TX_MODE_NONE, WL_TX_CHAINS_4, &he);
            } else  if (k > 2) {
                cnt = ppr_get_he_ru_mcs(clm_ru_txpwr_limit, idx, WL_TX_NSS_2,
                        WL_TX_MODE_TXBF, k, &he);
            } else {
                cnt = 1; he.pwr[0] = 0;
            }
            if (cnt == 0) err = BCME_ERROR;
            val |= offset_calc(he.pwr[0], max_pwr) << ((k-1) << 3);
            //mcs3-5, group1 in half dB, mcs6-8, group2 in half dB
            val2 |= ((offset_calc(he.pwr[3], he.pwr[0])/2) |
                ((offset_calc(he.pwr[6], he.pwr[0])/2) << 4)) <<
                ((k-1) << 3);
            //mcs9-11, group3 in half dB
            val3 |= (offset_calc(he.pwr[9], he.pwr[0])/2) << ((k-1) << 3);
        }
        wlc_write_reg_obj(pi, addr, val);
        wlc_write_reg_obj(pi, addr2, val2);
        wlc_write_reg_obj(pi, addr3, val3);
        PHY_TXPWR(("HE RU%d 2ss_SPEXP/TXBF_SPEXP1 = %x\n", idx, val));
    }

    /* TB-PPDU RU1992 */
    if (max_bw == 4) {
        addr = 0xf0038 + 29;
        addr2 = addr + HWPPR_PERRATETBL_OFFSET;
        addr3 = addr + (HWPPR_PERRATETBL_OFFSET << 1);
        val2 = 0; val3 = 0;
        for (k = 1; k <= 4; k++) { /* There are 4 offset entries */
            /* Copy from 160MHz SU */
            val = wlc_read_reg_obj(pi, addr + k);
            wlc_write_reg_obj(pi, addr + 72 + k, val);
            wlc_write_reg_obj(pi, addr2 + 72 + k, val2);
            wlc_write_reg_obj(pi, addr3 + 72 + k, val3);
        }
    }

    /* HE RE */
    addr = 0xf0038;
    /* Copy from 20MHz SU for HE RE-242 */
    val = wlc_read_reg_obj(pi, addr + 29);
    addr2 = addr + HWPPR_PERRATETBL_OFFSET;
    addr3 = addr + (HWPPR_PERRATETBL_OFFSET << 1);
    val2 = 0; val3 = 0;
    wlc_write_reg_obj(pi, addr + 105, val);
    wlc_write_reg_obj(pi, addr2 + 105, val2);
    wlc_write_reg_obj(pi, addr3 + 105, val3);
    /* Copy from 20MHz UB for HE RE-106 */
    val = wlc_read_reg_obj(pi, addr + 61);
    val2 = 0; val3 = 0;
    wlc_write_reg_obj(pi, addr + 106, val);
    wlc_write_reg_obj(pi, addr2 + 106, val2);
    wlc_write_reg_obj(pi, addr3 + 106, val3);
#endif /* WL11AX */

    if (err == BCME_ERROR)
        PPR_ERROR(("%s failed!\n", __FUNCTION__));

    return err;
}

/* TODO: The code could be optimized by moving the common code to phy/cmn */
/* [PHY_RE_ARCH] There are two functions: Bigger function wlc_phy_txpower_recalc_target
 * and smaller function phy_tpc_recalc_tgt which in turn call their phy specific functions
 * which are named in a haphazard manner. This needs to be cleaned up.
 */

#define CC_CODE_LEN_BYTES (3)
#define NUM_LOG_ENTRIES (8)

static void
wlc_phy_txpower_recalc_target_ac_big(phy_type_tpc_ctx_t *ctx, ppr_t *tx_pwr_target,
    ppr_t *srom_max_txpwr, ppr_t *reg_txpwr_limit, ppr_t *txpwr_targets)
{
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = info->pi;
    int8 tx_pwr_max = 0;
    int8 tx_pwr_min = 255;
    uint8 mintxpwr = 0;
    int8 maxpwr = 127;
    uint8 core;
    uint16 regVal;
    uint16 hwPprForceEn = 0;
    uint16 forcePwrOffset;
#ifdef WL_MU_TX
    uint8 bwtype;
    int8 floor_pwr = 127;
    ppr_vht_mcs_rateset_t srom_bl_pwr;
#endif /* WL_MU_TX */
#if (defined(WLTEST) || defined(WLPKTENG))
    int16 openloop_pwrctrl_delta;
    bool mac_enabled = FALSE;
#ifdef WLC_TXCAL
    int8 olpc_anchor = 0;
#endif /* WLC_TXCAL */
#endif
    bool min_tx_pwr_check = FALSE;
    chanspec_t chspec = pi->radio_chanspec;
    int tssi_visi_thresh;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);
    uint8 qdb_cap;
    /* Combine user target, regulatory limit, SROM/HW/board limit and power
     * percentage to get a tx power target for each rate.
     */

    if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        if (info->ti->data->txpwroverride) {
            hwPprForceEn = 1;
            forcePwrOffset = 0;
        } else {
            hwPprForceEn = 0;
            forcePwrOffset = 0;
        }
        regVal = wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG0_OFFSET(pi));
        regVal = wlc_bitor_reg(forcePwrOffset, (regVal & 0x7f00), 0);
        regVal = wlc_bitor_reg(regVal, hwPprForceEn, 15);
        wlapi_bmac_write_ihr(pi->sh->physhim,
            D11_TXE_PPR_CFG0_OFFSET(pi), regVal);
    }

    FOREACH_CORE(pi, core) {
        /* The user target is the starting point for determining the transmit
         * power.  If pi->txoverride is true, then use the user target as the
         * tx power target.
         */
        ppr_set_cmn_val(tx_pwr_target, info->ti->data->tx_user_target);

#if defined(WLTEST) || defined(WL_EXPORT_TXPOWER)
        /* Only allow tx power override for internal or test builds. */
        if (!info->ti->data->txpwroverride)
#endif
        {
            /* Get board/hw limit */
            wlc_phy_txpower_sromlimit(pi, chspec, &mintxpwr, srom_max_txpwr, core);
#ifdef TXPWRBACKOFF
            if (info->phy_txpwrbackoff->enable) {
                wlc_phy_apply_txpwr_backoff_acphy(info, srom_max_txpwr);
            }
#endif /* TXPWRBACKOFF */
#ifdef WL_MU_TX
            if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                ACMAJORREV_33(pi->pubpi->phy_rev)) {
                if (CHSPEC_IS20(chspec)) {
                    bwtype = WL_TX_BW_20;
                } else if (CHSPEC_IS40(chspec)) {
                    bwtype = WL_TX_BW_40;
                } else if (CHSPEC_IS80(chspec)) {
                    bwtype = WL_TX_BW_80;
                } else if (CHSPEC_IS160(chspec)) {
                    bwtype = WL_TX_BW_160;
                } else {
                    bwtype = 0xff;
                }

                /* get the txpwr corresponds to c9s1 board limit
                 * as evm floor txpwr
                 */
                ppr_get_vht_mcs(srom_max_txpwr, bwtype,
                    WL_TX_NSS_1, WL_TX_MODE_NONE,
                    WL_TX_NSS_1, &srom_bl_pwr);
                floor_pwr = (floor_pwr > srom_bl_pwr.pwr[9])?
                    srom_bl_pwr.pwr[9]: floor_pwr;
            }
#endif /* WL_MU_TX */

            /* Save the Board Limit for Radio Debugability Radar */
            tx_pwr_max = ppr_get_max(srom_max_txpwr);
            pi->tpci->data->txpwr_max_boardlim_percore[core] = tx_pwr_max;

            if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
                /* Program HWPPR Tables */
                wlc_phy_set_hwppr_bl_tbl(info->pi, srom_max_txpwr,
                        tx_pwr_max, chspec);
                wlc_phy_set_hwppr_rl_tbl(info->pi, reg_txpwr_limit,
                        tx_pwr_max, chspec);
                //wlc_phy_zero_hwppr_tbl(pi);
                //wlc_phy_hwppr_en(pi, FALSE);
                wlc_phy_dump_hwppr_tbl(pi);
            }

            /* Common Code Start */
            /* Choose minimum of provided regulatory and board/hw limits */
            ppr_compare_min(srom_max_txpwr, reg_txpwr_limit);

            /* Subtract 4 (1.0db) for 4313(IPA) as we are doing PA trimming
             * otherwise subtract 6 (1.5 db)
             * to ensure we don't go over
             * the limit given a noisy power detector.  The result
             * is now a target, not a limit.
             */
            ppr_minus_cmn_val(srom_max_txpwr, pi->tx_pwr_backoff);

            /* Choose least of user and now combined regulatory/hw targets */
            ppr_compare_min(tx_pwr_target, srom_max_txpwr);

            /* Board specific fix reduction */

            /* Apply max power percentage cap */
            if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
                phy_info_acphy_t *pi_ac =
                    (phy_info_acphy_t *)pi->u.pi_acphy;
                if (pi->tpci->data->txpwr_percent < 100) {
                /* apply pwr_percent to max pwr and cap each rate */
                    qdb_cap = ppr_get_max(tx_pwr_target) -
                    wlc_phy_percent_to_qdb(pi->tpci->data->txpwr_percent);
                    if (qdb_cap < wlc_phy_tssivisible_thresh_acphy(pi)) {
                        qdb_cap = wlc_phy_tssivisible_thresh_acphy(pi);
                    }
                    pi_ac->tpci->txpwr_cap[core] = qdb_cap;
                    wlc_phy_txpwr_cap_enable(pi, ENABLE);
                } else {
                    pi_ac->tpci->txpwr_cap[core] = 127;
                }
            }

            /* Apply power output degrade */
            if (pi->tpci->data->txpwr_degrade != 0) {
                qdb_cap = MAX((ppr_get_max(tx_pwr_target)
                        - pi->tpci->data->txpwr_degrade),
                        wlc_phy_tssivisible_thresh_acphy(pi));
                ppr_apply_max(tx_pwr_target, qdb_cap);
            }

            /* Common Code End */

            /* Enforce min power and save result as power target.
             * LCNPHY references off the minimum so this is not appropriate for it.
             */

            maxpwr = MIN(maxpwr,
                wlc_phy_calc_ppr_pwr_cap_acphy(pi,
                core, ppr_get_max(tx_pwr_target)));
            mintxpwr = wlc_phy_txpwrctrl_update_minpwr_acphy(pi);
            PHY_INFORM(("%s, core %d, maxpwr %d, mintxpwr %d\n",
                __FUNCTION__, core, maxpwr, (int8)mintxpwr));
            if (maxpwr >= (int8)mintxpwr) {
#ifdef WLC_TXCAL
                if (!phy_tssical_get_olpc_idx_valid_in_use(pi->tssicali))
#endif /* WLC_TXCAL */
                {
                    /* maxpwr is the max power among all rates
                     * (min accross cores)
                     * mintxpwr is the visibility threshold
                     * Rates will be disabled ONLY if some rates
                     * are above visibility thresh and some below.
                     * If all rates are above visi thresh, no OLPC.
                     * If all rates are below visi thresh,
                     *     disable OLPC, dont disable rates.
                     * If LUT txcal based OLPC is used, use OLPC
                     *     but don't disable rates.
                     */
                    ppr_force_disabled(tx_pwr_target, mintxpwr);
                }
            }
        }

        if (ACMAJORREV_130(pi->pubpi->phy_rev) && ACMINORREV_1(pi)) {
            /* for 6715-HWPPR, use max board_limit set earlier */
            if (wlc_phy_hwppr_ison(pi)) {
                tx_pwr_max = pi->tpci->data->txpwr_max_boardlim_percore[core];
                regVal = wlapi_bmac_read_ihr(pi->sh->physhim,
                    D11_TXE_PPR_CFG1_OFFSET(pi));
                regVal = regVal >> 8;
                regVal = wlc_bitor_reg(tx_pwr_max, regVal, 8);
                wlapi_bmac_write_ihr(pi->sh->physhim,
                    D11_TXE_PPR_CFG1_OFFSET(pi), regVal);

                PHY_TXPWR(("%s: HW-PPR is enabled\n", __FUNCTION__));
            }
            else {
                tx_pwr_max = ppr_get_max(tx_pwr_target);
            }
        } else {
            tx_pwr_max = ppr_get_max(tx_pwr_target);
        }

        if (tx_pwr_max < (pi->min_txpower * WLC_TXPWR_DB_FACTOR)) {
            tx_pwr_max = pi->min_txpower * WLC_TXPWR_DB_FACTOR;
        }
        /* Ensure that the Max power is at least equal to the TSSI
         * visbility threshold.
         */
        tssi_visi_thresh = wlc_phy_tssivisible_thresh_acphy(pi);
        if (tx_pwr_max < tssi_visi_thresh) {
            tx_pwr_max = tssi_visi_thresh;
        }

        tx_pwr_min = ppr_get_min(tx_pwr_target, mintxpwr);

#ifdef WL11AX
        wlc_phy_set_ru_txpwr_offset(info, mintxpwr, &tx_pwr_max, &tx_pwr_min, core);
#endif /* WL11AX */

        /* Common Code Start */
        /* Now calculate the tx_power_offset and update the hardware... */
        pi->tx_power_max_per_core[core] = tx_pwr_max;
        pi->tx_power_min_per_core[core] = tx_pwr_min;

#ifdef WLTXPWR_CACHE
        if (wlc_phy_txpwr_cache_is_cached(pi->txpwr_cache, pi->radio_chanspec) == TRUE) {
            wlc_phy_set_cached_pwr_min(pi->txpwr_cache, pi->radio_chanspec, core,
                tx_pwr_min);
            wlc_phy_set_cached_pwr_max(pi->txpwr_cache, pi->radio_chanspec, core,
                tx_pwr_max);
            wlc_phy_set_cached_boardlim(pi->txpwr_cache, pi->radio_chanspec, core,
                pi->tpci->data->txpwr_max_boardlim_percore[core]);
        }
#endif
        pi->openlp_tx_power_min = tx_pwr_min;
        info->ti->data->txpwrnegative = 0;

        min_tx_pwr_check = CHSPEC_IS2G(chspec) ? (tx_pwr_min <
            (PHY_TXPWR_MIN_ACPHY_EPA_2G * WLC_TXPWR_DB_FACTOR)) :
                ((tx_pwr_min < PHY_TXPWR_MIN_ACPHY_EPA_5G * WLC_TXPWR_DB_FACTOR));
        if (min_tx_pwr_check) {
            /* PHY_FATAL_ERROR_MESG((" %s: TxMinPwr less than Supported MinPower\n",
             *        __FUNCTION__));
             * PHY_FATAL_ERROR(pi, PHY_RC_TXPOWER_LIMITS);
             */
            /* Log the channel & country info when this issue occurs.
             * 1st element in the log arrays pi->ccode[] and pi->chanspec_array[]
             * correspond to the latest failure case.
              */
            int8 length, i;
            uint16 *chanspec_array = pi->tpci->data->chanspec_array;
            char *ccode = pi->tpci->data->ccode;
            pi->tpci->data->minpwrlimit_fail++;
            if (pi->tpci->data->ccode_ptr) {
                length = CC_CODE_LEN_BYTES;
                for (i = NUM_LOG_ENTRIES-2; i >= 0; i--) {
                    strncpy(ccode + length*(i+1), ccode + length*i,
                            CC_CODE_LEN_BYTES);
                    chanspec_array[i+1] = chanspec_array[i];
                }
                strncpy(ccode, pi->tpci->data->ccode_ptr, length);
            }
            chanspec_array[0] = pi->radio_chanspec;
        }

        PHY_NONE(("wl%d: %s: min %d max %d\n", pi->sh->unit, __FUNCTION__,
            tx_pwr_min, tx_pwr_max));

        PHY_TXPWR(("WL_EAP wl%d: %s: channel %d, min %d (%d.%d dBm) max %d (%d.%d dBm)\n",
            pi->sh->unit, "recalc_target", CHSPEC_CHANNEL(chspec),
            tx_pwr_min,
            tx_pwr_min >> 2, 0 == (tx_pwr_min & 3) ? 0 : 1 == (tx_pwr_min & 3) ? 25 :
            2 == (tx_pwr_min & 3) ? 50 : 75,
            tx_pwr_max,
            tx_pwr_max >> 2, 0 == (tx_pwr_max & 3) ? 0 : 1 == (tx_pwr_max & 3) ? 25 :
            2 == (tx_pwr_max & 3) ? 50 : 75));

        /* determinate the txpower offset by either of the following 2 methods:
         * txpower_offset = txpower_max - txpower_target OR
         * txpower_offset = txpower_target - txpower_min
         * return curpower for last core loop since we are now checking
         * MIN_cores(MAX_rates(power)) for rate disabling
         * Only the last core loop info is valid
         */
        if (core == PHYCORENUM((pi)->pubpi->phy_corenum) - 1) {
            info->ti->data->curpower_display_core = stf_shdata->phytxchain;
            if (txpwr_targets != NULL)
                ppr_copy_struct(tx_pwr_target, txpwr_targets);
        }
        /* Common Code End */

        ppr_cmn_val_minus(tx_pwr_target, pi->tx_power_max_per_core[core]);
        ppr_compare_max(pi->tx_power_offset, tx_pwr_target);

        if (TINY_RADIO(pi)) {
            phy_tpc_cck_corr(pi);
        }
    }    /* CORE loop */

    /* Common Code Start */
#ifdef WLTXPWR_CACHE
    if (wlc_phy_txpwr_cache_is_cached(pi->txpwr_cache, pi->radio_chanspec) == TRUE) {
        wlc_phy_set_cached_pwr(pi->sh->osh, pi->txpwr_cache, pi->radio_chanspec,
            TXPWR_CACHE_POWER_OFFSETS, pi->tx_power_offset);
    }
#endif
    /*
     * PHY_ERROR(("#####The final power offset limit########\n"));
     * ppr_mcs_printf(pi->tx_power_offset);
     */
    /* Common Code End */

#if (defined(WLTEST) || defined(WLPKTENG))
    /* for 4360A/B0, when targetPwr is out of the tssi visibility range,
     * force the power offset to be the delta between the lower bound of visibility
     * range and the targetPwr
     */
    if (info->ti->data->txpwroverrideset) {
        if (ACMAJORREV_0(pi->pubpi->phy_rev) ||
            ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev) ||
            ACMAJORREV_128(pi->pubpi->phy_rev)) {
            openloop_pwrctrl_delta = wlc_phy_tssivisible_thresh_acphy(pi) -
                info->ti->data->tx_user_target;

            if (openloop_pwrctrl_delta > 0) {
                if (R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC)
                    mac_enabled = TRUE;
                ppr_set_cmn_val(pi->tx_power_offset, 0);

#ifdef WLC_TXCAL
                olpc_anchor = phy_tssical_get_olpc_anchor(pi->tssicali);
#endif /* WLC_TXCAL */

                phy_tpc_recalc_tgt(pi->tpci);

                /* Stop PKTENG if already running.. */
                if (!mac_enabled)
                    wlapi_enable_mac(pi->sh->physhim);
                wlapi_bmac_pkteng(pi->sh->physhim, 0, 0);
                OSL_DELAY(100);

                /* Turn ON Power Control */
                wlapi_suspend_mac_and_wait(pi->sh->physhim);
                wlc_phy_txpwrctrl_enable_acphy(pi, 1);

                FOREACH_CORE(pi, core) {
#ifdef WLC_TXCAL
                    if ((olpc_anchor == 0) || (olpc_anchor
                        < phy_tssical_get_olpc_threshold(pi->tssicali))) {
                        pi->tx_power_max_per_core[core] =
                            wlc_phy_tssivisible_thresh_acphy(pi) & 0xff;
                    } else {
                        pi->tx_power_max_per_core[core]
                            = olpc_anchor;
                        openloop_pwrctrl_delta = olpc_anchor -
                            info->ti->data->tx_user_target;
                    }
#endif /* WLC_TXCAL */
                    /* Set TX Power here for PKTENG */
                    wlc_phy_txpwrctrl_set_target_acphy
                        (pi, pi->tx_power_max_per_core[core], core);
                }
#ifdef WLC_TXCAL
                /* If table based txcal anchor txidx is used for OLPC, No need to
                 * send out training packets in txpwr ovr mode
                 * Init idx will be set based on cal info from
                 * pi->olpci->olpc_anchor_idx
                 */
                if (!phy_tssical_get_olpc_idx_valid_in_use(pi->tssicali))
#endif /* WLC_TXCAL */
                {
                    wlapi_enable_mac(pi->sh->physhim);

                    /* Start PKTENG to settle TX power Control */
                    wlapi_bmac_pkteng(pi->sh->physhim, 1, 100);
                    OSL_DELAY(1000);
                    if (!mac_enabled)
                        wlapi_suspend_mac_and_wait(pi->sh->physhim);
                    OSL_DELAY(100);
                }

                /* Toggle Power Control to save off base index */
                wlc_phy_txpwrctrl_enable_acphy(pi, 0);
                if (openloop_pwrctrl_delta > 127) {
                    openloop_pwrctrl_delta = 127;
#ifdef WLC_TXCAL
                } else if (openloop_pwrctrl_delta < -128) {
                    openloop_pwrctrl_delta = -128;
#endif /* WLC_TXCAL */
                }
                ppr_set_cmn_val(pi->tx_power_offset,
                    (int8) openloop_pwrctrl_delta);
#ifdef WLTXPWR_CACHE
                wlc_phy_txpwr_cache_invalidate(pi->txpwr_cache);
#endif  /* WLTXPWR_CACHE */
                PHY_NONE(("###offset: %d targetPwr %d###\n",
                    openloop_pwrctrl_delta,
                    pi->tx_power_max_per_core[0]));
            }
        } else if (ACMAJORREV_47_129_130(pi->pubpi->phy_rev) ||
                ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
            openloop_pwrctrl_delta = wlc_phy_tssivisible_thresh_acphy(pi) -
                info->ti->data->tx_user_target;
            if (openloop_pwrctrl_delta > 0 && pi->u.pi_acphy->tpci->olpc_mode_caldone) {
                FOREACH_CORE(pi, core) {
                    pi->tx_power_max_per_core[core] =
                        wlc_phy_tssivisible_thresh_acphy(pi) & 0xff;
                    /* Set TX Power here for PKTENG */
                    wlc_phy_txpwrctrl_set_target_acphy
                        (pi, pi->tx_power_max_per_core[core], core);
                }

                if (openloop_pwrctrl_delta > 127) {
                    openloop_pwrctrl_delta = 127;
                } else if (openloop_pwrctrl_delta < -128) {
                    openloop_pwrctrl_delta = -128;
                }
                ppr_set_cmn_val(pi->tx_power_offset,
                    (int8) openloop_pwrctrl_delta);
#ifdef WLTXPWR_CACHE
                wlc_phy_txpwr_cache_invalidate(pi->txpwr_cache);
#endif  /* WLTXPWR_CACHE */
                PHY_NONE(("###offset: %d targetPwr %d###\n",
                    openloop_pwrctrl_delta,
                    pi->tx_power_max_per_core[0]));
            }
        }
        info->ti->data->txpwroverrideset = FALSE;
    }
#endif

#if defined(WL_MU_TX) && defined(WLVASIP)
    if ((ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        ACMAJORREV_47_51_129_130_131(pi->pubpi->phy_rev))) {
        /* '-6' (1/4dB step) stands for 1.5dB backoff for txpwrctrl error */
        floor_pwr = tx_pwr_max - (floor_pwr - 6);
        floor_pwr = (floor_pwr >= 0)? floor_pwr: 0;
        phy_ac_tpc_offload_ppr_to_svmp(pi, pi->tx_power_offset, (int16) floor_pwr);
    }
#endif /* WL_MU_TX && WLVASIP */
    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        if (pi->u.pi_acphy->tpci->txpwr_cap_status == ENABLE) {
            wlc_phy_txpwr_cap_enable(pi, ENABLE);
        }
    }
}

#ifdef WL_SAR_SIMPLE_CONTROL
static void
BCMATTACHFN(wlc_phy_nvram_dynamicsarctrl_read)(phy_info_t *pi)
{
/* Nvram parameter to get sarlimits customized by user
 * Value interpetation:
 *  dynamicsarctrl_2g = 0x[core3][core2][core1][core0]
 * each core# has the bitmask followings:
 * 8th bit : 0 - sarlimit enable / 1 - sarlimit disable
 * 0 ~ 7 bits : qdbm power val (0x7f as a maxumum)
 */
    char phy_var_name[25];
    phy_tpc_data_t *data = pi->tpci->data;
    (void)snprintf(phy_var_name, sizeof(phy_var_name), "dynamicsarctrl_2g");
    if ((PHY_GETVAR_SLICE(pi, phy_var_name)) != NULL) {
        data->cfg->dynamic_sarctrl_2g = (uint32)PHY_GETINTVAR_SLICE(pi, phy_var_name);
    }

    (void)snprintf(phy_var_name, sizeof(phy_var_name), "dynamicsarctrl_5g");
    if ((PHY_GETVAR_SLICE(pi, phy_var_name)) != NULL) {
        data->cfg->dynamic_sarctrl_5g = (uint32)PHY_GETINTVAR_SLICE(pi, phy_var_name);
    }

    (void)snprintf(phy_var_name, sizeof(phy_var_name), "dynamicsarctrl_2g_2");
    if ((PHY_GETVAR_SLICE(pi, phy_var_name)) != NULL) {
        data->cfg->dynamic_sarctrl_2g_2 = (uint32)PHY_GETINTVAR_SLICE(pi, phy_var_name);
    } else {
        data->cfg->dynamic_sarctrl_2g_2 = data->cfg->dynamic_sarctrl_2g;
    }

    (void)snprintf(phy_var_name, sizeof(phy_var_name), "dynamicsarctrl_5g_2");
    if ((PHY_GETVAR_SLICE(pi, phy_var_name)) != NULL) {
        data->cfg->dynamic_sarctrl_5g_2 = (uint32)PHY_GETINTVAR_SLICE(pi, phy_var_name);
    } else {
        data->cfg->dynamic_sarctrl_5g_2 = data->cfg->dynamic_sarctrl_5g;
    }
#if (MAX_RSDB_MAC_NUM > 1)
    /* RSDB PARAMETERS */
    (void)snprintf(phy_var_name, sizeof(phy_var_name), "rsdb_dynamicsarctrl_2g");
    if ((PHY_GETVAR_SLICE(pi, phy_var_name)) != NULL) {
        data->cfg->rsdb_dynamic_sarctrl_2g = (uint32)PHY_GETINTVAR_SLICE(pi, phy_var_name);
    } else {
        data->cfg->rsdb_dynamic_sarctrl_2g = data->cfg->dynamic_sarctrl_2g;
    }

    (void)snprintf(phy_var_name, sizeof(phy_var_name), "rsdb_dynamicsarctrl_5g");
    if ((PHY_GETVAR_SLICE(pi, phy_var_name)) != NULL) {
        data->cfg->rsdb_dynamic_sarctrl_5g = (uint32)PHY_GETINTVAR_SLICE(pi, phy_var_name);
    } else {
        data->cfg->rsdb_dynamic_sarctrl_5g = data->cfg->dynamic_sarctrl_5g;
    }
    (void)snprintf(phy_var_name, sizeof(phy_var_name), "rsdb_dynamicsarctrl_2g_1");
    if ((PHY_GETVAR_SLICE(pi, phy_var_name)) != NULL) {
        data->cfg->rsdb_dynamic_sarctrl_2g_1 =
            (uint32)PHY_GETINTVAR_SLICE(pi, phy_var_name);
    } else {
        data->cfg->rsdb_dynamic_sarctrl_2g_1 = 0;
    }

    (void)snprintf(phy_var_name, sizeof(phy_var_name), "rsdb_dynamicsarctrl_5g_1");
    if ((PHY_GETVAR_SLICE(pi, phy_var_name)) != NULL) {
        pi->rsdb_dynamic_sarctrl_5g_1 =
            (uint32)PHY_GETINTVAR_SLICE(pi, phy_var_name);
    } else {
        pi->rsdb_dynamic_sarctrl_5g_1 = 0;
    }

    (void)snprintf(phy_var_name, sizeof(phy_var_name), "rsdb_dynamicsarctrl_2g_2");
    if ((PHY_GETVAR_SLICE(pi, phy_var_name)) != NULL) {
        data->cfg->rsdb_dynamic_sarctrl_2g_2 =
            (uint32)PHY_GETINTVAR_SLICE(pi, phy_var_name);
    } else {
        data->cfg->rsdb_dynamic_sarctrl_2g_2 = data->cfg->rsdb_dynamic_sarctrl_2g;
    }

    (void)snprintf(phy_var_name, sizeof(phy_var_name), "rsdb_dynamicsarctrl_5g_2");
    if ((PHY_GETVAR_SLICE(pi, phy_var_name)) != NULL) {
        data->cfg->rsdb_dynamic_sarctrl_5g_2 =
            (uint32)PHY_GETINTVAR_SLICE(pi, phy_var_name);
    } else {
        data->cfg->rsdb_dynamic_sarctrl_5g_2 = data->cfg->rsdb_dynamic_sarctrl_5g;
    }
#endif /* MAX_RSDB_MAC_NUM */
}

static bool
wlc_phy_isenabled_dynamic_sarctrl(phy_info_t *pi)
{
    /* api function to check sar_enable status */
    return wlapi_get_sarenable(pi->sh->physhim);
}
#endif /* WL_SAR_SIMPLE_CONTROL */

static bool
phy_ac_tpc_wd(phy_wd_ctx_t *ctx)
{
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = info->pi;
    bool suspend = FALSE;
#if defined(WLC_TXCAL)
    int16 txcal_delta_temp;
    int16 txcal_currtemp = 0;
#endif  /* WLC_TXCAL */

#if defined(PREASSOC_PWRCTRL)
    /* Suspend MAC if haven't done so */
    wlc_phy_conditional_suspend(pi, &suspend);
    phy_ac_tpc_shortwindow_upd(ctx, FALSE);
    /* Resume MAC */
    wlc_phy_conditional_resume(pi, &suspend);
#endif /* PREASSOC_PWRCTRL */

    if (pi->tpci->data->tx_pwr_ctrl_damping_en && !info->txpwr_damping_factor_set &&
        (PHY_TOTAL_TX_FRAMES(pi) -
            pi->tpci->data->base_index_init_invalid_frame_cnt >= 150)) {
        suspend = !(R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC);
        if (!suspend)
            wlapi_suspend_mac_and_wait(pi->sh->physhim);
        MOD_PHYREG(pi, TxPwrCtrlDamping, DeltaPwrDamp, pi->tpci->data->deltapwrdamp);
        if (!suspend)
            wlapi_enable_mac(pi->sh->physhim);
        info->txpwr_damping_factor_set = 1;
    }

#if defined(WLC_TXCAL)
    if (phy_tssical_get_olpc_idx_valid_in_use(pi->tssicali)) {
        int16 last_calc_temp = phy_tssical_get_last_calc_temp(pi->tssicali);
        wlapi_suspend_mac_and_wait(pi->sh->physhim);
        /* Recalculate baseindex if temperature changed for txcal based OLPC */
        txcal_currtemp = wlc_phy_tempsense_acphy(pi);
        txcal_delta_temp =
            (txcal_currtemp > last_calc_temp)?
            txcal_currtemp - last_calc_temp:
            last_calc_temp - txcal_currtemp;
        if (txcal_delta_temp >= TXCAL_OLPC_RECALC_TEMP) {
            wlc_phy_txcal_olpc_idx_recal_acphy(pi, TRUE);
            PHY_TXPWR(("%s TXCAL_OLPC: temp change exceed thresh\n",
                __FUNCTION__));
            phy_tssical_set_last_calc_temp(pi->tssicali, txcal_currtemp);
        }
        wlapi_enable_mac(pi->sh->physhim);
    }
#endif  /* WLC_TXCAL */

    return TRUE;
}

int16
wlc_phy_calc_adjusted_cap_rgstr_acphy(phy_info_t *pi, uint8 core)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
#ifdef WL_SARLIMIT
    return MIN(pi->tpci->data->sarlimit[core],
        pi->tx_power_max_per_core[core] + pi_ac->tpci->txpwr_offset[core]);
#else
    return pi->tx_power_max_per_core[core] + pi_ac->tpci->txpwr_offset[core];
#endif /* WL_SARLIMIT */
}

/* ********************************************* */
/*                Internal Definitions                    */
/* ********************************************* */
#define PWRCTRL_SHORTW_AVG 1
#define PWRCTRL_LONGW_AVG 4
#define PWRCTRL_MIN_INIT_IDX 5
#define PWRCTRL_MAX_INIT_IDX 127
#define SAMPLE_TSSI_AFTER_100_SAMPLES 100
#define SAMPLE_TSSI_AFTER_110_SAMPLES 110
#define SAMPLE_TSSI_AFTER_111_SAMPLES 111
#define SAMPLE_TSSI_AFTER_115_SAMPLES 115
#define SAMPLE_TSSI_AFTER_150_SAMPLES 150
#define SAMPLE_TSSI_AFTER_160_SAMPLES 160
#define SAMPLE_TSSI_AFTER_170_SAMPLES 170
#define SAMPLE_TSSI_AFTER_185_SAMPLES 185
#define SAMPLE_TSSI_AFTER_200_SAMPLES 200
#define SAMPLE_TSSI_AFTER_220_SAMPLES 220
#define SAMPLE_TSSI_AFTER_190_SAMPLES 190

#define ACPHY_TBL_ID_ESTPWRLUTS(core)    \
    (((core == 0) ? ACPHY_TBL_ID_ESTPWRLUTS0 : \
    ((core == 1) ? ACPHY_TBL_ID_ESTPWRLUTS1 : \
    ((core == 2) ? ACPHY_TBL_ID_ESTPWRLUTS2 : ACPHY_TBL_ID_ESTPWRLUTS3))))
#define NUM_PA_PARAM 3

void
wlc_phy_read_patrim_srom(phy_info_t *pi, int16 *start, uint16 len)
{
    srom11_pwrdet_t *pwrdet = pi->pwrdet_ac;
    uint8 core;
    uint8 band;
    uint16 offset;

    /* Updated PA trim support for SROM16
     * Store full PA params directly, rather than store offsets only
     */
    FOREACH_CORE(pi, core) {
        offset = core * CH_5G_4BAND * NUM_PA_PARAM;
        for (band = 0; band < CH_5G_4BAND; band++) {
            pwrdet->pwrdet_a1[core][band+1] = start[offset++];
            pwrdet->pwrdet_b0[core][band+1] = start[offset++];
            pwrdet->pwrdet_b1[core][band+1] = start[offset++];
        }
        /* 4357 : num core = 2 */
        offset = 2 * CH_5G_4BAND * NUM_PA_PARAM + core * NUM_PA_PARAM;
        pwrdet->pwrdet_a1[core][WL_CHAN_FREQ_RANGE_2G] = start[offset++];
        pwrdet->pwrdet_b0[core][WL_CHAN_FREQ_RANGE_2G] = start[offset++];
        pwrdet->pwrdet_b1[core][WL_CHAN_FREQ_RANGE_2G] = start[offset++];
    }

#ifdef VER_0
    /* Origianl PA trim support for SROM16
     * Store delta power only, and calculate back PA params in PHY FW
     * There is mismatch observed between CAL and recalculated params due
     * missed resolution due to fixed point calculation. Will be removed
     * once new approach is blessed.
     */
    int16 delta;
    int32 prod;

    /* PA trim update formulas
        - a_{1,new} = a_{1,orig}
        - b_{0,new} = b_{0,orig} + 64Perr = b_{0,orig} + delta
        - b_{1,new} = b_{1,orig} + a_{1,orig}*Perr/32
                    = b_{1,orig} + a_{1,orig}*delta/(64*32)
    */

    FOREACH_CORE(pi, core) {
        for (band = 0; band < CH_5G_4BAND; band++) {
            delta = start[core*CH_5G_4BAND+band];
            pwrdet->pwrdet_b0[core][band+1] += delta;

            prod = pwrdet->pwrdet_a1[core][band+1]*delta;
            if (prod > 0)
                pwrdet->pwrdet_b1[core][band+1] += (int16)(prod >> 11);
            else
                pwrdet->pwrdet_b1[core][band+1] -= (int16)((-1*prod) >> 11);
        }
        delta = start[core+30];
        pwrdet->pwrdet_b0[core][WL_CHAN_FREQ_RANGE_2G] += delta;

        prod = pwrdet->pwrdet_a1[core][WL_CHAN_FREQ_RANGE_2G]*delta;
        if (prod > 0)
            pwrdet->pwrdet_b1[core][WL_CHAN_FREQ_RANGE_2G] += (int16)(prod >> 11);
        else
            pwrdet->pwrdet_b1[core][WL_CHAN_FREQ_RANGE_2G] -= (int16)((-1*prod) >> 11);
    }
#endif /* #ifdef VER_0 */
}

static void
wlc_phy_txpwrctrl_pwr_setup_acphy(phy_info_t *pi)
{
#define ESTPWRLUTS_TBL_OFFSET        0
#define ESTPWRSHFTLUTS_TBL_LEN        24
#define ESTPWRSHFTLUTS_TBL_OFFSET    0
#define BTCX_PU_OFF_TPC_HOLD        (1<<12)
#define MAXPWRRANGE_22DBM    88
#define MAXPWRRANGE_20DBM    80
#define MINPWRRANGE_11DBM    44

    uint8 stall_val;
    phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;
    phy_ac_tpc_info_t *ti = pi_ac->tpci;
    srom11_pwrdet_t *pwrdet = pi->pwrdet_ac;
    int8   target_min_limit;
    int16  a1[PAPARAM_SET_NUM], b0[PAPARAM_SET_NUM], b1[PAPARAM_SET_NUM];
    uint8  chan_freq_range, iidx, core_freq_segment_map;
    uint8  core, core2range, idx_set[2], k;
    int32  pwr_est;
    uint32 idx;
    uint16 regval[ESTPWRLUTS_TBL_LEN];
    uint32 shfttblval[ESTPWRSHFTLUTS_TBL_LEN];
    uint8  tssi_delay;
    uint32 pdoffs = 0;
    bool flag2rangeon;

#ifdef PREASSOC_PWRCTRL
    bool init_idx_carry_from_lastchan;
    uint8 step_size, prev_target_qdbm;
#endif
    struct _tp_qtrdbm {
        uint8 core;
        int8 target_pwr_qtrdbm;
    } tp_qtrdbm_each_core[PHY_CORE_MAX]; /* TP for each core */
    uint core_count = 0;
    phy_ac_chanmgr_data_t *ch_data = phy_ac_chanmgr_get_data(pi_ac->chanmgri);
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

#if defined(PHYCAL_CACHING)
    ch_calcache_t *ctx = NULL;
    ctx = wlc_phy_get_chanctx(pi, pi->radio_chanspec);
    BCM_REFERENCE(ctx);
#endif /* PHYCAL_CACHING */
    BCM_REFERENCE(stf_shdata);

    iidx = 0;

#ifdef WLC_TXCAL
    phy_tssical_set_txcal_status(pi->tssicali, 0);
#endif    /* WLC_TXCAL */

    flag2rangeon =
        ((CHSPEC_IS2G(pi->radio_chanspec) && pi->tpci->data->cfg->srom_tworangetssi2g) ||
        (CHSPEC_ISPHY5G6G(pi->radio_chanspec) &&
        pi->tpci->data->cfg->srom_tworangetssi5g)) && PHY_IPA(pi);
    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));
    PHY_CHANLOG(pi, __FUNCTION__, TS_ENTER, 0);

    if (BF3_TSSI_DIV_WAR(pi->u.pi_acphy) && ACMAJORREV_4(pi->pubpi->phy_rev)) {
        /* DIV_WAR is priority between DIV WAR & two range */
        flag2rangeon = 0;
    }
    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    ACPHY_REG_LIST_START
        ACPHY_DISABLE_STALL_ENTRY(pi)
        /* enable TSSI */
        MOD_PHYREG_ENTRY(pi, TSSIMode, tssiEn, 1)
        MOD_PHYREG_ENTRY(pi, TxPwrCtrlCmd, txPwrCtrl_en, 0)
    ACPHY_REG_LIST_EXECUTE(pi);

    /* Initialize ALL PA Param arrays a1, b0, b1 to be all zero */
    for (idx = 0; idx < PAPARAM_SET_NUM; idx++) {
        a1[idx] = 0;
        b0[idx] = 0;
        b1[idx] = 0;
    }

    if (RADIOREV(pi->pubpi->radiorev) == 4 || RADIOREV(pi->pubpi->radiorev) == 8 ||
        ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev) ||
        ACMAJORREV_4(pi->pubpi->phy_rev) || ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* 4360B0/B1/4350 using 0.5dB-step gaintbl, bbmult interpolation enabled */
        MOD_PHYREG(pi, TxPwrCtrlCmd, bbMultInt_en, 1);
#ifdef PREASSOC_PWRCTRL
        step_size = 2;
#endif
    } else if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        /* correct check should be use bbMultInt_en=1 if 0.5 dB gain table
         * bbMultInt_en=0 if 0.25 dB gain table
         */
        if (ROUTER_4349(pi)) {
            MOD_PHYREG(pi, TxPwrCtrlCmd, bbMultInt_en,
                !(phy_ac_tbl_get_data(pi_ac->tbli)->is_p25TxGainTbl));
        } else {
            if (PHY_IPA(pi)) {
                    MOD_PHYREG(pi, TxPwrCtrlCmd, bbMultInt_en, 0);
            } else {
                    MOD_PHYREG(pi, TxPwrCtrlCmd, bbMultInt_en, 1);
            }
        }
#ifdef PREASSOC_PWRCTRL
        step_size = 2;
#endif
    } else {
    /* disable bbmult interpolation
       to work with a 0.25dB step txGainTbl
    */
        MOD_PHYREG(pi, TxPwrCtrlCmd, bbMultInt_en, 0);
#ifdef PREASSOC_PWRCTRL
        step_size = 1;
#endif
    }

    FOREACH_CORE(pi, core) {
        core_freq_segment_map = ch_data->core_freq_mapping[core];

        /* Get pwrdet params from SROM for current subband */
        chan_freq_range = phy_ac_chanmgr_get_chan_freq_range(pi, pi->radio_chanspec,
            core_freq_segment_map);
        /* Check if 2 range should be activated for band */
        /* First load PA Param sets for corresponding band/frequemcy range */
        /* for all cores 0 to PHYNUMCORE()-1 */
        switch (chan_freq_range) {
        case WL_CHAN_FREQ_RANGE_2G:
        case WL_CHAN_FREQ_RANGE_5G_BAND0:
        case WL_CHAN_FREQ_RANGE_5G_BAND1:
        case WL_CHAN_FREQ_RANGE_5G_BAND2:
        case WL_CHAN_FREQ_RANGE_5G_BAND3:
            a1[core] =  pwrdet->pwrdet_a1[core][chan_freq_range];
            b0[core] =  pwrdet->pwrdet_b0[core][chan_freq_range];
            b1[core] =  pwrdet->pwrdet_b1[core][chan_freq_range];
            PHY_TXPWR(("wl%d: %s: pwrdet core%d: a1=%d b0=%d b1=%d\n",
                pi->sh->unit, __FUNCTION__, core,
                a1[core], b0[core], b1[core]));
            break;
        }

        /* Set cck pwr offset from nvram */
        if (CHSPEC_IS2G(pi->radio_chanspec) && (TINY_RADIO(pi) ||
                ACMAJORREV_128(pi->pubpi->phy_rev))) {
            int16 cckPwrOffset = pi->tpci->data->cfg->cckpwroffset[core];

            if (READ_PHYREGFLD(pi, perPktIdleTssiCtrlcck,
                base_index_cck_en) != 1) {
                cckPwrOffset -= pi->sromi->cckPwrIdxCorr;
            }

            MOD_PHYREGCEE(pi, TxPwrCtrlTargetPwr_path, core,
                          cckPwrOffset, cckPwrOffset);
        }
        /* Next if special consitions are met, load additional PA Param sets */
        /* for corresponding band/frequemcy range */
        if (flag2rangeon || (BF3_TSSI_DIV_WAR(pi_ac) &&
            (ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_4(pi->pubpi->phy_rev) ||
                ACMAJORREV_128(pi->pubpi->phy_rev)))) {
            /* If two-range TSSI scheme is enabled via flag2rangeon, */
            /* or if TSSI divergence WAR is enable for 4350, load PHYCORENUM() */
            /* additional PA Param sets for corresponding band/frequemcy range. */
            /* For 4350, extra PA Params are used for CCK in 2G band or */
            /* for 40/80 MHz bands in 5G band */
            if ((phy_get_phymode(pi) == PHYMODE_RSDB) &&
                ACMAJORREV_4(pi->pubpi->phy_rev)) {
                core2range = TSSI_DIVWAR_INDX;
            } else {
                core2range = PHYCORENUM(pi->pubpi->phy_corenum) + core;
            }
            ASSERT(core2range < PAPARAM_SET_NUM);
            a1[core2range] = pwrdet->pwrdet_a1[core2range][chan_freq_range];
            b0[core2range] = pwrdet->pwrdet_b0[core2range][chan_freq_range];
            b1[core2range] = pwrdet->pwrdet_b1[core2range][chan_freq_range];

            PHY_TXPWR(("wl%d: %s: pwrdet %s core%d: a1=%d b0=%d b1=%d\n",
                       pi->sh->unit, __FUNCTION__,
                       (flag2rangeon) ? "2nd-TSSI" : "CCK/40/80MHz",
                       core, a1[core2range], b0[core2range], b1[core2range]));
        }
    }

    /* target power */
    wlc_phy_txpwrctrl_update_minpwr_acphy(pi);
    target_min_limit = pi->min_txpower * WLC_TXPWR_DB_FACTOR;

    FOREACH_ACTV_CORE(pi, stf_shdata->phytxchain, core) {
        int8 target_pwr_qtrdbm;
        target_pwr_qtrdbm = (int8)pi->tx_power_max_per_core[core];
        /* never target below the min threashold */
        if (target_pwr_qtrdbm < target_min_limit)
            target_pwr_qtrdbm = target_min_limit;

        if (ACMAJORREV_2(pi->pubpi->phy_rev)) {
            int16  tssifloor;

            chan_freq_range = phy_ac_chanmgr_get_chan_freq_range(pi,
                pi->radio_chanspec, PRIMARY_FREQ_SEGMENT);
            tssifloor = (int16)pwrdet->tssifloor[core][chan_freq_range];
            if ((tssifloor != 0x3ff) && (tssifloor != 0)) {
                uint8 maxpwr = wlc_phy_set_txpwr_clamp_acphy(pi, core);
                if (maxpwr < target_pwr_qtrdbm) {
                    target_pwr_qtrdbm = maxpwr;
                }
            }
        }

        tp_qtrdbm_each_core[core_count].core = core;
        tp_qtrdbm_each_core[core_count].target_pwr_qtrdbm = target_pwr_qtrdbm;
        ++core_count;
            /* PHY_ERROR(("####targetPwr: %d#######\n",
             * tp_qtrdbm_each_core[core_count].target_pwr_qtrdbm));
         */
    }

    /* determine pos/neg TSSI slope */
    if (CHSPEC_IS2G(pi->radio_chanspec)) {
        MOD_PHYREG(pi, TSSIMode, tssiPosSlope, pi->fem2g->tssipos);
    } else {
        MOD_PHYREG(pi, TSSIMode, tssiPosSlope, pi->fem5g->tssipos);
    }
    MOD_PHYREG(pi, TSSIMode, tssiPosSlope, 1);

    /* disable txpwrctrl during idleTssi measurement, etc */
    MOD_PHYREG(pi, TxPwrCtrlCmd, txPwrCtrl_en, 0);
    if (flag2rangeon) {
        if (ACMAJORREV_128(pi->pubpi->phy_rev))
            MOD_PHYREG(pi, TxPwrCtrlPwrRange2, maxPwrRange2, MAXPWRRANGE_22DBM);
        else
            MOD_PHYREG(pi, TxPwrCtrlPwrRange2, maxPwrRange2, MAXPWRRANGE_20DBM);
        MOD_PHYREG(pi, TxPwrCtrlPwrRange2, minPwrRange2, MINPWRRANGE_11DBM);
    } else {
        MOD_PHYREG(pi, TxPwrCtrlPwrRange2, maxPwrRange2, 0);
        MOD_PHYREG(pi, TxPwrCtrlPwrRange2, minPwrRange2, 1);
    }

#ifdef PREASSOC_PWRCTRL
    FOREACH_ACTV_CORE(pi, stf_shdata->phytxchain, core) {
        init_idx_carry_from_lastchan = (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) ?
                ti->pwr_ctrl_save->status_idx_carry_5g[core]
                : ti->pwr_ctrl_save->status_idx_carry_2g[core];
        if (!init_idx_carry_from_lastchan) {
            /* 4360B0 using 0.5dB-step gaintbl so start with a lower starting idx */
            if (RADIOID_IS(pi->pubpi->radioid, BCM2069_ID) &&
                ((RADIOREV(pi->pubpi->radiorev) == 4) ||
                (RADIOREV(pi->pubpi->radiorev) == 7) ||
                (RADIOREV(pi->pubpi->radiorev) == 8) ||
                (RADIOREV(pi->pubpi->radiorev) == 11) ||
                (RADIOREV(pi->pubpi->radiorev) == 13) ||
                ACMAJORREV_5(pi->pubpi->phy_rev))) {
                iidx = 20;
            } else {
                iidx = 50;
            }
#if defined(WLC_TXCAL)
            if (phy_tssical_get_olpc_idx_valid_in_use(pi->tssicali)) {
                wlc_phy_olpc_idx_tempsense_comp_acphy(pi, &iidx, core);
            }
#endif /* WLC_TXCAL */
            MOD_PHYREGCEE(pi, TxPwrCtrlInit_path, core, pwrIndex_init_path, iidx);
            if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
                ti->pwr_ctrl_save->status_idx_carry_5g[core] = TRUE;
            } else {
                ti->pwr_ctrl_save->status_idx_carry_2g[core] = TRUE;
            }
            /* XXX This is the handshake for te code that
               is put in for open loop power control
               128 means do NOT restore the value in the
               open loop function
            */
            ti->txpwrindex_hw_save[core] = 128;
        } else {
        /* set power index initial condition */
            int32 new_iidx;

            if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
                iidx = ti->pwr_ctrl_save->status_idx_5g[core];
                prev_target_qdbm = ti->pwr_ctrl_save->pwr_qdbm_5g[core];
                if ((ti->pwr_ctrl_save->stored_not_restored_5g[core])) {
                    /* XXX This is the handshake for
                       the code that is put in for open loop power control
                       128 means do NOT restore the value in the
                       open loop function
                    */
                    ti->pwr_ctrl_save->stored_not_restored_5g[core] = FALSE;
                    ti->txpwrindex_hw_save[core] = 128;
                }
            } else {
                iidx = ti->pwr_ctrl_save->status_idx_2g[core];
                prev_target_qdbm = ti->pwr_ctrl_save->pwr_qdbm_2g[core];
                if ((ti->pwr_ctrl_save->stored_not_restored_2g[core])) {
                    /* XXX This is the handshake for
                       the code that is put in for open loop power control
                       128 means do NOT restore the value in the
                       open loop function
                    */
                    ti->pwr_ctrl_save->stored_not_restored_2g[core] = FALSE;
                    ti->txpwrindex_hw_save[core] = 128;

                }
            }
            new_iidx = (int32)iidx + ((int32)tp_qtrdbm_each_core[core].target_pwr_qtrdbm
                      - prev_target_qdbm) / step_size;

            /* XXX Sanity Check
               XXX make sure iidx is within bounds to be safe
            */
            if (new_iidx < PWRCTRL_MIN_INIT_IDX) {
                iidx = PWRCTRL_MIN_INIT_IDX;
            } else if (new_iidx > PWRCTRL_MAX_INIT_IDX) {
                iidx = PWRCTRL_MAX_INIT_IDX;
            } else {
                iidx = (uint8)new_iidx;
            }
            if (!pi->tpci->data->ovrinitbaseidx) {
                MOD_PHYREGCEE(pi, TxPwrCtrlInit_path, core, pwrIndex_init_path,
                    iidx);
                if (!PHY_IPA(pi) && ACMAJORREV_4(pi->pubpi->phy_rev)) {
                /* JIRA: SW4349-1379
                 * FOR ePA chips, to improve CCK spectral mask margins, bphy scale
                 * is increased to 0x5F. this improves SM margins by >2dB, with an
                 * acceptable degradation in the EVM. This change also mandates the
                 * use of separate loop for cck TPC (target power control)
                 */
                    MOD_PHYREGCEE(pi, TxPwrCtrlInit_path, core,
                        pwr_index_init_cck_path, iidx);
                }
            }
        }
    }
#else
    if ((RADIOREV(pi->pubpi->radiorev) == 4) ||
        (RADIOREV(pi->pubpi->radiorev) == 8) ||
        (RADIOREV(pi->pubpi->radiorev) == 13) ||
        (ACMAJORREV_5(pi->pubpi->phy_rev))) {
        iidx = 20;
    } else {
        iidx = 50;
    }
#if defined(PHYCAL_CACHING)
    if (!ctx || !ctx->valid)
#endif /* PHYCAL_CACHING */
    {
        FOREACH_ACTV_CORE(pi, stf_shdata->phytxchain, core) {
#if defined(WLC_TXCAL)
            if (phy_tssical_get_olpc_idx_valid_in_use(pi->tssicali)) {
                wlc_phy_olpc_idx_tempsense_comp_acphy(pi,
                    &pi->tpci->data->base_index_init[core], core);
                if (TINY_RADIO(pi)) {
                    wlc_phy_olpc_idx_tempsense_comp_acphy(pi,
                        &pi->tpci->data->base_index_cck_init[core], core);
                }
            }
#endif /* WLC_TXCAL */
            if (!pi->tpci->data->ovrinitbaseidx) {
                MOD_PHYREGCEE(pi, TxPwrCtrlInit_path, core,
                    pwrIndex_init_path, iidx);
                if (!PHY_IPA(pi) && ACMAJORREV_4(pi->pubpi->phy_rev)) {
                /* JIRA: SW4349-1379
                 * FOR ePA chips, to improve CCK spectral mask margins, bphy scale
                 * is increased to 0x5F. this improves SM margins by >2dB, with an
                 * acceptable degradation in the EVM. This change also mandates the
                 * use of separate loop for cck TPC (target power control)
                 */
                    MOD_PHYREGCEE(pi, TxPwrCtrlInit_path, core,
                        pwr_index_init_cck_path, iidx);
                }
            }
        }
    }

#endif /* PREASSOC_PWRCTRL */
    /* MOD_PHYREG(pi, TxPwrCtrlIdleTssi, rawTssiOffsetBinFormat, 1); */

    /* sample TSSI at 7.5us */
    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        tssi_delay = wlc_phy_ac_set_tssi_params_majrev128(pi);
    } else {
        tssi_delay = wlc_phy_ac_set_tssi_params_legacy(pi);
    }
    MOD_PHYREG(pi, TxPwrCtrlNnum, Ntssi_delay, tssi_delay);
    if (pi->tpci->data->cfg->bphy_scale != 0) {
        MOD_PHYREG(pi, BphyControl3, bphyScale20MHz, pi->tpci->data->cfg->bphy_scale);
    }
    if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en &&
        ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* STF starts around count 410, 410 + (8 samples @~1.25Mhz = ~650
         * Adding margin to accomodate vdet filter induced delay
         * Count of 750 includes close to half of LTF
         * http://confluence.broadcom.com/x/1ohlFw
         */
        ACPHY_REG_LIST_START
                ACPHYREG_BCAST_ENTRY(pi, lowRateTssiDlyOFDM0, 750)
                ACPHYREG_BCAST_ENTRY(pi, lowRateTssiDlyBPHY0, 750)
                ACPHYREG_BCAST_ENTRY(pi, lowRateTssiDlyIDLE0, 750)
        ACPHY_REG_LIST_EXECUTE(pi);
        MOD_PHYREG(pi, lowRateTssi0, lb_lowrate_fifo_len_in_bits, 3);
        MOD_PHYREG(pi, lowRateTssi1, lb_lowrate_fifo_len_in_bits, 3);
    }
#if defined(PREASSOC_PWRCTRL)
    /* average over 2 or 16 packets */
    wlc_phy_pwrctrl_shortwindow_upd_acphy(pi, pi->tpci->data->channel_short_window);
#else
    MOD_PHYREG(pi, TxPwrCtrlNnum, Npt_intg_log2, PWRCTRL_LONGW_AVG);
#endif /* PREASSOC_PWRCTRL */

    /* decouple IQ comp and LOFT comp from Power Control */
    MOD_PHYREG(pi, TxPwrCtrlCmd, use_txPwrCtrlCoefsIQ, 0);
    if (ACREV_IS(pi->pubpi->phy_rev, 1) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
        if ((CHSPEC_ISPHY5G6G(pi->radio_chanspec) &&
            (pi->sromi->epa_on_during_txiqlocal) &&
            !(pi->sromi->precal_tx_idx)) ||
            (pi->sh->boardtype == BCM94360MCM5)) {
            MOD_PHYREG(pi, TxPwrCtrlCmd, use_txPwrCtrlCoefsLO, 0);
        } else {
            MOD_PHYREG(pi, TxPwrCtrlCmd, use_txPwrCtrlCoefsLO, 1);
        }

    } else {
        MOD_PHYREG(pi, TxPwrCtrlCmd, use_txPwrCtrlCoefsLO, 0);
    }

    /* adding maxCap for each Tx chain */
    if (0) {
        FOREACH_ACTV_CORE(pi, stf_shdata->phytxchain, core) {
            if (core == 0) {
                MOD_PHYREG(pi, TxPwrCapping_path0, maxTxPwrCap_path0, 80);
            } else if (core == 1) {
                MOD_PHYREG(pi, TxPwrCapping_path1, maxTxPwrCap_path1, 32);
            } else if (core == 2) {
                MOD_PHYREG(pi, TxPwrCapping_path2, maxTxPwrCap_path2, 32);
            }
        }
    }
#ifdef WL_SARLIMIT
    wlc_phy_set_sarlimit_acphy(ti);
#endif
    while (core_count > 0) {
        --core_count;
        if (ti->offset_targetpwr) {
            uint8 tgt_pwr_qdbm = tp_qtrdbm_each_core[core_count].target_pwr_qtrdbm;

            tgt_pwr_qdbm -= (ti->offset_targetpwr * WLC_TXPWR_DB_FACTOR);
            wlc_phy_txpwrctrl_set_target_acphy(pi, tgt_pwr_qdbm, 0);
        } else {
            /* set target powers */
            wlc_phy_txpwrctrl_set_target_acphy(pi,
                tp_qtrdbm_each_core[core_count].target_pwr_qtrdbm,
                tp_qtrdbm_each_core[core_count].core);
        }
        PHY_TXPWR(("wl%d: %s: txpwrctl[%d]: %d\n",
            pi->sh->unit, __FUNCTION__, tp_qtrdbm_each_core[core_count].core,
                      tp_qtrdbm_each_core[core_count].target_pwr_qtrdbm));
    }
#ifdef ENABLE_FCBS
    if (IS_FCBS(pi) && pi->phy_fcbs->FCBS_INPROG)
        pi->phy_fcbs->FCBS_INPROG = 0;
    else {
#endif
    /* load estimated power tables (maps TSSI to power in dBm)
     *    entries in tx power table 0000xxxxxx
     */

    if (BF3_TSSI_DIV_WAR(pi_ac) &&
         (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
         (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)))) {
        if ((CHSPEC_IS80(pi->radio_chanspec) || PHY_AS_80P80(pi, pi->radio_chanspec)) &&
            CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
            /* core 2 conatins 40-80mhz paparam
             * core 0 conatins 20mhz paparam
             */
            idx_set[0] = 2; idx_set[1] = 0;
        } else if (CHSPEC_IS160(pi->radio_chanspec)) {
            idx_set[0] = 2; idx_set[1] = 0; // FIXME
        } else if (CHSPEC_IS40(pi->radio_chanspec)) {
            /* core 1 conatins 40mhz paparam
             * core 0 contains 20mhz paparam
             */
            if (PHY_IPA(pi) && CHSPEC_IS2G(pi->radio_chanspec)) {
                idx_set[0] = 0; idx_set[1] = 1;
            } else {
                idx_set[0] = 1; idx_set[1] = 0;
            }
        } else {
            /* core 0 conatins 20mhz OFDM paparam
             * core 1 contains 20mhz CCK paparam
             */
            idx_set[0] = 0;
            idx_set[1] = (TINY_RADIO(pi)) ? 0 : 1;
        }

        for (idx = 0; idx < ESTPWRLUTS_TBL_LEN; idx++) {
            for (k = 0, regval[idx] = 0; k < 2; k++) {
                core = idx_set[k];
                if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                    ACMAJORREV_33(pi->pubpi->phy_rev) ||
                    (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                    !ACMAJORREV_128(pi->pubpi->phy_rev))) {
                    int32 num, den;
                    num = 8 * (16 * b0[core] + b1[core] * idx);
                    den = 32768 + a1[core] * idx;
                    pwr_est = MAX(((4 * num + den/2)/den), -8);
                    pwr_est = MIN(pwr_est, 0x7F);
                } else {
                    pwr_est = wlc_phy_tssi2dbm_acphy(pi, idx,
                        a1[core], b0[core], b1[core]);
                }
                regval[idx] |= (uint16)(pwr_est&0xff) << (8*k);
            }
        }
        /* Est Pwr Table is 128x16 Table for 4335 */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_ESTPWRLUTS0, ESTPWRLUTS_TBL_LEN,
                                  ESTPWRLUTS_TBL_OFFSET, 16, regval);
    } else if (BF3_TSSI_DIV_WAR(pi_ac) && (ACMAJORREV_2(pi->pubpi->phy_rev))) {
        if ((CHSPEC_IS80(pi->radio_chanspec) || CHSPEC_IS40(pi->radio_chanspec)) &&
            CHSPEC_ISPHY5G6G(pi->radio_chanspec) && !PHY_IPA(pi)) {
            /* core 2/3 contains 40-80mhz paparam
             * core 0/1 contains 20mhz paparam
             */
            idx_set[0] = 2; idx_set[1] = 0;
        } else if (CHSPEC_IS20(pi->radio_chanspec) &&
            CHSPEC_IS2G(pi->radio_chanspec) && PHY_IPA(pi)) {
            /* core 0,1 conatins 20mhz OFDM paparam
             * core 2,3 contains 20mhz CCK paparam
             */
            idx_set[0] = 0;  idx_set[1] = 2;
        } else {
            /* For 80P80 case,bits<7:0> should have 20MHz,bits<15:8> should have
            PAPARMS correspond to 40/80MHz, bits<23:16> are same as bits <15:8>
            currently.
            */
            if (CHSPEC_IS8080(pi->radio_chanspec)) {
                idx_set[0] = 0; idx_set[1] = 2;
            } else {
                idx_set[0] = 0; idx_set[1] = 0;
            }
        }

        FOREACH_ACTV_CORE(pi, stf_shdata->phytxchain, core) {
            uint8 core_off;

            for (idx = 0; idx < ESTPWRLUTS_TBL_LEN; idx++) {
                for (k = 0, regval[idx] = 0; k < 2; k++) {
                    core_off = core + ((core < 2)  ?
                                       idx_set[k] : 0);

                    pwr_est = wlc_phy_tssi2dbm_acphy(pi, idx,
                                                     a1[core_off],
                                                     b0[core_off],
                                                     b1[core_off]);

                    regval[idx] |= (uint16)(pwr_est&0xff) << (8*k);
                }
            }
            /* Est Pwr Table is 128x16 Table. Limit Write to 16 bits */
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_ESTPWRLUTS(core),
                ESTPWRLUTS_TBL_LEN, ESTPWRLUTS_TBL_OFFSET, 16, regval);

        }
    } else if ((ACMAJORREV_4(pi->pubpi->phy_rev) ||
            ACMAJORREV_128(pi->pubpi->phy_rev)) &&
            (flag2rangeon || BF3_TSSI_DIV_WAR(pi_ac))) {
        uint32 *estPwrLutReg24bit;
        if ((flag2rangeon)) {
                idx_set[0] = 2; idx_set[1] = 0;
        } else if (BF3_TSSI_DIV_WAR(pi_ac)) {
            if ((CHSPEC_IS80(pi->radio_chanspec) ||
                CHSPEC_IS40(pi->radio_chanspec)) &&
                CHSPEC_ISPHY5G6G(pi->radio_chanspec) &&
                (ACMAJORREV_4(pi->pubpi->phy_rev))) {
                /* core 2/3 contains 40-80mhz paparam
                 * core 0/1 contains 20mhz paparam
                 */
                idx_set[0] = 0; idx_set[1] = 2;
            } else if (CHSPEC_BW_LE20(pi->radio_chanspec) &&
                CHSPEC_IS2G(pi->radio_chanspec)) {
                /* core 0,1 conatins 20mhz OFDM paparam
                 * core 2,3 contains 20mhz CCK paparam
                 */
                idx_set[0] = 0;  idx_set[1] = 2;
            } else {
                /* For 80P80 case,bits<7:0> should have 20MHz,bits<15:8> should have
                PAPARMS correspond to 40/80MHz, bits<23:16> are same as bits <15:8>
                currently.
                */
                if (CHSPEC_IS8080(pi->radio_chanspec)) {
                    idx_set[0] = 0; idx_set[1] = 2;
                } else {
                    idx_set[0] = 0; idx_set[1] = 0;
                }
            }
        }
        FOREACH_ACTV_CORE(pi, stf_shdata->phytxchain, core) {
            uint8 core_off;

            for (idx = 0; idx < ESTPWRLUTS_TBL_LEN; idx++) {
                for (k = 0, regval[idx] = 0; k < 2; k++) {
                    core_off = core + ((core < 2)  ?
                                       idx_set[k] : 0);
                    pwr_est = wlc_phy_tssi2dbm_acphy(pi, idx,
                                                     a1[core_off],
                                                     b0[core_off],
                                                     b1[core_off]);
                    regval[idx] |= (uint16)(pwr_est&0xff) << (8*k);
                }
            }
            /* Acquire the memory buffer, The correct size has to be
             * registered during attach
             */
            estPwrLutReg24bit = phy_cache_acquire_reuse_buffer(pi->cachei,
                ESTPWRLUTS_TBL_LEN*4);
            /* Est Power LUT is of 24 bits.
            bit<7:0> corresponds to 20MHz BW case
            bits<15:0> corresponds to 40/80MHz case
            bits<23:16> corresponds to 80+80 MHz case.
            Tx Power control multi mode is set to
            4 for 80P80 case
            */
            for (idx = 0; idx < ESTPWRLUTS_TBL_LEN; idx++) {
                estPwrLutReg24bit[idx] =
                    ((((uint32)regval[idx]&0xFF00)
                    << 8) | (uint32)regval[idx]);
            }
            /* Est Pwr Tbl is 128x24 Table.
            Limit Write to 32 bits
            */
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_ESTPWRLUTS(core), ESTPWRLUTS_TBL_LEN,
                ESTPWRLUTS_TBL_OFFSET, 32, estPwrLutReg24bit);
            /* Release the memory buffer */
            phy_cache_release_reuse_buffer(pi->cachei, estPwrLutReg24bit);
        }
    } else {
#ifdef WLC_TXCAL
        if (phy_tssical_get_pwr_tssi_tbl_in_use(pi->tssicali)) {
            wlc_phy_apply_pwr_tssi_tble_chan_acphy(pi);
        } else
#endif    /* WLC_TXCAL */
        {
        FOREACH_ACTV_CORE(pi, stf_shdata->phytxchain, core) {
            for (idx = 0; idx < ESTPWRLUTS_TBL_LEN; idx++) {
                pwr_est = wlc_phy_tssi2dbm_acphy(pi, idx,
                                                 a1[core], b0[core], b1[core]);
                if (flag2rangeon) {
                    int32  pwr_est2range;

                    /* iPa - ToDo 2 range TSSI */
                    core2range = PHYCORENUM(pi->pubpi->phy_corenum) + core;
                    ASSERT(core2range < PAPARAM_SET_NUM);

                    pwr_est2range = wlc_phy_tssi2dbm_acphy(pi, idx,
                                                           a1[core2range],
                                                           b0[core2range],
                                                           b1[core2range]);

                    regval[idx] =
                        (uint16)((pwr_est2range&0xff) +
                        ((pwr_est&0xff)<<8));
                } else {
                    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
                        regval[idx] = (uint16)(((pwr_est&0xff)<< 8)
                            | (pwr_est&0xff));
                    } else {
                        regval[idx] = (uint16)(pwr_est&0xff);
                    }
                }
            }
            /* Est Pwr Table is 128x8 Table. Limit Write to 8 bits */
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_ESTPWRLUTS(core),
                ESTPWRLUTS_TBL_LEN, ESTPWRLUTS_TBL_OFFSET, 16, regval);
        }
        }
    }

    /* start to populate estPwrShftTbl */
    for (idx = 0; idx < ESTPWRSHFTLUTS_TBL_LEN; idx++) {
        if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
            if ((idx == 0)||(idx == 2)||(idx == 4)||((idx > 6)&&(idx < 10))) {
                /* 20in40 and 20in80 subband cases */
                pdoffs = 0;
                FOREACH_ACTV_CORE(pi, stf_shdata->hw_phytxchain, core) {
                    /* core_freq_segment_map is only required for 80P80 mode.
                    For other modes, it is ignored
                    */
                    core_freq_segment_map = ch_data->core_freq_mapping[core];
                    chan_freq_range = phy_ac_chanmgr_get_chan_freq_range(pi,
                        pi->radio_chanspec, PRIMARY_FREQ_SEGMENT);
                    pdoffs = wlc_phy_pdoffset_cal_acphy(pdoffs,
                        pwrdet->pdoffset5gsubband[core],
                        chan_freq_range, core);
                    }
                shfttblval[idx] = pdoffs & 0xffffff;
            } else if ((idx == 1) || ((idx > 4) && (idx < 7)) || (idx == 14)) {
                pdoffs = 0;
                FOREACH_ACTV_CORE(pi, stf_shdata->hw_phytxchain, core) {
                    /* core_freq_segment_map is only required for 80P80 mode.
                    For other modes, it is ignored
                    */
                    core_freq_segment_map = ch_data->core_freq_mapping[core];
                    chan_freq_range = phy_ac_chanmgr_get_chan_freq_range(pi,
                        pi->radio_chanspec, PRIMARY_FREQ_SEGMENT);
                    pdoffs = wlc_phy_pdoffset_cal_acphy(pdoffs,
                        pwrdet->pdoffset40[core], chan_freq_range, core);
                }
                shfttblval[idx] = pdoffs & 0xffffff;
            } else if (idx == 10) {
                pdoffs = 0;
                FOREACH_ACTV_CORE(pi, stf_shdata->hw_phytxchain, core) {
                    /* core_freq_segment_map is only required for 80P80 mode.
                    For other modes, it is ignored
                    */
                    core_freq_segment_map = ch_data->core_freq_mapping[core];
                    chan_freq_range = phy_ac_chanmgr_get_chan_freq_range(pi,
                        pi->radio_chanspec, PRIMARY_FREQ_SEGMENT);
                    pdoffs = wlc_phy_pdoffset_cal_acphy(pdoffs,
                        pwrdet->pdoffset80[core], chan_freq_range, core);
                }
                shfttblval[idx] = pdoffs & 0xffffff;
            } else {
                shfttblval[idx] = 0;
            }
        } else {
            {
#ifdef POWPERCHANNL
            if (PWRPERCHAN_ENAB(pi)) {
                if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
                    if (idx == 3) {
                        pdoffs = 0;
                        /* for now temp based offst support is
                         * not added
                         */
                        FOREACH_ACTV_CORE(pi, stf_shdata->hw_phytxchain,
                            core) {
                            core_freq_segment_map =
                            ch_data->core_freq_mapping[core];
                            chan_freq_range =
                            phy_ac_chanmgr_get_chan_freq_range(pi,
                            pi->radio_chanspec, PRIMARY_FREQ_SEGMENT);
                            pdoffs = wlc_phy_pdoffset_cal_acphy(pdoffs,
                            pwrdet->PwrOffsets2GNormTemp[core][
                            CHSPEC_CHANNEL(pi->radio_chanspec)-1],
                            chan_freq_range, core);
                        }
                        shfttblval[idx] = pdoffs & 0xffff;
                    } else if (idx == 17) {
                        pdoffs = 0;
                        /* for now temp based offset support is not added */
                        FOREACH_ACTV_CORE(pi, stf_shdata->hw_phytxchain,
                            core) {
                            core_freq_segment_map =
                            ch_data->core_freq_mapping[core];
                            chan_freq_range =
                            phy_ac_chanmgr_get_chan_freq_range(pi,
                            pi->radio_chanspec, PRIMARY_FREQ_SEGMENT);
                            pdoffs = wlc_phy_pdoffset_cal_acphy(pdoffs,
                            pwrdet->PwrOffsets2GNormTemp[core][
                            CHSPEC_CHANNEL(pi->radio_chanspec)-1]
                            + pwrdet->pdoffsetcck[core],
                            chan_freq_range, core);
                        }
                        shfttblval[idx] = pdoffs & 0xffff;
                    } else {
                        shfttblval[idx] = 0;
                    }
                }
            }
            else
#endif /* POWPERCHANNL */
            {
                if (idx == 3) {
                    pdoffs = 0;
                    FOREACH_ACTV_CORE(pi, stf_shdata->hw_phytxchain, core) {
                        core_freq_segment_map =
                            ch_data->core_freq_mapping[core];
                        chan_freq_range =
                            phy_ac_chanmgr_get_chan_freq_range(pi,
                            pi->radio_chanspec, PRIMARY_FREQ_SEGMENT);
                        pdoffs = wlc_phy_pdoffset_cal_acphy(pdoffs,
                            pwrdet->pdoffset2g20in20[core],
                            chan_freq_range, core);
                    }
                    shfttblval[idx] = pdoffs & 0xffffff;
                } else if (idx == 17) {
                    pdoffs = 0;
                    FOREACH_ACTV_CORE(pi, stf_shdata->hw_phytxchain, core) {
                        core_freq_segment_map =
                            ch_data->core_freq_mapping[core];
                        chan_freq_range =
                            phy_ac_chanmgr_get_chan_freq_range(pi,
                            pi->radio_chanspec, PRIMARY_FREQ_SEGMENT);
                        pdoffs = wlc_phy_pdoffset_cal_acphy(pdoffs,
                            pwrdet->pdoffsetcck[core],
                            chan_freq_range, core);
                    }
                    shfttblval[idx] = pdoffs & 0xffffff;
                } else {
                    if (pwrdet->pdoffset2g40_flag == 1) {
                        shfttblval[idx] = 0;
                    } else {
                        shfttblval[idx] = 0;
                        if (idx == 5) {
                            pdoffs = 0;
                            FOREACH_ACTV_CORE(pi,
                                    stf_shdata->hw_phytxchain,
                                    core) {
                                core_freq_segment_map =
                                ch_data->core_freq_mapping[
                                core];

                                chan_freq_range =
                                phy_ac_chanmgr_get_chan_freq_range(
                                pi, pi->radio_chanspec,
                                PRIMARY_FREQ_SEGMENT);

                                pdoffs =
                                wlc_phy_pdoffset_cal_acphy(pdoffs,
                                        pwrdet->pdoffset2g40[core],
                                       chan_freq_range, core);
                            }
                            shfttblval[idx] = pdoffs & 0xffffff;
                        }
                    }
                }
            }
        }
    }

    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
            shfttblval[idx] = 0;
        }
    }

    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        /*
        1.    In RSDB mode, just write to EstPWrShiftluts0
        2.    In 2x2 mode, since this table used to be a common table,
            write same entries to EstPWrShiftluts0 and EstPWrShiftluts1.
        3.    In 80p80 mode, TBD by SubraK
        */
        FOREACH_ACTV_CORE(pi, stf_shdata->hw_phytxchain, core) {
            for (idx = 0; idx < 24; idx++) {
                shfttblval[idx] = (shfttblval[idx]>>(8*core));
            }
            /*
            Only Least significant 8 bits
            <7:0> of shfttblval[idx] are written into phy tbl
            */
            wlc_phy_table_write_acphy(pi, wlc_phy_get_tbl_id_estpwrshftluts(pi, core),
                ESTPWRSHFTLUTS_TBL_LEN, ESTPWRSHFTLUTS_TBL_OFFSET, 32, shfttblval);
        }
    } else {
        uint16 tableID;

        if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
            tableID = AC2PHY_TBL_ID_ESTPWRSHFTLUTS0;
        } else {
            tableID = ACPHY_TBL_ID_ESTPWRSHFTLUTS;
        }
        wlc_phy_table_write_acphy(pi, tableID, ESTPWRSHFTLUTS_TBL_LEN,
                                  ESTPWRSHFTLUTS_TBL_OFFSET, 32, shfttblval);
    }
#ifdef ENABLE_FCBS
    }
#endif
    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        uint8 txpwrindexlimit;
        /* Setting Tx gain table index limit value */
        if (CHSPEC_IS2G(pi->radio_chanspec)) {
            /* currently, assume that both cck and OFDM have same index
             * cap value. Once, ucode support is available, fix this
             */
            txpwrindexlimit = ti->txpwrindexlimit[ACPHY_TXPWRINDEX_LIM_2G_CCK];
        } else {
            uint8 indx = ACPHY_TXPWRINDEX_LIM_5G_LL;
            uint8 subband = phy_ac_chanmgr_get_chan_freq_range(pi,
                pi->radio_chanspec, PRIMARY_FREQ_SEGMENT);
            switch (subband) {
                case WL_CHAN_FREQ_RANGE_5G_BAND0:
                    indx = ACPHY_TXPWRINDEX_LIM_5G_LL;
                    break;
                case WL_CHAN_FREQ_RANGE_5G_BAND1:
                    indx = ACPHY_TXPWRINDEX_LIM_5G_LH;
                    break;
                case WL_CHAN_FREQ_RANGE_5G_BAND2:
                    indx = ACPHY_TXPWRINDEX_LIM_5G_HL;
                    break;
                case WL_CHAN_FREQ_RANGE_5G_BAND3:
                    indx = ACPHY_TXPWRINDEX_LIM_5G_HH;
                    break;
                default:
                    PHY_ERROR(("wl%d: %s: Unrecognized subband: %d\n",
                    pi->sh->unit, __FUNCTION__, subband));
                    break;
            }
            txpwrindexlimit = ti->txpwrindexlimit[indx];
        }

        /* HW internally does right shift by 1 before capping */
        txpwrindexlimit <<= 1;
        FOREACH_CORE(pi, core) {
            MOD_PHYREGCE(pi, TxPwrCtrl_Multi_Mode, core,
                txPwrIndexLimit, txpwrindexlimit);
        }
    }

    /* code to store tssiSensMaxPwr of core0, core 1 and core 2 SHMs
    to re-store later as part of BT-WLAN transition
     */
    // NOTE: Phy cores upto three are taken care.
    if ((PHYCORENUM(pi->pubpi->phy_corenum) > 2)) {
        wlapi_bmac_write_shm(pi->sh->physhim, M_TSSI_SENS_LMT2(pi),
        (READ_PHYREGFLD(pi, TxPwrCtrlCore2TSSISensLmt, tssiSensMaxPwr)));
    }
    if ((PHYCORENUM(pi->pubpi->phy_corenum) > 1)) {
        wlapi_bmac_write_shm(pi->sh->physhim, M_TSSI_SENS_LMT1(pi),
            ((READ_PHYREGFLD(pi, TxPwrCtrlCore1TSSISensLmt, tssiSensMaxPwr) << 8) |
            (READ_PHYREGFLD(pi, TxPwrCtrlCore0TSSISensLmt, tssiSensMaxPwr))));
    } else if (PHYCORENUM(pi->pubpi->phy_corenum) == 1) {
        wlapi_bmac_write_shm(pi->sh->physhim, M_TSSI_SENS_LMT1(pi),
        (READ_PHYREGFLD(pi, TxPwrCtrlCore0TSSISensLmt, tssiSensMaxPwr)));
    }

    /* code to store Npt_intg_log2 SHM to re-store later as part of
     * BT-WLAN transition
     */
    wlapi_bmac_write_shm(pi->sh->physhim, M_TPCNNUM_INTG_LOG2(pi),
        (READ_PHYREGFLD(pi, TxPwrCtrlNnum, Npt_intg_log2)));
    /* Set C_BTCX_HFLG_PU_OFF_TPC_HOLD bit in M_BTCX_HOST_FLAGS */
    wlapi_btc_hflg(pi->sh->physhim, TRUE, BTCX_PU_OFF_TPC_HOLD);

    ACPHY_ENABLE_STALL(pi, stall_val);
    PHY_CHANLOG(pi, __FUNCTION__, TS_EXIT, 0);
}

int8
wlc_phy_tone_pwrctrl(phy_info_t *pi, int8 tx_idx, uint8 core)
{
    int8 pwr;
    int8 targetpwr = -99;
    int16  idle_tssi[PHY_CORE_MAX], tone_tssi[PHY_CORE_MAX];
    uint16 adjusted_tssi[PHY_CORE_MAX];
    int16 a1[PHY_CORE_MAX];
    int16 b1[PHY_CORE_MAX];
    int16 b0[PHY_CORE_MAX];
    int8 postive_slope = 1;
    int8 targetidx;
    int8 deltapwr;
    int16 tmpidx;
    txgain_setting_t txgain_settings;
    int8 orig_rxfarrow_shift = 0;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

    BCM_REFERENCE(stf_shdata);

    wlc_phy_get_papd_cal_pwr_acphy(pi, &targetpwr, &tx_idx, core);

    if (targetpwr == -99) {
        targetidx = -1;
    } else {
        wlc_phy_get_paparams_for_band_acphy(pi, a1, b0, b1);
        /* meas the idle tssi */
        wlc_phy_txpwrctrl_idle_tssi_meas_acphy(pi);
        idle_tssi[core] = READ_PHYREGCE(pi, TxPwrCtrlIdleTssi_path, core) & 0x3ff;
        idle_tssi[core] = idle_tssi[core] - 1023;

        /* prevent crs trigger */
        phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, TRUE);
        if (!(ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev))) {
            orig_rxfarrow_shift = READ_PHYREGFLD(pi, RxSdFeConfig6, rx_farrow_rshift_0);
            MOD_PHYREG(pi, RxSdFeConfig6, rx_farrow_rshift_0, 2);
        }

        if (RADIOID_IS(pi->pubpi->radioid, BCM20698_ID))
            wlc_phy_tssi_radio_setup_acphy_20698(pi, 0);
        else if (RADIOID_IS(pi->pubpi->radioid, BCM20704_ID))
            wlc_phy_tssi_radio_setup_acphy_20704(pi, 0);
        else if (RADIOID_IS(pi->pubpi->radioid, BCM20707_ID))
            wlc_phy_tssi_radio_setup_acphy_20707(pi, 0);
        else if (RADIOID_IS(pi->pubpi->radioid, BCM20708_ID))
            wlc_phy_tssi_radio_setup_acphy_20708(pi, 0);
        else if (RADIOID_IS(pi->pubpi->radioid, BCM20710_ID))
            wlc_phy_tssi_radio_setup_acphy_20710(pi, 0);
        else
            wlc_phy_tssi_radio_setup_acphy(pi, stf_shdata->hw_phyrxchain, 0);

        wlc_phy_txpwr_by_index_acphy(pi, 1, tx_idx);
        wlc_phy_get_txgain_settings_by_index_acphy(
            pi, &txgain_settings, tx_idx);
        wlc_phy_poll_samps_WAR_acphy(pi, tone_tssi,
            TRUE, FALSE, &txgain_settings, FALSE, TRUE, 0, 0);

        adjusted_tssi[core] = 1023 - postive_slope * (tone_tssi[core] - idle_tssi[core]);
        adjusted_tssi[core] = adjusted_tssi[core] >> 3;
        /* prevent crs trigger */
        phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, FALSE);
        if (!(ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev))) {
            MOD_PHYREG(pi, RxSdFeConfig6, rx_farrow_rshift_0, orig_rxfarrow_shift);
        }
        pwr = wlc_phy_tssi2dbm_acphy(pi, adjusted_tssi[core], a1[core], b0[core], b1[core]);

        /* delta pwr in qdb */
        deltapwr = (targetpwr * WLC_TXPWR_DB_FACTOR) - pwr;
        if (ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
            /* for 4350 with 0.5dB step size gaintable */
            tmpidx = (int16)tx_idx - (int16)(deltapwr >> 1);
        } else {
            tmpidx = (int16)tx_idx - (int16)deltapwr;
        }
        if (tmpidx < 0)
            targetidx = 0;
        else if (tmpidx > MAX_TX_IDX)
            targetidx = MAX_TX_IDX;
        else
            targetidx = (int8)tmpidx;
    }

    return targetidx;
}

static int32
phy_ac_tpc_get_estpwrlut_srom12(acphy_paparams_t *params,
        uint8 pa_fittype, int32 idx)
{
    int32 firstTerm = 0, secondTerm = 0, thirdTerm = 0, fourthTerm = 0;
    int32 ctrSqr = idx * idx;
    int32 pwr_est = 0;

    if (pa_fittype == 0) {
        /* logdetect */
        firstTerm  = (int32)params->a * 128;
        secondTerm = ((int32)params->b * idx) / 2;
        thirdTerm  = ((int32)params->c * ctrSqr) / 128;
        fourthTerm = ((int32)params->d * idx) / (((int32)idx - 128));
        pwr_est = (firstTerm + secondTerm +
                thirdTerm + fourthTerm) / 8192;
    } else if (pa_fittype == 1) {
        /* diode type */
        firstTerm = (int32)params->a * 16;
        secondTerm = (params->b * ctrSqr) / 4096;
        if (idx == 0)
            thirdTerm = 0;
        else
            thirdTerm = params->c * ctrSqr /
                (ctrSqr - ((int32)params->d * 2));
        pwr_est = (firstTerm + secondTerm + thirdTerm) / 1024;
    } else if (pa_fittype == 3) {
        /* original - used by 47622/6756 in 6GHz */
        firstTerm  = 8 * (16 * (int32)params->a + (int32)params->b * idx);
        secondTerm = 32768 + (int32)params->c * idx;
        pwr_est = MAX(((4 * firstTerm) / secondTerm), -8);
        pwr_est = MIN(pwr_est, 0x7F);
    } else {
        /* original - 4366 MCH5L flavor */
        firstTerm  = 8 * (16 * (int32)params->b
                + (int32)params->c * idx);
        secondTerm = 32768 + (int32)params->a * idx;
        pwr_est = MAX(((4 * firstTerm + secondTerm/2)
                    /secondTerm), -8);
        pwr_est = MIN(pwr_est, 0x7F);
    }
    return pwr_est;

}

static void
phy_ac_tpc_get_paparams_percore_srom12(phy_info_t *pi, uint8 chan_freq_range,
        acphy_paparams_t *params, uint8 core, uint8 cck)
{
    srom12_pwrdet_t *pwrdet = pi->pwrdet_ac;
    if (cck == 1) {
        params[core].a =  pwrdet->pwrdet_a[core][CH_2G_GROUP + CH_5G_5BAND];
        params[core].b =  pwrdet->pwrdet_b[core][CH_2G_GROUP + CH_5G_5BAND];
        params[core].c =  pwrdet->pwrdet_c[core][CH_2G_GROUP + CH_5G_5BAND];
        params[core].d =  pwrdet->pwrdet_d[core][CH_2G_GROUP + CH_5G_5BAND];
    } else {
        switch (chan_freq_range) {
            case WL_CHAN_FREQ_RANGE_2G:
            case WL_CHAN_FREQ_RANGE_5G_BAND0:
            case WL_CHAN_FREQ_RANGE_5G_BAND1:
            case WL_CHAN_FREQ_RANGE_5G_BAND2:
            case WL_CHAN_FREQ_RANGE_5G_BAND3:
            case WL_CHAN_FREQ_RANGE_5G_BAND4:
                params[core].a = pwrdet->
                    pwrdet_a[core][chan_freq_range];
                params[core].b = pwrdet->
                    pwrdet_b[core][chan_freq_range];
                params[core].c = pwrdet->
                    pwrdet_c[core][chan_freq_range];
                params[core].d = pwrdet->
                    pwrdet_d[core][chan_freq_range];
                break;
            case WL_CHAN_FREQ_RANGE_2G_40:
            case WL_CHAN_FREQ_RANGE_5G_BAND0_40:
            case WL_CHAN_FREQ_RANGE_5G_BAND1_40:
            case WL_CHAN_FREQ_RANGE_5G_BAND2_40:
            case WL_CHAN_FREQ_RANGE_5G_BAND3_40:
            case WL_CHAN_FREQ_RANGE_5G_BAND4_40:
                /* Adjust index for 40M */
                params[core].a = pwrdet->
                    pwrdet_a_40[core][chan_freq_range -
                        WL_CHAN_FREQ_RANGE_2G_40];
                params[core].b = pwrdet->
                    pwrdet_b_40[core][chan_freq_range -
                        WL_CHAN_FREQ_RANGE_2G_40];
                params[core].c = pwrdet->
                    pwrdet_c_40[core][chan_freq_range -
                        WL_CHAN_FREQ_RANGE_2G_40];
                params[core].d = pwrdet->
                    pwrdet_d_40[core][chan_freq_range -
                        WL_CHAN_FREQ_RANGE_2G_40];
                break;
            case WL_CHAN_FREQ_RANGE_5G_BAND0_80:
            case WL_CHAN_FREQ_RANGE_5G_BAND1_80:
            case WL_CHAN_FREQ_RANGE_5G_BAND2_80:
            case WL_CHAN_FREQ_RANGE_5G_BAND3_80:
            case WL_CHAN_FREQ_RANGE_5G_BAND4_80:
                /* Adjust index for 80M */
                params[core].a = pwrdet->
                    pwrdet_a_80[core][chan_freq_range -
                        WL_CHAN_FREQ_RANGE_5G_BAND0_80];
                params[core].b = pwrdet->
                    pwrdet_b_80[core][chan_freq_range -
                        WL_CHAN_FREQ_RANGE_5G_BAND0_80];
                params[core].c = pwrdet->
                    pwrdet_c_80[core][chan_freq_range -
                        WL_CHAN_FREQ_RANGE_5G_BAND0_80];
                params[core].d = pwrdet->
                    pwrdet_d_80[core][chan_freq_range -
                        WL_CHAN_FREQ_RANGE_5G_BAND0_80];
                break;
            case WL_CHAN_FREQ_RANGE_5G_BAND0_160:
            case WL_CHAN_FREQ_RANGE_5G_BAND1_160:
            case WL_CHAN_FREQ_RANGE_5G_BAND2_160:
            case WL_CHAN_FREQ_RANGE_5G_BAND3_160:
                /* Adjust index for 160M */
                params[core].a = pwrdet->
                    pwrdet_a_160[core][chan_freq_range -
                        WL_CHAN_FREQ_RANGE_5G_BAND0_160];
                params[core].b = pwrdet->
                    pwrdet_b_160[core][chan_freq_range -
                        WL_CHAN_FREQ_RANGE_5G_BAND0_160];
                params[core].c = pwrdet->
                    pwrdet_c_160[core][chan_freq_range -
                        WL_CHAN_FREQ_RANGE_5G_BAND0_160];
                params[core].d = pwrdet->
                    pwrdet_d_160[core][chan_freq_range -
                        WL_CHAN_FREQ_RANGE_5G_BAND0_160];
                break;
            default:
                PHY_ERROR(("wl%d: %s: pwrdet core%d: a=%d b=%d c=%d d=%d\n",
                        pi->sh->unit, __FUNCTION__, core,
                        params[core].a, params[core].b,
                        params[core].c, params[core].d));
                break;
        }
    }
}

static void
phy_ac_tpc_get_paparams_srom12(phy_info_t *pi, uint8 chan_freq_range, acphy_paparams_t *params)
{
    uint8  core;

    FOREACH_CORE(pi, core) {
        phy_ac_tpc_get_paparams_percore_srom12(pi, chan_freq_range, params, core, 0);
    }
}

static void
phy_ac_tpc_get_cck_paparams_srom18(phy_info_t *pi, acphy_paparams_t *params)
{
    uint8  core;

    FOREACH_CORE(pi, core) {
        phy_ac_tpc_get_paparams_percore_srom12(pi, 0, params, core, 1);
    }
}

static void
phy_ac_tpc_get_paparams_80p80_srom12(phy_info_t *pi, uint8 *chan_freqs, acphy_paparams_t *params)
{
    uint8 core, chan_freq_range;

    FOREACH_CORE(pi, core) {
        if (ACMAJORREV_33(pi->pubpi->phy_rev)) {
            /* core 0/1: 80L, core 2/3: 80U */
            chan_freq_range = (core <= 1) ? chan_freqs[0] : chan_freqs[1];
        } else {
            chan_freq_range = chan_freqs[0];
        }

        phy_ac_tpc_get_paparams_percore_srom12(pi, chan_freq_range, params, core, 0);
    }
}

int8
wlc_phy_fittype_srom12_acphy(phy_info_t *pi)
{
    int8 pdet_range;
    if (CHSPEC_ISPHY5G6G(pi->radio_chanspec))
        pdet_range = pi->tpci->data->cfg->srom_5g_pdrange_id;
    else
        pdet_range = pi->tpci->data->cfg->srom_2g_pdrange_id;
    switch (pdet_range) {
    case 24:
        return 0; /* Log detector for both 2G and 5G */
    case 25:
    case 26:
        return 1; /* Diode detector for both 2G and 5G */
    default:
        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            if (pi->sromi->sr13_dettype_en) {
                if (CHSPEC_ISPHY5G6G(pi->radio_chanspec))
                    return pi->sromi->dettype_5g;
                else
                    return pi->sromi->dettype_2g;
            } else {
                if (pdet_range == 0) {
                    /* logdetect - 4366 MC card */
                    return 0;
                } else if (ACMAJORREV_51_131(pi->pubpi->phy_rev) &&
                        CHSPEC_ISPHY5G6G(pi->radio_chanspec) &&
                        (pdet_range == 2)) {
                /* original: 47622/6756 flavor */
                    return 3;
                } else if ((CHSPEC_ISPHY5G6G(pi->radio_chanspec) &&
                        (pdet_range == 2))) {
                /* original: 4366 MCH5L flavor */
                    return 2;
                } else {
                /* diode type: 4366 MCH2L,MCM2/5L, etc */
                    return 1;
                }
            }
        } else {
            if (CHSPEC_ISPHY5G6G(pi->radio_chanspec))
                return pi->sromi->dettype_5g;
            else
                return pi->sromi->dettype_2g;
        }
    }
}

#if BAND5G || BAND6G
static uint8
wlc_phy_num_paparams_acphy(phy_info_t *pi, bool force5g6g)
{
    int8 pdet_range;
    if (CHSPEC_ISPHY5G6G(pi->radio_chanspec) || force5g6g) {
        pdet_range = pi->tpci->data->cfg->srom_5g_pdrange_id;
    } else {
        pdet_range = pi->tpci->data->cfg->srom_2g_pdrange_id;
    }

    if ((!(pi->sromi->sr13_dettype_en)) &&
        ACMAJORREV_51_131(pi->pubpi->phy_rev) &&
        (CHSPEC_ISPHY5G6G(pi->radio_chanspec) || force5g6g) &&
        (pdet_range == 2)) {
        /* For 47622/6756 with Qorvo 6G FEM we want to use 3-paparams */
        return 3;
    } else {
        return 4;
    }
}
#endif /* BAND5G || BAND6G */

static void
wlc_phy_get_srom12_pdoffset_acphy(phy_info_t *pi, int8 *pdoffs, int8 *pdoffs_160)
{

    srom12_pwrdet_t *pwrdet = pi->pwrdet_ac;
    int8  poffs1, poffs2, poffs3;
    uint8 core, band = 0;
    uint8 bands[NUM_CHANS_IN_CHAN_BONDING];
    uint8 high_subband_offset = 0;

    memset(pdoffs, 0, 2*PHY_CORE_MAX * sizeof(int8));
    memset(pdoffs_160, 0, PHY_CORE_MAX * sizeof(int8));

    if (!(SROMREV(pi->sh->sromrev) >= 12 &&
          (ACMAJORREV_32(pi->pubpi->phy_rev) ||
           ACMAJORREV_33(pi->pubpi->phy_rev) ||
           (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))))) {
        return;
    }

    /* to figure out which subband is in 5G */
    /* in the range of 0, 1, 2, 3, 4, 5 */
    if (PHY_AS_80P80(pi, pi->radio_chanspec)) {
        phy_ac_chanmgr_get_chan_freq_range_80p80_srom12(pi,
            pi->radio_chanspec, bands);
        bands[0] =    bands[0] - WL_CHAN_FREQ_RANGE_5G_BAND0_80 + 1;
        bands[1] =    bands[1] - WL_CHAN_FREQ_RANGE_5G_BAND0_80 + 1;
    } else {
        band = phy_ac_chanmgr_get_chan_freq_range_srom12(pi, pi->radio_chanspec);
        if (WL_CHAN_FREQ_RANGE_BW160(band)) {
            ASSERT(CHSPEC_IS160(pi->radio_chanspec));
            if (band > WL_CHAN_FREQ_RANGE_5G_BAND3_160) {
                high_subband_offset = WL_CHAN_FREQ_RANGE_5G_BAND4_160 -
                    WL_CHAN_FREQ_RANGE_5G_BAND3_160 - 1;
            }
            band = band - WL_CHAN_FREQ_RANGE_5G_BAND0_160 - high_subband_offset + 1;
        } else if (WL_CHAN_FREQ_RANGE_BW80(band)) {
            ASSERT(CHSPEC_IS80(pi->radio_chanspec) ||
            (PHY_AS_80P80(pi, pi->radio_chanspec)));
            if (band > WL_CHAN_FREQ_RANGE_5G_BAND4_80) {
                high_subband_offset = WL_CHAN_FREQ_RANGE_5G_BAND5_80 -
                    WL_CHAN_FREQ_RANGE_5G_BAND4_80 - 1;
            }
            band = band - WL_CHAN_FREQ_RANGE_5G_BAND0_80 - high_subband_offset + 1;
        } else if (WL_CHAN_FREQ_RANGE_BW40(band)) {
            ASSERT(CHSPEC_IS40(pi->radio_chanspec));
            if (band > WL_CHAN_FREQ_RANGE_5G_BAND4_40) {
                high_subband_offset = WL_CHAN_FREQ_RANGE_5G_BAND5_40 -
                    WL_CHAN_FREQ_RANGE_5G_BAND4_40 - 1;
            }
            band = band - WL_CHAN_FREQ_RANGE_2G_40 - high_subband_offset;
        } else {
            ASSERT(CHSPEC_IS20(pi->radio_chanspec));
            if (band > WL_CHAN_FREQ_RANGE_5G_BAND4) {
                high_subband_offset = WL_CHAN_FREQ_RANGE_5G_BAND5 -
                    WL_CHAN_FREQ_RANGE_5G_BAND4 - 1;
            }
            band = band - high_subband_offset;
        }
    }

    FOREACH_CORE(pi, core) {
        if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
            if (PHY_AS_80P80(pi, pi->radio_chanspec)) {
                if (ACMAJORREV_33(pi->pubpi->phy_rev) ||
                    (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                    !ACMAJORREV_128(pi->pubpi->phy_rev))) {
                    /* core 0/1: 80L, core 2/3: 80U */
                    band = (core <= 1) ? bands[0] : bands[1];
                } else {
                    band = bands[0];
                }
                poffs1 = (uint8)(pwrdet->pdoffset20in80[core][band]);
                poffs2 = (uint8)(pwrdet->pdoffset40in80[core][band]);
                poffs1 -= (poffs1 >= 16)? 32 : 0;
                poffs2 -= (poffs2 >= 16)? 32 : 0;
                pdoffs[core] = poffs1;
                pdoffs[PHY_CORE_MAX+core] = poffs2;
                pdoffs_160[core] = 0;
            } else if (CHSPEC_IS160(pi->radio_chanspec)) {
                if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
                    poffs1 = (uint8)(pwrdet->pdoffset20in160[core][band]);
                    poffs2 = (uint8)(pwrdet->pdoffset40in160[core][band]);
                    poffs3 = (uint8)(pwrdet->pdoffset80in160[core][band]);
                    poffs1 -= (poffs1 >= 16)? 32 : 0;
                    poffs2 -= (poffs2 >= 16)? 32 : 0;
                    poffs3 -= (poffs3 >= 16)? 32 : 0;
                    pdoffs[core] = poffs1;
                    pdoffs[PHY_CORE_MAX+core] = poffs2;
                    pdoffs_160[core] = poffs3;
                } else {
                    pdoffs[PHY_CORE_MAX+core] = pdoffs[core] = 0;
                    pdoffs_160[core] = 0;
                    PHY_INFORM(("%s:%d: %s PDOFFSET is not ready"
                    "for BW160\n", __FILE__, __LINE__, __FUNCTION__));
                }
            } else if (CHSPEC_IS80(pi->radio_chanspec)) {
                poffs1 = (uint8)(pwrdet->pdoffset20in80[core][band]);
                poffs2 = (uint8)(pwrdet->pdoffset40in80[core][band]);
                poffs1 -= (poffs1 >= 16)? 32 : 0;
                poffs2 -= (poffs2 >= 16)? 32 : 0;
                pdoffs[core] = poffs1;
                pdoffs[PHY_CORE_MAX+core] = poffs2;
                pdoffs_160[core] = 0;
            } else if (CHSPEC_IS40(pi->radio_chanspec)) {
                poffs1 = (uint8)(pwrdet->pdoffset20in40[core][band]);
                poffs1 -= (poffs1 >= 16)? 32 : 0;
                pdoffs[core] = poffs1;
                pdoffs[PHY_CORE_MAX+core] = 0;
                pdoffs_160[core] = 0;
            } else {
                pdoffs[core] = 0;
                pdoffs[PHY_CORE_MAX+core] = 0;
                pdoffs_160[core] = 0;
            }
        } else {
            if (CHSPEC_IS40(pi->radio_chanspec)) {
                poffs1 = (uint8)(pwrdet->pdoffset20in40[core][band]);
                poffs1 -= (poffs1 >= 16)? 32 : 0;
                pdoffs[core] = poffs1;
                /* pdoffset for cck 20in40MHz */
                poffs2 = (uint8)(pwrdet->pdoffsetcck20m[core]);
                poffs2 -= (poffs2 >= 16)? 32 : 0;
                pdoffs[PHY_CORE_MAX+core] = poffs2;
                pdoffs_160[core] = 0;
            } else {
                pdoffs[core] = 0;
                /* pdoffset for cck in 20MHz */
                poffs2 = (uint8)(pwrdet->pdoffsetcck[core]);
                poffs2 -= (poffs2 >= 16)? 32 : 0;
                pdoffs[PHY_CORE_MAX+core] = poffs2;
                pdoffs_160[core] = 0;
            }
        }
    }
}

void wlc_phy_txpwrctrl_idle_tssi_phyreg_setup_acphy(phy_info_t *pi)
{
    uint8  tssi_delay = 0;
    uint8 stall_val;
    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    ACPHY_REG_LIST_START
        ACPHY_DISABLE_STALL_ENTRY(pi)
        /* enable TSSI */
        MOD_PHYREG_ENTRY(pi, TSSIMode, tssiEn, 1)
        MOD_PHYREG_ENTRY(pi, TxPwrCtrlCmd, txPwrCtrl_en, 0)
    ACPHY_REG_LIST_EXECUTE(pi);

    /* determine pos/neg TSSI slope */
    if (CHSPEC_IS2G(pi->radio_chanspec)) {
        MOD_PHYREG(pi, TSSIMode, tssiPosSlope, pi->fem2g->tssipos);
    } else {
        MOD_PHYREG(pi, TSSIMode, tssiPosSlope, pi->fem5g->tssipos);
    }
    MOD_PHYREG(pi, TSSIMode, tssiPosSlope, 1);

    /* When to sample TSSI */
    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        tssi_delay = 200;
        MOD_PHYREG(pi, perPktIdleTssiCtrl, perPktIdleTssiUpdate_en, 0);
        MOD_PHYREG(pi, TSSIMode, TwoPwrRange, 0);
        if (!ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, TssiAccumCtrl, tssi_accum_en, 1);
            MOD_PHYREG(pi, TssiAccumCtrl, tssi_filter_pos, 1);
        } else {
            MOD_PHYREG(pi, TssiAccumCtrl, tssi_accum_en, 0);
            MOD_PHYREG(pi, TssiAccumCtrl, tssi_filter_pos, 0);
        }
        MOD_PHYREG(pi, TssiAccumCtrl, Ntssi_accum_delay, 200);
        MOD_PHYREG(pi, TssiAccumCtrl, Ntssi_intg_log2, 4);

        MOD_PHYREG(pi, TssiAccumCtrlcck, Ntssi_accum_delay_cck, 200);
        MOD_PHYREG(pi, TssiAccumCtrlcck, Ntssi_intg_log2_cck, 4);
    } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        tssi_delay = wlc_phy_ac_set_tssi_params_majrev128(pi);
    } else if (IS_4364(pi)) {
        tssi_delay = SAMPLE_TSSI_AFTER_160_SAMPLES;
        MOD_PHYREG(pi, TssiAccumCtrl, tssi_accum_en, 1);
        MOD_PHYREG(pi, TssiAccumCtrl, Ntssi_intg_log2, 4);
        MOD_PHYREG(pi, TssiAccumCtrl, Ntssi_accum_delay, tssi_delay);
        MOD_PHYREG(pi, TssiAccumCtrl, tssi_filter_pos, 1);
    } else if (CHSPEC_IS2G(pi->radio_chanspec)) {
        if (pi->tpci->data->cfg->srom_2g_pdrange_id >= 5) {
            tssi_delay = 150;
        } else if (pi->tpci->data->cfg->srom_2g_pdrange_id >= 4) {
            tssi_delay = 220;
        } else {
            tssi_delay = 150;
        }
    } else {
        if (pi->tpci->data->cfg->srom_5g_pdrange_id >= 5) {
            tssi_delay = 150;
        } else if (pi->tpci->data->cfg->srom_5g_pdrange_id >= 4) {
            tssi_delay = 220;
        } else {
            tssi_delay = 150;
        }
    }
    MOD_PHYREG(pi, TxPwrCtrlNnum, Ntssi_delay, tssi_delay);
    ACPHY_ENABLE_STALL(pi, stall_val);
}

static void
wlc_phy_txpwrctrl_pwr_setup_srom12_acphy(phy_info_t *pi)
{
    uint8 stall_val;
    phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;
    srom12_pwrdet_t *pwrdet = pi->pwrdet_ac;
#if BAND6G
    srom18_5gext_pwrdet_t *pwrdet_5gext = pi->pwrdet_ac_5gext;
#endif /* BAND6G */
    /* Need 2D array of paparams because of multimode */
    acphy_paparams_t paparams[TPC_ESTPWRTBLS_MAX][PHY_CORE_MAX] = {{{0, 0, 0, 0}}};
    int32  idx;
    uint8  shfttbl_len;
    uint32 shfttblval[64];
    int32  pwr_est_bw[TPC_ESTPWRTBLS_MAX], pwr_est_cck, tbl_len;
    int32  pwr_est_2gbw20;
    uint32 *regval;
    int8   pdoffsets[PHY_CORE_MAX*2];
    int8   pdoffsets_80in160[PHY_CORE_MAX];
    int32  firstTerm = 0, secondTerm = 0, thirdTerm = 0, fourthTerm = 0;
    int32  ctrSqr;
    int8   target_min_limit;
    uint8  chan_freq_range, iidx, chan_freq = 0;
    uint8  chan_freqs[NUM_CHANS_IN_CHAN_BONDING];
    uint8  core, rx_coremask, val;
    uint8  mult_mode = 0;
    uint8  pa_fittype = 0;
    uint8 pdet_range_id = 0;
    uint8 a_band = 0;
    uint32 lowrate_tssi_delay = 0;
    bool   using_estpwr_lut_cck = FALSE;
    bool   using_estpwr_lut_2gbw20 = FALSE;
    uint8 bw_idx;

#ifdef PREASSOC_PWRCTRL
    bool init_idx_carry_from_lastchan;
    uint8 step_size, prev_target_qdbm;
#endif
    struct _tp_qtrdbm {
        uint8 core;
        int8 target_pwr_qtrdbm;
    } tp_qtrdbm_each_core[PHY_CORE_MAX]; /* TP for each core */
    uint core_count = 0;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);
#if defined(PHYCAL_CACHING)
    ch_calcache_t *ctx = NULL;
    ctx = wlc_phy_get_chanctx(pi, pi->radio_chanspec);
    BCM_REFERENCE(ctx);
#endif /* PHYCAL_CACHING */

    iidx = 0;
    tbl_len = 128;
    pwr_est_cck = 0;
    pwr_est_2gbw20 = 0;

    ASSERT(SROMREV(pi->sh->sromrev) >= 12);

    BCM_REFERENCE(rx_coremask);

    if ((regval = (uint32*)phy_malloc(pi, 128 * sizeof(uint32))) == NULL) {
        PHY_ERROR(("%s: phy_malloc failed\n", __FUNCTION__));
        return;
    }

    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        rx_coremask = stf_shdata->hw_phytxchain;
    } else {
        rx_coremask = stf_shdata->phytxchain;
    }

    if (CHSPEC_IS2G(pi->radio_chanspec)) {
        pdet_range_id = phy_tpc_get_2g_pdrange_id(pi->tpci);
        a_band = 0;
    } else {
        pdet_range_id = phy_tpc_get_5g_pdrange_id(pi->tpci);
        if (ACMAJORREV_130(pi->pubpi->phy_rev) && CHSPEC_IS80(pi->radio_chanspec)) {
            a_band = 2;
        } else {
            a_band = 1;
        }
    }
    if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
        lowrate_tssi_delay = lowrate_tssi_delay_set47_1[pdet_range_id][a_band];
    } else if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
        lowrate_tssi_delay = 470;
    } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        lowrate_tssi_delay = 750; /* Pending bringup */
    } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
        lowrate_tssi_delay = lowrate_tssi_delay_set129[pdet_range_id][a_band];
    } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        lowrate_tssi_delay = lowrate_tssi_delay_set130[pdet_range_id][a_band];
    } else {
        lowrate_tssi_delay = 750;
    }
    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));
    wlc_phy_txpwrctrl_idle_tssi_phyreg_setup_acphy(pi);
    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);

    if (RADIOREV(pi->pubpi->radiorev) == 4 ||
        RADIOREV(pi->pubpi->radiorev) == 8 ||
        ACMAJORREV_2(pi->pubpi->phy_rev) ||
        ACMAJORREV_5(pi->pubpi->phy_rev) ||
        ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        /* 4360B0/B1/4350 using 0.5dB-step gaintbl, bbmult interpolation enabled */
        MOD_PHYREG(pi, TxPwrCtrlCmd, bbMultInt_en, 1);
#ifdef PREASSOC_PWRCTRL
        step_size = 2;
#endif
    } else if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        if (PHY_IPA(pi)) {
            MOD_PHYREG(pi, TxPwrCtrlCmd, bbMultInt_en, 0);
        } else {
            MOD_PHYREG(pi, TxPwrCtrlCmd, bbMultInt_en, 1);
        }
#ifdef PREASSOC_PWRCTRL
        step_size = 2;
#endif
    } else {
        /* disable bbmult interpolation
           to work with a 0.25dB step txGainTbl
        */
        MOD_PHYREG(pi, TxPwrCtrlCmd, bbMultInt_en, 0);
#ifdef PREASSOC_PWRCTRL
        step_size = 1;
#endif
    }

    /* Get the band (chan_freq_range) to get the pwrdet params from SROM */
    if (IS_4364_3x3(pi)||
        (ACMAJORREV_5(pi->pubpi->phy_rev) && ACMINORREV_1(pi))) {
        chan_freq_range = phy_ac_chanmgr_get_chan_freq_range_srom12(pi, 0);
    } else if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
               ACMAJORREV_33(pi->pubpi->phy_rev) ||
               ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        if (PHY_AS_80P80(pi, pi->radio_chanspec)) {
            phy_ac_chanmgr_get_chan_freq_range_80p80_srom12(pi, 0, chan_freqs);
            // FIXME: 80p80
            chan_freq_range = chan_freqs[0];
        } else {
            chan_freq_range = phy_ac_chanmgr_get_chan_freq_range_srom12(pi, 0);
        }
    } else {
        chan_freq_range = phy_ac_chanmgr_get_chan_freq_range_srom12(pi, pi->radio_chanspec);
    }

    /* Get pwrdet params from SROM for current subband */
    if (ACMAJORREV_128(pi->pubpi->phy_rev) || ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
        FOREACH_CORE(pi, core) {
            if (WL_CHAN_FREQ_RANGE_LT_BAND5(chan_freq_range)) {
                phy_ac_tpc_get_paparams_percore_multimode_srom12(pi,
                    chan_freq_range,
                    paparams, core,
                    pi->sromi->sr18_cck_paparam_en);

                if (CHSPEC_IS2G(pi->radio_chanspec) &&
                    pi->sromi->sr18_cck_paparam_en) {
                    using_estpwr_lut_cck = TRUE;
                }
            } else {
#if BAND6G
                /* Get additional 6G pwrdet params from SROM for current subband */
                /* 5G/6G subband5/6 will go here */
                phy_ac_tpc_get_5gext_paparams_percore_multimode_srom18(pi,
                    chan_freq_range,
                    paparams, core);
#endif /* BAND6G */
            }
        }
    } else if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        if (chan_freq_range < WL_CHAN_FREQ_RANGE_5G_BAND5) {
            if (PHY_AS_80P80(pi, pi->radio_chanspec)) {
                phy_ac_tpc_get_paparams_80p80_srom12(pi,
                    chan_freqs, &paparams[BW0][0]);
            } else {
                phy_ac_tpc_get_paparams_srom12(pi,
                    chan_freq_range, &paparams[BW0][0]);
            }
        } else {
#if BAND6G
            /* Get additional 6g pwrdet params from SROM for current subband */
            /* 5G subband5, UNII-4, will go here */
            phy_ac_tpc_get_5gext_paparams_srom18(pi, chan_freq_range,
                    &paparams[BW0][0]);
#endif /* BAND6G */
        }
        if (CHSPEC_IS2G(pi->radio_chanspec)) {
            if (ACMAJORREV_47_129_130(pi->pubpi->phy_rev)) {
                if (pi->sromi->sr18_cck_paparam_en) {
                    using_estpwr_lut_cck = TRUE;
                }
                if (pi->tpci->data->cfg->srom_2g_pdrange_id == 4 &&
                    CHSPEC_IS40(pi->radio_chanspec)) {
                    using_estpwr_lut_2gbw20 = TRUE;
                    phy_ac_tpc_get_paparams_srom12(pi,
                        WL_CHAN_FREQ_RANGE_2G, &paparams[BW1][0]);
                } else {
                    using_estpwr_lut_2gbw20 = FALSE;
                }
                phy_ac_tpc_get_cck_paparams_srom18(pi, &paparams[BW2][0]);
            } else {
                if (pi->tpci->data->cfg->srom_2g_pdrange_id == 2) {
                    /* Using separate estPwrLuts for 2G CCK for
                     * the 2G-only board to improve the performance
                     */
                    using_estpwr_lut_cck = TRUE;
                    if (CHSPEC_IS20(pi->radio_chanspec)) {
                        phy_ac_tpc_get_paparams_srom12(pi,
                            WL_CHAN_FREQ_RANGE_5G_BAND0,
                            &paparams[BW2][0]);
                    } else {
                        phy_ac_tpc_get_paparams_srom12(pi,
                            WL_CHAN_FREQ_RANGE_5G_BAND0_40,
                            &paparams[BW2][0]);
                    }
                }
            }
        }
    } else {
        FOREACH_CORE(pi, core) {
            switch (chan_freq_range) {
            case WL_CHAN_FREQ_RANGE_2G:
            case WL_CHAN_FREQ_RANGE_5G_BAND0:
            case WL_CHAN_FREQ_RANGE_5G_BAND1:
            case WL_CHAN_FREQ_RANGE_5G_BAND2:
            case WL_CHAN_FREQ_RANGE_5G_BAND3:
            case WL_CHAN_FREQ_RANGE_5G_BAND4:
                paparams[BW0][core].a = pwrdet->pwrdet_a[core][chan_freq_range];
                paparams[BW0][core].b = pwrdet->pwrdet_b[core][chan_freq_range];
                paparams[BW0][core].c = pwrdet->pwrdet_c[core][chan_freq_range];
                paparams[BW0][core].d = pwrdet->pwrdet_d[core][chan_freq_range];
                break;
            case WL_CHAN_FREQ_RANGE_2G_40:
            case WL_CHAN_FREQ_RANGE_5G_BAND0_40:
            case WL_CHAN_FREQ_RANGE_5G_BAND1_40:
            case WL_CHAN_FREQ_RANGE_5G_BAND2_40:
            case WL_CHAN_FREQ_RANGE_5G_BAND3_40:
            case WL_CHAN_FREQ_RANGE_5G_BAND4_40:
                /* Adjust index for 40M */
                paparams[BW0][core].a = pwrdet->pwrdet_a_40[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_2G_40];
                paparams[BW0][core].b = pwrdet->pwrdet_b_40[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_2G_40];
                paparams[BW0][core].c = pwrdet->pwrdet_c_40[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_2G_40];
                paparams[BW0][core].d = pwrdet->pwrdet_d_40[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_2G_40];
                break;
            case WL_CHAN_FREQ_RANGE_5G_BAND0_80:
            case WL_CHAN_FREQ_RANGE_5G_BAND1_80:
            case WL_CHAN_FREQ_RANGE_5G_BAND2_80:
            case WL_CHAN_FREQ_RANGE_5G_BAND3_80:
            case WL_CHAN_FREQ_RANGE_5G_BAND4_80:
                /* Adjust index for 80M */
                paparams[BW0][core].a = pwrdet->pwrdet_a_80[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND0_80];
                paparams[BW0][core].b = pwrdet->pwrdet_b_80[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND0_80];
                paparams[BW0][core].c = pwrdet->pwrdet_c_80[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND0_80];
                paparams[BW0][core].d = pwrdet->pwrdet_d_80[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND0_80];
                break;
            default:
                PHY_ERROR(("wl%d: %s: pwrdet core%d: a=%d b=%d c=%d d=%d\n",
                   pi->sh->unit, __FUNCTION__, core,
                   paparams[BW0][core].a, paparams[BW0][core].b,
                   paparams[BW0][core].c, paparams[BW0][core].d));
                break;
            }
        }
    }

    /* target power */
    wlc_phy_txpwrctrl_update_minpwr_acphy(pi);
    target_min_limit = pi->min_txpower * WLC_TXPWR_DB_FACTOR;

    FOREACH_ACTV_CORE(pi, rx_coremask, core) {
        int8 target_pwr_qtrdbm;
        target_pwr_qtrdbm = (int8)pi->tx_power_max_per_core[core];
        /* never target below the min threashold */
        if (target_pwr_qtrdbm < target_min_limit)
            target_pwr_qtrdbm = target_min_limit;

        if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
            tp_qtrdbm_each_core[core].target_pwr_qtrdbm = target_pwr_qtrdbm;
        } else {
            tp_qtrdbm_each_core[core_count].core = core;
            tp_qtrdbm_each_core[core_count].target_pwr_qtrdbm = target_pwr_qtrdbm;

            ++core_count;
        }
    }

#ifdef PREASSOC_PWRCTRL
    FOREACH_ACTV_CORE(pi, rx_coremask, core) {
        init_idx_carry_from_lastchan = (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) ?
            pi_ac->tpci->pwr_ctrl_save->status_idx_carry_5g[core]
            : pi_ac->tpci->pwr_ctrl_save->status_idx_carry_2g[core];
        if (!init_idx_carry_from_lastchan) {
            /* 4360B0 using 0.5dB-step gaintbl so start with a lower starting idx */
            if ((RADIOREV(pi->pubpi->radiorev) == 4) ||
               (RADIOREV(pi->pubpi->radiorev) == 8) ||
               (ACMAJORREV_5(pi->pubpi->phy_rev))) {
            iidx = 20;
            } else if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
            iidx = ti->data->base_index_init[core];
            } else {
            iidx = 50;
            }
            if (!pi->tpci->data->ovrinitbaseidx) {
                MOD_PHYREGCEE(pi, TxPwrCtrlInit_path, core,
                        pwrIndex_init_path, iidx);
            }
            if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
            pi_ac->tpci->pwr_ctrl_save->status_idx_carry_5g[core] = TRUE;
            } else {
            pi_ac->tpci->pwr_ctrl_save->status_idx_carry_2g[core] = TRUE;
            }
            /* XXX This is the handshake for te code that
               is put in for open loop power control
               128 means do NOT restore the value in the
               open loop function
            */
            pi_ac->tpci->txpwrindex_hw_save[core] = 128;
        } else {
            /* set power index initial condition */
            int32 new_iidx;

            if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
            iidx = pi_ac->tpci->pwr_ctrl_save->status_idx_5g[core];
            prev_target_qdbm = pi_ac->tpci->pwr_ctrl_save->pwr_qdbm_5g[core];
            if ((pi_ac->tpci->pwr_ctrl_save->stored_not_restored_5g[core])) {
                /* XXX This is the handshake for
                   the code that is put in for open loop power control
                   128 means do NOT restore the value in the
                   open loop function
                */
                pi_ac->tpci->pwr_ctrl_save->stored_not_restored_5g[core]
                = FALSE;
                pi_ac->tpci->txpwrindex_hw_save[core] = 128;

            }
            } else {
            iidx = pi_ac->tpci->pwr_ctrl_save->status_idx_2g[core];
            prev_target_qdbm = pi_ac->tpci->pwr_ctrl_save->pwr_qdbm_2g[core];
            if ((pi_ac->tpci->pwr_ctrl_save->stored_not_restored_2g[core])) {
                /* XXX This is the handshake for
                   the code that is put in for open loop power control
                   128 means do NOT restore the value in the
                   open loop function
                */
                pi_ac->tpci->pwr_ctrl_save->stored_not_restored_2g[core] = FALSE;
                pi_ac->tpci->txpwrindex_hw_save[core] = 128;

            }
            }
            new_iidx = (int32)iidx - ((int32)tp_qtrdbm_each_core[core].target_pwr_qtrdbm
                          - prev_target_qdbm) / step_size;

            /* XXX Sanity Check
               XXX make sure iidx is within bounds to be safe
            */
            if (new_iidx < PWRCTRL_MIN_INIT_IDX) {
            iidx = PWRCTRL_MIN_INIT_IDX;
            } else if (new_iidx > PWRCTRL_MAX_INIT_IDX) {
            iidx = PWRCTRL_MAX_INIT_IDX;
            } else {
            iidx = (uint8)new_iidx;
            }
            if (!pi->tpci->data->ovrinitbaseidx) {
                MOD_PHYREGCEE(pi, TxPwrCtrlInit_path,
                        core, pwrIndex_init_path, iidx);
            }
        }
    }
#else
    if ((RADIOREV(pi->pubpi->radiorev) == 4) || (RADIOREV(pi->pubpi->radiorev) == 8) ||
        (RADIOREV(pi->pubpi->radiorev) == 10) ||
        (RADIOREV(pi->pubpi->radiorev) == 11) ||
        (RADIOREV(pi->pubpi->radiorev) == 13) ||
        (ACMAJORREV_5(pi->pubpi->phy_rev))) {
        iidx = 20;
    } else {
        iidx = 50;
    }
#if defined(PHYCAL_CACHING)
    if (!ctx || !ctx->valid)
#endif /* PHYCAL_CACHING */
        FOREACH_ACTV_CORE(pi, rx_coremask, core) {
            if (!pi->tpci->data->ovrinitbaseidx) {
                MOD_PHYREGCEE(pi, TxPwrCtrlInit_path,
                        core, pwrIndex_init_path, iidx);
            }
        }
#endif /* PREASSOC_PWRCTRL */
    /* MOD_PHYREG(pi, TxPwrCtrlIdleTssi, rawTssiOffsetBinFormat, 1); */

    /* multi-mode setup */
    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
            if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en) {
                mult_mode = 1;
            } else {
                mult_mode = 0;
            }
            if (ACMAJORREV_47_129_130(pi->pubpi->phy_rev)) {
                mult_mode = 6;
                FOREACH_ACTV_CORE(pi, rx_coremask, core) {
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_20_40_path,
                        core, twoPwrRangemode_20, 0);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_20_40_path,
                        core, twoPwrRangemode_40, 0);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_20_40_path,
                        core, table_lowpwr_20, 0);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_20_40_path,
                        core, table_highpwr_20, 0);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_20_40_path,
                        core, table_lowpwr_40, 0);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_20_40_path,
                        core, table_highpwr_40, 0);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_80_160_path,
                        core, twoPwrRangemode_80, 0);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_80_160_path,
                        core, twoPwrRangemode_160, 0);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_80_160_path,
                        core, table_lowpwr_80, 0);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_80_160_path,
                        core, table_highpwr_80, 0);
                    // 11b and BW160 are shared
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_80_160_path,
                        core, table_lowpwr_160, 1);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_80_160_path,
                        core, table_highpwr_160, 1);
                }
                if (CHSPEC_IS2G(pi->radio_chanspec) &&
                    (using_estpwr_lut_2gbw20 == TRUE)) {
                    FOREACH_ACTV_CORE(pi, rx_coremask, core) {
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_20_40_path,
                        core, table_lowpwr_20, 2);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_20_40_path,
                        core, table_highpwr_20, 2);
                    }
                }
                //ER packet offset due to 3dB L-STF/LTF boosting
                MOD_PHYREG(pi, txPwrCtrl11ax, he_range_ext_power_offset, 12);

            } else if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
                mult_mode = 6;
                FOREACH_ACTV_CORE(pi, rx_coremask, core) {
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_20_40_path,
                        core, twoPwrRangemode_20, 0);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_20_40_path,
                        core, twoPwrRangemode_40, 0);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_20_40_path,
                        core, table_lowpwr_20, 0);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_20_40_path,
                        core, table_highpwr_20, 0);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_20_40_path,
                        core, table_lowpwr_40, 1);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_20_40_path,
                        core, table_highpwr_40, 1);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_80_160_path,
                        core, twoPwrRangemode_80, 0);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_80_160_path,
                        core, twoPwrRangemode_160, 0);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_80_160_path,
                        core, table_lowpwr_80, 2);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_80_160_path,
                        core, table_highpwr_80, 2);
                    // 11b and BW160 are shared
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_80_160_path,
                        core, table_lowpwr_160, 3);
                    MOD_PHYREGCEE(pi, Tssi2estpwrtblmap_80_160_path,
                        core, table_highpwr_160, 3);
                }

                //ER packet offset due to 3dB L-STF/LTF boosting
                MOD_PHYREG(pi, txPwrCtrl11ax, he_range_ext_power_offset, 12);
            }
        } else {
            mult_mode = 2;
        }

        MOD_PHYREG(pi, TxPwrCtrl_Multi_Mode0, multi_mode, mult_mode);
        MOD_PHYREG(pi, TxPwrCtrl_Multi_Mode1, multi_mode, mult_mode);
        if (!ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, TxPwrCtrl_Multi_Mode2, multi_mode, mult_mode);
            if (!ACMAJORREV_129(pi->pubpi->phy_rev))
                MOD_PHYREG(pi, TxPwrCtrl_Multi_Mode3, multi_mode, mult_mode);
        }
    } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        mult_mode = 2;
        MOD_PHYREG(pi, TxPwrCtrl_Multi_Mode0, multi_mode, mult_mode);
        MOD_PHYREG(pi, TxPwrCtrl_Multi_Mode1, multi_mode, mult_mode);
    }

    if ((pi->tpci->data->cfg->bphy_scale != 0) &&
        !ACMAJORREV_32(pi->pubpi->phy_rev) &&
        !ACMAJORREV_33(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, BphyControl3, bphyScale20MHz,
            pi->tpci->data->cfg->bphy_scale);
    }

#if defined(PREASSOC_PWRCTRL)
    /* average over 2 or 16 packets */
    wlc_phy_pwrctrl_shortwindow_upd_acphy(pi, pi->tpci->data->channel_short_window);
#else
    if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en) {
            FOREACH_CORE(pi, core) {
                MOD_PHYREGCE(pi, lowRateTssiDlyOFDM, core, Ntssi_hw_delay_ofdm,
                        lowrate_tssi_delay);
                MOD_PHYREGCE(pi, lowRateTssiDlyBPHY, core, Ntssi_hw_delay_bphy,
                        lowrate_tssi_delay);
                MOD_PHYREGCE(pi, lowRateTssiDlyIDLE, core, Ntssi_hw_delay_idle,
                        lowrate_tssi_delay);
                MOD_PHYREGCE(pi, lowRateTssi, core, lb_lowrate_fifo_len_in_bits,
                        3);
            }
            ACPHY_REG_LIST_START
                MOD_PHYREG_ENTRY(pi, TxPwrCtrlNnum, Npt_intg_log2, 1)
                MOD_PHYREG_ENTRY(pi, TxPwrCtrlDamping, DeltaPwrDamp, 64)
                MOD_PHYREG_ENTRY(pi, TssiAccumCtrl, Ntssi_intg_log2, 4)
                MOD_PHYREG_ENTRY(pi, TxPwrCtrlNnum, Ntssi_delay, 150)
                MOD_PHYREG_ENTRY(pi, TssiAccumCtrl, Ntssi_accum_delay, 0)
                MOD_PHYREG_ENTRY(pi, TssiAccumCtrlcck, Ntssi_accum_delay_cck, 0)
                MOD_PHYREG_ENTRY(pi, TssiAccumCtrlcck, Ntssi_intg_log2_cck, 4)
                MOD_PHYREG_ENTRY(pi, perPktIdleTssiCtrl, perPktIdleTssiUpdate_en, 1)
                MOD_PHYREG_ENTRY(pi, TSSIMode, TwoPwrRange, 0)
            ACPHY_REG_LIST_EXECUTE(pi);
        } else {
            MOD_PHYREG(pi, TxPwrCtrlNnum, Npt_intg_log2, 2);
            MOD_PHYREG(pi, TxPwrCtrlDamping, DeltaPwrDamp, 16);
        }
        if (CHSPEC_IS2G(pi->radio_chanspec)) {
            pi->tx_pwr_backoff = pwrdet->pwr_backoff[0];
        } else {
            pi->tx_pwr_backoff = pwrdet->pwr_backoff[1];
        }
    } else {
        MOD_PHYREG(pi, TxPwrCtrlNnum, Npt_intg_log2, PWRCTRL_LONGW_AVG);
    }
#endif /* PREASSOC_PWRCTRL */

    /* decouple IQ comp and LOFT comp from Power Control */
    MOD_PHYREG(pi, TxPwrCtrlCmd, use_txPwrCtrlCoefsIQ, 0);
    val = ((phy_txiqlocal_num_multilo(pi) != 0) || ACREV_IS(pi->pubpi->phy_rev, 1)) ? 1 : 0;
    MOD_PHYREG(pi, TxPwrCtrlCmd, use_txPwrCtrlCoefsLO, val);

#ifdef WL_SARLIMIT
    wlc_phy_set_sarlimit_acphy(pi_ac->tpci);
#endif
    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
        FOREACH_ACTV_CORE(pi, rx_coremask, core) {
        if (pi_ac->tpci->offset_targetpwr) {
            if (core == 0) {
            uint8 tgt_pwr_qdbm = tp_qtrdbm_each_core[core].target_pwr_qtrdbm;
            tgt_pwr_qdbm -= (pi_ac->tpci->offset_targetpwr * WLC_TXPWR_DB_FACTOR);
            wlc_phy_txpwrctrl_set_target_acphy(pi, tgt_pwr_qdbm, 0);
            }
        } else {
            /* set target powers */
            wlc_phy_txpwrctrl_set_target_acphy(pi, tp_qtrdbm_each_core[core].
                               target_pwr_qtrdbm, core);
        }
        PHY_TXPWR(("wl%d: %s: txpwrctl[%d]: %d\n",
            pi->sh->unit, __FUNCTION__, core,
            tp_qtrdbm_each_core[core].target_pwr_qtrdbm));
        }
    } else {
        while (core_count > 0) {
        --core_count;
        if (pi_ac->tpci->offset_targetpwr) {
            uint8 tgt_pwr_qdbm = tp_qtrdbm_each_core[core_count].target_pwr_qtrdbm;
            tgt_pwr_qdbm -= (pi_ac->tpci->offset_targetpwr * WLC_TXPWR_DB_FACTOR);
            wlc_phy_txpwrctrl_set_target_acphy(pi, tgt_pwr_qdbm, 0);
        } else {
            /* set target powers */
            wlc_phy_txpwrctrl_set_target_acphy(pi, tp_qtrdbm_each_core[core_count].
                               target_pwr_qtrdbm, tp_qtrdbm_each_core
                               [core_count].core);
        }
        PHY_TXPWR(("wl%d: %s: txpwrctl[%d]: %d\n",
                   pi->sh->unit, __FUNCTION__, tp_qtrdbm_each_core[core_count].core,
                   tp_qtrdbm_each_core[core_count].target_pwr_qtrdbm));
        }
    }
#ifdef ENABLE_FCBS
    if (IS_FCBS(pi) && pi->phy_fcbs->FCBS_INPROG)
        pi->phy_fcbs->FCBS_INPROG = 0;
    else {
#endif
        /* load estimated power tables (maps TSSI to power in dBm)
         *    entries in tx power table 0000xxxxxx
         */
        tbl_len = 128;

        if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
            ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
            wlc_phy_get_srom12_pdoffset_acphy(pi, pdoffsets, pdoffsets_80in160);

            // the following is for debugging only
            PHY_TXPWR(("wl%d: %s: pdoffsets: ", pi->sh->unit, __FUNCTION__));
            for (idx = 0; idx < ARRAYSIZE(pdoffsets); idx++) {
                PHY_TXPWR(("%d ", pdoffsets[idx]));
            }
            for (idx = 0; idx < ARRAYSIZE(pdoffsets_80in160); idx++) {
                PHY_TXPWR(("%d ", pdoffsets_80in160[idx]));
            }
            PHY_TXPWR(("\n"));

            /* get PA fittype */
            pa_fittype = wlc_phy_fittype_srom12_acphy(pi);
            PHY_TXPWR(("wl%d: %s:pa_fittype = %d\n",
                pi->sh->unit, __FUNCTION__, pa_fittype));
        } else if (ACMAJORREV_47_129_130(pi->pubpi->phy_rev)) {
            /* get PA fittype */
            pa_fittype = wlc_phy_fittype_srom12_acphy(pi);
            PHY_TXPWR(("wl%d: %s:pa_fittype = %d\n",
                pi->sh->unit, __FUNCTION__, pa_fittype));
        } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
            /* get PA fittype */
            pa_fittype = wlc_phy_fittype_srom12_acphy(pi);
            PHY_TXPWR(("wl%d: %s:pa_fittype = %d\n",
                pi->sh->unit, __FUNCTION__, pa_fittype));
        }
#ifdef WLC_TXCAL
    if (phy_tssical_get_pwr_tssi_tbl_in_use(pi->tssicali) == 1) {
        wlc_phy_apply_pwr_tssi_tble_chan_acphy(pi);
    } else {
#endif        /* WLC_TXCAL */
            FOREACH_ACTV_CORE(pi, rx_coremask, core)
            {
                PHY_TXPWR(("wl%d: %s: paparam of core%d = [%4x, %4x, %4x, %4x]\n",
                    pi->sh->unit, __FUNCTION__, core,
                    paparams[BW0][core].a, paparams[BW0][core].b,
                    paparams[BW0][core].c, paparams[BW0][core].d));
                PHY_TXPWR(("wl%d: %s: cck paparam of core%d = [%4x, "
                    "%4x, %4x, %4x]\n",
                    pi->sh->unit, __FUNCTION__, core,
                    paparams[BW2][core].a, paparams[BW2][core].b,
                    paparams[BW2][core].c, paparams[BW2][core].d));

                for (idx = 0; idx < tbl_len; idx++) {
                ctrSqr = idx * idx;
                if (ACMAJORREV_128(pi->pubpi->phy_rev) ||
                    ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
                    pwr_est_bw[BW0] = phy_ac_tpc_get_estpwrlut_srom12(
                            &paparams[BW0][core], pa_fittype, idx);
                } else if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                    ACMAJORREV_33(pi->pubpi->phy_rev) ||
                    ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
                    pwr_est_bw[BW0] = phy_ac_tpc_get_estpwrlut_srom12(
                            &paparams[BW0][core], pa_fittype, idx);

                    if (using_estpwr_lut_cck) {
                        pwr_est_cck = phy_ac_tpc_get_estpwrlut_srom12(
                                &paparams[BW2][core],
                                pa_fittype, idx);
                    }
                    if (using_estpwr_lut_2gbw20) {
                        pwr_est_2gbw20 = phy_ac_tpc_get_estpwrlut_srom12(
                                &paparams[BW1][core],
                                pa_fittype, idx);
                    }
                } else {
                    if (wlc_phy_fittype_srom12_acphy(pi) == 0) {
                        firstTerm  = (int32)paparams[BW0][core].a * 128;
                        secondTerm = ((int32)paparams[BW0][core].b * idx)
                                / 2;
                        thirdTerm  = ((int32)paparams[BW0][core].c *
                                ctrSqr) / 128;
                        fourthTerm = ((int32)paparams[BW0][core].d * idx) /
                            (((int32)idx - 128));
                        pwr_est_bw[BW0] = MAX((firstTerm + secondTerm +
                            thirdTerm + fourthTerm) / 8192, 0);
                        pwr_est_bw[BW0] = MIN(pwr_est_bw[BW0], 0x7e);
                    } else {
                        firstTerm = (int32)paparams[BW0][core].a * 16;
                        secondTerm = (paparams[BW0][core].b *
                                ctrSqr) / 4096;
                        if (idx == 0)
                            thirdTerm = 0;
                        else
                            thirdTerm = paparams[BW0][core].c *
                                ctrSqr / (ctrSqr -
                                ((int32)paparams[BW0][core].d *
                                2));
                        pwr_est_bw[BW0] = MAX((firstTerm + secondTerm +
                            thirdTerm) / 1024, 0);
                        pwr_est_bw[BW0] = MIN(pwr_est_bw[0], 0x7e);
                    }
                }

                if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                    ACMAJORREV_33(pi->pubpi->phy_rev)) {
                    /* The EstPwrShift table has 3 columns
                     * in 5G 20/40/80 and in 2G 20/40/CCK
                     */
                    if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
                        pwr_est_bw[BW1] = pwr_est_bw[BW0] +
                            pdoffsets[core];
                        pwr_est_bw[BW2] = pwr_est_bw[BW0] +
                            pdoffsets[core+PHY_CORE_MAX];
                    } else {
                        if (CHSPEC_IS20(pi->radio_chanspec)) {
                            pwr_est_bw[BW1] = pwr_est_bw[BW0];
                            pwr_est_bw[BW2] = 0;
                            if (using_estpwr_lut_cck) {
                                pwr_est_bw[BW0] = pwr_est_cck;
                            } else {
                                pwr_est_bw[BW0] = pwr_est_bw[BW1]
                                + pdoffsets[core+PHY_CORE_MAX];
                            }
                        } else {
                            pwr_est_bw[BW2] = pwr_est_bw[BW0];
                            pwr_est_bw[BW1] = pwr_est_bw[BW2] +
                                pdoffsets[core];
                            /* CCK pdoffset in 40MHz only */
                            if (using_estpwr_lut_cck) {
                                pwr_est_bw[BW0] = pwr_est_cck;
                            } else {
                                pwr_est_bw[BW0] = pwr_est_bw[BW2]
                                + pdoffsets[core+PHY_CORE_MAX];
                            }
                        }
                    }

                    for (bw_idx = 0; bw_idx < TPC_ESTPWRTBLS_MAX; bw_idx++) {
                        pwr_est_bw[bw_idx] = MIN(0x7f,
                                (MAX(pwr_est_bw[bw_idx],  0)));
                    }

                    regval[idx] = ((uint32)(pwr_est_bw[BW1] & 0xff)) |
                            (((uint32)(pwr_est_bw[BW2] & 0xff)) << 8) |
                            (((uint32)(pwr_est_bw[BW0] & 0xff)) << 16);
                } else if (ACMAJORREV_128(pi->pubpi->phy_rev) ||
                    ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
                    pwr_est_bw[BW1] = phy_ac_tpc_get_estpwrlut_srom12(
                                &paparams[BW1][core],
                                pa_fittype, idx);
                    pwr_est_bw[BW3] = 0;
                    if (!using_estpwr_lut_cck &&
                        CHSPEC_IS2G(pi->radio_chanspec)) {
                        pwr_est_bw[BW2] = pwr_est_bw[BW0];
                        pwr_est_bw[BW3] = pwr_est_bw[BW0];
                    } else {
                        pwr_est_bw[BW2] = phy_ac_tpc_get_estpwrlut_srom12(
                                &paparams[BW2][core],
                                pa_fittype, idx);
                        if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
                            if (CHSPEC_IS2G(pi->radio_chanspec)) {
                                pwr_est_bw[BW3] =
                                phy_ac_tpc_get_estpwrlut_srom12(
                                    &paparams[BW2][core],
                                    pa_fittype, idx);
                            } else if (
                                !ACMAJORREV_51(pi->pubpi->phy_rev))
                                {
                                pwr_est_bw[BW3] =
                                phy_ac_tpc_get_estpwrlut_srom12(
                                    &paparams[BW3][core],
                                    pa_fittype, idx);
                            }
                        }
                    }
                    for (bw_idx = 0; bw_idx < TPC_ESTPWRTBLS_MAX; bw_idx++) {
                        pwr_est_bw[bw_idx] = MIN(0x7f,
                                (MAX(pwr_est_bw[bw_idx],  0)));
                    }

                    regval[idx] = ((uint32)(pwr_est_bw[BW0] & 0xff)) |
                            (((uint32)(pwr_est_bw[BW1] & 0xff)) << 8) |
                            (((uint32)(pwr_est_bw[BW2] & 0xff)) << 16) |
                            (((uint32)(pwr_est_bw[BW3] & 0xff)) << 24);
                } else if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
                    /* The EstPwrShift table has 4 columns but all are set
                     * the same, offsets are compensated in the estPwrShftTbl
                     */
                    pwr_est_bw[BW1] = pwr_est_bw[BW0];
                    pwr_est_bw[BW2] = pwr_est_bw[BW0];
                    if (ACMAJORREV_47_129_130(pi->pubpi->phy_rev)) {
                        if (CHSPEC_IS2G(pi->radio_chanspec)) {
                            if (using_estpwr_lut_cck) {
                                pwr_est_bw[BW1] = pwr_est_cck;
                            } else {
                                pwr_est_bw[BW1] = pwr_est_bw[BW0];
                            }
                            if (using_estpwr_lut_2gbw20) {
                                pwr_est_bw[BW2] = pwr_est_2gbw20;
                            } else {
                                pwr_est_bw[BW2] = pwr_est_bw[BW0];
                            }
                        }
                    }
                    for (bw_idx = 0; bw_idx < TPC_ESTPWRTBLS_MAX; bw_idx++) {
                        pwr_est_bw[bw_idx] = MIN(0x7f,
                                (MAX(pwr_est_bw[bw_idx],  0)));
                    }
                    regval[idx] = ((uint32)(pwr_est_bw[BW0] & 0xff)) |
                            (((uint32)(pwr_est_bw[BW1] & 0xff)) << 8) |
                            (((uint32)(pwr_est_bw[BW2] & 0xff)) << 16) |
                            (((uint32)(pwr_est_bw[BW0] & 0xff)) << 24);
                } else {
                    regval[idx] = (uint16)(pwr_est_bw[BW0]&0xff);
                }
                PHY_TXPWR(("idx = %d, pwrest = %u (0x%x) \n",
                    idx, regval[idx], regval[idx]));
                }

                if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                    ACMAJORREV_33(pi->pubpi->phy_rev) ||
                    ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
                    /* Est Pwr Table is 128x24 Table. */
                    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_ESTPWRLUTS(core),
                        tbl_len, 0, 32, regval);
                } else {
                    /* Est Pwr Table is 128x8 Table. Limit Write to 8 bits */
                    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_ESTPWRLUTS(core),
                        tbl_len, 0, 16, regval);
                }
            }
#ifdef WLC_TXCAL
        }
#endif        /* WLC_TXCAL */
        /* start to populate estPwrShftTbl */

        if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
            if (chan_freq_range < WL_CHAN_FREQ_RANGE_5G_BAND5) {
                if (CHSPEC_IS80(pi->radio_chanspec) ||
                    PHY_AS_80P80(pi, pi->radio_chanspec)) {
                    chan_freq = chan_freq_range - WL_CHAN_FREQ_RANGE_5G_BAND0_80;
                } else if (CHSPEC_IS160(pi->radio_chanspec)) {
                    chan_freq = chan_freq_range - WL_CHAN_FREQ_RANGE_5G_BAND0_160;
                } else if (CHSPEC_IS40(pi->radio_chanspec)) {
                    chan_freq = chan_freq_range - WL_CHAN_FREQ_RANGE_5G_BAND0_40;
                } else {
                    chan_freq = chan_freq_range - WL_CHAN_FREQ_RANGE_5G_BAND0;
                }
            } else {
                if (CHSPEC_IS80(pi->radio_chanspec)) {
                    chan_freq = chan_freq_range - WL_CHAN_FREQ_RANGE_5G_BAND5_80;
                } else if (CHSPEC_IS160(pi->radio_chanspec)) {
                    chan_freq = chan_freq_range - WL_CHAN_FREQ_RANGE_5G_BAND4_160;
                } else if (CHSPEC_IS40(pi->radio_chanspec)) {
                    chan_freq = chan_freq_range - WL_CHAN_FREQ_RANGE_5G_BAND5_40;
                } else {
                    chan_freq = chan_freq_range - WL_CHAN_FREQ_RANGE_5G_BAND5;
                }
            }
        }

        // 24~40 of ESTPWRSHFTLUTS0 is for BW160 definition
        if (PHY_AS_80P80_CAP(pi) || CHSPEC_IS160(pi->radio_chanspec)) {
            shfttbl_len = 40;
        } else {
            shfttbl_len = 24;
        }
        if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
            shfttbl_len = 64;
        }
        if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
            /* Do not use ESTPWRSHFTLUTS table but offset ESTLUTPWR table instead */
            memset(shfttblval, 0, sizeof(shfttblval));
        } else if (ACMAJORREV_47_51_129_130_131(pi->pubpi->phy_rev) ||
            ACMAJORREV_128(pi->pubpi->phy_rev)) {
            if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
                if (chan_freq_range < WL_CHAN_FREQ_RANGE_5G_BAND5) {
                    phy_ac_fill_estpwrshft_table(pi, pwrdet,
                        chan_freq + 1, shfttbl_len, shfttblval);
                }
#if BAND6G
                else {
                    phy_ac_fill_estpwrshft_table_5gext(pi, pwrdet_5gext,
                    chan_freq, shfttbl_len, shfttblval);
                }
#endif /* BAND6G */
            } else if (CHSPEC_IS2G(pi->radio_chanspec)) {
                phy_ac_fill_estpwrshft_table(pi, pwrdet, chan_freq,
                shfttbl_len, shfttblval);
            }
        } else {
            memset(shfttblval, 0, sizeof(shfttblval));
        }
        if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
            ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
            wlc_phy_table_write_acphy(pi, AC2PHY_TBL_ID_ESTPWRSHFTLUTS0,
                shfttbl_len, 0, 32, shfttblval);
        } else {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_ESTPWRSHFTLUTS,
                shfttbl_len, 0, 32, shfttblval);
        }

#ifdef ENABLE_FCBS
    }
#endif

    ACPHY_ENABLE_STALL(pi, stall_val);
    if (regval) {
        phy_mfree(pi, regval, 128 * sizeof(uint32));
    }
}

/* Multi-mode 2 pa parameter assignment */
static void
phy_ac_tpc_get_paparams_percore_multimode_srom12(phy_info_t *pi, uint8 chan_freq_range,
        acphy_paparams_t params[][PHY_CORE_MAX], uint8 core, uint8 cck)
{
    /* Supports 4 tables each in a different 8 bit sections of the EstPwrLut
    **
    ** Params is 2D array, first dimension is the EstPwrLut table (for each BW/CCK),
    ** second dimension is core.
    ** Input chan_freq_range is defined in phy_ac_chanmgr_chan2freq_range_srom12
    */

    int offset = 0;
    srom12_pwrdet_t *pwrdet = pi->pwrdet_ac;

    switch (chan_freq_range) {
        case WL_CHAN_FREQ_RANGE_2G:
        case WL_CHAN_FREQ_RANGE_5G_BAND0:
        case WL_CHAN_FREQ_RANGE_5G_BAND1:
        case WL_CHAN_FREQ_RANGE_5G_BAND2:
        case WL_CHAN_FREQ_RANGE_5G_BAND3:
        case WL_CHAN_FREQ_RANGE_5G_BAND4:
            offset = 0;
            break;
        case WL_CHAN_FREQ_RANGE_2G_40:
        case WL_CHAN_FREQ_RANGE_5G_BAND0_40:
        case WL_CHAN_FREQ_RANGE_5G_BAND1_40:
        case WL_CHAN_FREQ_RANGE_5G_BAND2_40:
        case WL_CHAN_FREQ_RANGE_5G_BAND3_40:
        case WL_CHAN_FREQ_RANGE_5G_BAND4_40:
            /* Adjust index for 40M */
            offset = 6;
            break;
        case WL_CHAN_FREQ_RANGE_5G_BAND0_80:
        case WL_CHAN_FREQ_RANGE_5G_BAND1_80:
        case WL_CHAN_FREQ_RANGE_5G_BAND2_80:
        case WL_CHAN_FREQ_RANGE_5G_BAND3_80:
        case WL_CHAN_FREQ_RANGE_5G_BAND4_80:
            /* Adjust index for 80M */
            offset = 11;
            break;
        case WL_CHAN_FREQ_RANGE_5G_BAND0_160:
        case WL_CHAN_FREQ_RANGE_5G_BAND1_160:
        case WL_CHAN_FREQ_RANGE_5G_BAND2_160:
        case WL_CHAN_FREQ_RANGE_5G_BAND3_160:
            /* Adjust index for 160M */
            offset = 21;
            break;
        case WL_CHAN_FREQ_RANGE_5G_BAND4_160:
            offset = 32;
            break;

        default:
            PHY_ERROR(("wl%d: %s: pwrdet core%d, unsupported chan_freq_range:%d \n",
                    pi->sh->unit, __FUNCTION__, core, chan_freq_range));
            break;
    }

    ASSERT(chan_freq_range >= offset);

    params[BW0][core].a = pwrdet->pwrdet_a[core][chan_freq_range - offset];
    params[BW0][core].b = pwrdet->pwrdet_b[core][chan_freq_range - offset];
    params[BW0][core].c = pwrdet->pwrdet_c[core][chan_freq_range - offset];
    params[BW0][core].d = pwrdet->pwrdet_d[core][chan_freq_range - offset];

    params[BW1][core].a = pwrdet->pwrdet_a_40[core][chan_freq_range - offset];
    params[BW1][core].b = pwrdet->pwrdet_b_40[core][chan_freq_range - offset];
    params[BW1][core].c = pwrdet->pwrdet_c_40[core][chan_freq_range - offset];
    params[BW1][core].d = pwrdet->pwrdet_d_40[core][chan_freq_range - offset];

    if (chan_freq_range - offset > 0) {
        params[BW2][core].a = pwrdet->pwrdet_a_80[core][chan_freq_range - offset - 1];
        params[BW2][core].b = pwrdet->pwrdet_b_80[core][chan_freq_range - offset - 1];
        params[BW2][core].c = pwrdet->pwrdet_c_80[core][chan_freq_range - offset - 1];
        params[BW2][core].d = pwrdet->pwrdet_d_80[core][chan_freq_range - offset - 1];

        params[BW3][core].a = pwrdet->pwrdet_a_160[core][chan_freq_range - offset - 1];
        params[BW3][core].b = pwrdet->pwrdet_b_160[core][chan_freq_range - offset - 1];
        params[BW3][core].c = pwrdet->pwrdet_c_160[core][chan_freq_range - offset - 1];
        params[BW3][core].d = pwrdet->pwrdet_d_160[core][chan_freq_range - offset - 1];
    }

    if (cck == 1 && CHSPEC_IS2G(pi->radio_chanspec)) {
        params[BW2][core].a = pwrdet->pwrdet_a[core][CH_2G_GROUP + CH_5G_5BAND];
        params[BW2][core].b = pwrdet->pwrdet_b[core][CH_2G_GROUP + CH_5G_5BAND];
        params[BW2][core].c = pwrdet->pwrdet_c[core][CH_2G_GROUP + CH_5G_5BAND];
        params[BW2][core].d = pwrdet->pwrdet_d[core][CH_2G_GROUP + CH_5G_5BAND];
    }
}

static uint8
phy_ac_add_pdoff_ch14(uint8 pdoff1, uint8 pdoff2)
{
    /* this add signed 5-bitwide and 4-bitwide in unsigned format */
    int8 pd1 = (pdoff1 > 16) ? pdoff1 - 32 : pdoff1;
    int8 pd2 = (pdoff2 > 8) ? pdoff2 - 16 : pdoff2;
    int8 pd = pd1 + pd2;

    pd = (pd < -16) ? -16 : (pd > 15) ? 15 : pd; // limit to 5 bits
    return (uint8) (pd >= 0) ? pd : pd + 32;
}

/* Get the content for the ESTPWRSHFTLUTS_TBL table */
static void
phy_ac_fill_estpwrshft_table(phy_info_t *pi, srom12_pwrdet_t *pwrdet, uint8 chan_freq,
        uint8 shfttbl_len, uint32 *shfttblval)
{
    uint8  core, idx1;
    uint8  poffs;
    uint32 pdoffs;
    uint8  chan_freq_phybw160 = 0;
    int fc = wf_channel2mhz(CHSPEC_CHANNEL(pi->radio_chanspec),
        CHSPEC_IS2G(pi->radio_chanspec) ? WF_CHAN_FACTOR_2_4_G
        : CHSPEC_IS5G(pi->radio_chanspec) ? WF_CHAN_FACTOR_5_G
        : WF_CHAN_FACTOR_6_G);
    bool is_ch14 = (fc == 2484);

    PHY_TXPWR(("wl%d: %s: ESTPWRSHFTLUT table for band %d\n",
               pi->sh->unit, __FUNCTION__, chan_freq));

    for (idx1 = 0; idx1 < shfttbl_len; idx1++) {
        pdoffs = 0;
        FOREACH_CORE(pi, core) {
            if (ACMAJORREV_47_51_129_130_131(pi->pubpi->phy_rev) ||
                ACMAJORREV_128(pi->pubpi->phy_rev)) {

                if (ACMAJORREV_130(pi->pubpi->phy_rev) && SUBBANB5GV6(pi) &&
                    CHSPEC_IS160(pi->radio_chanspec) &&
                    (wf_chspec_primary20_chan(pi->radio_chanspec) >= 165)) {
                    // consider chan 163o 80mhz segment as uni-4
                    // and having a separate pd-offset as 5G subband-5
                    // note: this part of coding style is not optimal
                    //       and should be cleaned/addressed in srom19
                    chan_freq_phybw160 = 0;
                } else {
                    chan_freq_phybw160 = chan_freq;
                }

                switch (idx1) {
                case 0:
                // Ofdm20LL
                poffs = (uint8)(pwrdet->pdoffset20in80[core][chan_freq]);
                break;
                case 1:
                // Ofdm40L
                poffs = (uint8)(pwrdet->pdoffset40in80[core][chan_freq]);
                break;
                case 2:
                // Ofdm20L
                poffs = (uint8)(pwrdet->pdoffset20in40[core][chan_freq]);
                break;
                case 3:
                // Ofdm20in20
                poffs = 0;
                break;
                case 4:
                // Ofdm20U
                poffs = (uint8)(pwrdet->pdoffset20in40[core][chan_freq]);
                break;
                case 5:
                // Ofdm40in40
                poffs = 0;
                break;
                case 6:
                // Ofdm40U
                poffs = (uint8)(pwrdet->pdoffset40in80[core][chan_freq]);
                break;
                case 7:
                // Ofdm20UU
                poffs = (uint8)(pwrdet->pdoffset20in80[core][chan_freq]);
                break;
                case 8:
                // Ofdm20LU
                poffs = (uint8)(pwrdet->pdoffset20in80[core][chan_freq]);
                break;
                case 9:
                // Ofdm20UL
                poffs = (uint8)(pwrdet->pdoffset20in80[core][chan_freq]);
                break;
                case 10:
                // Ofdm40in40
                poffs = 0;
                break;
                case 16:
                // bphy20L
                poffs = (uint8)(pwrdet->pdoffsetcck20m[core]);
                break;
                case 17:
                // bphy20in20
                if (ACMAJORREV_130(pi->pubpi->phy_rev) && is_ch14) {
                    poffs = phy_ac_add_pdoff_ch14(pwrdet->pdoffsetcck[core],
                            pwrdet->pdoffsetcckch14[core]);
                } else {
                    poffs = (uint8)(pwrdet->pdoffsetcck[core]);
                }
                break;
                case 18:
                // bphy20U
                poffs = (uint8)(pwrdet->pdoffsetcck20m[core]);
                break;
                case 23:
                // Ofdm20LLL
                poffs = (uint8)(pwrdet->pdoffset20in160[core][chan_freq_phybw160]);
                break;
                case 24:
                // Ofdm40LL
                poffs = (uint8)(pwrdet->pdoffset40in160[core][chan_freq_phybw160]);
                break;
                case 25:
                // Ofdm40LU
                poffs = (uint8)(pwrdet->pdoffset40in160[core][chan_freq_phybw160]);
                break;
                case 26:
                // Ofdm20LUU
                poffs = (uint8)(pwrdet->pdoffset20in160[core][chan_freq_phybw160]);
                break;
                case 27:
                // Ofdm20LLU
                poffs = (uint8)(pwrdet->pdoffset20in160[core][chan_freq_phybw160]);
                break;
                case 28:
                // Ofdm20LUL
                poffs = (uint8)(pwrdet->pdoffset20in160[core][chan_freq_phybw160]);
                break;
                case 29:
                // Ofdm80L
                poffs = (uint8)(pwrdet->pdoffset80in160[core][chan_freq_phybw160]);
                break;
                case 30:
                // Ofdm20ULL
                poffs = (uint8)(pwrdet->pdoffset20in160[core][chan_freq_phybw160]);
                break;
                case 31:
                // Ofdm40UL
                poffs = (uint8)(pwrdet->pdoffset40in160[core][chan_freq_phybw160]);
                break;
                case 32:
                // Ofdm40UU
                poffs = (uint8)(pwrdet->pdoffset40in160[core][chan_freq_phybw160]);
                break;
                case 33:
                // Ofdm20UUU
                poffs = (uint8)(pwrdet->pdoffset20in160[core][chan_freq_phybw160]);
                break;
                case 34:
                // Ofdm20ULU
                poffs = (uint8)(pwrdet->pdoffset20in160[core][chan_freq_phybw160]);
                break;
                case 35:
                // Ofdm20UUL
                poffs = (uint8)(pwrdet->pdoffset20in160[core][chan_freq_phybw160]);
                break;
                case 36:
                // Ofdm80U
                poffs = (uint8)(pwrdet->pdoffset80in160[core][chan_freq_phybw160]);
                break;
                default:
                poffs = 0;
                break;
                }
            } else {
                if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
                    if ((idx1 == 0)||((idx1 > 6)&&(idx1 < 10))) {
                        poffs = (uint8)(pwrdet->
                        pdoffset20in80[core][chan_freq]);
                    } else if ((idx1 == 1)||(idx1 == 6)) {
                        poffs = (uint8)(pwrdet->
                        pdoffset40in80[core][chan_freq]);
                    } else if ((idx1 == 2) || (idx1 == 4)) {
                        poffs = (uint8)(pwrdet->
                        pdoffset20in40[core][chan_freq]);
                    } else {
                        poffs = 0;
                    }
                } else {
                    if (idx1 == 17) {
                        /* CCK 20in20 */
                        poffs = (uint8)(pwrdet->
                        pdoffsetcck[core]);
                    } else if (idx1 == 16 || idx1 == 18) {
                        /* CCK 20in40 (bphy20L and bphy20U) */
                        poffs = (uint8)(pwrdet->
                        pdoffsetcck20m[core]);
                    } else {
                        poffs = 0;
                    }
                }
            }
            poffs = (poffs > 15) ? (0xe0 | poffs) : poffs;
            pdoffs = pdoffs | (poffs << core*8);
        }
        PHY_TXPWR(("idx = %d, pdoffs = 0x%08x\n", idx1, pdoffs));
        shfttblval[idx1] = pdoffs;
    }
}

#ifdef PREASSOC_PWRCTRL
static void
wlc_phy_pwrctrl_shortwindow_upd_acphy(phy_info_t *pi, bool shortterm)
{
    if (shortterm) {
        /* 2 packet avergaing */
        MOD_PHYREG(pi, TxPwrCtrlNnum, Npt_intg_log2, PWRCTRL_SHORTW_AVG);
        if (ACMAJORREV_4(pi->pubpi->phy_rev) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            MOD_PHYREG(pi, TxPwrCtrlDamping, DeltaPwrDamp, 64);
        }
    } else {
        if (ACMAJORREV_4(pi->pubpi->phy_rev) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            /* 4 packet avergaing with Damp of 0.25 */
            MOD_PHYREG(pi, TxPwrCtrlNnum, Npt_intg_log2, 2);
            MOD_PHYREG(pi, TxPwrCtrlDamping, DeltaPwrDamp, 16);
        } else {
            /* 16 packet avergaing */
            MOD_PHYREG(pi, TxPwrCtrlNnum, Npt_intg_log2, PWRCTRL_LONGW_AVG);
        }
    }
}

#endif /* PREASSOC_PWRCTRL */

static uint8
wlc_phy_txpwrctrl_get_target_acphy(phy_info_t *pi, uint8 core)
{
    /* set target powers in 6.2 format (in dBs) */
    switch (core) {
    case 0:
        return READ_PHYREGFLD(pi, TxPwrCtrlTargetPwr_path0, targetPwr0);
        break;
    case 1:
        return READ_PHYREGFLD(pi, TxPwrCtrlTargetPwr_path1, targetPwr1);
        break;
    case 2:
        return READ_PHYREGFLD(pi, TxPwrCtrlTargetPwr_path2, targetPwr2);
        break;
    case 3:
        return READ_PHYREGFLD(pi, TxPwrCtrlTargetPwr_path3, targetPwr3);
        break;
    }
    return 0;
}

static
void wlc_phy_get_tssi_floor_acphy(phy_info_t *pi, int16 *floor)
{
    srom11_pwrdet_t *pwrdet = pi->pwrdet_ac;
    uint8 chan_freq_range, core, core_freq_segment_map;

    FOREACH_CORE(pi, core) {
        /* core_freq_segment_map is only required for 80P80 mode.
        For other modes, it is ignored
        */
        core_freq_segment_map =
            phy_ac_chanmgr_get_data(pi->u.pi_acphy->chanmgri)->core_freq_mapping[core];

        chan_freq_range = phy_ac_chanmgr_get_chan_freq_range(pi, pi->radio_chanspec,
            core_freq_segment_map);

        switch (chan_freq_range) {
        case WL_CHAN_FREQ_RANGE_2G:
        case WL_CHAN_FREQ_RANGE_5G_BAND0:
        case WL_CHAN_FREQ_RANGE_5G_BAND1:
        case WL_CHAN_FREQ_RANGE_5G_BAND2:
        case WL_CHAN_FREQ_RANGE_5G_BAND3:
            floor[core] = pwrdet->tssifloor[core][chan_freq_range];
        break;
        }
    }

}

#if defined(WLTEST) || defined(ATE_BUILD)
void
wlc_phy_tone_pwrctrl_loop(phy_info_t *pi, int8 targetpwr_dBm)
{
    uint8 core = 0; /* presently this functionality only required in RSDB mode */
    int8 pwr;
    int16  idle_tssi[PHY_CORE_MAX], tone_tssi[PHY_CORE_MAX];
    uint16 adjusted_tssi[PHY_CORE_MAX];
    int16 a1[PHY_CORE_MAX];
    int16 b1[PHY_CORE_MAX];
    int16 b0[PHY_CORE_MAX];
    int8 postive_slope = 1;
    int8 targetidx;
    int8 tx_idx;
    int8 deltapwr;
    txgain_setting_t txgain_settings;
    int8 orig_rxfarrow_shift = 0;
    uint16 orig_RxSdFeConfig6 = 0;
    bool bbmult_interpolation;

    if (targetpwr_dBm == -99) {
        targetidx = -1;
    } else {
        wlc_phy_get_paparams_for_band_acphy(pi, a1, b0, b1);

        /* meas the idle tssi */
        wlc_phy_txpwrctrl_idle_tssi_meas_acphy(pi);
        idle_tssi[core] = READ_PHYREGCE(pi, TxPwrCtrlIdleTssi_path, core) & 0x3ff;
        idle_tssi[core] = idle_tssi[core] - 1023;

        /* prevent crs trigger */
        phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, TRUE);

        if (!(ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)||
            ACMAJORREV_4(pi->pubpi->phy_rev))) {
            orig_rxfarrow_shift = READ_PHYREGFLD(pi, RxSdFeConfig6, rx_farrow_rshift_0);
            MOD_PHYREG(pi, RxSdFeConfig6, rx_farrow_rshift_0, 2);
        }

        if (RADIOID_IS(pi->pubpi->radioid, BCM20698_ID)) {
            /* TBD for 20698 radio */
            targetidx = -1;
            return;
        } else if (RADIOID_IS(pi->pubpi->radioid, BCM20704_ID)) {
            /* TBD for 20704 radio */
            targetidx = -1;
            return;
        } else if (RADIOID_IS(pi->pubpi->radioid, BCM20707_ID)) {
            /* TBD for 20707 radio */
            targetidx = -1;
            return;
        } else if (RADIOID_IS(pi->pubpi->radioid, BCM20710_ID)) {
            /* TBD for 20710 radio */
            targetidx = -1;
            return;
        }

        if ((RADIOID_IS(pi->pubpi->radioid, BCM20693_ID)) ||
                (RADIOID_IS(pi->pubpi->radioid, BCM20698_ID)) ||
                (RADIOID_IS(pi->pubpi->radioid, BCM20704_ID)) ||
                (RADIOID_IS(pi->pubpi->radioid, BCM20707_ID)) ||
                (RADIOID_IS(pi->pubpi->radioid, BCM20710_ID))) {
            phy_ac_tssi_loopback_path_setup(pi, LOOPBACK_FOR_TSSICAL);
            if (TINY_RADIO(pi) || (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                !ACMAJORREV_128(pi->pubpi->phy_rev))) {
                MOD_PHYREG(pi, RxSdFeConfig1, farrow_rshift_force, 1);
                orig_RxSdFeConfig6 = READ_PHYREG(pi, RxSdFeConfig6);
                MOD_PHYREG(pi, RxSdFeConfig6, rx_farrow_rshift_0,
                    READ_PHYREGFLD(pi, RxSdFeConfig1, farrow_rshift_tx));
            }
        } else
            wlc_phy_tssi_radio_setup_acphy(pi,
                    phy_stf_get_data(pi->stfi)->hw_phyrxchain, 0);

        tx_idx = 30;
        wlapi_suspend_mac_and_wait(pi->sh->physhim);
        phy_utils_phyreg_enter(pi);
        wlc_phy_txpwrctrl_enable_acphy(pi, PHY_TPC_HW_OFF);
        wlc_phy_txpwr_by_index_acphy(pi, (1 << core), tx_idx);
        wlc_phy_get_txgain_settings_by_index_acphy(
            pi, &txgain_settings, tx_idx);
        wlc_phy_poll_samps_WAR_acphy(pi, tone_tssi,
            TRUE, FALSE, &txgain_settings, FALSE, TRUE, 0, 0);
        adjusted_tssi[core] = 1023 - postive_slope * (tone_tssi[core] - idle_tssi[core]);
        adjusted_tssi[core] = adjusted_tssi[core] >> 3;

        if (TINY_RADIO(pi) || (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
             MOD_PHYREG(pi, RxSdFeConfig1, farrow_rshift_force, 0);
             WRITE_PHYREG(pi, RxSdFeConfig6, orig_RxSdFeConfig6);
        }
        /* prevent crs trigger */
        phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, TRUE);
        if (!(ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)||
            ACMAJORREV_4(pi->pubpi->phy_rev))) {
             MOD_PHYREG(pi, RxSdFeConfig6, rx_farrow_rshift_0, orig_rxfarrow_shift);
        }
        pwr = wlc_phy_tssi2dbm_acphy(pi, adjusted_tssi[core], a1[core], b0[core], b1[core]);

        /* delta pwr in qdb */
        deltapwr = (targetpwr_dBm * WLC_TXPWR_DB_FACTOR) - pwr;
        if (ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
            /* for 4350 with 0.5dB step size gaintable */
            targetidx = tx_idx - (deltapwr >> 1);
        } else {
            bbmult_interpolation = READ_PHYREGFLD(pi, TxPwrCtrlCmd, bbMultInt_en);
            if (ACMAJORREV_4(pi->pubpi->phy_rev) &&
                CHSPEC_ISPHY5G6G(pi->radio_chanspec)    && bbmult_interpolation) {
                targetidx = tx_idx - (deltapwr >> 1);
            } else {
                targetidx = tx_idx - deltapwr;
            }
        }

        wlc_phy_txpwr_by_index_acphy(pi, (1 << core), targetidx);
        phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, TRUE);
        wlc_phy_tx_tone_acphy(pi, ACPHY_IQCAL_TONEFREQ_2MHz, ACPHY_RXCAL_TONEAMP,
            TX_TONE_IQCAL_MODE_OFF, FALSE);
        phy_utils_phyreg_exit(pi);
        wlapi_enable_mac(pi->sh->physhim);
        phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, FALSE);
    }
}
#endif

static uint32
wlc_phy_pdoffset_cal_acphy(uint32 pdoffs, uint16 pdoffset, uint8 band, uint8 core)
{
    uint8 pdoffs_t;
    switch (band) {
    case WL_CHAN_FREQ_RANGE_2G:
        pdoffs_t = pdoffset & 0xf; break;
    case WL_CHAN_FREQ_RANGE_5G_BAND0:
        pdoffs_t = pdoffset & 0xf; break;
    case WL_CHAN_FREQ_RANGE_5G_BAND1:
        pdoffs_t = (pdoffset >> 4) & 0xf; break;
    case WL_CHAN_FREQ_RANGE_5G_BAND2:
        pdoffs_t = (pdoffset >> 8) & 0xf; break;
    case WL_CHAN_FREQ_RANGE_5G_BAND3:
        pdoffs_t = (pdoffset >> 12) & 0xf; break;
    default:
        pdoffs_t = pdoffset & 0xf; break;
    }

    pdoffs_t = (pdoffs_t > 7) ? (0xf0|pdoffs_t) : pdoffs_t;
    pdoffs   = pdoffs | (pdoffs_t << core*8);
    return pdoffs;

}

static bool
wlc_phy_txpwrctrl_ison_acphy(phy_info_t *pi)
{
    bool suspend = FALSE;
    bool retval;
    uint16 mask = (ACPHY_TxPwrCtrlCmd_txPwrCtrl_en_MASK(pi->pubpi->phy_rev) |
                   ACPHY_TxPwrCtrlCmd_hwtxPwrCtrl_en_MASK(pi->pubpi->phy_rev) |
                   ACPHY_TxPwrCtrlCmd_use_txPwrCtrlCoefs_MASK(pi->pubpi->phy_rev));

    /* Suspend MAC if haven't done so */
    wlc_phy_conditional_suspend(pi, &suspend);

    retval = ((READ_PHYREG(pi, TxPwrCtrlCmd) & mask) == mask);

    /* Resume MAC */
    wlc_phy_conditional_resume(pi, &suspend);

    return retval;
}

static uint8
wlc_phy_set_txpwr_clamp_acphy(phy_info_t *pi, uint8 core)
{
    uint16 idle_tssi_shift, adj_tssi_min;
    int16 tssi_floor[PHY_CORE_MAX] = {0};
    int16 idleTssi_2C = 0;
    int16 a1[PHY_CORE_MAX];
    int16 b0[PHY_CORE_MAX];
    int16 b1[PHY_CORE_MAX];
    uint8 pwr = 0;

    wlc_phy_get_tssi_floor_acphy(pi, tssi_floor);
    wlc_phy_get_paparams_for_band_acphy(pi, a1, b0, b1);

    idleTssi_2C = READ_PHYREGCE(pi, TxPwrCtrlIdleTssi_path, core) & 0x3ff;
    if (idleTssi_2C >= 512) {
        idle_tssi_shift = idleTssi_2C - 1023 - (-512);
    } else {
        idle_tssi_shift = 1023 + idleTssi_2C - 511;
    }
    idle_tssi_shift = idle_tssi_shift + 4;
    adj_tssi_min = MAX(tssi_floor[core], idle_tssi_shift);
    /* convert to 7 bits */
    adj_tssi_min = adj_tssi_min >> 3;

    pwr = wlc_phy_tssi2dbm_acphy(pi, adj_tssi_min, a1[core], b0[core], b1[core]);

    return pwr;
}

static int
wlc_phy_txpower_check_target_in_limits_acphy(phy_info_t *pi)
{
    uint8 core;
    int8 target_pwr_reg;
#ifdef WLC_TXCAL
    int8 txpwr_min;
#endif
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

    BCM_REFERENCE(stf_shdata);

    if (!pi->tpci->data->txpwroverride) {
        FOREACH_ACTV_CORE(pi, stf_shdata->phytxchain, core) {
            /* Check if target power register value is greater than board limits */
            target_pwr_reg = wlc_phy_txpwrctrl_get_target_acphy(pi, core);
            if (target_pwr_reg > pi->tpci->data->txpwr_max_boardlim_percore[core]) {
                PHY_FATAL_ERROR_MESG(("core%d, target_pwr_reg = %d, "
                    "board_lim = %d\n",
                    core, target_pwr_reg,
                    pi->tpci->data->txpwr_max_boardlim_percore[core]));
                PHY_FATAL_ERROR(pi, PHY_RC_TXPOWER_LIMITS);
                /* Change Target Register Value to be board_limit - backoff */
                wlc_phy_txpwrctrl_set_target_acphy(pi,
                    (pi->tpci->data->txpwr_max_boardlim_percore[core] -
                    PHY_TXPWRBCKOF_DEF),
                    core);
            }

#ifdef WLC_TXCAL
            /* Check if min power per rate is lower than the limit with OLPC on */
            if (!phy_tssical_get_disable_olpc(pi->tssicali)) {
                /* TXCAL Data based OLPC */
                if (phy_tssical_get_olpc_idx_valid_in_use(pi->tssicali)) {
                    txpwr_min = pi->tx_power_min_per_core[core];
                    if ((txpwr_min <
                        ACPHY_MIN_POWER_SUPPORTED_WITH_OLPC_QDBM) &&
                        (txpwr_min != WL_RATE_DISABLED)) {
                            PHY_FATAL_ERROR_MESG(("core%d, "
                                "txpwr_min = %d\n",
                                core, txpwr_min));
                            PHY_FATAL_ERROR(pi, PHY_RC_TXPOWER_LIMITS);
                    }
                }
            }
#endif /* WLC_TXCAL */
        }
    }
    return BCME_OK;
}

static void
wlc_phy_txpower_recalc_target_acphy(phy_info_t *pi)
{
    uint8 core;

    PHY_CHANLOG(pi, __FUNCTION__, TS_ENTER, 0);
#ifdef POWPERCHANNL
    if (PWRPERCHAN_ENAB(pi)) {
        if (!(ACMAJORREV_4(pi->pubpi->phy_rev)))
            /* update the board - limits per channel if in 2G Band */
            wlc_phy_tx_target_pwr_per_channel_set_acphy(pi);
    }
#endif /* POWPERCHANNL */
    wlapi_high_update_txppr_offset(pi->sh->physhim, pi->tx_power_offset,
        pi->tpci->data->ru_tx_power_offset);
    /* recalc targets -- turns hwpwrctrl off */

    if (IS_4364_3x3(pi)) {
       /* Disabling TPC for 4364 */
       MOD_PHYREG(pi, TxPwrCtrlCmd, txPwrCtrl_en, 0);
       FOREACH_CORE(pi, core) {
          wlc_phy_txpwrctrl_enable_acphy(pi, PHY_TPC_HW_OFF);
          wlc_phy_txpwr_by_index_acphy(pi, (1 << core), 70);
        }
    } else {
       if (SROMREV(pi->sh->sromrev) < 12) {
          wlc_phy_txpwrctrl_pwr_setup_acphy(pi);
       } else {
          wlc_phy_txpwrctrl_pwr_setup_srom12_acphy(pi);
       }
    }
    wlc_phy_txpower_check_target_in_limits_acphy(pi);

    /* restore power control */
    wlc_phy_txpwrctrl_enable_acphy(pi, pi->txpwrctrl);
    PHY_CHANLOG(pi, __FUNCTION__, TS_EXIT, 0);
}

#if defined(WL_MU_TX) && defined(WLVASIP)
static void
phy_ac_tpc_offload_ppr_to_svmp(phy_info_t *pi, ppr_t* tx_power_offset, int16 floor_pwr)
{
    uint8  k, m, n, bwtype, stall_val, num_set;
    int16  pwr0, pwr1, min_boff;
    /* additional backoff of steered frm pwr with nss_tot = 1, 2, 3, 4 and sounding pwr */
    int16  stpwr_boffs[5] = {0, 0, 4, 6, 0};
    uint32 mem_id, mem_addr;
    uint32 txbf_ppr_buff[8], svmp_addr, pwr_vht, pwr_he;
    ppr_vht_mcs_rateset_t pwr_bkf_vht;
    ppr_he_mcs_rateset_t  pwr_bkf_he;
    wl_tx_mode_t tx_mode[5] = {WL_TX_MODE_TXBF, WL_TX_MODE_TXBF, WL_TX_MODE_TXBF,
        WL_TX_MODE_TXBF, WL_TX_MODE_NONE};
    wl_tx_nss_t nss[5] = {WL_TX_NSS_1, WL_TX_NSS_2, WL_TX_NSS_3, WL_TX_NSS_4, WL_TX_NSS_4};
#if defined(WL_PSMX)
    int8 ndp_pwroffs[D11_MU_NDPPWR_MAXMCS+1] = {0};
    int ndppwr_num = D11_MU_NDPPWR_MAXMCS+1;
    uint16 ndp_pwr_tbl;
#endif /* WL_PSMX */
    uint txchain_cnt;
    uint8 txchain, rxchain;
    bool mac_enabled = FALSE;
    bool txpwroverride = pi->tpci->data->txpwroverride;
    bool ax_chip = !(ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev));

    if (wlapi_get_vasip_fw_loaded(pi->sh->physhim) == FALSE)
        return;

    //if (ACMAJORREV_47_129_130(pi->pubpi->phy_rev)) {
    //    mem_id = 8;
    //    mem_addr = 0x420;
    //} else if  (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
    //    mem_id = 3;
    //    mem_addr = 0x420;
    //} else {
    //    mem_id = 4;
    //    mem_addr = 0xe90;
    //}
    /* VASIP FW in KUDU consists of addr info so get it for all chips dynamically */
    svmp_addr = wlapi_get_svmp_addr_txbf_ppr_tbl(pi->sh->physhim);
    mem_id = SVMPADDR2MEMIDX(svmp_addr);
    mem_addr = SVMPADDR2MEMADDR(svmp_addr);

    if (R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC) {
        mac_enabled = TRUE;
    }
    if (mac_enabled) {
        /* Suspend MAC if haven't done so */
        wlapi_suspend_mac_and_wait(pi->sh->physhim);
    }
    /* disable stall */
    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    ACPHY_DISABLE_STALL(pi);
    BCM_REFERENCE(txbf_ppr_buff);
    phy_ac_tpc_stf_chain_get_valid(pi->u.pi_acphy->tpci, &txchain, &rxchain);
    txchain_cnt = PHY_BITSCNT(txchain);
    switch (txchain_cnt) {
    case 2:
        nss[2] = WL_TX_NSS_2;
        nss[3] = WL_TX_NSS_2;
        nss[4] = WL_TX_NSS_2;
        tx_mode[1] = WL_TX_MODE_NONE; // 2x2 NSS2_TXBF0 adopts SDM PPR
        tx_mode[2] = WL_TX_MODE_NONE;
        tx_mode[3] = WL_TX_MODE_NONE;
        txchain_cnt = WL_TX_CHAINS_2;
        break;
    case 3:
        nss[3] = WL_TX_NSS_3;
        nss[4] = WL_TX_NSS_3;
        tx_mode[2] = WL_TX_MODE_NONE; // 3x3 NSS3_TXBF0 adopts SDM PPR
        tx_mode[3] = WL_TX_MODE_NONE;
        txchain_cnt = WL_TX_CHAINS_3;
        break;
    case 4:
        tx_mode[3] = WL_TX_MODE_NONE; // 4x4 NSS4_TXBF0 adopts SDM PPR
        txchain_cnt = WL_TX_CHAINS_4;
        break;
    case 1:
    default:
        /* If 4 TX chains, set txchains according to the enum. For 1 or 2 chain
         * configurations, this code is a don't care anyway.
         */
        txchain_cnt = WL_TX_CHAINS_4;
        break;
    }

    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1, 0x8000, 32, &mem_id);

    if (CHSPEC_IS20(pi->radio_chanspec)) {
        num_set = 1;
    } else if (CHSPEC_IS40(pi->radio_chanspec)) {
        num_set = 2;
    } else if (CHSPEC_IS80(pi->radio_chanspec)) {
        num_set = 3;
    } else if (CHSPEC_IS160(pi->radio_chanspec)) {
        if (ACMAJORREV_130_131(pi->pubpi->phy_rev)) {
            num_set = 4;
        } else {
            num_set = 3;
        }
    } else {
        num_set = 0;
    }

    for (k = 0; k < num_set; k++) {
        if (CHSPEC_IS20(pi->radio_chanspec)) {
            bwtype = (k == 0)? WL_TX_BW_20: 0xff;
        } else if (CHSPEC_IS40(pi->radio_chanspec)) {
            bwtype = (k == 0)? WL_TX_BW_20IN40: ((k == 1)? WL_TX_BW_40: 0xff);
        } else if (CHSPEC_IS80(pi->radio_chanspec)) {
            bwtype = (k == 0)? WL_TX_BW_20IN80: ((k == 1)? WL_TX_BW_40IN80:
                    ((k == 2)? WL_TX_BW_80: 0xff));
        } else if (CHSPEC_IS160(pi->radio_chanspec)) {
            if (ACMAJORREV_130_131(pi->pubpi->phy_rev)) {
                bwtype = (k == 0)? WL_TX_BW_20IN160: ((k == 1)? WL_TX_BW_40IN160:
                ((k == 2)? WL_TX_BW_80IN160:((k == 3)? WL_TX_BW_160: 0xff)));
            } else {
                bwtype = (k == 0)? WL_TX_BW_20IN160: ((k == 1)? WL_TX_BW_40IN160:
                    ((k == 2)? WL_TX_BW_80IN160: 0xff));
            }
        } else {
            bwtype = 0xff;
        }
        /* loop through steered frame 1ss, 2ss, 3ss, 4ss followed by openloop 4ss */
        for (n = 0; n < 5; n++) {
            min_boff = (n == 0)? 0: (floor_pwr + stpwr_boffs[n]);
            ppr_get_vht_mcs(tx_power_offset, bwtype, nss[n], tx_mode[n],
                txchain_cnt, &pwr_bkf_vht);
            if (ax_chip) {
                ppr_get_he_mcs(tx_power_offset, bwtype, nss[n], tx_mode[n],
                    txchain_cnt, &pwr_bkf_he, WL_HE_RT_SU);
            }
            for (m = 0; m < 8; m++) {
                /* VHT */
                if (2*m < WL_RATESET_SZ_VHT_MCS_P) {
                    pwr0 = pwr_bkf_vht.pwr[2*m];
                    pwr1 = pwr_bkf_vht.pwr[2*m+1];
                    /* only apply power bounding for mcs >= 8 || ndp */
                    /* override mode: should just follow pwr_backoff.pwr[] */
                    if ((m >= 4 || n == 4) && !txpwroverride) {
                        pwr0 = (pwr0 < min_boff)? min_boff: pwr0;
                        pwr1 = (pwr1 < min_boff)? min_boff: pwr1;
                    }
                    /* convert to 2's complement format */
                    pwr0 += (pwr0 >= 0)? 0: 256;
                    pwr1 += (pwr1 >= 0)? 0: 256;
                    pwr_vht = (pwr0 & 0xFF) | ((pwr1 & 0xFF) << 16);
                } else {
                    pwr_vht = 0;
                }
                /* HE */
                if (ax_chip && (2*m < WL_RATESET_SZ_HE_MCS)) {
                    pwr0 = pwr_bkf_he.pwr[2*m];
                    pwr1 = pwr_bkf_he.pwr[2*m+1];
                    /* only apply power bounding for mcs >= 8 || ndp */
                    /* override mode: should just follow pwr_backoff.pwr[] */
                    if ((m >= 4 || n == 4) && !txpwroverride) {
                        pwr0 = (pwr0 < min_boff)? min_boff: pwr0;
                        pwr1 = (pwr1 < min_boff)? min_boff: pwr1;
                    }
                    /* convert to 2's complement format */
                    pwr0 += (pwr0 >= 0)? 0: 256;
                    pwr1 += (pwr1 >= 0)? 0: 256;
                    pwr_he = ((pwr0 & 0xFF) << 8) | ((pwr1 & 0xFF) << 24);
                } else {
                    pwr_he = 0;
                }
                if (bwtype == 0xff) {
                    txbf_ppr_buff[m] = 0;
                } else {
                    txbf_ppr_buff[m] = (pwr_vht | pwr_he);
                }
            }

            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 8,
                    mem_addr, 32, &txbf_ppr_buff[0]);
            mem_addr += 8;

#if defined(WL_PSMX)
            /* Save the MU NDP pwroffs table */
            if (!ax_chip)
                ndppwr_num = MIN(D11_MU_NDPPWR_MAXMCS+1, WL_RATESET_SZ_VHT_MCS);

            if ((bwtype <= WL_TX_BW_80 || bwtype ==  WL_TX_BW_80IN160) && n == 4) {
                for (m = 0; m < ndppwr_num; m++) {
                    if (ax_chip)
                        ndp_pwroffs[m] =
                            MAX(pwr_bkf_vht.pwr[m], pwr_bkf_he.pwr[m]);
                    else
                        ndp_pwroffs[m] = pwr_bkf_vht.pwr[m];
                }
            }
#endif /* WL_PSMX */
        }
    }

#if defined(WL_PSMX)
    if (wlapi_psmx_enabled(pi->sh->physhim)) {
        int8 spwr0, spwr1; // update to shm
        wlapi_suspend_macx_and_wait(pi->sh->physhim);

        if (ax_chip) {
            ndp_pwr_tbl = MX_VMU_NDPPWR_TBL(pi);
            for (m = 0; m < ndppwr_num; m += 2) {
                /* lower byte for even mcs , higher byte for odd mcs */
                spwr0 = pwr_bkf_vht.pwr[m] >= 0 ?
                    pwr_bkf_vht.pwr[m]: pwr_bkf_vht.pwr[m] + 256;
                spwr1 = pwr_bkf_vht.pwr[m+1] >= 0 ?
                    pwr_bkf_vht.pwr[m+1]: pwr_bkf_vht.pwr[m+1] + 256;
                spwr0 = spwr0 & 0x7F;
                spwr1 = spwr1 & 0x7F;
                wlapi_bmac_write_shmx(pi->sh->physhim, ndp_pwr_tbl + m,
                        spwr0 | (spwr1 << 8));
            }

            ndp_pwr_tbl = MX_HMU_NDPPWR_TBL(pi);
            for (m = 0; m < ndppwr_num; m += 2) {
                /* lower byte for even mcs , higher byte for odd mcs */
                spwr0 = pwr_bkf_he.pwr[m] >= 0 ?
                    pwr_bkf_he.pwr[m]: pwr_bkf_he.pwr[m] + 256;
                spwr1 = pwr_bkf_he.pwr[m+1] >= 0 ?
                    pwr_bkf_he.pwr[m+1]: pwr_bkf_he.pwr[m+1] + 256;
                spwr0 = spwr0 & 0x7F;
                spwr1 = spwr1 & 0x7F;
                wlapi_bmac_write_shmx(pi->sh->physhim, ndp_pwr_tbl + m,
                        spwr0 | (spwr1 << 8));
            }
        } else {
            ndp_pwr_tbl = MX_NDPPWR_TBL(pi);
            for (m = 0; m < ndppwr_num; m += 2) {
                /* lower byte for even mcs , higher byte for odd mcs */
                spwr0 = ndp_pwroffs[m] >= 0 ? ndp_pwroffs[m]: ndp_pwroffs[m] + 256;
                spwr1 = ndp_pwroffs[m+1] >= 0 ?
                        ndp_pwroffs[m+1]: ndp_pwroffs[m+1] + 256;
                if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                    ACMAJORREV_33(pi->pubpi->phy_rev)) {
                    spwr0 = (spwr0 >> 1) & 0x7F;
                    spwr1 = (spwr1 >> 1) & 0x7F;
                }
                wlapi_bmac_write_shmx(pi->sh->physhim, ndp_pwr_tbl + m,
                        spwr0 | (spwr1 << 8));
            }
        }
        wlapi_enable_macx(pi->sh->physhim);
    }
#endif /* WL_PSMX */

    /* restore stall value */
    ACPHY_ENABLE_STALL(pi, stall_val);

    if (mac_enabled) {
        /* Resume suspended MAC */
        wlapi_enable_mac(pi->sh->physhim);
    }
}
#endif /* WL_MU_TX && WLVASIP */

void
phy_ac_tpc_stf_chain_get_valid(phy_ac_tpc_info_t *tpci, uint8 *txchain, uint8 *rxchain)
{
    phy_info_t *pi = tpci->pi;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

    BCM_REFERENCE(stf_shdata);

    *txchain = stf_shdata->hw_phytxchain;
    *rxchain = stf_shdata->hw_phyrxchain;
    if (pi->sromi->sr13_en_sw_txrxchain_mask) {
        *txchain &= pi->sromi->sw_txchain_mask;
        *rxchain &= pi->sromi->sw_rxchain_mask;
    }
}

static void
wlc_phy_txpwrctrl_setminpwr(phy_info_t *pi)
{
    if (ACMAJORREV_2(pi->pubpi->phy_rev)) {
        pi->min_txpower = PHY_TXPWR_MIN_ACPHY2X2;
    } else if (ACMAJORREV_4(pi->pubpi->phy_rev) || IS_4364(pi)) {
        pi->min_txpower = PHY_TXPWR_MIN_ACPHY_EPA_2G;
        pi->min_txpower_5g = PHY_TXPWR_MIN_ACPHY_EPA_5G;
    } else if (ACMAJORREV_47_51_129_130_131(pi->pubpi->phy_rev)) {
        pi->min_txpower = pi->min_txpower_5g = PHY_TXPWR_MIN_HEPHY;
        PHY_TXPWR(("11ax %s set min tx power to %d \n",
            __FUNCTION__, pi->min_txpower));
    } else {
        pi->min_txpower = PHY_TXPWR_MIN_ACPHY;
    }
}

int8
wlc_phy_txpwrctrl_update_minpwr_acphy(phy_info_t *pi)
{
    int8 mintxpwr;

    wlc_phy_txpwrctrl_setminpwr(pi);

    if ((ACMAJORREV_4(pi->pubpi->phy_rev) || IS_4364(pi)) &&
        (CHSPEC_ISPHY5G6G(pi->radio_chanspec))) {
        mintxpwr = pi->min_txpower_5g * WLC_TXPWR_DB_FACTOR;
    } else {
        mintxpwr = pi->min_txpower * WLC_TXPWR_DB_FACTOR;
    }

    return mintxpwr;
}

#ifdef POWPERCHANNL
void
BCMATTACHFN(wlc_phy_tx_target_pwr_per_channel_limit_acphy)(phy_info_t *pi)
{ /* Limit the max and min offset values */
    srom11_pwrdet_t *pwrdet = pi->pwrdet_ac;
    uint8 core, ch_ind;
    FOREACH_CORE(pi, core) {
        for (ch_ind = 0; ch_ind < CH20MHz_NUM_2G; ch_ind++) {
            if (pwrdet->PwrOffsets2GNormTemp[core][ch_ind] >
                PWR_PER_CH_POS_OFFSET_LIMIT_QDBM) {
                pwrdet->PwrOffsets2GNormTemp[core][ch_ind] =
                    PWR_PER_CH_POS_OFFSET_LIMIT_QDBM;
            } else if (pwrdet->PwrOffsets2GNormTemp[core][ch_ind] <
                    -PWR_PER_CH_NEG_OFFSET_LIMIT_QDBM) {
                pwrdet->PwrOffsets2GNormTemp[core][ch_ind] =
                    -PWR_PER_CH_NEG_OFFSET_LIMIT_QDBM;
            }
            if (pwrdet->PwrOffsets2GLowTemp[core][ch_ind] >
                PWR_PER_CH_POS_OFFSET_LIMIT_QDBM) {
                pwrdet->PwrOffsets2GLowTemp[core][ch_ind] =
                    PWR_PER_CH_POS_OFFSET_LIMIT_QDBM;
            } else if (pwrdet->PwrOffsets2GLowTemp[core][ch_ind] <
                    -PWR_PER_CH_NEG_OFFSET_LIMIT_QDBM) {
                pwrdet->PwrOffsets2GLowTemp[core][ch_ind] =
                    -PWR_PER_CH_NEG_OFFSET_LIMIT_QDBM;
            }
            if (pwrdet->PwrOffsets2GHighTemp[core][ch_ind] >
                    PWR_PER_CH_POS_OFFSET_LIMIT_QDBM) {
                    pwrdet->PwrOffsets2GHighTemp[core][ch_ind] =
                        PWR_PER_CH_POS_OFFSET_LIMIT_QDBM;
            } else if (pwrdet->PwrOffsets2GHighTemp[core][ch_ind] <
                        -PWR_PER_CH_NEG_OFFSET_LIMIT_QDBM) {
                    pwrdet->PwrOffsets2GHighTemp[core][ch_ind] =
                        -PWR_PER_CH_NEG_OFFSET_LIMIT_QDBM;
            }
        }
    }
}
void
wlc_phy_tx_target_pwr_per_channel_decide_run_acphy(phy_info_t *pi)
{ /* Decide if should recaculate power per channel due to temp diff */
    srom11_pwrdet_t *pwrdet = pi->pwrdet_ac;
    uint8 ch = CHSPEC_CHANNEL(pi->radio_chanspec);
    int16 temp = phy_ac_temp_get(pi->u.pi_acphy->tempi); /* previous temprature */

    if (ch > CH20MHz_NUM_2G) /* 2 GHz channels only */
        return;
    if ((temp == 255) ){ /* This value is invalid - do not decide based on temp */
        return;
    }

    /* Check if temprature measurment is in a diffrenent temprature zone, */
    /*    with margin, than the Target power settings */
    if (((temp < pwrdet->Low2NormTemp - PWR_PER_CH_TEMP_MIN_STEP) &&
        (pwrdet->CurrentTempZone != PWR_PER_CH_LOW_TEMP)) ||
        ((temp > pwrdet->High2NormTemp + PWR_PER_CH_TEMP_MIN_STEP) &&
        (pwrdet->CurrentTempZone != PWR_PER_CH_HIGH_TEMP)) ||
        ((temp > pwrdet->Low2NormTemp + PWR_PER_CH_TEMP_MIN_STEP) &&
        (temp < pwrdet->High2NormTemp - PWR_PER_CH_TEMP_MIN_STEP) &&
        (pwrdet->CurrentTempZone != PWR_PER_CH_NORM_TEMP))) {
            wlc_phy_txpower_recalc_target_acphy(pi);
        }
}

static void
wlc_phy_tx_target_pwr_per_channel_set_acphy(phy_info_t *pi)
{
    srom11_pwrdet_t *pwrdet = pi->pwrdet_ac;
    uint8 core, ch_ind = CHSPEC_CHANNEL(pi->radio_chanspec)-1;
    int16 temp = phy_ac_temp_get(pi->u.pi_acphy->tempi); /* Copy temprture without measuring */

    ASSERT(pi->sh->sromrev >= 11);

    if (ch_ind >= CH20MHz_NUM_2G)
        return;

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    FOREACH_CORE(pi, core) {
        if ((pwrdet->Low2NormTemp != 0xff) && (temp < pwrdet->Low2NormTemp) &&
            (temp != 255)) {
            pwrdet->max_pwr[core][0] = pwrdet->max_pwr_SROM2G[core] +
                pwrdet->PwrOffsets2GLowTemp[core][ch_ind];
            pwrdet->CurrentTempZone = PWR_PER_CH_LOW_TEMP;
        } else if ((pwrdet->High2NormTemp != 0xff) && (temp > pwrdet->High2NormTemp) &&
            (temp != 255)) {
            pwrdet->max_pwr[core][0] = pwrdet->max_pwr_SROM2G[core] +
                pwrdet->PwrOffsets2GHighTemp[core][ch_ind];
            pwrdet->CurrentTempZone = PWR_PER_CH_HIGH_TEMP;
        } else {
            pwrdet->max_pwr[core][0] = pwrdet->max_pwr_SROM2G[core] +
                pwrdet->PwrOffsets2GNormTemp[core][ch_ind];
            pwrdet->CurrentTempZone = PWR_PER_CH_NORM_TEMP;
        }
        PHY_TXPWR(("wl%d: %s: core = %d  ChannelInd=%d Temprature=%d, ",
            pi->sh->unit, __FUNCTION__,
            core, ch_ind, temp));
        PHY_TXPWR(("Ch max pwr=%d, 2G max pwr =%d, Offset = %d \n",
            pwrdet->max_pwr[core][0],
            pwrdet->max_pwr_SROM2G[core],
            pwrdet->PwrOffsets2GNormTemp[core][ch_ind]));
    }
}
#endif /* POWPERCHANNL */

static int
BCMATTACHFN(phy_ac_tpc_nvram_attach)(phy_ac_tpc_info_t *tpci)
{
    uint8 i;
    phy_info_t *pi = tpci->pi;
#ifndef BOARD_FLAGS3
    uint32 bfl3; /* boardflags3 */
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
#endif

    for (i = 0; i < NUM_TXPWRINDEX_LIM; i++) {
        tpci->txpwrindexlimit[i] = (uint8) (PHY_GETINTVAR_ARRAY_DEFAULT_SLICE
            (pi, rstr_txpwrindexlimit, i, 0));
    }
    pi->sromi->precal_tx_idx = ((pi->sh->boardflags & BFL_SROM11_PRECAL_TX_IDX) != 0);
    pi->sromi->txidxcap2g = (uint8)PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_txidxcap2g, 0);
    pi->sromi->txidxcap5g = (uint8)PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_txidxcap5g, 0);

#ifndef BOARD_FLAGS3
    if ((PHY_GETVAR_SLICE(pi, rstr_boardflags3)) != NULL) {
        bfl3 = (uint32)PHY_GETINTVAR_SLICE(pi, rstr_boardflags3);
        BF3_TXGAINTBLID(pi_ac) = (bfl3 & BFL3_TXGAINTBLID) >> BFL3_TXGAINTBLID_SHIFT;
        BF3_PPR_BIT_EXT(pi_ac) = (bfl3 & BFL3_PPR_BIT_EXT) >> BFL3_PPR_BIT_EXT_SHIFT;
        BF3_2GTXGAINTBL_BLANK(pi_ac) = (bfl3 & BFL3_2GTXGAINTBL_BLANK) >>
            BFL3_2GTXGAINTBL_BLANK_SHIFT;
        BF3_5GTXGAINTBL_BLANK(pi_ac) = (bfl3 & BFL3_5GTXGAINTBL_BLANK) >>
            BFL3_5GTXGAINTBL_BLANK_SHIFT;
    } else {
        BF3_TXGAINTBLID(pi_ac) = 0;
        BF3_PPR_BIT_EXT(pi_ac) = 0;
        BF3_2GTXGAINTBL_BLANK(pi_ac) = 0;
        BF3_5GTXGAINTBL_BLANK(pi_ac) = 0;
    }
#endif /* BOARD_FLAGS3 */

    pi->sromi->dettype_2g = (pi->sh->boardflags4 & BFL4_SROM12_2G_DETTYPE) >> 1;
    pi->sromi->dettype_5g = (pi->sh->boardflags4 & BFL4_SROM12_5G_DETTYPE) >> 2;
    pi->sromi->sr13_dettype_en = (pi->sh->boardflags4 & BFL4_SROM13_DETTYPE_EN) >> 3;
    pi->sromi->sr13_cck_spur_en = (pi->sh->boardflags4 & BFL4_SROM13_CCK_SPUR_EN) >> 4;
    pi->sromi->sr13_en_sw_txrxchain_mask =
            ((pi->sh->boardflags4 & BFL4_SROM13_EN_SW_TXRXCHAIN_MASK) != 0);
    pi->sromi->sr13_1p5v_cbuck = ((pi->sh->boardflags4 & BFL4_SROM13_1P5V_CBUCK) != 0);
    pi->sromi->epa_on_during_txiqlocal = ((pi->sh->boardflags2 &
        BFL2_SROM11_EPA_ON_DURING_TXIQLOCAL) != 0);
    pi->tpci->data->tx_pwr_ctrl_damping_en    = (int8) (PHY_GETINTVAR_DEFAULT_SLICE(pi,
        rstr_pwrdampingen, 0));
    pi->sromi->sr18_cck_paparam_en = (pi->sh->boardflags4 & BFL4_SROM18_CCK_PAPARAM_EN) >> 16;
    pi->sromi->sr18_txpwr_cap_en = (pi->sh->boardflags4 & BFL4_SROM18_TXPWR_CAP_EN) >> 18;
    pi->sromi->sr18_pkt_based_idletssi_en =
        (pi->sh->boardflags4 & BFL4_SROM18_PKT_BASED_IDLETSSI) >> 25;

    return BCME_OK;
}

/* ********************************************* */
/*                External Definitions                    */
/* ********************************************* */

uint8
wlc_phy_tssi2dbm_acphy(phy_info_t *pi, int32 tssi, int32 a1, int32 b0, int32 b1)
{
        int32 num, den;
        int8 pwrest = 0;
        uint8 core;
        uint8 phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;

        BCM_REFERENCE(phyrxchain);

        FOREACH_ACTV_CORE(pi, phyrxchain, core) {
            num = 8*(16*b0+b1*tssi);
            den = 32768+a1*tssi;
            pwrest = MAX(((4*num+den/2)/den), -8);
            if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                ACMAJORREV_33(pi->pubpi->phy_rev) ||
                (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                !ACMAJORREV_128(pi->pubpi->phy_rev))) {
                pwrest = MIN(pwrest, 0x7f);
            } else {
                pwrest = MIN(pwrest, 0x7e);
            }
        }
        return pwrest;
}

void
wlc_phy_read_txgain_acphy(phy_info_t *pi)
{
    uint8 core;
    uint8 stall_val;
    txgain_setting_t txcal_txgain[4];
    uint16 lpfgain;
    uint8 phyrxchain;

    BCM_REFERENCE(phyrxchain);

    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    ACPHY_DISABLE_STALL(pi);

    phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;
    FOREACH_ACTV_CORE(pi, phyrxchain, core) {
        /* store off orig tx radio gain */
        if (core == 3) {
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x501, 16,
                                     &(txcal_txgain[core].rad_gain));
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x504, 16,
                                     &(txcal_txgain[core].rad_gain_mi));
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x507, 16,
                                     &(txcal_txgain[core].rad_gain_hi));
            /* for 6715 lpf_bq2_gain is taken from the tx-gaincode direct contrl */
            if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                !ACMAJORREV_128(pi->pubpi->phy_rev) &&
                !ACMAJORREV_130(pi->pubpi->phy_rev)) {
                // Tx BQ2 gain is in a 3-bit field at different location
                wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x51e, 16,
                                         &lpfgain);
                txcal_txgain[core].rad_gain &= 0xFF0F;
                txcal_txgain[core].rad_gain |= (lpfgain & 0x0007) << 4;
            }
        } else {
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, (0x100 + core), 16,
                                     &(txcal_txgain[core].rad_gain));
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, (0x103 + core), 16,
                                     &(txcal_txgain[core].rad_gain_mi));
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, (0x106 + core), 16,
                                     &(txcal_txgain[core].rad_gain_hi));
            /* for 6715 lpf_bq2_gain is taken from the tx-gaincode direct contrl */
            if (ACMAJORREV_GE37(pi->pubpi->phy_rev) &&
                !ACMAJORREV_130(pi->pubpi->phy_rev)) {
                // Tx BQ2 gain is in a 3-bit field at different location
                wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                        (0x17e + 16*core), 16, &lpfgain);
                txcal_txgain[core].rad_gain &= 0xFF0F;
                txcal_txgain[core].rad_gain |= (lpfgain & 0x0007) << 4;
            }
        }
        wlc_phy_get_tx_bbmult_acphy(pi, &(txcal_txgain[core].bbmult),  core);
        PHY_NONE(("\n radio gain = 0x%x%x%x, bbm=%d  \n",
            txcal_txgain[core].rad_gain_hi,
            txcal_txgain[core].rad_gain_mi,
            txcal_txgain[core].rad_gain,
            txcal_txgain[core].bbmult));
    }
    ACPHY_ENABLE_STALL(pi, stall_val);
}

void
wlc_phy_txpwr_by_index_acphy(phy_info_t *pi, uint8 core_mask, int8 txpwrindex)
{
    uint8 core;
    uint8 stall_val;
    bool suspend = FALSE;
    uint16 coeff_vals;
    uint8 papd_lut_select;
    uint8 papdlutsel_table_ids[] = {ACPHY_TBL_ID_PAPDLUTSELECT0, ACPHY_TBL_ID_PAPDLUTSELECT1,
        ACPHY_TBL_ID_PAPDLUTSELECT2, ACPHY_TBL_ID_PAPDLUTSELECT3};

    /* Suspend MAC if haven't done so */
    wlc_phy_conditional_suspend(pi, &suspend);

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    (void) wlc_phy_set_txpwr_by_index_acphy(pi, core_mask, txpwrindex);

    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    ACPHY_DISABLE_STALL(pi);

    /* Set tx power based on an input "index"
     * (Emulate what HW power control would use for a given table index)
     */
    FOREACH_ACTV_CORE(pi, core_mask, core) {

        /* Check txprindex >= 0 */
        if (txpwrindex < 0)
            ASSERT(0); /* negative index not supported */

        if (PHY_PAPDEN(pi)) {
            if ((pi->acphy_txpwrctrl == PHY_TPC_HW_OFF) ||
                (TINY_RADIO(pi)) || ACMAJORREV_51_129_130_131(pi->pubpi->phy_rev)) {
                int16 rfPwrLutVal;
                uint8 rfpwrlut_table_ids[] = { ACPHY_TBL_ID_RFPWRLUTS0,
                    ACPHY_TBL_ID_RFPWRLUTS1, ACPHY_TBL_ID_RFPWRLUTS2,
                    ACPHY_TBL_ID_RFPWRLUTS3};

                if ((!TINY_RADIO(pi)) &&
                    (!ACMAJORREV_51_131(pi->pubpi->phy_rev)) &&
                    (!ACMAJORREV_128(pi->pubpi->phy_rev)) &&
                    (!ACMAJORREV_129_130(pi->pubpi->phy_rev))) {
                    MOD_PHYREGCEE(pi, EpsilonTableAdjust, core, epsilonOffset,
                    0);
                }
                MOD_PHYREGCEE(pi, PapdEnable, core, gain_dac_rf_override,
                    1);
                wlc_phy_table_read_acphy(pi, rfpwrlut_table_ids[core],
                    1, txpwrindex, 16, &rfPwrLutVal);
                MOD_PHYREGCEE(pi, PapdEnable, core, gain_dac_rf_reg,
                    rfPwrLutVal);

                if (WBPAPD_ENAB(pi) && ACMAJORREV_130(pi->pubpi->phy_rev)) {
                    MOD_PHYREGCEE(pi, PapdLutSel0, core, papd_lut_select_ovr,
                        1);
                    wlc_phy_table_read_acphy(pi, papdlutsel_table_ids[core],
                        1, txpwrindex, 8, &papd_lut_select);
                    MOD_PHYREGCEE(pi, PapdLutSel0, core,
                        papd_lut_select_ovr_val, (papd_lut_select & 0x7));
                    MOD_PHYREGCEE(pi, PapdLutSel0, core,
                        papd_lut_select_ovr_val_cck, (papd_lut_select>>3));
                }

#ifdef WL_ETMODE
                if (ET_ENAB(pi)) {
                    int etflag;
                    etflag = (int8) pi->ff->_et;
                    /* For Envelope Tracking supported cases only...
                     * There is a bug in 43012, which necessitates a negation...
                     * rf_pwr_ovrd. This needs to be in for TPC OFF when ...
                     * Envelope Tracking is enabled/ operating in ET mode
                     */
                    if (etflag > 0) {
                        int16 rfPwrLutVal;
                        uint8 rfpwrlut_table_ids[] =
                            { ACPHY_TBL_ID_RFPWRLUTS0,
                            ACPHY_TBL_ID_RFPWRLUTS1,
                            ACPHY_TBL_ID_RFPWRLUTS2};
                        wlc_phy_table_read_acphy(pi,
                            rfpwrlut_table_ids[core],
                            1, txpwrindex, 16, &rfPwrLutVal);
                        MOD_PHYREGCE(pi, rf_pwr_ovrd, core, rf_pwr_ovrd, 1);
                        MOD_PHYREGCE(pi, rf_pwr_ovrd, core,
                            rf_pwr_ovrd_value, -1*rfPwrLutVal);
                    }
                }
#endif /* WL_ETMODE */
            }
        }

        if (ACMAJORREV_47_129_130(pi->pubpi->phy_rev) &&
            (CHSPEC_ISPHY5G6G(pi->radio_chanspec) == 1)) {
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_LOFTCOEFFLUTS(core), 1,
                txpwrindex, 16, &coeff_vals);
            wlc_phy_cal_txiqlo_coeffs_acphy(pi, CAL_COEFF_WRITE,
                &coeff_vals, TB_OFDM_COEFFS_D,  core);
            wlc_phy_cal_txiqlo_coeffs_acphy(pi, CAL_COEFF_WRITE,
                &coeff_vals, TB_BPHY_COEFFS_D,  core);
        }

        /* Update the per-core state of power index */
        pi->u.pi_acphy->txpwrindex[core] = txpwrindex;
    }
    ACPHY_ENABLE_STALL(pi, stall_val);
    /* Resume MAC */
    wlc_phy_conditional_resume(pi, &suspend);
}

void
wlc_phy_get_txgain_settings_by_index_acphy(phy_info_t *pi, txgain_setting_t *txgain_settings,
                                     int8 txpwrindex)
{
    uint16 txgain[3];

    wlc_phy_table_read_acphy(pi, wlc_phy_get_tbl_id_gainctrlbbmultluts(pi, 0),
        1, txpwrindex, 48, &txgain);

    txgain_settings->rad_gain    = ((txgain[0] >> 8) & 0xff) + ((txgain[1] & 0xff) << 8);
    txgain_settings->rad_gain_mi = ((txgain[1] >> 8) & 0xff) + ((txgain[2] & 0xff) << 8);
    txgain_settings->rad_gain_hi = ((txgain[2] >> 8) & 0xff);
    txgain_settings->bbmult      = (txgain[0] & 0xff);
}

void
wlc_phy_get_tx_bbmult_acphy(phy_info_t *pi, uint16 *bb_mult, uint16 core)
{
    uint16 tbl_ofdm_offset[] = { 99, 103, 107, 111};
    uint8 iqlocal_tbl_id = wlc_phy_get_tbl_id_iqlocal(pi, core);

    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        core = 0;
    }

    wlc_phy_table_read_acphy(pi, iqlocal_tbl_id, 1,
                             tbl_ofdm_offset[core], 16,
                             bb_mult);
}

void
wlc_phy_set_tx_bbmult_acphy(phy_info_t *pi, uint16 *bb_mult, uint16 core)
{
    uint16 tbl_ofdm_offset[] = { 99, 103, 107, 111};
    uint16 tbl_bphy_offset[] = {115, 119, 123, 127};
    uint8 stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    uint8 iqlocal_tbl_id = wlc_phy_get_tbl_id_iqlocal(pi, core);

    ACPHY_DISABLE_STALL(pi);

    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        core = 0;
    }

    wlc_phy_table_write_acphy(pi, iqlocal_tbl_id, 1,
                              tbl_ofdm_offset[core], 16,
                              bb_mult);
    wlc_phy_table_write_acphy(pi, iqlocal_tbl_id, 1,
                              tbl_bphy_offset[core], 16,
                              bb_mult);
    ACPHY_ENABLE_STALL(pi, stall_val);
}

uint32
wlc_phy_txpwr_idx_get_acphy(phy_info_t *pi)
{
    uint8 core, rx_coremask;
    uint32 pwr_idx[] = {0, 0, 0, 0};
    uint32 tmp = 0;
    bool suspend = FALSE;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

    BCM_REFERENCE(stf_shdata);

    BCM_REFERENCE(rx_coremask);
    rx_coremask = stf_shdata->hw_phyrxchain;

    /* Suspend MAC if haven't done so */
    wlc_phy_conditional_suspend(pi, &suspend);

    if (wlc_phy_txpwrctrl_ison_acphy(pi)) {
        FOREACH_ACTV_CORE(pi, rx_coremask, core) {
            pwr_idx[core] = READ_PHYREGFLDCE(pi, TxPwrCtrlStatus_path, core, baseIndex);
        }
    } else {
        FOREACH_ACTV_CORE(pi, rx_coremask, core) {
            pwr_idx[core] = (pi->u.pi_acphy->txpwrindex[core] & 0xff);
        }
    }
    tmp = (pwr_idx[3] << 24) | (pwr_idx[2] << 16) | (pwr_idx[1] << 8) | pwr_idx[0];

    /* Resume MAC */
    wlc_phy_conditional_resume(pi, &suspend);
    return tmp;
}

void
wlc_phy_txpwrctrl_enable_acphy(phy_info_t *pi, uint8 ctrl_type)
{
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t *)pi->u.pi_acphy->tpci;
    uint16 mask;
    uint8 core, rx_coremask;
    bool suspend = FALSE;
#if defined(WLC_TXCAL)
    bool olpc_idx_valid_in_use = phy_tssical_get_olpc_idx_valid_in_use(pi->tssicali);
#endif /* WLC_TXCAL */
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

    BCM_REFERENCE(stf_shdata);

    /* Suspend MAC if haven't done so */
    wlc_phy_conditional_suspend(pi, &suspend);

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    /* check for recognized commands */
    switch (ctrl_type) {
    case PHY_TPC_HW_OFF:
    case PHY_TPC_HW_ON:
        pi->txpwrctrl = ctrl_type;
        break;
    default:
        PHY_ERROR(("wl%d: %s: Unrecognized ctrl_type: %d\n",
            pi->sh->unit, __FUNCTION__, ctrl_type));
        break;
    }

    BCM_REFERENCE(rx_coremask);
    rx_coremask = stf_shdata->hw_phyrxchain;

    if (ctrl_type == PHY_TPC_HW_OFF) {
        /* save previous txpwr index if txpwrctl was enabled */
        if (wlc_phy_txpwrctrl_ison_acphy(pi)) {
            FOREACH_ACTV_CORE(pi, rx_coremask, core) {
                tpci->txpwrindex_hw_save[core] =
                    ((ACMAJORREV_51_131(pi->pubpi->phy_rev) &&
                    tpci->txpwrindex_hw_save[core] == 128) ?
                    tpci->ti->data->base_index_init[core] :
                    READ_PHYREGFLDCE(pi, TxPwrCtrlStatus_path, core,
                    baseIndex));
                PHY_TXPWR(("wl%d: %s PWRCTRL: %d Cache Baseindex: %d Core: %d\n",
                    pi->sh->unit, __FUNCTION__, ctrl_type,
                    tpci->txpwrindex_hw_save[core], core));
            }
        }
        tpci->txpwrindex_hw_save_chan = CHSPEC_CHANNEL(pi->radio_chanspec);
        /* Disable hw txpwrctrl */
        mask = ACPHY_TxPwrCtrlCmd_txPwrCtrl_en_MASK(pi->pubpi->phy_rev) |
               ACPHY_TxPwrCtrlCmd_hwtxPwrCtrl_en_MASK(pi->pubpi->phy_rev) |
               ACPHY_TxPwrCtrlCmd_use_txPwrCtrlCoefs_MASK(pi->pubpi->phy_rev);
        _PHY_REG_MOD(pi, ACPHY_TxPwrCtrlCmd(pi->pubpi->phy_rev), mask, 0);
    } else {
        /* XXX FIXME: measure idle tssi when txpwrctrl is enabled
           to be removed when txpwrctrl is turned on by default
        */
        /* Setup the loopback path here */
        phy_ac_tssi_loopback_path_setup(pi, LOOPBACK_FOR_TSSICAL);

        /* Enable hw txpwrctrl */
        mask = ACPHY_TxPwrCtrlCmd_txPwrCtrl_en_MASK(pi->pubpi->phy_rev) |
            ACPHY_TxPwrCtrlCmd_hwtxPwrCtrl_en_MASK(pi->pubpi->phy_rev) |
            ACPHY_TxPwrCtrlCmd_use_txPwrCtrlCoefs_MASK(pi->pubpi->phy_rev);
        _PHY_REG_MOD(pi, ACPHY_TxPwrCtrlCmd(pi->pubpi->phy_rev), mask, mask);
        FOREACH_ACTV_CORE(pi, rx_coremask, core) {
            if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                ACMAJORREV_33(pi->pubpi->phy_rev) ||
                (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                !ACMAJORREV_128(pi->pubpi->phy_rev))) {
                if (tpci->txpwrindex_hw_save[core] != 128) {
                    MOD_PHYREGCEE(pi, TxPwrCtrlInit_path, core,
                    pwrIndex_init_path, tpci->txpwrindex_hw_save[core]);
                    PHY_TXPWR(("wl%d:%s PWRCTRL:%d Restore Baseindex:"
                    "%d Core:%d\n",
                    pi->sh->unit, __FUNCTION__, ctrl_type,
                    tpci->txpwrindex_hw_save[core], core));
                }
            } else {
#if defined(WLC_TXCAL)
                if (tpci->ti->data->txpwroverride &&
                    olpc_idx_valid_in_use) {
                    /* If in override mode and table based txcal */
                    /* olpc anchor power tx idx is valid, */
                    /* set init idx based on anchor power tx idx */
                    /* and target power */
                    uint8 iidx = 0;
                    wlc_phy_olpc_idx_tempsense_comp_acphy(pi, &iidx, core);
                    MOD_PHYREGCEE(pi, TxPwrCtrlInit_path, core,
                        pwrIndex_init_path, iidx);
                } else {
                    if ((tpci->txpwrindex_hw_save_chan !=
                        CHSPEC_CHANNEL(pi->radio_chanspec)) &&
                        olpc_idx_valid_in_use) {
                        /* If the saved txpwrindex is for another
                           channel. Do not reapply it to prevent
                           the baseindex for txcal based olpc gets
                           overwritten
                        */
                        tpci->txpwrindex_hw_save[core] = 128;
                    }
#endif /* WLC_TXCAL */
                    if (tpci->txpwrindex_hw_save[core] != 128) {
                        wlc_phy_txpwrctrl_set_baseindex(pi, core,
                            tpci->txpwrindex_hw_save[core], 1);
                        PHY_TXPWR(("wl%d: %s PWRCTRL: %d "
                            "Restore Baseindex:%d Core: %d\n",
                            pi->sh->unit, __FUNCTION__,
                            ctrl_type,
                            tpci->txpwrindex_hw_save[core], core));
                    }
                    if (TINY_RADIO(pi) &&
                        (tpci->txpwrindex_cck_hw_save[core] !=
                            128)) {
                        wlc_phy_txpwrctrl_set_baseindex(pi, core,
                            tpci->txpwrindex_cck_hw_save[core],
                            0);
                        PHY_TXPWR(("wl%d: %s PWRCTRL: %d Restore"
                            " Baseindex cck:"
                            "%d Core: %d\n",
                            pi->sh->unit, __FUNCTION__,
                                ctrl_type,
                            tpci->txpwrindex_cck_hw_save[core], core));
                    }
#if defined(WLC_TXCAL)
                }
#endif /* WLC_TXCAL */
            }
        }

        if (!TINY_RADIO(pi)) {
            FOREACH_ACTV_CORE(pi, stf_shdata->hw_phyrxchain, core) {
                MOD_PHYREGCEE(pi, PapdEnable, core, gain_dac_rf_override, 0);
#ifdef WL_ETMODE
                if (ET_ENAB(pi)) {
                    MOD_PHYREGCE(pi, rf_pwr_ovrd, core, rf_pwr_ovrd, 0);
                }
#endif /* WL_ETMODE */
            }
        }
    }

    if (TINY_RADIO(pi) || (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        uint16 txPwrCtrlCmd = READ_PHYREGFLD(pi, TxPwrCtrlCmd, hwtxPwrCtrl_en);
        /* set phyreg(PapdEnable$core.gain_dac_rf_override$core)
         * [expr !$phyreg(TxPwrCtrlCmd.hwtxPwrCtrl_en)]
         */
        FOREACH_ACTV_CORE(pi, stf_shdata->hw_phyrxchain, core) {
            MOD_PHYREGCEE(pi, PapdEnable, core, gain_dac_rf_override,
                (txPwrCtrlCmd ? 0 : 1));
        }

        if (WBPAPD_ENAB(pi) && txPwrCtrlCmd) {
            FOREACH_CORE(pi, core) {
                MOD_PHYREGCEE(pi, PapdLutSel0, core, papd_lut_select_ovr, 0);
            }
        }
    }

#ifdef WLC_TXSHAPER
    if (ACMAJORREV_GE130(pi->pubpi->phy_rev)) {
        phy_ac_txss_txshaper_power_control_enable(pi, (ctrl_type != PHY_TPC_HW_OFF));
    }
#endif

    /* Resume MAC */
    wlc_phy_conditional_resume(pi, &suspend);
}

uint32
wlc_phy_txpwr_cap_get_acphy(phy_info_t *pi)
{
    uint8 core, rx_coremask;
    uint32 pwr_idx[] = {0, 0, 0, 0};
    uint32 tmp = 0;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);
    phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;

    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        rx_coremask = stf_shdata->hw_phyrxchain;
    } else {
        rx_coremask = stf_shdata->phyrxchain;
    }

    FOREACH_ACTV_CORE(pi, rx_coremask, core) {
        pwr_idx[core] = pi_ac->tpci->txpwr_cap[core];
    }
    tmp = (pwr_idx[3] << 24) | (pwr_idx[2] << 16) | (pwr_idx[1] << 8) | pwr_idx[0];
    return tmp;
}

void
wlc_phy_txpwr_cap_set_acphy(phy_info_t *pi, int8 *pwr_cap)
{
    uint8 core, rx_coremask;
    phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);
    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        rx_coremask = stf_shdata->hw_phyrxchain;
    } else {
        rx_coremask = stf_shdata->phyrxchain;
    }
    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    FOREACH_ACTV_CORE(pi, rx_coremask, core) {
        pi_ac->tpci->txpwr_cap[core] = pwr_cap[core];
    }
    if (pi_ac->tpci->txpwr_cap[0] == 0) {
        wlc_phy_txpwr_cap_enable(pi, DISABLE);
    } else {
        wlc_phy_txpwr_cap_enable(pi, ENABLE);
    }
}

void  wlc_phy_txpwr_cap_enable(phy_info_t *pi, bool enable)
{
    bool suspend = FALSE;
    uint8 core, rx_coremask;
    phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);
    uint8 stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);

    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        rx_coremask = stf_shdata->hw_phyrxchain;
    } else {
        rx_coremask = stf_shdata->phyrxchain;
    }
    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    ACPHY_DISABLE_STALL(pi);

    /* Suspend MAC if haven't done so */
    wlc_phy_conditional_suspend(pi, &suspend);
    if (enable == DISABLE) {
        FOREACH_ACTV_CORE(pi, rx_coremask, core) {
            pi_ac->tpci->txpwr_cap[core] = 127;
        }
    }
    MOD_PHYREG(pi, TxPwrCtrlCmd, pwrCapPerTxmode_en, enable);
    pi_ac->tpci->txpwr_cap_status = enable;
    FOREACH_ACTV_CORE(pi, rx_coremask, core) {
        MOD_PHYREGCEE(pi, TxPwrCapping_path, core, maxTxPwrCap_path,
            pi_ac->tpci->txpwr_cap[core]);
        MOD_PHYREGCEE(pi, TxPwrCapping_path, core, maxTxPwrCap_MIMO2_path,
            pi_ac->tpci->txpwr_cap[core]);
        MOD_PHYREGCEE(pi, TxPwrCapping1_path, core, maxTxPwrCap_MIMO3_path,
            pi_ac->tpci->txpwr_cap[core]);
        MOD_PHYREGCEE(pi, TxPwrCapping1_path, core, maxTxPwrCap_MIMO4_path,
            pi_ac->tpci->txpwr_cap[core]);
    }
    /* Resume MAC */
    wlc_phy_conditional_resume(pi, &suspend);
    ACPHY_ENABLE_STALL(pi, stall_val);
}

/* This is added because wlc_phy_nvram_attach_acphy calls this function
 * XXX  - If possible, we should have only one function
 */
void phy_ac_tpc_ipa_upd(phy_type_tpc_ctx_t *ctx)
{
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = info->pi;
    pi->ipa2g_on = ((pi->epagain2g == 2) || (pi->extpagain2g == 2));
    pi->ipa5g_on = ((pi->epagain5g == 2) || (pi->extpagain5g == 2));
}

static bool
phy_ac_tpc_hw_ctrl_get(phy_type_tpc_ctx_t *ctx)
{
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = info->pi;
    return pi->txpwrctrl;
}

static void
phy_ac_tpc_set(phy_type_tpc_ctx_t *ctx, ppr_t *reg_pwr)
{
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = info->pi;
    uint8 tx_pwr_ctrl_state = pi->txpwrctrl;
    wlc_phy_txpwrctrl_enable_acphy(pi, PHY_TPC_HW_OFF);
    wlc_phy_txpower_recalc_target(pi, reg_pwr, NULL);
    wlc_phy_txpwrctrl_enable_acphy(pi, tx_pwr_ctrl_state);
    wlc_phy_set_tssisens_lim_acphy(pi, TRUE);
}

static void
phy_ac_tpc_set_flags(phy_type_tpc_ctx_t *ctx, phy_tx_power_t *power)
{
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = info->pi;
    power->rf_cores = PHYCORENUM(pi->pubpi->phy_corenum);
    power->flags |= (WL_TX_POWER_F_MIMO);
    if (pi->txpwrctrl == PHY_TPC_HW_ON)
        power->flags |= (WL_TX_POWER_F_ENABLED | WL_TX_POWER_F_HW);
}

static void
phy_ac_tpc_set_max(phy_type_tpc_ctx_t *ctx, phy_tx_power_t *power)
{
    uint8 core;
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = info->pi;
    /* Store the maximum target power among all rates */
    FOREACH_CORE(pi, core) {
        power->tx_power_max[core] = pi->tx_power_max_per_core[core];
#ifdef WL_SARLIMIT
        power->SARLIMIT[core] = pi->tpci->data->sarlimit[core];
#endif
    }
}

/* set txgain in case txpwrctrl is disabled (fixed power) */
void
wlc_phy_txpwr_fixpower_acphy(phy_info_t *pi)
{
    uint8 core, rx_coremask;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

    BCM_REFERENCE(stf_shdata);

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    PHY_CHANLOG(pi, __FUNCTION__, TS_ENTER, 0);

    BCM_REFERENCE(rx_coremask);
    rx_coremask = stf_shdata->hw_phyrxchain;

    FOREACH_ACTV_CORE(pi, rx_coremask, core) {
        wlc_phy_txpwr_by_index_acphy(pi, (1 << core), pi->u.pi_acphy->txpwrindex[core]);
    }
    PHY_CHANLOG(pi, __FUNCTION__, TS_EXIT, 0);
}

#ifdef PPR_API
/* add 1MSB to represent 5bit-width ppr value, for mcs10 and mcs11 only */
void
wlc_phy_txpwr_ppr_bit_ext_srom13_mcs8to11(phy_info_t *pi, ppr_vht_mcs_rateset_t* vht, uint8 bshift)
{
    const struct srom13_ppr *sr13 = &pi->ppr->u.sr13;
    uint8 i;
    uint8 ppr_bit_ext, msb = 0;

    ppr_bit_ext = 1;

    if (ppr_bit_ext) {
        /* msb: bit 3 for mcs11, bit 2 for mcs10, bit 1 for mcs9, bit 0 for mcs8 */
        for (i = 0; i < MCS_PPREXP_GROUP; i++) {
            msb |= (((sr13->ppmcsexp[i] >> bshift) & 0x1) << i);
        }

        /* this added 1MSB is the 4th bit, so left shift 4 bits
        * then left shift 1 more bit since boardlimit is 0.5dB format
        * bit 0-3 for mcs8-11
        */
        vht->pwr[PPREXP_MCS8]  -= ((msb & 0x1) << 4) << 1;
        vht->pwr[PPREXP_MCS9]  -= ((msb & 0x2) << 3) << 1;
        if (sizeof(*vht) > WL_RATESET_SZ_VHT_MCS) {
            vht->pwr[PPREXP_MCS_P_10] -= ((msb & 0x4) << 2) << 1;
            vht->pwr[PPREXP_MCS_P_11] -= ((msb & 0x8) << 1) << 1;
        }
    }
}

void
wlc_phy_txpwr_ruppr_srom18(phy_info_t *pi, ppr_he_mcs_rateset_t* he,
    uint8 tmp_max_pwr, wl_he_rate_type_t ru_type, bool isgband)
{
    const struct srom18_ruppr *sr18_ruppr = &pi->ppr->ru.sr18_ruppr;
    uint16 rupprwd[RUPPR_GROUP];
    uint8 idx = 0;
    if (ru_type == WL_HE_RT_RU26 || ru_type == WL_HE_RT_RU52 ||
        ru_type == WL_HE_RT_RU106) {
        if (isgband == TRUE) {
            for (idx = 0; idx < RUPPR_GROUP; idx++) {
                rupprwd[idx] = sr18_ruppr->offset_2g_ru106[idx];
            }
        } else {
            for (idx = 0; idx < RUPPR_GROUP; idx++) {
                rupprwd[idx] = sr18_ruppr->offset_5g_ru106[idx];
            }
        }
    } else if (ru_type == WL_HE_RT_RU242) {
        if (isgband == TRUE) {
            for (idx = 0; idx < RUPPR_GROUP; idx++) {
                rupprwd[idx] = sr18_ruppr->offset_2g_ru242[idx];
            }
        } else {
            for (idx = 0; idx < RUPPR_GROUP; idx++) {
                rupprwd[idx] = sr18_ruppr->offset_5g_ru242[idx];
            }
        }
    } else if (ru_type == WL_HE_RT_RU484) {
        for (idx = 0; idx < RUPPR_GROUP; idx++) {
            rupprwd[idx] = sr18_ruppr->offset_5g_ru484[idx];
        }
    } else if (ru_type == WL_HE_RT_RU996) {
        for (idx = 0; idx < RUPPR_GROUP; idx++) {
            rupprwd[idx] = sr18_ruppr->offset_5g_ru996[idx];
        }
    } else {
        PHY_ERROR(("%s: should not call me this file without RU supported!\n",
            __FUNCTION__));
        ASSERT(0);
        return;
    }
    he->pwr[0] = rupprwd[0] & 0xf;
    he->pwr[1] = rupprwd[0] & 0xf;
    he->pwr[2] = rupprwd[0] & 0xf;
    he->pwr[3] = (rupprwd[0] & 0xf0) >> 4;
    he->pwr[4] = (rupprwd[0] & 0xf00) >> 8;
    he->pwr[5] = (rupprwd[0] & 0xf000) >> 12;
    he->pwr[6] = rupprwd[1] & 0x1f;
    he->pwr[7] = (rupprwd[1] & 0x3e0) >> 5;
    he->pwr[8] = (rupprwd[1] & 0x7c00) >> 10;
    he->pwr[9] = rupprwd[2] & 0x1f;
    he->pwr[10] = (rupprwd[2] & 0x3e0) >> 5;
    he->pwr[11] = (rupprwd[2] & 0x7c00) >> 10;
    for (idx = 0; idx < WL_RATESET_SZ_HE_MCS; idx++) {
        he->pwr[idx] = tmp_max_pwr - (he->pwr[idx] << 1);
    }
}

/* for MCS20&40_2G case, 10 rates only */
static void
wlc_phy_txpwr_srom11_convert_mcs_2g(uint32 po, uint8 nibble,
         uint8 tmp_max_pwr, ppr_vht_mcs_rateset_t* vht) {
    uint8 i;
    int8 offset;
    offset = (nibble + 8)%16 - 8;

    for (i = 0; i < WL_RATESET_SZ_VHT_MCS; i++) {
        if ((i == 1)||(i == 2)) {
            vht->pwr[i] = vht->pwr[0];
        } else {
            vht->pwr[i] = tmp_max_pwr - ((po & 0xf)<<1);
            po = po >> 4;
        }
    }
    vht->pwr[1] -= (offset << 1);
    vht->pwr[2] = vht->pwr[1];
}

#ifdef WL11AC

/* for MCS10/11 cases, 2 rates only */
static void
wlc_phy_txpwr_srom13_ext_1024qam_convert_mcs_2g(uint16 po, uint8 pkt_bw,
         uint8 tmp_max_pwr, ppr_vht_mcs_rateset_t* vht) {

    if (!(sizeof(*vht) > WL_RATESET_SZ_VHT_MCS)) {
        PHY_ERROR(("%s: should not call me this file without VHT MCS10/11 supported!\n",
            __FUNCTION__));
        ASSERT(0);
        return;
    }

    if (pkt_bw == WL_PKT_BW_20) {
        vht->pwr[PPREXP_MCS_P_10] = tmp_max_pwr - ((po & 0xf) << 1);
        vht->pwr[PPREXP_MCS_P_11] = tmp_max_pwr - (((po >> 4) & 0xf) << 1);
    } else if (pkt_bw == WL_PKT_BW_40) {
        vht->pwr[PPREXP_MCS_P_10] = tmp_max_pwr - (((po >> 8) & 0xf) << 1);
        vht->pwr[PPREXP_MCS_P_11] = tmp_max_pwr - (((po >> 12) & 0xf) << 1);
    } else {
        PHY_ERROR(("%s: Invalid pkt_bw = %d!\n", __FUNCTION__, pkt_bw));
    }
}
#endif /* WL11AC */

/* for 2G Legacy 40Dup mode, providing the base pwr */
static void
wlc_phy_txpwr_srom11_convert_ofdm_2g_dup40(uint32 po, uint8 nibble,
         uint8 tmp_max_pwr, ppr_ofdm_rateset_t* ofdm) {
    uint8 i;
    int8 offset;
    offset = (nibble + 8)%16 - 8;
    for (i = 0; i < WL_RATESET_SZ_OFDM; i++) {
        if ((i == 1)||(i == 2)||(i == 3)) {
            ofdm->pwr[i] = ofdm->pwr[0];
        } else {
            ofdm->pwr[i] = tmp_max_pwr - ((po & 0xf) <<1);
            po = po >> 4;
        }
    }
    ofdm->pwr[2] -= (offset << 1);
    ofdm->pwr[3] = ofdm->pwr[2];
}

/* for ofdm20in40_2G case, 8 rates only */
static void
wlc_phy_txpwr_srom11_convert_ofdm_offset(ppr_ofdm_rateset_t* po,
                                         uint8 nibble2, ppr_ofdm_rateset_t* ofdm)
{
    uint8 i;
    int8 offsetL, offsetH;
    offsetL = ((nibble2 & 0xf) + 8)%16 - 8;
    offsetH = (((nibble2>>4) & 0xf) + 8)%16 - 8;
    for (i = 0; i < WL_RATESET_SZ_OFDM; i++) {
        if (i < 6)
            ofdm->pwr[i] = po->pwr[i] + (offsetL << 1);
        else
            ofdm->pwr[i] = po->pwr[i] + (offsetH << 1);
    }
}

/* for mcs20in40_2G case, 10 rates only */
static void
wlc_phy_txpwr_srom11_convert_mcs_offset(ppr_vht_mcs_rateset_t* po,
                                        uint8 nibble2, ppr_vht_mcs_rateset_t* vht)
{
    uint8 i;
    int8 offsetL, offsetH;
    offsetL = ((nibble2 & 0xf) + 8)%16 - 8;
    offsetH = (((nibble2>>4) & 0xf) + 8)%16 - 8;
    for (i = 0; i < sizeof(*vht); i++) {
        if (i < 5)
            vht->pwr[i] = po->pwr[i] + (offsetL << 1);
        else
            vht->pwr[i] = po->pwr[i] + (offsetH << 1);
    }
}

#if BAND5G
/* for ofdm20_5G case, 8 rates only */
static void
wlc_phy_txpwr_srom11_convert_ofdm_5g(uint32 po, uint8 nibble,
         uint8 tmp_max_pwr, ppr_ofdm_rateset_t* ofdm) {
    uint8 i;
    int8 offset;
    offset = (nibble + 8)%16 - 8;
    for (i = 0; i < WL_RATESET_SZ_OFDM; i++) {
        if ((i == 1)||(i == 2)||(i == 3)) {
            ofdm->pwr[i] = ofdm->pwr[0];
        } else {
            ofdm->pwr[i] = tmp_max_pwr - ((po & 0xf) <<1);
            po = po >> 4;
        }
    }
    ofdm->pwr[2] -= (offset << 1);
    ofdm->pwr[3] = ofdm->pwr[2];
}

/* for MCS20&40_5G case, 10 rates only */
static void
wlc_phy_txpwr_srom11_convert_mcs_5g(uint32 po, uint8 nibble,
         uint8 tmp_max_pwr, ppr_vht_mcs_rateset_t* vht) {
    uint8 i;
    int8 offset;
    uint32 shift_check = 0;
    offset = (nibble + 8)%16 - 8;
    for (i = 0; i < sizeof(*vht); i++) {
        if ((i == 1)||(i == 2)) {
            vht->pwr[i] = vht->pwr[0];
        } else {
            if (++shift_check > sizeof(po)*2)
                break;
            vht->pwr[i] = tmp_max_pwr - ((po & 0xf)<<1);
            po = po >> 4;
        }
    }
    vht->pwr[1] -= (offset << 1);
    vht->pwr[2] = vht->pwr[1];
}

#ifdef WL11AC
/* for MCS10/11 cases, 2 rates only */
static void
wlc_phy_txpwr_srom13_ext_1024qam_convert_mcs_5g(uint32 po, uint8 pkt_bw,
         uint8 tmp_max_pwr, ppr_vht_mcs_rateset_t* vht) {

    if (!(sizeof(*vht) > 10)) {
        PHY_ERROR(("%s: should not call me this file without VHT MCS10/11 supported!\n",
                __FUNCTION__));
        return;
    }

    if (pkt_bw == WL_PKT_BW_20) {
        vht->pwr[10] = tmp_max_pwr - ((po & 0xf) << 1);
        vht->pwr[11] = tmp_max_pwr - (((po >> 4) & 0xf) << 1);
    } else if (pkt_bw == WL_PKT_BW_40) {
        vht->pwr[10] = tmp_max_pwr - (((po >> 8) & 0xf) << 1);
        vht->pwr[11] = tmp_max_pwr - (((po >> 12) & 0xf) << 1);
    } else if (pkt_bw == WL_PKT_BW_80) {
        vht->pwr[10] = tmp_max_pwr - (((po >> 16) & 0xf) << 1);
        vht->pwr[11] = tmp_max_pwr - (((po >> 20) & 0xf) << 1);
    } else if (pkt_bw == WL_PKT_BW_160) {
        vht->pwr[10] = tmp_max_pwr - (((po >> 24) & 0xf) << 1);
        vht->pwr[11] = tmp_max_pwr - (((po >> 28) & 0xf) << 1);
    } else {
        PHY_ERROR(("%s: Invalid pkt_bw = %d!\n", __FUNCTION__, pkt_bw));
    }
}
#endif /* WL11AC */
#endif /* BAND5G */

static void
wlc_phy_ppr_set_mcs(ppr_t* tx_srom_max_pwr, uint8 bwtype,
          ppr_vht_mcs_rateset_t* pwr_offsets, phy_info_t *pi) {
        int8 tmp_mcs8, tmp_mcs9;
    uint8 phy_core_num = PHYCORENUM(pi->pubpi->phy_corenum);
    if (ACMAJORREV_33(pi->pubpi->phy_rev) &&
        PHY_AS_80P80(pi, pi->radio_chanspec)) {
        phy_core_num = phy_core_num >> 1;
    }
    ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_NONE,
        WL_TX_CHAINS_1, (const ppr_vht_mcs_rateset_t*)pwr_offsets);

    if (phy_core_num > 1) {
        /* for vht_S1x2_CDD */
        ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_CDD,
            WL_TX_CHAINS_2, (const ppr_vht_mcs_rateset_t*)pwr_offsets);
        /* for vht_S2x2_STBC */
        ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_STBC,
            WL_TX_CHAINS_2, (const ppr_vht_mcs_rateset_t*)pwr_offsets);
        /* for vht_S2x2_SDM */
        ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_NONE,
            WL_TX_CHAINS_2, (const ppr_vht_mcs_rateset_t*)pwr_offsets);
#if defined(WL_BEAMFORMING) && !defined(WLTXBF_DISABLED)
        /* for vht_S1x2_TXBF */
        ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_TXBF,
            WL_TX_CHAINS_2, (const ppr_vht_mcs_rateset_t*)pwr_offsets);
        tmp_mcs8 = pwr_offsets->pwr[8];
        tmp_mcs9 = pwr_offsets->pwr[9];
        pwr_offsets->pwr[8] = WL_RATE_DISABLED;
        pwr_offsets->pwr[9] = WL_RATE_DISABLED;
        /* for vht_S2x2_TXBF */
        /* VHT8SS2_TXBF0 and VHT9SS2_TXBF0 are invalid */
        ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_TXBF,
            WL_TX_CHAINS_2, (const ppr_vht_mcs_rateset_t*)pwr_offsets);
        pwr_offsets->pwr[8] = tmp_mcs8;
        pwr_offsets->pwr[9] = tmp_mcs9;
#endif /* WL_BEAMFORMING && !WLTXBF_DISABLED */
        if (phy_core_num > 2) {
            /* for vht_S1x3_CDD */
            ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_CDD,
                WL_TX_CHAINS_3, (const ppr_vht_mcs_rateset_t*)pwr_offsets);
            /* for vht_S2x3_STBC */
            ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_STBC,
                WL_TX_CHAINS_3, (const ppr_vht_mcs_rateset_t*)pwr_offsets);
            /* for vht_S2x3_SDM */
            ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_NONE,
                WL_TX_CHAINS_3, (const ppr_vht_mcs_rateset_t*)pwr_offsets);
            /* for vht_S3x3_SDM */
            ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3, WL_TX_MODE_NONE,
                WL_TX_CHAINS_3, (const ppr_vht_mcs_rateset_t*)pwr_offsets);
#if defined(WL_BEAMFORMING) && !defined(WLTXBF_DISABLED)
            /* for vht_S1x3_TXBF */
            ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_TXBF,
                WL_TX_CHAINS_3, (const ppr_vht_mcs_rateset_t*)pwr_offsets);
            /* for vht_S2x3_TXBF */
            ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_TXBF,
                WL_TX_CHAINS_3, (const ppr_vht_mcs_rateset_t*)pwr_offsets);
            tmp_mcs8 = pwr_offsets->pwr[8];
            tmp_mcs9 = pwr_offsets->pwr[9];
            pwr_offsets->pwr[8] = WL_RATE_DISABLED;
            pwr_offsets->pwr[9] = WL_RATE_DISABLED;
            /* for vht_S3x3_TXBF */
            /* VHT8SS3_TXBF0 and VHT9SS3_TXBF0 are invalid */
            ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3, WL_TX_MODE_TXBF,
                WL_TX_CHAINS_3, (const ppr_vht_mcs_rateset_t*)pwr_offsets);
            pwr_offsets->pwr[8] = tmp_mcs8;
            pwr_offsets->pwr[9] = tmp_mcs9;
#endif /* WL_BEAMFORMING && !WLTXBF_DISABLED */
            if (phy_core_num > 3) {
                /* for vht_S1x4_CDD */
                ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1,
                    WL_TX_MODE_CDD, WL_TX_CHAINS_4,
                    (const ppr_vht_mcs_rateset_t*)pwr_offsets);
                /* for vht_S2x4_STBC */
                ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2,
                    WL_TX_MODE_STBC, WL_TX_CHAINS_4,
                    (const ppr_vht_mcs_rateset_t*)pwr_offsets);
                /* for vht_S2x4_SDM */
                ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2,
                    WL_TX_MODE_NONE, WL_TX_CHAINS_4,
                    (const ppr_vht_mcs_rateset_t*)pwr_offsets);
                /* for vht_S3x4_SDM */
                ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3,
                    WL_TX_MODE_NONE, WL_TX_CHAINS_4,
                    (const ppr_vht_mcs_rateset_t*)pwr_offsets);
                /* for vht_S4x4_SDM */
                ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_4,
                    WL_TX_MODE_NONE, WL_TX_CHAINS_4,
                    (const ppr_vht_mcs_rateset_t*)pwr_offsets);
#if defined(WL_BEAMFORMING) && !defined(WLTXBF_DISABLED)
                /* for vht_S1x4_TXBF */
                ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_vht_mcs_rateset_t*)pwr_offsets);
                /* for vht_S2x4_TXBF */
                ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_vht_mcs_rateset_t*)pwr_offsets);
                /* for vht_S3x4_TXBF */
                ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_vht_mcs_rateset_t*)pwr_offsets);
                /* for vht_S4x4_TXBF */
                ppr_set_vht_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_4,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_vht_mcs_rateset_t*)pwr_offsets);
#endif /* WL_BEAMFORMING && !WLTXBF_DISABLED */
            }
        }
    }
    BCM_REFERENCE(tmp_mcs8);
    BCM_REFERENCE(tmp_mcs9);
}

static void
wlc_phy_ppr_set_he_mcs(ppr_t* tx_srom_max_pwr, uint8 bwtype,
          ppr_he_mcs_rateset_t* pwr_offsets, phy_info_t *pi) {
    int8 tmp_mcs8, tmp_mcs9;
    uint8 phy_core_num = PHYCORENUM(pi->pubpi->phy_corenum);

    /* If the chip is HE incapable, don't set HE power limits */
    if ((pi->u.pi_acphy->phy_caps & PHY_CAP_HE) == 0) {
        return;
    }

    ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_NONE,
        WL_TX_CHAINS_1, (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_SU);
    ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_NONE,
        WL_TX_CHAINS_1, (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_UB);
    ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_NONE,
        WL_TX_CHAINS_1, (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_LUB);

    if (phy_core_num > 1) {
        /* for he_S1x2_CDD */
        ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_CDD,
            WL_TX_CHAINS_2, (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_SU);
        ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_CDD,
            WL_TX_CHAINS_2, (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_UB);
        ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_CDD,
            WL_TX_CHAINS_2, (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_LUB);
        /* for he_S2x2_SDM */
        ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_NONE,
            WL_TX_CHAINS_2, (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_SU);
        ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_NONE,
            WL_TX_CHAINS_2, (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_UB);
        ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_NONE,
            WL_TX_CHAINS_2, (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_LUB);
#if defined(WL_BEAMFORMING) && !defined(WLTXBF_DISABLED)
        /* for he_S1x2_TXBF */
        ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_TXBF,
            WL_TX_CHAINS_2, (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_SU);
        ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_TXBF,
            WL_TX_CHAINS_2, (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_UB);
        ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_TXBF,
            WL_TX_CHAINS_2, (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_LUB);
        tmp_mcs8 = pwr_offsets->pwr[8];
        tmp_mcs9 = pwr_offsets->pwr[9];
        pwr_offsets->pwr[8] = WL_RATE_DISABLED;
        pwr_offsets->pwr[9] = WL_RATE_DISABLED;
        /* for he_S2x2_TXBF */
        /* HE8SS2_TXBF0 and HE9SS2_TXBF0 are invalid */
        ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_TXBF,
            WL_TX_CHAINS_2, (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_SU);
        ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_TXBF,
            WL_TX_CHAINS_2, (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_UB);
        ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_TXBF,
            WL_TX_CHAINS_2, (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_LUB);
        pwr_offsets->pwr[8] = tmp_mcs8;
        pwr_offsets->pwr[9] = tmp_mcs9;
#endif /* WL_BEAMFORMING && !WLTXBF_DISABLED */
        if (phy_core_num > 2) {
            /* for he_S1x3_CDD */
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_CDD,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_SU);
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_CDD,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_UB);
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_CDD,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_LUB);
            /* for he_S2x3_SDM */
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_NONE,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_SU);
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_NONE,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_UB);
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_NONE,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_LUB);
            /* for he_S3x3_SDM */
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3, WL_TX_MODE_NONE,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_SU);
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3, WL_TX_MODE_NONE,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_UB);
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3, WL_TX_MODE_NONE,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_LUB);
#if defined(WL_BEAMFORMING) && !defined(WLTXBF_DISABLED)
            /* for he_S1x3_TXBF */
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_TXBF,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_SU);
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_TXBF,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_UB);
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1, WL_TX_MODE_TXBF,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_LUB);
            /* for he_S2x3_TXBF */
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_TXBF,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_SU);
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_TXBF,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_UB);
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2, WL_TX_MODE_TXBF,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_LUB);
            tmp_mcs8 = pwr_offsets->pwr[8];
            tmp_mcs9 = pwr_offsets->pwr[9];
            pwr_offsets->pwr[8] = WL_RATE_DISABLED;
            pwr_offsets->pwr[9] = WL_RATE_DISABLED;
            /* for he_S3x3_TXBF */
            /* HE8SS3_TXBF0 and HE9SS3_TXBF0 are invalid */
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3, WL_TX_MODE_TXBF,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_SU);
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3, WL_TX_MODE_TXBF,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_UB);
            ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3, WL_TX_MODE_TXBF,
                WL_TX_CHAINS_3, (const ppr_he_mcs_rateset_t*)pwr_offsets,
                WL_HE_RT_LUB);
            pwr_offsets->pwr[8] = tmp_mcs8;
            pwr_offsets->pwr[9] = tmp_mcs9;
#endif /* WL_BEAMFORMING && !WLTXBF_DISABLED */
            if (phy_core_num > 3) {
                /* for he_S1x4_CDD */
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1,
                    WL_TX_MODE_CDD, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_SU);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1,
                    WL_TX_MODE_CDD, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_UB);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1,
                    WL_TX_MODE_CDD, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_LUB);
                /* for he_S2x4_SDM */
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2,
                    WL_TX_MODE_NONE, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_SU);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2,
                    WL_TX_MODE_NONE, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_UB);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2,
                    WL_TX_MODE_NONE, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_LUB);
                /* for he_S3x4_SDM */
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3,
                    WL_TX_MODE_NONE, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_SU);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3,
                    WL_TX_MODE_NONE, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_UB);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3,
                    WL_TX_MODE_NONE, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_LUB);
                /* for he_S4x4_SDM */
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_4,
                    WL_TX_MODE_NONE, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_SU);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_4,
                    WL_TX_MODE_NONE, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_UB);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_4,
                    WL_TX_MODE_NONE, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_LUB);
#if defined(WL_BEAMFORMING) && !defined(WLTXBF_DISABLED)
                /* for he_S1x4_TXBF */
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_SU);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_UB);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_1,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_LUB);
                /* for he_S2x4_TXBF */
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_SU);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_UB);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_2,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_LUB);
                /* for he_S3x4_TXBF */
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_SU);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_UB);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_3,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_LUB);
                /* for he_S4x4_TXBF */
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_4,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_SU);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_4,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_UB);
                ppr_set_he_mcs(tx_srom_max_pwr, bwtype, WL_TX_NSS_4,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (const ppr_he_mcs_rateset_t*)pwr_offsets, WL_HE_RT_LUB);
#endif /* WL_BEAMFORMING && !WLTXBF_DISABLED */
            }
        }
    }
    BCM_REFERENCE(tmp_mcs8);
    BCM_REFERENCE(tmp_mcs9);
}

static void
wlc_phy_ppr_set_he_mcs_ru(ppr_ru_t* tx_srom_max_pwr, uint8 bwtype,
          ppr_he_mcs_rateset_t* pwr_offsets, phy_info_t *pi, wl_he_rate_type_t ru_type) {
    int8 tmp_mcs8, tmp_mcs9;
    uint8 phy_core_num = PHYCORENUM(pi->pubpi->phy_corenum);

    /* If the chip is HE incapable, don't set HE power limits */
    if ((pi->u.pi_acphy->phy_caps & PHY_CAP_HE) == 0) {
        return;
    }

    ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_1, WL_TX_MODE_NONE,
        WL_TX_CHAINS_1, (ppr_he_mcs_rateset_t*)pwr_offsets);

    if (phy_core_num > 1) {
        /* for he_S1x2_CDD */
        ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_1, WL_TX_MODE_CDD,
            WL_TX_CHAINS_2, (ppr_he_mcs_rateset_t*)pwr_offsets);
        /* for he_S2x2_SDM */
        ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_2, WL_TX_MODE_NONE,
            WL_TX_CHAINS_2, (ppr_he_mcs_rateset_t*)pwr_offsets);
#if defined(WL_BEAMFORMING) && !defined(WLTXBF_DISABLED)
        /* for he_S1x2_TXBF */
        ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_1, WL_TX_MODE_TXBF,
            WL_TX_CHAINS_2, (ppr_he_mcs_rateset_t*)pwr_offsets);
        tmp_mcs8 = pwr_offsets->pwr[8];
        tmp_mcs9 = pwr_offsets->pwr[9];
        pwr_offsets->pwr[8] = WL_RATE_DISABLED;
        pwr_offsets->pwr[9] = WL_RATE_DISABLED;
        /* for he_S2x2_TXBF */
        /* HE8SS2_TXBF0 and HE9SS2_TXBF0 are invalid */
        ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_2, WL_TX_MODE_TXBF,
            WL_TX_CHAINS_2, (ppr_he_mcs_rateset_t*)pwr_offsets);
        pwr_offsets->pwr[8] = tmp_mcs8;
        pwr_offsets->pwr[9] = tmp_mcs9;
#endif /* WL_BEAMFORMING && !WLTXBF_DISABLED */
        if (phy_core_num > 2) {
            /* for he_S1x3_CDD */
            ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_1, WL_TX_MODE_CDD,
                WL_TX_CHAINS_3, (ppr_he_mcs_rateset_t*)pwr_offsets);
            /* for he_S2x3_SDM */
            ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_2, WL_TX_MODE_NONE,
                WL_TX_CHAINS_3, (ppr_he_mcs_rateset_t*)pwr_offsets);
            /* for he_S3x3_SDM */
            ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_3, WL_TX_MODE_NONE,
                WL_TX_CHAINS_3, (ppr_he_mcs_rateset_t*)pwr_offsets);
#if defined(WL_BEAMFORMING) && !defined(WLTXBF_DISABLED)
            /* for he_S1x3_TXBF */
            ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_1, WL_TX_MODE_TXBF,
                WL_TX_CHAINS_3, (ppr_he_mcs_rateset_t*)pwr_offsets);
            /* for he_S2x3_TXBF */
            ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_2, WL_TX_MODE_TXBF,
                WL_TX_CHAINS_3, (ppr_he_mcs_rateset_t*)pwr_offsets);
            tmp_mcs8 = pwr_offsets->pwr[8];
            tmp_mcs9 = pwr_offsets->pwr[9];
            pwr_offsets->pwr[8] = WL_RATE_DISABLED;
            pwr_offsets->pwr[9] = WL_RATE_DISABLED;
            /* for he_S3x3_TXBF */
            /* HE8SS3_TXBF0 and HE9SS3_TXBF0 are invalid */
            ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_3, WL_TX_MODE_TXBF,
                WL_TX_CHAINS_3, (ppr_he_mcs_rateset_t*)pwr_offsets);
            pwr_offsets->pwr[8] = tmp_mcs8;
            pwr_offsets->pwr[9] = tmp_mcs9;
#endif /* WL_BEAMFORMING && !WLTXBF_DISABLED */
            if (phy_core_num > 3) {
                /* for he_S1x4_CDD */
                ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_1,
                    WL_TX_MODE_CDD, WL_TX_CHAINS_4,
                    (ppr_he_mcs_rateset_t*)pwr_offsets);
                /* for he_S2x4_SDM */
                ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_2,
                    WL_TX_MODE_NONE, WL_TX_CHAINS_4,
                    (ppr_he_mcs_rateset_t*)pwr_offsets);
                /* for he_S3x4_SDM */
                ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_3,
                    WL_TX_MODE_NONE, WL_TX_CHAINS_4,
                    (ppr_he_mcs_rateset_t*)pwr_offsets);
                /* for he_S4x4_SDM */
                ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_4,
                    WL_TX_MODE_NONE, WL_TX_CHAINS_4,
                    (ppr_he_mcs_rateset_t*)pwr_offsets);
#if defined(WL_BEAMFORMING) && !defined(WLTXBF_DISABLED)
                /* for he_S1x4_TXBF */
                ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_1,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (ppr_he_mcs_rateset_t*)pwr_offsets);
                /* for he_S2x4_TXBF */
                ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_2,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (ppr_he_mcs_rateset_t*)pwr_offsets);
                /* for he_S3x4_TXBF */
                ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_3,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (ppr_he_mcs_rateset_t*)pwr_offsets);
                /* for he_S4x4_TXBF */
                ppr_set_he_ru_mcs(tx_srom_max_pwr, ru_type, WL_TX_NSS_4,
                    WL_TX_MODE_TXBF, WL_TX_CHAINS_4,
                    (ppr_he_mcs_rateset_t*)pwr_offsets);
#endif /* WL_BEAMFORMING && !WLTXBF_DISABLED */
            }
        }
    }
    BCM_REFERENCE(tmp_mcs8);
    BCM_REFERENCE(tmp_mcs9);
}

static uint8
wlc_phy_make_byte(uint16 nibbleH, uint16 nibbleL)
{
    return (uint8) (((nibbleH & 0xf) << 4) | (nibbleL & 0xf));
}

static void
wlc_phy_txpwr_apply_srom11(phy_info_t *pi, uint8 band, chanspec_t chanspec,
                           uint8 tmp_max_pwr, ppr_t *tx_srom_max_pwr)
{
    uint8 nibbles;
    uint8 ppr_bit_ext, msb;
    const struct srom11_ppr *sr11 = &pi->ppr->u.sr11;
    ppr_bit_ext = BF3_PPR_BIT_EXT(pi->u.pi_acphy);

    if (CHSPEC_IS2G(chanspec))
    {
        ppr_ofdm_rateset_t    ofdm20_offset_2g;
        ppr_vht_mcs_rateset_t    mcs20_offset_2g;
        memset(&mcs20_offset_2g, WL_RATE_DISABLED, sizeof(mcs20_offset_2g));

        /* 2G - OFDM_20 */
        wlc_phy_txpwr_srom_convert_ofdm(sr11->ofdm_2g.bw20, tmp_max_pwr, &ofdm20_offset_2g);

        /* 2G - MCS_20 */
        nibbles = (sr11->offset_2g >> 8) & 0xf;   /* 2LSB is needed */
        wlc_phy_txpwr_srom11_convert_mcs_2g(sr11->mcs_2g.bw20, nibbles,
                tmp_max_pwr, &mcs20_offset_2g);
#ifdef NO_PROPRIETARY_VHT_RATES
#else
#ifdef WL11AC
        wlc_phy_txpwr_srom11_ext_1024qam_convert_mcs_2g(sr11->pp1024qam2g, chanspec,
                tmp_max_pwr, &mcs20_offset_2g);
#endif /* WL11AC */
#endif /* NO_PROPRIETARY_VHT_RATES */

        if (ppr_bit_ext) {
            /* msb: bit 1 for mcs9, bit 0 for mcs8
             * sb40and80hr5glpo, nib3 is 2G
             * bit13 and bit12 are 2g-20MHz: mcs9,mcs8
             */
            msb = (sr11->offset_40in80_h[0] >> 12) & 0x3;
            wlc_phy_txpwr_ppr_bit_ext_mcs8and9(&mcs20_offset_2g, msb);
        }

        if (CHSPEC_BW_LE20(chanspec)) {
            ppr_dsss_rateset_t    cck20_offset;

            /* 2G - CCK */
            wlc_phy_txpwr_srom_convert_cck(sr11->cck.bw20, tmp_max_pwr, &cck20_offset,
                0);

            wlc_phy_ppr_set_dsss(tx_srom_max_pwr, WL_TX_BW_20, &cck20_offset, pi);
            wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_20, &ofdm20_offset_2g, pi);
            wlc_phy_ppr_set_mcs(tx_srom_max_pwr,  WL_TX_BW_20, &mcs20_offset_2g, pi);
        } else if (CHSPEC_IS40(chanspec)) {
            ppr_dsss_rateset_t    cck20in40_offset = {{0, }};
            ppr_ofdm_rateset_t    ofdm20in40_offset_2g = {{0, }};
            ppr_ofdm_rateset_t    ofdmdup40_offset_2g = {{0, }};
            ppr_ofdm_rateset_t    ofdm40_offset_2g = {{0, }};
            ppr_vht_mcs_rateset_t    mcs40_offset_2g;
            ppr_vht_mcs_rateset_t    mcs20in40_offset_2g;
            memset(&mcs40_offset_2g, WL_RATE_DISABLED, sizeof(mcs40_offset_2g));
            memset(&mcs20in40_offset_2g, WL_RATE_DISABLED, sizeof(mcs20in40_offset_2g));

            /* 2G - CCK */
            wlc_phy_txpwr_srom_convert_cck(sr11->cck.bw20in40,
                    tmp_max_pwr, &cck20in40_offset, 0);

            /* 2G - MCS_40 */
            nibbles = (sr11->offset_2g >> 12) & 0xf;   /* 3LSB is needed */
            wlc_phy_txpwr_srom11_convert_mcs_2g(sr11->mcs_2g.bw40, nibbles,
                    tmp_max_pwr, &mcs40_offset_2g);

#ifndef NO_PROPRIETARY_VHT_RATES
#ifdef WL11AC
            wlc_phy_txpwr_srom11_ext_1024qam_convert_mcs_2g(sr11->pp1024qam2g, chanspec,
                tmp_max_pwr, &mcs40_offset_2g);
#endif /* WL11AC */
#endif /* NO_PROPRIETARY_VHT_RATES */

            if (ppr_bit_ext) {
                /* msb: bit 1 for mcs9, bit 0 for mcs8
                 * sb40and80hr5glpo, nib3 is 2G
                 * bit15 and bit14 are 2g-40MHz: mcs9,mcs8
                 */
                msb = (sr11->offset_40in80_h[0] >> 14) & 0x3;
                wlc_phy_txpwr_ppr_bit_ext_mcs8and9(&mcs40_offset_2g, msb);
            }
            /* this is used for 2g_ofdm_dup40 mode,
             * remapping mcs40_offset_2g to ofdm40_offset_2g as the basis for dup
             */
            wlc_phy_txpwr_srom11_convert_ofdm_2g_dup40(sr11->mcs_2g.bw40,
                    nibbles, tmp_max_pwr, &ofdm40_offset_2g);

            /* 2G - OFDM_20IN40 */
            nibbles = wlc_phy_make_byte(sr11->offset_20in40_h, sr11->offset_20in40_l);
            wlc_phy_txpwr_srom11_convert_ofdm_offset(&ofdm20_offset_2g, nibbles,
                &ofdm20in40_offset_2g);

            /* 2G - MCS_20IN40 */
            wlc_phy_txpwr_srom11_convert_mcs_offset(&mcs20_offset_2g, nibbles,
                    &mcs20in40_offset_2g);

            /* 2G OFDM_DUP40 */
            nibbles = wlc_phy_make_byte(sr11->offset_dup_h, sr11->offset_dup_l);
            wlc_phy_txpwr_srom11_convert_ofdm_offset(&ofdm40_offset_2g, nibbles,
                    &ofdmdup40_offset_2g);

            wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_40, &ofdmdup40_offset_2g,
                pi);
            wlc_phy_ppr_set_mcs(tx_srom_max_pwr,  WL_TX_BW_40, &mcs40_offset_2g, pi);

            wlc_phy_ppr_set_dsss(tx_srom_max_pwr, WL_TX_BW_20IN40,
                                 &cck20in40_offset, pi);
            wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_20IN40,
                                 &ofdm20in40_offset_2g, pi);
            wlc_phy_ppr_set_mcs(tx_srom_max_pwr,  WL_TX_BW_20IN40,
                                 &mcs20in40_offset_2g, pi);
        }
    }

#if BAND5G
    else if (CHSPEC_ISPHY5G6G(chanspec)) {
        uint8 band5g = band - 1;
        int bitN = (band == 1) ? 4 : ((band == 2) ? 8 : 12);
        ppr_ofdm_rateset_t    ofdm20_offset_5g;
        ppr_vht_mcs_rateset_t    mcs20_offset_5g;
        memset(&mcs20_offset_5g, WL_RATE_DISABLED, sizeof(mcs20_offset_5g));

        /* 5G 11agnac_20IN20 */
        nibbles = sr11->offset_5g[band5g] & 0xf;        /* 0LSB */
        wlc_phy_txpwr_srom11_convert_ofdm_5g(sr11->ofdm_5g.bw20[band5g],
                nibbles, tmp_max_pwr, &ofdm20_offset_5g);
        wlc_phy_txpwr_srom11_convert_mcs_5g(sr11->ofdm_5g.bw20[band5g],
                nibbles, tmp_max_pwr, &mcs20_offset_5g);
#ifdef NO_PROPRIETARY_VHT_RATES
#else
#ifdef WL11AC
        wlc_phy_txpwr_srom11_ext_1024qam_convert_mcs_5g(sr11->pp1024qam5g[band5g],
                chanspec, tmp_max_pwr, &mcs20_offset_5g);
#endif /* WL11AC */
#endif /* NO_PROPRIETARY_VHT_RATES */
        if (ppr_bit_ext) {
            /* msb: bit 1 for mcs9, bit 0 for mcs8
             * sb40and80hr5glpo, nib2 and nib1 is 5G-low
             * sb40and80hr5gmpo, nib2 and nib1 is 5G-mid
             * sb40and80hr5ghpo, nib2 and nib1 is 5G-high
             * bit5 and bit4 are 5g-20MHz: mcs9,mcs8
             */
            msb = (sr11->offset_40in80_h[band5g] >> 4) & 0x3;
            wlc_phy_txpwr_ppr_bit_ext_mcs8and9(&mcs20_offset_5g, msb);
        }

        if (CHSPEC_BW_LE20(chanspec)) {
            wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_20, &ofdm20_offset_5g, pi);
            wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_20, &mcs20_offset_5g, pi);
        } else {
            ppr_ofdm_rateset_t    ofdm40_offset_5g = {{0, }};
            ppr_vht_mcs_rateset_t    mcs40_offset_5g = {{0, }};
            memset(&mcs40_offset_5g, WL_RATE_DISABLED, sizeof(mcs40_offset_5g));

            /* 5G 11nac 40IN40 */
            nibbles = (sr11->offset_5g[band5g] >> 4) & 0xf; /* 1LSB */
            wlc_phy_txpwr_srom11_convert_mcs_5g(sr11->mcs_5g.bw40[band5g],
                    nibbles, tmp_max_pwr, &mcs40_offset_5g);
#ifdef NO_PROPRIETARY_VHT_RATES
#else
#ifdef WL11AC
            wlc_phy_txpwr_srom11_ext_1024qam_convert_mcs_5g(
                    sr11->pp1024qam5g[band5g], chanspec,
                        tmp_max_pwr, &mcs40_offset_5g);
#endif /* WL11AC */
#endif /* NO_PROPRIETARY_VHT_RATES */
            if (ppr_bit_ext) {
                /* msb: bit 1 for mcs9, bit 0 for mcs8
                 * sb40and80hr5glpo, nib2 and nib1 is 5G-low
                 * sb40and80hr5gmpo, nib2 and nib1 is 5G-mid
                 * sb40and80hr5ghpo, nib2 and nib1 is 5G-high
                 * bit7andbit6 are 5g-40MHz: mcs9, mcs8
                 */
                msb = (sr11->offset_40in80_h[band5g] >> 6) & 0x3;
                wlc_phy_txpwr_ppr_bit_ext_mcs8and9(&mcs40_offset_5g, msb);
            }

            /* same for ofdm 5g dup40 in 40MHz and dup80 in 80MHz */
            wlc_phy_txpwr_srom11_convert_ofdm_5g(sr11->mcs_5g.bw40[band5g],
                    nibbles, tmp_max_pwr, &ofdm40_offset_5g);

            if (CHSPEC_IS40(chanspec)) {
                ppr_ofdm_rateset_t    ofdm20in40_offset_5g;
                ppr_ofdm_rateset_t    ofdmdup40_offset_5g;
                ppr_vht_mcs_rateset_t    mcs20in40_offset_5g;
                memset(&mcs20in40_offset_5g, WL_RATE_DISABLED,
                    sizeof(mcs20in40_offset_5g));

                /* 5G 11agnac_20IN40 */
                nibbles = wlc_phy_make_byte(sr11->offset_20in40_h >> bitN,
                                            sr11->offset_20in40_l >> bitN);
                wlc_phy_txpwr_srom11_convert_ofdm_offset(&ofdm20_offset_5g,
                        nibbles, &ofdm20in40_offset_5g);
                wlc_phy_txpwr_srom11_convert_mcs_offset(&mcs20_offset_5g,
                        nibbles, &mcs20in40_offset_5g);

                /* 5G ofdm_DUP40 */
                nibbles = wlc_phy_make_byte(sr11->offset_dup_h >> bitN,
                                            sr11->offset_dup_l >> bitN);
                wlc_phy_txpwr_srom11_convert_ofdm_offset((ppr_ofdm_rateset_t*)
                      &ofdm40_offset_5g, nibbles, &ofdmdup40_offset_5g);

                wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_40,
                                     &ofdmdup40_offset_5g, pi);
                wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_40,
                                     &mcs40_offset_5g, pi);

                wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_20IN40,
                                     &ofdm20in40_offset_5g, pi);
                wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_20IN40,
                                     &mcs20in40_offset_5g, pi);

#ifdef WL11AC
            } else if (CHSPEC_IS80(chanspec)) {
                ppr_ofdm_rateset_t    ofdm20in80_offset_5g = {{0, }};
                ppr_ofdm_rateset_t    ofdm80_offset_5g = {{0, }};
                ppr_ofdm_rateset_t    ofdmdup80_offset_5g = {{0, }};
                ppr_ofdm_rateset_t    ofdmquad80_offset_5g = {{0, }};
                ppr_vht_mcs_rateset_t    mcs80_offset_5g;
                ppr_vht_mcs_rateset_t    mcs20in80_offset_5g;
                ppr_vht_mcs_rateset_t    mcs40in80_offset_5g;
                memset(&mcs80_offset_5g, WL_RATE_DISABLED, sizeof(mcs80_offset_5g));
                memset(&mcs20in80_offset_5g, WL_RATE_DISABLED,
                    sizeof(mcs20in80_offset_5g));
                memset(&mcs40in80_offset_5g, WL_RATE_DISABLED,
                    sizeof(mcs40in80_offset_5g));

                /* 5G 11nac 80IN80 */
                nibbles = (sr11->offset_5g[band5g] >> 8) & 0xf; /* 2LSB */
                wlc_phy_txpwr_srom11_convert_mcs_5g(sr11->mcs_5g.bw80[band5g],
                        nibbles, tmp_max_pwr, &mcs80_offset_5g);
#ifdef NO_PROPRIETARY_VHT_RATES
#else
                wlc_phy_txpwr_srom11_ext_1024qam_convert_mcs_5g(
                        sr11->pp1024qam5g[band5g], chanspec,
                            tmp_max_pwr, &mcs80_offset_5g);
#endif
                wlc_phy_txpwr_srom11_convert_ofdm_5g(sr11->mcs_5g.bw80[band5g],
                        nibbles, tmp_max_pwr, &ofdm80_offset_5g);

                if (ppr_bit_ext) {
                    /* msb: bit 1 for mcs9, bit 0 for mcs8
                     * sb40and80hr5glpo, nib2 and nib1 is 5G-low
                     * sb40and80hr5gmpo, nib2 and nib1 is 5G-mid
                     * sb40and80hr5ghpo, nib2 and nib1 is 5G-high
                     * bit9andbit8 are 5g-80MHz: mcs9,mcs8
                     */
                    msb = (sr11->offset_40in80_h[band5g] >> 8) & 0x3;
                    wlc_phy_txpwr_ppr_bit_ext_mcs8and9(&mcs80_offset_5g, msb);
                }

                /* 5G ofdm_QUAD80, 80in80 */
                nibbles = wlc_phy_make_byte(sr11->offset_dup_h >> bitN,
                                            sr11->offset_dup_l >> bitN);
                wlc_phy_txpwr_srom11_convert_ofdm_offset((ppr_ofdm_rateset_t*)
                        &ofdm80_offset_5g, nibbles, &ofdmquad80_offset_5g);

                /* 5G ofdm_DUP40in80 */
                wlc_phy_txpwr_srom11_convert_ofdm_offset((ppr_ofdm_rateset_t*)
                        &ofdm40_offset_5g, nibbles, &ofdmdup80_offset_5g);

                /* 5G 11agnac_20Ul/20LU/20UU/20LL */
                /* 8 for 20LU/20UL subband  */
                nibbles = wlc_phy_make_byte(sr11->offset_20in80_h[band5g],
                                            sr11->offset_20in80_l[band5g]);
                wlc_phy_txpwr_srom11_convert_ofdm_offset(
                        &ofdm20_offset_5g, nibbles, &ofdm20in80_offset_5g);
                wlc_phy_txpwr_srom11_convert_mcs_offset(
                        &mcs20_offset_5g, nibbles, &mcs20in80_offset_5g);

                if ((CHSPEC_CTL_SB(chanspec) == WL_CHANSPEC_CTL_SB_UU) ||
                    (CHSPEC_CTL_SB(chanspec) == WL_CHANSPEC_CTL_SB_LL)) {
                    /* for 20UU/20LL subband = offset + 20UL/20LU */
                    /* 8 for 20LL/20UU subband  */
                    nibbles = wlc_phy_make_byte(
                            sr11->offset_20in80_h[band5g] >> 2,
                            sr11->offset_20in80_l[band5g] >> 2);
                    wlc_phy_txpwr_srom11_convert_ofdm_offset(
                        &ofdm20in80_offset_5g, nibbles, &ofdm20in80_offset_5g);
                    wlc_phy_txpwr_srom11_convert_mcs_offset(
                        &mcs20in80_offset_5g, nibbles, &mcs20in80_offset_5g);
                }

                /* 5G 11nac_40IN80 */
                nibbles = wlc_phy_make_byte(sr11->offset_40in80_h[band5g],
                                            sr11->offset_40in80_l[band5g]);
                wlc_phy_txpwr_srom11_convert_mcs_offset(&mcs40_offset_5g,
                    nibbles, &mcs40in80_offset_5g);

                /* for 80IN80MHz OFDM or OFDMQUAD80 */
                wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_80,
                                     &ofdmquad80_offset_5g, pi);
                /* for 80IN80MHz HT */
                wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_80,
                                     &mcs80_offset_5g, pi);
                /* for ofdm_20IN80: S1x1, S1x2, S1x3 */
                wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_20IN80,
                                     &ofdm20in80_offset_5g, pi);
                /* for 20IN80MHz HT */
                wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_20IN80,
                                     &mcs20in80_offset_5g, pi);
                /* for 40IN80MHz HT */
                wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_40IN80,
                                     &mcs40in80_offset_5g, pi);

                /* for ofdm_40IN80: S1x1, S1x2, S1x3 */
                wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_40IN80,
                                     &ofdmdup80_offset_5g, pi);
#endif /* WL11AC */

            }
        }
    }
#endif /* BAND5G */
}

#ifdef WL11AC
static void
BCMATTACHFN(wlc_phy_txpwr_srom13_read_ppr)(phy_info_t *pi)
{
    uint8 nibble, nibble01;
    if (!(SROMREV(pi->sh->sromrev) < 13)) {
        /* Read and interpret the power-offset parameters from the SROM for each
         *  band/subband
         */
        ASSERT(pi->sh->sromrev >= 13);

        PHY_INFORM(("Get SROM 13 Power Offset per rate\n"));
        /* --------------2G------------------- */
        /* 2G CCK */
        pi->ppr->u.sr13.cck.bw20 = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_cckbw202gpo);
        pi->ppr->u.sr13.cck.bw20in40 = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_cckbw20ul2gpo);

        pi->ppr->u.sr13.offset_2g = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ofdmlrbw202gpo);
        /* 2G OFDM_20 */
        nibble = pi->ppr->u.sr13.offset_2g & 0xf;
        nibble01 = (nibble<<4)|nibble;
        nibble = (pi->ppr->u.sr13.offset_2g>>4) & 0xf;
        pi->ppr->u.sr13.ofdm_2g.bw20 = (((nibble<<8)|(nibble<<12))|(nibble01)) & 0xffff;
        pi->ppr->u.sr13.ofdm_2g.bw20 |=
            (((uint16)PHY_GETINTVAR(pi, rstr_dot11agofdmhrbw202gpo)) << 16);
        /* 2G MCS_20 */
        pi->ppr->u.sr13.mcs_2g.bw20 = (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw202gpo);
        /* 2G MCS_40 */
        pi->ppr->u.sr13.mcs_2g.bw40 = (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw402gpo);

        pi->ppr->u.sr13.offset_20in40_l =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in40lrpo);
        pi->ppr->u.sr13.offset_20in40_h =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in40hrpo);

        pi->ppr->u.sr13.offset_dup_h =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_dot11agduphrpo);
        pi->ppr->u.sr13.offset_dup_l =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_dot11agduplrpo);

        pi->ppr->u.sr13.pp1024qam2g = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_mcs1024qam2gpo);
        pi->ppr->u.sr13.cck_filter_offset = (uint8)(PHY_GETINTVAR_SLICE(pi,
            rstr_cckbw202gpodelta)& 0xf);
        pi->ppr->u.sr13.cck_filter_offset_ch14 = (uint8)((PHY_GETINTVAR_SLICE(pi,
            rstr_cckbw202gpodelta) >> 4)& 0xf);
#if BAND5G
        /* ---------------5G--------------- */
        /* 5G 11agnac_20IN20 */
        pi->ppr->u.sr13.ofdm_5g.bw20[0] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw205glpo);
        pi->ppr->u.sr13.ofdm_5g.bw20[1] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw205gmpo);
        pi->ppr->u.sr13.ofdm_5g.bw20[2] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw205ghpo);
        pi->ppr->u.sr13.ofdm_5g.bw20[3] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw205gx1po);
        pi->ppr->u.sr13.ofdm_5g.bw20[4] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw205gx2po);

        pi->ppr->u.sr13.offset_5g[0]    = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_mcslr5glpo);
        pi->ppr->u.sr13.offset_5g[1] = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_mcslr5gmpo);
        pi->ppr->u.sr13.offset_5g[2] = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_mcslr5ghpo);
        pi->ppr->u.sr13.offset_5g[3] = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_mcslr5gx1po);
        pi->ppr->u.sr13.offset_5g[4] = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_mcslr5gx2po);

        /* 5G 11nac 40IN40 */
        pi->ppr->u.sr13.mcs_5g.bw40[0] = (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw405glpo);
        pi->ppr->u.sr13.mcs_5g.bw40[1] = (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw405gmpo);
        pi->ppr->u.sr13.mcs_5g.bw40[2] = (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw405ghpo);
        pi->ppr->u.sr13.mcs_5g.bw40[3] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw405gx1po);
        pi->ppr->u.sr13.mcs_5g.bw40[4] =
                (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw405gx2po);

        /* 5G 11nac 80IN80 */
        pi->ppr->u.sr13.mcs_5g.bw80[0] = (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw805glpo);
        pi->ppr->u.sr13.mcs_5g.bw80[1] = (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw805gmpo);
        pi->ppr->u.sr13.mcs_5g.bw80[2] = (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw805ghpo);
        pi->ppr->u.sr13.mcs_5g.bw80[3] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw805gx1po);
        pi->ppr->u.sr13.mcs_5g.bw80[4] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw805gx2po);

        pi->ppr->u.sr13.offset_20in80_l[0] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in80and160lr5glpo);
        pi->ppr->u.sr13.offset_20in80_h[0] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in80and160hr5glpo);
        pi->ppr->u.sr13.offset_20in80_l[1] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in80and160lr5gmpo);
        pi->ppr->u.sr13.offset_20in80_h[1] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in80and160hr5gmpo);
        pi->ppr->u.sr13.offset_20in80_l[2] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in80and160lr5ghpo);
        pi->ppr->u.sr13.offset_20in80_h[2] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in80and160hr5ghpo);
        pi->ppr->u.sr13.offset_20in80_l[3] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in80and160lr5gx1po);
        pi->ppr->u.sr13.offset_20in80_h[3] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in80and160hr5gx1po);
        pi->ppr->u.sr13.offset_20in80_l[4] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in80and160lr5gx2po);
        pi->ppr->u.sr13.offset_20in80_h[4] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in80and160hr5gx2po);

        pi->ppr->u.sr13.offset_40in80_l[0] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb40and80lr5glpo);
        pi->ppr->u.sr13.offset_40in80_h[0] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb40and80hr5glpo);
        pi->ppr->u.sr13.offset_40in80_l[1] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb40and80lr5gmpo);
        pi->ppr->u.sr13.offset_40in80_h[1] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb40and80hr5gmpo);
        pi->ppr->u.sr13.offset_40in80_l[2] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb40and80lr5ghpo);
        pi->ppr->u.sr13.offset_40in80_h[2] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb40and80hr5ghpo);
        pi->ppr->u.sr13.offset_40in80_l[3] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb40and80lr5gx1po);
        pi->ppr->u.sr13.offset_40in80_h[3] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb40and80hr5gx1po);
        pi->ppr->u.sr13.offset_40in80_l[4] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb40and80lr5gx2po);
        pi->ppr->u.sr13.offset_40in80_h[4] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb40and80hr5gx2po);

        /* 5G 11nac 160IN160 */
        pi->ppr->u.sr13.mcs_5g.bw160[0] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw1605glpo);
        pi->ppr->u.sr13.mcs_5g.bw160[1] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw1605gmpo);
        pi->ppr->u.sr13.mcs_5g.bw160[2] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw1605ghpo);
        pi->ppr->u.sr13.mcs_5g.bw160[3] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw1605gx1po);
        pi->ppr->u.sr13.mcs_5g.bw160[4] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw1605gx2po);

        /* extension fields in SROM 13 */
        pi->ppr->u.sr13.pp1024qam5g[0] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcs1024qam5glpo);
        pi->ppr->u.sr13.pp1024qam5g[1] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcs1024qam5gmpo);
        pi->ppr->u.sr13.pp1024qam5g[2] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcs1024qam5ghpo);
        pi->ppr->u.sr13.pp1024qam5g[3] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcs1024qam5gx1po);
        pi->ppr->u.sr13.pp1024qam5g[4] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcs1024qam5gx2po);

        pi->ppr->u.sr13.ppmcsexp[0] = (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcs8poexp);
        pi->ppr->u.sr13.ppmcsexp[1] = (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcs9poexp);
        pi->ppr->u.sr13.ppmcsexp[2] = (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcs10poexp);
        pi->ppr->u.sr13.ppmcsexp[3] = (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcs11poexp);

#endif /* BAND5G */
    }
}

void
wlc_phy_txpwr_apply_srom13_2g_bw2040(phy_info_t *pi, chanspec_t chanspec,
                           uint8 tmp_max_pwr, ppr_t *tx_srom_max_pwr)
{
    uint8 nibbles, cck_filter_offset;
    uint32 ofdm_po;
    const struct srom13_ppr *sr13 = &pi->ppr->u.sr13;
    ppr_ru_t *board_ru_txpwr_limit;
    bool cck_dig_filt_bflg4 =
        ((pi->sh->boardflags4 & BFL4_SROM13_CCK_DIG_FILT) != 0);
    int fc = wf_channel2mhz(CHSPEC_CHANNEL(pi->radio_chanspec),
        CHSPEC_IS2G(pi->radio_chanspec) ? WF_CHAN_FACTOR_2_4_G
        : CHSPEC_IS5G(pi->radio_chanspec) ? WF_CHAN_FACTOR_5_G
        : WF_CHAN_FACTOR_6_G);
    //if ((pi->u.pi_acphy->phy_caps & PHY_CAP_HE) == 0) {
    board_ru_txpwr_limit = pi->tpci->data->board_ru_txpwr_limit;
    //}
    if (fc == 2484) {
        // unit is in 1 dB
        cck_filter_offset = sr13->cck_filter_offset_ch14 * 2;
        if (cck_filter_offset == 0 && ACMAJORREV_47(pi->pubpi->phy_rev)) {
            // backoff 7dB for 43684 if no further charaterization
            cck_filter_offset = 7 * 2;
        }
    } else if (cck_dig_filt_bflg4 == TRUE) {
        // unit is in 0.5 dB
        cck_filter_offset = sr13->cck_filter_offset;
    } else {
        cck_filter_offset = 0;
    }
    if (CHSPEC_IS2G(chanspec)) {
        ppr_ofdm_rateset_t    ofdm20_offset_2g;
        ppr_vht_mcs_rateset_t    mcs20_offset_2g;
        ppr_he_mcs_rateset_t    he_mcs20_offset_2g;

        /* 2G - OFDM_20 */
        ofdm_po = (uint32)(sr13->ofdm_2g.bw20);

        wlc_phy_txpwr_srom_convert_ofdm(ofdm_po, tmp_max_pwr, &ofdm20_offset_2g);

        /* 2G - MCS_20 */
        nibbles = (sr13->offset_2g >> 8) & 0xf;   /* 2LSB is needed */
        wlc_phy_txpwr_srom11_convert_mcs_2g(sr13->mcs_2g.bw20, nibbles,
                tmp_max_pwr, &mcs20_offset_2g);

        wlc_phy_txpwr_srom13_ext_1024qam_convert_mcs_2g(sr13->pp1024qam2g,
                WL_PKT_BW_20, tmp_max_pwr, &mcs20_offset_2g);
        /* No shift needed for 2g bw20 ppr mcsexp */
        wlc_phy_txpwr_ppr_bit_ext_srom13_mcs8to11(pi, &mcs20_offset_2g, 0);

        bcopy(&mcs20_offset_2g, &he_mcs20_offset_2g, sizeof(mcs20_offset_2g));

        if (CHSPEC_IS20(chanspec)) {
            ppr_dsss_rateset_t    cck20_offset;
            ppr_he_mcs_rateset_t    he_ru106_2g;
            wlc_phy_txpwr_ruppr_srom18(pi, &he_ru106_2g, tmp_max_pwr,
                WL_HE_RT_RU106, TRUE);
            /* 2G - CCK */
            wlc_phy_txpwr_srom_convert_cck(sr13->cck.bw20, tmp_max_pwr, &cck20_offset,
                cck_filter_offset);
            wlc_phy_ppr_set_dsss(tx_srom_max_pwr, WL_TX_BW_20, &cck20_offset, pi);
            wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_20, &ofdm20_offset_2g, pi);
            wlc_phy_ppr_set_mcs(tx_srom_max_pwr,  WL_TX_BW_20, &mcs20_offset_2g, pi);
            wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_20, &he_mcs20_offset_2g,
                pi);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20, &he_ru106_2g,
                pi, WL_HE_RT_RU26);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20, &he_ru106_2g,
                pi, WL_HE_RT_RU52);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20, &he_ru106_2g,
                pi, WL_HE_RT_RU106);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20,
                &he_mcs20_offset_2g, pi, WL_HE_RT_RU242);
        } else if (CHSPEC_IS40(chanspec)) {
            ppr_dsss_rateset_t    cck20in40_offset;
            ppr_ofdm_rateset_t    ofdm20in40_offset_2g;
            ppr_ofdm_rateset_t    ofdmdup40_offset_2g;
            ppr_ofdm_rateset_t    ofdm40_offset_2g;
            ppr_vht_mcs_rateset_t    mcs40_offset_2g;
            ppr_vht_mcs_rateset_t    mcs20in40_offset_2g;
            ppr_he_mcs_rateset_t    he_mcs40_offset_2g;
            ppr_he_mcs_rateset_t    he_mcs20in40_offset_2g;
            ppr_he_mcs_rateset_t    he_ru106_2g;
            ppr_he_mcs_rateset_t    he_ru242_2g;
            /* 2G - CCK */
            wlc_phy_txpwr_srom_convert_cck(sr13->cck.bw20in40, tmp_max_pwr,
                    &cck20in40_offset, cck_filter_offset);

            /* 2G - MCS_40 */
            nibbles = (sr13->offset_2g >> 12) & 0xf;   /* 3LSB is needed */
            wlc_phy_txpwr_srom11_convert_mcs_2g(sr13->mcs_2g.bw40, nibbles,
                    tmp_max_pwr, &mcs40_offset_2g);

            wlc_phy_txpwr_srom13_ext_1024qam_convert_mcs_2g(sr13->pp1024qam2g,
                WL_PKT_BW_40, tmp_max_pwr, &mcs40_offset_2g);
            /* shift 8 bits for 2g bw40 ppr mcsexp */
            wlc_phy_txpwr_ppr_bit_ext_srom13_mcs8to11(pi, &mcs40_offset_2g, 8);

            /* this is used for 2g_ofdm_dup40 mode,
             * remapping mcs40_offset_2g to ofdm40_offset_2g as the basis for dup
             */
            wlc_phy_txpwr_srom11_convert_ofdm_2g_dup40(sr13->mcs_2g.bw40,
                    nibbles, tmp_max_pwr, &ofdm40_offset_2g);
            /* 2G - OFDM_20IN40 */
            nibbles = wlc_phy_make_byte(sr13->offset_20in40_h, sr13->offset_20in40_l);
            wlc_phy_txpwr_srom11_convert_ofdm_offset(&ofdm20_offset_2g, nibbles,
                &ofdm20in40_offset_2g);
            /* 2G - MCS_20IN40 */
            wlc_phy_txpwr_srom11_convert_mcs_offset(&mcs20_offset_2g, nibbles,
                    &mcs20in40_offset_2g);
            /* 2G OFDM_DUP40 */
            nibbles = wlc_phy_make_byte(sr13->offset_dup_h, sr13->offset_dup_l);
            wlc_phy_txpwr_srom11_convert_ofdm_offset(&ofdm40_offset_2g, nibbles,
                    &ofdmdup40_offset_2g);

            bcopy(&mcs40_offset_2g, &he_mcs40_offset_2g, sizeof(mcs40_offset_2g));
            bcopy(&mcs20in40_offset_2g, &he_mcs20in40_offset_2g,
                sizeof(mcs20in40_offset_2g));

            wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_40, &ofdmdup40_offset_2g,
                pi);
            wlc_phy_ppr_set_mcs(tx_srom_max_pwr,  WL_TX_BW_40, &mcs40_offset_2g, pi);
            wlc_phy_ppr_set_dsss(tx_srom_max_pwr, WL_TX_BW_20IN40,
                                 &cck20in40_offset, pi);
            wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_20IN40,
                                 &ofdm20in40_offset_2g, pi);
            wlc_phy_ppr_set_mcs(tx_srom_max_pwr,  WL_TX_BW_20IN40,
                                 &mcs20in40_offset_2g, pi);
            wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr,  WL_TX_BW_40,
                &he_mcs40_offset_2g, pi);
            wlc_phy_txpwr_ruppr_srom18(pi, &he_ru106_2g, tmp_max_pwr,
                WL_HE_RT_RU106, TRUE);
            wlc_phy_txpwr_ruppr_srom18(pi, &he_ru242_2g, tmp_max_pwr,
                WL_HE_RT_RU242, TRUE);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40,
                &he_ru106_2g, pi, WL_HE_RT_RU26);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit,  WL_TX_BW_40,
                &he_ru106_2g, pi, WL_HE_RT_RU52);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit,  WL_TX_BW_40,
                &he_ru106_2g, pi, WL_HE_RT_RU106);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit,  WL_TX_BW_40,
                &he_ru242_2g, pi, WL_HE_RT_RU242);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit,  WL_TX_BW_40,
                &he_mcs40_offset_2g, pi, WL_HE_RT_RU484);
            wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr,  WL_TX_BW_20IN40,
                &he_mcs20in40_offset_2g, pi);
            /*
            //no need to set subband RU in DRV, it has to be picked up in ucode
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit,  WL_TX_BW_20IN40,
                &he_ru106_2g, pi, WL_HE_RT_RU26);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit,  WL_TX_BW_20IN40,
                &he_ru106_2g, pi, WL_HE_RT_RU52);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit,  WL_TX_BW_20IN40,
                &he_ru106_2g, pi, WL_HE_RT_RU106);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit,  WL_TX_BW_20IN40,
                &he_mcs20in40_offset_2g, pi, WL_HE_RT_RU242);
            */
        }
    }
}

#if BAND5G
void
wlc_phy_txpwr_apply_srom13_5g_bw40(phy_info_t *pi, uint8 band, uint8 tmp_max_pwr,
        ppr_t *tx_srom_max_pwr, sr13_ppr_5g_rateset_t *rate5g)
{
    uint8 nibbles;
    int bitN = (band == 1) ? 4 : ((band == 2) ? 8 : 12);
    ppr_ofdm_rateset_t      ofdm20in40_offset_5g;
    ppr_ofdm_rateset_t      ofdmdup40_offset_5g;
    ppr_vht_mcs_rateset_t   mcs20in40_offset_5g;
    ppr_he_mcs_rateset_t    he_mcs20in40_offset_5g;
    ppr_he_mcs_rateset_t    he_ru106_5g;
    ppr_he_mcs_rateset_t    he_ru242_5g;

    const struct srom13_ppr *sr13 = &pi->ppr->u.sr13;
    ppr_ru_t *board_ru_txpwr_limit;

    //if ((pi->u.pi_acphy->phy_caps & PHY_CAP_HE) == 0) {
    board_ru_txpwr_limit = pi->tpci->data->board_ru_txpwr_limit;
    //}
    /* 5G 11agnac_20IN40 */
    nibbles = wlc_phy_make_byte(sr13->offset_20in40_h >> bitN, sr13->offset_20in40_l >> bitN);
    wlc_phy_txpwr_srom11_convert_ofdm_offset(&rate5g->ofdm20_offset_5g, nibbles,
        &ofdm20in40_offset_5g);
    wlc_phy_txpwr_srom11_convert_mcs_offset(&rate5g->mcs20_offset_5g, nibbles,
        &mcs20in40_offset_5g);

    /* 5G ofdm_DUP40 */
    nibbles = wlc_phy_make_byte(sr13->offset_dup_h >> bitN, sr13->offset_dup_l >> bitN);
    wlc_phy_txpwr_srom11_convert_ofdm_offset((ppr_ofdm_rateset_t*)&rate5g->ofdm40_offset_5g,
          nibbles, &ofdmdup40_offset_5g);

    bcopy(&mcs20in40_offset_5g, &he_mcs20in40_offset_5g, sizeof(mcs20in40_offset_5g));

    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_40, &ofdmdup40_offset_5g, pi);
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_40, &rate5g->mcs40_offset_5g, pi);

    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_20IN40, &ofdm20in40_offset_5g, pi);
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_20IN40, &mcs20in40_offset_5g, pi);

    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_40, &rate5g->he_mcs40_offset_5g, pi);
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru106_5g, tmp_max_pwr, WL_HE_RT_RU106, FALSE);
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru242_5g, tmp_max_pwr, WL_HE_RT_RU242, FALSE);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40, &he_ru106_5g,
        pi, WL_HE_RT_RU26);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40, &he_ru106_5g,
        pi, WL_HE_RT_RU52);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40, &he_ru106_5g,
        pi, WL_HE_RT_RU106);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40, &he_ru242_5g,
        pi, WL_HE_RT_RU242);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40, &rate5g->he_mcs40_offset_5g,
        pi, WL_HE_RT_RU484);
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_20IN40, &he_mcs20in40_offset_5g, pi);
    //no need to set subband RU in DRV, it has to be picked up in ucode
    /*
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20IN40, &he_ru106_5g,
        pi, WL_HE_RT_RU26);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20IN40, &he_ru106_5g,
        pi, WL_HE_RT_RU52);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20IN40, &he_ru106_5g,
        pi, WL_HE_RT_RU106);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20IN40, &he_mcs20in40_offset_5g,
        pi, WL_HE_RT_RU242);
    */
}

void
wlc_phy_txpwr_apply_srom13_5g_bw80(phy_info_t *pi, uint8 band, chanspec_t chanspec,
        uint8 tmp_max_pwr, ppr_t *tx_srom_max_pwr, sr13_ppr_5g_rateset_t *rate5g)
{
    uint8 nibbles;
    uint8 band5g = band - 1;
    int bitN = (band == 1) ? 4 : ((band == 2) ? 8 : 12);
    ppr_ofdm_rateset_t      ofdm20in80_offset_5g;
    ppr_ofdm_rateset_t      ofdmdup80_offset_5g;
    ppr_ofdm_rateset_t      ofdmquad80_offset_5g;
    ppr_vht_mcs_rateset_t   mcs20in80_offset_5g;
    ppr_vht_mcs_rateset_t   mcs40in80_offset_5g;
    ppr_he_mcs_rateset_t    he_mcs20in80_offset_5g;
    ppr_he_mcs_rateset_t    he_mcs40in80_offset_5g;
    ppr_he_mcs_rateset_t    he_ru106_5g;
    ppr_he_mcs_rateset_t    he_ru242_5g;
    ppr_he_mcs_rateset_t    he_ru484_5g;

    const struct srom13_ppr *sr13 = &pi->ppr->u.sr13;
    ppr_ru_t *board_ru_txpwr_limit;

    //if ((pi->u.pi_acphy->phy_caps & PHY_CAP_HE) == 0) {
    board_ru_txpwr_limit = pi->tpci->data->board_ru_txpwr_limit;
    //}
    /* 5G ofdm_QUAD80, 80in80 */
    nibbles = wlc_phy_make_byte(sr13->offset_dup_h >> bitN, sr13->offset_dup_l >> bitN);
    wlc_phy_txpwr_srom11_convert_ofdm_offset((ppr_ofdm_rateset_t*)&rate5g->ofdm80_offset_5g,
        nibbles, &ofdmquad80_offset_5g);

    /* 5G ofdm_DUP40in80 */
    wlc_phy_txpwr_srom11_convert_ofdm_offset((ppr_ofdm_rateset_t*)&rate5g->ofdm40_offset_5g,
        nibbles, &ofdmdup80_offset_5g);

    /* 5G 11agnac_20Ul/20LU/20UU/20LL, 8 for 20LU/20UL subband */
    nibbles = wlc_phy_make_byte(sr13->offset_20in80_h[band5g], sr13->offset_20in80_l[band5g]);
    wlc_phy_txpwr_srom11_convert_ofdm_offset(&rate5g->ofdm20_offset_5g, nibbles,
        &ofdm20in80_offset_5g);
    wlc_phy_txpwr_srom11_convert_mcs_offset(&rate5g->mcs20_offset_5g, nibbles,
        &mcs20in80_offset_5g);

    if ((CHSPEC_CTL_SB(chanspec) == WL_CHANSPEC_CTL_SB_UU) ||
        (CHSPEC_CTL_SB(chanspec) == WL_CHANSPEC_CTL_SB_LL))
    {
        /* for 20UU/20LL subband = offset + 20UL/20LU, 8 for 20LL/20UU subband */
        nibbles = wlc_phy_make_byte(sr13->offset_20in80_h[band5g] >> 2,
            sr13->offset_20in80_l[band5g] >> 2);
        wlc_phy_txpwr_srom11_convert_ofdm_offset(&ofdm20in80_offset_5g, nibbles,
            &ofdm20in80_offset_5g);
        wlc_phy_txpwr_srom11_convert_mcs_offset(&mcs20in80_offset_5g, nibbles,
            &mcs20in80_offset_5g);
    }

    /* 5G 11nac_40IN80 */
    nibbles = wlc_phy_make_byte(sr13->offset_40in80_h[band5g], sr13->offset_40in80_l[band5g]);
    wlc_phy_txpwr_srom11_convert_mcs_offset(&rate5g->mcs40_offset_5g, nibbles,
        &mcs40in80_offset_5g);

    bcopy(&mcs20in80_offset_5g, &he_mcs20in80_offset_5g, sizeof(mcs20in80_offset_5g));
    bcopy(&mcs40in80_offset_5g, &he_mcs40in80_offset_5g, sizeof(mcs40in80_offset_5g));

    /* for 80IN80MHz OFDM or OFDMQUAD80 */
    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_80, &ofdmquad80_offset_5g, pi);
    /* for 80IN80MHz HT */
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_80,  &rate5g->mcs80_offset_5g, pi);
    /* for ofdm_20IN80: S1x1, S1x2, S1x3 */
    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_20IN80, &ofdm20in80_offset_5g, pi);
    /* for 20IN80MHz HT */
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_20IN80, &mcs20in80_offset_5g, pi);
    /* for 40IN80MHz HT */
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_40IN80, &mcs40in80_offset_5g, pi);
    /* for ofdm_40IN80: S1x1, S1x2, S1x3 */
    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_40IN80, &ofdmdup80_offset_5g, pi);

    /* for 80IN80MHz HE */
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru106_5g, tmp_max_pwr, WL_HE_RT_RU106, FALSE);
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru242_5g, tmp_max_pwr, WL_HE_RT_RU242, FALSE);
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru484_5g, tmp_max_pwr, WL_HE_RT_RU484, FALSE);
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_80,  &rate5g->he_mcs80_offset_5g, pi);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80,  &he_ru106_5g,
        pi, WL_HE_RT_RU26);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80,  &he_ru106_5g,
        pi, WL_HE_RT_RU52);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80,  &he_ru106_5g,
        pi, WL_HE_RT_RU106);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80,  &he_ru242_5g,
        pi, WL_HE_RT_RU242);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80,  &he_ru484_5g,
        pi, WL_HE_RT_RU484);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80,  &rate5g->he_mcs80_offset_5g,
        pi, WL_HE_RT_RU996);
    /* for 20IN80MHz HE */
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_20IN80, &he_mcs20in80_offset_5g, pi);
    //no need to set subband RU in DRV, it has to be picked up in ucode
    /*
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20IN80, &he_ru106_5g,
        pi, WL_HE_RT_RU26);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20IN80, &he_ru106_5g,
        pi, WL_HE_RT_RU52);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20IN80, &he_ru106_5g,
        pi, WL_HE_RT_RU106);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20IN80, &he_mcs20in80_offset_5g,
        pi, WL_HE_RT_RU242);
    */
    /* for 40IN80MHz HE */
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_40IN80, &he_mcs40in80_offset_5g, pi);
    //no need to set subband RU in DRV, it has to be picked up in ucode
    /*
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40IN80, &he_ru106_5g,
        pi, WL_HE_RT_RU26);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40IN80, &he_ru106_5g,
        pi, WL_HE_RT_RU52);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40IN80, &he_ru106_5g,
        pi, WL_HE_RT_RU106);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40IN80, &he_ru242_5g,
        pi, WL_HE_RT_RU242);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40IN80, &he_mcs40in80_offset_5g,
        pi, WL_HE_RT_RU484);
    */
}

void
wlc_phy_txpwr_apply_srom13_5g_bw160(phy_info_t *pi, uint8 band, chanspec_t chanspec,
        uint8 tmp_max_pwr, ppr_t *tx_srom_max_pwr, sr13_ppr_5g_rateset_t *rate5g)
{
    uint8 nibbles;
    uint8 band5g = band - 1;
    int bitN = (band == 1) ? 4 : ((band == 2) ? 8 : 12);
    ppr_ofdm_rateset_t      ofdm160_offset_5g;
    ppr_ofdm_rateset_t      ofdm20in160_offset_5g;
    ppr_ofdm_rateset_t      ofdmdup160_offset_5g;
    ppr_ofdm_rateset_t      ofdmquad160_offset_5g;
    ppr_ofdm_rateset_t      ofdmoct160_offset_5g;
    ppr_vht_mcs_rateset_t   mcs160_offset_5g;
    ppr_vht_mcs_rateset_t   mcs20in160_offset_5g;
    ppr_vht_mcs_rateset_t   mcs40in160_offset_5g;
    ppr_vht_mcs_rateset_t   mcs80in160_offset_5g;
    ppr_he_mcs_rateset_t    he_mcs160_offset_5g;
    ppr_he_mcs_rateset_t    he_mcs20in160_offset_5g;
    ppr_he_mcs_rateset_t    he_mcs40in160_offset_5g;
    ppr_he_mcs_rateset_t    he_mcs80in160_offset_5g;

    ppr_he_mcs_rateset_t    he_ru106_5g;
    ppr_he_mcs_rateset_t    he_ru242_5g;
    ppr_he_mcs_rateset_t    he_ru484_5g;
    ppr_he_mcs_rateset_t    he_ru996_5g;

    const struct srom13_ppr *sr13 = &pi->ppr->u.sr13;
    ppr_ru_t *board_ru_txpwr_limit;

    //if ((pi->u.pi_acphy->phy_caps & PHY_CAP_HE) == 0) {
    board_ru_txpwr_limit = pi->tpci->data->board_ru_txpwr_limit;
    //}
    /* 5G 11nac 160IN160, shift 12 for  bandwidth 160 */
    nibbles = (sr13->offset_5g[band5g] >> 12) & 0xf;
    wlc_phy_txpwr_srom11_convert_ofdm_5g(sr13->mcs_5g.bw160[band5g], nibbles,
        tmp_max_pwr, &ofdm160_offset_5g);
    wlc_phy_txpwr_srom11_convert_mcs_5g(sr13->mcs_5g.bw160[band5g], nibbles,
        tmp_max_pwr, &mcs160_offset_5g);

    wlc_phy_txpwr_srom13_ext_1024qam_convert_mcs_5g(sr13->pp1024qam5g[band5g], WL_PKT_BW_160,
        tmp_max_pwr, &mcs160_offset_5g);
    /* shift band5g +  25 bits for 5g bw160 ppr mcsexp */
    wlc_phy_txpwr_ppr_bit_ext_srom13_mcs8to11(pi, &mcs160_offset_5g, band5g + 25);

    /* 5G ofdm_OCT160, 160in160 */
    nibbles = wlc_phy_make_byte(sr13->offset_dup_h >> bitN, sr13->offset_dup_l >> bitN);
    wlc_phy_txpwr_srom11_convert_ofdm_offset((ppr_ofdm_rateset_t*)&ofdm160_offset_5g,
        nibbles, &ofdmoct160_offset_5g);

    /* 5G ofdm_QUAD80in160 */
    wlc_phy_txpwr_srom11_convert_ofdm_offset((ppr_ofdm_rateset_t*)&rate5g->ofdm80_offset_5g,
        nibbles, &ofdmquad160_offset_5g);

    /* 5G ofdm_DUP40in160 */
    wlc_phy_txpwr_srom11_convert_ofdm_offset((ppr_ofdm_rateset_t*)&rate5g->ofdm40_offset_5g,
        nibbles, &ofdmdup160_offset_5g);

    /* 5G 11agnac_20IN160 */
    nibbles = wlc_phy_make_byte(sr13->offset_20in80_h[band5g] >> 4,
        sr13->offset_20in80_l[band5g] >> 4);
    wlc_phy_txpwr_srom11_convert_ofdm_offset(&rate5g->ofdm20_offset_5g, nibbles,
        &ofdm20in160_offset_5g);
    wlc_phy_txpwr_srom11_convert_mcs_offset(&rate5g->mcs20_offset_5g, nibbles,
        &mcs20in160_offset_5g);
    if ((CHSPEC_CTL_SB(chanspec) == WL_CHANSPEC_CTL_SB_UUU) ||
        (CHSPEC_CTL_SB(chanspec) == WL_CHANSPEC_CTL_SB_LLL)) {
        /* 12 for 20LLL/20UUU subband w.r.t 20in160 sb */
        nibbles = wlc_phy_make_byte(sr13->offset_20in80_h[band5g] >> 12,
            sr13->offset_20in80_l[band5g] >> 12);
        wlc_phy_txpwr_srom11_convert_ofdm_offset(&ofdm20in160_offset_5g, nibbles,
            &ofdm20in160_offset_5g);
        wlc_phy_txpwr_srom11_convert_mcs_offset(&mcs20in160_offset_5g, nibbles,
            &mcs20in160_offset_5g);
    }
    /* 5G 11nac_40IN160 */
    nibbles = wlc_phy_make_byte(sr13->offset_40in80_h[band5g] >> 4,
            sr13->offset_40in80_l[band5g] >> 4);
    wlc_phy_txpwr_srom11_convert_mcs_offset(&rate5g->mcs40_offset_5g, nibbles,
        &mcs40in160_offset_5g);
    if ((CHSPEC_CTL_SB(chanspec) == WL_CHANSPEC_CTL_SB_UU) ||
        (CHSPEC_CTL_SB(chanspec) == WL_CHANSPEC_CTL_SB_LL)) {
        /* 12 for 40LL/40UU subband w.r.t to 40LU/UL */
        nibbles = wlc_phy_make_byte(sr13->offset_40in80_h[band5g] >> 12,
            sr13->offset_40in80_l[band5g] >> 12);
        wlc_phy_txpwr_srom11_convert_mcs_offset(&mcs40in160_offset_5g, nibbles,
            &mcs40in160_offset_5g);
    }

    /* 5G 11nac_80IN160 */
    nibbles = wlc_phy_make_byte(sr13->offset_40in80_h[band5g] >> 8,
        sr13->offset_40in80_l[band5g] >> 8);
    wlc_phy_txpwr_srom11_convert_mcs_offset(&rate5g->mcs80_offset_5g, nibbles,
        &mcs80in160_offset_5g);

    bcopy(&mcs160_offset_5g, &he_mcs160_offset_5g, sizeof(mcs160_offset_5g));
    bcopy(&mcs20in160_offset_5g, &he_mcs20in160_offset_5g, sizeof(mcs20in160_offset_5g));
    bcopy(&mcs40in160_offset_5g, &he_mcs40in160_offset_5g, sizeof(mcs40in160_offset_5g));
    bcopy(&mcs80in160_offset_5g, &he_mcs80in160_offset_5g, sizeof(mcs80in160_offset_5g));

    /* for 160IN160MHz OFDM or OFDMQUAD160 */
    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_160, &ofdmoct160_offset_5g, pi);
    /* for ofdm_20IN160: S1x1, S1x2, S1x3, S1x4 */
    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_20IN160, &ofdm20in160_offset_5g, pi);
    /* for ofdm_40IN160: S1x1, S1x2, S1x3, S1x4 */
    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_40IN160, &ofdmdup160_offset_5g, pi);
    /* for ofdm_80IN160: S1x1, S1x2, S1x3, S1x4 */
    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_80IN160, &ofdmquad160_offset_5g, pi);

    /* for 160IN160MHz HT */
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_160, &mcs160_offset_5g, pi);
    /* for 20IN160MHz HT */
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_20IN160, &mcs20in160_offset_5g, pi);
    /* for 40IN160MHz HT */
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_40IN160, &mcs40in160_offset_5g, pi);
    /* for 80IN160MHz HT */
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_80IN160, &mcs80in160_offset_5g, pi);

    /* for 160IN160MHz HT */
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru106_5g, tmp_max_pwr, WL_HE_RT_RU106, FALSE);
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru242_5g, tmp_max_pwr, WL_HE_RT_RU242, FALSE);
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru484_5g, tmp_max_pwr, WL_HE_RT_RU484, FALSE);
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru996_5g, tmp_max_pwr, WL_HE_RT_RU996, FALSE);
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_160, &he_mcs160_offset_5g, pi);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_160, &he_ru106_5g,
        pi, WL_HE_RT_RU26);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_160, &he_ru106_5g,
        pi, WL_HE_RT_RU52);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_160, &he_ru106_5g,
        pi, WL_HE_RT_RU106);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_160, &he_ru242_5g,
        pi, WL_HE_RT_RU242);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_160, &he_ru484_5g,
        pi, WL_HE_RT_RU484);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_160, &he_ru996_5g,
        pi, WL_HE_RT_RU996);
    /* for 20IN160MHz HT */
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_20IN160, &he_mcs20in160_offset_5g, pi);
    //no need to set subband RU in DRV, it has to be picked up in ucode
    /*
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20IN160, &he_ru106_5g,
        pi, WL_HE_RT_RU26);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20IN160, &he_ru106_5g,
        pi, WL_HE_RT_RU52);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20IN160, &he_ru106_5g,
        pi, WL_HE_RT_RU106);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20IN160, &he_mcs20in160_offset_5g,
        pi, WL_HE_RT_RU242);
    */
    /* for 40IN160MHz HT */
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_40IN160, &he_mcs40in160_offset_5g, pi);
    //no need to set subband RU in DRV, it has to be picked up in ucode
    /*
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40IN160, &he_ru106_5g,
        pi, WL_HE_RT_RU26);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40IN160, &he_ru106_5g,
        pi, WL_HE_RT_RU52);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40IN160, &he_ru106_5g,
        pi, WL_HE_RT_RU106);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40IN160, &he_ru242_5g,
        pi, WL_HE_RT_RU242);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40IN160, &he_mcs40in160_offset_5g,
        pi, WL_HE_RT_RU484);
    */
    /* for 80IN160MHz HT */
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_80IN160, &he_mcs80in160_offset_5g, pi);
    //no need to set subband RU in DRV, it has to be picked up in ucode
    /*
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80IN160, &he_ru106_5g,
        pi, WL_HE_RT_RU26);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80IN160, &he_ru106_5g,
        pi, WL_HE_RT_RU52);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80IN160, &he_ru106_5g,
        pi, WL_HE_RT_RU106);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80IN160, &he_ru242_5g,
        pi, WL_HE_RT_RU242);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80IN160, &he_ru484_5g,
        pi, WL_HE_RT_RU484);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80IN160, &he_mcs80in160_offset_5g,
        pi, WL_HE_RT_RU996);
    */
}
#endif /* BAND5G */

void
wlc_phy_txpwr_apply_srom13(phy_info_t *pi, uint8 band, chanspec_t chanspec,
                           uint8 tmp_max_pwr, ppr_t *tx_srom_max_pwr)
{
    if (CHSPEC_IS2G(chanspec)) {
        wlc_phy_txpwr_apply_srom13_2g_bw2040(pi, chanspec, tmp_max_pwr, tx_srom_max_pwr);
    }
#if BAND5G
    else if (CHSPEC_ISPHY5G6G(chanspec)) {
        uint8 nibbles;
        uint8 band5g = band - 1;
        sr13_ppr_5g_rateset_t rate5g;
        const struct srom13_ppr *sr13 = &pi->ppr->u.sr13;
        ppr_he_mcs_rateset_t    he_ru106_5g;
        ppr_ru_t *board_ru_txpwr_limit = pi->tpci->data->board_ru_txpwr_limit;
        /* 5G 11agnac_20IN20 */
        nibbles = sr13->offset_5g[band5g] & 0xf;                /* 0LSB */
        wlc_phy_txpwr_srom11_convert_ofdm_5g(sr13->ofdm_5g.bw20[band5g],
            nibbles, tmp_max_pwr, &rate5g.ofdm20_offset_5g);
        wlc_phy_txpwr_srom11_convert_mcs_5g(sr13->ofdm_5g.bw20[band5g],
            nibbles, tmp_max_pwr, &rate5g.mcs20_offset_5g);

        wlc_phy_txpwr_srom13_ext_1024qam_convert_mcs_5g(sr13->pp1024qam5g[band5g],
            WL_PKT_BW_20, tmp_max_pwr, &rate5g.mcs20_offset_5g);
        /* shift band5g + 1 for 5g bw20 ppr mcsexp */
        wlc_phy_txpwr_ppr_bit_ext_srom13_mcs8to11(pi, &rate5g.mcs20_offset_5g, band5g + 1);
        bcopy(&rate5g.mcs20_offset_5g, &rate5g.he_mcs20_offset_5g,
            sizeof(rate5g.mcs20_offset_5g));
        wlc_phy_txpwr_ruppr_srom18(pi, &he_ru106_5g, tmp_max_pwr, WL_HE_RT_RU106, FALSE);
        if (CHSPEC_IS20(chanspec)) {
            wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_20,
                &rate5g.ofdm20_offset_5g, pi);
            wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_20,
                &rate5g.mcs20_offset_5g, pi);
            wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_20,
                &rate5g.he_mcs20_offset_5g, pi);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20,
                &he_ru106_5g, pi, WL_HE_RT_RU26);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20,
                &he_ru106_5g, pi, WL_HE_RT_RU52);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20,
                &he_ru106_5g, pi, WL_HE_RT_RU106);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20,
                &rate5g.he_mcs20_offset_5g, pi, WL_HE_RT_RU242);
        } else {
            /* 5G 11nac 40IN40 */
            nibbles = (sr13->offset_5g[band5g] >> 4) & 0xf; /* 1LSB */
            wlc_phy_txpwr_srom11_convert_mcs_5g(sr13->mcs_5g.bw40[band5g],
                nibbles, tmp_max_pwr, &rate5g.mcs40_offset_5g);

            wlc_phy_txpwr_srom13_ext_1024qam_convert_mcs_5g(sr13->pp1024qam5g[band5g],
                WL_PKT_BW_40, tmp_max_pwr, &rate5g.mcs40_offset_5g);
            /* shift band5g + 9 for 5g bw40 ppr mcsexp */
            wlc_phy_txpwr_ppr_bit_ext_srom13_mcs8to11(pi, &rate5g.mcs40_offset_5g,
                band5g + 9);

            bcopy(&rate5g.mcs40_offset_5g, &rate5g.he_mcs40_offset_5g,
                sizeof(rate5g.mcs40_offset_5g));

            /* same for ofdm 5g dup40 in 40MHz and dup80 in 80MHz */
            wlc_phy_txpwr_srom11_convert_ofdm_5g(sr13->mcs_5g.bw40[band5g], nibbles,
                tmp_max_pwr, &rate5g.ofdm40_offset_5g);

            if (CHSPEC_IS40(chanspec)) {
                wlc_phy_txpwr_apply_srom13_5g_bw40(pi, band, tmp_max_pwr,
                    tx_srom_max_pwr, &rate5g);
            } else {
                /* 5G 11nac 80IN80 */
                nibbles = (sr13->offset_5g[band5g] >> 8) & 0xf; /* 2LSB */
                wlc_phy_txpwr_srom11_convert_ofdm_5g(sr13->mcs_5g.bw80[band5g],
                    nibbles, tmp_max_pwr, &rate5g.ofdm80_offset_5g);
                wlc_phy_txpwr_srom11_convert_mcs_5g(sr13->mcs_5g.bw80[band5g],
                    nibbles, tmp_max_pwr, &rate5g.mcs80_offset_5g);
                wlc_phy_txpwr_srom13_ext_1024qam_convert_mcs_5g(
                    sr13->pp1024qam5g[band5g], WL_PKT_BW_80, tmp_max_pwr,
                    &rate5g.mcs80_offset_5g);
                /* shift band5g + 17 for 5g bw80 ppr mcsexp */
                wlc_phy_txpwr_ppr_bit_ext_srom13_mcs8to11(pi,
                    &rate5g.mcs80_offset_5g, band5g + 17);

                bcopy(&rate5g.mcs80_offset_5g, &rate5g.he_mcs80_offset_5g,
                    sizeof(rate5g.mcs80_offset_5g));

                if (CHSPEC_IS80(chanspec)) {
                    wlc_phy_txpwr_apply_srom13_5g_bw80(pi, band, chanspec,
                        tmp_max_pwr, tx_srom_max_pwr, &rate5g);
                } else if (CHSPEC_IS160(chanspec)) {
                    wlc_phy_txpwr_apply_srom13_5g_bw160(pi, band, chanspec,
                        tmp_max_pwr, tx_srom_max_pwr, &rate5g);
                }
            }
        }
    }
#endif /* BAND5G */
}

#if BAND6G
static void
phy_ac_tpc_get_5gext_paparams_srom18(phy_info_t *pi, uint8 chan_freq_range,
        acphy_paparams_t *params)
{
    uint8  core;

    FOREACH_CORE(pi, core) {
        phy_ac_tpc_get_5gext_paparams_percore_srom18(pi,
                chan_freq_range, params, core);
    }
}

static void
phy_ac_tpc_get_5gext_paparams_percore_srom18(phy_info_t *pi, uint8 chan_freq_range,
        acphy_paparams_t *params, uint8 core)
{
    srom18_5gext_pwrdet_t *pwrdet_5gext = pi->pwrdet_ac_5gext;
    if (chan_freq_range < WL_CHAN_FREQ_RANGE_5G_BAND5) {
        /* when chan_freq_range is not used new defined srom, just exit */
        return;
    }
    switch (chan_freq_range) {
        case WL_CHAN_FREQ_RANGE_5G_BAND5:
        case WL_CHAN_FREQ_RANGE_5G_BAND6:
            params[core].a = pwrdet_5gext->
                pwrdet_a[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND5];
            params[core].b = pwrdet_5gext->
                pwrdet_b[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND5];
            params[core].c = pwrdet_5gext->
                pwrdet_c[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND5];
            params[core].d = pwrdet_5gext->
                pwrdet_d[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND5];
            break;
        case WL_CHAN_FREQ_RANGE_5G_BAND5_40:
        case WL_CHAN_FREQ_RANGE_5G_BAND6_40:
            /* Adjust index for 40M */
            params[core].a = pwrdet_5gext->
                pwrdet_a_40[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND5_40];
            params[core].b = pwrdet_5gext->
                pwrdet_b_40[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND5_40];
            params[core].c = pwrdet_5gext->
                pwrdet_c_40[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND5_40];
            params[core].d = pwrdet_5gext->
                pwrdet_d_40[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND5_40];
            break;
        case WL_CHAN_FREQ_RANGE_5G_BAND5_80:
        case WL_CHAN_FREQ_RANGE_5G_BAND6_80:
            /* Adjust index for 80M */
            params[core].a = pwrdet_5gext->
                pwrdet_a_80[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND5_80];
            params[core].b = pwrdet_5gext->
                pwrdet_b_80[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND5_80];
            params[core].c = pwrdet_5gext->
                pwrdet_c_80[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND5_80];
            params[core].d = pwrdet_5gext->
                pwrdet_d_80[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND5_80];
            break;
        case WL_CHAN_FREQ_RANGE_5G_BAND4_160:
        case WL_CHAN_FREQ_RANGE_5G_BAND5_160:
        case WL_CHAN_FREQ_RANGE_5G_BAND6_160:
            /* Adjust index for 160M */
            params[core].a = pwrdet_5gext->
                pwrdet_a_160[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND4_160];
            params[core].b = pwrdet_5gext->
                pwrdet_b_160[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND4_160];
            params[core].c = pwrdet_5gext->
                pwrdet_c_160[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND4_160];
            params[core].d = pwrdet_5gext->
                pwrdet_d_160[core][chan_freq_range -
                    WL_CHAN_FREQ_RANGE_5G_BAND4_160];
            break;
        default:
            PHY_ERROR(("wl%d: %s: pwrdet_5gext core%d: a=%d b=%d c=%d d=%d\n",
                    pi->sh->unit, __FUNCTION__, core,
                    params[core].a, params[core].b,
                    params[core].c, params[core].d));
            break;
    }
}

#if BAND6G
/* Multi-mode pa parameter assignment for 5G ext (6G) */
static void
phy_ac_tpc_get_5gext_paparams_percore_multimode_srom18(phy_info_t *pi, uint8 chan_freq_range,
        acphy_paparams_t params[][PHY_CORE_MAX], uint8 core)
{
    /* Supports 4 tables each in a different 8 bit sections of the EstPwrLut
    **
    ** Params is 2D array, first dimension is the EstPwrLut table (for each BW/CCK),
    ** second dimension is core.
    ** Input chan_freq_range is defined in phy_ac_chanmgr_chan2freq_range_srom12
    */

    int offset = 0;
    srom18_5gext_pwrdet_t *pwrdet_5gext = pi->pwrdet_ac_5gext;
    srom12_pwrdet_t *pwrdet = pi->pwrdet_ac;

    if (chan_freq_range < WL_CHAN_FREQ_RANGE_5G_BAND5) {
        /* when chan_freq_range is not used new defined srom, just exit */
        return;
    }

    switch (chan_freq_range) {
        case WL_CHAN_FREQ_RANGE_5G_BAND5:
        case WL_CHAN_FREQ_RANGE_5G_BAND6:
            offset = WL_CHAN_FREQ_RANGE_5G_BAND5;
            break;

        case WL_CHAN_FREQ_RANGE_5G_BAND5_40:
        case WL_CHAN_FREQ_RANGE_5G_BAND6_40:
            /* Adjust index for 40M */
            offset = WL_CHAN_FREQ_RANGE_5G_BAND5_40;
            break;

        case WL_CHAN_FREQ_RANGE_5G_BAND5_80:
        case WL_CHAN_FREQ_RANGE_5G_BAND6_80:
            /* Adjust index for 80M */
            offset = WL_CHAN_FREQ_RANGE_5G_BAND5_80;
            break;

        case WL_CHAN_FREQ_RANGE_5G_BAND4_160:
        case WL_CHAN_FREQ_RANGE_5G_BAND5_160:
        case WL_CHAN_FREQ_RANGE_5G_BAND6_160:
            /* Adjust index for 160M */
            offset = WL_CHAN_FREQ_RANGE_5G_BAND4_160;
            break;

        default:
            PHY_ERROR(("wl%d: %s: pwrdet core%d, unsupported chan_freq_range:%d \n",
                    pi->sh->unit, __FUNCTION__, core, chan_freq_range));
            break;
    }

    ASSERT(chan_freq_range >= offset);

    /* SROM18 has the following complication with "pa5g" and "pa5gext" PA params used for 6GHz:
     * 6GHz has 7 subbands. For BW20/40/80, first 5 subbands are located in "pa5g" (pwrdet) and
     * the last 2 subbands in "pa5gext" (pwrdet_5gext).
     * However for BW160 it is different - first 4 subbands are located in "pa5g" (pwrdet) and
     * the last 3 subbands in "pa5gext" (pwrdet_5gext).
     */
    if (chan_freq_range == WL_CHAN_FREQ_RANGE_5G_BAND4_160) {
        /* For 160MHz subband4, lower multimode bandwidths need to be loaded from pa5g 4th
         * entry (pwrdet), not from pa5gext (pwrdet_5gext). For subband4, only 160MHz comes
         * from pa5gext (loaded below).
         */
        params[BW0][core].a = pwrdet->pwrdet_a[core][3];
        params[BW0][core].b = pwrdet->pwrdet_b[core][3];
        params[BW0][core].c = pwrdet->pwrdet_c[core][3];
        params[BW0][core].d = pwrdet->pwrdet_d[core][3];

        params[BW1][core].a = pwrdet->pwrdet_a_40[core][3];
        params[BW1][core].b = pwrdet->pwrdet_b_40[core][3];
        params[BW1][core].c = pwrdet->pwrdet_c_40[core][3];
        params[BW1][core].d = pwrdet->pwrdet_d_40[core][3];

        params[BW2][core].a = pwrdet->pwrdet_a_80[core][3];
        params[BW2][core].b = pwrdet->pwrdet_b_80[core][3];
        params[BW2][core].c = pwrdet->pwrdet_c_80[core][3];
        params[BW2][core].d = pwrdet->pwrdet_d_80[core][3];
    } else if ((chan_freq_range == WL_CHAN_FREQ_RANGE_5G_BAND5_160) ||
            (chan_freq_range == WL_CHAN_FREQ_RANGE_5G_BAND6_160)) {
        /* For 160MHz subband5 and higher, load lower multimode bandwidths from the same
         * pa5gext (pwrdet_5gext), but with the offset -1 due to 160MHz switching to
         * pa5gext one subband earlier than lower bandwidths.
         */
        params[BW0][core].a = pwrdet_5gext->pwrdet_a[core][chan_freq_range - offset - 1];
        params[BW0][core].b = pwrdet_5gext->pwrdet_b[core][chan_freq_range - offset - 1];
        params[BW0][core].c = pwrdet_5gext->pwrdet_c[core][chan_freq_range - offset - 1];
        params[BW0][core].d = pwrdet_5gext->pwrdet_d[core][chan_freq_range - offset - 1];

        params[BW1][core].a =
            pwrdet_5gext->pwrdet_a_40[core][chan_freq_range - offset - 1];
        params[BW1][core].b =
            pwrdet_5gext->pwrdet_b_40[core][chan_freq_range - offset - 1];
        params[BW1][core].c =
            pwrdet_5gext->pwrdet_c_40[core][chan_freq_range - offset - 1];
        params[BW1][core].d =
            pwrdet_5gext->pwrdet_d_40[core][chan_freq_range - offset - 1];

        params[BW2][core].a =
            pwrdet_5gext->pwrdet_a_80[core][chan_freq_range - offset - 1];
        params[BW2][core].b =
            pwrdet_5gext->pwrdet_b_80[core][chan_freq_range - offset - 1];
        params[BW2][core].c =
            pwrdet_5gext->pwrdet_c_80[core][chan_freq_range - offset - 1];
        params[BW2][core].d =
            pwrdet_5gext->pwrdet_d_80[core][chan_freq_range - offset - 1];
    } else {
        /* For all other BW load normally from pa5gext (pwrdet_5gext) */
        params[BW0][core].a = pwrdet_5gext->pwrdet_a[core][chan_freq_range - offset];
        params[BW0][core].b = pwrdet_5gext->pwrdet_b[core][chan_freq_range - offset];
        params[BW0][core].c = pwrdet_5gext->pwrdet_c[core][chan_freq_range - offset];
        params[BW0][core].d = pwrdet_5gext->pwrdet_d[core][chan_freq_range - offset];

        params[BW1][core].a = pwrdet_5gext->pwrdet_a_40[core][chan_freq_range - offset];
        params[BW1][core].b = pwrdet_5gext->pwrdet_b_40[core][chan_freq_range - offset];
        params[BW1][core].c = pwrdet_5gext->pwrdet_c_40[core][chan_freq_range - offset];
        params[BW1][core].d = pwrdet_5gext->pwrdet_d_40[core][chan_freq_range - offset];

        params[BW2][core].a = pwrdet_5gext->pwrdet_a_80[core][chan_freq_range - offset];
        params[BW2][core].b = pwrdet_5gext->pwrdet_b_80[core][chan_freq_range - offset];
        params[BW2][core].c = pwrdet_5gext->pwrdet_c_80[core][chan_freq_range - offset];
        params[BW2][core].d = pwrdet_5gext->pwrdet_d_80[core][chan_freq_range - offset];
    }

    params[BW3][core].a = pwrdet_5gext->pwrdet_a_160[core][chan_freq_range - offset];
    params[BW3][core].b = pwrdet_5gext->pwrdet_b_160[core][chan_freq_range - offset];
    params[BW3][core].c = pwrdet_5gext->pwrdet_c_160[core][chan_freq_range - offset];
    params[BW3][core].d = pwrdet_5gext->pwrdet_d_160[core][chan_freq_range - offset];
}
#endif /* BAND6G */

/* Get the content for the ESTPWRSHFTLUTS_TBL table */
static void
phy_ac_fill_estpwrshft_table_5gext(phy_info_t *pi, srom18_5gext_pwrdet_t *pwrdet_5gext,
    uint8 chan_freq, uint8 shfttbl_len, uint32 *shfttblval)
{
    uint8  core, idx1;
    uint8  poffs;
    uint32 pdoffs;

    PHY_TXPWR(("wl%d: %s: ESTPWRSHFTLUT table for band %d\n",
               pi->sh->unit, __FUNCTION__, chan_freq));

    for (idx1 = 0; idx1 < shfttbl_len; idx1++) {
        pdoffs = 0;
        FOREACH_CORE(pi, core) {
            switch (idx1) {
                case 0:
                // Ofdm20LL
                poffs = (uint8)(pwrdet_5gext->pdoffset20in80[core][chan_freq]);
                break;
                case 1:
                // Ofdm40L
                poffs = (uint8)(pwrdet_5gext->pdoffset40in80[core][chan_freq]);
                break;
                case 2:
                // Ofdm20L
                poffs = (uint8)(pwrdet_5gext->pdoffset20in40[core][chan_freq]);
                break;
                case 3:
                // Ofdm20in20
                poffs = 0;
                break;
                case 4:
                // Ofdm20U
                poffs = (uint8)(pwrdet_5gext->pdoffset20in40[core][chan_freq]);
                break;
                case 5:
                // Ofdm40in40
                poffs = 0;
                break;
                case 6:
                // Ofdm40U
                poffs = (uint8)(pwrdet_5gext->pdoffset40in80[core][chan_freq]);
                break;
                case 7:
                // Ofdm20UU
                poffs = (uint8)(pwrdet_5gext->pdoffset20in80[core][chan_freq]);
                break;
                case 8:
                // Ofdm20LU
                poffs = (uint8)(pwrdet_5gext->pdoffset20in80[core][chan_freq]);
                break;
                case 9:
                // Ofdm20UL
                poffs = (uint8)(pwrdet_5gext->pdoffset20in80[core][chan_freq]);
                break;
                case 10:
                // Ofdm40in40
                poffs = 0;
                break;
                case 16:
                // bphy20L
                poffs = 0;
                break;
                case 17:
                // bphy20in20
                poffs = 0;
                break;
                case 18:
                // bphy20U
                poffs = 0;
                break;
                case 23:
                // Ofdm20LLL
                poffs = (uint8)(pwrdet_5gext->pdoffset20in160[core][chan_freq]);
                break;
                case 24:
                // Ofdm40LL
                poffs = (uint8)(pwrdet_5gext->pdoffset40in160[core][chan_freq]);
                break;
                case 25:
                // Ofdm40LU
                poffs = (uint8)(pwrdet_5gext->pdoffset40in160[core][chan_freq]);
                break;
                case 26:
                // Ofdm20LUU
                poffs = (uint8)(pwrdet_5gext->pdoffset20in160[core][chan_freq]);
                break;
                case 27:
                // Ofdm20LLU
                poffs = (uint8)(pwrdet_5gext->pdoffset20in160[core][chan_freq]);
                break;
                case 28:
                // Ofdm20LUL
                poffs = (uint8)(pwrdet_5gext->pdoffset20in160[core][chan_freq]);
                break;
                case 29:
                // Ofdm80L
                poffs = (uint8)(pwrdet_5gext->pdoffset80in160[core][chan_freq]);
                break;
                case 30:
                // Ofdm20ULL
                poffs = (uint8)(pwrdet_5gext->pdoffset20in160[core][chan_freq]);
                break;
                case 31:
                // Ofdm40UL
                poffs = (uint8)(pwrdet_5gext->pdoffset40in160[core][chan_freq]);
                break;
                case 32:
                // Ofdm40UU
                poffs = (uint8)(pwrdet_5gext->pdoffset40in160[core][chan_freq]);
                break;
                case 33:
                // Ofdm20UUU
                poffs = (uint8)(pwrdet_5gext->pdoffset20in160[core][chan_freq]);
                break;
                case 34:
                // Ofdm20ULU
                poffs = (uint8)(pwrdet_5gext->pdoffset20in160[core][chan_freq]);
                break;
                case 35:
                // Ofdm20UUL
                poffs = (uint8)(pwrdet_5gext->pdoffset20in160[core][chan_freq]);
                break;
                case 36:
                // Ofdm80U
                poffs = (uint8)(pwrdet_5gext->pdoffset80in160[core][chan_freq]);
                break;
                default:
                poffs = 0;
                break;
            }
            poffs = (poffs > 15) ? (0xe0 | poffs) : poffs;
            pdoffs = pdoffs | (poffs << core*8);
        }
        PHY_TXPWR(("idx = %d, pdoffs = 0x%08x\n", idx1, pdoffs));
        shfttblval[idx1] = pdoffs;
    }
}

void
wlc_phy_txpwr_apply_srom18_5gext_bw40(phy_info_t *pi, uint8 band_5gext, uint8 tmp_max_pwr,
        ppr_t *tx_srom_max_pwr, sr13_ppr_5g_rateset_t *rate5g)
{
    uint8 nibbles;
    //int bitN = (band == 1) ? 4 : ((band == 2) ? 8 : 12);
    ppr_ofdm_rateset_t      ofdm20in40_offset_5g;
    ppr_ofdm_rateset_t      ofdmdup40_offset_5g;
    ppr_vht_mcs_rateset_t   mcs20in40_offset_5g;
    ppr_he_mcs_rateset_t    he_mcs20in40_offset_5g;
    ppr_he_mcs_rateset_t    he_ru106_5g;
    ppr_he_mcs_rateset_t    he_ru242_5g;

    //const struct srom18_5gextppr *sr18_5gext = &pi->ppr->uext.sr18_5gext;
    ppr_ru_t *board_ru_txpwr_limit;

    board_ru_txpwr_limit = pi->tpci->data->board_ru_txpwr_limit;
    /* 5G 11agnac_20IN40 */
    //nibbles = wlc_phy_make_byte(sr18_5gext->offset_20in40_h >> bitN,
    //sr18_5gext->offset_20in40_l >> bitN);
    nibbles = 0;
    wlc_phy_txpwr_srom11_convert_ofdm_offset(&rate5g->ofdm20_offset_5g, nibbles,
        &ofdm20in40_offset_5g);
    wlc_phy_txpwr_srom11_convert_mcs_offset(&rate5g->mcs20_offset_5g, nibbles,
        &mcs20in40_offset_5g);

    /* 5G ofdm_DUP40 */
    //nibbles = wlc_phy_make_byte(sr18_5gext->offset_dup_h >> bitN,
    //sr18_5gext->offset_dup_l >> bitN);
    nibbles = 0;
    wlc_phy_txpwr_srom11_convert_ofdm_offset((ppr_ofdm_rateset_t*)&rate5g->ofdm40_offset_5g,
          nibbles, &ofdmdup40_offset_5g);

    bcopy(&mcs20in40_offset_5g, &he_mcs20in40_offset_5g, sizeof(mcs20in40_offset_5g));

    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_40, &ofdmdup40_offset_5g, pi);
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_40, &rate5g->mcs40_offset_5g, pi);

    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_20IN40, &ofdm20in40_offset_5g, pi);
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_20IN40, &mcs20in40_offset_5g, pi);

    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_40, &rate5g->he_mcs40_offset_5g, pi);
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru106_5g, tmp_max_pwr, WL_HE_RT_RU106, FALSE);
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru242_5g, tmp_max_pwr, WL_HE_RT_RU242, FALSE);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40, &he_ru106_5g,
        pi, WL_HE_RT_RU26);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40, &he_ru106_5g,
        pi, WL_HE_RT_RU52);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40, &he_ru106_5g,
        pi, WL_HE_RT_RU106);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40, &he_ru242_5g,
        pi, WL_HE_RT_RU242);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_40, &rate5g->he_mcs40_offset_5g,
        pi, WL_HE_RT_RU484);
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_20IN40, &he_mcs20in40_offset_5g, pi);
}

void
wlc_phy_txpwr_apply_srom18_5gext_bw80(phy_info_t *pi, uint8 band_5gext, chanspec_t chanspec,
        uint8 tmp_max_pwr, ppr_t *tx_srom_max_pwr, sr13_ppr_5g_rateset_t *rate5g)
{
    uint8 nibbles;
    //int bitN = (band == 1) ? 4 : ((band == 2) ? 8 : 12);
    ppr_ofdm_rateset_t      ofdm20in80_offset_5g;
    ppr_ofdm_rateset_t      ofdmdup80_offset_5g;
    ppr_ofdm_rateset_t      ofdmquad80_offset_5g;
    ppr_vht_mcs_rateset_t   mcs20in80_offset_5g;
    ppr_vht_mcs_rateset_t   mcs40in80_offset_5g;
    ppr_he_mcs_rateset_t    he_mcs20in80_offset_5g;
    ppr_he_mcs_rateset_t    he_mcs40in80_offset_5g;
    ppr_he_mcs_rateset_t    he_ru106_5g;
    ppr_he_mcs_rateset_t    he_ru242_5g;
    ppr_he_mcs_rateset_t    he_ru484_5g;

    const struct srom18_5gextppr *sr18_5gext = &pi->ppr->uext.sr18_5gext;
    ppr_ru_t *board_ru_txpwr_limit;

    //if ((pi->u.pi_acphy->phy_caps & PHY_CAP_HE) == 0) {
    board_ru_txpwr_limit = pi->tpci->data->board_ru_txpwr_limit;
    //}
    /* 5G ofdm_QUAD80, 80in80 */
    //nibbles = wlc_phy_make_byte(sr18_5gext->offset_dup_h >> bitN,
    //sr18_5gext->offset_dup_l >> bitN);
    nibbles = 0;
    wlc_phy_txpwr_srom11_convert_ofdm_offset((ppr_ofdm_rateset_t*)&rate5g->ofdm80_offset_5g,
        nibbles, &ofdmquad80_offset_5g);

    /* 5G ofdm_DUP40in80 */
    wlc_phy_txpwr_srom11_convert_ofdm_offset((ppr_ofdm_rateset_t*)&rate5g->ofdm40_offset_5g,
        nibbles, &ofdmdup80_offset_5g);

    /* 5G 11agnac_20Ul/20LU/20UU/20LL, 8 for 20LU/20UL subband */
    nibbles = wlc_phy_make_byte(sr18_5gext->offset_20in80_h[band_5gext],
        sr18_5gext->offset_20in80_l[band_5gext]);
    wlc_phy_txpwr_srom11_convert_ofdm_offset(&rate5g->ofdm20_offset_5g, nibbles,
        &ofdm20in80_offset_5g);
    wlc_phy_txpwr_srom11_convert_mcs_offset(&rate5g->mcs20_offset_5g, nibbles,
        &mcs20in80_offset_5g);

    if ((CHSPEC_CTL_SB(chanspec) == WL_CHANSPEC_CTL_SB_UU) ||
        (CHSPEC_CTL_SB(chanspec) == WL_CHANSPEC_CTL_SB_LL))
    {
        /* for 20UU/20LL subband = offset + 20UL/20LU, 8 for 20LL/20UU subband */
        nibbles = wlc_phy_make_byte(sr18_5gext->offset_20in80_h[band_5gext] >> 2,
            sr18_5gext->offset_20in80_l[band_5gext] >> 2);
        wlc_phy_txpwr_srom11_convert_ofdm_offset(&ofdm20in80_offset_5g, nibbles,
            &ofdm20in80_offset_5g);
        wlc_phy_txpwr_srom11_convert_mcs_offset(&mcs20in80_offset_5g, nibbles,
            &mcs20in80_offset_5g);
    }

    /* 5G 11nac_40IN80 */
    nibbles = wlc_phy_make_byte(sr18_5gext->offset_40in80_h[band_5gext],
        sr18_5gext->offset_40in80_l[band_5gext]);
    wlc_phy_txpwr_srom11_convert_mcs_offset(&rate5g->mcs40_offset_5g, nibbles,
        &mcs40in80_offset_5g);

    bcopy(&mcs20in80_offset_5g, &he_mcs20in80_offset_5g, sizeof(mcs20in80_offset_5g));
    bcopy(&mcs40in80_offset_5g, &he_mcs40in80_offset_5g, sizeof(mcs40in80_offset_5g));

    /* for 80IN80MHz OFDM or OFDMQUAD80 */
    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_80, &ofdmquad80_offset_5g, pi);
    /* for 80IN80MHz HT */
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_80,  &rate5g->mcs80_offset_5g, pi);
    /* for ofdm_20IN80: S1x1, S1x2, S1x3 */
    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_20IN80, &ofdm20in80_offset_5g, pi);
    /* for 20IN80MHz HT */
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_20IN80, &mcs20in80_offset_5g, pi);
    /* for 40IN80MHz HT */
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_40IN80, &mcs40in80_offset_5g, pi);
    /* for ofdm_40IN80: S1x1, S1x2, S1x3 */
    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_40IN80, &ofdmdup80_offset_5g, pi);

    /* for 80IN80MHz HE */
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru106_5g, tmp_max_pwr, WL_HE_RT_RU106, FALSE);
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru242_5g, tmp_max_pwr, WL_HE_RT_RU242, FALSE);
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru484_5g, tmp_max_pwr, WL_HE_RT_RU484, FALSE);
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_80,  &rate5g->he_mcs80_offset_5g, pi);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80,  &he_ru106_5g,
        pi, WL_HE_RT_RU26);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80,  &he_ru106_5g,
        pi, WL_HE_RT_RU52);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80,  &he_ru106_5g,
        pi, WL_HE_RT_RU106);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80,  &he_ru242_5g,
        pi, WL_HE_RT_RU242);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80,  &he_ru484_5g,
        pi, WL_HE_RT_RU484);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_80,  &rate5g->he_mcs80_offset_5g,
        pi, WL_HE_RT_RU996);
    /* for 20IN80MHz HE */
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_20IN80, &he_mcs20in80_offset_5g, pi);
    /* for 40IN80MHz HE */
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_40IN80, &he_mcs40in80_offset_5g, pi);
}

void
wlc_phy_txpwr_apply_srom18_5gext_bw160(phy_info_t *pi, uint8 band_5gext, chanspec_t chanspec,
        uint8 tmp_max_pwr, ppr_t *tx_srom_max_pwr, sr13_ppr_5g_rateset_t *rate5g)
{
    uint8 nibbles;
    //int bitN = (band == 1) ? 4 : ((band == 2) ? 8 : 12);
    ppr_ofdm_rateset_t      ofdm160_offset_5g;
    ppr_ofdm_rateset_t      ofdm20in160_offset_5g;
    ppr_ofdm_rateset_t      ofdmdup160_offset_5g;
    ppr_ofdm_rateset_t      ofdmquad160_offset_5g;
    ppr_ofdm_rateset_t      ofdmoct160_offset_5g;
    ppr_vht_mcs_rateset_t   mcs160_offset_5g;
    ppr_vht_mcs_rateset_t   mcs20in160_offset_5g;
    ppr_vht_mcs_rateset_t   mcs40in160_offset_5g;
    ppr_vht_mcs_rateset_t   mcs80in160_offset_5g;
    ppr_he_mcs_rateset_t    he_mcs160_offset_5g;
    ppr_he_mcs_rateset_t    he_mcs20in160_offset_5g;
    ppr_he_mcs_rateset_t    he_mcs40in160_offset_5g;
    ppr_he_mcs_rateset_t    he_mcs80in160_offset_5g;

    ppr_he_mcs_rateset_t    he_ru106_5g;
    ppr_he_mcs_rateset_t    he_ru242_5g;
    ppr_he_mcs_rateset_t    he_ru484_5g;
    ppr_he_mcs_rateset_t    he_ru996_5g;

    const struct srom18_5gextppr *sr18_5gext = &pi->ppr->uext.sr18_5gext;
    ppr_ru_t *board_ru_txpwr_limit;

    //if ((pi->u.pi_acphy->phy_caps & PHY_CAP_HE) == 0) {
    board_ru_txpwr_limit = pi->tpci->data->board_ru_txpwr_limit;
    //}
    /* 5G 11nac 160IN160, shift 12 for  bandwidth 160 */
    nibbles = (sr18_5gext->offset_5g[band_5gext] >> 12) & 0xf;
    wlc_phy_txpwr_srom11_convert_ofdm_5g(sr18_5gext->mcs_5g.bw160[band_5gext], nibbles,
        tmp_max_pwr, &ofdm160_offset_5g);
    wlc_phy_txpwr_srom11_convert_mcs_5g(sr18_5gext->mcs_5g.bw160[band_5gext], nibbles,
        tmp_max_pwr, &mcs160_offset_5g);

    wlc_phy_txpwr_srom13_ext_1024qam_convert_mcs_5g(sr18_5gext->pp1024qam5g[band_5gext],
        WL_PKT_BW_160, tmp_max_pwr, &mcs160_offset_5g);
    /* shift band5g +  30 bits for subband5&6 bw160 ppr mcsexp */
    wlc_phy_txpwr_ppr_bit_ext_srom13_mcs8to11(pi, &mcs160_offset_5g, band_5gext + 30);

    /* 5G ofdm_OCT160, 160in160 */
    //nibbles = wlc_phy_make_byte(sr18_5gext->offset_dup_h >> bitN,
    //sr18_5gext->offset_dup_l >> bitN);
    nibbles = 0;
    wlc_phy_txpwr_srom11_convert_ofdm_offset((ppr_ofdm_rateset_t*)&ofdm160_offset_5g,
        nibbles, &ofdmoct160_offset_5g);

    /* 5G ofdm_QUAD80in160 */
    wlc_phy_txpwr_srom11_convert_ofdm_offset((ppr_ofdm_rateset_t*)&rate5g->ofdm80_offset_5g,
        nibbles, &ofdmquad160_offset_5g);

    /* 5G ofdm_DUP40in160 */
    wlc_phy_txpwr_srom11_convert_ofdm_offset((ppr_ofdm_rateset_t*)&rate5g->ofdm40_offset_5g,
        nibbles, &ofdmdup160_offset_5g);

    /* 5G 11agnac_20IN160 */
    nibbles = wlc_phy_make_byte(sr18_5gext->offset_20in80_h[band_5gext] >> 4,
        sr18_5gext->offset_20in80_l[band_5gext] >> 4);
    wlc_phy_txpwr_srom11_convert_ofdm_offset(&rate5g->ofdm20_offset_5g, nibbles,
        &ofdm20in160_offset_5g);
    wlc_phy_txpwr_srom11_convert_mcs_offset(&rate5g->mcs20_offset_5g, nibbles,
        &mcs20in160_offset_5g);
    if ((CHSPEC_CTL_SB(chanspec) == WL_CHANSPEC_CTL_SB_UUU) ||
        (CHSPEC_CTL_SB(chanspec) == WL_CHANSPEC_CTL_SB_LLL)) {
        /* 12 for 20LLL/20UUU subband w.r.t 20in160 sb */
        nibbles = wlc_phy_make_byte(sr18_5gext->offset_20in80_h[band_5gext] >> 12,
            sr18_5gext->offset_20in80_l[band_5gext] >> 12);
        wlc_phy_txpwr_srom11_convert_ofdm_offset(&ofdm20in160_offset_5g, nibbles,
            &ofdm20in160_offset_5g);
        wlc_phy_txpwr_srom11_convert_mcs_offset(&mcs20in160_offset_5g, nibbles,
            &mcs20in160_offset_5g);
    }
    /* 5G 11nac_40IN160 */
    nibbles = wlc_phy_make_byte(sr18_5gext->offset_40in80_h[band_5gext] >> 4,
            sr18_5gext->offset_40in80_l[band_5gext] >> 4);
    wlc_phy_txpwr_srom11_convert_mcs_offset(&rate5g->mcs40_offset_5g, nibbles,
        &mcs40in160_offset_5g);
    if ((CHSPEC_CTL_SB(chanspec) == WL_CHANSPEC_CTL_SB_UU) ||
        (CHSPEC_CTL_SB(chanspec) == WL_CHANSPEC_CTL_SB_LL)) {
        /* 12 for 40LL/40UU subband w.r.t to 40LU/UL */
        nibbles = wlc_phy_make_byte(sr18_5gext->offset_40in80_h[band_5gext] >> 12,
            sr18_5gext->offset_40in80_l[band_5gext] >> 12);
        wlc_phy_txpwr_srom11_convert_mcs_offset(&mcs40in160_offset_5g, nibbles,
            &mcs40in160_offset_5g);
    }

    /* 5G 11nac_80IN160 */
    nibbles = wlc_phy_make_byte(sr18_5gext->offset_40in80_h[band_5gext] >> 8,
        sr18_5gext->offset_40in80_l[band_5gext] >> 8);
    wlc_phy_txpwr_srom11_convert_mcs_offset(&rate5g->mcs80_offset_5g, nibbles,
        &mcs80in160_offset_5g);

    bcopy(&mcs160_offset_5g, &he_mcs160_offset_5g, sizeof(mcs160_offset_5g));
    bcopy(&mcs20in160_offset_5g, &he_mcs20in160_offset_5g, sizeof(mcs20in160_offset_5g));
    bcopy(&mcs40in160_offset_5g, &he_mcs40in160_offset_5g, sizeof(mcs40in160_offset_5g));
    bcopy(&mcs80in160_offset_5g, &he_mcs80in160_offset_5g, sizeof(mcs80in160_offset_5g));

    /* for 160IN160MHz OFDM or OFDMQUAD160 */
    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_160, &ofdmoct160_offset_5g, pi);
    /* for ofdm_20IN160: S1x1, S1x2, S1x3, S1x4 */
    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_20IN160, &ofdm20in160_offset_5g, pi);
    /* for ofdm_40IN160: S1x1, S1x2, S1x3, S1x4 */
    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_40IN160, &ofdmdup160_offset_5g, pi);
    /* for ofdm_80IN160: S1x1, S1x2, S1x3, S1x4 */
    wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_80IN160, &ofdmquad160_offset_5g, pi);

    /* for 160IN160MHz HT */
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_160, &mcs160_offset_5g, pi);
    /* for 20IN160MHz HT */
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_20IN160, &mcs20in160_offset_5g, pi);
    /* for 40IN160MHz HT */
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_40IN160, &mcs40in160_offset_5g, pi);
    /* for 80IN160MHz HT */
    wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_80IN160, &mcs80in160_offset_5g, pi);

    /* for 160IN160MHz HT */
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru106_5g, tmp_max_pwr, WL_HE_RT_RU106, FALSE);
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru242_5g, tmp_max_pwr, WL_HE_RT_RU242, FALSE);
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru484_5g, tmp_max_pwr, WL_HE_RT_RU484, FALSE);
    wlc_phy_txpwr_ruppr_srom18(pi, &he_ru996_5g, tmp_max_pwr, WL_HE_RT_RU996, FALSE);
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_160, &he_mcs160_offset_5g, pi);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_160, &he_ru106_5g,
        pi, WL_HE_RT_RU26);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_160, &he_ru106_5g,
        pi, WL_HE_RT_RU52);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_160, &he_ru106_5g,
        pi, WL_HE_RT_RU106);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_160, &he_ru242_5g,
        pi, WL_HE_RT_RU242);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_160, &he_ru484_5g,
        pi, WL_HE_RT_RU484);
    wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_160, &he_ru996_5g,
        pi, WL_HE_RT_RU996);
    /* for 20IN160MHz HT */
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_20IN160, &he_mcs20in160_offset_5g, pi);
    /* for 40IN160MHz HT */
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_40IN160, &he_mcs40in160_offset_5g, pi);
    /* for 80IN160MHz HT */
    wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_80IN160, &he_mcs80in160_offset_5g, pi);
}

void
wlc_phy_txpwr_apply_5gext_srom18(phy_info_t *pi, uint8 band_5gext, chanspec_t chanspec,
                           uint8 tmp_max_pwr, ppr_t *tx_srom_max_pwr)
{
    ppr_ru_t *board_ru_txpwr_limit;

    board_ru_txpwr_limit = pi->tpci->data->board_ru_txpwr_limit;

    if (CHSPEC_IS6G(chanspec) || SUBBANB5GV6(pi)) {
        uint8 nibbles;
        sr13_ppr_5g_rateset_t rate5g;
        const struct srom18_5gextppr *sr18_5gext = &pi->ppr->uext.sr18_5gext;
        ppr_he_mcs_rateset_t    he_ru106_5g;
        /* 5G 11agnac_20IN20 */
        nibbles = sr18_5gext->offset_5g[band_5gext] & 0xf; /* 0LSB */
        wlc_phy_txpwr_srom11_convert_ofdm_5g(sr18_5gext->ofdm_5g.bw20[band_5gext],
            nibbles, tmp_max_pwr, &rate5g.ofdm20_offset_5g);
        wlc_phy_txpwr_srom11_convert_mcs_5g(sr18_5gext->ofdm_5g.bw20[band_5gext],
            nibbles, tmp_max_pwr, &rate5g.mcs20_offset_5g);

        wlc_phy_txpwr_srom13_ext_1024qam_convert_mcs_5g(
            sr18_5gext->pp1024qam5g[band_5gext],
            WL_PKT_BW_20, tmp_max_pwr, &rate5g.mcs20_offset_5g);
        /* shift band5g + 6 for subband5&6 bw20 ppr mcsexp */
        wlc_phy_txpwr_ppr_bit_ext_srom13_mcs8to11(pi,
            &rate5g.mcs20_offset_5g, band_5gext + 6);
        bcopy(&rate5g.mcs20_offset_5g, &rate5g.he_mcs20_offset_5g,
            sizeof(rate5g.mcs20_offset_5g));
        wlc_phy_txpwr_ruppr_srom18(pi, &he_ru106_5g, tmp_max_pwr, WL_HE_RT_RU106, FALSE);
        if (CHSPEC_IS20(chanspec)) {
            wlc_phy_ppr_set_ofdm(tx_srom_max_pwr, WL_TX_BW_20,
                &rate5g.ofdm20_offset_5g, pi);
            wlc_phy_ppr_set_mcs(tx_srom_max_pwr, WL_TX_BW_20,
                &rate5g.mcs20_offset_5g, pi);
            wlc_phy_ppr_set_he_mcs(tx_srom_max_pwr, WL_TX_BW_20,
                &rate5g.he_mcs20_offset_5g, pi);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20,
                &he_ru106_5g, pi, WL_HE_RT_RU26);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20,
                &he_ru106_5g, pi, WL_HE_RT_RU52);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20,
                &he_ru106_5g, pi, WL_HE_RT_RU106);
            wlc_phy_ppr_set_he_mcs_ru(board_ru_txpwr_limit, WL_TX_BW_20,
                &rate5g.he_mcs20_offset_5g, pi, WL_HE_RT_RU242);
        } else {
            /* 5G 11nac 40IN40 */
            nibbles = (sr18_5gext->offset_5g[band_5gext] >> 4) & 0xf; /* 1LSB */
            wlc_phy_txpwr_srom11_convert_mcs_5g(sr18_5gext->mcs_5g.bw40[band_5gext],
                nibbles, tmp_max_pwr, &rate5g.mcs40_offset_5g);

            wlc_phy_txpwr_srom13_ext_1024qam_convert_mcs_5g(
                sr18_5gext->pp1024qam5g[band_5gext],
                WL_PKT_BW_40, tmp_max_pwr, &rate5g.mcs40_offset_5g);
            /* shift band5g + 14 for subband5&6 bw40 ppr mcsexp */
            wlc_phy_txpwr_ppr_bit_ext_srom13_mcs8to11(pi, &rate5g.mcs40_offset_5g,
                band_5gext + 14);

            bcopy(&rate5g.mcs40_offset_5g, &rate5g.he_mcs40_offset_5g,
                sizeof(rate5g.mcs40_offset_5g));

            /* same for ofdm 5g dup40 in 40MHz and dup80 in 80MHz */
            wlc_phy_txpwr_srom11_convert_ofdm_5g(sr18_5gext->mcs_5g.bw40[band_5gext],
                nibbles, tmp_max_pwr, &rate5g.ofdm40_offset_5g);

            if (CHSPEC_IS40(chanspec)) {
                wlc_phy_txpwr_apply_srom18_5gext_bw40(pi, band_5gext, tmp_max_pwr,
                    tx_srom_max_pwr, &rate5g);
            } else {
                /* 5G 11nac 80IN80 */
                 /* 2LSB */
                nibbles = (sr18_5gext->offset_5g[band_5gext] >> 8) & 0xf;
                wlc_phy_txpwr_srom11_convert_ofdm_5g(
                    sr18_5gext->mcs_5g.bw80[band_5gext],
                    nibbles, tmp_max_pwr, &rate5g.ofdm80_offset_5g);
                wlc_phy_txpwr_srom11_convert_mcs_5g(
                    sr18_5gext->mcs_5g.bw80[band_5gext],
                    nibbles, tmp_max_pwr, &rate5g.mcs80_offset_5g);
                wlc_phy_txpwr_srom13_ext_1024qam_convert_mcs_5g(
                    sr18_5gext->pp1024qam5g[band_5gext], WL_PKT_BW_80,
                        tmp_max_pwr, &rate5g.mcs80_offset_5g);
                /* shift band5g + 22 for subband5&6 bw80 ppr mcsexp */
                wlc_phy_txpwr_ppr_bit_ext_srom13_mcs8to11(pi,
                    &rate5g.mcs80_offset_5g, band_5gext + 22);

                bcopy(&rate5g.mcs80_offset_5g, &rate5g.he_mcs80_offset_5g,
                    sizeof(rate5g.mcs80_offset_5g));

                if (CHSPEC_IS80(chanspec)) {
                    wlc_phy_txpwr_apply_srom18_5gext_bw80(pi, band_5gext,
                        chanspec, tmp_max_pwr, tx_srom_max_pwr, &rate5g);
                } else if (CHSPEC_IS160(chanspec)) {
                    wlc_phy_txpwr_apply_srom18_5gext_bw160(pi, band_5gext,
                        chanspec, tmp_max_pwr, tx_srom_max_pwr, &rate5g);
                }
            }
        }
    }
}
#endif /* BAND6G */

#endif /* WL11AC */

/* add 1MSB to represent 5bit-width ppr value, for mcs8 and mcs9 only */
static void
wlc_phy_txpwr_ppr_bit_ext_mcs8and9(ppr_vht_mcs_rateset_t* vht, uint8 msb)
{
    /* this added 1MSB is the 4th bit, so left shift 4 bits
     * then left shift 1 more bit since boardlimit is 0.5dB format
     */
    vht->pwr[8] -= ((msb & 0x1) << 4) << 1;
    vht->pwr[9] -= ((msb & 0x2) << 3) << 1;
}

#endif /* PPR_API */

static void
wlc_phy_txpower_sromlimit_get_acphy(phy_info_t *pi, chanspec_t chanspec,
                                        ppr_t *max_pwr, uint8 core)
{
    uint8 band = 0, band_srom = 0;
    uint8 tmp_max_pwr = 0, core_freq_segment_map;
    int8 deltaPwr = 0;
    srom11_pwrdet_t *pwrdet11 = pi->pwrdet_ac;

    ASSERT(core < PHY_CORE_MAX);
    ASSERT((pi->sromi->subband5Gver == PHY_SUBBAND_4BAND) ||
            (pi->sromi->subband5Gver == PHY_MAXNUM_5GSUBBANDS) ||
            (pi->sromi->subband5Gver == PHY_MAXNUM_5GSUBBANDS_V6));

    /* core_freq_segment_map is only required for 80P80 mode.
    For other modes, it is ignored
    */
    core_freq_segment_map =
        phy_ac_chanmgr_get_data(pi->u.pi_acphy->chanmgri)->core_freq_mapping[core];

    /* to figure out which subband is in 5G */
    /* in the range of 0, 1, 2, 3 */
    band = phy_ac_chanmgr_get_chan_freq_range(pi, chanspec, core_freq_segment_map);

    tmp_max_pwr = pwrdet11->max_pwr[0][band];
    if (PHYCORENUM(pi->pubpi->phy_corenum) > 1)
        tmp_max_pwr = MIN(tmp_max_pwr, pwrdet11->max_pwr[1][band]);
    if (PHYCORENUM(pi->pubpi->phy_corenum) > 2)
        tmp_max_pwr = MIN(tmp_max_pwr, pwrdet11->max_pwr[2][band]);

    /*    --------  in 5g_ext case  -----------
     *    if 5170 <= freq < 5250, then band = 1;
     *    if 5250 <= freq < 5500, then band = 2;
     *    if 5500 <= freq < 5745, then band = 3;
     *    if 5745 <= freq,        then band = 4;

     *    --------  in 5g case  ---------------
     *    if 5170 <= freq < 5500, then band = 1;
     *    if 5500 <= freq < 5745, then band = 2;
     *    if 5745 <= freq,        then band = 3;
     */
    /*  -------- 4 subband to 3 subband mapping --------
     *    subband#0 -> low
     *    subband#1 -> mid
     *    subband#2 -> high
     *    subband#3 -> high
     */

    if (band <= WL_CHAN_FREQ_RANGE_5G_BAND2)
        band_srom = band;
    else
        band_srom = band - 1;
    wlc_phy_txpwr_apply_srom11(pi, band_srom, chanspec, tmp_max_pwr, max_pwr);
    deltaPwr = pwrdet11->max_pwr[core][band] - tmp_max_pwr;

    if (deltaPwr > 0)
        ppr_plus_cmn_val(max_pwr, deltaPwr);

    ppr_apply_max(max_pwr, pwrdet11->max_pwr[core][band]);
}

static void
wlc_phy_txpower_sromlimit_get_srom12_acphy(phy_info_t *pi, chanspec_t chanspec,
                                        ppr_t *max_pwr, uint8 core)
{

    uint8 band_ori = 0;
    uint8 band = 0;
    uint8 tmp_max_pwr = 0;
    uint8 chans[NUM_CHANS_IN_CHAN_BONDING];
    int8 deltaPwr = 0;
    srom12_pwrdet_t *pwrdet = pi->pwrdet_ac;
#if BAND6G
    uint8 band_5gext = 0;
    srom18_5gext_pwrdet_t *pwrdet_5gext = pi->pwrdet_ac_5gext;
#endif /* BAND6G */

    ASSERT(core < PHY_CORE_MAX);
    ASSERT(SROMREV(pi->sh->sromrev) >= 12);
    if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
        ASSERT(pi->sromi->subband5Gver == PHY_MAXNUM_5GSUBBANDS ||
                pi->sromi->subband5Gver == PHY_MAXNUM_5GSUBBANDS_V6);
    }

    /* to figure out which subband is in 5G */
    /* in the range of 0, 1, 2, 3, 4, 5, 6 */
    if (ACMAJORREV_33(pi->pubpi->phy_rev) && PHY_AS_80P80(pi, chanspec)) {
        phy_ac_chanmgr_get_chan_freq_range_80p80_srom12(pi, chanspec, chans);
        band_ori = (core <= 1) ? chans[0] : chans[1];
    } else {
        band_ori = phy_ac_chanmgr_get_chan_freq_range_srom12(pi, chanspec);
    }
    if (band_ori < WL_CHAN_FREQ_RANGE_5G_BAND5 ||
        band_ori == WL_CHAN_FREQ_RANGE_5G_BAND4_160) {
        if (band_ori >= WL_CHAN_FREQ_RANGE_2G_40) {
            if (CHSPEC_IS80(chanspec) || PHY_AS_80P80(pi, chanspec)) {
                band = band_ori - WL_CHAN_FREQ_RANGE_5G_BAND0_80 + 1;
            } else if (CHSPEC_IS160(chanspec)) {
                if (band_ori == WL_CHAN_FREQ_RANGE_5G_BAND4_160) {
                    /* WL_CHAN_FREQ_RANGE_5G_BAND4_160 happens in 6G only
                     * srom18 6G BW160 ch4 uses x2 ppr
                     */
                    band = 5;
                } else if (band_ori == WL_CHAN_FREQ_RANGE_5G_BAND3_160 &&
                    CHSPEC_IS5G(pi->radio_chanspec)) {
                    //srom18 5G BW160 ch3 uses x2 ppr
                    band = 5;
                }  else {
                    band = band_ori - WL_CHAN_FREQ_RANGE_5G_BAND0_160 + 1;
                }
            } else {
                ASSERT(CHSPEC_IS40(chanspec));
                band = band_ori - WL_CHAN_FREQ_RANGE_2G_40;
            }
        } else {
            band = band_ori;
        }
        tmp_max_pwr = pwrdet->max_pwr[0][band];
        if (PHYCORENUM(pi->pubpi->phy_corenum) > 1) {
            tmp_max_pwr = MIN(tmp_max_pwr, pwrdet->max_pwr[1][band]);
        }
        if (PHYCORENUM(pi->pubpi->phy_corenum) > 2) {
            tmp_max_pwr = MIN(tmp_max_pwr, pwrdet->max_pwr[2][band]);
        }
        if (PHYCORENUM(pi->pubpi->phy_corenum) > 3) {
            tmp_max_pwr = MIN(tmp_max_pwr, pwrdet->max_pwr[3][band]);
        }
    }
#if BAND6G
    else {
        if (CHSPEC_IS80(chanspec)) {
            band_5gext = band_ori - WL_CHAN_FREQ_RANGE_5G_BAND5_80;
        } else if (CHSPEC_IS160(chanspec)) {
            band_5gext = band_ori - WL_CHAN_FREQ_RANGE_5G_BAND5_160;
        } else if (CHSPEC_IS40(chanspec)) {
            band_5gext = band_ori - WL_CHAN_FREQ_RANGE_5G_BAND5_40;
        } else {
            band_5gext = band_ori - WL_CHAN_FREQ_RANGE_5G_BAND5;
        }
        tmp_max_pwr = pwrdet_5gext->max_pwr[0][band_5gext];
        if (PHYCORENUM(pi->pubpi->phy_corenum) > 1) {
            tmp_max_pwr = MIN(tmp_max_pwr, pwrdet_5gext->max_pwr[1][band_5gext]);
        }
        if (PHYCORENUM(pi->pubpi->phy_corenum) > 2) {
            tmp_max_pwr = MIN(tmp_max_pwr, pwrdet_5gext->max_pwr[2][band_5gext]);
        }
        if (PHYCORENUM(pi->pubpi->phy_corenum) > 3) {
            tmp_max_pwr = MIN(tmp_max_pwr, pwrdet_5gext->max_pwr[3][band_5gext]);
        }
    }

#endif /* BAND6G */
    /* Enable per-core power cap based on boardflags4
    If TXPWR_CAP_EN is set, will apply max power of per band per core
    */
    if (SROMREV(pi->sh->sromrev) >= 18 && (pi->sromi->sr18_txpwr_cap_en)) {
        phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;
        uint8 idx = 0;
        for (idx = 0; idx < pi->pubpi->phy_corenum; idx++) {
            if (band_ori < WL_CHAN_FREQ_RANGE_5G_BAND5 ||
                band_ori == WL_CHAN_FREQ_RANGE_5G_BAND4_160) {
                pi_ac->tpci->txpwr_cap[idx] = pwrdet->max_pwr[idx][band];
            }
#if BAND6G
            else {
                pi_ac->tpci->txpwr_cap[idx] =
                    pwrdet_5gext->max_pwr[idx][band_5gext];
            }
#endif /* BAND6G */
        }
        wlc_phy_txpwr_cap_enable(pi, ENABLE);
    }
    if (SROMREV(pi->sh->sromrev) < 13) {
        wlc_phy_txpwr_apply_srom11(pi, band, chanspec, tmp_max_pwr, max_pwr);
    }
#ifdef WL11AC
    else {
        if (band_ori < WL_CHAN_FREQ_RANGE_5G_BAND5 ||
            band_ori == WL_CHAN_FREQ_RANGE_5G_BAND4_160) {
            wlc_phy_txpwr_apply_srom13(pi, band, chanspec, tmp_max_pwr, max_pwr);
        }
#if BAND6G
        else {
            if (SROMREV(pi->sh->sromrev) == 18) {
                wlc_phy_txpwr_apply_5gext_srom18(pi, band_5gext, chanspec,
                    tmp_max_pwr, max_pwr);
            }
        }
#endif /* BAND6G */
    }
#endif /* WL11AC */
    if (band_ori < WL_CHAN_FREQ_RANGE_5G_BAND5 ||
        band_ori == WL_CHAN_FREQ_RANGE_5G_BAND4_160) {
        deltaPwr = pwrdet->max_pwr[core][band] - tmp_max_pwr;
    }
#if BAND6G
    else {
        deltaPwr = pwrdet_5gext->max_pwr[core][band_5gext] - tmp_max_pwr;
    }
#endif /* BAND6G */
    if (deltaPwr > 0) {
        ppr_plus_cmn_val(max_pwr, deltaPwr);
    }
    if (band_ori < WL_CHAN_FREQ_RANGE_5G_BAND5 ||
        band_ori == WL_CHAN_FREQ_RANGE_5G_BAND4_160) {
        ppr_apply_max(max_pwr, pwrdet->max_pwr[core][band]);
    }
#if BAND6G
    else {
        ppr_apply_max(max_pwr, pwrdet_5gext->max_pwr[core][band_5gext]);
    }
#endif /* BAND6G */
}

static void
phy_ac_tpc_sromlimit_get(phy_type_tpc_ctx_t *ctx, chanspec_t chanspec, ppr_t *max_pwr, uint8 core)
{
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = info->pi;
    if (SROMREV(pi->sh->sromrev) < 12) {
      wlc_phy_txpower_sromlimit_get_acphy(pi, chanspec, max_pwr, core);
    } else {
      wlc_phy_txpower_sromlimit_get_srom12_acphy(pi, chanspec, max_pwr, core);
    }
}

/* report estimated power and adjusted estimated power in quarter dBms */
void
wlc_phy_txpwr_est_pwr_acphy(phy_info_t *pi, uint8 *Pout, uint8 *Pout_adj)
{
    uint8 core, rx_coremask;
    int8 val;
    bool val_ignore;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

    BCM_REFERENCE(stf_shdata);

    /* JIRA: SW4345-571: Ignore valid bit when reading estPwr for wl curpower for 4345
     *Please refer to RB33098 for more details
     */
    /* Do not check for valid bit for 4345(major rev3) since Reset CCA in
     * ucode clears that bit
     */
    val_ignore = (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev));
    if (val_ignore)
        val = 1;

    BCM_REFERENCE(rx_coremask);
    rx_coremask = stf_shdata->hw_phyrxchain;

    FOREACH_ACTV_CORE(pi, rx_coremask, core) {
        if (!val_ignore) {
        val = READ_PHYREGFLDCE(pi, EstPower_path, core, estPowerValid);
        }

        /* Read the Actual Estimated Powers without adjustment */
        if (val) {
            Pout[core] = READ_PHYREGFLDCE(pi, EstPower_path, core, estPower);
        } else {
            Pout[core] = 0;
        }
        if (!val_ignore) {
        val = READ_PHYREGFLDCE(pi, TxPwrCtrlStatus_path, core, estPwrAdjValid);
        }
        if (val) {
            Pout_adj[core] = READ_PHYREGFLDCE(pi, TxPwrCtrlStatus_path, core,
                                              estPwr_adj);
        } else {
            Pout_adj[core] = 0;
        }
    }
}

#ifdef WL11AX
/* report mu power quarter dBms based on txctrlwd */
void
wlc_phy_txpwr_est_pwr_mu_acphy(phy_info_t *pi, phy_txctrl_info_t* txctrlwd_info)
{
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);
    uint64 tbl_val_64[14];
    phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;
    phy_ac_tpc_info_t *tpci = pi_ac->tpci;
    phy_ac_txctrlwd_info_t *ac_txctrlwd = tpci->phy_ac_txctrlwd_info;
    uint8 n, m, j;
    uint16 tmp, n_user_offset;
    uint16 sc_num = 0;
    uint16 n_sc_clm = 0;

    BCM_REFERENCE(stf_shdata);
    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_AXMACPHYIFTBL, 14, 0, 64,
            &tbl_val_64[0]);
        /*
        for (n = 0; n < 14; n++) {
            printf("0x%-2x     0x%-4x   0x%-8x   0x%-8x\n",
                ACPHY_TBL_ID_AXMACPHYIFTBL, n, (uint32)(tbl_val_64[n] & 0xffffffff),
                (uint32)(tbl_val_64[n] >> 32) & 0xffffffff);
        }
        */

        for (n = 0; n <= TXCTRL_N_PWR; n++) {
            m = (uint8)(txctrlwd_tbl[n][0]/64);
            j = (uint8)(txctrlwd_tbl[n][0]%64);
            ac_txctrlwd->common_info[n] = wlc_phy_64bit_shift_parse(tbl_val_64[m],
                j, txctrlwd_tbl[n][1]);
        }
        for (n = 0; n < ac_txctrlwd->common_info[TXCTRL_N_PWR]; n++) {
            m = (uint8)((txctrlwd_tbl[TXCTRL_POWER_OFFSET_0][0] + 8*n)/64);
            j = (uint8)((txctrlwd_tbl[TXCTRL_POWER_OFFSET_0][0] + 8*n)%64);
            ac_txctrlwd->pwr_offset[n] = wlc_phy_64bit_shift_parse(tbl_val_64[m],
                j, 0xff);
        }
        n_user_offset = txctrlwd_tbl[TXCTRL_POWER_OFFSET_0][0] +
            8*ac_txctrlwd->common_info[TXCTRL_N_PWR];
        m = (uint8)(n_user_offset/64);
        j = (uint8)(n_user_offset%64);
        ac_txctrlwd->n_usr = wlc_phy_64bit_shift_parse(tbl_val_64[m],
                j, 0xff);
        // per-user info
        for (n = 0; n < ac_txctrlwd->n_usr; n++) {
            tmp = (n_user_offset+txctrlwd_tbl_mu[TXCTRL_BFM_MU][0]+48*n);
            m = (uint8)(tmp/64); j = (uint8)(tmp%64);
            ac_txctrlwd->bfm_mu[n] = wlc_phy_64bit_shift_parse(tbl_val_64[m],
                j, txctrlwd_tbl_mu[TXCTRL_BFM_MU][1]);
            tmp = (n_user_offset+txctrlwd_tbl_mu[TXCTRL_USRIDX_MU][0]+48*n);
            m = (uint8)(tmp/64); j = (uint8)(tmp%64);
            ac_txctrlwd->usridx_mu[n] = wlc_phy_64bit_shift_parse(tbl_val_64[m],
                j, txctrlwd_tbl_mu[TXCTRL_USRIDX_MU][1]);
            tmp = (n_user_offset+txctrlwd_tbl_mu[TXCTRL_RUIDX_MU][0]+48*n);
            m = (uint8)(tmp/64); j = (uint8)(tmp%64);
            ac_txctrlwd->ruidx_mu[n] = wlc_phy_64bit_shift_parse(tbl_val_64[m],
                j, txctrlwd_tbl_mu[TXCTRL_RUIDX_MU][1]);
            tmp = (n_user_offset+txctrlwd_tbl_mu[TXCTRL_MCS_NSS_MU][0]+48*n);
            m = (uint8)(tmp/64); j = (uint8)(tmp%64);
            ac_txctrlwd->mcs_nss_mu[n] = wlc_phy_64bit_shift_parse(tbl_val_64[m],
                j, txctrlwd_tbl_mu[TXCTRL_MCS_NSS_MU][1]);
            tmp = (n_user_offset+txctrlwd_tbl_mu[TXCTRL_LDPC_MU][0]+48*n);
            m = (uint8)(tmp/64); j = (uint8)(tmp%64);
            ac_txctrlwd->ldpc_mu[n] = wlc_phy_64bit_shift_parse(tbl_val_64[m],
                j, txctrlwd_tbl_mu[TXCTRL_LDPC_MU][1]);
            tmp = (n_user_offset+txctrlwd_tbl_mu[TXCTRL_AID_MU][0]+48*n);
            m = (uint8)(tmp/64); j = (uint8)(tmp%64);
            ac_txctrlwd->aid_mu[n] = wlc_phy_64bit_shift_parse(tbl_val_64[m],
                j, txctrlwd_tbl_mu[TXCTRL_AID_MU][1]);

            txctrlwd_info->bfm_mu[n] = ac_txctrlwd->bfm_mu[n];
            txctrlwd_info->usridx_mu[n] = ac_txctrlwd->usridx_mu[n];
            txctrlwd_info->ruidx_mu[n] = ac_txctrlwd->ruidx_mu[n];
            txctrlwd_info->mcs_nss_mu[n] = ac_txctrlwd->mcs_nss_mu[n];
            txctrlwd_info->ldpc_mu[n] = ac_txctrlwd->ldpc_mu[n];
            txctrlwd_info->aid_mu[n] = ac_txctrlwd->aid_mu[n];
        }
        //condier SU CLM now
        n_sc_clm = tone_sc_clm[ac_txctrlwd->common_info[TXCTRL_PKTBW]][0];
        for (n = 0; n < ac_txctrlwd->n_usr; n++) {
            sc_num = sc_num + ruidx_scnum[ac_txctrlwd->ruidx_mu[n]];
        }
        ac_txctrlwd->rl_backoff = wlc_phy_ten_log_ten(n_sc_clm, sc_num);

        wlc_phy_txctrl_hesigext_parse(pi, tbl_val_64, ac_txctrlwd);

        txctrlwd_info->frame_type = ac_txctrlwd->common_info[TXCTRL_FRAME_TYPE];
        txctrlwd_info->he_format = ac_txctrlwd->common_info[TXCTRL_HE_FORMAT];
        txctrlwd_info->not_sounding = ac_txctrlwd->common_info[TXCTRL_NOT_SOUNDING];
        txctrlwd_info->preamble = ac_txctrlwd->common_info[TXCTRL_PREAMBLE];
        txctrlwd_info->mcs_nss = ac_txctrlwd->common_info[TXCTRL_MCS_NSS];
        txctrlwd_info->stbc = ac_txctrlwd->common_info[TXCTRL_STBC];
        txctrlwd_info->n_user = ac_txctrlwd->n_usr;
        txctrlwd_info->n_pwr = ac_txctrlwd->common_info[TXCTRL_N_PWR];
        txctrlwd_info->mu = ac_txctrlwd->common_info[TXCTRL_MU];
        txctrlwd_info->core_mask = ac_txctrlwd->common_info[TXCTRL_COREMASK];
        txctrlwd_info->ant_cfg = ac_txctrlwd->common_info[TXCTRL_ANTCFG];
        txctrlwd_info->pktbw = ac_txctrlwd->common_info[TXCTRL_PKTBW];
        txctrlwd_info->subband = ac_txctrlwd->common_info[TXCTRL_SUBBAND_LOCATION];
        txctrlwd_info->partial_ofdma_subband =
            ac_txctrlwd->common_info[TXCTRL_PARTIAL_OFDMA_SUBBAND];
        txctrlwd_info->dynBW_present = ac_txctrlwd->common_info[TXCTRL_DYNBW_PRESENT];
        txctrlwd_info->dynBW_mode = ac_txctrlwd->common_info[TXCTRL_DYNBW_MOD];
        txctrlwd_info->scrambler = ac_txctrlwd->common_info[TXCTRL_SCRAMBLER];
        txctrlwd_info->cfo_comp_val = ac_txctrlwd->common_info[TXCTRL_CFO_COMP_VAL];
        txctrlwd_info->cfo_comp_en = ac_txctrlwd->common_info[TXCTRL_CFO_COMP_EN];
        txctrlwd_info->has_trigger_info = ac_txctrlwd->common_info[TXCTRL_HAS_TRIGGER];
        for (n = 0; n < ac_txctrlwd->common_info[TXCTRL_N_PWR]; n++) {
            txctrlwd_info->pwr_offset[n] = ac_txctrlwd->pwr_offset[n];
        }
        txctrlwd_info->rl_backoff = ac_txctrlwd->rl_backoff;
        txctrlwd_info->cp_ltf_size = ac_txctrlwd->hesig_ext[HESIGEXT_CPLTFSIZE];
        if (txctrlwd_info->cp_ltf_size != 0xff) {
            txctrlwd_info->heltf_mask    =
                (bool)ac_txctrlwd->hesig_ext[HESIGEXT_HELTF_MASK];
            txctrlwd_info->num_heltf     =
                ac_txctrlwd->hesig_ext[HESIGEXT_NUM_HELTF];
            txctrlwd_info->ldpc_extra    =
                (bool)ac_txctrlwd->hesig_ext[HESIGEXT_LDPC_EXTRA];
            txctrlwd_info->afactor       =
                ac_txctrlwd->hesig_ext[HESIGEXT_AFACTOR];
            txctrlwd_info->disamb        =
                (bool)ac_txctrlwd->hesig_ext[HESIGEXT_DISAMB];
            txctrlwd_info->doppler       =
                (bool)ac_txctrlwd->hesig_ext[HESIGEXT_DOPPLER];
            txctrlwd_info->ldpc          =
                (bool)ac_txctrlwd->hesig_ext[HESIGEXT_CODING];
            txctrlwd_info->mcs           =
                ac_txctrlwd->hesig_ext[HESIGEXT_MCS];
            txctrlwd_info->dcm           =
                (bool)ac_txctrlwd->hesig_ext[HESIGEXT_DCM];
            txctrlwd_info->nss_start_pos =
                ac_txctrlwd->hesig_ext[HESIGEXT_NSSSTART];
            txctrlwd_info->nss           =
                ac_txctrlwd->hesig_ext[HESIGEXT_NSS];
        }
        PHY_TXPWR(("wl%d:%s *******txctrlwd parsing*******\n",
        pi->sh->unit, __FUNCTION__));
        PHY_TXPWR(("wl%d:%s frame_type=%d\n",
        pi->sh->unit, __FUNCTION__, ac_txctrlwd->common_info[TXCTRL_FRAME_TYPE]));
        PHY_TXPWR(("wl%d:%s he_format=%d\n",
        pi->sh->unit, __FUNCTION__, ac_txctrlwd->common_info[TXCTRL_HE_FORMAT]));
        PHY_TXPWR(("wl%d:%s not_sounding=%d\n",
        pi->sh->unit, __FUNCTION__, ac_txctrlwd->common_info[TXCTRL_NOT_SOUNDING]));
        PHY_TXPWR(("wl%d:%s preamble=%d\n",
        pi->sh->unit, __FUNCTION__, ac_txctrlwd->common_info[TXCTRL_PREAMBLE]));
        PHY_TXPWR(("wl%d:%s mcs_nss=0x%-2x\n",
        pi->sh->unit, __FUNCTION__, ac_txctrlwd->common_info[TXCTRL_MCS_NSS]));
        PHY_TXPWR(("wl%d:%s stbc=%d\n",
        pi->sh->unit, __FUNCTION__, ac_txctrlwd->common_info[TXCTRL_STBC]));
        PHY_TXPWR(("wl%d:%s pktbw=%d\n",
        pi->sh->unit, __FUNCTION__, ac_txctrlwd->common_info[TXCTRL_PKTBW]));
        PHY_TXPWR(("wl%d:%s subband_location=%d\n",
        pi->sh->unit, __FUNCTION__, ac_txctrlwd->common_info[TXCTRL_SUBBAND_LOCATION]));
        PHY_TXPWR(("wl%d:%s mu=%d\n",
        pi->sh->unit, __FUNCTION__, ac_txctrlwd->common_info[TXCTRL_MU]));
        PHY_TXPWR(("wl%d:%s coremask=%d\n",
        pi->sh->unit, __FUNCTION__, ac_txctrlwd->common_info[TXCTRL_COREMASK]));
        PHY_TXPWR(("wl%d:%s antcfg=%d\n",
        pi->sh->unit, __FUNCTION__, ac_txctrlwd->common_info[TXCTRL_ANTCFG]));
        PHY_TXPWR(("wl%d:%s TXCTRL_N_PWR=%d\n",
        pi->sh->unit, __FUNCTION__, ac_txctrlwd->common_info[TXCTRL_N_PWR]));
        for (n = 0; n < ac_txctrlwd->common_info[TXCTRL_N_PWR]; n++) {
            PHY_TXPWR(("wl%d:%s power_offset_%d=%d\n",
            pi->sh->unit, __FUNCTION__, n, ac_txctrlwd->pwr_offset[n]));
        }
        PHY_TXPWR(("wl%d:%s n_usr=%d\n",
        pi->sh->unit, __FUNCTION__, ac_txctrlwd->n_usr));

        for (n = 0; n < ac_txctrlwd->n_usr; n++) {
            PHY_TXPWR(("wl%d:%s *******USER-%d*******\n",
            pi->sh->unit, __FUNCTION__, n));
            PHY_TXPWR(("wl%d:%s bfm_mu=%d\n",
            pi->sh->unit, __FUNCTION__, ac_txctrlwd->bfm_mu[n]));
            PHY_TXPWR(("wl%d:%s ruidx_mu=%d\n",
            pi->sh->unit, __FUNCTION__, ac_txctrlwd->ruidx_mu[n]));
            PHY_TXPWR(("wl%d:%s mcs_nss_mu=0x%-2x\n",
            pi->sh->unit, __FUNCTION__, ac_txctrlwd->mcs_nss_mu[n]));
            PHY_TXPWR(("wl%d:%s ldpc_mu=%d\n",
            pi->sh->unit, __FUNCTION__, ac_txctrlwd->ldpc_mu[n]));
            PHY_TXPWR(("wl%d:%s aid_mu=%d\n",
            pi->sh->unit, __FUNCTION__, ac_txctrlwd->aid_mu[n]));
        }
        if (txctrlwd_info->cp_ltf_size != 0xff) {
            PHY_TXPWR(("wl%d:%s *******HESIG-EXT*******\n",
            pi->sh->unit, __FUNCTION__));
            PHY_TXPWR(("wl%d:%s cp_ltf_size=%d\n",
            pi->sh->unit, __FUNCTION__, txctrlwd_info->cp_ltf_size));
            PHY_TXPWR(("wl%d:%s heltf_mask=%d\n",
            pi->sh->unit, __FUNCTION__, txctrlwd_info->heltf_mask));
            PHY_TXPWR(("wl%d:%s ldpc_extra=%d\n",
            pi->sh->unit, __FUNCTION__, txctrlwd_info->ldpc_extra));
            PHY_TXPWR(("wl%d:%s afactor=%d\n",
            pi->sh->unit, __FUNCTION__, txctrlwd_info->afactor));
            PHY_TXPWR(("wl%d:%s disamb=%d\n",
            pi->sh->unit, __FUNCTION__, txctrlwd_info->disamb));
            PHY_TXPWR(("wl%d:%s doppler=%d\n",
            pi->sh->unit, __FUNCTION__, txctrlwd_info->doppler));
            PHY_TXPWR(("wl%d:%s ldpc=%d\n",
            pi->sh->unit, __FUNCTION__, txctrlwd_info->ldpc));
            PHY_TXPWR(("wl%d:%s mcs=%d\n",
            pi->sh->unit, __FUNCTION__, txctrlwd_info->mcs));
            PHY_TXPWR(("wl%d:%s dcm=%d\n",
            pi->sh->unit, __FUNCTION__, txctrlwd_info->dcm));
            PHY_TXPWR(("wl%d:%s nss_start_pos=%d\n",
            pi->sh->unit, __FUNCTION__, txctrlwd_info->nss_start_pos));
            PHY_TXPWR(("wl%d:%s nss=%d\n",
            pi->sh->unit, __FUNCTION__, txctrlwd_info->nss));
        }
        PHY_TXPWR(("wl%d:%s Total SC in CLM      = %d\n",
        pi->sh->unit, __FUNCTION__, n_sc_clm));
        PHY_TXPWR(("wl%d:%s Total SC in DL-OFDMA      = %d\n",
        pi->sh->unit, __FUNCTION__, sc_num));
        PHY_TXPWR(("wl%d:%s RL BACKOFF      = %d\n",
        pi->sh->unit, __FUNCTION__, ac_txctrlwd->rl_backoff));
    }
}
#endif /* WL11AX */

#ifdef PREASSOC_PWRCTRL
static void
wlc_phy_store_tx_pwrctrl_setting_acphy(phy_type_tpc_ctx_t *ctx, chanspec_t previous_channel)
{
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = tpci->pi;
    uint8 core, iidx, rx_coremask;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

    BCM_REFERENCE(stf_shdata);

    if (!pi->sh->up)
        return;

    if (CHSPEC_ISPHY5G6G(previous_channel)) {
        tpci->pwr_ctrl_save->last_chan_stored_5g = previous_channel;

    } else {
        tpci->pwr_ctrl_save->last_chan_stored_2g = previous_channel;

    }

    BCM_REFERENCE(rx_coremask);
    rx_coremask = stf_shdata->hw_phyrxchain;

    FOREACH_ACTV_CORE(pi, rx_coremask, core) {
        iidx = READ_PHYREGFLDCE(pi, TxPwrCtrlStatus_path, core, baseIndex);
        if (CHSPEC_ISPHY5G6G(previous_channel)) {
            tpci->pwr_ctrl_save->status_idx_5g[core] = iidx;
            tpci->pwr_ctrl_save->pwr_qdbm_5g[core] =
                    wlc_phy_txpwrctrl_get_target_acphy(pi, core);
            tpci->pwr_ctrl_save->stored_not_restored_5g[core] = TRUE;

        } else {
            tpci->pwr_ctrl_save->status_idx_2g[core] = iidx;
            tpci->pwr_ctrl_save->pwr_qdbm_2g[core] =
                    wlc_phy_txpwrctrl_get_target_acphy(pi, core);
            tpci->pwr_ctrl_save->stored_not_restored_2g[core] = TRUE;
        }

    }
}
#endif /* PREASSOC_PWRCTRL */

void
wlc_phy_txpwrctrl_set_target_acphy(phy_info_t *pi, uint8 pwr_qtrdbm, uint8 core)
{
    /* set target powers in 6.2 format (in dBs) */
    switch (core) {
    case 0:
        MOD_PHYREG(pi, TxPwrCtrlTargetPwr_path0, targetPwr0, pwr_qtrdbm);
        break;
    case 1:
        MOD_PHYREG(pi, TxPwrCtrlTargetPwr_path1, targetPwr1, pwr_qtrdbm);
        break;
    case 2:
        MOD_PHYREG(pi, TxPwrCtrlTargetPwr_path2, targetPwr2, pwr_qtrdbm);
        break;
    case 3:
        MOD_PHYREG(pi, TxPwrCtrlTargetPwr_path3, targetPwr3, pwr_qtrdbm);
        break;
    }
}

/* Set init index and cycle clocks to propagate init index to base index */
void
wlc_phy_txpwrctrl_set_baseindex(phy_info_t *pi, uint8 core, uint8 baseindex, bool frame_type)
{
    uint16 txpwrctrlcmd_SAVE;
#if defined(WLC_TXCAL)
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
#endif

    txpwrctrlcmd_SAVE = READ_PHYREG(pi, TxPwrCtrlCmd);
    if (frame_type) {
        MOD_PHYREGCEE(pi, TxPwrCtrlInit_path, core,
            pwrIndex_init_path, baseindex);
    } else {
        MOD_PHYREGCEE(pi, TxPwrCtrlInit_path, core,
            pwr_index_init_cck_path, baseindex);
    }

#if defined(WLC_TXCAL)
    /* Update saved baseindex */
    if (phy_tssical_get_olpc_idx_valid_in_use(pi->tssicali)) {
        if (frame_type) {
            pi_ac->tpci->txpwrindex_hw_save[core] = baseindex;
        } else {
            pi_ac->tpci->txpwrindex_cck_hw_save[core] = baseindex;
        }
    }
#endif    /* WLC_TXCAL */

    if (!ACMAJORREV_128(pi->pubpi->phy_rev)) {
        wlapi_bmac_phyclk_fgc(pi->sh->physhim, ON);
        MOD_PHYREG(pi, TxPwrCtrlCmd, txPwrCtrl_en, 0);
        MOD_PHYREG(pi, TxPwrCtrlCmd, hwtxPwrCtrl_en, 1);
        MOD_PHYREG(pi, TxPwrCtrlCmd, txpwrctrlReset, 1);
        OSL_DELAY(2);
        MOD_PHYREG(pi, TxPwrCtrlCmd, txpwrctrlReset, 0);
        WRITE_PHYREG(pi, TxPwrCtrlCmd, txpwrctrlcmd_SAVE);
        wlapi_bmac_phyclk_fgc(pi->sh->physhim, OFF);
    }
    PHY_TXPWR(("%s baseindex set to %d for core %d\n",
        __FUNCTION__, baseindex, core));
}

void
BCMATTACHFN(wlc_phy_txpwrctrl_config_acphy)(phy_info_t *pi)
{
    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    pi->hwpwrctrl_capable = TRUE;
    pi->txpwrctrl = PHY_TPC_HW_ON;
    pi->phy_5g_pwrgain = TRUE;
    pi->hwpprctrl = PHY_HWPPR_OFF;

    //43684 MCM board ID, 0x84D = 2125
    if (pi->sh->boardtype == 2125 && ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        //TPC off for MCM P351 or ealier, 0x1351 = 4945, 0x1361 = 4961
        //TPC off for MCM P352/362 due to SROM boardlimit has issue
        if ((pi->sh->boardrev <= 4946) || (pi->sh->boardrev == 4961) ||
            (pi->sh->boardrev == 4962)) {
            pi->txpwrctrl = PHY_TPC_HW_OFF;
        }
    }

    if (ISSIM_ENAB(pi->sh->sih)) {
        pi->txpwrctrl = PHY_TPC_HW_OFF;
    }

}

int
phy_ac_tpc_txpower_core_offset_set(phy_type_tpc_ctx_t *ctx, struct phy_txcore_pwr_offsets *offsets)
{
    int8 core_offset;
    int core, offset_changed = FALSE;
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = info->pi;

    FOREACH_CORE(pi, core) {
        core_offset = offsets->offset[core];
        if (core_offset != 0 && core > pi->pubpi->phy_corenum) {
            return BCME_BADARG;
        }

        if (info->txpwr_offset[core] != core_offset) {
            offset_changed = TRUE;
            info->txpwr_offset[core] = core_offset;
        }
    }

    /* Apply the new per-core targets to the hw */
    if (pi->sh->clk && offset_changed) {
        wlapi_suspend_mac_and_wait(pi->sh->physhim);
        wlc_phy_txpower_recalc_target_acphy(pi);
        wlapi_enable_mac(pi->sh->physhim);
    }
    return BCME_OK;
}

int
phy_ac_tpc_txpower_core_offset_get(phy_type_tpc_ctx_t *ctx, struct phy_txcore_pwr_offsets *offsets)
{
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    int core;

    memset(offsets, 0, sizeof(struct phy_txcore_pwr_offsets));

    FOREACH_CORE(info->pi, core) {
        offsets->offset[core] = info->txpwr_offset[core];
    }
    return BCME_OK;
}

#if defined(WL_SARLIMIT) || defined(WL_SAR_SIMPLE_CONTROL)
static void
wlc_phy_set_sarlimit_acphy(phy_type_tpc_ctx_t *ctx)
{
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = tpci->pi;
    uint core;
    int16 txpwr_sarcap[3] = { 0, 0, 0 };
    bool is_txpwrcap_changed = FALSE;
    int host_flags6;

#ifdef FCC_PWR_LIMIT_2G
    uint cur_chan = CHSPEC_CHANNEL(pi->radio_chanspec);
    phy_tpc_data_t *data = tpci->ti->data;
#endif /* FCC_PWR_LIMIT_2G */
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

    BCM_REFERENCE(stf_shdata);

    ASSERT(pi->sh->clk);
    wlapi_suspend_mac_and_wait(pi->sh->physhim);
    phy_utils_phyreg_enter(pi);
    FOREACH_ACTV_CORE(pi, stf_shdata->phyrxchain, core) {
        txpwr_sarcap[core] = pi->tpci->data->sarlimit[core];
#ifdef FCC_PWR_LIMIT_2G
        if (data->cfg->srom_tworangetssi2g &&
            (data->fcc_pwr_limit_2g || data->cfg->fccpwroverride)) {
            if (cur_chan == 12 && data->cfg->fccpwrch12 > 0) {
                txpwr_sarcap[core] = MIN(data->cfg->fccpwrch12, txpwr_sarcap[core]);
            } else if (cur_chan == 13 && data->cfg->fccpwrch13 > 0) {
                txpwr_sarcap[core] = MIN(data->cfg->fccpwrch13, txpwr_sarcap[core]);
            }
        }
#endif /* FCC_PWR_LIMIT_2G */
#ifdef WL_SARLIMIT
        if ((tpci->txpwr_offset[core] != 0) &&
            !CHSPEC_ISPHY5G6G(pi->radio_chanspec))
            txpwr_sarcap[core] = wlc_phy_calc_adjusted_cap_rgstr_acphy(pi, core);
#endif
        if (txpwr_sarcap[core] < WLC_TXPWR_MAX) {
            is_txpwrcap_changed = TRUE;
        }
    }

    /* if MHF6_TXPWRCAP_EN bit is set,
     * firmware couldn't control the txpower capping register.
     */
    host_flags6 = wlapi_bmac_read_shm(pi->sh->physhim, M_HOST_FLAGS6(pi));
    if (is_txpwrcap_changed) {
        wlapi_bmac_write_shm(pi->sh->physhim, M_HOST_FLAGS6(pi),
            host_flags6 & ~MHF6_TXPWRCAP_EN);
    } else {
        wlapi_bmac_write_shm(pi->sh->physhim, M_HOST_FLAGS6(pi),
            host_flags6 | MHF6_TXPWRCAP_EN);
    }

    IF_ACTV_CORE(pi, stf_shdata->phyrxchain, 0)
            MOD_PHYREG(pi, TxPwrCapping_path0,
                       maxTxPwrCap_path0, txpwr_sarcap[0]);
    IF_ACTV_CORE(pi, stf_shdata->phyrxchain, 1)
            MOD_PHYREG(pi, TxPwrCapping_path1,
                       maxTxPwrCap_path1, txpwr_sarcap[1]);
    IF_ACTV_CORE(pi, stf_shdata->phyrxchain, 2)
            MOD_PHYREG(pi, TxPwrCapping_path2,
                       maxTxPwrCap_path2, txpwr_sarcap[2]);
    phy_utils_phyreg_exit(pi);
    wlapi_enable_mac(pi->sh->physhim);
}
#endif /* WL_SARLIMIT || WL_SAR_SIMPLE_CONTROL */

static void
wlc_acphy_avvmid_txcal(phy_type_tpc_ctx_t *ctx, wlc_phy_avvmid_txcal_t *avvmidinfo, bool set)
{
    uint8 i, j;
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    if (set) {
        for (i = 0; i < PHY_CORE_MAX; i++) {
            for (j = 0; j < NUM_SUBBANDS_FOR_AVVMID; j++) {
                info->aci->sromi->avvmid_set_from_nvram[i][j][0] =
                    avvmidinfo->avvmid[i][j].Av;
                info->aci->sromi->avvmid_set_from_nvram[i][j][1] =
                    avvmidinfo->avvmid[i][j].Vmid;
            }
        }
    } else {
        for (i = 0; i < PHY_CORE_MAX; i++) {
            for (j = 0; j < NUM_SUBBANDS_FOR_AVVMID; j++) {
                avvmidinfo->avvmid[i][j].Av =
                    info->aci->sromi->avvmid_set_from_nvram[i][j][0];
                avvmidinfo->avvmid[i][j].Vmid =
                    info->aci->sromi->avvmid_set_from_nvram[i][j][1];
            }
        }
    }
}

static void
BCMATTACHFN(wlc_phy_txpwr_srom11_read_pwrdet)(phy_info_t *pi, srom11_pwrdet_t * pwrdet,
    uint8 param, uint8 band, uint8 offset,  const char * name)
{
    pwrdet->pwrdet_a1[param][band] =
        (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, name, offset);
    offset++;
    pwrdet->pwrdet_b0[param][band] =
        (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, name, offset);
    offset++;
    pwrdet->pwrdet_b1[param][band] =
        (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, name, offset);
}

#if BAND5G
static void
BCMATTACHFN(wlc_phy_txpwr_srom11_read_5Gsubbands)(phy_info_t *pi, srom11_pwrdet_t * pwrdet,
    uint8 subband, bool update_rsdb_core1_params, uint8 ant)
{
    uint8 b, b_triple_offset;

    b = subband + CH_2G_GROUP;
    b_triple_offset = 3 * subband;

    /* CORE 0 */
    pwrdet->max_pwr[0][b] = (int8)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_maxp5ga0, subband);
    wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 0, b, b_triple_offset, rstr_pa5ga0);
    pwrdet->tssifloor[0][b] = (int16)PHY_GETINTVAR_ARRAY_DEFAULT(pi,
        rstr_tssifloor5g, subband, 0);

    /* CORE 1 no reuse */
    if ((PHYCORENUM(pi->pubpi->phy_corenum) > 1) ||
        (update_rsdb_core1_params == TRUE)) {
        pwrdet->max_pwr[ant][b] =
            (int8)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_maxp5ga1, subband);
        pwrdet->tssifloor[ant][b] = (int16)PHY_GETINTVAR_ARRAY_DEFAULT(pi,
            rstr_tssifloor5g, subband, 0);
    }
    /* CORE 1 or reuse */
    if (PHYCORENUM(pi->pubpi->phy_corenum) > 1 || update_rsdb_core1_params == TRUE)  {
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, ant, b, b_triple_offset, rstr_pa5ga1);
    }
    /* CORE 2 no reuse */
    if (PHYCORENUM(pi->pubpi->phy_corenum) > 2) {
        pwrdet->max_pwr[2][b] = (int8)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_maxp5ga2, subband);
        pwrdet->tssifloor[2][b] = (int16)PHY_GETINTVAR_ARRAY_DEFAULT(pi,
            rstr_tssifloor5g, subband, 0);
    }
    /* CORE 2 or reuse */
    if (PHYCORENUM(pi->pubpi->phy_corenum) > 2 ||
        (pi->tpci->data->cfg->srom_tworangetssi5g &&
            ACMAJORREV_2(pi->pubpi->phy_rev))) {
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 2, b, b_triple_offset, rstr_pa5ga2);
    }
    /* CORE 3 reuse only */
    if (pi->tpci->data->cfg->srom_tworangetssi5g &&
        ACMAJORREV_2(pi->pubpi->phy_rev)) {
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 3, b, b_triple_offset, rstr_pa5ga3);
    }
    if (pi->tpci->data->cfg->srom_tworangetssi5g &&
        ACMAJORREV_4(pi->pubpi->phy_rev)) {
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 3, b, b_triple_offset, rstr_pa5ga2);
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 3, b, b_triple_offset, rstr_pa5ga3);
    }
    if ((ACMAJORREV_2(pi->pubpi->phy_rev) ||
        ACMAJORREV_4(pi->pubpi->phy_rev)) &&
        BF3_TSSI_DIV_WAR(pi->u.pi_acphy)) {
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 2, b,
            b_triple_offset, rstr_pa5gbw4080a0);
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 2 + ant, b,
            b_triple_offset, rstr_pa5gbw4080a1);
    }
}
#endif /* BAND5G */

static bool BCMATTACHFN(wlc_phy_txpwr_srom11_read)(phy_type_tpc_ctx_t *ctx)
{
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = info->pi;
    srom11_pwrdet_t *pwrdet = pi->pwrdet_ac;
    uint8 ant = 1;
    bool update_rsdb_core1_params = FALSE;
#ifdef POWPERCHANNL
    uint8 ch;
#endif /* POWPERCHANNL */
    ASSERT(pi->sh->sromrev >= 11);

    if (ACMAJORREV_4(pi->pubpi->phy_rev) && (phy_get_phymode(pi) == PHYMODE_RSDB))
    {
        /* update pi[0] to hold pwrdet params for all cores */
        /* This is required for mimo operation */
        if (phy_get_current_core(pi) == PHY_RSBD_PI_IDX_CORE0)
        {
            pi->pubpi->phy_corenum <<= 1;
        }
        else if (phy_get_current_core(pi) == PHY_RSBD_PI_IDX_CORE1)
        {
            /* Incase of RSDB Core1 PA PD MAXPwr Params needs to be
             * updated from Core1 nvram params
             */
            ant = 0;
            update_rsdb_core1_params = TRUE;
        }
    }
    /* read pwrdet params for each band/subband */

    /* 2G band */
    pwrdet->max_pwr[0][0] = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp2ga0);
    wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 0, 0, 0, rstr_pa2ga0);
    pwrdet->tssifloor[0][0] = (int16)PHY_GETINTVAR_ARRAY_DEFAULT(pi,
        rstr_tssifloor2g, 0, 0);
#ifdef POWPERCHANNL
    if (PWRPERCHAN_ENAB(pi)) {
        /* power per channel and Temp */
        pwrdet->max_pwr_SROM2G[0] = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp2ga0);

        PHY_TXPWR(("wl%d: %s: Loading max = %d \n",
            pi->sh->unit, __FUNCTION__,
            pwrdet->max_pwr_SROM2G[0]));
        for (ch = 0; ch < CH20MHz_NUM_2G; ch++) {
            pwrdet->PwrOffsets2GNormTemp[0][ch] =
                (int8)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi, rstr_PowOffs2GTNA0,
                ch, 0);
            pwrdet->PwrOffsets2GLowTemp[0][ch] =
                (int8)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi, rstr_PowOffs2GTLA0,
                ch, 0);
            pwrdet->PwrOffsets2GHighTemp[0][ch] =
                (int8)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi, rstr_PowOffs2GTHA0,
                ch, 0);

            PHY_TXPWR(("Core=0 Ch=%d Offset: Norm=%d Low=%d High=%d\n",
                ch, pwrdet->PwrOffsets2GNormTemp[0][ch],
                pwrdet->PwrOffsets2GLowTemp[0][ch],
                pwrdet->PwrOffsets2GHighTemp[0][ch]));
        }
        pwrdet->Low2NormTemp =
            (int16)PHY_GETINTVAR_ARRAY_DEFAULT(pi, rstr_PowOffsTempRange, 0, 0xff);
        pwrdet->High2NormTemp =
            (int16)PHY_GETINTVAR_ARRAY_DEFAULT(pi, rstr_PowOffsTempRange, 1, 0xff);

        PHY_TXPWR((" Low Temp Limit=%d    High Temp Limit=%d \n",
            pwrdet->Low2NormTemp, pwrdet->High2NormTemp));
    }
#endif  /* POWPERCHANNL */
    if ((PHYCORENUM(pi->pubpi->phy_corenum) > 1) ||
        (update_rsdb_core1_params == TRUE)) {
        pwrdet->max_pwr[ant][0] = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp2ga1);
        pwrdet->tssifloor[ant][0] = (int16)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(
            pi, rstr_tssifloor2g, 1, 0);
#ifdef POWPERCHANNL
        if (PWRPERCHAN_ENAB(pi)) {
            pwrdet->max_pwr_SROM2G[ant] = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp2ga1);
            for (ch = 0; ch < CH20MHz_NUM_2G; ch++) {
                pwrdet->PwrOffsets2GNormTemp[ant][ch] =
                        (int8)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
                        rstr_PowOffs2GTNA1, ch, 0);
                pwrdet->PwrOffsets2GLowTemp[ant][ch] =
                        (int8)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
                        rstr_PowOffs2GTLA1, ch, 0);
                pwrdet->PwrOffsets2GHighTemp[ant][ch] =
                        (int8)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
                        rstr_PowOffs2GTHA1, ch, 0);

                PHY_TXPWR(("Core=1 Ch=%d Offset:", ch));
                PHY_TXPWR(("Norm=%d Low=%d High=%d\n",
                    pwrdet->PwrOffsets2GNormTemp[ant][ch],
                    pwrdet->PwrOffsets2GLowTemp[ant][ch],
                    pwrdet->PwrOffsets2GHighTemp[ant][ch]));
            }
        }
#endif  /* POWPERCHANNL */
    }

    if (PHYCORENUM(pi->pubpi->phy_corenum) > 1 || update_rsdb_core1_params == TRUE) {
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, ant, 0, 0, rstr_pa2ga1);
    }

    if (PHYCORENUM(pi->pubpi->phy_corenum) > 2 ||
        (pi->tpci->data->cfg->srom_tworangetssi2g &&
        ACMAJORREV_2(pi->pubpi->phy_rev))) {
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 2, 0, 0, rstr_pa2ga2);
    }
    if ((pi->tpci->data->cfg->srom_tworangetssi2g &&
        ACMAJORREV_4(pi->pubpi->phy_rev))) {
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 2, 0, 0, rstr_pa2ga2);
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 2, 0, 0, rstr_pa2ga3);
    }

    if ((pi->tpci->data->cfg->srom_tworangetssi2g &&
        ACMAJORREV_128(pi->pubpi->phy_rev))) {
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 2, 0, 0, rstr_pa2ga2);
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 3, 0, 0, rstr_pa2ga3);
    }

    if (PHYCORENUM(pi->pubpi->phy_corenum) > 2) {
        pwrdet->max_pwr[2][0] = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp2ga2);
        pwrdet->tssifloor[2][0] = (int16)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(
            pi, rstr_tssifloor2g, 2, 0);
#ifdef POWPERCHANNL
        if (PWRPERCHAN_ENAB(pi)) {
            pwrdet->max_pwr_SROM2G[2] =
                (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp2ga2);
            for (ch = 0; ch < CH20MHz_NUM_2G; ch++) {
                pwrdet->PwrOffsets2GNormTemp[2][ch] =
                        (int8)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
                        rstr_PowOffs2GTNA2, ch, 0);
                pwrdet->PwrOffsets2GLowTemp[2][ch] =
                        (int8)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
                        rstr_PowOffs2GTLA2, ch, 0);
                pwrdet->PwrOffsets2GHighTemp[2][ch] =
                        (int8)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
                        rstr_PowOffs2GTHA2, ch, 0);
            }
            /* input range limit for power per channel */
            wlc_phy_tx_target_pwr_per_channel_limit_acphy(pi);
        }
#endif  /* POWPERCHANNL */
    }
    if (pi->tpci->data->cfg->srom_tworangetssi2g &&
        ACMAJORREV_2(pi->pubpi->phy_rev)) {
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 3, 0, 0, rstr_pa2ga3);
    }
#ifdef POWPERCHANNL
    if (PWRPERCHAN_ENAB(pi)) {
        /* input range limit for power per channel */
        wlc_phy_tx_target_pwr_per_channel_limit_acphy(pi);
    }
#endif  /* POWPERCHANNL */
    if ((BF3_TSSI_DIV_WAR(pi->u.pi_acphy)) &&
        (((ACMAJORREV_2(pi->pubpi->phy_rev)) && (PHY_IPA(pi))) ||
        (ACMAJORREV_4(pi->pubpi->phy_rev)))) {
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 1, 0, 0, rstr_pa2gccka0);
        wlc_phy_txpwr_srom11_read_pwrdet(pi, pwrdet, 2, 0, 0, rstr_pa2gccka1);
    }

    /* 5G band */
#if BAND5G
    wlc_phy_txpwr_srom11_read_5Gsubbands(pi, pwrdet, 0, update_rsdb_core1_params, ant);
    wlc_phy_txpwr_srom11_read_5Gsubbands(pi, pwrdet, 1, update_rsdb_core1_params, ant);
    wlc_phy_txpwr_srom11_read_5Gsubbands(pi, pwrdet, 2, update_rsdb_core1_params, ant);
    wlc_phy_txpwr_srom11_read_5Gsubbands(pi, pwrdet, 3, update_rsdb_core1_params, ant);
#endif  /* BAND5G */

    wlc_phy_txpwr_srom11_read_ppr(pi);
    /* read out power detect offset values */
    pwrdet->pdoffset2g40_flag = (uint8)PHY_GETINTVAR_SLICE(pi, rstr_pdoffset2g40mvalid);
    pwrdet->pdoffset40[0] = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_pdoffset40ma0);
    pwrdet->pdoffset80[0] = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_pdoffset80ma0);
    pwrdet->pdoffset5gsubband[0] = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_pdoffset5gsubbanda0);
    pwrdet->pdoffset2g40[0] = (uint8)PHY_GETINTVAR_SLICE(pi, rstr_pdoffset2g40ma0);
    pwrdet->pdoffsetcck[0] = (uint8)PHY_GETINTVAR_SLICE(pi, rstr_pdoffsetcckma0);
    pi->tpci->data->cfg->cckpwroffset[0] = (int8)PHY_GETINTVAR_SLICE(pi, rstr_cckpwroffset0);
    pi->sh->cckPwrIdxCorr = (int8) PHY_GETINTVAR_SLICE(pi, rstr_cckPwrIdxCorr);
    pi->sromi->txpwr2gAdcScale = (uint8) PHY_GETINTVAR_DEFAULT(pi, rstr_txpwr2gAdcScale, -1);
    pi->sromi->txpwr5gAdcScale = (uint8) PHY_GETINTVAR_DEFAULT(pi, rstr_txpwr5gAdcScale, -1);
    if ((PHYCORENUM(pi->pubpi->phy_corenum) > 1) ||
        (update_rsdb_core1_params == TRUE)) {
        pwrdet->pdoffset40[ant] = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_pdoffset40ma1);
        pwrdet->pdoffset80[ant] = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_pdoffset80ma1);
        pwrdet->pdoffset5gsubband[ant] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_pdoffset5gsubbanda1);
        pwrdet->pdoffset2g40[ant] = (uint8)PHY_GETINTVAR_SLICE(pi, rstr_pdoffset2g40ma1);
        pwrdet->pdoffsetcck[ant] = (uint8)PHY_GETINTVAR_SLICE(pi, rstr_pdoffsetcckma1);
        pi->tpci->data->cfg->cckpwroffset[ant] = (int8)PHY_GETINTVAR_SLICE(pi,
            rstr_cckpwroffset1);
    }
    if (PHYCORENUM(pi->pubpi->phy_corenum) > 2) {
        pwrdet->pdoffset40[2] = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_pdoffset40ma2);
        pwrdet->pdoffset80[2] = (uint16)PHY_GETINTVAR_SLICE(pi, rstr_pdoffset80ma2);
        pwrdet->pdoffset2g40[2] = (uint8)PHY_GETINTVAR_SLICE(pi, rstr_pdoffset2g40ma2);
        pwrdet->pdoffsetcck[2] = (uint8)PHY_GETINTVAR_SLICE(pi, rstr_pdoffsetcckma2);
        pi->tpci->data->cfg->cckpwroffset[2] = (int8)PHY_GETINTVAR_SLICE(pi,
            rstr_cckpwroffset2);
    }
    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        if ((phy_get_phymode(pi) == PHYMODE_RSDB) &&
            (phy_get_current_core(pi) == PHY_RSBD_PI_IDX_CORE0)) {
            pi->pubpi->phy_corenum >>= 1;
        }
    }
    pi->phy_tempsense_offset = (int8)PHY_GETINTVAR_SLICE(pi, rstr_tempoffset);
    if (pi->phy_tempsense_offset == -1) {
        pi->phy_tempsense_offset = 0;
    } else if (pi->phy_tempsense_offset != 0) {
        if (pi->phy_tempsense_offset >
            (ACPHY_SROM_TEMPSHIFT + ACPHY_SROM_MAXTEMPOFFSET)) {
            pi->phy_tempsense_offset = ACPHY_SROM_MAXTEMPOFFSET;
        } else if (pi->phy_tempsense_offset < (ACPHY_SROM_TEMPSHIFT +
            ACPHY_SROM_MINTEMPOFFSET)) {
            pi->phy_tempsense_offset = ACPHY_SROM_MINTEMPOFFSET;
        } else {
            pi->phy_tempsense_offset -= ACPHY_SROM_TEMPSHIFT;
        }
    }

    return TRUE;

}

#if defined(WL_EAP_BOARD_RF_5G_FILTER)

#if BAND5G
#ifdef BCA_HNDROUTER /* keep mkname static, only used with BCM949408EAP */
/*
 *  mkname - create a new string, given a starting base (name) and
 *          a trailer (suffix), then store into a given buffer. E.g.:
 *          dest[] = name[]+suffix[]+'\0'
 *  Used to create special nvram names for filtered-mode tssi calibration data
 */
static const char * mkname(char *dest, int maxlen, const char *name, const char *suffix)
{
    if (dest && name && suffix && (maxlen > 0)) {
        strncpy(dest, name, maxlen);
        strncat(dest, suffix, maxlen);
    }
    return (const char*)dest;
}
#endif /* BCA_HNDROUTER * keep mkname static, only used with BCM949408EAP */

/*
 * wl_eap_txpwr_srom12_read_filtered - override srom rev 12 tx power
 * coefficients with values from nvram.
 *
 * Configuration for the BCM949408EAP reference design.
 * BCM4366_D11AC5G_ID port uses a GPIO to enable a 5G band-pass filter.
 * Filter-disabled (by-passed) values are stored in the on-board SPROM;
 * filter-enabled values are stored in nvram, with the extension "-filter"
 * Check if in filter mode, and override the SPROM values, with nvram
 * values here.
 */
static bool
BCMATTACHFN(wl_eap_txpwr_srom12_read_filtered)(phy_info_t *pi, int b, int i, int j)
{
    bool rc = FALSE;
#if defined(BCA_HNDROUTER) /* BCM949408EAP */
    srom12_pwrdet_t *pwrdet = pi->pwrdet_ac;
    #define TSSI_FILTER_SUFFIX "-filter"

    /* If BCM949408EAP in filtered mode, use filter-based data */
    if ((pi->sh->boardtype == 0x0798) &&
        ((BOARD_5G_FILTER_BLOCKS_UNII1 + BOARD_5G_FILTER_BLOCKS_UNII2A) ==
            pi->sh->board5gfilter)) {
        char buf[64];
        int len = sizeof(buf);
        buf[0] = '\0'; // initialize end of string

        /* check for existance of the nvram values */
        if (PHY_GETINTVAR_ARRAY_SLICE(pi,
            mkname(buf, len, rstr_pa5ga0, TSSI_FILTER_SUFFIX), i) == 0) {
            PHY_ERROR(("%s Error - missing TSSI nvrams for filter mode ('%s')\n",
                __FUNCTION__, buf));
            return false;
        }

        /* 5G_BANDS_20MHz */
        pwrdet->pwrdet_a[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
            mkname(buf, len, rstr_pa5ga0, TSSI_FILTER_SUFFIX), i);
        pwrdet->pwrdet_b[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
            mkname(buf, len, rstr_pa5ga0, TSSI_FILTER_SUFFIX), i+1);
        pwrdet->pwrdet_c[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
            mkname(buf, len, rstr_pa5ga0, TSSI_FILTER_SUFFIX), i+2);
        pwrdet->pwrdet_d[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
            mkname(buf, len, rstr_pa5ga0, TSSI_FILTER_SUFFIX), i+3);
        /* 5G_BANDS_40MHz */
        pwrdet->pwrdet_a_40[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
            mkname(buf, len, rstr_pa5g40a0, TSSI_FILTER_SUFFIX), i);
        pwrdet->pwrdet_b_40[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
            mkname(buf, len, rstr_pa5g40a0, TSSI_FILTER_SUFFIX), i+1);
        pwrdet->pwrdet_c_40[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
            mkname(buf, len, rstr_pa5g40a0, TSSI_FILTER_SUFFIX), i+2);
        pwrdet->pwrdet_d_40[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
            mkname(buf, len, rstr_pa5g40a0, TSSI_FILTER_SUFFIX), i+3);
        /* 5G_BANDS_80MHz */
        pwrdet->pwrdet_a_80[0][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
            mkname(buf, len, rstr_pa5g80a0, TSSI_FILTER_SUFFIX), i);
        pwrdet->pwrdet_b_80[0][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
            mkname(buf, len, rstr_pa5g80a0, TSSI_FILTER_SUFFIX), i+1);
        pwrdet->pwrdet_c_80[0][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
            mkname(buf, len, rstr_pa5g80a0, TSSI_FILTER_SUFFIX), i+2);
        pwrdet->pwrdet_d_80[0][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
            mkname(buf, len, rstr_pa5g80a0, TSSI_FILTER_SUFFIX), i+3);
        if (SROMREV(pi->sh->sromrev) >= 18 && j < 4) {
            /* 5G_BANDS_160MHz */
            pwrdet->pwrdet_a_160[0][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g160a0, TSSI_FILTER_SUFFIX), i);
            pwrdet->pwrdet_b_160[0][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g160a0, TSSI_FILTER_SUFFIX), i+1);
            pwrdet->pwrdet_c_160[0][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g160a0, TSSI_FILTER_SUFFIX), i+2);
            pwrdet->pwrdet_d_160[0][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g160a0, TSSI_FILTER_SUFFIX), i+3);
        }

        if (PHYCORENUM(pi->pubpi->phy_corenum) > 1) {
            /* 5G_BANDS_20MHz */
            pwrdet->pwrdet_a[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5ga1, TSSI_FILTER_SUFFIX), i);
            pwrdet->pwrdet_b[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5ga1, TSSI_FILTER_SUFFIX), i+1);
            pwrdet->pwrdet_c[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5ga1, TSSI_FILTER_SUFFIX), i+2);
            pwrdet->pwrdet_d[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5ga1, TSSI_FILTER_SUFFIX), i+3);
            /* 5G_BANDS_40MHz */
            pwrdet->pwrdet_a_40[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g40a1, TSSI_FILTER_SUFFIX), i);
            pwrdet->pwrdet_b_40[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g40a1, TSSI_FILTER_SUFFIX), i+1);
            pwrdet->pwrdet_c_40[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g40a1, TSSI_FILTER_SUFFIX), i+2);
            pwrdet->pwrdet_d_40[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g40a1, TSSI_FILTER_SUFFIX), i+3);
            /* 5G_BANDS_80MHz */
            pwrdet->pwrdet_a_80[1][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g80a1, TSSI_FILTER_SUFFIX), i);
            pwrdet->pwrdet_b_80[1][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g80a1, TSSI_FILTER_SUFFIX), i+1);
            pwrdet->pwrdet_c_80[1][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g80a1, TSSI_FILTER_SUFFIX), i+2);
            pwrdet->pwrdet_d_80[1][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g80a1, TSSI_FILTER_SUFFIX), i+3);
            if (SROMREV(pi->sh->sromrev) >= 18 && j < 4) {
                /* 5G_BANDS_160MHz */
                pwrdet->pwrdet_a_160[1][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    mkname(buf, len, rstr_pa5g160a1, TSSI_FILTER_SUFFIX), i);
                pwrdet->pwrdet_b_160[1][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    mkname(buf, len, rstr_pa5g160a1, TSSI_FILTER_SUFFIX), i+1);
                pwrdet->pwrdet_c_160[1][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    mkname(buf, len, rstr_pa5g160a1, TSSI_FILTER_SUFFIX), i+2);
                pwrdet->pwrdet_d_160[1][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    mkname(buf, len, rstr_pa5g160a1, TSSI_FILTER_SUFFIX), i+3);
            }
        }

        if (PHYCORENUM(pi->pubpi->phy_corenum) > 2) {
            /* 5G_BANDS_20MHz */
            pwrdet->pwrdet_a[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5ga2, TSSI_FILTER_SUFFIX), i);
            pwrdet->pwrdet_b[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5ga2, TSSI_FILTER_SUFFIX), i+1);
            pwrdet->pwrdet_c[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5ga2, TSSI_FILTER_SUFFIX), i+2);
            pwrdet->pwrdet_d[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5ga2, TSSI_FILTER_SUFFIX), i+3);
            /* 5G_BANDS_40MHz */
            pwrdet->pwrdet_a_40[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g40a2, TSSI_FILTER_SUFFIX), i);
            pwrdet->pwrdet_b_40[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g40a2, TSSI_FILTER_SUFFIX), i+1);
            pwrdet->pwrdet_c_40[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g40a2, TSSI_FILTER_SUFFIX), i+2);
            pwrdet->pwrdet_d_40[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g40a2, TSSI_FILTER_SUFFIX), i+3);
            /* 5G_BANDS_80MHz */
            pwrdet->pwrdet_a_80[2][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g80a2, TSSI_FILTER_SUFFIX), i);
            pwrdet->pwrdet_b_80[2][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g80a2, TSSI_FILTER_SUFFIX), i+1);
            pwrdet->pwrdet_c_80[2][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g80a2, TSSI_FILTER_SUFFIX), i+2);
            pwrdet->pwrdet_d_80[2][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                mkname(buf, len, rstr_pa5g80a2, TSSI_FILTER_SUFFIX), i+3);
            if (SROMREV(pi->sh->sromrev) >= 18 && j < 4) {
                /* 5G_BANDS_160MHz */
                pwrdet->pwrdet_a_160[2][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    mkname(buf, len, rstr_pa5g160a2, TSSI_FILTER_SUFFIX), i);
                pwrdet->pwrdet_b_160[2][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    mkname(buf, len, rstr_pa5g160a2, TSSI_FILTER_SUFFIX), i+1);
                pwrdet->pwrdet_c_160[2][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    mkname(buf, len, rstr_pa5g160a2, TSSI_FILTER_SUFFIX), i+2);
                pwrdet->pwrdet_d_160[2][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    mkname(buf, len, rstr_pa5g160a2, TSSI_FILTER_SUFFIX), i+3);
            }
        }
        if (PHYCORENUM(pi->pubpi->phy_corenum) > 3) {
            /* 5G_BANDS_20MHz */
            pwrdet->pwrdet_a[3][b] = (int16)PHY_GETINTVAR_ARRAY(pi,
                mkname(buf, len, rstr_pa5ga3, TSSI_FILTER_SUFFIX), i);
            pwrdet->pwrdet_b[3][b] = (int16)PHY_GETINTVAR_ARRAY(pi,
                mkname(buf, len, rstr_pa5ga3, TSSI_FILTER_SUFFIX), i+1);
            pwrdet->pwrdet_c[3][b] = (int16)PHY_GETINTVAR_ARRAY(pi,
                mkname(buf, len, rstr_pa5ga3, TSSI_FILTER_SUFFIX), i+2);
            pwrdet->pwrdet_d[3][b] = (int16)PHY_GETINTVAR_ARRAY(pi,
                mkname(buf, len, rstr_pa5ga3, TSSI_FILTER_SUFFIX), i+3);
            /* 5G_BANDS_40MHz */
            pwrdet->pwrdet_a_40[3][b] = (int16)PHY_GETINTVAR_ARRAY(pi,
                mkname(buf, len, rstr_pa5g40a3, TSSI_FILTER_SUFFIX), i);
            pwrdet->pwrdet_b_40[3][b] = (int16)PHY_GETINTVAR_ARRAY(pi,
                mkname(buf, len, rstr_pa5g40a3, TSSI_FILTER_SUFFIX), i+1);
            pwrdet->pwrdet_c_40[3][b] = (int16)PHY_GETINTVAR_ARRAY(pi,
                mkname(buf, len, rstr_pa5g40a3, TSSI_FILTER_SUFFIX), i+2);
            pwrdet->pwrdet_d_40[3][b] = (int16)PHY_GETINTVAR_ARRAY(pi,
                mkname(buf, len, rstr_pa5g40a3, TSSI_FILTER_SUFFIX), i+3);
            /* 5G_BANDS_80MHz */
            pwrdet->pwrdet_a_80[3][j] = (int16)PHY_GETINTVAR_ARRAY(pi,
                mkname(buf, len, rstr_pa5g80a3, TSSI_FILTER_SUFFIX), i);
            pwrdet->pwrdet_b_80[3][j] = (int16)PHY_GETINTVAR_ARRAY(pi,
                mkname(buf, len, rstr_pa5g80a3, TSSI_FILTER_SUFFIX), i+1);
            pwrdet->pwrdet_c_80[3][j] = (int16)PHY_GETINTVAR_ARRAY(pi,
                mkname(buf, len, rstr_pa5g80a3, TSSI_FILTER_SUFFIX), i+2);
            pwrdet->pwrdet_d_80[3][j] = (int16)PHY_GETINTVAR_ARRAY(pi,
                mkname(buf, len, rstr_pa5g80a3, TSSI_FILTER_SUFFIX), i+3);
            if (SROMREV(pi->sh->sromrev) >= 18 && j < 4) {
                /* 5G_BANDS_160MHz */
                pwrdet->pwrdet_a_160[3][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    mkname(buf, len, rstr_pa5g160a3, TSSI_FILTER_SUFFIX), i);
                pwrdet->pwrdet_b_160[3][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    mkname(buf, len, rstr_pa5g160a3, TSSI_FILTER_SUFFIX), i+1);
                pwrdet->pwrdet_c_160[3][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    mkname(buf, len, rstr_pa5g160a3, TSSI_FILTER_SUFFIX), i+2);
                pwrdet->pwrdet_d_160[3][j] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    mkname(buf, len, rstr_pa5g160a3, TSSI_FILTER_SUFFIX), i+3);
            }
        }

        rc = true; /* Used filtered values */
    }
#else
    BCM_REFERENCE(pi);
    BCM_REFERENCE(b);
    BCM_REFERENCE(i);
    BCM_REFERENCE(j);
#endif // defined(BCA_HNDROUTER) /* BCM949408EAP */
    return rc;
}
#endif /* BAND5G */
#endif /* WL_EAP_BOARD_RF_5G_FILTER */

static bool BCMATTACHFN(wlc_phy_txpwr_srom12_read)(phy_type_tpc_ctx_t *ctx)
{
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = info->pi;

    srom12_pwrdet_t *pwrdet = pi->pwrdet_ac;
    uint8 b = 0, core;
    if (!(SROMREV(pi->sh->sromrev) < 12)) {
#if BAND5G
        uint8 i = 0, j = 0, maxval, numparams;
#endif

        ASSERT(pi->sh->sromrev >= 12);

        /* read pwrdet params for each band/subband/bandwidth */
        /* 2G_20MHz */
        pwrdet->pwrdet_a[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga0, 0);
        pwrdet->pwrdet_b[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga0, 1);
        pwrdet->pwrdet_c[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga0, 2);
        pwrdet->pwrdet_d[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga0, 3);
        if (pi->sh->sromrev == 18 || ACMAJORREV_128(pi->pubpi->phy_rev)) {
            pwrdet->pwrdet_a[0][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka0, 0);
            pwrdet->pwrdet_b[0][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka0, 1);
            pwrdet->pwrdet_c[0][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka0, 2);
            pwrdet->pwrdet_d[0][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka0, 3);
        }
        /* 2G_40MHz */
        pwrdet->pwrdet_a_40[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a0, 0);
        pwrdet->pwrdet_b_40[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a0, 1);
        pwrdet->pwrdet_c_40[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a0, 2);
        pwrdet->pwrdet_d_40[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a0, 3);

        if (PHYCORENUM(pi->pubpi->phy_corenum) > 1) {
        /* 2G_20MHz */
        pwrdet->pwrdet_a[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga1, 0);
        pwrdet->pwrdet_b[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga1, 1);
        pwrdet->pwrdet_c[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga1, 2);
        pwrdet->pwrdet_d[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga1, 3);
        if (pi->sh->sromrev == 18 || ACMAJORREV_128(pi->pubpi->phy_rev)) {
            pwrdet->pwrdet_a[1][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka1, 0);
            pwrdet->pwrdet_b[1][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka1, 1);
            pwrdet->pwrdet_c[1][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka1, 2);
            pwrdet->pwrdet_d[1][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka1, 3);
        }
        /* 2G_40MHz */
        pwrdet->pwrdet_a_40[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a1, 0);
        pwrdet->pwrdet_b_40[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a1, 1);
        pwrdet->pwrdet_c_40[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a1, 2);
        pwrdet->pwrdet_d_40[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a1, 3);
        }
        if (PHYCORENUM(pi->pubpi->phy_corenum) > 2) {
        /* 2G_20MHz */
        pwrdet->pwrdet_a[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga2, 0);
        pwrdet->pwrdet_b[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga2, 1);
        pwrdet->pwrdet_c[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga2, 2);
        pwrdet->pwrdet_d[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga2, 3);
        if (pi->sh->sromrev == 18) {
            pwrdet->pwrdet_a[2][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka2, 0);
            pwrdet->pwrdet_b[2][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka2, 1);
            pwrdet->pwrdet_c[2][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka2, 2);
            pwrdet->pwrdet_d[2][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka2, 3);
        }
        /* 2G_40MHz */
        pwrdet->pwrdet_a_40[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a2, 0);
        pwrdet->pwrdet_b_40[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a2, 1);
        pwrdet->pwrdet_c_40[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a2, 2);
        pwrdet->pwrdet_d_40[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a2, 3);
        }
        if (PHYCORENUM(pi->pubpi->phy_corenum) > 3) {
        /* 2G_20MHz */
        pwrdet->pwrdet_a[3][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga3, 0);
        pwrdet->pwrdet_b[3][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga3, 1);
        pwrdet->pwrdet_c[3][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga3, 2);
        pwrdet->pwrdet_d[3][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2ga3, 3);
        if (pi->sh->sromrev == 18) {
            pwrdet->pwrdet_a[3][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka3, 0);
            pwrdet->pwrdet_b[3][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka3, 1);
            pwrdet->pwrdet_c[3][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka3, 2);
            pwrdet->pwrdet_d[3][b + CH_2G_GROUP + CH_5G_5BAND] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g20ccka3, 3);
        }
        /* 2G_40MHz */
        pwrdet->pwrdet_a_40[3][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a3, 0);
        pwrdet->pwrdet_b_40[3][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a3, 1);
        pwrdet->pwrdet_c_40[3][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a3, 2);
        pwrdet->pwrdet_d_40[3][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa2g40a3, 3);
        }

        ++b;

#if BAND5G
        numparams = wlc_phy_num_paparams_acphy(pi, TRUE);
        maxval = CH_5G_5BAND * numparams;
        for (i = 0; i < maxval; i = i + numparams) {
        j = b - 1; /* 5G 80 MHz index starts from 0 */

#if defined(WL_EAP_BOARD_RF_5G_FILTER)
        /* Check for special filtered TSSI values from nvram */
        if (wl_eap_txpwr_srom12_read_filtered(pi, b, i, j)) {
            b++; /* done at end of for scope */
            continue;
        }
#endif /* WL_EAP_BOARD_RF_5G_FILTER */

        /* 5G_BANDS_20MHz */
        pwrdet->pwrdet_a[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga0, i);
        pwrdet->pwrdet_b[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga0, i+1);
        pwrdet->pwrdet_c[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga0, i+2);
        pwrdet->pwrdet_d[0][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga0, i+3);

        /* 5G_BANDS_40MHz */
        pwrdet->pwrdet_a_40[0][b] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a0, i);
        pwrdet->pwrdet_b_40[0][b] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a0, i+1);
        pwrdet->pwrdet_c_40[0][b] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a0, i+2);
        pwrdet->pwrdet_d_40[0][b] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a0, i+3);

        /* 5G_BANDS_80MHz */
        pwrdet->pwrdet_a_80[0][j] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a0, i);
        pwrdet->pwrdet_b_80[0][j] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a0, i+1);
        pwrdet->pwrdet_c_80[0][j] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a0, i+2);
        pwrdet->pwrdet_d_80[0][j] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a0, i+3);
        if (SROMREV(pi->sh->sromrev) >= 18 && j < 4) {
            /* 5G_BANDS_160MHz */
            pwrdet->pwrdet_a_160[0][j] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a0, i);
            pwrdet->pwrdet_b_160[0][j] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a0, i+1);
            pwrdet->pwrdet_c_160[0][j] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a0, i+2);
            pwrdet->pwrdet_d_160[0][j] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a0, i+3);
        }

        if (PHYCORENUM(pi->pubpi->phy_corenum) > 1) {
            /* 5G_BANDS_20MHz */
            pwrdet->pwrdet_a[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga1, i);
            pwrdet->pwrdet_b[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga1, i+1);
            pwrdet->pwrdet_c[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga1, i+2);
            pwrdet->pwrdet_d[1][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga1, i+3);
            /* 5G_BANDS_40MHz */
            pwrdet->pwrdet_a_40[1][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a1, i);
            pwrdet->pwrdet_b_40[1][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a1, i+1);
            pwrdet->pwrdet_c_40[1][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a1, i+2);
            pwrdet->pwrdet_d_40[1][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a1, i+3);
            /* 5G_BANDS_80MHz */
            pwrdet->pwrdet_a_80[1][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a1, i);
            pwrdet->pwrdet_b_80[1][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a1, i+1);
            pwrdet->pwrdet_c_80[1][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a1, i+2);
            pwrdet->pwrdet_d_80[1][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a1, i+3);
            if (SROMREV(pi->sh->sromrev) >= 18 && j < 4) {
                /* 5G_BANDS_160MHz */
                pwrdet->pwrdet_a_160[1][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a1, i);
                pwrdet->pwrdet_b_160[1][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a1, i+1);
                pwrdet->pwrdet_c_160[1][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a1, i+2);
                pwrdet->pwrdet_d_160[1][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a1, i+3);
            }
        }

        if (PHYCORENUM(pi->pubpi->phy_corenum) > 2) {
            /* 5G_BANDS_20MHz */
            pwrdet->pwrdet_a[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga2, i);
            pwrdet->pwrdet_b[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga2, i+1);
            pwrdet->pwrdet_c[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga2, i+2);
            pwrdet->pwrdet_d[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga2, i+3);
            /* 5G_BANDS_40MHz */
            pwrdet->pwrdet_a_40[2][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a2, i);
            pwrdet->pwrdet_b_40[2][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a2, i+1);
            pwrdet->pwrdet_c_40[2][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a2, i+2);
            pwrdet->pwrdet_d_40[2][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a2, i+3);
            /* 5G_BANDS_80MHz */
            pwrdet->pwrdet_a_80[2][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a2, i);
            pwrdet->pwrdet_b_80[2][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a2, i+1);
            pwrdet->pwrdet_c_80[2][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a2, i+2);
            pwrdet->pwrdet_d_80[2][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a2, i+3);
            if (SROMREV(pi->sh->sromrev) >= 18 && j < 4) {
                /* 5G_BANDS_160MHz */
                pwrdet->pwrdet_a_160[2][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a2, i);
                pwrdet->pwrdet_b_160[2][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a2, i+1);
                pwrdet->pwrdet_c_160[2][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a2, i+2);
                pwrdet->pwrdet_d_160[2][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a2, i+3);
            }
        }
        if (PHYCORENUM(pi->pubpi->phy_corenum) > 3) {
            /* 5G_BANDS_20MHz */
            pwrdet->pwrdet_a[3][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga3, i);
            pwrdet->pwrdet_b[3][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga3, i+1);
            pwrdet->pwrdet_c[3][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga3, i+2);
            pwrdet->pwrdet_d[3][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5ga3, i+3);
            /* 5G_BANDS_40MHz */
            pwrdet->pwrdet_a_40[3][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a3, i);
            pwrdet->pwrdet_b_40[3][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a3, i+1);
            pwrdet->pwrdet_c_40[3][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a3, i+2);
            pwrdet->pwrdet_d_40[3][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g40a3, i+3);

            /* 5G_BANDS_80MHz */
            pwrdet->pwrdet_a_80[3][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a3, i);
            pwrdet->pwrdet_b_80[3][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a3, i+1);
            pwrdet->pwrdet_c_80[3][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a3, i+2);
            pwrdet->pwrdet_d_80[3][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g80a3, i+3);
            if (SROMREV(pi->sh->sromrev) >= 18 && j < 4) {
                /* 5G_BANDS_160MHz */
                pwrdet->pwrdet_a_160[3][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a3, i);
                pwrdet->pwrdet_b_160[3][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a3, i+1);
                pwrdet->pwrdet_c_160[3][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a3, i+2);
                pwrdet->pwrdet_d_160[3][j] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5g160a3, i+3);
            }
        }
        ++b;
        }

        i = 0;
        pwrdet->max_pwr[0][i++] = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp2gb0a0);
        pwrdet->max_pwr[0][i++] = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb0a0);
        pwrdet->max_pwr[0][i++] = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb1a0);
        pwrdet->max_pwr[0][i++] = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb2a0);
        pwrdet->max_pwr[0][i++] = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb3a0);
        pwrdet->max_pwr[0][i++] = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb4a0);

        if (PHYCORENUM(pi->pubpi->phy_corenum) > 1) {
        i = 0;
        pwrdet->max_pwr[1][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp2gb0a1);
        pwrdet->max_pwr[1][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb0a1);
        pwrdet->max_pwr[1][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb1a1);
        pwrdet->max_pwr[1][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb2a1);
        pwrdet->max_pwr[1][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb3a1);
        pwrdet->max_pwr[1][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb4a1);
        }

        if (PHYCORENUM(pi->pubpi->phy_corenum) > 2) {
        i = 0;
        pwrdet->max_pwr[2][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp2gb0a2);
        pwrdet->max_pwr[2][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb0a2);
        pwrdet->max_pwr[2][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb1a2);
        pwrdet->max_pwr[2][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb2a2);
        pwrdet->max_pwr[2][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb3a2);
        pwrdet->max_pwr[2][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb4a2);
        }
        if (PHYCORENUM(pi->pubpi->phy_corenum) > 3) {
        i = 0;
        pwrdet->max_pwr[3][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp2gb0a3);
        pwrdet->max_pwr[3][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb0a3);
        pwrdet->max_pwr[3][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb1a3);
        pwrdet->max_pwr[3][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb2a3);
        pwrdet->max_pwr[3][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb3a3);
        pwrdet->max_pwr[3][i++]  = (int8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb4a3);
        }
#endif /* BAND5G */
    }

    if (SROMREV(pi->sh->sromrev) < 13) {
        wlc_phy_txpwr_srom12_read_ppr(pi);
    }
#ifdef WL11AC
    else {
        wlc_phy_txpwr_srom13_read_ppr(pi);
    }
#endif /* WL11AC */

    if (!(SROMREV(pi->sh->sromrev) < 12)) {
        /* read out power detect offset values */
        FOREACH_CORE(pi, core) {
            /* 0 is reserved for 2G band, which BW80/160 are not supported */
            pwrdet->pdoffset20in80[core][0] = 0;
            pwrdet->pdoffset40in80[core][0] = 0;
            pwrdet->pdoffset20in160[core][0] = 0;
            pwrdet->pdoffset40in160[core][0] = 0;
            pwrdet->pdoffset80in160[core][0] = 0;
            if (core < 3) {
                pwrdet->pdoffsetcck[core] =
                    PDOFFSET(pi, rstr_pdoffset2gcck, core);
                pwrdet->pdoffsetcck20m[core] =
                    PDOFFSET(pi, rstr_pdoffset2gcck20m, core);
                pwrdet->pdoffset20in40[core][0] =
                    PDOFFSET(pi, rstr_pdoffset20in40m2g, core);
                pwrdet->pdoffset20in40[core][1] =
                    PDOFFSET(pi, rstr_pdoffset20in40m5gb0, core);
                pwrdet->pdoffset20in40[core][2] =
                    PDOFFSET(pi, rstr_pdoffset20in40m5gb1, core);
                pwrdet->pdoffset20in40[core][3] =
                    PDOFFSET(pi, rstr_pdoffset20in40m5gb2, core);
                pwrdet->pdoffset20in40[core][4] =
                    PDOFFSET(pi, rstr_pdoffset20in40m5gb3, core);
                pwrdet->pdoffset20in40[core][5] =
                    PDOFFSET(pi, rstr_pdoffset20in40m5gb4, core);
                pwrdet->pdoffset20in80[core][1] =
                    PDOFFSET(pi, rstr_pdoffset20in80m5gb0, core);
                pwrdet->pdoffset20in80[core][2] =
                    PDOFFSET(pi, rstr_pdoffset20in80m5gb1, core);
                pwrdet->pdoffset20in80[core][3] =
                    PDOFFSET(pi, rstr_pdoffset20in80m5gb2, core);
                pwrdet->pdoffset20in80[core][4] =
                    PDOFFSET(pi, rstr_pdoffset20in80m5gb3, core);
                pwrdet->pdoffset20in80[core][5] =
                    PDOFFSET(pi, rstr_pdoffset20in80m5gb4, core);
                pwrdet->pdoffset40in80[core][1] =
                    PDOFFSET(pi, rstr_pdoffset40in80m5gb0, core);
                pwrdet->pdoffset40in80[core][2] =
                    PDOFFSET(pi, rstr_pdoffset40in80m5gb1, core);
                pwrdet->pdoffset40in80[core][3] =
                    PDOFFSET(pi, rstr_pdoffset40in80m5gb2, core);
                pwrdet->pdoffset40in80[core][4] =
                    PDOFFSET(pi, rstr_pdoffset40in80m5gb3, core);
                pwrdet->pdoffset40in80[core][5] =
                    PDOFFSET(pi, rstr_pdoffset40in80m5gb4, core);
                if (SROMREV(pi->sh->sromrev) >= 18) {
                    pwrdet->pdoffset20in160[core][1] =
                    PDOFFSET(pi, rstr_pdoffset20in160m5gc0, core);
                    pwrdet->pdoffset20in160[core][2] =
                    PDOFFSET(pi, rstr_pdoffset20in160m5gc1, core);
                    pwrdet->pdoffset20in160[core][3] =
                    PDOFFSET(pi, rstr_pdoffset20in160m5gc2, core);
                    pwrdet->pdoffset20in160[core][4] =
                    PDOFFSET(pi, rstr_pdoffset20in160m5gc3, core);
                    pwrdet->pdoffset40in160[core][1] =
                    PDOFFSET(pi, rstr_pdoffset40in160m5gc0, core);
                    pwrdet->pdoffset40in160[core][2] =
                    PDOFFSET(pi, rstr_pdoffset40in160m5gc1, core);
                    pwrdet->pdoffset40in160[core][3] =
                    PDOFFSET(pi, rstr_pdoffset40in160m5gc2, core);
                    pwrdet->pdoffset40in160[core][4] =
                    PDOFFSET(pi, rstr_pdoffset40in160m5gc3, core);
                    pwrdet->pdoffset80in160[core][1] =
                    PDOFFSET(pi, rstr_pdoffset80in160m5gc0, core);
                    pwrdet->pdoffset80in160[core][2] =
                    PDOFFSET(pi, rstr_pdoffset80in160m5gc1, core);
                    pwrdet->pdoffset80in160[core][3] =
                    PDOFFSET(pi, rstr_pdoffset80in160m5gc2, core);
                    pwrdet->pdoffset80in160[core][4] =
                    PDOFFSET(pi, rstr_pdoffset80in160m5gc3, core);
                    // index 0 is rsvd for 2G, hence safe to use under phybw160
                    pwrdet->pdoffset20in160[core][0] =
                    PDOFFSET(pi, rstr_pdoffset20in160m5gc4, core);
                    pwrdet->pdoffset40in160[core][0] =
                    PDOFFSET(pi, rstr_pdoffset40in160m5gc4, core);
                    pwrdet->pdoffset80in160[core][0] =
                    PDOFFSET(pi, rstr_pdoffset80in160m5gc4, core);
                }
            } else {
                pwrdet->pdoffsetcck[core] =
                    PDOFFSET(pi, rstr_pdoffset20in40m2gcore3, 1);
                pwrdet->pdoffsetcck20m[core] =
                    PDOFFSET(pi, rstr_pdoffset20in40m2gcore3, 2);
                pwrdet->pdoffset20in40[core][0] =
                    PDOFFSET(pi, rstr_pdoffset20in40m2gcore3, 0);
                pwrdet->pdoffset20in40[core][1] =
                    PDOFFSET(pi, rstr_pdoffset20in40m5gcore3, 0);
                pwrdet->pdoffset20in40[core][2] =
                    PDOFFSET(pi, rstr_pdoffset20in40m5gcore3, 1);
                pwrdet->pdoffset20in40[core][3] =
                    PDOFFSET(pi, rstr_pdoffset20in40m5gcore3, 2);
                pwrdet->pdoffset20in40[core][4] =
                    PDOFFSET(pi, rstr_pdoffset20in40m5gcore3_1, 0);
                pwrdet->pdoffset20in40[core][5] =
                    PDOFFSET(pi, rstr_pdoffset20in40m5gcore3_1, 1);
                pwrdet->pdoffset20in80[core][1] =
                    PDOFFSET(pi, rstr_pdoffset20in80m5gcore3, 0);
                pwrdet->pdoffset20in80[core][2] =
                    PDOFFSET(pi, rstr_pdoffset20in80m5gcore3, 1);
                pwrdet->pdoffset20in80[core][3] =
                    PDOFFSET(pi, rstr_pdoffset20in80m5gcore3, 2);
                pwrdet->pdoffset20in80[core][4] =
                    PDOFFSET(pi, rstr_pdoffset20in80m5gcore3_1, 0);
                pwrdet->pdoffset20in80[core][5] =
                    PDOFFSET(pi, rstr_pdoffset20in80m5gcore3_1, 1);
                pwrdet->pdoffset40in80[core][1] =
                    PDOFFSET(pi, rstr_pdoffset40in80m5gcore3, 0);
                pwrdet->pdoffset40in80[core][2] =
                    PDOFFSET(pi, rstr_pdoffset40in80m5gcore3, 1);
                pwrdet->pdoffset40in80[core][3] =
                    PDOFFSET(pi, rstr_pdoffset40in80m5gcore3, 2);
                pwrdet->pdoffset40in80[core][4] =
                    PDOFFSET(pi, rstr_pdoffset40in80m5gcore3_1, 0);
                pwrdet->pdoffset40in80[core][5] =
                    PDOFFSET(pi, rstr_pdoffset40in80m5gcore3_1, 1);
                if (SROMREV(pi->sh->sromrev) >= 18) {
                    pwrdet->pdoffset20in160[core][1] =
                        PDOFFSET(pi, rstr_pdoffset20in160m5gcore3, 0);
                    pwrdet->pdoffset20in160[core][2] =
                        PDOFFSET(pi, rstr_pdoffset20in160m5gcore3, 1);
                    pwrdet->pdoffset20in160[core][3] =
                        PDOFFSET(pi, rstr_pdoffset20in160m5gcore3_1, 0);
                    pwrdet->pdoffset20in160[core][4] =
                        PDOFFSET(pi, rstr_pdoffset20in160m5gcore3_1, 1);
                    pwrdet->pdoffset40in160[core][1] =
                        PDOFFSET(pi, rstr_pdoffset40in160m5gcore3, 0);
                    pwrdet->pdoffset40in160[core][2] =
                        PDOFFSET(pi, rstr_pdoffset40in160m5gcore3, 1);
                    pwrdet->pdoffset40in160[core][3] =
                        PDOFFSET(pi, rstr_pdoffset40in160m5gcore3_1, 0);
                    pwrdet->pdoffset40in160[core][4] =
                        PDOFFSET(pi, rstr_pdoffset40in160m5gcore3_1, 1);
                    pwrdet->pdoffset80in160[core][1] =
                        PDOFFSET(pi, rstr_pdoffset80in160m5gcore3, 0);
                    pwrdet->pdoffset80in160[core][2] =
                        PDOFFSET(pi, rstr_pdoffset80in160m5gcore3, 1);
                    pwrdet->pdoffset80in160[core][3] =
                        PDOFFSET(pi, rstr_pdoffset80in160m5gcore3_1, 0);
                    pwrdet->pdoffset80in160[core][4] =
                        PDOFFSET(pi, rstr_pdoffset80in160m5gcore3_1, 1);
                    // index 0 is rsvd for 2G, hence safe to use under phybw160
                    pwrdet->pdoffset20in160[core][0] =
                        PDOFFSET(pi, rstr_pdoffset20in160m5gcore3_2, 0);
                    pwrdet->pdoffset40in160[core][0] =
                        PDOFFSET(pi, rstr_pdoffset40in160m5gcore3_2, 0);
                    pwrdet->pdoffset80in160[core][0] =
                        PDOFFSET(pi, rstr_pdoffset80in160m5gcore3_2, 0);
                }
            }
            // read pdoffsetcck for ch14
            if (SROMREV(pi->sh->sromrev) >= 18) {
                pwrdet->pdoffsetcckch14[core] = (uint8) (PHY_GETINTVAR(pi,
                        rstr_pdoffset2gcckch14) >> (4 * core)) & 0xf;
            }

            // read ppr backof from srom12
            if (SROMREV(pi->sh->sromrev) == 12) {
                pwrdet->pwr_backoff[0] = 2*(uint8)PHY_GETINTVAR_DEFAULT_SLICE(pi,
                    rstr_ppr_backoff_2g, 3);
                pwrdet->pwr_backoff[1] = 2*(uint8)PHY_GETINTVAR_DEFAULT_SLICE(pi,
                    rstr_ppr_backoff_5g, 3);
            }
            // read ppr backof from srom18
            if (SROMREV(pi->sh->sromrev) == 18) {
                pwrdet->pwr_backoff[0] = 2*(uint8)PHY_GETINTVAR_DEFAULT_SLICE(pi,
                    rstr_ppr_backoff_2g, 3);
                pwrdet->pwr_backoff[1] = 2*(uint8)PHY_GETINTVAR_DEFAULT_SLICE(pi,
                    rstr_ppr_backoff_5g, 3);
                PHY_INFORM(("Get SROM 18 RU Power Offset per rate\n"));
                pi->ppr->ru.sr18_ruppr.offset_2g_ru106[0] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru106ppr_2g_0);
                pi->ppr->ru.sr18_ruppr.offset_2g_ru106[1] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru106ppr_2g_1);
                pi->ppr->ru.sr18_ruppr.offset_2g_ru106[2] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru106ppr_2g_2);
                pi->ppr->ru.sr18_ruppr.offset_2g_ru242[0] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru242ppr_2g_0);
                pi->ppr->ru.sr18_ruppr.offset_2g_ru242[1] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru242ppr_2g_1);
                pi->ppr->ru.sr18_ruppr.offset_2g_ru242[2] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru242ppr_2g_2);
                pi->ppr->ru.sr18_ruppr.offset_5g_ru106[0] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru106ppr_5g_0);
                pi->ppr->ru.sr18_ruppr.offset_5g_ru106[1] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru106ppr_5g_1);
                pi->ppr->ru.sr18_ruppr.offset_5g_ru106[2] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru106ppr_5g_2);
                pi->ppr->ru.sr18_ruppr.offset_5g_ru242[0] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru242ppr_5g_0);
                pi->ppr->ru.sr18_ruppr.offset_5g_ru242[1] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru242ppr_5g_1);
                pi->ppr->ru.sr18_ruppr.offset_5g_ru242[2] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru242ppr_5g_2);
                pi->ppr->ru.sr18_ruppr.offset_5g_ru484[0] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru484ppr_5g_0);
                pi->ppr->ru.sr18_ruppr.offset_5g_ru484[1] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru484ppr_5g_1);
                pi->ppr->ru.sr18_ruppr.offset_5g_ru484[2] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru484ppr_5g_2);
                pi->ppr->ru.sr18_ruppr.offset_5g_ru996[0] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru996ppr_5g_0);
                pi->ppr->ru.sr18_ruppr.offset_5g_ru996[1] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru996ppr_5g_1);
                pi->ppr->ru.sr18_ruppr.offset_5g_ru996[2] =
                    (uint16)PHY_GETINTVAR_SLICE(pi, rstr_ru996ppr_5g_2);
            }
        }

        pi->phy_tempsense_offset = (int8)PHY_GETINTVAR_SLICE(pi, rstr_tempoffset);
        if (pi->phy_tempsense_offset < 0) {
        pi->phy_tempsense_offset = 0;
        } else if (pi->phy_tempsense_offset != 0) {
        if (pi->phy_tempsense_offset >
            (ACPHY_SROM_TEMPSHIFT + ACPHY_SROM_MAXTEMPOFFSET)) {
            pi->phy_tempsense_offset = ACPHY_SROM_MAXTEMPOFFSET;
        } else if (pi->phy_tempsense_offset < (ACPHY_SROM_TEMPSHIFT +
                                               ACPHY_SROM_MINTEMPOFFSET)) {
            pi->phy_tempsense_offset = ACPHY_SROM_MINTEMPOFFSET;
        } else {
            pi->phy_tempsense_offset -= ACPHY_SROM_TEMPSHIFT;
        }
        }

    }
    return TRUE;
}
#if BAND6G
static void
BCMATTACHFN(wlc_phy_txpwr_srom18_read_5gextppr)(phy_info_t *pi)
{
    if (SROMREV(pi->sh->sromrev) >= 18) {
        /* Read and interpret the power-offset parameters from the SROM for each
         *  band/subband
         */

        PHY_INFORM(("Get SROM 18 Subband5&6 Power Offset per rate\n"));

        /* ---------------5G--------------- */
        /* 5G 11agnac_20IN20 */
        pi->ppr->uext.sr18_5gext.ofdm_5g.bw20[0] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw205gx3po);
        pi->ppr->uext.sr18_5gext.ofdm_5g.bw20[1] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw205gx4po);

        pi->ppr->uext.sr18_5gext.offset_5g[0] = (uint16)PHY_GETINTVAR_SLICE(pi,
            rstr_mcslr5gx3po);
        pi->ppr->uext.sr18_5gext.offset_5g[1] = (uint16)PHY_GETINTVAR_SLICE(pi,
            rstr_mcslr5gx4po);

        /* 5G 11nac 40IN40 */
        pi->ppr->uext.sr18_5gext.mcs_5g.bw40[0] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw405gx3po);
        pi->ppr->uext.sr18_5gext.mcs_5g.bw40[1] =
                (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw405gx4po);

        /* 5G 11nac 80IN80 */
        pi->ppr->uext.sr18_5gext.mcs_5g.bw80[0] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw805gx3po);
        pi->ppr->uext.sr18_5gext.mcs_5g.bw80[1] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw805gx4po);

        pi->ppr->uext.sr18_5gext.offset_20in80_l[0] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in80and160lr5gx3po);
        pi->ppr->uext.sr18_5gext.offset_20in80_h[0] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in80and160hr5gx3po);
        pi->ppr->uext.sr18_5gext.offset_20in80_l[1] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in80and160lr5gx4po);
        pi->ppr->uext.sr18_5gext.offset_20in80_h[1] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb20in80and160hr5gx4po);

        pi->ppr->uext.sr18_5gext.offset_40in80_l[0] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb40and80lr5gx3po);
        pi->ppr->uext.sr18_5gext.offset_40in80_h[0] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb40and80hr5gx3po);
        pi->ppr->uext.sr18_5gext.offset_40in80_l[1] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb40and80lr5gx4po);
        pi->ppr->uext.sr18_5gext.offset_40in80_h[1] =
            (uint16)PHY_GETINTVAR_SLICE(pi, rstr_sb40and80hr5gx4po);

        pi->ppr->uext.sr18_5gext.mcs_5g.bw160[0] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw1605gx3po);
        pi->ppr->uext.sr18_5gext.mcs_5g.bw160[1] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcsbw1605gx4po);

        pi->ppr->uext.sr18_5gext.pp1024qam5g[0] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcs1024qam5gx3po);
        pi->ppr->uext.sr18_5gext.pp1024qam5g[1] =
            (uint32)PHY_GETINTVAR_SLICE(pi, rstr_mcs1024qam5gx4po);

    }
}

static bool BCMATTACHFN(wlc_phy_txpwr_5gext_srom18_read)(phy_type_tpc_ctx_t *ctx)
{
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = info->pi;
    srom18_5gext_pwrdet_t *pwrdet_5gext = pi->pwrdet_ac_5gext;
    uint8 b = 0, core;
    uint8 i = 0, maxval = 0, maxval_n160 = 0, numparams;
    ASSERT(pi->sh->sromrev >= 18);

    numparams = wlc_phy_num_paparams_acphy(pi, TRUE);
    maxval = (CH_6G_BANDEXT + 1) * numparams;
    maxval_n160 = (CH_6G_BANDEXT) * numparams;

    /* read pwrdet params for each band/subband/bandwidth */
    for (i = 0; i < maxval; i = i + numparams) {
        if (i < maxval_n160) {
            /* 6GEXT_BANDS_20MHz */
            pwrdet_5gext->pwrdet_a[0][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gexta0, i);
            pwrdet_5gext->pwrdet_b[0][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gexta0, i+1);
            pwrdet_5gext->pwrdet_c[0][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gexta0, i+2);
            pwrdet_5gext->pwrdet_d[0][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gexta0, i+3);
            /* 6GEXT_BANDS_40MHz */
            pwrdet_5gext->pwrdet_a_40[0][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a0, i);
            pwrdet_5gext->pwrdet_b_40[0][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a0, i+1);
            pwrdet_5gext->pwrdet_c_40[0][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a0, i+2);
            pwrdet_5gext->pwrdet_d_40[0][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a0, i+3);

            /* 6GEXT_BANDS_80MHz */
            pwrdet_5gext->pwrdet_a_80[0][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a0, i);
            pwrdet_5gext->pwrdet_b_80[0][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a0, i+1);
            pwrdet_5gext->pwrdet_c_80[0][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a0, i+2);
            pwrdet_5gext->pwrdet_d_80[0][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a0, i+3);
        }
        /* 5G_BANDS_160MHz */
        pwrdet_5gext->pwrdet_a_160[0][b] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a0, i);
        pwrdet_5gext->pwrdet_b_160[0][b] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a0, i+1);
        pwrdet_5gext->pwrdet_c_160[0][b] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a0, i+2);
        pwrdet_5gext->pwrdet_d_160[0][b] =
            (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a0, i+3);

        if (PHYCORENUM(pi->pubpi->phy_corenum) > 1) {
            if (i < maxval_n160) {
                /* 6GEXT_BANDS_20MHz */
                pwrdet_5gext->pwrdet_a[1][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gexta1, i);
                pwrdet_5gext->pwrdet_b[1][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gexta1, i+1);
                pwrdet_5gext->pwrdet_c[1][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gexta1, i+2);
                pwrdet_5gext->pwrdet_d[1][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gexta1, i+3);
                /* 6GEXT_BANDS_40MHz */
                pwrdet_5gext->pwrdet_a_40[1][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a1, i);
                pwrdet_5gext->pwrdet_b_40[1][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a1, i+1);
                pwrdet_5gext->pwrdet_c_40[1][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a1, i+2);
                pwrdet_5gext->pwrdet_d_40[1][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a1, i+3);
                /* 6GEXT_BANDS_80MHz */
                pwrdet_5gext->pwrdet_a_80[1][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a1, i);
                pwrdet_5gext->pwrdet_b_80[1][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a1, i+1);
                pwrdet_5gext->pwrdet_c_80[1][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a1, i+2);
                pwrdet_5gext->pwrdet_d_80[1][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a1, i+3);
            }
            /* 6GEXT_BANDS_160MHz */
            pwrdet_5gext->pwrdet_a_160[1][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a1, i);
            pwrdet_5gext->pwrdet_b_160[1][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a1, i+1);
            pwrdet_5gext->pwrdet_c_160[1][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a1, i+2);
            pwrdet_5gext->pwrdet_d_160[1][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a1, i+3);
        }

        if (PHYCORENUM(pi->pubpi->phy_corenum) > 2) {
            if (i < maxval_n160) {
                /* 6GEXT_BANDS_20MHz */
                pwrdet_5gext->pwrdet_a[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    rstr_pa5gexta2, i);
                pwrdet_5gext->pwrdet_b[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    rstr_pa5gexta2, i+1);
                pwrdet_5gext->pwrdet_c[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    rstr_pa5gexta2, i+2);
                pwrdet_5gext->pwrdet_d[2][b] = (int16)PHY_GETINTVAR_ARRAY_SLICE(pi,
                    rstr_pa5gexta2, i+3);
                /* 6GEXT_BANDS_40MHz */
                pwrdet_5gext->pwrdet_a_40[2][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a2, i);
                pwrdet_5gext->pwrdet_b_40[2][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a2, i+1);
                pwrdet_5gext->pwrdet_c_40[2][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a2, i+2);
                pwrdet_5gext->pwrdet_d_40[2][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a2, i+3);
                /* 6GEXT_BANDS_80MHz */
                pwrdet_5gext->pwrdet_a_80[2][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a2, i);
                pwrdet_5gext->pwrdet_b_80[2][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a2, i+1);
                pwrdet_5gext->pwrdet_c_80[2][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a2, i+2);
                pwrdet_5gext->pwrdet_d_80[2][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a2, i+3);
            }
            /* 6GEXT_BANDS_160MHz */
            pwrdet_5gext->pwrdet_a_160[2][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a2, i);
            pwrdet_5gext->pwrdet_b_160[2][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a2, i+1);
            pwrdet_5gext->pwrdet_c_160[2][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a2, i+2);
            pwrdet_5gext->pwrdet_d_160[2][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a2, i+3);
        }
        if (PHYCORENUM(pi->pubpi->phy_corenum) > 3) {
            if (i < maxval_n160) {
                /* 6GEXT_BANDS_20MHz */
                pwrdet_5gext->pwrdet_a[3][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gexta3, i);
                pwrdet_5gext->pwrdet_b[3][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gexta3, i+1);
                pwrdet_5gext->pwrdet_c[3][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gexta3, i+2);
                pwrdet_5gext->pwrdet_d[3][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gexta3, i+3);
                /* 6GEXT_BANDS_40MHz */
                pwrdet_5gext->pwrdet_a_40[3][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a3, i);
                pwrdet_5gext->pwrdet_b_40[3][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a3, i+1);
                pwrdet_5gext->pwrdet_c_40[3][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a3, i+2);
                pwrdet_5gext->pwrdet_d_40[3][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext40a3, i+3);
                /* 6GEXT_BANDS_80MHz */
                pwrdet_5gext->pwrdet_a_80[3][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a3, i);
                pwrdet_5gext->pwrdet_b_80[3][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a3, i+1);
                pwrdet_5gext->pwrdet_c_80[3][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a3, i+2);
                pwrdet_5gext->pwrdet_d_80[3][b] =
                    (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext80a3, i+3);
            }
            /* 6GEXT_BANDS_160MHz */
            pwrdet_5gext->pwrdet_a_160[3][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a3, i);
            pwrdet_5gext->pwrdet_b_160[3][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a3, i+1);
            pwrdet_5gext->pwrdet_c_160[3][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a3, i+2);
            pwrdet_5gext->pwrdet_d_160[3][b] =
                (int16)PHY_GETINTVAR_ARRAY_SLICE(pi, rstr_pa5gext160a3, i+3);
        }
        ++b;
    }

    i = 0;
    pwrdet_5gext->max_pwr[0][i++] = (uint8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb5a0);
    pwrdet_5gext->max_pwr[0][i++] = (uint8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb6a0);
    if (PHYCORENUM(pi->pubpi->phy_corenum) > 1) {
        i = 0;
        pwrdet_5gext->max_pwr[1][i++]  = (uint8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb5a1);
        pwrdet_5gext->max_pwr[1][i++]  = (uint8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb6a1);
    }

    if (PHYCORENUM(pi->pubpi->phy_corenum) > 2) {
        i = 0;
        pwrdet_5gext->max_pwr[2][i++]  = (uint8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb5a2);
        pwrdet_5gext->max_pwr[2][i++]  = (uint8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb6a2);
    }
    if (PHYCORENUM(pi->pubpi->phy_corenum) > 3) {
        i = 0;
        pwrdet_5gext->max_pwr[3][i++]  = (uint8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb5a3);
        pwrdet_5gext->max_pwr[3][i++]  = (uint8)PHY_GETINTVAR_SLICE(pi, rstr_maxp5gb6a3);
    }
    //FIXME overwrite maxpower to 25dBm when it is not programmed in srom
    FOREACH_CORE(pi, core) {
        if (pwrdet_5gext->max_pwr[core][0] == 255) {
            pwrdet_5gext->max_pwr[core][0] = 100;
        }
        if (pwrdet_5gext->max_pwr[core][1] == 255) {
            pwrdet_5gext->max_pwr[core][1] = 100;
        }
    }

    wlc_phy_txpwr_srom18_read_5gextppr(pi);
    /* read out power detect offset values */
    FOREACH_CORE(pi, core) {
        /* 0 is reserved for 2G band, which BW80/160 are not supported */
        pwrdet_5gext->pdoffset20in80[core][0] = 0;
        pwrdet_5gext->pdoffset40in80[core][0] = 0;
        pwrdet_5gext->pdoffset20in160[core][0] = 0;
        pwrdet_5gext->pdoffset40in160[core][0] = 0;
        pwrdet_5gext->pdoffset80in160[core][0] = 0;
        if (core < 3) {
            pwrdet_5gext->pdoffset20in40[core][0] =
                PDOFFSET(pi, rstr_pdoffset20in40m5gb5, core);
            pwrdet_5gext->pdoffset20in40[core][1] =
                PDOFFSET(pi, rstr_pdoffset20in40m5gb6, core);
            pwrdet_5gext->pdoffset20in80[core][0] =
                PDOFFSET(pi, rstr_pdoffset20in80m5gb5, core);
            pwrdet_5gext->pdoffset20in80[core][1] =
                PDOFFSET(pi, rstr_pdoffset20in80m5gb6, core);
            pwrdet_5gext->pdoffset40in80[core][0] =
                PDOFFSET(pi, rstr_pdoffset40in80m5gb5, core);
            pwrdet_5gext->pdoffset40in80[core][1] =
                PDOFFSET(pi, rstr_pdoffset40in80m5gb6, core);
            pwrdet_5gext->pdoffset20in160[core][0] =
                PDOFFSET(pi, rstr_pdoffset20in160m5gc4, core);
            pwrdet_5gext->pdoffset20in160[core][1] =
                PDOFFSET(pi, rstr_pdoffset20in160m5gc5, core);
            pwrdet_5gext->pdoffset20in160[core][2] =
                PDOFFSET(pi, rstr_pdoffset20in160m5gc6, core);
            pwrdet_5gext->pdoffset40in160[core][0] =
                PDOFFSET(pi, rstr_pdoffset40in160m5gc4, core);
            pwrdet_5gext->pdoffset40in160[core][1] =
                PDOFFSET(pi, rstr_pdoffset40in160m5gc5, core);
            pwrdet_5gext->pdoffset40in160[core][2] =
                PDOFFSET(pi, rstr_pdoffset40in160m5gc6, core);
            pwrdet_5gext->pdoffset80in160[core][0] =
                PDOFFSET(pi, rstr_pdoffset80in160m5gc4, core);
            pwrdet_5gext->pdoffset80in160[core][1] =
                PDOFFSET(pi, rstr_pdoffset80in160m5gc5, core);
            pwrdet_5gext->pdoffset80in160[core][2] =
                PDOFFSET(pi, rstr_pdoffset80in160m5gc6, core);
        } else {
            pwrdet_5gext->pdoffset20in40[core][0] =
                PDOFFSET(pi, rstr_pdoffset20in40m5gcore3_2, 0);
            pwrdet_5gext->pdoffset20in40[core][1] =
                PDOFFSET(pi, rstr_pdoffset20in40m5gcore3_2, 1);
            pwrdet_5gext->pdoffset20in80[core][0] =
                PDOFFSET(pi, rstr_pdoffset20in80m5gcore3_2, 0);
            pwrdet_5gext->pdoffset20in80[core][1] =
                PDOFFSET(pi, rstr_pdoffset20in80m5gcore3_2, 1);
            pwrdet_5gext->pdoffset40in80[core][0] =
                PDOFFSET(pi, rstr_pdoffset40in80m5gcore3_2, 0);
            pwrdet_5gext->pdoffset40in80[core][1] =
                PDOFFSET(pi, rstr_pdoffset40in80m5gcore3_2, 1);
            pwrdet_5gext->pdoffset20in160[core][0] =
                    PDOFFSET(pi, rstr_pdoffset20in160m5gcore3_2, 0);
            pwrdet_5gext->pdoffset20in160[core][1] =
                    PDOFFSET(pi, rstr_pdoffset20in160m5gcore3_2, 1);
            pwrdet_5gext->pdoffset20in160[core][2] =
                    PDOFFSET(pi, rstr_pdoffset20in160m5gcore3_2, 2);
            pwrdet_5gext->pdoffset40in160[core][0] =
                    PDOFFSET(pi, rstr_pdoffset40in160m5gcore3_2, 0);
            pwrdet_5gext->pdoffset40in160[core][1] =
                    PDOFFSET(pi, rstr_pdoffset40in160m5gcore3_2, 1);
            pwrdet_5gext->pdoffset40in160[core][2] =
                    PDOFFSET(pi, rstr_pdoffset40in160m5gcore3_2, 2);
            pwrdet_5gext->pdoffset80in160[core][0] =
                    PDOFFSET(pi, rstr_pdoffset80in160m5gcore3_2, 0);
            pwrdet_5gext->pdoffset80in160[core][1] =
                    PDOFFSET(pi, rstr_pdoffset80in160m5gcore3_2, 1);
            pwrdet_5gext->pdoffset80in160[core][2] =
                    PDOFFSET(pi, rstr_pdoffset80in160m5gcore3_2, 2);
        }
    }
    return TRUE;
}
#endif /* BAND6G */
#if defined(WLTEST)
void
wlc_phy_iovar_patrim_acphy(phy_info_t *pi, int32 *ret_int_ptr)
{
    if ((ACMAJORREV_2(pi->pubpi->phy_rev) || (ACMAJORREV_4(pi->pubpi->phy_rev))) &&
        BF3_TSSI_DIV_WAR(pi->u.pi_acphy)) {
        if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
            if (CHSPEC_IS20(pi->radio_chanspec)) {
                *ret_int_ptr = 0x0;
            }
            else {
                *ret_int_ptr = 0x3;
            }
        }
        else {
            if (ACMAJORREV_4(pi->pubpi->phy_rev))
                *ret_int_ptr = 0x14;
            else
                *ret_int_ptr = 0x0;
        }
    } else if ((SROMREV(pi->sh->sromrev) >= 18 || ACMAJORREV_128(pi->pubpi->phy_rev)) &&
        pi->sromi->sr18_cck_paparam_en &&
        (CHSPEC_IS2G(pi->radio_chanspec))) {
            *ret_int_ptr = 0x14;
    } else
        *ret_int_ptr = 0x0;
}

void
wlc_phy_txpwr_ovrinitbaseidx(phy_info_t *pi)
{
    uint8 core, ovrval;
    phy_tpc_data_t *data = pi->tpci->data;
    ovrval = CHSPEC_IS2G(pi->radio_chanspec) ?
        data->cfg->initbaseidx2govrval : data->cfg->initbaseidx5govrval;
    if (ovrval != 255) {
        FOREACH_CORE(pi, core) {
            data->base_index_init[core] = ovrval;
            wlc_phy_txpwrctrl_set_baseindex(pi, core,
                data->base_index_init[core], 1);
        }
    }
}
#endif

void
wlc_phy_get_paparams_for_band_acphy(phy_info_t *pi, int16 *a1, int16 *b0, int16 *b1)
{

    srom11_pwrdet_t *pwrdet = pi->pwrdet_ac;
    uint8 chan_freq_range, core, core_freq_segment_map;
    phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;

    BCM_REFERENCE(pi_ac);
    FOREACH_CORE(pi, core) {
        /* core_freq_segment_map is only required for 80P80 mode.
        For other modes, it is ignored
        */
        core_freq_segment_map =
            phy_ac_chanmgr_get_data(pi->u.pi_acphy->chanmgri)->core_freq_mapping[core];

        /* Get pwrdet params from SROM for current subband */
        chan_freq_range = phy_ac_chanmgr_get_chan_freq_range(pi, pi->radio_chanspec,
            core_freq_segment_map);

        switch (chan_freq_range) {
        case WL_CHAN_FREQ_RANGE_2G:
        case WL_CHAN_FREQ_RANGE_5G_BAND0:
        case WL_CHAN_FREQ_RANGE_5G_BAND1:
        case WL_CHAN_FREQ_RANGE_5G_BAND2:
        case WL_CHAN_FREQ_RANGE_5G_BAND3:
            a1[core] =  (int16)pwrdet->pwrdet_a1[core][chan_freq_range];
            b0[core] =  (int16)pwrdet->pwrdet_b0[core][chan_freq_range];
            b1[core] =  (int16)pwrdet->pwrdet_b1[core][chan_freq_range];
            PHY_TXPWR(("wl%d: %s: pwrdet core%d: a1=%d b0=%d b1=%d\n",
                pi->sh->unit, __FUNCTION__, core,
                a1[core], b0[core], b1[core]));
            if (BF3_TSSI_DIV_WAR(pi_ac) &&
                    ACMAJORREV_128(pi->pubpi->phy_rev)) {
                uint8 core2range =
                    PHYCORENUM(pi->pubpi->phy_corenum) + core;
                ASSERT(core2range < PAPARAM_SET_NUM);
                a1[core2range] =
                    (int16)pwrdet->pwrdet_a1[core2range][chan_freq_range];
                b0[core2range] =
                    (int16)pwrdet->pwrdet_b0[core2range][chan_freq_range];
                b1[core2range] =
                    (int16)pwrdet->pwrdet_b1[core2range][chan_freq_range];
            }
            break;
        }
    }
}

#if defined(PREASSOC_PWRCTRL)
static void
phy_ac_tpc_shortwindow_upd(phy_type_tpc_ctx_t *ctx, bool new_channel)
{
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = info->pi;
    uint shmaddr;
    uint32 txallfrm_cnt, txallfrm_diff;

    if ((!pi->sh->up))
        return;

    shmaddr = MACSTAT_ADDR(pi, MCSTOFF_TXFRAME);
    /* default is long term */
    pi->tpci->data->channel_short_window = FALSE;

    phy_utils_phyreg_enter(pi);
    if (new_channel) {
        wlc_phy_pwrctrl_shortwindow_upd_acphy(pi, TRUE);
        pi->tpci->data->channel_short_window = TRUE;
        pi->tpci->data->txallfrm_cnt_ref = wlapi_bmac_read_shm(pi->sh->physhim, shmaddr);
    } else {
        txallfrm_cnt = wlapi_bmac_read_shm(pi->sh->physhim, shmaddr);
        if (pi->tpci->data->txallfrm_cnt_ref > txallfrm_cnt) {
            pi->tpci->data->txallfrm_cnt_ref = 0;
        } else {
            txallfrm_diff = txallfrm_cnt - pi->tpci->data->txallfrm_cnt_ref;
            if (txallfrm_diff > NUM_FRAME_BEFORE_PWRCTRL_CHANGE) {
                wlc_phy_pwrctrl_shortwindow_upd_acphy(pi, FALSE);
                pi->tpci->data->channel_short_window = FALSE;

            } else {
                wlc_phy_pwrctrl_shortwindow_upd_acphy(pi, TRUE);
                pi->tpci->data->channel_short_window = TRUE;
            }
        }
    }
    phy_utils_phyreg_exit(pi);
}
#endif /* PREASSOC_PWRCTRL */

#ifdef WLC_TXCAL
void
wlc_phy_txcal_olpc_idx_recal_acphy(phy_info_t *pi, bool compute_idx)
{
    uint8 core;
    uint8 hw_phyrxchain = phy_stf_get_data(pi->stfi)->hw_phyrxchain;

    BCM_REFERENCE(hw_phyrxchain);

    FOREACH_ACTV_CORE(pi, hw_phyrxchain, core) {
        if (compute_idx) {
            wlc_phy_olpc_idx_tempsense_comp_acphy(pi,
                &pi->tpci->data->base_index_init[core], core);
            if (TINY_RADIO(pi)) {
                wlc_phy_olpc_idx_tempsense_comp_acphy(pi,
                    &pi->tpci->data->base_index_cck_init[core], core);
            }
        }
        wlc_phy_txpwrctrl_set_baseindex(pi, core,
            pi->tpci->data->base_index_init[core], 1);
        if (TINY_RADIO(pi)) {
            wlc_phy_txpwrctrl_set_baseindex(pi, core,
                pi->tpci->data->base_index_cck_init[core], 0);
        }
    }
}
#endif /* WLC_TXCAL */

#if (defined(WLOLPC) && !defined(WLOLPC_DISABLED)) || defined(BCMDBG) || \
    defined(WLTEST)
void
chanspec_clr_olpc_dbg_mode(phy_ac_tpc_info_t *info)
{
    info->olpc_dbg_mode = FALSE;
}
#endif /* ((WLOLPC) && !(WLOLPC_DISABLED)) || (BCMDBG) || (WLTEST) */

#if defined(PHYCAL_CACHING)
#if defined(WLOLPC)
static void
phy_ac_tpc_update_olpc_cal(phy_type_tpc_ctx_t *tpc_ctx, bool set, bool dbg)
{
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t *)tpc_ctx;
    phy_info_t *pi = tpci->pi;
    ch_calcache_t *ctx = wlc_phy_get_chanctx(pi, pi->radio_chanspec);
    acphy_calcache_t *cache;

    PHY_TRACE(("%s\n", __FUNCTION__));

#if defined(BCMDBG) || defined(WLTEST)
    if (dbg) {
        /* If dbg mode, ignore caching and channel context */
        tpci->olpc_dbg_mode = dbg;
        if (set) {
            tpci->olpc_dbg_mode_caldone = set;
            tpci->olpc_mode_caldone = set;
            wlapi_suspend_mac_and_wait(pi->sh->physhim);
            /* Toggle Power Control to save off base index */
            wlc_phy_txpwrctrl_enable_acphy(pi, 0);
            wlc_phy_txpwrctrl_enable_acphy(pi, 1);
            wlc_phy_set_tssisens_lim_acphy(pi, TRUE);
            wlapi_enable_mac(pi->sh->physhim);
        }
    } else
#endif /* BCMDBG || WLTEST */
    {
        if (ctx && set) {
            /* do the following only if
             * caching exist and also cal is done
             */
            cache = &ctx->u.acphy_cache;
            cache->olpc_caldone = set;
            tpci->olpc_mode_caldone = set;
            wlc_phy_set_tssisens_lim_acphy(pi, tpci->ti->data->txpwroverride);
            wlapi_suspend_mac_and_wait(pi->sh->physhim);
            /* Toggle Power Control to save off base index */
            wlc_phy_txpwrctrl_enable_acphy(pi, 0);
            wlc_phy_txpwrctrl_enable_acphy(pi, 1);
            wlapi_enable_mac(pi->sh->physhim);
        }
    }
}
#endif /* WLOLPC */
#endif /* PHYCAL_CACHING */

#ifdef FCC_PWR_LIMIT_2G
void
wlc_phy_fcc_pwr_limit_set_acphy(phy_type_tpc_ctx_t *tpc_ctx, bool enable)
{
    phy_ac_tpc_info_t *info = (phy_ac_tpc_info_t *)tpc_ctx;
    phy_info_t *pi = info->pi;
    uint target_ch = CHSPEC_CHANNEL(pi->radio_chanspec);
    int8 reflimit;
    int8 limit[3];
    uint core;
    bool is_txpwrcap_changed = FALSE;
    int host_flags6;

#ifdef WL_SAR_SIMPLE_CONTROL
    bool restore_sarlimit = TRUE;
#endif /* WL_SAR_SIMPLE_CONTROL */
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

    BCM_REFERENCE(stf_shdata);

    if (!ACMAJORREV_128(pi->pubpi->phy_rev)) {
        if (!pi->tpci->data->cfg->srom_tworangetssi2g)
            return;
    }

    if (!pi->sh->clk) {
        PHY_ERROR(("wl%d: %s: PHY not up\n", pi->sh->unit,
            __FUNCTION__));
        return;
    }

    limit[0] = limit[1] = limit[2] = WLC_TXPWR_MAX;

    FOREACH_ACTV_CORE(pi, stf_shdata->phyrxchain, core) {
#ifdef WL_SAR_SIMPLE_CONTROL
        uint8 cur_core = (phy_get_phymode(pi) == PHYMODE_RSDB) ?
            phy_get_current_core(pi) : core;
        reflimit = pi->tpci->data->sarlimit[cur_core];
#else
        reflimit = WLC_TXPWR_MAX;
#endif /* WL_SAR_SIMPLE_CONTROL */
    }

    if (enable) {
        if (target_ch == 12 && pi->tpci->data->cfg->fccpwrch12 > 0) {
            limit[core] = MIN(reflimit, pi->tpci->data->cfg->fccpwrch12);

#ifdef WL_SAR_SIMPLE_CONTROL
            restore_sarlimit = FALSE;
#endif /* WL_SAR_SIMPLE_CONTROL */
        } else if (target_ch == 13 && pi->tpci->data->cfg->fccpwrch13 > 0) {
            limit[core] = MIN(reflimit, pi->tpci->data->cfg->fccpwrch13);

#ifdef WL_SAR_SIMPLE_CONTROL
            restore_sarlimit = FALSE;
#endif /* WL_SAR_SIMPLE_CONTROL */
        }
        if (limit[core] < WLC_TXPWR_MAX) {
            is_txpwrcap_changed = TRUE;
        }
    }

#ifdef WL_SAR_SIMPLE_CONTROL
    if (restore_sarlimit) {
        wlc_phy_dynamic_sarctrl_set(pi, wlc_phy_isenabled_dynamic_sarctrl(pi));
        return;
    }
#endif /* WL_SAR_SIMPLE_CONTROL */

    /* if MHF6_TXPWRCAP_EN bit is set,
     * firmware couldn't control the txpower capping register.
     */
    host_flags6 = wlapi_bmac_read_shm(pi->sh->physhim, M_HOST_FLAGS6(pi));
    if (is_txpwrcap_changed) {
        wlapi_bmac_write_shm(pi->sh->physhim, M_HOST_FLAGS6(pi),
            host_flags6 & ~MHF6_TXPWRCAP_EN);
    } else {
        wlapi_bmac_write_shm(pi->sh->physhim, M_HOST_FLAGS6(pi),
            host_flags6 | MHF6_TXPWRCAP_EN);
    }

    wlapi_suspend_mac_and_wait(pi->sh->physhim);
    phy_utils_phyreg_enter(pi);

    IF_ACTV_CORE(pi, stf_shdata->phyrxchain, 0) {
        MOD_PHYREG(pi, TxPwrCapping_path0,
            maxTxPwrCap_path0, limit[0]);
    }
    IF_ACTV_CORE(pi, stf_shdata->phyrxchain, 1) {
        MOD_PHYREG(pi, TxPwrCapping_path1,
            maxTxPwrCap_path1, limit[1]);
    }
    IF_ACTV_CORE(pi, stf_shdata->phyrxchain, 2) {
        MOD_PHYREG(pi, TxPwrCapping_path2,
            maxTxPwrCap_path2, limit[2]);
    }

    phy_utils_phyreg_exit(pi);
    wlapi_enable_mac(pi->sh->physhim);
}
#endif /* FCC_PWR_LIMIT_2G */

uint8
wlc_phy_ac_set_tssi_params_legacy(phy_info_t *pi)
{
    uint8  mult_mode = 1;
    uint8  tssi_delay;
    uint8  core;
    bool flag2rangeon;
    phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);
    BCM_REFERENCE(stf_shdata);
    BCM_REFERENCE(pi_ac);
    flag2rangeon =
        ((CHSPEC_IS2G(pi->radio_chanspec) && pi->tpci->data->cfg->srom_tworangetssi2g) ||
        (CHSPEC_ISPHY5G6G(pi->radio_chanspec) &&
        pi->tpci->data->cfg->srom_tworangetssi5g)) && PHY_IPA(pi);

    if (ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
            if (BOARDFLAGS(GENERIC_PHY_INFO(pi)->boardflags) & BFL_TSSIAVG) {
                tssi_delay = SAMPLE_TSSI_AFTER_150_SAMPLES;
                ACPHY_REG_LIST_START
                    MOD_PHYREG_ENTRY(pi, TssiAccumCtrl, Ntssi_intg_log2, 4)
                    MOD_PHYREG_ENTRY(pi, TssiAccumCtrl, tssi_accum_en, 1)
                    MOD_PHYREG_ENTRY(pi, TssiAccumCtrl, tssi_filter_pos, 1)
                ACPHY_REG_LIST_EXECUTE(pi);
                MOD_PHYREG(pi, TssiAccumCtrl, Ntssi_accum_delay, tssi_delay);
            } else {
                if (PHY_IPA(pi)) {
                    tssi_delay = SAMPLE_TSSI_AFTER_170_SAMPLES;
                } else {
                    if (CHSPEC_IS2G(pi->radio_chanspec)) {
                        if (pi->tpci->data->cfg->srom_2g_pdrange_id == 21) {
                            tssi_delay = SAMPLE_TSSI_AFTER_200_SAMPLES;
                        } else {
                            tssi_delay = SAMPLE_TSSI_AFTER_150_SAMPLES;
                        }
                    } else {
                        if (pi->tpci->data->cfg->srom_5g_pdrange_id == 23) {
                            tssi_delay = SAMPLE_TSSI_AFTER_200_SAMPLES;
                        } else {
                            tssi_delay = SAMPLE_TSSI_AFTER_150_SAMPLES;
                        }
                    }
                }
            }
        } else if (TINY_RADIO(pi)) {
            uint8  tssi_delay_cck;
            tssi_delay = SAMPLE_TSSI_AFTER_150_SAMPLES;
            if (TINY_RADIO(pi)) {
                if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
                    tssi_delay = SAMPLE_TSSI_AFTER_185_SAMPLES;
                    MOD_PHYREG(pi, perPktIdleTssiCtrl,
                            perPktIdleTssiUpdate_en, 0);
                    /* Enable the TwoPowerRange flag so that multi_mode
                    TPC works correctly
                    */
                    if (BF3_TSSI_DIV_WAR(pi_ac) || flag2rangeon)
                        MOD_PHYREG(pi, TSSIMode, TwoPwrRange, 1);
                    else
                        MOD_PHYREG(pi, TSSIMode, TwoPwrRange, 0);
                    if (!flag2rangeon) {
                        MOD_PHYREG(pi, TxPwrCtrlPwrRange2,
                            maxPwrRange2, 127);
                        MOD_PHYREG(pi, TxPwrCtrlPwrRange2,
                            minPwrRange2, -128);
                    }
                    if (CHSPEC_ISPHY5G6G(pi->radio_chanspec) &&
                        (BF3_TSSI_DIV_WAR(pi_ac))) {
                        mult_mode = 4;
                    } else {
                        mult_mode = 1;
                    }
                    FOREACH_ACTV_CORE(pi, stf_shdata->phytxchain, core) {
                        if (core == 0) {
                            MOD_PHYREG(pi, TxPwrCtrl_Multi_Mode0,
                            multi_mode, mult_mode);
                        } else if (core == 1) {
                            MOD_PHYREG(pi, TxPwrCtrl_Multi_Mode1,
                            multi_mode, mult_mode);
                        }
                    }
                } else {
                    tssi_delay = SAMPLE_TSSI_AFTER_100_SAMPLES;
                }
                if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                    ACMAJORREV_33(pi->pubpi->phy_rev) ||
                    (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                    !ACMAJORREV_128(pi->pubpi->phy_rev))) {
                    tssi_delay_cck =  SAMPLE_TSSI_AFTER_115_SAMPLES;
                } else {
                    tssi_delay_cck =  SAMPLE_TSSI_AFTER_220_SAMPLES;
                }
            } else {
                tssi_delay = 150;
                tssi_delay_cck = 0;
            }

            if (PHY_IPA(pi) && !(TINY_RADIO(pi))) {     /* this is for 4335C0 iPA */
                tssi_delay = SAMPLE_TSSI_AFTER_170_SAMPLES;
            } else {
                /* Enable tssi accum for C0. also change
                   the tssi digi filter position.
                */
                /* this helps to reduce the tssi noise. */
                MOD_PHYREG(pi, TssiAccumCtrl, Ntssi_accum_delay, tssi_delay);
                ACPHY_REG_LIST_START
                    MOD_PHYREG_ENTRY(pi, TssiAccumCtrl, Ntssi_intg_log2, 4)
                    MOD_PHYREG_ENTRY(pi, TssiAccumCtrl, tssi_accum_en, 1)
                    MOD_PHYREG_ENTRY(pi, TssiAccumCtrl, tssi_filter_pos, 1)
                ACPHY_REG_LIST_EXECUTE(pi);
                if (TINY_RADIO(pi)) {
                    MOD_PHYREG(pi, TssiAccumCtrlcck, Ntssi_accum_delay_cck,
                        tssi_delay_cck);
                    MOD_PHYREG(pi, TssiAccumCtrlcck, Ntssi_intg_log2_cck, 1);
                }
            }

} else {
        if (CHSPEC_IS2G(pi->radio_chanspec)) {
            if (pi->tpci->data->cfg->srom_2g_pdrange_id >= 5) {
                tssi_delay = SAMPLE_TSSI_AFTER_200_SAMPLES;
            } else if (pi->tpci->data->cfg->srom_2g_pdrange_id >= 4) {
                tssi_delay = SAMPLE_TSSI_AFTER_220_SAMPLES;
            } else {
                tssi_delay = SAMPLE_TSSI_AFTER_150_SAMPLES;
            }
        } else {
            if (pi->tpci->data->cfg->srom_5g_pdrange_id >= 5) {
                tssi_delay = SAMPLE_TSSI_AFTER_200_SAMPLES;
            } else if (pi->tpci->data->cfg->srom_5g_pdrange_id >= 4) {
                tssi_delay = SAMPLE_TSSI_AFTER_220_SAMPLES;
            } else {
                tssi_delay = SAMPLE_TSSI_AFTER_150_SAMPLES;
            }
        }
    }
    return tssi_delay;
}

uint8
wlc_phy_ac_set_tssi_params_maj36(phy_info_t *pi)
{

    ACPHY_REG_LIST_START

        MOD_PHYREG_ENTRY(pi, TxPwrCtrlDamping, DeltaPwrDamp, 16)
        MOD_PHYREG_ENTRY(pi, TxPwrCtrlNnum, Ntssi_delay,
            75)
        MOD_PHYREG_ENTRY(pi, TssiAccumCtrl, Ntssi_accum_delay, 150)
        MOD_PHYREG_ENTRY(pi, TssiAccumCtrlcck, Ntssi_accum_delay_cck,
            75)
        MOD_PHYREG_ENTRY(pi, TssiAccumCtrl, tssi_accum_en, 1)
        MOD_PHYREG_ENTRY(pi, TssiAccumCtrl, tssi_filter_pos, 1)
        MOD_PHYREG_ENTRY(pi, TssiAccumCtrl, Ntssi_intg_log2, 4)
        MOD_PHYREG_ENTRY(pi, TssiAccumCtrlcck, Ntssi_intg_log2_cck, 3)
        MOD_PHYREG_ENTRY(pi, perPktIdleTssiCtrl,
            perPktIdleTssiUpdate_en, 0)
        MOD_PHYREG_ENTRY(pi, TSSIMode, TwoPwrRange, 0)
        MOD_PHYREG_ENTRY(pi, TxPwrCtrl_Multi_Mode0, multi_mode, 0x3)
        MOD_PHYREG_ENTRY(pi, TxPwrCtrlCmd, bbMultInt_en, 0x0)

    ACPHY_REG_LIST_EXECUTE(pi)

    return SAMPLE_TSSI_AFTER_185_SAMPLES;
}

uint8
wlc_phy_ac_set_tssi_params_majrev128(phy_info_t *pi)
{
    bool flag2rangeon =
        ((CHSPEC_IS2G(pi->radio_chanspec) &&
        phy_tpc_get_tworangetssi2g(pi->tpci)) || (CHSPEC_ISPHY5G6G(pi->radio_chanspec) &&
        phy_tpc_get_tworangetssi5g(pi->tpci))) && PHY_IPA(pi);
    uint8 twopwrrange, multi_mode;

    if (flag2rangeon || BF3_TSSI_DIV_WAR(pi->u.pi_acphy)) {
        twopwrrange = 1;
        if (CHSPEC_ISPHY5G6G(pi->radio_chanspec))
            multi_mode = 3;
        else
            multi_mode = 1;
    } else {
        twopwrrange = 0;
        multi_mode = 1;
    }

    ACPHY_REG_LIST_START
        MOD_PHYREG_ENTRY(pi, TxPwrCtrlNnum, Ntssi_delay, 200)
        MOD_PHYREG_ENTRY(pi, perPktIdleTssiCtrl,
            perPktIdleTssiUpdate_en, 0)
        MOD_PHYREG_ENTRY(pi, TssiAccumCtrl, Ntssi_intg_log2, 4)
        MOD_PHYREG_ENTRY(pi, TssiAccumCtrlcck, Ntssi_intg_log2_cck, 2)
        MOD_PHYREG_ENTRY(pi, TssiAccumCtrl, Ntssi_accum_delay, 200)
        MOD_PHYREG_ENTRY(pi, TssiAccumCtrlcck, Ntssi_accum_delay_cck, 200)
    ACPHY_REG_LIST_EXECUTE(pi)

    if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en) {
        MOD_PHYREG(pi, TssiAccumCtrl, tssi_accum_en, 0);
    } else {
        MOD_PHYREG(pi, TssiAccumCtrl, tssi_accum_en, 1);
    }
    MOD_PHYREG(pi, TSSIMode, TwoPwrRange, twopwrrange);
    MOD_PHYREG(pi, TxPwrCtrl_Multi_Mode0, multi_mode, multi_mode);

    return SAMPLE_TSSI_AFTER_200_SAMPLES;
}

int8
wlc_phy_calc_ppr_pwr_cap_acphy(phy_info_t *pi, uint8 core, int8 maxpwr)
{
    /* Store MIN(MAX_rates(tgtpwr), sarlimit, txpwrcap) in pi structure */
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

    if (core >= PHY_CORE_MAX) {
        phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

        BCM_REFERENCE(stf_shdata);

        PHY_ERROR(("%s: Invalid PHY core[0x%02x], setting cap to %d,"
            " hw_phytxchain[0x%02x] hw_phyrxchain[0x%02x]\n",
            __FUNCTION__, core, 127, stf_shdata->hw_phytxchain,
            stf_shdata->hw_phyrxchain));
        return 127;
    }

#ifdef WL_SARLIMIT
    pi->tpci->data->adjusted_pwr_cap[core] = MIN(pi->tpci->data->sarlimit[core],
        maxpwr + pi_ac->tpci->txpwr_offset[core]);
#else
    pi->tpci->data->adjusted_pwr_cap[core] = maxpwr + pi_ac->tpci->txpwr_offset[core];
#endif

#ifdef WLC_TXPWRCAP
    if (PHY_TXPWRCAP_ENAB(pi->sh->physhim)) {
        pi->tpci->data->adjusted_pwr_cap[core] =
            MIN(phy_txpwrcap_tbl_get_max_percore(pi, core),
            pi->tpci->data->adjusted_pwr_cap[core]);
    }
#endif /* WLC_TXPWRCAP */
    return pi->tpci->data->adjusted_pwr_cap[core];
}

uint16
wlc_phy_set_txpwr_by_index_acphy(phy_info_t *pi, uint8 core_mask, int8 txpwrindex)
{
    uint8 core;
    uint16 lpfgain;
    txgain_setting_t txgain_settings;
    uint8 stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    ASSERT(core_mask);
    ASSERT(txpwrindex >= 0);    /* negative index not supported */

    ACPHY_DISABLE_STALL(pi);

    /* Set zero tx gain */
    memset(&txgain_settings, 0, sizeof(txgain_setting_t));
    /* Set tx power based on an input "index"
     * (Emulate what HW power control would use for a given table index)
     */
    FOREACH_ACTV_CORE(pi, core_mask, core) {
        /* Read tx gain table */
        wlc_phy_get_txgain_settings_by_index_acphy(pi, &txgain_settings, txpwrindex);

        lpfgain = ((txgain_settings.rad_gain & 0x70) >> 4);
        lpfgain = lpfgain | (lpfgain <<3) | (lpfgain <<6);
        /* Override gains: DAC, Radio and BBmult */
        if (core == 3) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                0x501, 16, &(txgain_settings.rad_gain));
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                0x504, 16, &(txgain_settings.rad_gain_mi));
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                0x507, 16, &(txgain_settings.rad_gain_hi));
            /* for 6715 lpf_bq2_gain is taken from the tx-gaincode direct contrl */
            if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                !ACMAJORREV_128(pi->pubpi->phy_rev) &&
                !ACMAJORREV_130(pi->pubpi->phy_rev)) {
                // Tx BQ2 gain is in a 3-bit field at different location
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                    0x51e, 16, &lpfgain);
            }
        } else {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                (0x100 + core), 16, &(txgain_settings.rad_gain));
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                (0x103 + core), 16, &(txgain_settings.rad_gain_mi));
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                (0x106 + core), 16, &(txgain_settings.rad_gain_hi));
            /* for 6715 lpf_bq2_gain is taken from the tx-gaincode direct contrl */
            if (ACMAJORREV_GE37(pi->pubpi->phy_rev) &&
                !ACMAJORREV_130(pi->pubpi->phy_rev)) {
                // Tx BQ2 gain is in a 3-bit field at different location
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                    (0x17e + 16*core), 16, &lpfgain);
        }
        }
        wlc_phy_set_tx_bbmult_acphy(pi, &txgain_settings.bbmult, core);

#ifdef WLC_TXSHAPER
        if (ACMAJORREV_GE130(pi->pubpi->phy_rev)) {
            phy_ac_txss_txshaper_gain_index_set(pi, core, txpwrindex,
                    txgain_settings.bbmult);
        }
#endif

        PHY_TXPWR(("wl%d: %s: Fixed txpwrindex for core%d is %d\n",
                  pi->sh->unit, __FUNCTION__, core, txpwrindex));
    }
    ACPHY_ENABLE_STALL(pi, stall_val);

    return txgain_settings.bbmult;
}

#if defined(WLTEST)
static int
phy_ac_tpc_set_pavars(phy_type_tpc_ctx_t *ctx, void *a, void *p)
{
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = tpci->pi;
    uint16 inpa[WL_PHY_PAVARS_LEN];
    uint j = 3; /* PA parameters start from offset 3 */
    int chain, freq_range, num_paparams = PHY_CORE_MAX;
#ifdef WL_CHAN_FREQ_RANGE_5G_4BAND
    int n;
#endif

    bcopy(p, inpa, sizeof(inpa));

    chain = inpa[2];
    freq_range = inpa[1];

    if ((ACMAJORREV_2(pi->pubpi->phy_rev) ||
        ACMAJORREV_4(pi->pubpi->phy_rev)) &&
        BF3_TSSI_DIV_WAR(pi->u.pi_acphy)) {
        num_paparams = 4;
    }

    if (inpa[0] != PHY_TYPE_AC) {
        PHY_ERROR(("Wrong phy type %d\n", inpa[0]));
        return BCME_BADARG;
    }

    if (chain > (num_paparams - 1)) {
        PHY_ERROR(("Wrong chain number %d\n", chain));
        return BCME_BADARG;
    }
#if BAND6G
    if (SROMREV(pi->sh->sromrev) >= 18 &&
            (PHY_HAS_6G(pi) || SUBBANB5GV6(pi))) {
        srom18_5gext_pwrdet_t *pwrdet_5gext = pi->pwrdet_ac_5gext;
        switch (freq_range) {
        case WL_CHAN_FREQ_RANGE_5GEXT_5BAND:
        for (n = 0; n < 2; n++) {
            pwrdet_5gext->pwrdet_a[chain][n] = inpa[j++];
            pwrdet_5gext->pwrdet_b[chain][n] = inpa[j++];
            pwrdet_5gext->pwrdet_c[chain][n] = inpa[j++];
            pwrdet_5gext->pwrdet_d[chain][n] = inpa[j++];
        }
        break;
        case WL_CHAN_FREQ_RANGE_5GEXT_5BAND_40:
        for (n = 0; n < 2; n++) {
            pwrdet_5gext->pwrdet_a_40[chain][n] = inpa[j++];
            pwrdet_5gext->pwrdet_b_40[chain][n] = inpa[j++];
            pwrdet_5gext->pwrdet_c_40[chain][n] = inpa[j++];
            pwrdet_5gext->pwrdet_d_40[chain][n] = inpa[j++];
        }
        break;
        case WL_CHAN_FREQ_RANGE_5GEXT_5BAND_80:
        for (n = 0; n < 2; n++) {
            pwrdet_5gext->pwrdet_a_80[chain][n] = inpa[j++];
            pwrdet_5gext->pwrdet_b_80[chain][n] = inpa[j++];
            pwrdet_5gext->pwrdet_c_80[chain][n] = inpa[j++];
            pwrdet_5gext->pwrdet_d_80[chain][n] = inpa[j++];
        }
        break;
        case WL_CHAN_FREQ_RANGE_5GEXT_5BAND_160:
        for (n = 0; n < 3; n++) {
            pwrdet_5gext->pwrdet_a_160[chain][n] = inpa[j++];
            pwrdet_5gext->pwrdet_b_160[chain][n] = inpa[j++];
            pwrdet_5gext->pwrdet_c_160[chain][n] = inpa[j++];
            pwrdet_5gext->pwrdet_d_160[chain][n] = inpa[j++];
        }
        default:
        break;
        }
    }
#endif /* BAND6G */
    if (SROMREV(pi->sh->sromrev) >= 12) {
        srom12_pwrdet_t *pwrdet = pi->pwrdet_ac;
        switch (freq_range) {
        case WL_CHAN_FREQ_RANGE_2G:
        pwrdet->pwrdet_a[chain][freq_range] = inpa[j++];
        pwrdet->pwrdet_b[chain][freq_range] = inpa[j++];
        pwrdet->pwrdet_c[chain][freq_range] = inpa[j++];
        pwrdet->pwrdet_d[chain][freq_range] = inpa[j++];
        break;
        case WL_CHAN_FREQ_RANGE_2G_20_CCK:
        if (SROMREV(pi->sh->sromrev) >= 18 || ACMAJORREV_128(pi->pubpi->phy_rev)) {
            // CCK PAPARAM
            pwrdet->pwrdet_a[chain][CH_2G_GROUP + CH_5G_5BAND]
                = inpa[j++];
            pwrdet->pwrdet_b[chain][CH_2G_GROUP + CH_5G_5BAND]
                = inpa[j++];
            pwrdet->pwrdet_c[chain][CH_2G_GROUP + CH_5G_5BAND]
                = inpa[j++];
            pwrdet->pwrdet_d[chain][CH_2G_GROUP + CH_5G_5BAND]
                = inpa[j++];
        } else {
            PHY_ERROR(("bandrange %d is out of scope\n", inpa[1]));
            return BCME_OUTOFRANGECHAN;
        }
        break;
        case WL_CHAN_FREQ_RANGE_2G_40:
        pwrdet->pwrdet_a_40[chain][freq_range-6] = inpa[j++];
        pwrdet->pwrdet_b_40[chain][freq_range-6] = inpa[j++];
        pwrdet->pwrdet_c_40[chain][freq_range-6] = inpa[j++];
        pwrdet->pwrdet_d_40[chain][freq_range-6] = inpa[j++];
        break;
        /* allow compile in branches without 4BAND definition */
#ifdef WL_CHAN_FREQ_RANGE_5G_4BAND
        case WL_CHAN_FREQ_RANGE_5G_BAND4:
        pwrdet->pwrdet_a[chain][freq_range] = inpa[j++];
        pwrdet->pwrdet_b[chain][freq_range] = inpa[j++];
        pwrdet->pwrdet_c[chain][freq_range] = inpa[j++];
        pwrdet->pwrdet_d[chain][freq_range] = inpa[j++];
        break;

        case WL_CHAN_FREQ_RANGE_5G_BAND0:
        case WL_CHAN_FREQ_RANGE_5G_BAND1:
        case WL_CHAN_FREQ_RANGE_5G_BAND2:
        case WL_CHAN_FREQ_RANGE_5G_BAND3:
        if (ACMAJORREV_2(pi->pubpi->phy_rev) ||
            ACMAJORREV_5(pi->pubpi->phy_rev)) {
            pwrdet->pwrdet_a[chain][freq_range] = inpa[j++];
            pwrdet->pwrdet_b[chain][freq_range] = inpa[j++];
            pwrdet->pwrdet_c[chain][freq_range] = inpa[j++];
            pwrdet->pwrdet_d[chain][freq_range] = inpa[j++];
        } else {
            PHY_ERROR(("bandrange %d is out of scope\n", inpa[1]));
            return BCME_OUTOFRANGECHAN;
        }
        break;
        case WL_CHAN_FREQ_RANGE_5G_BAND0_40:
        case WL_CHAN_FREQ_RANGE_5G_BAND1_40:
        case WL_CHAN_FREQ_RANGE_5G_BAND2_40:
        case WL_CHAN_FREQ_RANGE_5G_BAND3_40:
        case WL_CHAN_FREQ_RANGE_5G_BAND4_40:
        pwrdet->pwrdet_a_40[chain][freq_range-6] = inpa[j++];
        pwrdet->pwrdet_b_40[chain][freq_range-6] = inpa[j++];
        pwrdet->pwrdet_c_40[chain][freq_range-6] = inpa[j++];
        pwrdet->pwrdet_d_40[chain][freq_range-6] = inpa[j++];
        break;
        case WL_CHAN_FREQ_RANGE_5G_BAND0_80:
        case WL_CHAN_FREQ_RANGE_5G_BAND1_80:
        case WL_CHAN_FREQ_RANGE_5G_BAND2_80:
        case WL_CHAN_FREQ_RANGE_5G_BAND3_80:
        case WL_CHAN_FREQ_RANGE_5G_BAND4_80:
        pwrdet->pwrdet_a_80[chain][freq_range-12] = inpa[j++];
        pwrdet->pwrdet_b_80[chain][freq_range-12] = inpa[j++];
        pwrdet->pwrdet_c_80[chain][freq_range-12] = inpa[j++];
        pwrdet->pwrdet_d_80[chain][freq_range-12] = inpa[j++];
        break;
        case WL_CHAN_FREQ_RANGE_5G_BAND0_160:
        case WL_CHAN_FREQ_RANGE_5G_BAND1_160:
        case WL_CHAN_FREQ_RANGE_5G_BAND2_160:
        case WL_CHAN_FREQ_RANGE_5G_BAND3_160:
        pwrdet->pwrdet_a_160[chain][freq_range-22] = inpa[j++];
        pwrdet->pwrdet_b_160[chain][freq_range-22] = inpa[j++];
        pwrdet->pwrdet_c_160[chain][freq_range-22] = inpa[j++];
        pwrdet->pwrdet_d_160[chain][freq_range-22] = inpa[j++];
        break;
        case WL_CHAN_FREQ_RANGE_5G_5BAND:
        for (n = 1; n <= 5; n++) {
            pwrdet->pwrdet_a[chain][n] = inpa[j++];
            pwrdet->pwrdet_b[chain][n] = inpa[j++];
            pwrdet->pwrdet_c[chain][n] = inpa[j++];
            pwrdet->pwrdet_d[chain][n] = inpa[j++];
        }
        break;
        case WL_CHAN_FREQ_RANGE_5G_5BAND_40:
        for (n = 1; n <= 5; n++) {
            pwrdet->pwrdet_a_40[chain][n] = inpa[j++];
            pwrdet->pwrdet_b_40[chain][n] = inpa[j++];
            pwrdet->pwrdet_c_40[chain][n] = inpa[j++];
            pwrdet->pwrdet_d_40[chain][n] = inpa[j++];
        }
        break;
        case WL_CHAN_FREQ_RANGE_5G_5BAND_80:
        for (n = 0; n <= 4; n++) {
            pwrdet->pwrdet_a_80[chain][n] = inpa[j++];
            pwrdet->pwrdet_b_80[chain][n] = inpa[j++];
            pwrdet->pwrdet_c_80[chain][n] = inpa[j++];
            pwrdet->pwrdet_d_80[chain][n] = inpa[j++];
        }
        break;
        case WL_CHAN_FREQ_RANGE_5G_5BAND_160:
        for (n = 0; n < 4; n++) {
            pwrdet->pwrdet_a_160[chain][n] = inpa[j++];
            pwrdet->pwrdet_b_160[chain][n] = inpa[j++];
            pwrdet->pwrdet_c_160[chain][n] = inpa[j++];
            pwrdet->pwrdet_d_160[chain][n] = inpa[j++];
        }
        break;
#endif /* WL_CHAN_FREQ_RANGE_5G_4BAND */
        default:
        PHY_ERROR(("bandrange %d is out of scope\n", inpa[1]));
        return BCME_OUTOFRANGECHAN;
        }
    } else {
        srom11_pwrdet_t *pwrdet11 = pi->pwrdet_ac;
        switch (freq_range) {
        case WL_CHAN_FREQ_RANGE_2G:
        pwrdet11->pwrdet_a1[chain][freq_range] = inpa[j++];
        pwrdet11->pwrdet_b0[chain][freq_range] = inpa[j++];
        pwrdet11->pwrdet_b1[chain][freq_range] = inpa[j++];
        break;
        /* allow compile in branches without 4BAND definition */
#ifdef WL_CHAN_FREQ_RANGE_5G_4BAND
        case WL_CHAN_FREQ_RANGE_5G_4BAND:
        for (n = 1; n <= 4; n ++) {
            pwrdet11->pwrdet_a1[chain][n] = inpa[j++];
            pwrdet11->pwrdet_b0[chain][n] = inpa[j++];
            pwrdet11->pwrdet_b1[chain][n] = inpa[j++];
        }
        break;

        case WL_CHAN_FREQ_RANGE_5G_BAND0:
        case WL_CHAN_FREQ_RANGE_5G_BAND1:
        case WL_CHAN_FREQ_RANGE_5G_BAND2:
        case WL_CHAN_FREQ_RANGE_5G_BAND3:
        if (ACMAJORREV_2(pi->pubpi->phy_rev) ||
            ACMAJORREV_5(pi->pubpi->phy_rev)) {
            pwrdet11->pwrdet_a1[chain][freq_range] = inpa[j++];
            pwrdet11->pwrdet_b0[chain][freq_range] = inpa[j++];
            pwrdet11->pwrdet_b1[chain][freq_range] = inpa[j++];
        } else {
            PHY_ERROR(("bandrange %d is out of scope\n", inpa[1]));
            return BCME_OUTOFRANGECHAN;
        }
        break;
#endif /* WL_CHAN_FREQ_RANGE_5G_4BAND */
        default:
        PHY_ERROR(("bandrange %d is out of scope\n", inpa[1]));
        return BCME_OUTOFRANGECHAN;
        }
    }
    return BCME_OK;
}

static int
phy_ac_tpc_get_pavars(phy_type_tpc_ctx_t *ctx, void *a, void *p)
{
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = tpci->pi;
    uint16 *outpa = a;
    uint16 inpa[WL_PHY_PAVARS_LEN];
    uint j = 3; /* PA parameters start from offset 3 */
    int chain, freq_range, num_paparams = PHY_CORE_MAX;
#ifdef WL_CHAN_FREQ_RANGE_5G_4BAND
    int n;
#endif

    bcopy(p, inpa, sizeof(inpa));

    outpa[0] = inpa[0]; /* Phy type */
    outpa[1] = inpa[1]; /* Band range */
    outpa[2] = inpa[2]; /* Chain */

    chain = inpa[2];
    freq_range = inpa[1];

    if ((ACMAJORREV_2(pi->pubpi->phy_rev) ||
        (ACMAJORREV_4(pi->pubpi->phy_rev))) &&
        BF3_TSSI_DIV_WAR(pi->u.pi_acphy)) {
        num_paparams = 4;
    }

    if (inpa[0] != PHY_TYPE_AC) {
        PHY_ERROR(("Wrong phy type %d\n", inpa[0]));
        outpa[0] = PHY_TYPE_NULL;
        return BCME_BADARG;
    }
    if (chain > (num_paparams - 1)) {
        PHY_ERROR(("Wrong chain number %d\n", chain));
        outpa[0] = PHY_TYPE_NULL;
        return BCME_BADARG;
    }
#if BAND6G
    if (SROMREV(pi->sh->sromrev) >= 18) {
        srom18_5gext_pwrdet_t *pwrdet_5gext = pi->pwrdet_ac_5gext;
        switch (freq_range) {
        case WL_CHAN_FREQ_RANGE_5GEXT_5BAND:
        for (n = 0; n < 2; n++) {
        if (PHY_HAS_6G(pi) || SUBBANB5GV6(pi)) {
            outpa[j++] = pwrdet_5gext->pwrdet_a[chain][n];
            outpa[j++] = pwrdet_5gext->pwrdet_b[chain][n];
            outpa[j++] = pwrdet_5gext->pwrdet_c[chain][n];
            outpa[j++] = pwrdet_5gext->pwrdet_d[chain][n];
        } else {
            outpa[j++] = 0;
            outpa[j++] = 0;
            outpa[j++] = 0;
            outpa[j++] = 0;
        }
        }
        return BCME_OK;
        case WL_CHAN_FREQ_RANGE_5GEXT_5BAND_40:
        for (n = 0; n < 2; n++) {
        if (PHY_HAS_6G(pi) || SUBBANB5GV6(pi)) {
            outpa[j++] = pwrdet_5gext->pwrdet_a_40[chain][n];
            outpa[j++] = pwrdet_5gext->pwrdet_b_40[chain][n];
            outpa[j++] = pwrdet_5gext->pwrdet_c_40[chain][n];
            outpa[j++] = pwrdet_5gext->pwrdet_d_40[chain][n];
        } else {
            outpa[j++] = 0;
            outpa[j++] = 0;
            outpa[j++] = 0;
            outpa[j++] = 0;
        }
        }
        return BCME_OK;
        case WL_CHAN_FREQ_RANGE_5GEXT_5BAND_80:
        for (n = 0; n < 2; n++) {
        if (PHY_HAS_6G(pi) || SUBBANB5GV6(pi)) {
            outpa[j++] = pwrdet_5gext->pwrdet_a_80[chain][n];
            outpa[j++] = pwrdet_5gext->pwrdet_b_80[chain][n];
            outpa[j++] = pwrdet_5gext->pwrdet_c_80[chain][n];
            outpa[j++] = pwrdet_5gext->pwrdet_d_80[chain][n];
        } else {
            outpa[j++] = 0;
            outpa[j++] = 0;
            outpa[j++] = 0;
            outpa[j++] = 0;
        }
        }
        return BCME_OK;
        case WL_CHAN_FREQ_RANGE_5GEXT_5BAND_160:
        for (n = 0; n < 3; n++) {
        if (PHY_HAS_6G(pi) || SUBBANB5GV6(pi)) {
            outpa[j++] = pwrdet_5gext->pwrdet_a_160[chain][n];
            outpa[j++] = pwrdet_5gext->pwrdet_b_160[chain][n];
            outpa[j++] = pwrdet_5gext->pwrdet_c_160[chain][n];
            outpa[j++] = pwrdet_5gext->pwrdet_d_160[chain][n];
        } else {
            outpa[j++] = 0;
            outpa[j++] = 0;
            outpa[j++] = 0;
            outpa[j++] = 0;
        }
        }
        return BCME_OK;
        default:
        break;
        }
    }
#endif /* BAND6G */
    if (SROMREV(pi->sh->sromrev) >= 12) {
        srom12_pwrdet_t *pwrdet = pi->pwrdet_ac;
        switch (freq_range) {
        case WL_CHAN_FREQ_RANGE_2G:
        outpa[j++] = pwrdet->pwrdet_a[chain][freq_range];
        outpa[j++] = pwrdet->pwrdet_b[chain][freq_range];
        outpa[j++] = pwrdet->pwrdet_c[chain][freq_range];
        outpa[j++] = pwrdet->pwrdet_d[chain][freq_range];
        break;
        case WL_CHAN_FREQ_RANGE_2G_20_CCK:
        if (SROMREV(pi->sh->sromrev) >= 18 || ACMAJORREV_128(pi->pubpi->phy_rev)) {
            // CCK PAPARAM
            outpa[j++] = pwrdet->
                pwrdet_a[chain][CH_2G_GROUP + CH_5G_5BAND];
            outpa[j++] = pwrdet->
                pwrdet_b[chain][CH_2G_GROUP + CH_5G_5BAND];
            outpa[j++] = pwrdet->
                pwrdet_c[chain][CH_2G_GROUP + CH_5G_5BAND];
            outpa[j++] = pwrdet->
                pwrdet_d[chain][CH_2G_GROUP + CH_5G_5BAND];
        } else {
            PHY_ERROR(("bandrange %d is out of scope\n", inpa[1]));
            return BCME_OUTOFRANGECHAN;
        }
        break;
        case WL_CHAN_FREQ_RANGE_2G_40:
        outpa[j++] = pwrdet->pwrdet_a_40[chain][freq_range-6];
        outpa[j++] = pwrdet->pwrdet_b_40[chain][freq_range-6];
        outpa[j++] = pwrdet->pwrdet_c_40[chain][freq_range-6];
        outpa[j++] = pwrdet->pwrdet_d_40[chain][freq_range-6];
        break;
        /* allow compile in branches without 4BAND definition */
#ifdef WL_CHAN_FREQ_RANGE_5G_4BAND
        case WL_CHAN_FREQ_RANGE_5G_BAND4:
        outpa[j++] = pwrdet->pwrdet_a[chain][freq_range];
        outpa[j++] = pwrdet->pwrdet_b[chain][freq_range];
        outpa[j++] = pwrdet->pwrdet_c[chain][freq_range];
        outpa[j++] = pwrdet->pwrdet_d[chain][freq_range];
        break;
        case WL_CHAN_FREQ_RANGE_5G_BAND0:
        case WL_CHAN_FREQ_RANGE_5G_BAND1:
        case WL_CHAN_FREQ_RANGE_5G_BAND2:
        case WL_CHAN_FREQ_RANGE_5G_BAND3:
        if (ACMAJORREV_2(pi->pubpi->phy_rev) ||
            ACMAJORREV_5(pi->pubpi->phy_rev)) {
            outpa[j++] = pwrdet->pwrdet_a[chain][freq_range];
            outpa[j++] = pwrdet->pwrdet_b[chain][freq_range];
            outpa[j++] = pwrdet->pwrdet_c[chain][freq_range];
            outpa[j++] = pwrdet->pwrdet_d[chain][freq_range];
        } else {
            PHY_ERROR(("bandrange %d is out of scope\n", inpa[1]));
            return BCME_OUTOFRANGECHAN;
        }
        break;
        case WL_CHAN_FREQ_RANGE_5G_BAND0_40:
        case WL_CHAN_FREQ_RANGE_5G_BAND1_40:
        case WL_CHAN_FREQ_RANGE_5G_BAND2_40:
        case WL_CHAN_FREQ_RANGE_5G_BAND3_40:
        case WL_CHAN_FREQ_RANGE_5G_BAND4_40:
        outpa[j++] =
            pwrdet->pwrdet_a_40[chain][freq_range - WL_CHAN_FREQ_RANGE_2G_40];
        outpa[j++] =
            pwrdet->pwrdet_b_40[chain][freq_range - WL_CHAN_FREQ_RANGE_2G_40];
        outpa[j++] =
            pwrdet->pwrdet_c_40[chain][freq_range - WL_CHAN_FREQ_RANGE_2G_40];
        outpa[j++] =
            pwrdet->pwrdet_d_40[chain][freq_range - WL_CHAN_FREQ_RANGE_2G_40];
        break;
        case WL_CHAN_FREQ_RANGE_5G_BAND0_80:
        case WL_CHAN_FREQ_RANGE_5G_BAND1_80:
        case WL_CHAN_FREQ_RANGE_5G_BAND2_80:
        case WL_CHAN_FREQ_RANGE_5G_BAND3_80:
        case WL_CHAN_FREQ_RANGE_5G_BAND4_80:
        outpa[j++] =
            pwrdet->pwrdet_a_80[chain][freq_range - WL_CHAN_FREQ_RANGE_5G_BAND0_80];
        outpa[j++] =
            pwrdet->pwrdet_b_80[chain][freq_range - WL_CHAN_FREQ_RANGE_5G_BAND0_80];
        outpa[j++] =
            pwrdet->pwrdet_c_80[chain][freq_range - WL_CHAN_FREQ_RANGE_5G_BAND0_80];
        outpa[j++] =
            pwrdet->pwrdet_d_80[chain][freq_range - WL_CHAN_FREQ_RANGE_5G_BAND0_80];
        break;
        case WL_CHAN_FREQ_RANGE_5G_BAND0_160:
        case WL_CHAN_FREQ_RANGE_5G_BAND1_160:
        case WL_CHAN_FREQ_RANGE_5G_BAND2_160:
        case WL_CHAN_FREQ_RANGE_5G_BAND3_160:
        outpa[j++] =
            pwrdet->pwrdet_a_160[chain][freq_range - WL_CHAN_FREQ_RANGE_5G_BAND0_160];
        outpa[j++] =
            pwrdet->pwrdet_b_160[chain][freq_range - WL_CHAN_FREQ_RANGE_5G_BAND0_160];
        outpa[j++] =
            pwrdet->pwrdet_c_160[chain][freq_range - WL_CHAN_FREQ_RANGE_5G_BAND0_160];
        outpa[j++] =
            pwrdet->pwrdet_d_160[chain][freq_range - WL_CHAN_FREQ_RANGE_5G_BAND0_160];
        break;
        case WL_CHAN_FREQ_RANGE_5G_5BAND:
        for (n = 1; n <= 5; n++) {
            outpa[j++] = pwrdet->pwrdet_a[chain][n];
            outpa[j++] = pwrdet->pwrdet_b[chain][n];
            outpa[j++] = pwrdet->pwrdet_c[chain][n];
            outpa[j++] = pwrdet->pwrdet_d[chain][n];
        }
        break;
        case WL_CHAN_FREQ_RANGE_5G_5BAND_40:
        for (n = 1; n <= 5; n++) {
            outpa[j++] = pwrdet->pwrdet_a_40[chain][n];
            outpa[j++] = pwrdet->pwrdet_b_40[chain][n];
            outpa[j++] = pwrdet->pwrdet_c_40[chain][n];
            outpa[j++] = pwrdet->pwrdet_d_40[chain][n];
        }
        break;
        case WL_CHAN_FREQ_RANGE_5G_5BAND_80:
        for (n = 0; n <= 4; n++) {
            outpa[j++] = pwrdet->pwrdet_a_80[chain][n];
            outpa[j++] = pwrdet->pwrdet_b_80[chain][n];
            outpa[j++] = pwrdet->pwrdet_c_80[chain][n];
            outpa[j++] = pwrdet->pwrdet_d_80[chain][n];
        }
        break;
        case WL_CHAN_FREQ_RANGE_5G_5BAND_160:
        for (n = 0; n < 4; n++) {
            outpa[j++] = pwrdet->pwrdet_a_160[chain][n];
            outpa[j++] = pwrdet->pwrdet_b_160[chain][n];
            outpa[j++] = pwrdet->pwrdet_c_160[chain][n];
            outpa[j++] = pwrdet->pwrdet_d_160[chain][n];
        }
        break;
#endif /* WL_CHAN_FREQ_RANGE_5G_4BAND */
        default:
        PHY_ERROR(("bandrange %d is out of scope\n", inpa[1]));
        return BCME_OUTOFRANGECHAN;
        }
    } else {
        srom11_pwrdet_t *pwrdet11 = pi->pwrdet_ac;
        switch (freq_range) {
        case WL_CHAN_FREQ_RANGE_2G:
        outpa[j++] = pwrdet11->pwrdet_a1[chain][freq_range];
        outpa[j++] = pwrdet11->pwrdet_b0[chain][freq_range];
        outpa[j++] = pwrdet11->pwrdet_b1[chain][freq_range];
        break;
        /* allow compile in branches without 4BAND definition */
#ifdef WL_CHAN_FREQ_RANGE_5G_4BAND
        case WL_CHAN_FREQ_RANGE_5G_4BAND:
        for (n = 1; n <= 4; n ++) {
            outpa[j++] = pwrdet11->pwrdet_a1[chain][n];
            outpa[j++] = pwrdet11->pwrdet_b0[chain][n];
            outpa[j++] = pwrdet11->pwrdet_b1[chain][n];
        }
        break;
        case WL_CHAN_FREQ_RANGE_5G_BAND0:
        case WL_CHAN_FREQ_RANGE_5G_BAND1:
        case WL_CHAN_FREQ_RANGE_5G_BAND2:
        case WL_CHAN_FREQ_RANGE_5G_BAND3:
        if (ACMAJORREV_2(pi->pubpi->phy_rev) ||
            ACMAJORREV_5(pi->pubpi->phy_rev)) {
            outpa[j++] = pwrdet11->pwrdet_a1[chain][freq_range];
            outpa[j++] = pwrdet11->pwrdet_b0[chain][freq_range];
            outpa[j++] = pwrdet11->pwrdet_b1[chain][freq_range];
        } else {
            PHY_ERROR(("bandrange %d is out of scope\n", inpa[1]));
            return BCME_OUTOFRANGECHAN;
        }
        break;
        default:
        PHY_ERROR(("bandrange %d is out of scope\n", inpa[1]));
        return BCME_OUTOFRANGECHAN;
        break;
        }
#endif /* WL_CHAN_FREQ_RANGE_5G_4BAND */
    }
    return BCME_OK;
}

int
phy_ac_tpc_txpower_get_instant_percore(phy_type_tpc_ctx_t *ctx, void *pwr)
{
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = tpci->pi;
    tx_inst_power_percore_t *power = (tx_inst_power_percore_t *)pwr;
    uint8 est_Pout_adj[PHY_CORE_MAX];
    uint8 core;

    FOREACH_CORE(pi, core) {
        power->txpwr_est_Pout[core] = 0;
    }

    /* Get power estimates */
    wlapi_suspend_mac_and_wait(pi->sh->physhim);
    phy_utils_phyreg_enter(pi);
    wlc_phy_txpwr_est_pwr_acphy(pi, power->txpwr_est_Pout, est_Pout_adj);
    phy_utils_phyreg_exit(pi);
    wlapi_enable_mac(pi->sh->physhim);

    return BCME_OK;
}

int
phy_ac_tpc_txpower_get_instant(phy_type_tpc_ctx_t *ctx, void *pwr)
{
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = tpci->pi;
    tx_inst_power_t *power = (tx_inst_power_t *)pwr;
    tx_inst_power_percore_t power_percore;
    uint8 est_Pout_adj[PHY_CORE_MAX];
    uint8 core;

    FOREACH_CORE(pi, core) {
        power_percore.txpwr_est_Pout[core] = 0;
    }

    /* Get power estimates */
    wlapi_suspend_mac_and_wait(pi->sh->physhim);
    phy_utils_phyreg_enter(pi);
    wlc_phy_txpwr_est_pwr_acphy(pi, power_percore.txpwr_est_Pout, est_Pout_adj);

    /* output the tx pwr est. for the first tx core */
    power->txpwr_est_Pout[0] = power_percore.txpwr_est_Pout[0];
    power->txpwr_est_Pout[1] = power_percore.txpwr_est_Pout[0];
    power->txpwr_est_Pout_gofdm = power_percore.txpwr_est_Pout[0];

    phy_utils_phyreg_exit(pi);
    wlapi_enable_mac(pi->sh->physhim);

    return BCME_OK;
}

#endif

static int8
phy_ac_tpc_get_maxtxpwr_lowlimit(phy_type_tpc_ctx_t *ctx)
{
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = tpci->pi;
    if (CHSPEC_IS2G(pi->radio_chanspec))
        return PHY_TXPWR_MIN_ACPHY_EPA_2G * WLC_TXPWR_DB_FACTOR;
    else
        return PHY_TXPWR_MIN_ACPHY_EPA_5G * WLC_TXPWR_DB_FACTOR;
}

#ifdef PHYCAL_CACHING
void
phy_ac_tpc_save_cache(phy_ac_tpc_info_t *ti, ch_calcache_t *ctx)
{
    phy_info_t *pi = ti->pi;
    uint8 core;
    PHY_CAL(("phy_ac_tpc_save_cache: 0x%x:\n", pi->radio_chanspec));
    FOREACH_CORE(pi, core) {
        /* Save base index */
        ctx->u.acphy_cache.baseidx[core] =
            READ_PHYREGFLDCE(pi, TxPwrCtrlStatus_path, core, baseIndex);
    }
#ifdef BCMDBG
    PHY_CAL(("wl%d: %s: Cached cal values for chanspec 0x%x are:\n",
        pi->sh->unit, __FUNCTION__,  pi->radio_chanspec));
    wlc_phy_cal_cache_dbg_acphy(pi, ctx);
#endif /* BCMDBG */
}
#endif /* PHYCAL_CACHING */

#ifdef RADIO_HEALTH_CHECK

static phy_crash_reason_t
phy_ac_tpc_healthcheck_baseindex(phy_type_tpc_ctx_t *ctx)
{
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t *)ctx;
    phy_info_t *pi = tpci->pi;
    uint8 lb_flag = 0, core;
    uint8 phyrxchain;
    BCM_REFERENCE(phyrxchain);

    /* Disable baseindex health check for 4357 for now */
    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        return PHY_RC_NONE;
    }

#if defined(WLC_TXCAL)
    if (phy_tssical_get_olpc_idx_valid_in_use(pi->tssicali)) {
        lb_flag = 1;
    }
#endif /* defined(WLC_TXCAL) */
    wlapi_suspend_mac_and_wait(pi->sh->physhim);
    phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;
    FOREACH_ACTV_CORE(pi, phyrxchain, core) {
        uint8 baseindex = READ_PHYREGFLDCE(pi, TxPwrCtrlStatus_path, core, baseIndex);
        uint8 baseindex_cck = 0;
        if (TINY_RADIO(pi)) {
            baseindex_cck = READ_PHYREGFLDCE(pi, TxPwrCtrlStatus_cck_path, core,
                baseIndex_cck);
        }
        /* Lower bound check for OLPC */
        if (lb_flag && (baseindex == 127 || baseindex_cck == 127)) {
            uint8 estPower[4], estPwr_adj[4];
            int16 temperature;
            wlc_phy_txpwr_est_pwr_acphy(pi, estPower, estPwr_adj);
            temperature = wlc_phy_tempsense_acphy(pi);
            PHY_FATAL_ERROR_MESG((" %s: baseIndex : %d, baseindex_cck : %d, "
                "estpower[0] : %d,estpower_adj[0]: %d, "
                "estpower[1] : %d,estpower_adj[1]: %d, "
                "estpower[2] : %d,estpower_adj[2]: %d, "
                "estpower[3] : %d,estpower_adj[3]: %d, "
                "temperature : %d\n", __FUNCTION__, baseindex, baseindex_cck,
                estPower[0], estPwr_adj[0], estPower[1], estPwr_adj[1],
                estPower[2], estPwr_adj[2], estPower[3], estPwr_adj[3],
                temperature));
                return (PHY_RC_BASEINDEX_LIMITS);
        }
    }
    wlapi_enable_mac(pi->sh->physhim);
    return PHY_RC_NONE;
}
int phy_ac_tpc_force_fail_baseindex(phy_ac_tpc_info_t *tpci)
{
#if (defined(WLC_TXCAL) || defined(WLTEST))
    uint8 core;
    phy_info_t *pi = tpci->pi;
    uint8 phyrxchain;

    BCM_REFERENCE(phyrxchain);

#if defined(WLC_TXCAL)
    phy_tssical_set_olpc_idx_valid(pi->tssicali, 1);
    phy_tssical_set_olpc_idx_in_use(pi->tssicali, 1);
#endif /* defined(WLC_TXCAL) */
    wlapi_suspend_mac_and_wait(pi->sh->physhim);
    phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;
    FOREACH_ACTV_CORE(pi, phyrxchain, core) {
        wlc_phy_txpwrctrl_set_baseindex(pi, core, 127, 0);
        wlc_phy_txpwrctrl_set_baseindex(pi, core, 127, 1);
    }
    wlapi_enable_mac(pi->sh->physhim);
    /* Send a cts to self for base index to apply */
    wlc_phy_cts2self(pi, 10);
#else
    return BCME_UNSUPPORTED;
#endif
    return BCME_OK;
}
#endif /* RADIO_HEALTH_CHECK */

static int
phy_ac_tpc_get_est_pout(phy_type_tpc_ctx_t *ctx,
    uint8* est_Pout, uint8* est_Pout_adj, uint8* est_Pout_cck)
{
    phy_ac_tpc_info_t *tpc_info = (phy_ac_tpc_info_t *) ctx;
    phy_info_t *pi = tpc_info->pi;

    *est_Pout_cck = 0;

    /* Get power estimates */
    wlapi_suspend_mac_and_wait(pi->sh->physhim);
    phy_utils_phyreg_enter(pi);
    wlc_phy_txpwr_est_pwr_acphy(pi, est_Pout, est_Pout_adj);
    phy_utils_phyreg_exit(pi);
    wlapi_enable_mac(pi->sh->physhim);

    return BCME_OK;
}

static uint8 wlc_phy_percent_to_qdb(uint8 percent)
{
    uint8 qdb;
    if (percent > 90) {
        qdb = 0;
    } else if (percent > 80) {
        qdb = 2;
    } else if (percent > 70) {
        qdb = 4;
    } else if (percent > 60) {
        qdb = 6;
    } else if (percent > 50) {
        qdb = 9;
    } else if (percent > 40) {
        qdb = 12;
    } else if (percent > 30) {
        qdb = 16;
    } else if (percent > 20) {
        qdb = 21;
    } else if (percent > 10) {
        qdb = 28;
    } else {
        qdb = 40;
    }
    return qdb;
}

#ifdef WL11AX
static int
phy_ac_tpc_get_txctrl_mu(phy_type_tpc_ctx_t *ctx, phy_txctrl_info_t *txctrlwd_info)
{
    phy_ac_tpc_info_t *tpc_info = (phy_ac_tpc_info_t *) ctx;
    phy_info_t *pi = tpc_info->pi;

    /* Get power estimates */
    wlapi_suspend_mac_and_wait(pi->sh->physhim);
    phy_utils_phyreg_enter(pi);
    wlc_phy_txpwr_est_pwr_mu_acphy(pi, txctrlwd_info);
    phy_utils_phyreg_exit(pi);
    wlapi_enable_mac(pi->sh->physhim);
    return BCME_OK;
}

static uint16
wlc_phy_64bit_shift_parse(uint64 input, uint8 shift, uint32 bitmap)
{
    uint32 output = 0;
    if (shift < 32) {
        output = (uint32)(input) & 0xffffffff;
    } else {
        output = (uint32)(input >> 32) & 0xffffffff;
        shift = shift - 32;
    }
    output = (uint16)((output >> shift) & bitmap);
    return output;
}

static uint16
wlc_phy_ten_log_ten(uint16 n_sc_clm, uint16 n_sc_dl)
{
    uint16 input;
    switch (n_sc_clm) {
        case 1992:
            input = (n_sc_dl*32 - n_sc_dl)/256; break;
        case 1480:
            input = (n_sc_dl*16 + n_sc_dl*4 + n_sc_dl)/128; break;
        case 996:
            input = (n_sc_dl*32 - n_sc_dl)/128; break;
        case 752:
            input = (n_sc_dl*32 + n_sc_dl*8 + n_sc_dl)/128; break;
        case 484:
            input = n_sc_dl*2; break;
        case 348:
            input = (n_sc_dl*64 + n_sc_dl*16 + n_sc_dl*8 + n_sc_dl)/128; break;
        case 242:
            input = n_sc_dl; break;
        case 182:
            input = n_sc_dl + (n_sc_dl*16 + n_sc_dl*4 + n_sc_dl)/64; break;
        case 106:
            return 0;
        default:
            return 0;
    }
    input = (input/8) - 12;
    if (input > 19) input = 19;
    return log10_tbl[input];
}

static void
wlc_phy_txctrl_hesigext_parse(phy_info_t *pi,
        uint64 *val, phy_ac_txctrlwd_info_t *txctrlwd)
{
    uint8 j;
    /* Set CP_LTF_SIZE 0xff to indicate invalid HESIG-EXT fields */
    txctrlwd->hesig_ext[HESIGEXT_CPLTFSIZE] = 0xff;
    if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        if ((txctrlwd->common_info[TXCTRL_FRAME_TYPE] == WL_FRAME_TYPE_HE) &&
            (txctrlwd->common_info[TXCTRL_HE_FORMAT] == WL_HE_FORMAT_TRI)) {
            for (j = 0; j < HESIGEXT_SIZE; j++) {
                txctrlwd->hesig_ext[j] = wlc_phy_64bit_shift_parse(
                                             val[txctrlwd_tbl_hesigext[j][0]],
                                             (uint8)txctrlwd_tbl_hesigext[j][1],
                                             txctrlwd_tbl_hesigext[j][2]);
            }
        }
    }
}
#endif /* WL11AX */

#ifdef TXPWRBACKOFF
#if defined(WLTXPWR_CACHE)
static bool
phy_ac_ppr_check_vt_backoff(phy_type_tpc_ctx_t *ctx, chanspec_t chspec)
{
    int8 maxpwr, cached_maxpwr;
    int8 vbat_tempsense_pwrbackoff;
    bool cache_ok = TRUE;
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t *) ctx;
    phy_info_t *pi = tpci->pi;
    if (tpci->phy_txpwrbackoff->enable) {
        /* boardlimits for core0 */
        wlc_phy_get_max_srom_txpower_band_acphy(pi, chspec, &maxpwr, 0, 1);
        cached_maxpwr = wlc_phy_get_cached_boardlim(pi->txpwr_cache, chspec, 0);
        vbat_tempsense_pwrbackoff = cached_maxpwr - maxpwr;
        cache_ok = (vbat_tempsense_pwrbackoff ==
                tpci->phy_txpwrbackoff->vbat_tempsense_pwrbackoff);
        PHY_INFORM(("**** phy_ac_ppr_check_vt_backoff: %d %d %d %d\n",
                (int)maxpwr, (int)cached_maxpwr,
                (int)vbat_tempsense_pwrbackoff, (int)cache_ok));
    }
    return cache_ok;

}
#endif /* WLTXPWR_CACHE */

uint8
wlc_phy_get_max_srom_txpower_band_acphy(phy_info_t *pi, chanspec_t chanspec,
        int8* p_maxpwr, uint8 core, uint8 maxcore)
{
    uint8 band = 0;
    int8 tmp_max_pwr = 0;
    maxcore = MIN(maxcore, PHYCORENUM(pi->pubpi->phy_corenum));
    if (SROMREV(pi->sh->sromrev) < 12) {
        int8 core_freq_segment_map;
        srom11_pwrdet_t *pwrdet11 = pi->pwrdet_ac;
        /* core_freq_segment_map is only required for 80P80 mode.
         *  For other modes, it is ignored
         */
        core_freq_segment_map =
            phy_ac_chanmgr_get_data(pi->u.pi_acphy->chanmgri)->core_freq_mapping[core];
        /* to figure out which subband is in 5G */
        /* in the range of 0, 1, 2, 3 */
        band = phy_ac_chanmgr_get_chan_freq_range(pi, chanspec, core_freq_segment_map);
        tmp_max_pwr = pwrdet11->max_pwr[0][band];
        if (maxcore > 1)
            tmp_max_pwr = MIN(tmp_max_pwr, pwrdet11->max_pwr[1][band]);
        if (maxcore > 2)
            tmp_max_pwr = MIN(tmp_max_pwr, pwrdet11->max_pwr[2][band]);
    } else if (SROMREV(pi->sh->sromrev) == 12) {
        srom12_pwrdet_t *pwrdet = pi->pwrdet_ac;
        /* to figure out which subband is in 5G */
        /* in the range of 0, 1, 2, 3, 4 */
        band = phy_ac_chanmgr_chan2freq_range_srom12(pi, chanspec);
        if (band >= WL_CHAN_FREQ_RANGE_2G_40) {
            if (CHSPEC_IS80(chanspec)) {
                band = band - WL_CHAN_FREQ_RANGE_5G_BAND0_80 + 1;
            } else {
                ASSERT(CHSPEC_IS40(chanspec));
                band = band - WL_CHAN_FREQ_RANGE_2G_40;
            }
        }
        tmp_max_pwr = pwrdet->max_pwr[0][band];
        if (maxcore > 1)
            tmp_max_pwr = MIN(tmp_max_pwr, pwrdet->max_pwr[1][band]);
        if (maxcore > 2)
            tmp_max_pwr = MIN(tmp_max_pwr, pwrdet->max_pwr[2][band]);
    } else {
        ASSERT(0);
    }
    *p_maxpwr = tmp_max_pwr;
    return band;
}

int16
phy_ac_tpc_get_vt_pwrbackoff(phy_type_tpc_ctx_t *ctx)
{
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t*)ctx;
    return tpci->phy_txpwrbackoff->vbat_tempsense_pwrbackoff;
}

/* Set Tx power backoff */
int
phy_ac_tpc_txbackoff_set(phy_type_tpc_ctx_t *ctx, int16 txpwr_backoff)
{
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t*)ctx;

    if (!tpci->phy_txpwrbackoff->enable) {
        return (BCME_ERROR);
    }

    if (txpwr_backoff > (-1 * TXPWRBACKOFF_RANGE_LO) ||
        txpwr_backoff < (-1 * TXPWRBACKOFF_RANGE_HI)) {
        return (BCME_RANGE);
    }

    tpci->phy_txpwrbackoff->vbat_tempsense_pwrbackoff = txpwr_backoff;
    wlapi_high_txpwr_limit_update_req(tpci->pi->sh->physhim);

    PHY_INFORM(("%s: wl%u set tx power backoff %d (in .25dB)\n",
        __FUNCTION__, tpci->pi->sh->unit,
        tpci->phy_txpwrbackoff->vbat_tempsense_pwrbackoff));

    return (BCME_OK);
}

bool
phy_ac_tpc_enable_power_backoff(phy_type_tpc_ctx_t *ctx, bool enable)
{
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t *)ctx;

    if (TXPWRBACKOFF_ENAB(tpci)) {
        tpci->phy_txpwrbackoff->enable = enable ? TRUE : FALSE;
        if (!tpci->phy_txpwrbackoff->enable) {
            tpci->phy_txpwrbackoff->vbat_tempsense_pwrbackoff = 0;
        }
    }
    return (tpci->phy_txpwrbackoff->enable);
}

bool
phy_ac_tpc_is_enabled_power_backoff(phy_type_tpc_ctx_t *ctx)
{
    phy_ac_tpc_info_t *tpci = (phy_ac_tpc_info_t *)ctx;

    return (tpci->phy_txpwrbackoff->enable);
}

int
phy_ac_tpc_dump_power_backoff(phy_type_tpc_ctx_t *ctx, phy_txpwrbackoff_info_t *pdata)
{
    phy_txpwrbackoff_t *plocal = ((phy_ac_tpc_info_t *)ctx)->phy_txpwrbackoff;

    ASSERT(pdata);
    if (!pdata) {
        return BCME_ERROR;
    }

    pdata->enable = plocal->enable;
    pdata->vbat_tempsense_pwrbackoff = plocal->vbat_tempsense_pwrbackoff;

    return BCME_OK;
}

#endif /* TXPWRBACKOFF */

static void
wlc_mac_register_init_for_hwppr(phy_info_t *pi)
{
    uint16 regVal;
    uint16 tpcBackOffDlOfdma = 6;                /* reg(TXE_PPR_CFG0) */
    uint16 tpcTargetPwr = 127, tpcBackOffTb = 6;        /* reg(TXE_PPR_CFG1) */
    uint16 tpcBackOffSu = 6;                /* reg(TXE_PPR_CFG1) */
    uint16 ruSpreadTh0Bw20 = 4, lubScaleBw20 = 340;        /* reg(TXE_PPR_CFG2) */
    uint16 ruSpreadTh1Bw20 = 7, ubScaleBw20 = 242;        /* reg(TXE_PPR_CFG3) */
    uint16 ruSpreadTh0Bw40 = 8, lubScaleBw40 = 178;        /* reg(TXE_PPR_CFG4) */
    uint16 ruSpreadTh1Bw40 = 13, ubScaleBw40 = 256;        /* reg(TXE_PPR_CFG5) */
    uint16 ruSpreadTh0Bw80 = 18, lubScaleBw80= 82;        /* reg(TXE_PPR_CFG6) */
    uint16 ruSpreadTh1Bw80 = 27, ubScaleBw80 = 128;        /* reg(TXE_PPR_CFG7) */
    uint16 ruSpreadTh0Bw160 = 36, lubScaleBw160 = 42;    /* reg(TXE_PPR_CFG8) */
    uint16 ruSpreadTh1Bw160 = 54, ubScaleBw160 = 62;    /* reg(TXE_PPR_CFG9) */
    uint16 vhtMuFloorBackoff = 38, heMuFloorBackoff = 40;    /* reg(TXE_PPR_CFG10) */
    uint16 stpwrBoff1ssTQ = 31, stpwrBoff2ssTQ = 0;        /* reg(TXE_PPR_CFG11) */
    uint16 stpwrBoff3ssTQ = 4, stpwrBoff4ssTQ = 6;        /* reg(TXE_PPR_CFG12) */
    uint16 stpwrBoff1ssNQ = 31, stpwrBoff2ssNQ = 2;        /* reg(TXE_PPR_CFG13) */
    uint16 stpwrBoff3ssNQ = 6, stpwrBoff4ssNQ = 8;        /* reg(TXE_PPR_CFG14) */
    uint16 stpwrBoffSndTQ = 0, stpwrBoffSndNQ = 2;        /* reg(TXE_PPR_CFG15) */
    uint16 minTxPwr = 216;                    /* reg(TXE_PPR_CFG16) */
    uint16 AdjPwr = 0, maxAllowedTxPwr = 127;        /* reg(TXE_PPR_CTL0) */

    if (ACMAJORREV_130(pi->pubpi->phy_rev) && ACMINORREV_1(pi)) {
        /* for 6715B0-HWPPR, use max board_limit set */
        tpcTargetPwr = pi->tpci->data->txpwr_max_boardlim_percore[0];
    }

    /* Initialize MAC register for HW-PPR */
    /* set reg(TXE_PPR_CFG0) */
    regVal = (tpcBackOffDlOfdma << 8);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG0_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG1) */
    regVal = wlc_bitor_reg(tpcTargetPwr, tpcBackOffTb, 8);
    regVal = wlc_bitor_reg(regVal, tpcBackOffSu, 12);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG1_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG2) */
    regVal = wlc_bitor_reg(ruSpreadTh0Bw20, lubScaleBw20, 6);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG2_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG3) */
    regVal = wlc_bitor_reg(ruSpreadTh1Bw20, ubScaleBw20, 6);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG3_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG4) */
    regVal = wlc_bitor_reg(ruSpreadTh0Bw40, lubScaleBw40, 6);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG4_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG5) */
    regVal = wlc_bitor_reg(ruSpreadTh1Bw40, ubScaleBw40, 6);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG5_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG6) */
    regVal = wlc_bitor_reg(ruSpreadTh0Bw80, lubScaleBw80, 6);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG6_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG7) */
    regVal = wlc_bitor_reg(ruSpreadTh1Bw80, ubScaleBw80, 6);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG7_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG8) */
    regVal = wlc_bitor_reg(ruSpreadTh0Bw160, lubScaleBw160, 6);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG8_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG9) */
    regVal = wlc_bitor_reg(ruSpreadTh1Bw160, ubScaleBw160, 6);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG9_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG10) */
    regVal = wlc_bitor_reg(vhtMuFloorBackoff, heMuFloorBackoff, 8);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG10_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG11) */
    regVal = wlc_bitor_reg(stpwrBoff1ssTQ, stpwrBoff2ssTQ, 8);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG11_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG12) */
    regVal = wlc_bitor_reg(stpwrBoff3ssTQ, stpwrBoff4ssTQ, 8);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG12_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG13) */
    regVal = wlc_bitor_reg(stpwrBoff1ssNQ, stpwrBoff2ssNQ, 8);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG13_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG14) */
    regVal = wlc_bitor_reg(stpwrBoff3ssNQ, stpwrBoff4ssNQ, 8);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG14_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG15) */
    regVal = wlc_bitor_reg(stpwrBoffSndTQ, stpwrBoffSndNQ, 8);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG15_OFFSET(pi), regVal);
    /* set reg(TXE_PPR_CFG16) */
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CFG16_OFFSET(pi), minTxPwr);
    /* set reg(TXE_PPR_CTL0) */
    regVal = wlc_bitor_reg(AdjPwr, maxAllowedTxPwr, 8);
    wlapi_bmac_write_ihr(pi->sh->physhim, D11_TXE_PPR_CTL0_OFFSET(pi), regVal);

    PHY_TXPWR(("%s: ============= HW-PPR MAC Registers ============\n", __FUNCTION__));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG0 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG0_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG1 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG1_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG2 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG2_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG3 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG3_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG4 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG4_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG5 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG5_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG6 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG6_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG7 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG7_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG8 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG8_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG9 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG9_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG10 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG10_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG11 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG11_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG12 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG12_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG13 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG13_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG14 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG14_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG15 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG15_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CFG16 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CFG16_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CTL0 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CTL0_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CTL1 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CTL1_OFFSET(pi))));
    PHY_TXPWR(("%s: D11_TXE_PPR_CTL2 = %d\n", __FUNCTION__,
            wlapi_bmac_read_ihr(pi->sh->physhim, D11_TXE_PPR_CTL2_OFFSET(pi))));

    /* When initial loading, MAC register for hwppr is set to initial hwpprctrl */
    if (ACMAJORREV_130(pi->pubpi->phy_rev) && ACMINORREV_1(pi)) {
        wlc_phy_hwppr_en(pi, pi->hwpprctrl);
    }
}

static uint16
wlc_bitor_reg(uint16 reg1, uint16 reg2, int lshft_reg2)
{
    return (reg1 | reg2 << lshft_reg2);
}
