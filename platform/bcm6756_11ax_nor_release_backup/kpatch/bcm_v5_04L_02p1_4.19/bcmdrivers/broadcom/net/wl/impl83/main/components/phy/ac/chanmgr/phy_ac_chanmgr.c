/*
 * ACPHY Channel Manager module implementation
 *
 * Copyright 2021 Broadcom
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
 * $Id: phy_ac_chanmgr.c 801805 2021-08-04 03:06:52Z $
 */

#include <wlc_cfg.h>
#if (ACCONF != 0) || (ACCONF2 != 0) || (ACCONF5 != 0)
#include <phy_cfg.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <phy_dbg.h>
#include <phy_mem.h>
#include <bcm_math.h>
#include <qmath.h>
#include "phy_type_chanmgr.h"
#include <phy_chanmgr_api.h>
#include <phy_ac.h>
#include <phy_ac_chanmgr.h>
#include <phy_ac_antdiv.h>
#include <phy_ac_tpc.h>
#include <phy_papdcal.h>
/* ************************ */
/* Modules used by this module */
/* ************************ */
#include <wlc_radioreg_20693.h>
#include <wlc_radioreg_20698.h>
#include <wlc_radioreg_20704.h>
#include <wlc_radioreg_20707.h>
#include <wlc_radioreg_20708.h>
#include <wlc_radioreg_20709.h>
#include <wlc_radioreg_20710.h>
#include <wlc_phy_radio.h>
#include <wlc_phy_shim.h>
#include <wlc_phyreg_ac.h>
#include <wlc_phytbl_20693.h>
#include <wlc_phytbl_20698.h>
#include <wlc_phytbl_20704.h>
#include <wlc_phytbl_20707.h>
#include <wlc_phytbl_20708.h>
#include <wlc_phytbl_20709.h>
#include <wlc_phytbl_20710.h>
#include <wlc_phytbl_ac.h>
#include <phy_btcx.h>
#include <phy_tpc.h>
#include <phy_ac_dccal.h>
#include <phy_ac_noise.h>
#include <phy_ac_hirssi.h>
#include <phy_rxgcrs_api.h>
#include <phy_rxiqcal.h>
#include <phy_txiqlocal.h>
#include <phy_ac_rxiqcal.h>
#include <phy_ac_txiqlocal.h>
#include <phy_rxgcrs.h>
#include <hndpmu.h>
#include <sbchipc.h>
#include <phy_utils_reg.h>
#include <phy_utils_channel.h>
#include <phy_utils_math.h>
#include <phy_utils_var.h>
#include <phy_ac_info.h>
#include <phy_ocl_api.h>
#include <phy_stf.h>
#include <phy_radar_st.h>
#include <phy_smc_api.h>
#include <phy_vasip_api.h>
#include <phy_wareng_api.h>
#include <phy_rxspur_api.h>
#include <phy_utils_pmu.h>

#ifdef WL_ETMODE
#include <phy_ac_et.h>
#endif /* WL_ETMODE */

#ifdef WL_DSI
#include "phy_ac_dsi.h"
#endif /* WL_DSI */

#include <phy_rstr.h>

#ifdef WLC_SW_DIVERSITY
#include "phy_ac_antdiv.h"
#endif

#ifndef ALL_NEW_PHY_MOD
/* < TODO: all these are going away... */
#include <wlc_phy_int.h>
/* TODO: all these are going away... > */
#endif
#include <bcmdevs.h>

#ifdef WL_DSI
#include "fcbs.h"
#endif /* WL_DSI */

#ifdef WL_AIR_IQ
enum {
    lna2g_cfg1 = 0,
    lna2g_cfg2,
    lna5g_cfg1,
    lna5g_cfg2,
    lna5g_cfg3,
    lna5g_cfg4,
    logen2g_rccr,
    logen5g_epapd,
    logen5g_rccr,
    rx_bb_2g_ovr_east,
    rxiqmix_cfg1,
    rxmix2g_cfg1,
    rxrf2g_cfg2,
    rx_top_2g_ovr_east2,
    rx_top_2g_ovr_east,
    rx_top_5g_ovr,
    tia_cfg1,
    tia_cfg5,
    tia_cfg6,
    tia_cfg7,
    tia_cfg8,
    lo2g_logen0_cfg1,
    lo2g_logen1_cfg1,
    lo2g_vco_drv_cfg1,
    lo5g_core0_cfg1,
    lo5g_core1_cfg1,
    lo5g_core2_cfg1,
    REGS_CNT_20693_3PLUS1
};
typedef struct {
    int8    save_count;
    uint16    regs[REGS_CNT_20693_3PLUS1];
} phy_ac_chanmgr_20693_3plus1_regs_t;
#endif /* WL_AIR_IQ */

#define RFSEQEXT_TBL_SZ_PER_CORE_28NM 11
#define NUM_BFD_CQI_INDEX 128
#define CQI_RPT_HEADER_RPTSIZE_SHIFT 2
#define CQI_RPT_HEADER_PKTBW_SHIFT 14
#define CQI_RPT_HEADER_PHYBW_SHIFT 16
#define CQI_RPT_HEADER_RUEND_SHIFT 18

/* module private states */
typedef struct phy_ac_chanmgr_config_info {
    uint8    srom_tssisleep_en; /* TSSI sleep enable */
    uint8    srom_txnoBW80ClkSwitch; /* 5G Tx BW80 AFE CLK switch */
    uint8    vlinpwr2g_from_nvram;
    uint8    vlinpwr5g_from_nvram;
    int8    srom_papdwar; /* papd war enable and threshold */
    bool    srom_paprdis; /* papr disable */
    bool    srom_nonbf_logen_mode_en;    /* to enable non-bf logen mode  */
} phy_ac_chanmgr_config_info_t;

struct phy_ac_chanmgr_info {
    phy_info_t        *pi;
    phy_ac_info_t        *aci;
    phy_chanmgr_info_t    *cmn_info;
    phy_ac_chanmgr_data_t *data; /* shared data */
    phy_ac_chanmgr_config_info_t *cfg; /* configuration params */
    chan_info_tx_farrow(*tx_farrow)[ACPHY_NUM_CHANS];
    chan_info_rx_farrow(*rx_farrow)[ACPHY_NUM_CHANS];
    uint8    acphy_cck_dig_filt_type;
    uint8    chsm_en, ml_en;
    uint8    use_fast_adc_20_40;
    uint8    acphy_enable_smth;
    uint8    acphy_smth_dump_mode;
    int8    tdcs_160_en;
    int8    chanup_ovrd;
    int8    lowratetssi_ovrd;
    int8    papr_en_iovar;
    int16    papr_gamma_iovar;
    int8    loftcompshift_ovrd;
    bool    vco_12GHz;
    bool    FifoReset; /* flag to hold FifoReset val */
    chanspec_t    radio_chanspec_sc;    /* 3x3_1x1 mode, setting */
    /* lesi_perband: used for NVRAM based LESI enable/disable
    * 1st entry: 2G, 2nd entry: 5G
    */
    int8 lesi_perband[2];
    bool bypass_idle_tssi;
    bool veloce_farrow_db;
    /* add other variable size variables here at the end */

    uint16 crsTxExtension;
#ifdef WL_AIR_IQ
    phy_ac_chanmgr_20693_3plus1_regs_t regs_save_20693_3plus1;
#endif
    int orig_min_fm_lp;
    int orig_max_span_lp;
    int orig_st_level_time;
    int origSigFld1Decode;
    int origSdFeClkEn;
    int origEdCrsEn;
    int origEdCrsEn80u;

    /* per-tone reciprocity compensation coefficients
    * used for implicit beamforming calibration
    */
    uint16 recip_packed_majrev32_33_37[256][3];
};

typedef struct {
    phy_ac_chanmgr_info_t info;
    phy_ac_chanmgr_data_t data;
    phy_ac_chanmgr_config_info_t cfg;
} phy_ac_chanmgr_mem_t;

#define N_BITS_RX_FARR (24)

#define ZEROS_TBL_SZ    64        /* in 32-bit DWORDs */

/* 20693 Radio functions */

/* local functions */
static void phy_ac_chanmgr_write_tx_farrow_tiny(phy_info_t *pi, chanspec_t chanspec,
    chanspec_t chanspec_sc, int sc_mode);
static void phy_ac_chanmgr_chanspec_set(phy_type_chanmgr_ctx_t *ctx, chanspec_t chanspec);
static void phy_ac_chanmgr_upd_interf_mode(phy_type_chanmgr_ctx_t *ctx, chanspec_t chanspec);
static int phy_ac_chanmgr_get_chanspec_bandrange(phy_type_chanmgr_ctx_t *ctx, chanspec_t chanspec);
static bool phy_ac_chanmgr_is_txbfcal(phy_type_chanmgr_ctx_t *ctx);
static bool phy_ac_chanmgr_is_smth_en(phy_type_chanmgr_ctx_t *ctx, bool ishe);
static void phy_ac_chanmgr_smth_per_nss_SUTxbf(phy_type_chanmgr_ctx_t *ctx, uint8 uint_val);
static uint8 phy_ac_chanmgr_chan2freq_range(phy_info_t *pi, chanspec_t chanspec);
static void wlc_tiny_setup_coarse_dcc(phy_info_t *pi);
void wlc_phy_toggle_resetcca_hwbit_acphy(phy_info_t *pi);
static void phy_ac_chanmgr_bypass_itssi(phy_type_chanmgr_ctx_t *ctx, bool force);
#if defined(WLTEST)
static int phy_ac_chanmgr_get_smth(phy_type_chanmgr_ctx_t *ctx, int32* ret_int_ptr);
static int phy_ac_chanmgr_set_smth(phy_type_chanmgr_ctx_t *ctx, int8 int_val);
#endif /* defined(WLTEST) */
static void phy_ac_chanmgr_tdcs_enable_160m(phy_info_t *pi, bool set_val);
static void phy_ac_chanmgr_tdcs_160m_rev47(phy_info_t *pi);
static void phy_ac_chanmgr_class_rev47(phy_info_t *pi);
static void phy_ac_chanmgr_sr_enable(phy_info_t *pi, uint8 set_val);
static void phy_ac_chanmgr_srg_bssColor(phy_info_t *pi, uint8 set_val, uint16 BM0, uint16 BM1,
    uint16 BM2, uint16 BM3);
static void phy_ac_chanmgr_pad_online_enable(phy_info_t *pi, bool set_val, bool up_check);
static void phy_ac_chanmgr_set_spexp_matrix(phy_info_t *pi);
static void phy_ac_chanmgr_dccal_force(phy_info_t *pi);
static void femctrl_clb_majrev_ge40(phy_info_t *pi, int band_is_2g, int slice);
static void wlc_phy_bphymrc_hwconfig(phy_info_t *pi);
static void phy_ac_chanmgr_set_bandsel_on_gpio(phy_info_t *pi, bool band2g, int gpio);
static const char BCMATTACHDATA(rstr_VlinPwr2g_cD)[]                  = "VlinPwr2g_c%d";
static const char BCMATTACHDATA(rstr_VlinPwr5g_cD)[]                  = "VlinPwr5g_c%d";
static const char BCMATTACHDATA(rstr_Vlinmask2g_cD)[]                 = "Vlinmask2g_c%d";
static const char BCMATTACHDATA(rstr_Vlinmask5g_cD)[]                 = "Vlinmask5g_c%d";
static const char BCMATTACHDATA(rstr_AvVmid_cD)[]                     = "AvVmid_c%d";

/* function to read femctrl params from nvram */
static void BCMATTACHFN(phy_ac_chanmgr_nvram_attach)(phy_ac_chanmgr_info_t *ac_info);
static void phy_ac_chanmgr_std_params_attach(phy_ac_chanmgr_info_t *ac_info);
static int phy_ac_chanmgr_init_chanspec(phy_type_chanmgr_ctx_t *ctx);
static int WLBANDINITFN(phy_ac_chanmgr_bsinit)(phy_type_chanmgr_ctx_t *ctx, chanspec_t chanspec,
    bool forced);
static int WLBANDINITFN(phy_ac_chanmgr_bwinit)(phy_type_chanmgr_ctx_t *ctx, chanspec_t chanspec);
static bool BCMATTACHFN(phy_ac_chanmgr_attach_farrow)(phy_ac_chanmgr_info_t *chanmgri);
static bool BCMATTACHFN(phy_ac_chanmgr_attach_paprr)(phy_ac_chanmgr_info_t *ac_info);

static void wlc_phy_set_papdlut_dynradioregtbl_majrev_ge40(phy_info_t *pi);
static void wlc_phy_set_analog_lpf(phy_info_t *pi, uint8 *tx_bq1, uint8 *tx_bq2,
                                   uint8 *rx_bq1, uint8 *rx_bq2);

#ifdef WL_AIR_IQ
static void wlc_phy_radio20693_setup_logen_3plus1(phy_info_t *pi, chanspec_t chanspec,
    chanspec_t chanspec_sc);
static void wlc_phy_radio20693_save_3plus1(phy_info_t *pi, uint8 core, uint8 restore);
static void wlc_phy_save_regs_3plus1(phy_info_t *pi, int32 set_val);
#endif
/* register phy type specific implementation */
phy_ac_chanmgr_info_t *
BCMATTACHFN(phy_ac_chanmgr_register_impl)(phy_info_t *pi, phy_ac_info_t *aci,
    phy_chanmgr_info_t *cmn_info)
{
    phy_ac_chanmgr_info_t *ac_info;
    phy_type_chanmgr_fns_t fns;

    PHY_TRACE(("%s\n", __FUNCTION__));

    /* allocate all storage together */
    if ((ac_info = phy_malloc(pi, sizeof(phy_ac_chanmgr_mem_t))) == NULL) {
        PHY_ERROR(("%s: phy_malloc failed\n", __FUNCTION__));
        goto fail;
    }

    /* Initialize params */
    ac_info->pi = pi;
    ac_info->aci = aci;
    ac_info->cmn_info = cmn_info;
    ac_info->data = &(((phy_ac_chanmgr_mem_t *) ac_info)->data);
    ac_info->cfg = &(((phy_ac_chanmgr_mem_t *) ac_info)->cfg);
    ac_info->radio_chanspec_sc = pi->radio_chanspec;

    if (!TINY_RADIO(pi) && !phy_ac_chanmgr_attach_farrow(ac_info)) {
        PHY_ERROR(("%s: phy_ac_chanmgr_attach_farrow failed\n", __FUNCTION__));
        goto fail;
    }
    if (ACMAJORREV_128(pi->pubpi->phy_rev) && !phy_ac_chanmgr_attach_paprr(ac_info)) {
        PHY_ERROR(("%s: phy_ac_chanmgr_attach_paprr failed\n", __FUNCTION__));
        goto fail;
    }
    /* register PHY type specific implementation */
    bzero(&fns, sizeof(fns));
    fns.init_chanspec = phy_ac_chanmgr_init_chanspec;
    fns.is_txbfcal = phy_ac_chanmgr_is_txbfcal;
    fns.is_smth_en = phy_ac_chanmgr_is_smth_en;
    fns.smth_per_nss_SUTxbf = phy_ac_chanmgr_smth_per_nss_SUTxbf;
    fns.get_bandrange = phy_ac_chanmgr_get_chanspec_bandrange;
    fns.chanspec_set = phy_ac_chanmgr_chanspec_set;
    fns.interfmode_upd = phy_ac_chanmgr_upd_interf_mode;
    fns.tdcs_enable_160m = phy_ac_chanmgr_tdcs_enable_160m;
    fns.sr_enable = phy_ac_chanmgr_sr_enable;
    fns.srg_bssColor = phy_ac_chanmgr_srg_bssColor;
    fns.pad_online_enable = phy_ac_chanmgr_pad_online_enable;
    fns.dccal_force = phy_ac_chanmgr_dccal_force;
    fns.bypass_itssi = phy_ac_chanmgr_bypass_itssi;
#if defined(WLTEST)
    fns.get_smth = phy_ac_chanmgr_get_smth;
    fns.set_smth = phy_ac_chanmgr_set_smth;
#endif /* defined(WLTEST) */
    fns.bsinit = phy_ac_chanmgr_bsinit;
    fns.bwinit = phy_ac_chanmgr_bwinit;
    fns.ctx = ac_info;

    /* Read VLIN signal parameters from NVRAM */
    if (!TINY_RADIO(pi))
        wlc_phy_nvram_vlin_params_read(pi);
    /* Read AVVMID signal from NVARM */
    wlc_phy_nvram_avvmid_read(pi);

    /* set up srom cfg */
    phy_ac_chanmgr_nvram_attach(ac_info);
    phy_ac_chanmgr_std_params_attach(ac_info);

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        if (ISSIM_ENAB(pi->sh->sih))
            pi->u.pi_acphy->sromi->srom_low_adc_rate_en = 0;
        else {
            pi->u.pi_acphy->sromi->srom_low_adc_rate_en =
                (uint8)PHY_GETINTVAR_DEFAULT_SLICE(pi,
                                                   rstr_low_adc_rate_en, 1);
            if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
                if (ac_info->lowratetssi_ovrd == -1) {
                    if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en == 1)
                        pi->u.pi_acphy->sromi->srom_low_adc_rate_en = 1;
                    else
                        pi->u.pi_acphy->sromi->srom_low_adc_rate_en = 0;
                } else {
                    pi->u.pi_acphy->sromi->srom_low_adc_rate_en =
                                    ac_info->lowratetssi_ovrd;
                }
            }
        }
    } else {
        pi->u.pi_acphy->sromi->srom_low_adc_rate_en = (uint8)PHY_GETINTVAR_DEFAULT_SLICE(pi,
            rstr_low_adc_rate_en, 0);
    }

    if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en >= 2) {
        PHY_ERROR(("%s: ERROR: wrong low_adc_rate_en value (%d)\n", __FUNCTION__,
            pi->u.pi_acphy->sromi->srom_low_adc_rate_en));
        goto fail;
    }

    if (phy_chanmgr_register_impl(cmn_info, &fns) != BCME_OK) {
        PHY_ERROR(("%s: phy_chanmgr_register_impl failed\n", __FUNCTION__));
        goto fail;
    }
#ifdef WL_AIR_IQ
    ac_info->regs_save_20693_3plus1.save_count = 0;
#endif
    memset(ac_info->recip_packed_majrev32_33_37, 0,
        sizeof(ac_info->recip_packed_majrev32_33_37));
    return ac_info;

    /* error handling */
fail:

    phy_ac_chanmgr_unregister_impl(ac_info);
    return NULL;
}

void
BCMATTACHFN(phy_ac_chanmgr_unregister_impl)(phy_ac_chanmgr_info_t *ac_info)
{
    phy_info_t *pi;
    phy_chanmgr_info_t *cmn_info;
    int num_bw;
#ifdef ACPHY_1X1_ONLY
    num_bw = 1;
#else
    num_bw = ACPHY_NUM_BW;
#endif

    if (ac_info == NULL)
        return;

    pi = ac_info->pi;
    cmn_info = ac_info->cmn_info;

    PHY_TRACE(("%s\n", __FUNCTION__));

    /* unregister from common */
    phy_chanmgr_unregister_impl(cmn_info);

    if (ac_info->tx_farrow != NULL) {
        phy_mfree(pi, ac_info->tx_farrow,
            num_bw * sizeof(*(ac_info->tx_farrow)));
    }
    if (ac_info->rx_farrow != NULL) {
        phy_mfree(pi, ac_info->rx_farrow,
            num_bw * sizeof(*(ac_info->rx_farrow)));
    }

    phy_mfree(pi, ac_info, sizeof(phy_ac_chanmgr_mem_t));
}

static void
BCMATTACHFN(phy_ac_chanmgr_std_params_attach)(phy_ac_chanmgr_info_t *chanmgri)
{
    phy_info_t *pi = chanmgri->pi;
    BCM_REFERENCE(pi);
    chanmgri->data->curr_band2g = CHSPEC_IS2G(chanmgri->pi->radio_chanspec);
    chanmgri->aci->curr_bw = CHSPEC_BW(chanmgri->pi->radio_chanspec);
    chanmgri->data->fast_adc_en = 0;
    chanmgri->use_fast_adc_20_40 = 0;
    if (ACMAJORREV_4(pi->pubpi->phy_rev) ||    ACMAJORREV_128(pi->pubpi->phy_rev)) {
        if (chanmgri->chsm_en) {
            chanmgri->acphy_enable_smth = SMTH_ENABLE;
        } else {
            chanmgri->acphy_enable_smth = SMTH_DISABLE;
        }
    } else if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
            ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        chanmgri->acphy_enable_smth = 0;

        if (ACMAJORREV_33(pi->pubpi->phy_rev) ||
            ACMAJORREV_47(pi->pubpi->phy_rev) ||
            ACMAJORREV_129(pi->pubpi->phy_rev) ||
            ACMAJORREV_130(pi->pubpi->phy_rev) ||
            (ACMAJORREV_131(pi->pubpi->phy_rev) && !ACMINORREV_0(pi))) {
            /* channel smoothing is not supported in 80p80 */
            if (!PHY_AS_80P80(pi, pi->radio_chanspec))
                chanmgri->acphy_enable_smth = 1;
        } else {
            chanmgri->acphy_enable_smth = 0;
        }
    }
    chanmgri->acphy_smth_dump_mode = SMTH_NODUMP;
    chanmgri->tdcs_160_en = 0; /* Disable 160M TDCS by default for 43684 B0 */
    chanmgri->chanup_ovrd = -1; /* Auto */
    chanmgri->lowratetssi_ovrd = -1; /* Default */
    chanmgri->papr_en_iovar = -1; /* Default */
    chanmgri->papr_gamma_iovar = -1; /* Default */
    chanmgri->loftcompshift_ovrd = -1; /* Default */
    chanmgri->data->vco_pll_adjust_mode = 2; /* auto switch mode for spur ch pll settings */
    pi->u.pi_acphy->c2csync_dac_clks_on = FALSE;
    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        /* 4349A0 uses 12Ghz VCO */
        chanmgri->vco_12GHz = TRUE;
    } else {
        /* Disable 12Ghz VCO for all other chips */
        chanmgri->vco_12GHz = FALSE;
    }

    chanmgri->veloce_farrow_db = FALSE;
    chanmgri->bypass_idle_tssi = FALSE;
    if (ISSIM_ENAB(pi->sh->sih)) {
        /* Only for QT/Veloce */
        if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            chanmgri->veloce_farrow_db =
                (READ_PHYREG(pi, BfrConfigReg0) & 0x200) ? 1 : 0;
        } else if (ACMAJORREV_47_129(pi->pubpi->phy_rev)) {
            /* 43684 veloce db uses this bit to signify that its a farrow db */
            /* FIXME_6710: might need to use a different bit for 6710 */
            chanmgri->veloce_farrow_db =  READ_PHYREGFLD(pi,
                    spur_can_fll_enable_p0, pi_en);
        }
    }

    /* 40 * (6-1) = 200; 6us of extended TX CRS (MAC is also assuming this),
       1us is subtracted to take care of extra tail length.
    */
    chanmgri->crsTxExtension = 200;
}

static int
WLBANDINITFN(phy_ac_chanmgr_init_chanspec)(phy_type_chanmgr_ctx_t *ctx)
{
    phy_ac_chanmgr_info_t *chanmgri = (phy_ac_chanmgr_info_t *)ctx;
    phy_info_acphy_t *pi_ac = chanmgri->aci;
    phy_info_t *pi = chanmgri->pi;
#ifdef WL_AIR_IQ
    uint16 phymode = phy_get_phymode(chanmgri->pi);
#endif /* WL_AIR_IQ */

    chanmgri->data->both_txchain_rxchain_eq_1 = FALSE;

    /* indicate chanspec control flow to follow init path */
    mboolset(pi_ac->CCTrace, CALLED_ON_INIT);
#ifdef WL_AIR_IQ
    /* After a reinit, must reset phymode */
    if (phymode == PHYMODE_BGDFS) {
        phy_set_phymode(pi, PHYMODE_MIMO);
    }
#endif /* WL_AIR_IQ */

#if defined(AP) && defined(RADAR)
    /* indicate first time radar detection */
    if (pi->radari != NULL) {
        phy_radar_first_indicator_set(pi);
    }
#endif

    wlc_phy_chanspec_set_acphy(pi, pi->radio_chanspec);
    mboolclr(pi_ac->CCTrace, CALLED_ON_INIT);
    chanmgri->data->init_done = TRUE;

    if (!(ACMAJORREV_4(pi->pubpi->phy_rev))) {
        if (TINY_RADIO(pi) || (ACMAJORREV_128(pi->pubpi->phy_rev) &&
            (pi->u.pi_acphy->sromi->srom_low_adc_rate_en))) {
            phy_ac_rfseq_mode_set(pi, 1);
        }
        if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en &&
            ACMAJORREV_47(pi->pubpi->phy_rev) &&
            chanmgri->bypass_idle_tssi == FALSE) {
            /* Disable core2core sync (and enable later)
             * as AFE overrides below can mess with it
             */
            phy_ac_chanmgr_core2core_sync_setup(chanmgri, FALSE);
            /* In low rate TSSI mode, adc running low,
             * use overrideds to configure ADC to normal mode
            */
            wlc_phy_low_rate_adc_enable_acphy(pi, FALSE);
            wlc_phy_radio20698_afe_div_ratio(pi, 1, 0, 0);
        }
        if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en &&
            ACMAJORREV_129(pi->pubpi->phy_rev) &&
            chanmgri->bypass_idle_tssi == FALSE) {
            /* Disable core2core sync (and enable later)
             * as AFE overrides below can mess with it
             */
            phy_ac_chanmgr_core2core_sync_setup(chanmgri, FALSE);
            /* In low rate TSSI mode, adc running low,
             * use overrideds to configure ADC to normal mode
            */
            wlc_phy_low_rate_adc_enable_acphy(pi, FALSE);
            wlc_phy_radio20707_afe_div_ratio(pi, 1);
        }
        if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en &&
            ACMAJORREV_130(pi->pubpi->phy_rev) &&
            chanmgri->bypass_idle_tssi == FALSE) {
            /* Disable core2core sync (and enable later)
             * as AFE overrides below can mess with it
             */
            phy_ac_chanmgr_core2core_sync_setup(chanmgri, FALSE);
            /* In low rate TSSI mode, adc running low,
             * use overrideds to configure ADC to normal mode
            */
            //wlc_phy_low_rate_adc_enable_acphy(pi, FALSE);
            //wlc_phy_radio20708_tx2cal_normal_adc_rate(pi, 1, 1);
        }
        if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en &&
            ACMAJORREV_51(pi->pubpi->phy_rev) &&
            chanmgri->bypass_idle_tssi == FALSE) {
            /* Disable core2core sync (and enable later)
             * as AFE overrides below can mess with it
             */
            phy_ac_chanmgr_core2core_sync_setup(chanmgri, FALSE);
            /* In low rate TSSI mode, adc running low,
             * use overrideds to configure ADC to normal mode
            */
            wlc_phy_low_rate_adc_enable_acphy(pi, FALSE);
            wlc_phy_radio20704_afe_div_ratio(pi, 1);
        }
        if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en &&
            ACMAJORREV_128(pi->pubpi->phy_rev) &&
            chanmgri->bypass_idle_tssi == FALSE) {
            /* In low rate TSSI mode, adc running low,
             * use overrideds to configure ADC to normal mode
            */
            wlc_phy_low_rate_adc_enable_acphy(pi, FALSE);
            wlc_phy_radio20709_afe_div_ratio(pi, 1);
        }
        if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en &&
            ACMAJORREV_131(pi->pubpi->phy_rev) &&
            chanmgri->bypass_idle_tssi == FALSE) {
            /* Disable core2core sync (and enable later)
             * as AFE overrides below can mess with it
             */
            phy_ac_chanmgr_core2core_sync_setup(chanmgri, FALSE);
            /* In low rate TSSI mode, adc running low,
             * use overrideds to configure ADC to normal mode
            */
            wlc_phy_low_rate_adc_enable_acphy(pi, FALSE);
            wlc_phy_radio20710_afe_div_ratio(pi, 1, 0);
        }

        if ((ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            wlc_phy_txpwrctrl_idle_tssi_phyreg_setup_acphy(pi);
        }
        if (chanmgri->bypass_idle_tssi == TRUE &&
            (ACMAJORREV_47_51_129_130_131(pi->pubpi->phy_rev) ||
            ACMAJORREV_128(pi->pubpi->phy_rev))) {
            // bypass idle tssi cal for DFS channels
        } else {
            wlc_phy_txpwrctrl_idle_tssi_meas_acphy(pi);
        }

        if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en &&
            ACMAJORREV_129(pi->pubpi->phy_rev) &&
            chanmgri->bypass_idle_tssi == FALSE) {
            /* Remove afe_div overrides */
            wlc_phy_radio20707_afe_div_ratio(pi, 0);
            wlc_phy_low_rate_adc_enable_acphy(pi, TRUE);
            if (pi->u.pi_acphy->c2c_sync_en) {
                /* Enable core2core sync back */
                phy_ac_chanmgr_core2core_sync_setup(chanmgri, TRUE);
            }
        }
        if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en &&
            ACMAJORREV_130(pi->pubpi->phy_rev) &&
            chanmgri->bypass_idle_tssi == FALSE) {
            /* Remove afe_div overrides */
            //wlc_phy_radio20708_tx2cal_normal_adc_rate(pi, 0, 1);
            //wlc_phy_low_rate_adc_enable_acphy(pi, TRUE);
            if (pi->u.pi_acphy->c2c_sync_en) {
                /* Enable core2core sync back */
                phy_ac_chanmgr_core2core_sync_setup(chanmgri, TRUE);
            }
        }
        if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en &&
            ACMAJORREV_131(pi->pubpi->phy_rev) &&
            chanmgri->bypass_idle_tssi == FALSE) {
            /* Remove afe_div overrides */
            wlc_phy_radio20710_afe_div_ratio(pi, 0, 0);
            wlc_phy_low_rate_adc_enable_acphy(pi, TRUE);
            if (pi->u.pi_acphy->c2c_sync_en) {
                /* Enable core2core sync back */
                phy_ac_chanmgr_core2core_sync_setup(chanmgri, TRUE);
            }
        }
        if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en &&
            ACMAJORREV_47(pi->pubpi->phy_rev) &&
            chanmgri->bypass_idle_tssi == FALSE) {
            /* Remove afe_div overrides */
            wlc_phy_radio20698_afe_div_ratio(pi, 0, 0, 0);
            wlc_phy_low_rate_adc_enable_acphy(pi, TRUE);
            if (pi->u.pi_acphy->c2c_sync_en) {
                /* Enable core2core sync back */
                phy_ac_chanmgr_core2core_sync_setup(chanmgri, TRUE);
            }
        }
        if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en &&
            ACMAJORREV_51(pi->pubpi->phy_rev) &&
            chanmgri->bypass_idle_tssi == FALSE) {
            /* Remove afe_div overrides */
            wlc_phy_radio20704_afe_div_ratio(pi, 0);
            wlc_phy_low_rate_adc_enable_acphy(pi, TRUE);
            if (pi->u.pi_acphy->c2c_sync_en) {
                /* Enable core2core sync back */
                phy_ac_chanmgr_core2core_sync_setup(chanmgri, TRUE);
            }
        }
        if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en &&
            ACMAJORREV_128(pi->pubpi->phy_rev) &&
            chanmgri->bypass_idle_tssi == FALSE) {
            /* Remove afe_div overrides */
            wlc_phy_radio20709_afe_div_ratio(pi, 0);
            wlc_phy_low_rate_adc_enable_acphy(pi, TRUE);
        }
        if (TINY_RADIO(pi) || (ACMAJORREV_128(pi->pubpi->phy_rev) &&
            (pi->u.pi_acphy->sromi->srom_low_adc_rate_en))) {
            phy_ac_rfseq_mode_set(pi, 0);
        }
    }

    return BCME_OK;
}

/* band specific init */
static int
WLBANDINITFN(phy_ac_chanmgr_bsinit)(phy_type_chanmgr_ctx_t *ctx, chanspec_t chanspec, bool forced)
{
    phy_ac_chanmgr_info_t *chanmgri = (phy_ac_chanmgr_info_t *)ctx;
    if (forced) {
        return phy_init(chanmgri->pi, chanspec);
    } else {
        return BCME_UNSUPPORTED;
    }
}

/* band width init */
static int
WLBANDINITFN(phy_ac_chanmgr_bwinit)(phy_type_chanmgr_ctx_t *ctx, chanspec_t chanspec)
{
    return BCME_UNSUPPORTED;
}

/* Internal data api between ac modules */
phy_ac_chanmgr_data_t *
phy_ac_chanmgr_get_data(phy_ac_chanmgr_info_t *ci)
{
    return ci->data;
}

void
phy_ac_chanmgr_set_both_txchain_rxchain(phy_ac_chanmgr_info_t *ci, uint8 rxchain, uint8 txchain)
{
    ci->data->both_txchain_rxchain_eq_1 = ((rxchain == 1) && (txchain == 1)) ? TRUE : FALSE;
}

#ifdef WL_PROXDETECT
void
phy_ac_chanmgr_save_smoothing(phy_ac_chanmgr_info_t *ci, uint8 *enable, uint8 *dump_mode)
{
    *enable = ci->acphy_enable_smth;
    *dump_mode = ci->acphy_smth_dump_mode;
}
#endif /* WL_PROXDETECT */
/* ********************************************* */
/*                Internal Definitions                    */
/* ********************************************* */
#define TXMAC_IFHOLDOFF_DEFAULT        0x12    /* 9.0us */
#define TXMAC_MACDELAY_DEFAULT        0x2a8    /* 8.5us */
#define TXDELAY_BW80        3    /* 3.0us */

#define TXMAC_IFHOLDOFF_43684B0        0x11    /* 8.5us */
#define TXMAC_IFHOLDOFF_6715        0x11    /* 8.5us */
#define TXMAC_IFHOLDOFF_63178        0x11    /* 8.5us */

#define ACPHY_VCO_2P5V    1
#define ACPHY_VCO_1P35V    0

#define WLC_TINY_GI_MULT_P12        4096U
#define WLC_TINY_GI_MULT_TWEAK_P12    4096U
#define WLC_TINY_GI_MULT            WLC_TINY_GI_MULT_P12

typedef struct _chan_info_common {
    uint16 chan;        /* channel number */
    uint16 freq;        /* in Mhz */
} chan_info_common_t;

static const uint16 qt_rfseq_val1[] = {0x8b5, 0x8b5, 0x8b5, 0x8b5};
static const uint16 qt_rfseq_val2[] = {0x0, 0x0, 0x0, 0x0};
static const uint16 rfseq_reset2rx_dly[] = {12, 2, 2, 4, 4, 6, 1, 4, 1, 2, 1, 1, 1, 1, 1, 1};
static const uint16 rfseq_updl_lpf_hpc_ml[] = {0x0aaa, 0x0aaa};
static const uint16 rfseq_updl_tia_hpc_ml[] = {0x0222, 0x0222};
static const uint16 rfseq_reset2rx_cmd[] = {0x4, 0x3, 0x6, 0x5, 0x2, 0x1, 0x8,
            0x2a, 0x2b, 0xf, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};

uint16 const rfseq_rx2tx_cmd[] =
{0x0, 0x1, 0x2, 0x8, 0x5, 0x0, 0x6, 0x3, 0xf, 0x4, 0x0, 0x35, 0xf, 0x0, 0x36, 0x1f};
static uint16 rfseq_rx2tx_dly_epa1_20[] =
    {0x8, 0x6, 0x4, 0x4, 0x6, 0x2, 0x10, 60, 0x2, 0x5, 0x1, 0x4, 0xe4, 0xfa, 0x2, 0x1};
static uint16 rfseq_rx2tx_dly_epa1_40[] =
    {0x8, 0x6, 0x4, 0x4, 0x6, 0x2, 0x10, 30, 0x2, 0xd, 0x1, 0x4, 0xfa, 0xfa, 0x2, 0x1};
static uint16 rfseq_rx2tx_dly_epa1_80[] =
    {0x8, 0x6, 0x4, 0x4, 0x6, 0x2, 0x10, 20, 0x2, 0x17, 0x1, 0x4, 0xfa, 0xfa, 0x2, 0x1};

static uint16 rfseq_rx2tx_cmd_noafeclkswitch[] =
        {0x0, 0x1, 0x5, 0x8, 0x2, 0x3d, 0x6, 0x3, 0xf, 0x4, 0x3e, 0x35, 0xf, 0x0, 0x36, 0x1f};
static uint16 rfseq_rx2tx_cmd_noafeclkswitch_dly[] =
        {0x8, 0x6, 0x6, 0x4, 0x4, 0x2, 0x10, 0x26, 0x2, 0x5, 0x1, 0x4, 0xfa, 0xfa, 0x2, 0x1};
static uint16 rfseq_rx2tx_cmd_afeclkswitch[] =
        {0x0, 0x3d, 0x3e, 0x1, 0x5, 0x8, 0x2, 0x6, 0x3, 0xf, 0x4, 0x35, 0xf, 0x0, 0x36, 0x1f};
static uint16 rfseq_rx2tx_cmd_afeclkswitch_dly[] =
        {0x2, 0x8, 0x1, 0xd, 0x6, 0x4, 0x4, 0x10, 0x24, 0x2, 0x5, 0x4, 0xfa, 0xfa, 0x2, 0x1};

static uint16 rfseq_tx2rx_cmd_noafeclkswitch[] =
  {0x00, 0x04, 0x03, 0x06, 0xb3, 0x02, 0x3d, 0x05, 0xb3, 0x01, 0x08, 0x2a, 0x3e, 0x0f, 0x2b, 0x1f};
static uint16 rfseq_tx2rx_dly_noafeclkswitch[] =
  {0x02, 0x01, 0x07, 0x04, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x0a, 0x0a, 0x04, 0x02, 0x01};

static uint16 rfseq_rx2tx_cmd_withtssisleep[] =
{0x0000, 0x0001, 0x0005, 0x0008, 0x0002, 0x0006, 0x0003, 0x000f, 0x0004, 0x0035,
0x000f, 0x0000, 0x0000, 0x0036, 0x0080, 0x001f};
static uint16 rfseq_rx2tx_dly_withtssisleep[] =
{0x0008, 0x0006, 0x0006, 0x0004, 0x0006, 0x0010, 0x0026, 0x0002, 0x0006, 0x0004,
0x00ff, 0x00ff, 0x00a8, 0x0004, 0x0001, 0x0001};
static uint16 rfseq_rx2tx_cmd_rev15_ipa[] =
        {0x0, 0x1, 0x5, 0x8, 0x2, 0x6, 0x35, 0x3, 0xf, 0x4, 0x0f, 0x0, 0x0, 0x36, 0x00, 0x1f};
static uint16 rfseq_rx2tx_cmd_rev15_ipa_withtssisleep[] =
        {0x0, 0x1, 0x5, 0x8, 0x2, 0x6, 0x35, 0x3, 0xf, 0x4, 0x0f, 0x0, 0x0, 0x36, 0x80, 0x1f};
static uint16 rfseq_rx2tx_dly_rev15_ipa20[] =
    {0x8, 0x6, 0x6, 0x4, 0x6, 0x10, 40, 0x26, 0x2, 0x6, 0xff, 0xff, 0x56, 0x4, 0x1, 0x1};
static uint16 rfseq_rx2tx_dly_rev15_ipa40[] =
    {0x8, 0x6, 0x6, 0x4, 0x6, 0x10, 16, 0x26, 0x2, 0x6, 0xff, 0xff, 0x6e, 0x4, 0x1, 0x1};

static const uint16 rfseq_tx2rx_cmd[] =
{0x4, 0x3, 0x6, 0x5, 0x0, 0x2, 0x1, 0x8, 0x2a, 0xf, 0x0, 0xf, 0x2b, 0x1f, 0x1f, 0x1f};
static uint16 rfseq_tx2rx_dly[] =
  {0x08, 0x4, 0x2, 0x2, 0x1, 0x3, 0x4, 0x6, 0x04, 0xa, 0x4, 0x2, 0x01, 0x01, 0x01, 0x1};

static const uint16 rf_updh_cmd_clamp[] = {0x2a, 0x07, 0x0a, 0x00, 0x08, 0x2b, 0x1f, 0x1f};
static const uint16 rf_updh_dly_clamp[] = {0x01, 0x02, 0x02, 0x02, 0x10, 0x01, 0x01, 0x01};
static const uint16 rf_updl_cmd_clamp[] = {0x2a, 0x07, 0x08, 0x0c, 0x0e, 0x2b, 0x1f, 0x1f};
static const uint16 rf_updl_dly_clamp[] = {0x01, 0x06, 0x12, 0x08, 0x10, 0x01, 0x01, 0x01};
static const uint16 rf_updu_cmd_clamp[] = {0x2a, 0x07, 0x08, 0x0e, 0x2b, 0x1f, 0x1f, 0x1f};
static const uint16 rf_updu_dly_clamp[] = {0x01, 0x06, 0x1e, 0x1c, 0x01, 0x01, 0x01, 0x01};

static const uint16 rf_updh_cmd_adcrst[] = {0x07, 0x0a, 0x00, 0x08, 0xb0, 0xb1, 0x1f, 0x1f};
static uint16 rf_updh_dly_adcrst[] = {0x02, 0x02, 0x02, 0x01, 0x0a, 0x01, 0x01, 0x01};
static const uint16 rf_updl_cmd_adcrst[] = {0x07, 0x08, 0x0c, 0x0e, 0xb0, 0xb2, 0x1f, 0x1f};
static uint16 rf_updl_dly_adcrst[] = {0x06, 0x12, 0x08, 0x01, 0x0a, 0x01, 0x01, 0x01};
static const uint16 rf_updu_cmd_adcrst[] = {0x07, 0x08, 0x0e, 0xb0, 0xb1, 0x1f, 0x1f, 0x1f};
static const uint16 rf_updu_dly_adcrst[] = {0x06, 0x1e, 0x1c, 0x0a, 0x01, 0x01, 0x01, 0x01};
static const uint16 rf_updl_dly_dvga[] =  {0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01};

static const uint32 clip2_tbl[6] =
    {0xaa85433, 0xaa96544, 0xaa97655, 0xaaa8766, 0xaaa9877, 0xaaaa988};
static const uint32 clip2_tbl_maj47[6] =
    {0xaa98643, 0xaaa9844, 0xaaaa966, 0xaaaa996, 0xaaaa996, 0xaaaaa99};
static const uint32 clip2_tbl_maj47_rX[6] =
    {0xaaa6433, 0xaaa7544, 0xaaa9766, 0xaaaa876, 0xaaaa997, 0xaaaaa99};
static const uint32 clip2_tbl_maj51[6] =
    {0xaaa6433, 0xaaa7544, 0xaaa9766, 0xaaaa876, 0xaaaa997, 0xaaaa877};
static const uint32 clip2_tbl_maj129[] =
    {0xa987433, 0xa987644, 0xa987655, 0xaa98766, 0xaa98777, 0xaaa9888, 0xaaaaa99};
static const uint32 clip2_tbl_maj130[] =   /* 6715 from clip2tbl index 0 */
    {0x9654321, 0x9987433, 0x9987644, 0x9987655, 0x9998766, 0x9998777, 0x9999888,
     0x9999999, 0x9999999, 0x9999999, 0x9999999};
static const uint32 clip2_tbl_maj131[6] =
    {0xaaa6433, 0xaaa7544, 0xaaa9766, 0xaaaa876, 0xaaaa997, 0xaaaa877};
static const uint8 avvmid_set[25][5][6] = {{{2, 1, 2,   107, 150, 110},  /* pdet_id = 0 */
                   {2, 2, 1,   157, 153, 160},
                   {2, 2, 1,   157, 153, 161},
                   {2, 2, 0,   157, 153, 186},
                   {2, 2, 0,   157, 153, 187}},
                   {{1, 0, 1,   159, 174, 161},  /* pdet_id = 1 */
                   {1, 0, 1,   160, 185, 156},
                   {1, 0, 1,   163, 185, 162},
                   {1, 0, 1,   169, 187, 167},
                   {1, 0, 1,   152, 188, 160}},
                   {{1, 1, 1,   159, 166, 166},  /* pdet_id = 2 */
                   {2, 2, 4,   140, 151, 100},
                   {2, 2, 3,   143, 153, 116},
                   {2, 2, 2,   143, 153, 140},
                   {2, 2, 2,   145, 160, 154}},
                   {{1, 1, 2,   130, 131, 106},  /* pdet_id = 3 */
                   {1, 1, 2,   130, 131, 106},
                   {1, 1, 2,   128, 127, 97},
                   {0, 1, 3,   159, 137, 75},
                   {0, 0, 3,   164, 162, 76}},
                   {{1, 1, 1,   156, 160, 158},  /* pdet_id = 4 */
                   {1, 1, 1,   156, 160, 158},
                   {1, 1, 1,   156, 160, 158},
                   {1, 1, 1,   156, 160, 158},
                   {1, 1, 1,   156, 160, 158}},
                   {{2, 2, 2,   104, 108, 106},  /* pdet_id = 5 */
                   {2, 2, 2,   104, 108, 106},
                   {2, 2, 2,   104, 108, 106},
                   {2, 2, 2,   104, 108, 106},
                   {2, 2, 2,   104, 108, 106}},
                   {{2, 0, 2,   102, 170, 104},  /* pdet_id = 6 */
                   {3, 4, 3,    82, 102,  82},
                   {1, 3, 1,   134, 122, 136},
                   {1, 3, 1,   134, 124, 136},
                   {2, 3, 2,   104, 122, 108}},
                   {{0, 0, 0,   180, 180, 180},  /* pdet_id = 7 */
                   {0, 0, 0,   180, 180, 180},
                   {0, 0, 0,   180, 180, 180},
                   {0, 0, 0,   180, 180, 180},
                   {0, 0, 0,   180, 180, 180}},
                   {{2, 1, 2,   102, 138, 104},  /* pdet_id = 8 */
                   {3, 5, 3,    82, 100,  82},
                   {1, 4, 1,   134, 116, 136},
                   {1, 3, 1,   134, 136, 136},
                   {2, 3, 2,   104, 136, 108}},
                   {{3, 2, 3,    90, 106,  86},  /* pdet_id = 9 */
                   {3, 1, 3,    90, 158,  90},
                   {2, 1, 2,   114, 158, 112},
                   {2, 1, 1,   116, 158, 142},
                   {2, 1, 1,   116, 158, 142}},
                   {{2, 2, 2,   152, 156, 156},  /* pdet_id = 10 */
                   {2, 2, 2,   152, 156, 156},
                   {2, 2, 2,   152, 156, 156},
                   {2, 2, 2,   152, 156, 156},
                   {2, 2, 2,   152, 156, 156}},
                   {{1, 1, 1,   134, 134, 134},  /* pdet_id = 11 */
                   {1, 1, 1,   136, 136, 136},
                   {1, 1, 1,   136, 136, 136},
                   {1, 1, 1,   136, 136, 136},
                   {1, 1, 1,   136, 136, 136}},
                   {{3, 3, 3,    90,  92,  86},  /* pdet_id = 12 */
                   {3, 3, 3,    90,  86,  90},
                   {2, 3, 2,   114,  86, 112},
                   {2, 2, 1,   116, 109, 142},
                   {2, 2, 1,   116, 110, 142}},
                   {{2, 2, 2,   112, 114, 112},  /* pdet_id = 13 */
                   {2, 2, 2,   114, 114, 114},
                   {2, 2, 2,   114, 114, 114},
                   {2, 2, 2,   113, 114, 112},
                   {2, 2, 2,   113, 114, 112}},
                   {{1, 1, 1,   134, 134, 134},  /* pdet_id = 14 */
                   {0, 0, 0,   168, 168, 168},
                   {0, 0, 0,   168, 168, 168},
                   {0, 0, 0,   168, 168, 168},
                   {0, 0, 0,   168, 168, 168}},
                   {{0, 0, 0,   172, 172, 172},  /* pdet_id = 15 */
                   {0, 0, 0,   168, 168, 168},
                   {0, 0, 0,   168, 168, 168},
                   {0, 0, 0,   168, 168, 168},
                   {0, 0, 0,   168, 168, 168}},
                   {{3, 2, 3,    90, 106,  86},  /* pdet_id = 16 */
                   {3, 0, 3,    90, 186,  90},
                   {2, 0, 2,   114, 186, 112},
                   {2, 0, 1,   116, 186, 142},
                   {2, 0, 1,   116, 186, 142}},
                   {{4, 4, 4,   50,  45,  50},  /* pdet_id = 17 */
                   {3, 3, 3,    82,  82, 82},
                   {3, 3, 3,    82,  82, 82},
                   {3, 3, 3,    82,  82, 82},
                   {3, 3, 3,    82,  82, 82}},
                   {{5, 5, 5,   61,  61,  61},  /* pdet_id = 18 */
                   {2, 2, 2,   122, 122, 122},
                   {2, 2, 2,   122, 122, 122},
                   {2, 2, 2,   122, 122, 122},
                   {2, 2, 2,   122, 122, 122}},
                   {{2, 2, 2,  152, 156, 156},  /* pdet_id = 19 */
                   {1, 1, 1,   165, 165, 165},
                   {1, 1, 1,   160, 160, 160},
                   {1, 1, 1,   152, 150, 160},
                   {1, 1, 1,   152, 150, 160}},
                       {{3, 3, 3,  108, 108, 108},  /* pdet_id = 20 */
                   {1, 1, 1,   160, 160, 160},
                   {1, 1, 1,   160, 160, 160},
                   {1, 1, 1,   160, 160, 160},
                   {1, 1, 1,   160, 160, 160}},
                       {{2, 2, 2,  110, 110, 110},  /* pdet_id = 21 */
                   {0, 0, 0,   168, 168, 168},
                   {0, 0, 0,   168, 168, 168},
                   {0, 0, 0,   168, 168, 168},
                   {0, 0, 0,   168, 168, 168}},
                   {{6, 6, 6,   40,  40,  40},  /* pdet_id = 22 */
                   {2, 2, 1,   115, 115, 142},
                   {1, 2, 1,   142, 115, 142},
                   {1, 1, 1,   142, 142, 142},
                   {1, 1, 1,   142, 142, 142}},
                   {{1, 1, 1,  156, 160, 158},  /* pdet_id = 23 */
                   {6, 6, 6,    47,  45,  48},
                   {1, 1, 1,   147, 146, 148},
                   {1, 1, 1,   146, 146, 152},
                   {1, 1, 1,   146, 146, 152}},
                   {{2, 2, 2,   120, 120, 120}, /* pdet_id =24 */
                   {2, 2, 2,   120, 120, 120},
                   {2, 2, 2,   120, 120, 120},
                   {2, 2, 2,   120, 120, 120},
                   {2, 2, 2,   120, 120, 120}}
};

static const uint8 avvmid_set1[16][5][2] = {
    {{1, 154}, {0, 168}, {0, 168}, {0, 168}, {0, 168}},  /* pdet_id = 0 */
    {{1, 145}, {1, 145}, {1, 145}, {1, 145}, {1, 145}},  /* pdet_id = 1 WLBGA */
    {{6,  76}, {1, 160}, {6,  76}, {6,  76}, {6,  76}},  /* pdet_id = 2 */
    {{1, 156}, {1, 152}, {1, 152}, {1, 152}, {1, 152}},  /* pdet_id = 3 */
    {{1, 152}, {1, 152}, {1, 152}, {1, 152}, {1, 152}},  /* pdet_id = 4 WLCSP */
    {{3, 100}, {3,  75}, {3,  75}, {3,  75}, {3,  75}},  /* pdet_id = 5 WLCSP TM */
    {{1, 152}, {0, 166}, {0, 166}, {0, 166}, {0, 166}},  /* pdet_id = 6 WLCSP HK */
    {{1, 145}, {3, 120}, {3, 120}, {3, 120}, {3, 125}},  /* pdet_id = 7 WLiPA */
    {{1, 145}, {1, 155}, {1, 155}, {1, 155}, {1, 155}},  /* pdet_id = 8 WLBGA C0 */
    {{1, 135}, {1, 165}, {1, 165}, {1, 165}, {1, 165}}   /* pdet_id = 9 WLBGA RR FEM */
};
static const uint8 avvmid_set2[16][5][4] = {
    {
        {1, 1, 145, 145},
        {1, 1, 145, 145},
        {1, 1, 145, 145},
        {1, 1, 145, 145},
        {1, 1, 145, 145}},  /* pdet_id = 0 */
    {
        {3, 3, 100, 100},
        {1, 1, 145, 145},
        {1, 1, 145, 145},
        {1, 1, 145, 145},
        {1, 1, 145, 145}},  /* pdet_id = 1 */
    {
        {4, 4,  95,  95},
        {1, 1, 145, 145},
        {1, 1, 145, 145},
        {1, 1, 145, 145},
        {1, 1, 145, 145}},  /* pdet_id = 2 */
    {
        {1, 1, 145, 145},
        {3, 3,  90,  90},
        {3, 3,  92,  92},
        {2, 3, 110,  90},
        {2, 3, 110,  93}},  /* pdet_id = 3 */
    {
        {2, 2, 140, 140},
        {2, 2, 145, 145},
        {2, 2, 145, 145},
        {2, 2, 145, 145},
        {2, 2, 145, 145}},  /* pdet_id = 4 */
    {
        {1, 1, 145, 145},
        {1, 1, 145, 145},
        {1, 1, 145, 145},
        {1, 1, 145, 145},
        {2, 2, 110, 110}},  /* pdet_id = 5 */
    {
        {3, 3, 98, 98},
        {2, 1, 122, 150},
        {2, 1, 122, 150},
        {2, 2, 122, 122},
        {2, 2, 122, 122}}  /* pdet_id = 6 */
};

static const uint8 avvmid_set3[16][5][2] = {
    {{1, 115}, {2, 90}, {2, 90}, {2, 90}, {2, 90}},  /* pdet_id = 0 4345 TC */
    {{0, 131}, {0, 134}, {0, 134}, {0, 134}, {0, 134}},  /* pdet_id = 1 4345TC FCBGA EPA */
    {{4, 132}, {4, 127}, {4, 127}, {4, 127}, {4, 127}},  /* pdet_id = 2 4345A0 fcbusol */
    {{0, 150}, {2, 97}, {2, 97}, {2, 97}, {2, 97}},  /* pdet_id = 3 4345A0 fcpagb ipa */
};
static const uint8 avvmid_set4[1][5][4] = {
    {
        {2, 2, 130, 130},
        {2, 2, 130, 130},
        {2, 2, 130, 130},
        {2, 2, 130, 130},
        {2, 2, 130, 130}},  /* pdet_id = 0 */
};

static const uint8 avvmid_set32[6][5][8] = {
    {
        {1, 1, 1, 1, 148, 148, 148, 148},
        {1, 1, 1, 1, 148, 148, 148, 148},
        {1, 1, 1, 1, 148, 148, 148, 148},
        {1, 1, 1, 1, 148, 148, 148, 148},
        {1, 1, 1, 1, 148, 148, 148, 148}},  /* pdet_id = 0 */
    {
        {1, 1, 1, 1, 148, 148, 148, 148},
        {1, 1, 1, 1, 148, 148, 148, 148},
        {1, 1, 1, 1, 148, 148, 148, 148},
        {1, 1, 1, 1, 148, 148, 148, 148},
        {1, 1, 1, 1, 148, 148, 148, 148}},  /* pdet_id = 1 */
    {
        {2, 2, 2, 2, 110, 110, 110, 110},
        {2, 2, 2, 2, 116, 116, 116, 116},
        {2, 2, 2, 2, 116, 116, 116, 116},
        {2, 2, 2, 2, 116, 116, 116, 116},
        {2, 2, 2, 2, 116, 116, 116, 116}},  /* pdet_id = 2 */
    {
        {1, 1, 1, 1, 154, 154, 154, 156},
        {2, 2, 2, 2, 136, 136, 136, 136},
        {2, 2, 2, 2, 136, 136, 136, 136},
        {2, 2, 2, 2, 136, 136, 136, 136},
        {2, 2, 2, 2, 136, 136, 136, 136}},  /* pdet_id = 3 */
    {
        {2, 2, 2, 2, 128, 128, 128, 128},
        {2, 2, 2, 2, 128, 128, 128, 128},
        {2, 2, 2, 2, 128, 128, 128, 128},
        {2, 2, 2, 2, 128, 128, 128, 128},
        {2, 2, 2, 2, 128, 128, 128, 128}},  /* pdet_id = 4 */
    {
        {2, 2, 2, 2, 110, 110, 110, 110},
        {2, 2, 2, 2, 116, 116, 116, 116},
        {2, 2, 2, 2, 116, 116, 116, 116},
        {2, 2, 2, 2, 116, 116, 116, 116},
        {2, 2, 2, 2, 116, 116, 116, 116}},  /* pdet_id = 5 */
};

static const uint8 avvmid_set47[10][5][8] = {
    {
    /* http://confluence.broadcom.com/pages/viewpage.action?pageId=431104865 */
        {4, 4, 4, 4, 120, 115, 115, 115},
        {4, 4, 4, 4, 75, 70, 76, 70},
        {4, 4, 4, 4, 75, 70, 76, 70},
        {4, 4, 4, 4, 75, 70, 76, 70},
        {4, 4, 4, 4, 75, 70, 76, 70}},  /* pdet_id = 0 */
    {
        {4, 4, 4, 4, 70, 70, 70, 70},
        {5, 5, 5, 5, 50, 50, 50, 50},
        {5, 5, 5, 5, 50, 50, 50, 50},
        {5, 5, 5, 5, 50, 50, 50, 50},
        {5, 5, 5, 5, 50, 50, 50, 50}},  /* pdet_id = 1 */
    {
        {4, 4, 4, 4, 82, 80, 79, 80},
        {2, 2, 2, 2, 155, 155, 155, 155},
        {2, 2, 2, 2, 155, 155, 155, 155},
        {2, 2, 2, 2, 155, 155, 155, 155},
        {2, 2, 2, 2, 155, 155, 155, 155}},  /* pdet_id = 2 */
    {
        {4, 4, 4, 4, 105, 105, 105, 105},
        {4, 4, 4, 4, 72, 70, 74, 74},
        {4, 4, 4, 4, 72, 70, 74, 74},
        {4, 4, 4, 4, 74, 72, 76, 76},
        {4, 4, 4, 4, 74, 72, 76, 76}},  /* pdet_id = 3 */
    {
        {4, 4, 4, 4, 85, 85, 85, 85},
        {2, 2, 2, 2, 128, 128, 128, 128},
        {2, 2, 2, 2, 128, 128, 128, 128},
        {2, 2, 2, 2, 128, 128, 128, 128},
        {2, 2, 2, 2, 128, 128, 128, 128}},  /* pdet_id = 4 */
    {
        {3, 3, 3, 3, 95, 95, 95, 95},
        {2, 2, 2, 2, 140, 140, 140, 140},
        {2, 2, 2, 2, 140, 140, 140, 140},
        {2, 2, 2, 2, 140, 140, 140, 140},
        {2, 2, 2, 2, 140, 140, 140, 140}},  /* pdet_id = 5 */
    {
        {4, 4, 4, 4, 85, 85, 85, 85},
        {2, 2, 2, 2, 140, 140, 140, 140},
        {2, 2, 2, 2, 140, 140, 140, 140},
        {2, 2, 2, 2, 140, 140, 140, 140},
        {2, 2, 2, 2, 140, 140, 140, 140}},  /* pdet_id = 6 */
    {
        {4, 4, 4, 4, 82, 82, 82, 82},
        {3, 3, 3, 3, 110, 110, 110, 110},
        {3, 3, 3, 3, 110, 110, 110, 110},
        {3, 3, 3, 3, 110, 110, 110, 110},
        {3, 3, 3, 3, 110, 110, 110, 110}},  /* pdet_id = 7 */
    {
        {6, 6, 6, 6, 65, 60, 60, 60},
        {5, 5, 5, 5, 50, 50, 50, 50},
        {5, 5, 5, 5, 50, 50, 50, 50},
        {5, 5, 5, 5, 50, 50, 50, 50},
        {5, 5, 5, 5, 50, 50, 50, 50}},  /* pdet_id = 8 */
    {
        {4, 4, 4, 4, 70, 70, 70, 70},
        {2, 2, 2, 2, 125, 125, 125, 125},
        {2, 2, 2, 2, 125, 125, 125, 125},
        {2, 2, 2, 2, 125, 125, 125, 125},
        {2, 2, 2, 2, 125, 125, 125, 125},},  /* pdet_id = 9 */
};

static const uint8 avvmid_set47_1[17][5][8] = {
    {
    /* http://confluence.broadcom.com/pages/viewpage.action?pageId=431104865 */
        {5, 5, 5, 5, 95, 95, 90, 95},
        {4, 4, 4, 4, 75, 80, 75, 82},
        {4, 4, 4, 4, 80, 80, 76, 82},
        {4, 4, 4, 4, 80, 80, 75, 83},
        {4, 4, 4, 4, 80, 80, 75, 84}},  /* pdet_id = 0 */
    {
        {4, 4, 4, 4, 70, 70, 70, 70},
        {5, 5, 5, 5, 62, 63, 63, 64},
        {5, 5, 5, 5, 62, 63, 62, 65},
        {5, 5, 5, 5, 60, 61, 61, 63},
        {5, 5, 5, 5, 61, 61, 61, 62}},  /* pdet_id = 1 */
    {
        {4, 4, 4, 4, 82, 80, 79, 80},
        {2, 2, 2, 2, 155, 155, 155, 155},
        {2, 2, 2, 2, 155, 155, 155, 155},
        {2, 2, 2, 2, 155, 155, 155, 155},
        {2, 2, 2, 2, 155, 155, 155, 155}},  /* pdet_id = 2 */
    {
        {4, 4, 4, 4, 105, 105, 105, 105},
        {4, 4, 4, 4, 72, 70, 74, 74},
        {4, 4, 4, 4, 72, 70, 74, 74},
        {4, 4, 4, 4, 74, 72, 76, 76},
        {4, 4, 4, 4, 74, 72, 76, 76}},  /* pdet_id = 3 */
    {
        {4, 4, 4, 4, 85, 85, 85, 85},
        {2, 2, 2, 2, 128, 128, 128, 128},
        {2, 2, 2, 2, 128, 128, 128, 128},
        {2, 2, 2, 2, 128, 128, 128, 128},
        {2, 2, 2, 2, 128, 128, 128, 128}},  /* pdet_id = 4 */
    {
        {3, 3, 3, 3, 95, 95, 95, 95},
        {2, 2, 2, 2, 140, 140, 140, 140},
        {2, 2, 2, 2, 140, 140, 140, 140},
        {2, 2, 2, 2, 140, 140, 140, 140},
        {2, 2, 2, 2, 140, 140, 140, 140}},  /* pdet_id = 5 */
    {
        {4, 4, 4, 4, 120, 120, 120, 120},
        {3, 3, 3, 3, 98, 98, 100, 102},
        {3, 3, 3, 3, 92, 92, 94, 94},
        {3, 3, 3, 3, 94, 94, 96, 96},
        {3, 3, 3, 3, 98, 98, 100, 102}},  /* pdet_id = 6 */
    {
        {4, 4, 4, 4, 82, 82, 82, 82},
        {3, 3, 3, 3, 110, 110, 110, 110},
        {3, 3, 3, 3, 110, 110, 110, 110},
        {3, 3, 3, 3, 110, 110, 110, 110},
        {3, 3, 3, 3, 110, 110, 110, 110}},  /* pdet_id = 7 */
    {
        {4, 4, 3, 4, 115, 120, 130, 120},
        {4, 4, 4, 4, 98, 96, 97, 98},
        {4, 4, 4, 4, 98, 95, 96, 97},
        {4, 4, 4, 4, 96, 95, 97, 97},
        {4, 4, 4, 4, 96, 95, 96, 97}},  /* pdet_id = 8 */
    {
        {4, 4, 4, 4, 70, 70, 70, 70},
        {2, 2, 2, 2, 125, 125, 125, 125},
        {2, 2, 2, 2, 125, 125, 125, 125},
        {2, 2, 2, 2, 125, 125, 125, 125},
        {2, 2, 2, 2, 125, 125, 125, 125}},  /* pdet_id = 9 */
    {
        {4, 4, 4, 4, 70, 70, 70, 70},
        {3, 3, 3, 3, 105, 105, 105, 105},
        {3, 3, 3, 3, 105, 105, 105, 105},
        {3, 3, 3, 3, 105, 105, 105, 105},
        {3, 3, 3, 3, 105, 105, 105, 105}},  /* pdet_id = 10 */
    {
        {3, 3, 3, 3, 95, 95, 95, 95},
        {3, 3, 3, 3, 115, 115, 115, 115},
        {3, 3, 3, 3, 115, 115, 115, 115},
        {3, 3, 3, 3, 115, 115, 115, 115},
        {3, 3, 3, 3, 115, 115, 115, 115}},  /* pdet_id = 11 */
    {
        {3, 3, 3, 3, 95, 95, 95, 95},
        {3, 4, 4, 4, 90, 80, 80, 90},
        {4, 4, 4, 4, 80, 80, 80, 90},
        {4, 4, 4, 4, 80, 80, 80, 90},
        {4, 4, 4, 4, 80, 80, 80, 90}},  /* pdet_id = 12 */
    {
        {4, 4, 4, 4, 105, 105, 105, 105},
        {4, 4, 4, 4, 95, 96, 96, 96},
        {4, 4, 4, 4, 98, 98, 100, 99},
        {4, 4, 4, 4, 99, 99, 101, 100},
        {4, 4, 4, 4, 102, 99, 103, 103}},  /* pdet_id = 13 */
    {
        {2, 2, 2, 2, 130, 130, 130, 130},
        {3, 3, 2, 2, 120, 120, 140, 140},
        {3, 3, 2, 2, 120, 120, 140, 140},
        {3, 3, 2, 2, 120, 120, 140, 140},
        {3, 3, 2, 2, 120, 120, 140, 140}},  /* pdet_id = 14 */
    {
        {2, 2, 2, 2, 130, 130, 130, 130},
        {5, 5, 5, 5, 45, 45, 40, 45},
        {5, 5, 5, 5, 45, 45, 40, 45},
        {5, 5, 5, 5, 45, 45, 40, 45},
        {5, 5, 5, 5, 45, 45, 40, 45}},  /* pdet_id = 15 */
    {
        {4, 4, 3, 4, 115, 120, 130, 120},
        {3, 3, 3, 3, 91, 91, 91, 93},
        {3, 3, 3, 3, 89, 89, 87, 89},
        {3, 3, 3, 3, 85, 85, 84, 86},
        {3, 3, 3, 3, 84, 82, 81, 85}},  /* pdet_id = 16 */
};

static const uint8 avvmid_set47_2[1][5][8] = {
    {
    /* http://confluence.broadcom.com/pages/viewpage.action?pageId=431104865 */
        {4, 4, 4, 4, 115, 105, 112, 107},
        {4, 4, 4, 4, 75, 80, 75, 82},
        {4, 4, 4, 4, 80, 80, 76, 80},
        {4, 4, 4, 4, 80, 80, 80, 78},
        {4, 4, 4, 4, 80, 80, 75, 84}}  /* pdet_id = 0 */
};

static const uint8 avvmid_set129[9][5][8] = {
    {
    /* http://confluence.broadcom.com/pages/viewpage.action?pageId=431104865 */
        {2, 2, 2, 0, 134, 130, 134, 0},
        {2, 2, 2, 0, 130, 128, 132, 0},
        {2, 2, 2, 0, 130, 128, 132, 0},
        {2, 2, 2, 0, 132, 130, 134, 0},
        {2, 2, 2, 0, 132, 130, 134, 0}},  /* pdet_id = 0 MC */
    {
        {2, 2, 2, 0, 125, 120, 125, 0},
        {2, 2, 2, 0, 125, 122, 125, 0},
        {2, 2, 2, 0, 128, 125, 128, 0},      /* pdet_id = 1 MCM */
        {2, 2, 2, 0, 132, 128, 132, 0},      /* SKY85347-11 (2.4G, low cost) + */
        {2, 2, 2, 0, 140, 138, 140, 0}},  /* SKY85772-11 (5G, low cost) */
    {
        {4, 4, 4, 0, 90, 85, 82, 0},
        {4, 4, 4, 0, 83, 84, 86, 0},
        {4, 4, 4, 0, 85, 87, 87, 0},
        {4, 4, 4, 0, 86, 88, 88, 0},
        {4, 4, 4, 0, 88, 90, 90, 0}},  /* pdet_id = 2 MCH2/5 */
    {
        {4, 4, 4, 0, 115, 115, 115, 0},
        {2, 2, 2, 0, 135, 135, 135, 0},
        {2, 2, 2, 0, 135, 135, 135, 0},
        {2, 2, 2, 0, 135, 135, 135, 0},
        {2, 2, 2, 0, 135, 135, 135, 0}},  /* pdet_id = 3 MCM2/5, MCM_QS */
    {
        {3, 3, 3, 0, 100, 100, 100, 0},
        {2, 2, 2, 0, 135, 135, 135, 0},
        {2, 2, 2, 0, 135, 135, 135, 0},
        {2, 2, 2, 0, 135, 135, 135, 0},      /* pdet_id = 4 MCM2 */
        {2, 2, 2, 0, 135, 135, 135, 0}},  /* with SKY85337-11 */
    {
        {2, 2, 2, 0, 122, 122, 122, 0},
        {2, 2, 2, 0, 115, 115, 115, 0},
        {2, 2, 2, 0, 115, 115, 115, 0},
        {2, 2, 2, 0, 118, 118, 118, 0},
        {2, 2, 2, 0, 120, 120, 120, 0}},      /* pdet_id = 5 MCM_SQ */
    {
        {2, 2, 2, 0, 90, 90, 90, 0},
        {2, 2, 2, 0, 90, 90, 90, 0},
        {2, 2, 2, 0, 90, 90, 90, 0},
        {2, 2, 2, 0, 90, 90, 90, 0},
        {2, 2, 2, 0, 90, 90, 90, 0}},  /* pdet_id = 6, MCM6_S */
    {
        {2, 2, 2, 0, 130, 130, 135, 0},
        {2, 2, 2, 0, 130, 130, 135, 0},
        {2, 2, 2, 0, 130, 130, 135, 0},
        {2, 2, 2, 0, 130, 130, 135, 0},
        {2, 2, 2, 0, 130, 130, 135, 0}},  /* pdet_id = 7, MCM6_Q */

    {
        {2, 2, 2, 0, 125, 125, 125, 0},
        {2, 2, 2, 0, 125, 125, 125, 0},
        {2, 2, 2, 0, 125, 125, 125, 0},
        {2, 2, 2, 0, 125, 125, 125, 0},
        {2, 2, 2, 0, 125, 125, 125, 0}}, /* pdet_id = 8, 6710MCH6 */

};

static const uint8 avvmid_set130[12][5][8] = {
    {
        {4, 4, 4, 4, 138, 140, 142, 142},
        {4, 4, 4, 4, 124, 120, 120, 124},
        {4, 4, 4, 4, 122, 125, 125, 125},
        {4, 4, 4, 4, 122, 120, 120, 124},
        {4, 4, 4, 4, 124, 122, 122, 125}},  /* pdet_id = 0 6715B0 MCM P210 */
    {
        {3, 3, 3, 3, 142, 140, 142,  140},
        {5, 5, 5, 5,  70,  70,  70,  70},
        {5, 5, 5, 5,  75,  70,  65,  70},
        {5, 5, 5, 5,  70,  70,  70,  70},
        {5, 5, 5, 5,  70,  70,  70,  70}},  /* pdet_id = 1 MCM P120 */
    {
        {4, 4, 4, 4, 105, 105, 105,  95},
        {4, 3, 4, 4, 114, 119, 105, 110},
        {4, 3, 4, 4, 116, 123, 106, 113},
        {4, 3, 4, 4, 116, 124, 108, 116},
        {4, 3, 4, 4, 120, 119, 109, 116}},  /* pdet_id = 2 MCH2/MCH5 */
    {
        {2, 2, 2, 2, 165, 160, 145, 145},
        {4, 4, 4, 4, 120, 120, 120, 120},
        {4, 4, 4, 4, 120, 120, 120, 120},
        {4, 4, 4, 4, 125, 125, 125, 125},
        {4, 4, 4, 4, 125, 125, 125, 125}},  /* pdet_id = 3 MCL2/MCL5 */
    {
        {2, 2, 2, 2, 170, 170, 170, 170},
        {4, 4, 4, 4, 125, 125, 125, 125},
        {4, 4, 4, 4, 125, 125, 125, 125},
        {4, 4, 4, 4, 125, 125, 125, 125},
        {4, 4, 4, 4, 120, 120, 120, 120}},  /* pdet_id = 4 MCH6 */
    {
        {4, 4, 3, 4, 120, 120, 125, 120},
        {4, 4, 4, 4, 102,  94,  91,  94},
        {4, 4, 4, 4, 100,  94,  91,  92},
        {4, 4, 4, 4,  98,  92,  87,  91},   /* pdet_id = 5, 2G = MCH2_SD (SKY ES5) */
        {4, 4, 4, 4,  94,  88,  84,  90}},  /*              5G = MCH6 SKY85780-11 ES4 */
    {
        {3, 3, 3, 3, 145, 145, 145, 145},
        {0, 0, 0, 0, 205, 200, 200, 200},
        {0, 0, 0, 0, 195, 195, 195, 195},
        {0, 0, 0, 0, 185, 185, 185, 185},   /* pdet_id = 6, 2G = MCH2_SD (SKY ES6) */
        {0, 0, 0, 0, 170, 170, 170, 170}},  /*              5G = MCH6D SKY85789-11 */
    {
        {4, 4, 4, 4, 150, 150, 145, 145},
        {3, 3, 3, 3, 145, 145, 145, 145},
        {3, 3, 3, 3, 145, 145, 145, 145},
        {3, 3, 3, 3, 145, 145, 145, 145},   /* pdet_id = 7, 2G = MCH2 Qorvo QPF4216B */
        {3, 3, 3, 3, 145, 145, 145, 145}},  /*              5G = 43794-MCL6T */
    {
        {4, 4, 4, 4,  85,  85,  95,  85},
        {2, 2, 2, 2, 169, 174, 172, 181},
        {2, 2, 1, 2, 170, 175, 191, 179},
        {2, 2, 1, 1, 173, 178, 197, 194},   /* pdet_id = 8, 2G = MCM2 SKY FEM */
        {1, 2, 1, 2, 186, 178, 193, 176}},  /*              5G = MCH5D SKY non-linear */
    {
        {2, 2, 2, 2, 165, 160, 145, 145},
        {4, 4, 3, 3, 120, 120, 145, 145},
        {4, 4, 3, 3, 120, 120, 145, 145},
        {4, 4, 3, 3, 125, 125, 145, 145},   /* pdet_id = 9, 43794MCLx */
        {4, 4, 3, 3, 125, 125, 145, 145}},  /*              2G/5G ant0,1, 6G ant2,3 */
    {
        {2, 2, 2, 2, 165, 160, 145, 145},
        {2, 2, 2, 2, 159, 167, 166, 161},
        {2, 2, 2, 3, 166, 162, 161, 145},
        {3, 2, 2, 2, 145, 167, 164, 163},   /* pdet_id = 10, 2G = MCM2SD */
        {3, 2, 2, 3, 146, 162, 164, 142}},  /*               6G = MCM6 Qorvo QPF4656 ES2 */
    {
        {2, 2, 2, 2, 165, 160, 145, 145},
        {4, 4, 4, 4,  94,  84,  89,  89},
        {4, 4, 4, 4,  95,  85,  89,  90},
        {4, 4, 4, 4,  96,  86,  90,  92},   /* pdet_id = 11, 2G = not assigned yet */
        {4, 4, 4, 4,  95,  87,  88,  91}},  /*               5G = MCM5D SKY non-linear */

    /* Note: if adding a new pdet_id, we need to update
     * lowrate_tssi_delay_set130 in src/ac/tpc/phy_ac_tpc.c as well
     */
};

#ifndef REGULATORY_DEBUG

static const uint32 rfseqext_rev47_2g5g[2][11][2] = {
    {
        {0x0074, 0x000698C3}, {0x0034, 0x00068C63}, {0x0014, 0x00068843},
        {0x0014, 0x00068843}, {0x0014, 0x00068843}, {0x0034, 0x00068C63},
        {0x0074, 0x000498C3}, {0x0034, 0x00048C63}, {0x0014, 0x00048843},
        {0x000C, 0x00048E47}, {0x000C, 0x00068E47}}, /* normal ADC rate mode */
    {
        {0x0070, 0x020698F3}, {0x0030, 0x02068CF3}, {0x0010, 0x020688F3},
        {0x0010, 0x020688F3}, {0x0010, 0x020688F3}, {0x0030, 0x02068CF3},
        {0x0074, 0x000498C3}, {0x0034, 0x00048C63}, {0x0014, 0x00048843},
        {0x000C, 0x00048E47}, {0x0008, 0x02068EF7}}  /* low ADC rate mode */
};

static const uint32 rfseqext_rev47_6gl[2][11][2] = {
    {
        {0x006A, 0x0006BDB3}, {0x002A, 0x00069EDB}, {0x000A, 0x00069293},
        {0x000A, 0x00069293}, {0x000A, 0x00069293}, {0x002A, 0x00069EDB},
        {0x006A, 0x0004BDB3}, {0x002A, 0x00049EDB}, {0x000A, 0x00049293},
        {0x000A, 0x00049097}, {0x000A, 0x00069097}}, /* normal ADC rate mode */
    {
        {0x0068, 0x0206BCF3}, {0x0028, 0x02069EF3}, {0x0008, 0x020692F3},
        {0x0008, 0x020692F3}, {0x0008, 0x020692F3}, {0x0028, 0x02069EF3},
        {0x006A, 0x0004BDB3}, {0x002A, 0x00049EDB}, {0x000A, 0x00049293},
        {0x000A, 0x00049097}, {0x0008, 0x020690F7}}  /* low ADC rate mode */
};

static const uint32 rfseqext_rev47_6gl_special[2][11][2] = {
    {
        {0x006A, 0x0006BDB3}, {0x002A, 0x00069EDB}, {0x000A, 0x00069293},
        {0x000A, 0x00069293}, {0x000A, 0x00069293}, {0x002A, 0x00069EDB},
        {0x006A, 0x0004BDB3}, {0x002A, 0x00049EDB}, {0x000A, 0x00049293},
        {0x000A, 0x0004908F}, {0x000A, 0x0006908F}}, /* normal ADC rate mode */
    {
        {0x0068, 0x0206BCF3}, {0x0028, 0x02069EF3}, {0x0008, 0x020692F3},
        {0x0008, 0x020692F3}, {0x0008, 0x020692F3}, {0x0028, 0x02069EF3},
        {0x006A, 0x0004BDB3}, {0x002A, 0x00049EDB}, {0x000A, 0x00049293},
        {0x000A, 0x0004908F}, {0x0008, 0x020690F7}}  /* low ADC rate mode */
};

static const uint32 rfseqext_rev47_6gu[2][11][2] = {
    {
        {0x006A, 0x0006BDE3}, {0x002A, 0x00069EF3}, {0x000A, 0x000692A3},
        {0x000A, 0x000692A3}, {0x000A, 0x000692A3}, {0x002A, 0x00069EF3},
        {0x006A, 0x0004BDE3}, {0x002A, 0x00049EF3}, {0x000A, 0x000492A3},
        {0x000A, 0x000492A7}, {0x000A, 0x000692A7}}, /* normal ADC rate mode */
    {
        {0x0068, 0x0206BCF3}, {0x0028, 0x02069EF3}, {0x0008, 0x020692F3},
        {0x0008, 0x020692F3}, {0x0008, 0x020692F3}, {0x0028, 0x02069EF3},
        {0x006A, 0x0004BDE3}, {0x002A, 0x00049EF3}, {0x000A, 0x000492A3},
        {0x000A, 0x000492A7}, {0x0008, 0x020692F7}}  /* low ADC rate mode */
};

static const uint32 rfseqext_rev130_2g5g[2][11][2] = {
    {
        {0x0006, 0x006AB183}, {0x0006, 0x00EA98C3}, {0x0006, 0x00E29083},
        {0x0000, 0x00028003}, {0x0000, 0x00028003}, {0x0000, 0x00028403},
        {0x0002, 0x0008B183}, {0x0002, 0x000898C3}, {0x0002, 0x00009083},
        {0x0002, 0x00008A87}, {0x0006, 0x01E28A87}}, /* normal ADC rate mode */
    {
        {0x0086, 0x006AB183}, {0x0086, 0x00EA98C3}, {0x0086, 0x00E29083},
        {0x0080, 0x00028003}, {0x0080, 0x00028003}, {0x0080, 0x00028403},
        {0x0002, 0x0008B183}, {0x0002, 0x000898C3}, {0x0002, 0x00009083},
        {0x0002, 0x00008A87}, {0x0086, 0x01E28A87}}  /* low ADC rate mode */
};

static const uint32 rfseqext_rev51_2g5g[2][11][2] = {
    /* https://confluence.broadcom.net/display/WLAN/RFSeqExt+table#RFSeqExttable-Version2 */
    {
        {0x0074, 0x000698C3}, {0x0034, 0x00068C63}, {0x0014, 0x00068843},
        {0x0014, 0x00068843}, {0x0014, 0x00068843}, {0x0034, 0x00068C63},
        {0x0074, 0x000498C3}, {0x0034, 0x00048C63}, {0x0014, 0x00048843},
        {0x000C, 0x00048E47}, {0x000C, 0x00068E47}}, /* normal ADC rate mode */
    {
        {0x0070, 0x020698F3}, {0x0030, 0x02068CF3}, {0x0010, 0x020688F3},
        {0x0010, 0x020688F3}, {0x0010, 0x020688F3}, {0x0030, 0x02068CF3},
        {0x0074, 0x000498C3}, {0x0034, 0x00048C63}, {0x0014, 0x00048843},
        {0x000C, 0x00048E47}, {0x0008, 0x02068EF7}}, /* low ADC rate mode */
};

static const uint32 rfseqext_rev51_2g5g_special[2][11][2] = {
    /* https://confluence.broadcom.net/display/WLAN/RFSeqExt+table#RFSeqExttable-Version2 */
    {
        {0x0074, 0x000698C3}, {0x0034, 0x00068C63}, {0x0014, 0x00068843},
        {0x0014, 0x00068843}, {0x0014, 0x00068843}, {0x0034, 0x00068C63},
        {0x0074, 0x000498C3}, {0x0034, 0x00048C63}, {0x0014, 0x00048843},
        {0x000A, 0x00048E8F}, {0x000A, 0x00068E8F}}, /* normal ADC rate mode */
    {
        {0x0070, 0x020698F3}, {0x0030, 0x02068CF3}, {0x0010, 0x020688F3},
        {0x0010, 0x020688F3}, {0x0010, 0x020688F3}, {0x0030, 0x02068CF3},
        {0x0074, 0x000498C3}, {0x0034, 0x00048C63}, {0x0014, 0x00048843},
        {0x000A, 0x00048E8F}, {0x0008, 0x02068EF7}}, /* low ADC rate mode */
};

static const uint32 rfseqext_rev51_6gl[2][11][2] = {
    /* https://confluence.broadcom.net/display/WLAN/RFSeqExt+table#RFSeqExttable-Version2 */
    {
        {0x006A, 0x0006B1B3}, {0x002A, 0x00069EDB}, {0x000A, 0x00069293},
        {0x000A, 0x00069293}, {0x000A, 0x00069293}, {0x002A, 0x00069EDB},
        {0x006A, 0x0004B1B3}, {0x002A, 0x00049EDB}, {0x000A, 0x00049293},
        {0x000A, 0x00049097}, {0x000A, 0x00069097}}, /* normal ADC rate mode */
    {
        {0x0068, 0x0206B0F3}, {0x0028, 0x02069EF3}, {0x0008, 0x020692F3},
        {0x0008, 0x020692F3}, {0x0008, 0x020692F3}, {0x0028, 0x02069EF3},
        {0x006A, 0x0004B1B3}, {0x002A, 0x00049EDB}, {0x000A, 0x00049293},
        {0x000A, 0x00049097}, {0x0008, 0x020690F7}}, /* low ADC rate mode */
};

static const uint32 rfseqext_rev51_6gl_special[2][11][2] = {
    /* https://confluence.broadcom.net/display/WLAN/RFSeqExt+table#RFSeqExttable-Version2 */
    {
        {0x006A, 0x0006B1B3}, {0x002A, 0x00069EDB}, {0x000A, 0x00069293},
        {0x000A, 0x00069293}, {0x000A, 0x00069293}, {0x002A, 0x00069EDB},
        {0x006A, 0x0004B1B3}, {0x002A, 0x00049EDB}, {0x000A, 0x00049293},
        {0x000A, 0x0004908F}, {0x000A, 0x0006908F}}, /* normal ADC rate mode */
    {
        {0x0068, 0x0206B0F3}, {0x0028, 0x02069EF3}, {0x0008, 0x020692F3},
        {0x0008, 0x020692F3}, {0x0008, 0x020692F3}, {0x0028, 0x02069EF3},
        {0x006A, 0x0004B1B3}, {0x002A, 0x00049EDB}, {0x000A, 0x00049293},
        {0x000A, 0x0004908F}, {0x0008, 0x020690F7}}, /* low ADC rate mode */
};

static const uint32 rfseqext_rev51_6gu[2][11][2] = {
    /* https://confluence.broadcom.net/display/WLAN/RFSeqExt+table#RFSeqExttable-Version2 */
    {
        {0x006A, 0x0006BDE3}, {0x002A, 0x00069EF3}, {0x000A, 0x000692A3},
        {0x000A, 0x000692A3}, {0x000A, 0x000692A3}, {0x002A, 0x00069EF3},
        {0x006A, 0x0004BDE3}, {0x002A, 0x00049EF3}, {0x000A, 0x000492A3},
        {0x000A, 0x000492A7}, {0x000A, 0x000692A7}}, /* normal ADC rate mode */
    {
        {0x0068, 0x0206BCF3}, {0x0028, 0x02069EF3}, {0x0008, 0x020692F3},
        {0x0008, 0x020692F3}, {0x0008, 0x020692F3}, {0x0028, 0x02069EF3},
        {0x006A, 0x0004BDE3}, {0x002A, 0x00049EF3}, {0x000A, 0x000492A3},
        {0x000A, 0x000492A7}, {0x0008, 0x020692F7}}, /* low ADC rate mode */
};

static const uint32 rfseqext_rev51_2G_dacdiv10[2][11][2] = {
    /* https://confluence.broadcom.net/display/WLAN/RFSeqExt+table#RFSeqExttable-Version2 */
    {
        {0x0074, 0x000694C3}, {0x0034, 0x00068C63}, {0x0014, 0x00068843},
        {0x0014, 0x00068843}, {0x0014, 0x00068843}, {0x0034, 0x00068C63},
        {0x0074, 0x000494C3}, {0x0034, 0x00048C63}, {0x0014, 0x00048843},
        {0x000C, 0x00048E47}, {0x000C, 0x00068E47}}, /* normal ADC rate mode */
    {
        {0x0070, 0x020694F3}, {0x0030, 0x02068CF3}, {0x0010, 0x020688F3},
        {0x0010, 0x020688F3}, {0x0010, 0x020688F3}, {0x0030, 0x02068CF3},
        {0x0074, 0x000494C3}, {0x0034, 0x00048C63}, {0x0014, 0x00048843},
        {0x000C, 0x00048E47}, {0x0008, 0x02068EF7}}, /* low ADC rate mode */
};

/**
 * XXX It is undesirable for Veloce clocks to run higher than a couple of hundreds of Mhz, since
 * higher would slow down the simulation. Therefore, the DAC and ADC clock divider values are
 * different in Veloce mode.
 */
static const uint32 rfseqext_veloce_2g[2][11][2] = {
    {
        {0x0068, 0x001E8843}, {0x002A, 0x00168423}, {0x000A, 0x000E8003},
        {0x0020, 0x00068003}, {0x0020, 0x00068003}, {0x0020, 0x00068003},
        {0x0068, 0x001C8843}, {0x002A, 0x00148423}, {0x000A, 0x000C8003},
        {0x000A, 0x00048007}, {0x000A, 0x00068007}},  /* normal ADC rate mode */
    {
        {0x0068, 0x001E8843}, {0x002A, 0x00168423}, {0x000A, 0x000E8003},
        {0x0020, 0x00068003}, {0x0020, 0x00068003}, {0x0020, 0x00068003},
        {0x0068, 0x001C8843}, {0x002A, 0x00148423}, {0x000A, 0x000C8003},
        {0x000A, 0x00048007}, {0x000A, 0x00068007}}, /* copy, no low ADC rate */
};

static const uint32 rfseqext_veloce_5g[2][11][2] = {
    {
        {0x006A, 0x001EB183}, {0x002A, 0x001698C3}, {0x000A, 0x000E9083},
        {0x0020, 0x00068043}, {0x0020, 0x00068023}, {0x0020, 0x00068443},
        {0x006A, 0x001CB183}, {0x002A, 0x001498C3}, {0x000A, 0x000C9083},
        {0x000A, 0x00049087}, {0x000A, 0x00069087}},  /* normal ADC rate mode */
    {
        {0x006A, 0x001EB183}, {0x002A, 0x001698C3}, {0x000A, 0x000E9083},
        {0x0020, 0x00068043}, {0x0020, 0x00068023}, {0x0020, 0x00068443},
        {0x006A, 0x001CB183}, {0x002A, 0x001498C3}, {0x000A, 0x000C9083},
        {0x000A, 0x00049087}, {0x000A, 0x00069087}}, /* copy, no low ADC rate */
};

#endif /* REGULATORY_DEBUG */

/* filter_type
 *    0 = used for jason chan-14:
 *        alpha = 0.2, in band response is the most flat, but ACPR is the worst
 *
 *    1 = used for 43602:
 *        Gaussian shaping filter (-0.5 dB Tx Power)
 *        alpha = 0.375, Fine tune scaling to align pwr_vs_idx with ofdm
 *
 *    2 = used for 4365/43684:
 *        alpha = 0.75, in band is the sharpest one, but ACPR is the best
 *
 *    table entry packing details:
 *        entry-0:  bphy20in20st0a2(B[23:12]) | bphy20in20st0a1(B[11:0])
 *        entry-1:  bphy20in20st1a2(B[23:12]) | bphy20in20st1a1(B[11:0])
 *        entry-2:  bphy20in20st2a2(B[23:12]) | bphy20in20st2a1(B[11:0])
 *        entry-3:  bphy20in20finescale(B[19:12]) | bphy20in20st2n(B[11:8]) |
 *                  bphy20in20st1n(B[7:4]) | bphy20in20st0n(B[3:0])
 */
const uint32 bphy_filt_coefs_rev130[][4] = {
    {0x339f6b, 0x1e5e29, 0x0f0cb2, 0x0a7323}, // filter_type = 0
    {0x1cbedb, 0x192d1d, 0x0f3c33, 0x07b333}, // filter_type = 1
    {0x1abedb, 0x172d1d, 0x0a9c77, 0x079333}, // filter_type = 2
};

/* Location of 20MHz bw afediv values in RFSeqExt table */
/* tx_20 tx_40 tx_80 tx_20_80 tx_40_80 tx_20_40 rx_20 rx_40 rx_80 tx_160 rx_160 */
#define RFSEQEXT_TX20 0
#define RFSEQEXT_RX20 6
#define RFSEQEXT_RX40 7
#define RFSEQEXT_RX80 8
#define RFSEQEXT_RX160 9

#define NUM_SUBBANDS_AV_VMID        (5)
#define NUM_VALUES_AV_VMID            (2)
#define MAX_NUM_OF_CORES_AV_VMID    (3)

static uint8
avvmid_set_maj36[][NUM_SUBBANDS_AV_VMID][MAX_NUM_OF_CORES_AV_VMID * NUM_VALUES_AV_VMID] = {
    {
    /* pdet_id = 0 */
        {5, 0, 0, 100, 0, 0},
        {3, 0, 0, 141, 0, 0},
        {3, 0, 0, 141, 0, 0},
        {3, 0, 0, 141, 0, 0},
        {3, 0, 0, 141, 0, 0}},
};

static uint8
avvmid_set_maj128[][NUM_SUBBANDS_AV_VMID][MAX_NUM_OF_CORES_AV_VMID * NUM_VALUES_AV_VMID] = {
    {
        /* pdet_id = 0 */
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0}},
        {
        /* pdet_id = 1 */
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0}},
        {
        /* pdet_id = 2 */
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0}},
        {
        /* pdet_id = 3 */
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0},
        {2, 2, 0, 135, 135, 0}},
};

uint8 papdluttbl[128] = {
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
    0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,
};
uint dynradioregtbl[2] = {35, 63};

uint8 *BCMRAMFN(get_avvmid_set_36)(phy_info_t *pi, uint16 pdet_range_id, uint16 subband_id);

uint8 *BCMRAMFN(get_avvmid_set_128)(phy_info_t *pi, uint16 pdet_range_id, uint16 subband_id);

uint16 const rfseq_rx2tx_dly[] =
    {0x8, 0x6, 0x6, 0x4, 0x4, 0x2, 0x10, 0x26, 0x2, 0x5, 0x1, 0x4, 0xfa, 0xfa, 0x2, 0x1};
uint16 const tiny_rfseq_rx2tx_cmd[] =
    {0x42, 0x1, 0x2, 0x8, 0x5, 0x6, 0x3, 0xf, 0x4, 0x35, 0xf, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};
uint16 const tiny_rfseq_rx2tx_dly[] =
    {0x8, 0x6, 0x6, 0x4, 0x4, 0x10, 0x26, 0x2, 0x5, 0x4, 0xFA, 0xFA, 0x1, 0x1, 0x1, 0x1};
uint16 const tiny_rfseq_rx2tx_tssi_sleep_cmd[] =
    {0x42, 0x1, 0x2, 0x8, 0x5, 0x6, 0x3, 0xf, 0x4, 0x35, 0xf, 0x00, 0x00, 0x36, 0x1f, 0x1f};
uint16 const tiny_rfseq_rx2tx_tssi_sleep_dly[] =
    {0x8, 0x6, 0x6, 0x4, 0x4, 0x10, 0x26, 0x2, 0x5, 0x4, 0xFA, 0xFA, 0x88, 0x1, 0x1, 0x1};
uint16 const tiny_rfseq_tx2rx_cmd[] =
    {0x4, 0x3, 0x6, 0x5, 0x85, 0x2, 0x1, 0x8, 0x2a, 0xf, 0x0, 0xf, 0x2b, 0x43, 0x1F};

/* tiny major rev4 RF Sequences : START */

/* Reset2Rx */
/* changing RF sequencer to add DCC reset */
static const uint16 rfseq_majrev4_reset2rx_cmd[] = {0x84, 0x4, 0x3, 0x6, 0x5, 0x2, 0x1, 0x8,
    0x2a, 0x2b, 0xf, 0x0, 0x0, 0x85, 0x41, 0x1f};
uint16 const rfseq_majrev4_reset2rx_dly[] = {10, 12, 2, 2, 4, 4, 6, 1, 4, 1, 2, 10, 1, 1, 1, 1};

/* Tx2Rx */
uint16 const rfseq_majrev4_tx2rx_cmd[] =
    {0x84, 0x4, 0x3, 0x6, 0x5, 0x85, 0x2, 0x1, 0x8, 0x2a, 0xf, 0x0, 0xf, 0x2b, 0x43, 0x1F};
uint16 const rfseq_majrev4_tx2rx_dly[] =
    {0x8, 0x8, 0x4, 0x2, 0x2, 0x1, 0x3, 0x4, 0x6, 0x4, 0x1, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1};

/* Rx2Tx */
/* Refer to tiny_rfseq_rx2tx_cmd */

/* Rx2Tx -- Cal */
uint16 const rfseq_majrev4_rx2tx_cal_cmd[] =
    {0x84, 0x1, 0x2, 0x8, 0x5, 0x3d, 0x85, 0x6, 0x3, 0xf, 0x4, 0x3e, 0x35, 0xf, 0x36, 0x1f};
uint16 const rfseq_majrev4_rx2tx_cal_dly[] =
    {0x8, 0x6, 0x6, 0x4, 0x4, 0x2, 0x12, 0x10, 0x26, 0x2, 0x5, 0x1, 0x4, 0xfa, 0x2, 0x1};
/* tiny major rev4 RF Sequences : END */

// rev32 rfseq
uint16 const rfseq_majrev32_rx2tx_cal_cmd[] =
    {0x84, 0x1, 0x2, 0x8, 0x5, 0x0, 0x85, 0x6, 0x3, 0xf, 0x4, 0x0, 0x35, 0xf, 0x36, 0x1f};
uint16 const rfseq_majrev32_rx2tx_cal_dly[] =
    {0x8, 0x6, 0x6, 0x4, 0x4, 0x2, 0x12, 0x10, 0x26, 0x17, 0x5, 0x1, 0x4, 0xfa, 0x2, 0x1};
uint16 const rfseq_majrev32_rx2tx_cmd[]    = {0x42, 0x01, 0x02, 0x08, 0x05, 0x06, 0x03, 0x0f,
    0x04, 0x35, 0x0f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};
uint16 const rfseq_majrev32_tx2rx_cmd[]    = {0x84, 0x04, 0x03, 0x06, 0x05, 0x85, 0x02, 0x01,
    0x08, 0x2a, 0x0f, 0x00, 0x0f, 0x2b, 0x43, 0x1F};
uint16 const rfseq_majrev32_reset2rx_cmd[] = {0x0, 0x04, 0x03, 0x06, 0x05, 0x02, 0x01, 0x08,
    0x2a, 0x2b, 0x0f, 0x00, 0x00, 0x0, 0x1f, 0x1f};
// change PA PU delay to 0x17 from 0x2 to resolve the LOFT issues
uint16 const rfseq_majrev32_rx2tx_dly[]    = {0x08, 0x06, 0x06, 0x04, 0x04, 0x10, 0x26, 0x17,
    0x05, 0x04, 0xFA, 0xFA, 0x01, 0x01, 0x01, 0x01};
uint16 const rfseq_majrev32_tx2rx_dly[]    = {0x08, 0x08, 0x04, 0x02, 0x02, 0x01, 0x03, 0x04,
    0x06, 0x04, 0x01, 0x02, 0x01, 0x01, 0x01, 0x01};
uint16 const rfseq_majrev32_reset2rx_dly[] = {0x0A, 0x0C, 0x02, 0x02, 0x04, 0x04, 0x06, 0x01,
    0x04, 0x01, 0x02, 0x0A, 0x01, 0x01, 0x01, 0x01};

/* Major rev47 RF Sequences : START */
/* Bundle scheme */
static const uint16 rfseq_majrev47_bundleScheme2 = 1;
/* Reset2Rx */
static const uint16 rfseq_majrev47_reset2rx_cmd[] =
    {0x4, 0x3, 0x6, 0x2, 0x5, 0x1, 0x8, 0x2a, 0x2b, 0xf, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};
static const uint16 rfseq_majrev47_reset2rx_dly[] =
    {0xc, 0x2, 0x2, 0x4, 0x4, 0x6, 0x1, 0x4, 0x1, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1};
/* Tx2Rx */
static const uint16 rfseq_majrev47_tx2rx_cmd[] =
    {0x85, 0x4, 0x3, 0x6, 0x5, 0x3d, 0x2, 0x1, 0x8, 0x2a, 0xf, 0x3e, 0xf, 0x2b, 0x86, 0x1f};
static const uint16 rfseq_majrev47_tx2rx_dly[] =
    {0x2, 0x8, 0x4, 0x2, 0x2, 0x1, 0x3, 0x4, 0x6, 0x4, 0xa, 0x4, 0x2, 0x1, 0x2, 0x1};
static const uint16 rfseq_majrev129_tx2rx_cmd[] =
    {0x4, 0x3, 0x6, 0x5, 0x3d, 0x2, 0x1, 0x8, 0x2a, 0xf, 0x3e, 0xf, 0x2b, 0x85, 0x86, 0x1f};
static const uint16 rfseq_majrev129_tx2rx_dly[] =
    {0x8, 0x4, 0x2, 0x2, 0x1, 0x3, 0x4, 0x6, 0x4, 0xa, 0x4, 0x2, 0x1, 0x4, 0x1, 0x1};
static const uint16 rfseq_majrev130_tx2rx_cmd[] =
    {0x85, 0x6, 0x5, 0x4, 0x3, 0x3d, 0x2, 0x1, 0x8, 0x2a, 0xf, 0x3e, 0xf, 0x2b, 0x86, 0x1f};
static const uint16 rfseq_majrev130_tx2rx_dly[] =
    {0x2, 0x2, 0x2, 0x8, 0x4, 0x1, 0x3, 0x4, 0x6, 0x4, 0xa, 0x4, 0x2, 0x1, 0x2, 0x1};

/* Rx2Tx */

static const uint16 rfseq_majrev47_rx2tx_cmd[] =
    {0x0, 0x1, 0x2, 0x8, 0x3d, 0x5, 0x3e, 0x6, 0x3, 0xf, 0x4, 0x35, 0xf, 0x0, 0x36, 0x1f};
static const uint16 rfseq_majrev47_rx2tx_dly[] =
    {0x18, 0x6, 0x6, 0x4, 0x4, 0x4, 0x1, 0x10, 0x26, 0x2, 0x1, 0x4, 0xfa, 0xfa, 0x2, 0x1};
/* Major rev47 RF Sequences : END */

/* Major rev128 RF Sequences : START */
/* Reset2Rx */
static const uint16 rfseq_majrev128_reset2rx_cmd[] =
    {0x4, 0x3, 0x6, 0x2, 0x5, 0x1, 0x8, 0x2a, 0x2b, 0xf, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f};
static const uint16 rfseq_majrev128_reset2rx_dly[] =
    {0xc, 0x2, 0x2, 0x4, 0x4, 0x6, 0x1, 0x4, 0x1, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1};
/* Tx2Rx */
static const uint16 rfseq_majrev128_tx2rx_cmd[] =
    {0x85, 0x4, 0x3, 0x6, 0x3d, 0x5, 0x2, 0x1, 0x8, 0x2a, 0xf, 0x3e, 0xf, 0x2b, 0x86, 0x1f};
static const uint16 rfseq_majrev128_tx2rx_dly[] =
    {0x2, 0x8, 0x4, 0x2, 0x2, 0x2, 0x3, 0x4, 0x6, 0x4, 0xa, 0x4, 0x2, 0x1, 0x2, 0x1};
static const uint16 rfseq_majrev128_tx2rx_cal_cmd[] =
    {0x4, 0x3, 0x6, 0x3d, 0x5, 0x2, 0x1, 0x8, 0x2a, 0xf, 0x3e, 0xf, 0x2b, 0x1f, 0x1f, 0x1f};
static const uint16 rfseq_majrev128_tx2rx_cal_dly[] =
    {0x8, 0x4, 0x2, 0x2, 0x2, 0x3, 0x4, 0x6, 0x4, 0xa, 0x4, 0x2, 0x1, 0x1, 0x1, 0x1};
/* Rx2Tx */
static const uint16 rfseq_majrev128_rx2tx_cmd[] =
    {0x0, 0x1, 0x2, 0x8, 0x3d, 0x5, 0x3e, 0x6, 0x3, 0xf, 0x4, 0x35, 0xf, 0x0, 0x36, 0x1f};
static const uint16 rfseq_majrev128_rx2tx_dly[] =
    {0x18, 0x6, 0x6, 0x4, 0x4, 0x4, 0x1, 0x10, 0x26, 0x2, 0x1, 0x4, 0xfa, 0xfa, 0x2, 0x1};
/* Major rev128 RF Sequences : END */

/* Major rev130 RF Sequences : START */
static const uint16 rfseq_majrev130_rx2tx_cmd[] =
    {0x98, 0x1, 0x2, 0x8, 0x3d, 0x5, 0x3e, 0x6, 0x3, 0xf, 0x4, 0x35, 0xf, 0x0, 0x36, 0x1f};
static const uint16 rfseq_majrev130_rx2tx_dly[] =
    {0x18, 0x6, 0x6, 0x4, 0x4, 0x4, 0x1, 0x10, 0x26, 0x2, 0x1, 0x4, 0xfa, 0xfa, 0x2, 0x1};
/* Major rev130 RF Sequences : END */

/* Channel smoothing MTE filter image */
#define CHANSMTH_FLTR_LENGTH 64
static CONST uint16 acphy_Smth_tbl_4349[] = {
    0x4a5c, 0xdba7, 0x1672,
    0xb167, 0x742d, 0xa5ca,
    0x4afe, 0x4aa6, 0x14f3,
    0x4176, 0x6f25, 0xa75a,
    0x7aca, 0xeca4, 0x1e94,
    0xf177, 0x4e27, 0xa7fa,
    0x0b46, 0xcead, 0x270c,
    0x3169, 0x4f1d, 0xa70b,
    0xda4e, 0xcb35, 0x1431,
    0xd1d2, 0x572e, 0xae6b,
    0x8a4b, 0x68bc, 0x1f62,
    0x81f6, 0xc826, 0xa4bb,
    0x2add, 0x6b37, 0x1d42,
    0xcaff, 0xdd9e, 0x0c6a,
    0xd0c6, 0xecad, 0xaff9,
    0xbad8, 0xe69d, 0x173a,
    0x20d1, 0xf5b7, 0xa579,
    0x6b71, 0xdb9c, 0x156a,
    0x60d6, 0xf345, 0xa6f9,
    0x0b42, 0xc6a6, 0x1f5a,
    0xb0d4, 0xe22e, 0x9c19,
    0x4bc4, 0x5aaf, 0x1c6b,
    0xc0cc, 0xc326, 0x9c49,
    0x1cf1, 0xddb7, 0x243b,
    0xe17a, 0xe21c, 0xa75a,
    0x6a50, 0xcb35, 0x1441,
    0xb1d3, 0x5d2e, 0xae4b,
    0x8a4b, 0x67bd, 0x1f72,
    0x71f7, 0xd826, 0xa4bb,
    0xfade, 0x6b36, 0x1d42,
    0xe153, 0xcf96, 0x0fc8,
    0xf0fc, 0x6e8c, 0x1539,
    0xd1fd, 0x7d94, 0x0da9,
    0xd047, 0xc08c, 0x1578,
    0x41c9, 0x4c9d, 0x1679,
    0xe043, 0x7696, 0x1459,
    0xf2f7, 0x7faf, 0x1d1a,
    0xd0e4, 0x4c9c, 0x1c49,
    0xe37e, 0xca9c, 0x1782,
    0x31ff, 0x7ba4, 0x2f1a,
    0xd243, 0xe69d, 0x16ba,
    0x616b, 0xddae, 0x2439,
    0xdc69, 0x46ae, 0x1fb2,
    0xf0c9, 0x5a97, 0x0658,
    0xa065, 0x7f85, 0x0c99,
    0xd174, 0x4a95, 0x0508,
    0x2074, 0xce86, 0x0d38,
    0xb152, 0xea9f, 0x0f08,
    0xd078, 0xd785, 0x0d38,
    0x71e3, 0xc29c, 0x0c48,
    0xc06e, 0xd684, 0x0c88,
    0x4262, 0x42a4, 0x1439,
    0x0058, 0xc78e, 0x1658,
    0xb2c4, 0x5cb5, 0x25da,
    0x60f5, 0x5694, 0x1dd9,
    0x02c6, 0xc39d, 0x1792,
    0x61ff, 0x7ba4, 0x2f3a,
    0xf246, 0xee9d, 0x16ca,
    0xe16c, 0xdfae, 0x2469,
    0x2c4d, 0x44af, 0x1fd2,
    0x0bcd, 0x4faf, 0x1c5b,
    0x30cb, 0x7e27, 0x9c6a,
    0xec42, 0xd3b6, 0x243b,
    0x0179, 0xd81d, 0xa77a
 };
static CONST uint16 acphy_Smth_tbl_tiny[] = {
    0x5fd2,    0x16fc,    0x0ce0,
    0x60ce,    0xc501,    0xfd2f,
    0xefe0,    0x09fc,    0x09e0,
    0x90eb,    0xc802,    0xfc5f,
    0xcfed,    0x01fd,    0x0690,
    0xf0ed,    0xd903,    0xfc0f,
    0xcff7,    0xfefe,    0x037f,
    0x30d2,    0xf605,    0xfc7f,
    0xbfd8,    0x4b00,    0x0860,
    0xb052,    0xf501,    0xfe6f,
    0xbfda,    0x33ff,    0x0750,
    0x3075,    0xfb03,    0xfdaf,
    0xefe8,    0x3500,    0x0530,
    0x4fe0,    0xe8f9,    0x119f,
    0x8119,    0x94fe,    0xfe0f,
    0x5fea,    0xe6fa,    0x0e5f,
    0x1142,    0x8aff,    0xfd4f,
    0xaff1,    0xe9fb,    0x0acf,
    0x2156,    0x8d00,    0xfc8f,
    0xfff7,    0xeefc,    0x075f,
    0xa151,    0x9d01,    0xfbef,
    0x2ffb,    0xf4fe,    0x045f,
    0x612f,    0xbd03,    0xfbbf,
    0x1ffe,    0xfaff,    0x021f,
    0xe0f4,    0xe704,    0xfc5f,
    0xafd8,    0x4b00,    0x0880,
    0xa052,    0xf401,    0xfe7f,
    0xafda,    0x33ff,    0x0770,
    0x3077,    0xfa03,    0xfdaf,
    0xdfe8,    0x3500,    0x0540,
    0x30a5,    0xc5f2,    0x1f1e,
    0x51f1,    0x23ec,    0x0a5f,
    0x607c,    0x06f6,    0x167f,
    0xb236,    0xffec,    0x0ade,
    0xc049,    0x67fa,    0x0cdf,
    0x4214,    0x13f2,    0x089f,
    0x001d,    0xc0fe,    0x051f,
    0x2191,    0x68fb,    0x044f,
    0x100f,    0x12fb,    0x0ef0,
    0xe07f,    0xc2fd,    0x01cf,
    0x2021,    0xe6fa,    0x0d5f,
    0x60d5,    0xa2fe,    0x021f,
    0x4ffc,    0x22fe,    0x07b0,
    0x2125,    0x2cf0,    0x32bd,
    0xc32b,    0x02d2,    0x125f,
    0x50e8,    0xb0f4,    0x27ed,
    0xe3a0,    0xc7ce,    0x14be,
    0x40a3,    0x57f8,    0x1bee,
    0x43bd,    0xa3d1,    0x14ce,
    0x8062,    0xf9fb,    0x10ee,
    0x3370,    0xa7da,    0x11fe,
    0xe030,    0x7cfd,    0x085f,
    0x12c1,    0xe2e8,    0x0c9e,
    0x4010,    0xd1ff,    0x02ef,
    0x41d4,    0x54f7,    0x05df,
    0xf011,    0x10fa,    0x0f10,
    0xd07f,    0xc2fd,    0x01cf,
    0x1023,    0xe4fa,    0x0d7f,
    0x40d7,    0xa1fe,    0x023f,
    0x3ffd,    0x22fe,    0x07c0,
    0x3ffb,    0xf6fe,    0x044f,
    0x912a,    0xc103,    0xfbaf,
    0x2ffd,    0xfaff,    0x021f,
    0xf0f1,    0xe904,    0xfc4f,
};

/* China 40M Spur WAR */
static const uint16 resamp_cnwar_5270[] = {0x4bda, 0x0038, 0x10e0, 0x4bda, 0x0038, 0x10e0,
0xed0e, 0x0068, 0xed0e, 0x0068};
static const uint16 resamp_cnwar_5310[] = {0x0000, 0x00d8, 0x0b40, 0x0000, 0x00d8, 0x0b40,
0x6c79, 0x0045, 0x6c79, 0x0045};

#ifdef WLSMC
static const uint32 Ndbps_ru_LUT[9][8] = {
{0x0018000C, 0x00300024, 0x00600048, 0x0078006C, 0x00A00090, 0x00C800B4, 0x00D200D2, 0x00D200D2},
{0x00660033, 0x00CC0099, 0x01980132, 0x01FE01CB, 0x02A80264, 0x035202FD, 0x037C037C, 0x037C037C},
{0x00EA0075, 0x01D4015F, 0x03A802BE, 0x0492041D, 0x0618057C, 0x079E06DB, 0x07FE07FE, 0x07FE07FE},
{0x03D401EA, 0x07A805BE, 0x0F500B7C, 0x1324113A, 0x198516F8, 0x1FE61CB6, 0x217F217F, 0x217F217F},
{0x00060003, 0x000C0009, 0x00180012, 0x001E001B, 0x00280024, 0x0032002D, 0x00320032, 0x00320032},
{0x0018000C, 0x00300024, 0x00600048, 0x0078006C, 0x00A00090, 0x00C800B4, 0x00C800C8, 0x00C800C8},
{0x003C001E, 0x0078005A, 0x00F000B4, 0x012C010E, 0x01900168, 0x01F401C2, 0x01F401F4, 0x01F401F4},
{0x00F00078, 0x01E00168, 0x03C002D0, 0x04B00438, 0x064005A0, 0x07D00708, 0x07D007D0, 0x07D007D0},
{0x01EC00F6, 0x03D802E2, 0x07B005C4, 0x099C08A6, 0x0CD00B88, 0x10040E6A, 0x10041004, 0x10041004}};

static const uint32 n_cbps_ru_LUT[3][6] = {
{0x00060003, 0x000C0006, 0x0012000C, 0x00120012, 0x00180018, 0x00180018},
{0x001A000D, 0x0033001A, 0x004D0033, 0x004D004D, 0x00660066, 0x00660066},
{0x003B001E, 0x0075003B, 0x00B00075, 0x00B000B0, 0x00EA00EA, 0x00EA00EA}};

static const uint32 dummy_rxctrl[] = {
0x20003001, 0x00000050, 0x01010000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
0x00000000, 0x00000000, 0x081a0281, 0x7fc00010, 0x0058240b, 0x440c0000};
#endif /* WLSMC */

static void wlc_phy_set_noise_var_shaping_acphy(phy_info_t *pi,
uint8 noise_var[][ACPHY_SPURWAR_NV_NTONES], int8 *tone_id, uint8 *core_nv);
static void phy_ac_chanmgr_papr_iir_filt_reprog(phy_info_t *pi);
static void chanspec_setup_papr(phy_info_t *pi, int8 papr_final_clipping, int8 papr_final_scaling);
static void wlc_phy_spurwar_nvshp_acphy(phy_info_t *pi, bool bw_chg, bool spurwar, bool nvshp);
static void wlc_phy_write_rx_farrow_acphy(phy_ac_chanmgr_info_t *ci, chanspec_t chanspec);

#ifndef WL_FDSS_DISABLED
static void wlc_phy_fdss_init(phy_info_t *pi);
static void wlc_phy_set_fdss_table(phy_info_t *pi);
static void wlc_phy_set_fdss_table_majrev_ge40(phy_info_t *pi);
#endif

static void wlc_phy_ac_lpf_cfg(phy_info_t *pi);
static void wlc_acphy_load_4349_specific_tbls(phy_info_t *pi);
static void wlc_acphy_load_radiocrisscross_phyovr_mode(phy_info_t *pi);
static void wlc_acphy_load_logen_tbl(phy_info_t *pi);
static void wlc_phy_set_regtbl_on_band_change_acphy_20693(phy_info_t *pi);
static void wlc_phy_load_channel_smoothing_tiny(phy_info_t *pi);
static void wlc_phy_set_reg_on_reset_acphy_20693(phy_info_t *pi);
static void wlc_phy_set_tbl_on_reset_acphy(phy_info_t *pi);
static void wlc_phy_set_regtbl_on_band_change_acphy(phy_info_t *pi);
static void wlc_phy_set_regtbl_on_bw_change_acphy(phy_info_t *pi);
static void wlc_phy_set_sdadc_pd_val_per_core_acphy(phy_info_t *pi);
static void chanspec_setup_regtbl_on_chan_change(phy_info_t *pi);
static void wlc_phy_set_sfo_on_chan_change_acphy(phy_info_t *pi, chanspec_t chanspec);
static void wlc_phy_write_sfo_params_acphy(phy_info_t *pi, uint16 fc);
static void wlc_phy_write_sfo_params_acphy_wave2(phy_info_t *pi, const uint16 *val_ptr);
static void wlc_phy_write_sfo_80p80_params_acphy(phy_info_t *pi, const uint16 *val_ptr,
const uint16 *val_ptr1);
static void acphy_load_txv_for_spexp(phy_info_t *pi);
static void wlc_phy_cfg_energydrop_timeout(phy_info_t *pi);
static void wlc_phy_set_reg_on_bw_change_acphy_noHWPhyReset(phy_info_t *pi);
static void wlc_phy_set_reg_on_bw_change_acphy_majorrev128(phy_info_t *pi);
static void wlc_phy_set_reg_on_bw_change_acphy_majorrev130(phy_info_t *pi);
static void wlc_phy_set_pdet_on_reset_acphy(phy_info_t *pi);
static void wlc_phy_program_bphy_txfilt_coeffs(phy_info_t *pi, uint8 filter_type);
static void wlc_phy_set_tx_iir_coeffs(phy_info_t *pi, bool cck, uint8 filter_type);
static void wlc_phy_tx_gm_gain_boost(phy_info_t *pi);
static void wlc_phy_write_rx_farrow_pre_tiny(phy_info_t *pi, chan_info_rx_farrow *rx_farrow,
chanspec_t chanspec);
static void wlc_phy_set_reg_on_reset_acphy(phy_info_t *pi);
static void wlc_phy_set_regtbl_on_rxchain(phy_info_t *pi, bool nrx_changed);
static void wlc_phy_set_analog_tx_lpf(phy_info_t *pi, uint16 mode_mask, int bq0_bw, int bq1_bw,
int rc_bw, int gmult, int gmult_rc, int core_num);
static void wlc_phy_set_tx_afe_dacbuf_cap(phy_info_t *pi, uint16 mode_mask, int dacbuf_cap,
int dacbuf_fixed_cap, int core_num);
static void wlc_phy_set_analog_rx_lpf(phy_info_t *pi, uint8 mode_mask, int bq0_bw, int bq1_bw,
int rc_bw, int gmult, int gmult_rc, int core_num);
#ifndef ACPHY_1X1_ONLY
static void wlc_phy_write_tx_farrow_acphy(phy_ac_chanmgr_info_t *ci, chanspec_t chanspec);
#endif
static void wlc_phy_radio20693_set_reset_table_bits(phy_info_t *pi, uint16 tbl_id, uint16 offset,
uint16 start, uint16 end, uint16 val, uint8 tblwidth);
static void wlc_acphy_dyn_papd_cfg_20693(phy_info_t *pi);
static void wlc_phy_set_bias_ipa_as_epa_acphy_20693(phy_info_t *pi, uint8 core);

static void wlc_phy_farrow_setup_28nm(phy_info_t *pi, uint16 dac_rate_mode);
static void wlc_phy_tx_precorr_sfo_setup(phy_info_t *pi);
static void wlc_phy_td_sfo_eco(phy_info_t *pi);
static void wlc_phy_ul_mac_aided(phy_info_t *pi);
static void wlc_phy_ul_mac_aided_timing(phy_info_t *pi);
static void wlc_phy_ul_mac_aided_war(phy_info_t *pi);
static void wlc_axphy_dump_txctlplcp_on_reset(phy_info_t *pi);
/* chanspec handle */
typedef void (*chanspec_module_t)(phy_info_t *pi);
chanspec_module_t * BCMRAMFN(get_chanspec_module_list)(void);

/* setup */
static void chanspec_setup(phy_info_t *pi);
static void chanspec_setup_phy(phy_info_t *pi);
static void chanspec_setup_cmn(phy_info_t *pi);

/* tune */
static void chanspec_tune_phy(phy_info_t *pi);
static void chanspec_tune_txpath(phy_info_t *pi);
static void chanspec_tune_rxpath(phy_info_t *pi);

/* wars & features */
static void chanspec_fw_enab(phy_info_t *pi);

/* cleanup */
static void chanspec_cleanup(phy_info_t *pi);

/* other helper functions */
static void chanspec_phy_table_access_war(phy_info_t *pi);
static void chanspec_setup_hirssi_ucode_cap(phy_info_t *pi);
static void chanspec_sparereg_war(phy_info_t *pi);
static void chanspec_prefcbs_init(phy_info_t *pi);
static bool chanspec_papr_enable(phy_info_t *pi);

/* phy setups */
static void chanspec_setup_phy_ACMAJORREV_51_131(phy_info_t *pi);
static void chanspec_setup_phy_ACMAJORREV_GE47(phy_info_t *pi);
static void chanspec_setup_phy_ACMAJORREV_128(phy_info_t *pi);
static void chanspec_setup_phy_ACMAJORREV_32(phy_info_t *pi);
static void chanspec_setup_phy_ACMAJORREV_5(phy_info_t *pi);
static void chanspec_setup_phy_ACMAJORREV_4(phy_info_t *pi);
static void chanspec_setup_phy_ACMAJORREV_2(phy_info_t *pi);
static void chanspec_setup_phy_ACMAJORREV_0(phy_info_t *pi);

/* phy tunables */
static void chanspec_tune_phy_ACMAJORREV_47(phy_info_t *pi);
static void chanspec_tune_phy_ACMAJORREV_129(phy_info_t *pi);
static void chanspec_tune_phy_ACMAJORREV_130(phy_info_t *pi);
static void chanspec_tune_phy_ACMAJORREV_51_131(phy_info_t *pi);
static void chanspec_tune_phy_ACMAJORREV_128(phy_info_t *pi);
static void chanspec_tune_phy_ACMAJORREV_32(phy_info_t *pi);
static void chanspec_tune_phy_ACMAJORREV_5(phy_info_t *pi);
static void chanspec_tune_phy_ACMAJORREV_4(phy_info_t *pi);
static void chanspec_tune_phy_ACMAJORREV_2(phy_info_t *pi);
static void chanspec_tune_phy_ACMAJORREV_0(phy_info_t *pi);

static void chanspec_tune_phy_dccal(phy_info_t *pi, bool force);

void wlc_phy_ac_shared_ant_femctrl_master(phy_info_t *pi);

#if (defined(WL_SISOCHIP) || !defined(SWCTRL_TO_BT_IN_COEX))
static void wlc_phy_ac_femctrl_mask_on_band_change(phy_info_t *pi);
#endif

static int phy_ac_chanmgr_switch_phymode_acphy(phy_info_t *pi, uint32 phymode);

chanspec_module_t chanspec_module_list[] = {
    chanspec_setup,
    chanspec_setup_tpc,
    chanspec_setup_radio,
    chanspec_setup_phy,
    chanspec_setup_cmn,
    chanspec_noise,
    chanspec_setup_rxgcrs,
    chanspec_tune_radio,
    chanspec_tune_phy,
    chanspec_tune_txpath,
    chanspec_tune_rxpath,
    chanspec_fw_enab,
    chanspec_cleanup,
    chanspec_btcx,
    NULL
};

chanspec_module_t *
BCMRAMFN(get_chanspec_module_list)(void)
{
    return chanspec_module_list;
}

static void
wlc_phy_config_bias_settings_20693(phy_info_t *pi)
{
    uint8 core;
    ASSERT(RADIOID_IS(pi->pubpi->radioid, BCM20693_ID));

    FOREACH_CORE(pi, core) {
        RADIO_REG_LIST_START
            MOD_RADIO_REG_20693_ENTRY(pi, TRSW2G_CFG1, core, trsw2g_pu, 0)
            MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_2G_OVR1_EAST, core, ovr_trsw2g_pu, 1)
            MOD_RADIO_REG_20693_ENTRY(pi, TRSW2G_CFG1, core, trsw2g_bias_pu, 0)
            MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_2G_OVR1_EAST, core,
                ovr_trsw2g_bias_pu, 1)
            MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_2G_OVR1_EAST, core,
                ovr_mx2g_idac_bbdc, 1)
            MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR2, core, ovr_mx5g_idac_bbdc, 1)
            MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR2, core, ovr_pad5g_idac_pmos, 1)
            MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR2, core, ovr_pad5g_idac_gm, 1)
            MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR3, core,
                ovr_pa5g_bias_filter_main, 1)
        RADIO_REG_LIST_EXECUTE(pi, core);
    }
    if (ROUTER_4349(pi)) {
        /* 53573A0: 5G Tx Low Power Radio Optimizations */
        /* proc 20693_tx5g_set_bias_ipa_opt_sv */
        if (PHY_IPA(pi)) {
            bool BW40, BW80;

            FOREACH_CORE(pi, core) {
                if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
                    BW40 = (CHSPEC_IS40(pi->radio_chanspec));
                    BW80 = (CHSPEC_IS80(pi->radio_chanspec) ||
                        CHSPEC_IS8080(pi->radio_chanspec));

                    RADIO_REG_LIST_START
                        MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR1, core,
                            ovr_trsw5g_pu, 0x1)
                        MOD_RADIO_REG_20693_ENTRY(pi, TRSW5G_CFG1, core,
                            trsw5g_pu, 0x0)
                        MOD_RADIO_REG_20693_ENTRY(pi, SPARE_CFG7, core,
                            swcap_sec_gate_off_5g, 0xf)
                        MOD_RADIO_REG_20693_ENTRY(pi, SPARE_CFG7, core,
                            swcap_sec_sd_on_5g, 0x10)
                        MOD_RADIO_REG_20693_ENTRY(pi, SPARE_CFG6, core,
                            swcap_pri_pd_5g, 0x1)
                        MOD_RADIO_REG_20693_ENTRY(pi, SPARE_CFG6, core,
                            swcap_pri_5g, 0x0)
                        MOD_RADIO_REG_20693_ENTRY(pi, SPARE_CFG6, core,
                            swcap_pri_gate_off_5g, 0x0)
                        MOD_RADIO_REG_20693_ENTRY(pi, SPARE_CFG6, core,
                            swcap_pri_sd_on_5g, 0x0)
                        MOD_RADIO_REG_20693_ENTRY(pi, PA5G_CFG3, core,
                            pa5g_ptat_slope_main, 0x0)
                        MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR2, core,
                            ovr_pa5g_idac_incap_compen_main, 0x1)
                    RADIO_REG_LIST_EXECUTE(pi, core);
                    MOD_RADIO_REG_20693(pi, PA5G_INCAP, core,
                        pa5g_idac_incap_compen_main,
                        IS_ACR(pi) ? ((BW40 || BW80) ? 0xc : 0x3a) :
                        0x16);
                    MOD_RADIO_REG_20693(pi, PA5G_IDAC3, core,
                        pa5g_idac_tuning_bias, 0x0);

                    MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR2, core,
                        ovr_pad5g_idac_gm, 1);
                    MOD_RADIO_REG_20693(pi, TXMIX5G_CFG8, core, pad5g_idac_gm,
                        IS_ACR(pi) ? ((BW40 || BW80) ? 0x3f : 0x26) :
                        0x1a);
                    MOD_RADIO_REG_20693(pi, TXGM5G_CFG1, core,
                        pad5g_idac_cascode,
                        IS_ACR(pi) ? ((BW40 || BW80) ? 0xf : 0xe) :
                        0xd);
                    MOD_RADIO_REG_20693(pi, SPARE_CFG10, core,
                        pad5g_idac_cascode2, 0x0);
                    MOD_RADIO_REG_20693(pi, PA5G_INCAP, core, pad5g_idac_pmos,
                        (IS_ACR(pi) && (BW40 || BW80)) ? 0xa : 0x1c);

                    MOD_RADIO_REG_20693(pi, PA5G_IDAC3, core,
                        pad5g_idac_tuning_bias, 0xd);
                    MOD_RADIO_REG_20693(pi, TXMIX5G_CFG6, core,
                        mx5g_ptat_slope_cascode, 0x0);
                    MOD_RADIO_REG_20693(pi, TXMIX5G_CFG6, core,
                        mx5g_ptat_slope_lodc, 0x2);
                    MOD_RADIO_REG_20693(pi, TXMIX5G_CFG4, core,
                        mx5g_idac_cascode, 0xf);
                    MOD_RADIO_REG_20693(pi, TXMIX5G_CFG5, core,
                        mx5g_idac_lodc, 0xa);
                    MOD_RADIO_REG_20693(pi, TXMIX5G_CFG5, core, mx5g_idac_bbdc,
                        (BW40 && IS_ACR(pi)) ? 0x2 : 0xc);

                    MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR2, core,
                        ovr_pa5g_idac_main, 1);
                    MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR3, core,
                        ovr_pa5g_idac_cas, 1);

                    if (BW80) {
                        MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                            pa5g_idac_main, (IS_ACR(pi)) ? 0x38 : 0x3a);
                        MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                            pa5g_idac_cas, (IS_ACR(pi)) ? 0x14 : 0x15);
                    } else if (BW40) {
                        MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                            pa5g_idac_main, (IS_ACR(pi)) ? 0x34 : 0x28);
                        MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                            pa5g_idac_cas, (IS_ACR(pi)) ? 0x14 : 0x13);
                    } else {
                        MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                            pa5g_idac_main, (IS_ACR(pi)) ? 0x20 : 0x1a);
                        MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                            pa5g_idac_cas, (IS_ACR(pi)) ? 0x12 : 0x11);
                    }

                    MOD_RADIO_REG_20693(pi, TXMIX5G_CFG3, core,
                        mx5g_pu_bleed, 0x0);
                    MOD_RADIO_REG_20693(pi, TXMIX5G_CFG1, core,
                        mx5g_idac_bleed_bias, 0x0);
                    MOD_RADIO_REG_20693(pi, TXMIX5G_CFG4, core,
                        mx5g_idac_tuning_bias, 0xd);
                } else {
                    /* 2G Tx settings */
                    MOD_RADIO_REG_20693(pi, TX_TOP_2G_OVR1_EAST, core,
                            ovr_pa2g_idac_main, 0x1);
                    MOD_RADIO_REG_20693(pi, TX_TOP_2G_OVR1_EAST, core,
                            ovr_pa2g_idac_cas, 0x1);
                    if (CHSPEC_IS40(pi->radio_chanspec)) {
                        MOD_RADIO_REG_20693(pi, PA2G_IDAC1, core,
                                pa2g_idac_main, 0x1e);
                        MOD_RADIO_REG_20693(pi, PA2G_IDAC1, core,
                                pa2g_idac_cas, 0x21);
                    } else {
                        MOD_RADIO_REG_20693(pi, PA2G_IDAC1, core,
                                pa2g_idac_main, 0x19);
                        MOD_RADIO_REG_20693(pi, PA2G_IDAC1, core,
                                pa2g_idac_cas, 0x24);
                    }

                    MOD_RADIO_REG_20693(pi, SPARE_CFG10, core,
                            pa2g_incap_pmos_src_sel_gnd, 0x0);
                    MOD_RADIO_REG_20693(pi, TX_TOP_2G_OVR1_EAST, core,
                            ovr_pa2g_idac_incap_compen_main, 0x1);
                    MOD_RADIO_REG_20693(pi, PA2G_INCAP, core,
                            pa2g_idac_incap_compen_main, 0x34);
                }
            }
        } if (!PHY_IPA(pi) && CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
            /* EVM Ramp: TxBias5G & Pad5G on */
            FOREACH_CORE(pi, core) {
                RADIO_REG_LIST_START
                    MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR1, core,
                        ovr_tx5g_bias_pu, 0x1)
                    MOD_RADIO_REG_20693_ENTRY(pi, TX5G_CFG1, core,
                        tx5g_bias_pu, 0x1)
                    MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR3, core,
                        ovr_pad5g_bias_cas_pu, 0x1)
                    MOD_RADIO_REG_20693_ENTRY(pi, TX5G_CFG1, core,
                        pad5g_bias_cas_pu, 0x1)
                    MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR2, core,
                        ovr_pad5g_pu, 0x1)
                    MOD_RADIO_REG_20693_ENTRY(pi, TX5G_CFG1, core,
                        pad5g_pu, 0x1)
                RADIO_REG_LIST_EXECUTE(pi, core);
            }
        }
    }
}

static void
wlc_phy_set_noise_var_shaping_acphy(phy_info_t *pi, uint8 noise_var[][ACPHY_SPURWAR_NV_NTONES],
                                             int8 *tone_id, uint8 *core_nv)
{
}

/**
 * Whenever the transmit power is less than a certain value, lower PA power consumption can be
 * achieved by selecting lower PA linearity. The VLIN signal towards the FEM is configured to
 * either be driven by the FEM control table or by a chip internal VLIN signal.
 */
void wlc_phy_vlin_en_acphy(phy_info_t *pi)
{
    uint8 band2g_idx, core;
    uint8 stall_val;
    int16 idle_tssi[PHY_CORE_MAX];
    uint16 adj_tssi1[PHY_CORE_MAX];
    uint16 adj_tssi2[PHY_CORE_MAX], adj_tssi3[PHY_CORE_MAX];
    int16 tone_tssi1[PHY_CORE_MAX];
    int16 tone_tssi2[PHY_CORE_MAX], tone_tssi3[PHY_CORE_MAX];
    int16 a1[PHY_CORE_MAX];
    int16 b0[PHY_CORE_MAX];
    int16 b1[PHY_CORE_MAX];
    uint8 pwr1, pwr2, pwr3;
    uint8 txidx1 = 40, txidx2 = 90, txidx3;
    struct _orig_reg_vals {
        uint8 core;
        uint16 orig_OVR3;
        uint16 orig_auxpga_cfg1;
        uint16 orig_auxpga_vmid;
        uint16 orig_iqcal_cfg1;
        uint16 orig_tx5g_tssi;
        uint16 orig_pa2g_tssi;
        uint16 orig_RfctrlIntc;
        uint16 orig_RfctrlOverrideRxPus;
        uint16 orig_RfctrlCoreRxPu;
        uint16 orig_RfctrlOverrideAuxTssi;
        uint16 orig_RfctrlCoreAuxTssi1;
        } orig_reg_vals[PHY_CORE_MAX];
    uint core_count = 0;
    txgain_setting_t curr_gain1, curr_gain2, curr_gain3;
    bool init_adc_inside = FALSE;
    uint16 save_afePuCtrl, save_gpio;
    uint16 orig_use_txPwrCtrlCoefs;
    uint16 fval2g_orig, fval5g_orig, fval2g, fval5g;
    uint32 save_chipc = 0;
    uint16 save_gpioHiOutEn;
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
    txgain_setting_t curr_gain4;
    int16 tone_tssi4[PHY_CORE_MAX];
    uint16 adj_tssi4[PHY_CORE_MAX];
    int bbmultcomp;
    uint16 tempmuxTxVlinOnFemCtrl2;
    uint16 txidxval;
    uint16 txgaintemp1[3], txgaintemp1a[3];
    uint16 tempmuxTxVlinOnFemCtrl, globpusmask;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

    BCM_REFERENCE(stf_shdata);

    ASSERT(RADIOID_IS(pi->pubpi->radioid, BCM2069_ID));
    /* prevent crs trigger */
    phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, TRUE);
    band2g_idx = CHSPEC_IS2G(pi->radio_chanspec);
    if (band2g_idx)    {
        pwr3 = pi_ac->chanmgri->cfg->vlinpwr2g_from_nvram;
        }
    else {
        pwr3 = pi_ac->chanmgri->cfg->vlinpwr5g_from_nvram;
        }
    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    ACPHY_DISABLE_STALL(pi);
    /* Turn off epa/ipa and unused rxrf part to prevent energy go into air */
    orig_use_txPwrCtrlCoefs = READ_PHYREGFLD(pi, TxPwrCtrlCmd,
    use_txPwrCtrlCoefs);
    FOREACH_ACTV_CORE(pi, stf_shdata->hw_phyrxchain, core) {
        /* save phy/radio regs going to be touched */
        orig_reg_vals[core_count].orig_RfctrlIntc = READ_PHYREGCE(pi,
        RfctrlIntc, core);
        orig_reg_vals[core_count].orig_RfctrlOverrideRxPus =
            READ_PHYREGCE(pi, RfctrlOverrideRxPus, core);
        orig_reg_vals[core_count].orig_RfctrlCoreRxPu =
            READ_PHYREGCE(pi, RfctrlCoreRxPus, core);
        orig_reg_vals[core_count].orig_RfctrlOverrideAuxTssi =
            READ_PHYREGCE(pi, RfctrlOverrideAuxTssi, core);
        orig_reg_vals[core_count].orig_RfctrlCoreAuxTssi1 =
            READ_PHYREGCE(pi, RfctrlCoreAuxTssi1, core);
        orig_reg_vals[core_count].orig_OVR3 = READ_RADIO_REGC(pi,
            RF, OVR3, core);
        orig_reg_vals[core_count].orig_auxpga_cfg1 =
            READ_RADIO_REGC(pi, RF, AUXPGA_CFG1, core);
        orig_reg_vals[core_count].orig_auxpga_vmid =
            READ_RADIO_REGC(pi, RF, AUXPGA_VMID, core);
        orig_reg_vals[core_count].orig_iqcal_cfg1 =
            READ_RADIO_REGC(pi, RF, IQCAL_CFG1, core);
        orig_reg_vals[core_count].orig_tx5g_tssi = READ_RADIO_REGC(pi,
            RF, TX5G_TSSI, core);
        orig_reg_vals[core_count].orig_pa2g_tssi = READ_RADIO_REGC(pi,
            RF, PA2G_TSSI, core);
        orig_reg_vals[core_count].core = core;
        /* set tssi_range = 0   (it suppose to bypass 10dB attenuation before pdet) */
        MOD_PHYREGCE(pi, RfctrlOverrideAuxTssi,  core, tssi_range, 1);
        MOD_PHYREGCE(pi, RfctrlCoreAuxTssi1,     core, tssi_range, 0);
        /* turn off lna and other unsed rxrf components */
        WRITE_PHYREGCE(pi, RfctrlOverrideRxPus, core, 0x7CE0);
        WRITE_PHYREGCE(pi, RfctrlCoreRxPus,     core, 0x0);
        ++core_count;
        }
    ACPHY_ENABLE_STALL(pi, stall_val);
    /* tssi loopback setup */
    phy_ac_tssi_loopback_path_setup(pi, LOOPBACK_FOR_TSSICAL);

    if (!init_adc_inside) {
        wlc_phy_init_adc_read(pi, &save_afePuCtrl, &save_gpio,
            &save_chipc, &fval2g_orig, &fval5g_orig,
            &fval2g, &fval5g, &stall_val, &save_gpioHiOutEn);
        }
    wlc_phy_get_paparams_for_band_acphy(pi, a1, b0, b1);
    FOREACH_ACTV_CORE(pi, stf_shdata->phyrxchain, core) {
        if (!init_adc_inside)
            wlc_phy_gpiosel_acphy(pi, 16+core, 1);
        /* Measure the Idle TSSI */
        wlc_phy_poll_samps_WAR_acphy(pi, idle_tssi, TRUE, TRUE, NULL,
        FALSE, init_adc_inside, core, 1);
        MOD_PHYREG(pi, TxPwrCtrlCmd, use_txPwrCtrlCoefs, 0);
        wlc_phy_get_txgain_settings_by_index_acphy(pi, &curr_gain1, txidx1);
        wlc_phy_poll_samps_WAR_acphy(pi, tone_tssi1, TRUE, FALSE,
            &curr_gain1, FALSE, init_adc_inside, core, 1);
        adj_tssi1[core] = 1024+idle_tssi[core]-tone_tssi1[core];
        adj_tssi1[core] = adj_tssi1[core] >> 3;
        pwr1 = wlc_phy_tssi2dbm_acphy(pi, adj_tssi1[core], a1[core], b0[core], b1[core]);
        wlc_phy_get_txgain_settings_by_index_acphy(pi, &curr_gain2, txidx2);
        wlc_phy_poll_samps_WAR_acphy(pi, tone_tssi2, TRUE, FALSE,
            &curr_gain2, FALSE, init_adc_inside, core, 1);
        adj_tssi2[core] = 1024+idle_tssi[core]-tone_tssi2[core];
        adj_tssi2[core] = adj_tssi2[core] >> 3;
        pwr2 = wlc_phy_tssi2dbm_acphy(pi, adj_tssi2[core], a1[core], b0[core], b1[core]);
        if (pwr2 != pwr1) {
            txidx3 = txidx1+(4*pwr3-pwr1) *(txidx2-txidx1)/(pwr2-pwr1);
        } else {
            txidx3 = txidx1;
        }
        wlc_phy_get_txgain_settings_by_index_acphy(pi, &curr_gain3, txidx3);
        wlc_phy_poll_samps_WAR_acphy(pi, tone_tssi3, TRUE, FALSE,
            &curr_gain3, FALSE, init_adc_inside, core, 1);
        adj_tssi3[core] = 1024+idle_tssi[core]-tone_tssi3[core];
        adj_tssi3[core] = adj_tssi3[core] >> 3;
        if (band2g_idx)    {
            globpusmask = 1<<(pi_ac->chanmgri->data->vlinmask2g_from_nvram);
        } else {
            globpusmask = 1<<(pi_ac->chanmgri->data->vlinmask5g_from_nvram);
        }
        tempmuxTxVlinOnFemCtrl = READ_PHYREGFLD(pi, RfctrlCoreGlobalPus,
            muxTxVlinOnFemCtrl);
        tempmuxTxVlinOnFemCtrl2 = (tempmuxTxVlinOnFemCtrl | globpusmask);
        MOD_PHYREG(pi, RfctrlCoreGlobalPus, muxTxVlinOnFemCtrl, tempmuxTxVlinOnFemCtrl2);
        wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_GAINCTRLBBMULTLUTS,
            1, txidx3, 48, &txgaintemp1);
        txgaintemp1a[0] = (txgaintemp1[0]|0x8000);
        txgaintemp1a[1] = txgaintemp1[1];
        txgaintemp1a[2] = txgaintemp1[2];
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_GAINCTRLBBMULTLUTS, 1,
            txidx3, 48, txgaintemp1a);
        wlc_phy_get_txgain_settings_by_index_acphy(pi, &curr_gain4, txidx3);
        wlc_phy_poll_samps_WAR_acphy(pi, tone_tssi4, TRUE, FALSE,
            &curr_gain4, FALSE, init_adc_inside, core, 1);
        adj_tssi4[core] = 1024+idle_tssi[core]-tone_tssi4[core];
        adj_tssi4[core] = adj_tssi4[core] >> 3;
        bbmultcomp = (int)((tone_tssi3[core]-tone_tssi4[core])/6);
        pi_ac->chanmgri->data->vlin_txidx = txidx3;
        pi_ac->chanmgri->data->bbmult_comp = bbmultcomp;
        for (txidxval = txidx3; txidxval < 128; txidxval++) {
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_GAINCTRLBBMULTLUTS, 1,
                txidxval, 48, &txgaintemp1);
            txgaintemp1a[0] = (txgaintemp1[0]|0x8000)+bbmultcomp;
            txgaintemp1a[1] = txgaintemp1[1];
            txgaintemp1a[2] = txgaintemp1[2];
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_GAINCTRLBBMULTLUTS, 1,
                txidxval, 48, txgaintemp1a);
            }
        if (!init_adc_inside)
            wlc_phy_restore_after_adc_read(pi, &save_afePuCtrl, &save_gpio,
            &save_chipc, &fval2g_orig, &fval5g_orig,
            &fval2g, &fval5g, &stall_val, &save_gpioHiOutEn);
        /* restore phy/radio regs */
        while (core_count > 0) {
            --core_count;
            phy_utils_write_radioreg(pi, RF_2069_OVR3(orig_reg_vals[core_count].core),
                orig_reg_vals[core_count].orig_OVR3);
            phy_utils_write_radioreg(pi,
                RF_2069_AUXPGA_CFG1(orig_reg_vals[core_count].core),
                orig_reg_vals[core_count].orig_auxpga_cfg1);
            phy_utils_write_radioreg(pi,
                RF_2069_AUXPGA_VMID(orig_reg_vals[core_count].core),
                orig_reg_vals[core_count].orig_auxpga_vmid);
            phy_utils_write_radioreg(pi,
                RF_2069_IQCAL_CFG1(orig_reg_vals[core_count].core),
                orig_reg_vals[core_count].orig_iqcal_cfg1);
            phy_utils_write_radioreg(pi,
                RF_2069_TX5G_TSSI(orig_reg_vals[core_count].core),
                orig_reg_vals[core_count].orig_tx5g_tssi);
            phy_utils_write_radioreg(pi,
                RF_2069_PA2G_TSSI(orig_reg_vals[core_count].core),
                orig_reg_vals[core_count].orig_pa2g_tssi);
            WRITE_PHYREGCE(pi, RfctrlIntc, orig_reg_vals[core_count].core,
                orig_reg_vals[core_count].orig_RfctrlIntc);
            WRITE_PHYREGCE(pi, RfctrlOverrideRxPus,
                orig_reg_vals[core_count].core,
                orig_reg_vals[core_count].orig_RfctrlOverrideRxPus);
            WRITE_PHYREGCE(pi, RfctrlCoreRxPus, orig_reg_vals[core_count].core,
                orig_reg_vals[core_count].orig_RfctrlCoreRxPu);
            WRITE_PHYREGCE(pi, RfctrlOverrideAuxTssi,
                orig_reg_vals[core_count].core,
                orig_reg_vals[core_count].orig_RfctrlOverrideAuxTssi);
            WRITE_PHYREGCE(pi, RfctrlCoreAuxTssi1,
                orig_reg_vals[core_count].core,
                orig_reg_vals[core_count].orig_RfctrlCoreAuxTssi1);
            }
        MOD_PHYREG(pi, TxPwrCtrlCmd, use_txPwrCtrlCoefs, orig_use_txPwrCtrlCoefs);
        /* prevent crs trigger */
        phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, FALSE);
        PHY_TRACE(("======= IQLOCAL PreCalGainControl : END =======\n"));
        }
}

/* customize papr shaping filters */
static void phy_ac_chanmgr_papr_iir_filt_reprog(phy_info_t *pi)
{
    uint16 k;

    // 20_in_20: [b, a] = cheby1(4, 1.7, 5/Fs*2); norm = 1.02
    // filt Fc is smaller, will be fixed in C0
    //
    // 20_40 and 20_80 are copied from 20_20 and 20_40
    // of the 4x filt designs, respectively,
    // [b, a] = cheby1(4, 1, 7/Fs*2);

    uint16 ppr_iir_phyreg_vals_rev32[][2] =
    {{ACPHY_papr_iir_20_20_group_dly(pi->pubpi.phy_rev), 4},
     {ACPHY_papr_iir_20_20_b10(pi->pubpi.phy_rev), 127},
     {ACPHY_papr_iir_20_20_b11(pi->pubpi.phy_rev), 254},
     {ACPHY_papr_iir_20_20_b12(pi->pubpi.phy_rev), 127},
     {ACPHY_papr_iir_20_20_a11(pi->pubpi.phy_rev), 341},
     {ACPHY_papr_iir_20_20_a12(pi->pubpi.phy_rev), 109},
     {ACPHY_papr_iir_20_20_b20(pi->pubpi.phy_rev), 126},
     {ACPHY_papr_iir_20_20_b21(pi->pubpi.phy_rev), 252},
     {ACPHY_papr_iir_20_20_b22(pi->pubpi.phy_rev), 126},
     {ACPHY_papr_iir_20_20_a21(pi->pubpi.phy_rev), 318},
     {ACPHY_papr_iir_20_20_a22(pi->pubpi.phy_rev), 82},
     {ACPHY_papr_iir_20_40_group_dly(pi->pubpi.phy_rev), 6},
     {ACPHY_papr_iir_20_40_b10(pi->pubpi.phy_rev), 66},
     {ACPHY_papr_iir_20_40_b11(pi->pubpi.phy_rev), 131},
     {ACPHY_papr_iir_20_40_b12(pi->pubpi.phy_rev), 66},
     {ACPHY_papr_iir_20_40_a11(pi->pubpi.phy_rev), 308},
     {ACPHY_papr_iir_20_40_a12(pi->pubpi.phy_rev), 111},
     {ACPHY_papr_iir_20_40_b20(pi->pubpi.phy_rev), 74},
     {ACPHY_papr_iir_20_40_b21(pi->pubpi.phy_rev), 149},
     {ACPHY_papr_iir_20_40_b22(pi->pubpi.phy_rev), 74},
     {ACPHY_papr_iir_20_40_a21(pi->pubpi.phy_rev), 306},
     {ACPHY_papr_iir_20_40_a22(pi->pubpi.phy_rev), 88},
     {ACPHY_papr_iir_20_80_group_dly(pi->pubpi.phy_rev), 6},
     {ACPHY_papr_iir_20_80_b10(pi->pubpi.phy_rev), 17},
     {ACPHY_papr_iir_20_80_b11(pi->pubpi.phy_rev), 35},
     {ACPHY_papr_iir_20_80_b12(pi->pubpi.phy_rev), 17},
     {ACPHY_papr_iir_20_80_a11(pi->pubpi.phy_rev), 274},
     {ACPHY_papr_iir_20_80_a12(pi->pubpi.phy_rev), 119},
     {ACPHY_papr_iir_20_80_b20(pi->pubpi.phy_rev), 20},
     {ACPHY_papr_iir_20_80_b21(pi->pubpi.phy_rev), 40},
     {ACPHY_papr_iir_20_80_b22(pi->pubpi.phy_rev), 20},
     {ACPHY_papr_iir_20_80_a21(pi->pubpi.phy_rev), 280},
     {ACPHY_papr_iir_20_80_a22(pi->pubpi.phy_rev), 106}};

    /* 20in20: cheby1(4,0.5,8/40*2) */
    uint16 ppr_iir_phyreg_vals_rev33[][2] =
    {{ACPHY_papr_iir_20_20_group_dly(pi->pubpi.phy_rev), 6},
     {ACPHY_papr_iir_20_20_b10(pi->pubpi.phy_rev), 299},
     {ACPHY_papr_iir_20_20_b11(pi->pubpi.phy_rev), 598},
     {ACPHY_papr_iir_20_20_b12(pi->pubpi.phy_rev), 299},
     {ACPHY_papr_iir_20_20_a11(pi->pubpi.phy_rev), 450},
     {ACPHY_papr_iir_20_20_a12(pi->pubpi.phy_rev), 92},
     {ACPHY_papr_iir_20_20_b20(pi->pubpi.phy_rev), 424},
     {ACPHY_papr_iir_20_20_b21(pi->pubpi.phy_rev), 847},
     {ACPHY_papr_iir_20_20_b22(pi->pubpi.phy_rev), 424},
     {ACPHY_papr_iir_20_20_a21(pi->pubpi.phy_rev), 397},
     {ACPHY_papr_iir_20_20_a22(pi->pubpi.phy_rev), 41},
     {ACPHY_papr_iir_20_40_group_dly(pi->pubpi.phy_rev), 6},
     {ACPHY_papr_iir_20_40_b10(pi->pubpi.phy_rev), 66},
     {ACPHY_papr_iir_20_40_b11(pi->pubpi.phy_rev), 131},
     {ACPHY_papr_iir_20_40_b12(pi->pubpi.phy_rev), 66},
     {ACPHY_papr_iir_20_40_a11(pi->pubpi.phy_rev), 308},
     {ACPHY_papr_iir_20_40_a12(pi->pubpi.phy_rev), 111},
     {ACPHY_papr_iir_20_40_b20(pi->pubpi.phy_rev), 74},
     {ACPHY_papr_iir_20_40_b21(pi->pubpi.phy_rev), 149},
     {ACPHY_papr_iir_20_40_b22(pi->pubpi.phy_rev), 74},
     {ACPHY_papr_iir_20_40_a21(pi->pubpi.phy_rev), 306},
     {ACPHY_papr_iir_20_40_a22(pi->pubpi.phy_rev), 88},
     {ACPHY_papr_iir_20_80_group_dly(pi->pubpi.phy_rev), 6},
     {ACPHY_papr_iir_20_80_b10(pi->pubpi.phy_rev), 17},
     {ACPHY_papr_iir_20_80_b11(pi->pubpi.phy_rev), 35},
     {ACPHY_papr_iir_20_80_b12(pi->pubpi.phy_rev), 17},
     {ACPHY_papr_iir_20_80_a11(pi->pubpi.phy_rev), 274},
     {ACPHY_papr_iir_20_80_a12(pi->pubpi.phy_rev), 119},
     {ACPHY_papr_iir_20_80_b20(pi->pubpi.phy_rev), 20},
     {ACPHY_papr_iir_20_80_b21(pi->pubpi.phy_rev), 40},
     {ACPHY_papr_iir_20_80_b22(pi->pubpi.phy_rev), 20},
     {ACPHY_papr_iir_20_80_a21(pi->pubpi.phy_rev), 280},
     {ACPHY_papr_iir_20_80_a22(pi->pubpi.phy_rev), 106}};

    uint16 ppr_iir_phyreg_vals_rev129[][2] =
    {{ACPHY_papr_iir_20_20_group_dly(pi->pubpi.phy_rev), 6},
     {ACPHY_papr_iir_20_20_b10(pi->pubpi.phy_rev), 66},
     {ACPHY_papr_iir_20_20_b11(pi->pubpi.phy_rev), 131},
     {ACPHY_papr_iir_20_20_b12(pi->pubpi.phy_rev), 66},
     {ACPHY_papr_iir_20_20_a11(pi->pubpi.phy_rev), -204},
     {ACPHY_papr_iir_20_20_a12(pi->pubpi.phy_rev), 111},
     {ACPHY_papr_iir_20_20_b20(pi->pubpi.phy_rev), 74},
     {ACPHY_papr_iir_20_20_b21(pi->pubpi.phy_rev), 149},
     {ACPHY_papr_iir_20_20_b22(pi->pubpi.phy_rev), 74},
     {ACPHY_papr_iir_20_20_a21(pi->pubpi.phy_rev), -206},
     {ACPHY_papr_iir_20_20_a22(pi->pubpi.phy_rev), 88},
     {ACPHY_papr_iir_20_40_group_dly(pi->pubpi.phy_rev), 6},
     {ACPHY_papr_iir_20_40_b10(pi->pubpi.phy_rev), 17},
     {ACPHY_papr_iir_20_40_b11(pi->pubpi.phy_rev), 35},
     {ACPHY_papr_iir_20_40_b12(pi->pubpi.phy_rev), 17},
     {ACPHY_papr_iir_20_40_a11(pi->pubpi.phy_rev), -238},
     {ACPHY_papr_iir_20_40_a12(pi->pubpi.phy_rev), 119},
     {ACPHY_papr_iir_20_40_b20(pi->pubpi.phy_rev), 20},
     {ACPHY_papr_iir_20_40_b21(pi->pubpi.phy_rev), 40},
     {ACPHY_papr_iir_20_40_b22(pi->pubpi.phy_rev), 20},
     {ACPHY_papr_iir_20_40_a21(pi->pubpi.phy_rev), -232},
     {ACPHY_papr_iir_20_40_a22(pi->pubpi.phy_rev), 106},
     {ACPHY_papr_iir_20_80_group_dly(pi->pubpi.phy_rev), 6},
     {ACPHY_papr_iir_20_80_b10(pi->pubpi.phy_rev), 4},
     {ACPHY_papr_iir_20_80_b11(pi->pubpi.phy_rev), 9},
     {ACPHY_papr_iir_20_80_b12(pi->pubpi.phy_rev), 4},
     {ACPHY_papr_iir_20_80_a11(pi->pubpi.phy_rev), -249},
     {ACPHY_papr_iir_20_80_a12(pi->pubpi.phy_rev), 123},
     {ACPHY_papr_iir_20_80_b20(pi->pubpi.phy_rev), 5},
     {ACPHY_papr_iir_20_80_b21(pi->pubpi.phy_rev), 10},
     {ACPHY_papr_iir_20_80_b22(pi->pubpi.phy_rev), 5},
     {ACPHY_papr_iir_20_80_a21(pi->pubpi.phy_rev), -244},
     {ACPHY_papr_iir_20_80_a22(pi->pubpi.phy_rev), 117}};

    if (!(ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)))) {
        return;
    }

    if (ACMAJORREV_32(pi->pubpi->phy_rev)) {
        for (k = 0; k < ARRAYSIZE(ppr_iir_phyreg_vals_rev32); k++)  {
            phy_utils_write_phyreg(pi, ppr_iir_phyreg_vals_rev32[k][0],
                    ppr_iir_phyreg_vals_rev32[k][1]);
        }
    } else if (ACMAJORREV_33(pi->pubpi->phy_rev) ||    ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
        for (k = 0; k < ARRAYSIZE(ppr_iir_phyreg_vals_rev33); k++)  {
            phy_utils_write_phyreg(pi, ppr_iir_phyreg_vals_rev33[k][0],
                    ppr_iir_phyreg_vals_rev33[k][1]);
        }
    } else if (ACMAJORREV_47_129(pi->pubpi->phy_rev)) {
        for (k = 0; k < ARRAYSIZE(ppr_iir_phyreg_vals_rev129); k++)  {
            phy_utils_write_phyreg(pi, ppr_iir_phyreg_vals_rev129[k][0],
                    ppr_iir_phyreg_vals_rev129[k][1]);
        }
    }
}

/* PAPRR Functions */
static void chanspec_setup_papr(phy_info_t *pi,
int8 papr_final_clipping, int8 papr_final_scaling)
{
    uint16 lowMcsGamma = 600, highMcsGamma = 1100, vhtMcsGamma_c8_c9 = 1100;
    uint16 highMcsGamma_c8_c11 = 1200;
    uint16 vhtMcsGamma_c10_c11 = 1100;
    uint32 gammaOffset_6878[4] = {0, 0, 0, 0};
    uint8 gainOffset[4] = {0, 0, 0, 0};
    uint32 gain = 128, gamma;
    uint32 gamma_6878[28];
    uint8 i, j, core, gain_6878[28];
    int16 *paprrmcsgamma = NULL;
    uint8 *paprrmcsgain = NULL;
    bool enable = chanspec_papr_enable(pi);
    int16 papr_gamma_iovar = 4095;

    if (pi->u.pi_acphy->chanmgri->papr_en_iovar != -1) {
        if (ACMAJORREV_GE129(pi->pubpi->phy_rev) ||
            ACMAJORREV_47(pi->pubpi->phy_rev))
            enable = pi->u.pi_acphy->chanmgri->papr_en_iovar;
    }
    if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
        if (pi->u.pi_acphy->chanmgri->papr_gamma_iovar != -1) {
            papr_gamma_iovar = pi->u.pi_acphy->chanmgri->papr_gamma_iovar;
            lowMcsGamma = papr_gamma_iovar;
            highMcsGamma = papr_gamma_iovar;
            highMcsGamma_c8_c11 = papr_gamma_iovar;
        } else {
            /* 6710: Used for 2G only. PAPR disabled for 5G. */
            lowMcsGamma = CHSPEC_IS20(pi->radio_chanspec) ? 620 : 640;
            highMcsGamma = 4095;
            highMcsGamma_c8_c11 = 4095;
        }
    } else if (ACMAJORREV_47_130(pi->pubpi->phy_rev)) {
        if (pi->u.pi_acphy->chanmgri->papr_gamma_iovar != -1) {
            papr_gamma_iovar = pi->u.pi_acphy->chanmgri->papr_gamma_iovar;
            lowMcsGamma = papr_gamma_iovar;
            highMcsGamma = papr_gamma_iovar;
            highMcsGamma_c8_c11 = papr_gamma_iovar;
        } else {
            /* 43684: Used for dup160 only. */
            lowMcsGamma = 1200;
            highMcsGamma = 1200;
            highMcsGamma_c8_c11 = 4095;
        }
    } else {
        if (CHSPEC_IS2G(pi->radio_chanspec)) {
            highMcsGamma = 950;
            vhtMcsGamma_c8_c9 = 950;
            lowMcsGamma = 600;
        } else {
            highMcsGamma = 1100;
            vhtMcsGamma_c8_c9 = 1100;
            vhtMcsGamma_c10_c11 = 1100;
            if ((ACMAJORREV_32(pi->pubpi->phy_rev) ||
                    ACMAJORREV_33(pi->pubpi->phy_rev) ||
                    (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                    !ACMAJORREV_128(pi->pubpi->phy_rev))) &&
                    (CHSPEC_IS20(pi->radio_chanspec)))
                lowMcsGamma = 700;
            else
                lowMcsGamma = 600;
        }
    }

    if (!PHY_IPA(pi) && ACMAJORREV_2(pi->pubpi->phy_rev)) {
        vhtMcsGamma_c8_c9 = 8191;
    }

    if (enable) {
        MOD_PHYREG_4(pi, papr_ctrl, papr_blk_en, enable,
            papr_final_clipping_en, papr_final_clipping,
            papr_final_scaling_en, papr_final_scaling,
            papr_override_enable, 0);

        if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
            /* RU26 TxEVM becomes bad with PAPR enabled */
            MOD_PHYREG(pi, papr_ctrl, papr_disable_he_mu_ppdu, 1);
            MOD_PHYREG(pi, papr_ctrl, papr_disable_he_tb_ppdu, 1);
        }

        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            FOREACH_CORE(pi, core) {
                MOD_PHYREGCE(pi, papr_gain_index_p, core, papr_enable, 1);
            }
            phy_ac_chanmgr_papr_iir_filt_reprog(pi);
        }

        if (ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_47_130(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            /* Program enable/gainidx */
            gamma = 0x0; /* not used fields */
            for (i = 1; i <= 3; i++) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, i,
                        32, &gamma);
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, 0x40 + i,
                        32, &gamma);
            }
            gamma = 0x80;
            for (i = 4; i <= 33; i++) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, i,
                        32, &gamma);
            }

            /* Program gamma1/gamma */
            for (i = 0x44; i <= 0x40 + 31; i++) {
                if ((i >= 0x44 && i <= 0x47) || (i >= 0x4C && i <= 0x4E) ||
                        (i >= 0x54 && i <= 0x56)) {
                    /* BPSK & QPSK */
                    gamma = (lowMcsGamma << 13) | lowMcsGamma;
                } else {
                    gamma = (highMcsGamma << 13) | highMcsGamma;
                    if (i >= 0x5C && i <= 0x5F) {
                        gamma = (highMcsGamma_c8_c11 << 13) |
                            highMcsGamma_c8_c11;
                    }
                }
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, i,
                        32, &gamma);
            }

            /* program GammaOffset fields */
            for (i = 0; i <= 2; i++) {
                uint16 gammaOffset[3] = {0, 100, 150};
                if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
                    /* 40M uses same gamma as 20in40 subband */
                    gammaOffset[1] = 0;
                    gammaOffset[2] = 0;
                }

                gamma = (gammaOffset[i] << 13) | gammaOffset[i];
                j = ((i == 2) ? 0 : (32 + i));
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, (0x40+j),
                        32, &gamma);
                gamma = 0x80;
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, j,
                        32, &gamma);
            }
        } else if (ACMAJORREV_47_130(pi->pubpi->phy_rev)) {
            gamma = 0x0; /* not used fields */
            for (i = 1; i <= 3; i++) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, i,
                        32, &gamma);
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, 0x40 + i,
                        32, &gamma);
            }
            for (i = 4; i <= 31; i++) {
                if (i <= 8) {
                /* Program enable/gainidx for 11ag 6~24mbps */
                    gamma = 0x80;
                } else {
                    gamma = 0x0;
                }
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, i,
                        32, &gamma);
            }

            /* Program gamma1/gamma for 11ag 6~24mbps */
            for (i = 0x44; i <= 0x40 + 31; i++) {
                /* 11ag all rate */
                if (i <= (0x40 + 8)) {
                    gamma = (lowMcsGamma << 13) | lowMcsGamma;
                } else {
                    gamma = (highMcsGamma_c8_c11 << 13) | highMcsGamma_c8_c11;
                }
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, i,
                        32, &gamma);
            }

            /* program GammaOffset fields */
            for (i = 0; i <= 3; i++) {
                gamma = 0;
                j = 32 + i;
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, (0x40+j),
                        32, &gamma);
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, j,
                        32, &gamma);
            }
        } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
            /* gain/gamma entries for different rates/bw */
            if (CHSPEC_IS2G(pi->radio_chanspec)) {
                if (!((CHSPEC_CHANNEL(pi->radio_chanspec) == 13) ||
                    (CHSPEC_CHANNEL(pi->radio_chanspec) == 1)) ||
                    (pi->fdss_bandedge_2g_en == 0)) {
                    paprrmcsgamma = pi->paprrmcsgamma2g;
                    paprrmcsgain = pi->paprrmcsgain2g;
                } else if ((CHSPEC_CHANNEL(pi->radio_chanspec) == 13)) {
                    paprrmcsgamma = pi->paprrmcsgamma2g_ch13;
                    paprrmcsgain = pi->paprrmcsgain2g_ch13;
                } else {
                    paprrmcsgamma = pi->paprrmcsgamma2g_ch1;
                    paprrmcsgain = pi->paprrmcsgain2g_ch1;
                }
            } else {
                if (CHSPEC_IS20(pi->radio_chanspec)) {
                    paprrmcsgamma = pi->paprrmcsgamma5g20;
                    paprrmcsgain = pi->paprrmcsgain5g20;
                } else if (CHSPEC_IS40(pi->radio_chanspec)) {
                    paprrmcsgamma = pi->paprrmcsgamma5g40;
                    paprrmcsgain = pi->paprrmcsgain5g40;
                } else {
                    paprrmcsgamma = pi->paprrmcsgamma5g80;
                    paprrmcsgain = pi->paprrmcsgain5g80;
                }
            }
            /* set gamma to default values if not explicitly specified */
            if (paprrmcsgamma[8] == -1) {
                paprrmcsgamma[8] = vhtMcsGamma_c8_c9;
            }
            if (paprrmcsgamma[9] == -1) {
                paprrmcsgamma[9] = vhtMcsGamma_c8_c9;
            }
            if (paprrmcsgamma[10] == -1) {
                 paprrmcsgamma[10] = vhtMcsGamma_c10_c11;
            }
            if (paprrmcsgamma[11] == -1) {
                 paprrmcsgamma[11] = vhtMcsGamma_c10_c11;
            }

            /* Hard code dot11ac vhtmcs8_9, vhtmcs10_11 gain/gamma values */
            gain_6878[24] = paprrmcsgain[8];
            gain_6878[25] = paprrmcsgain[9];
            gain_6878[26] = paprrmcsgain[10];
            gain_6878[27] = paprrmcsgain[11];

            gamma_6878[24] = (paprrmcsgamma[8] << 13) | paprrmcsgamma[8];
            gamma_6878[25] = (paprrmcsgamma[9] << 13) | paprrmcsgamma[9];
            gamma_6878[26] = (paprrmcsgamma[10] << 13) | paprrmcsgamma[10];
            gamma_6878[27] = (paprrmcsgamma[11] << 13) | paprrmcsgamma[11];

            for (j = 0; j < 8; j++) {
                if (paprrmcsgamma[j] == -1) {
                    if (j < 5) {
                        paprrmcsgamma[j] = lowMcsGamma;
                    } else {
                        paprrmcsgamma[j] = highMcsGamma;
                    }
                }
                /* Set m0 --> m7 settings for 11ag rates */
                gain_6878[j] = paprrmcsgain[j];
                gamma_6878[j] = (paprrmcsgamma[j] << 13) | paprrmcsgamma[j];

                /* Replicate m0 --> m7 settings for 11n and 11ac rates */
                gain_6878[j+8] = gain_6878[j];
                gain_6878[j+16] = gain_6878[j];
                gamma_6878[j+8] = gamma_6878[j];
                gamma_6878[j+16] = gamma_6878[j];
            }
            /* Write gain values from offset 4 to offset 31 */
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 28, 4, 8, &gain_6878);
            /* Write gain offsets from offset 32 to offset 35 */
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 4, 32, 8, &gainOffset);
            /* Write gamma values from offset 68 to offset 95 */
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 28, 68, 32, &gamma_6878);
            /* Write gamma offsets from offset 96 to offset 99 */
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 4, 96, 32,
                &gammaOffset_6878);
        } else {
            for (j = 4; j <= 32; j++) {
                if (j <= 29) {
                    /* gain entries for different rates */
                    gain = 128;
                } else {
                    /* gain offsets */
                    gain = 0;
                }
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, j, 32, &gain);
            }
            for (j = 0x44; j <= 0x5D; j++) {
                /* tbl offset starting 0x40 is gamma table */
                if (j >= 0x5C && j <= 0x5D) {
                    /* vht rate mcs8 mcs 9 256 QAM */
                    gamma = (vhtMcsGamma_c8_c9 << 13) | vhtMcsGamma_c8_c9;
                } else if ((j >= 0x44 && j <= 0x47) || (j >= 0x4c && j <= 0x4e) ||
                    (j >= 0x54 && j <= 0x56)) {
                    /* All BPSK and QPSK rates */
                    gamma = (lowMcsGamma << 13) | lowMcsGamma;
                } else {
                    /* ALL 16QAM and 64QAM rates */
                    gamma = (highMcsGamma << 13) | highMcsGamma;
                }
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, j, 32, &gamma);
            }
            for (i = 0, j = 0x5E; j <= 0x60; j++, i++) {
                if (ACMAJORREV_32(pi->pubpi->phy_rev)) {
                    uint16 gammaOffset[3] = {0, 100, 150};
                    gamma = (gammaOffset[i] << 13) | gammaOffset[i];
                    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, j, 32,
                        &gamma);
                } else {
                    uint16 gammaOffset[3] = {0, 0, 0};
                    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPR, 1, j, 32,
                        &gammaOffset[i]);
                }
            }
        }
    } else {
        MOD_PHYREG(pi, papr_ctrl, papr_blk_en, enable);
    }
}

static void
wlc_phy_spurwar_nvshp_acphy(phy_info_t *pi, bool bw_chg, bool spurwar, bool nvshp)
{
    uint8 i, core;
    uint8 core_nv = 0, core_sp = 0;
    uint8 noise_var[PHY_CORE_MAX][ACPHY_SPURWAR_NV_NTONES];
    int8 tone_id[ACPHY_SPURWAR_NV_NTONES];
    phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;

    /* Initialize variables */
    for (i = 0; i < ACPHY_SPURWAR_NV_NTONES; i++) {
        tone_id[i]   = 0;
        FOREACH_CORE(pi, core)
            noise_var[core][i] = 0;
    }

    /* Table reset req or not */
    if (nvshp && !bw_chg && !spurwar)
        nvshp = FALSE;

    if (spurwar || nvshp) {
        /* Reset Table */
        wlc_phy_reset_noise_var_shaping_acphy(pi);

        /* Call nvshp */
        if (nvshp)
            wlc_phy_set_noise_var_shaping_acphy(pi, noise_var, tone_id, &core_nv);

        /* Call spurwar */
        if (spurwar)
            phy_ac_spurwar(pi_ac->rxspuri, noise_var, tone_id, &core_sp);

        /* Write table
         * If both nvshp and spurwar tries to write same tone
         * priority lies with spurwar
         */
        wlc_phy_noise_var_shaping_acphy(pi, core_nv, core_sp, tone_id, noise_var, 0);
    }
}

/* Set up rx2tx rfseq tables differently for cal vs. packets for tiny */
/* to avoid problems with AGC lock-up */
void
phy_ac_rfseq_mode_set(phy_info_t *pi, bool cal_mode)
{
    if (cal_mode) {
        if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
                16, rfseq_majrev4_rx2tx_cal_cmd);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 112, 16,
                rfseq_majrev4_rx2tx_cal_dly);
        } else if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
                16, rfseq_majrev32_rx2tx_cal_cmd);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 112, 16,
                rfseq_majrev32_rx2tx_cal_dly);
        } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
            if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x10, 16,
                    rfseq_majrev128_tx2rx_cal_cmd);
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x80, 16,
                    rfseq_majrev128_tx2rx_cal_dly);
            }
        } else {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
                16, rfseq_rx2tx_cmd);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 112, 16,
                rfseq_rx2tx_dly);
        }
    } else {
        if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00, 16,
                tiny_rfseq_rx2tx_tssi_sleep_cmd);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 112, 16,
                tiny_rfseq_rx2tx_tssi_sleep_dly);
        } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
            if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x10, 16,
                    rfseq_majrev128_tx2rx_cmd);
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x80, 16,
                    rfseq_majrev128_tx2rx_dly);
            }
        } else if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00, 16,
                rfseq_majrev32_rx2tx_cmd);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x70, 16,
                rfseq_majrev32_rx2tx_dly);
        } else {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00, 16,
                tiny_rfseq_rx2tx_cmd);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 112, 16,
                tiny_rfseq_rx2tx_dly);
        }
    }
}

static void
wlc_phy_radio20693_set_reset_table_bits(phy_info_t *pi, uint16 tbl_id, uint16 offset,
    uint16 start, uint16 end, uint16 val, uint8 tblwidth)
{
    uint16 val_shift, mask;
    uint32 data[2];

    val_shift = val << start;
    mask  = ((1 << (end + 1)) - (1 << start));
    wlc_phy_table_read_acphy(pi, tbl_id, 1, offset, tblwidth, &data);

    data[0] = ((data[0] & mask) | val_shift);
    wlc_phy_table_write_acphy(pi, tbl_id, 1, offset, tblwidth, &data);
}

#ifndef ACPHY_1X1_ONLY
static void
wlc_phy_write_tx_farrow_acphy(phy_ac_chanmgr_info_t *ci, chanspec_t chanspec)
{
    uint8    ch = CHSPEC_CHANNEL(chanspec), afe_clk_num, afe_clk_den;
    uint16    a, b, lb_b = 0;
    uint32    fcw, lb_fcw, tmp_low = 0, tmp_high = 0;
    uint32  deltaphase;
    uint16  deltaphase_lo, deltaphase_hi;
    uint16  farrow_downsamp;
    phy_info_t *pi = ci->pi;
    uint32    fc = wf_channel2mhz(ch, CHSPEC_IS2G(pi->radio_chanspec) ? WF_CHAN_FACTOR_2_4_G
                                                                   : WF_CHAN_FACTOR_5_G);

    if (phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->dac_mode == 1) {
        if (CHSPEC_BW_LE20(chanspec)) {
            if (CHSPEC_ISPHY5G6G(chanspec)) {
                if ((RADIO2069_MAJORREV(pi->pubpi->radiorev) == 2) &&
                    !(ISSIM_ENAB(pi->sh->sih)) &&
                    (((((fc == 5180) && (pi->sh->chippkg != 2)) ||
                       ((fc >= 5200) && (fc <= 5320)) ||
                       ((fc >= 5745) && (fc <= 5825))) && !PHY_IPA(pi)))) {
                    a = 10;
                } else if (((RADIOMAJORREV(pi) == 2) &&
                            ((fc == 5745) || (fc == 5765) || (fc == 5825 &&
                        !PHY_IPA(pi)))) && !(ISSIM_ENAB(pi->sh->sih))) {
                    a = 18;
                } else {
                    a = 16;
                }
            } else {
                if ((RADIO2069_MAJORREV(pi->pubpi->radiorev) == 2) &&
                    !(ISSIM_ENAB(pi->sh->sih))) {
                    phy_ac_radio_data_t *r = phy_ac_radio_get_data(ci->aci->radioi);
                    if ((r->srom_txnospurmod2g == 0) && !PHY_IPA(pi)) {
                        a = 9;
                    } else if (((fc != 2412) && (fc != 2467)) ||
                        (pi->xtalfreq == 40000000) ||
                        (ACMAJORREV_2(pi->pubpi->phy_rev) &&
                        (ACMINORREV_1(pi) ||
                        ACMINORREV_3(pi) ||
                        ACMINORREV_5(pi)) &&
                        pi->xtalfreq == 37400000 && PHY_ILNA(pi))) {
                        a = 18;
                    } else {
                        a = 16;
                    }
                } else {
                    a = 16;
                }
            }
            b = 160;
        } else if (CHSPEC_IS40(chanspec)) {
            if (CHSPEC_ISPHY5G6G(chanspec)) {
                if ((RADIO2069_MAJORREV(pi->pubpi->radiorev) == 2) &&
                       !PHY_IPA(pi) && (fc != 5190)) {
                    a = 10;
                } else if ((RADIOMAJORREV(pi) == 2) &&
                       !PHY_IPA(pi) && (fc == 5190)) {
                    a = 6;
                } else if (((RADIOMAJORREV(pi) == 2) &&
                     ((fc == 5755) || (fc == 5550 && pi->xtalfreq == 40000000) ||
                      (fc == 5310 && pi->xtalfreq == 37400000 && PHY_IPA(pi)))) &&
                    !(ISSIM_ENAB(pi->sh->sih))) {
                    a = 9;
                } else {
                    a = 8;
                }
            } else {
                if ((RADIO2069_MAJORREV(pi->pubpi->radiorev) == 2) &&
                    !(ISSIM_ENAB(pi->sh->sih))) {
                    a = 9;
                } else {
                    a = 8;
                }
            }
            b = 320;
        } else {
            a = 6;
            b = 640;
        }
    } else if (phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->dac_mode == 2) {
        a = 6;
        b = 640;
        lb_b = 320;
    } else {
        a = 8;
        b = 320;
        lb_b = 320;
    }

    if (CHSPEC_IS2G(pi->radio_chanspec)) {
        afe_clk_num = 2;
        afe_clk_den = 3;
    } else {
        afe_clk_num = 3;
        afe_clk_den = 2;
        if (fc == 5290 && ACMAJORREV_2(pi->pubpi->phy_rev) &&
            ((ACMINORREV_1(pi) && pi->sh->chippkg == 2) ||
             ACMINORREV_3(pi)) && PHY_XTAL_IS37M4(pi)) {
            afe_clk_num = 4;
            afe_clk_den = 3;
        }
    }

    math_uint64_multiple_add(&tmp_high, &tmp_low, a * afe_clk_num * b,
        1 << 23, (fc * afe_clk_den) >> 1);
    math_uint64_divide(&fcw, tmp_high, tmp_low, fc * afe_clk_den);
    wlc_phy_tx_farrow_mu_setup(pi, fcw & 0xffff, (fcw & 0xff0000) >> 16, fcw & 0xffff,
        (fcw & 0xff0000) >> 16);
    /* DAC MODE 1 lbfarrow setup in rx_farrow_acphy */
    if (phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->dac_mode != 1) {
        math_uint64_multiple_add(&tmp_high, &tmp_low, fc * afe_clk_den,
                1 << 25, 0);
        math_uint64_divide(&lb_fcw, tmp_high, tmp_low, a * afe_clk_num * lb_b);
        deltaphase = (lb_fcw - 33554431) >> 1;
        deltaphase_lo = deltaphase & 0xffff;
        deltaphase_hi = (deltaphase >> 16) & 0xff;
        farrow_downsamp = fc * afe_clk_den / (a * afe_clk_num * lb_b);
        WRITE_PHYREG(pi, lbFarrowDeltaPhase_lo, deltaphase_lo);
        WRITE_PHYREG(pi, lbFarrowDeltaPhase_hi, deltaphase_hi);
        WRITE_PHYREG(pi, lbFarrowDriftPeriod, 5120);
        MOD_PHYREG(pi, lbFarrowCtrl, lb_farrow_downsampfactor, farrow_downsamp);
    }
}
#endif /* ACPHY_1X1_ONLY */

static void
wlc_phy_write_rx_farrow_acphy(phy_ac_chanmgr_info_t *ci, chanspec_t chanspec)
{
    uint16 deltaphase_lo, deltaphase_hi;
    uint8 ch = CHSPEC_CHANNEL(chanspec), num, den, bw, M, vco_div;
    uint32 deltaphase, farrow_in_out_ratio, fcw, tmp_low = 0, tmp_high = 0;
    uint16 drift_period, farrow_ctrl;
    uint8 farrow_outsft_reg, dec_outsft_reg, farrow_outscale_reg = 1;
    phy_info_t *pi = ci->pi;
    uint32 fc = wf_channel2mhz(ch, CHSPEC_IS2G(pi->radio_chanspec) ?
            WF_CHAN_FACTOR_2_4_G : WF_CHAN_FACTOR_5_G);
    if (CHSPEC_IS80(chanspec)) {
        farrow_outsft_reg = 0;
        dec_outsft_reg = 0;
    } else {
        if ((ACMAJORREV_0(pi->pubpi->phy_rev) && ACMINORREV_1(pi)) ||
             ACMINORREV_0(pi)) {
            farrow_outsft_reg = 2;
        } else {
            farrow_outsft_reg = 0;
        }
        dec_outsft_reg = 3;
    }

    if (CHSPEC_IS2G(pi->radio_chanspec)) {
        num = 3;
        den = 2;
    } else {
        num = 2;
        den = 3;
        if (CHSPEC_IS80(chanspec) && fc == 5290 && ACMAJORREV_2(pi->pubpi->phy_rev) &&
            ((ACMINORREV_1(pi) && pi->sh->chippkg == 2) ||
            ACMINORREV_3(pi)) && PHY_XTAL_IS37M4(pi)) {
            num = 3;
            den = 4;
        }
    }

    if (CHSPEC_IS2G(pi->radio_chanspec)) {
        if ((RADIO2069_MAJORREV(pi->pubpi->radiorev) == 2) && !(ISSIM_ENAB(pi->sh->sih))) {
            if (CHSPEC_IS40(chanspec)) {
                bw = 40;
                M = 4;
                vco_div = 18;
                drift_period = 1920;
            } else {
                phy_ac_radio_data_t *r = phy_ac_radio_get_data(ci->aci->radioi);
                if ((r->srom_txnospurmod2g == 0) && !PHY_IPA(pi)) {
                    bw = 20;
                    M = 8;
                    vco_div = 9;
                    drift_period = 2880;
                } else if ((fc != 2412 && fc != 2467) ||
                    (pi->xtalfreq == 40000000) ||
                    (ACMAJORREV_2(pi->pubpi->phy_rev) &&
                    (ACMINORREV_1(pi) ||
                    ACMINORREV_3(pi) ||
                    ACMINORREV_5(pi)) &&
                    pi->xtalfreq == 37400000 && PHY_ILNA(pi))) {
                    bw = 20;
                    M = 8;
                    vco_div = 18;
                    drift_period = 5760;
                } else {
                    bw = 20;
                    M = 8;
                    vco_div = 16;
                    drift_period = 5120;
                }
            }
        } else {
            bw = 20;
            M = 8;
            vco_div = 16;
            drift_period = 5120;
        }
    } else {
        if (CHSPEC_IS80(chanspec)) {
            bw = 80;
            M = 4;
            vco_div = 6;
            drift_period = 2880;
            if (fc == 5290 && (RADIO2069_MAJORREV(pi->pubpi->radiorev) == 2) &&
                ((((RADIOMINORREV(pi) == 4) ||
                   (RADIOMINORREV(pi) == 10) ||
                   (RADIOMINORREV(pi) == 11) ||
                   (RADIOMINORREV(pi) == 13)) &&
                  (pi->sh->chippkg == 2)) ||
                 ((RADIOMINORREV(pi) == 7) ||
                 (RADIOMINORREV(pi) == 9) ||
                 (RADIOMINORREV(pi) == 8) ||
                 (RADIOMINORREV(pi) == 12))) &&
                (pi->xtalfreq == 37400000)) {
                drift_period = 2560;
            }
        } else {
            if (RADIO2069_MAJORREV(pi->pubpi->radiorev) == 2) {
                if (CHSPEC_IS20(chanspec) &&
                    (((fc == 5180) && (pi->sh->chippkg != 2)) ||
                     ((fc >= 5200) && (fc <= 5320)) ||
                     ((fc >= 5745) && (fc <= 5825))) &&
                    !PHY_IPA(pi)) {
                    bw = 20;
                    M = 8;
                    vco_div = 10;
                    drift_period = 2400;
                } else if (CHSPEC_IS40(chanspec) && !PHY_IPA(pi) && (fc != 5190)) {
                    bw = 20;
                    M = 8;
                    vco_div = 20;
                    drift_period = 4800;
                } else if (CHSPEC_IS40(chanspec) && !PHY_IPA(pi) && (fc == 5190)) {
                    bw = 20;
                    M = 8;
                    vco_div = 12;
                    drift_period = 2880;
                } else if ((((fc == 5755 || (fc == 5550 &&
                    pi->xtalfreq == 40000000) ||
                    (fc == 5310 && pi->xtalfreq == 37400000 &&
                    PHY_IPA(pi))) && (CHSPEC_IS40(chanspec))) ||
                    ((fc == 5745 || fc == 5765 ||
                    (fc == 5825 && !PHY_IPA(pi))) &&
                    (CHSPEC_IS20(chanspec)))) && !(ISSIM_ENAB(pi->sh->sih))) {
                    bw = 20;
                    M = 8;
                    vco_div = 18;
                    drift_period = 4320;
                } else {
                    bw = 20;
                    M = 8;
                    vco_div = 16;
                    drift_period = 3840;
                }
            } else {
                bw = 20;
                M = 8;
                vco_div = 16;
                drift_period = 3840;
            }
        }
    }
    math_uint64_multiple_add(&tmp_high, &tmp_low, fc * num, 1 << 25, 0);
    math_uint64_divide(&fcw, tmp_high, tmp_low, (uint32) (den * vco_div * M * bw));

    farrow_in_out_ratio = (fcw >> 25);
    deltaphase = (fcw - 33554431)>>1;
    deltaphase_lo = deltaphase & 0xffff;
    deltaphase_hi = (deltaphase >> 16) & 0xff;
    farrow_ctrl = (dec_outsft_reg & 0x3) | ((farrow_outscale_reg & 0x3) << 2) |
        ((farrow_outsft_reg & 0x7) << 4) | ((farrow_in_out_ratio & 0x3) <<7);

    WRITE_PHYREG(pi, rxFarrowDeltaPhase_lo, deltaphase_lo);
    WRITE_PHYREG(pi, rxFarrowDeltaPhase_hi, deltaphase_hi);
    WRITE_PHYREG(pi, rxFarrowDriftPeriod, drift_period);
    WRITE_PHYREG(pi, rxFarrowCtrl, farrow_ctrl);
    MOD_PHYREG_3(pi, lbFarrowCtrl, lb_farrow_outShift, farrow_outsft_reg,
        lb_decimator_output_shift, dec_outsft_reg, lb_farrow_outScale, farrow_outscale_reg);
    /* Use the same settings for the loopback Farrow */
    if (phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->dac_mode == 1) {
        WRITE_PHYREG(pi, lbFarrowDeltaPhase_lo, deltaphase_lo);
        WRITE_PHYREG(pi, lbFarrowDeltaPhase_hi, deltaphase_hi);
        WRITE_PHYREG(pi, lbFarrowDriftPeriod, drift_period);
        MOD_PHYREG(pi, lbFarrowCtrl, lb_farrow_downsampfactor, farrow_in_out_ratio);
    }
}

#ifndef WL_FDSS_DISABLED
static void
wlc_phy_fdss_init(phy_info_t *pi)
{
    uint8 core;
    uint8 nbkpts = 5;
    FOREACH_CORE(pi, core) {
        if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
            if (CHSPEC_IS2G(pi->radio_chanspec)) {
                if (pi->fdss_level_2g[core] == 6) {
                    pi->fdss_interp_en = 0;
                    nbkpts = 4;
                } else if ((pi->fdss_level_2g[core] == 7) ||
                    (pi->fdss_level_2g[core] == 8)) {
                    pi->fdss_interp_en = 0;
                    nbkpts = 6;
                } else {
                    pi->fdss_interp_en = 1;
                }
            } else {
                if ((pi->fdss_level_5g[core] == 9) ||
                    (pi->fdss_level_5g[core] == 10) ||
                    (pi->fdss_level_5g[core] == 11)) {
                    pi->fdss_interp_en = 0;
                } else {
                    pi->fdss_interp_en = 1;
                }
            }
        }

        MOD_PHYREGCEE(pi, txfdss_ctrl, core, txfdss_enable, 1);
        MOD_PHYREGCEE(pi, txfdss_ctrl, core, txfdss_interp_enable, pi->fdss_interp_en);
        MOD_PHYREGCEE(pi, txfdss_cfgtbl, core, txfdss_num_20M_tbl, 2);
        MOD_PHYREGCEE(pi, txfdss_cfgtbl, core, txfdss_num_40M_tbl, 2);
        MOD_PHYREGCEE(pi, txfdss_cfgbrkpt0_, core, txfdss_num_20M_breakpoints, nbkpts);
        MOD_PHYREGCEE(pi, txfdss_cfgbrkpt0_, core, txfdss_num_40M_breakpoints, nbkpts);
        MOD_PHYREGCEE(pi, txfdss_cfgbrkpt1_, core, txfdss_num_80M_breakpoints, nbkpts);
        MOD_PHYREGCEE(pi, txfdss_scaleadj_en_, core, txfdss_scale_adj_enable, 7);
    }
}

static void
wlc_phy_set_fdss_table(phy_info_t *pi)
{
    uint8 core;
    uint8 nbkpts = 5;
    uint8 *fdss_tbl = NULL;
    uint8 *bkpt_tbl_20 = NULL;
    uint8 *bkpt_tbl_40 = NULL;
    uint8 *bkpt_tbl_80 = NULL;
    uint8 val = 0;
    uint8 mcstable[71] = {16, 16, 16, 16, 17, 17, 17, 17,
        16, 16, 16, 17, 17, 17, 17, 17,
        16, 16, 16, 17, 17, 17, 17, 17, 17, 17,
        16, 16, 16, 16, 17, 17, 17, 17,
        16, 16, 16, 17, 17, 17, 17, 17,
        16, 16, 16, 17, 17, 17, 17, 17, 17, 17,
        17,
        16, 16, 16, 17, 17, 17, 17, 17,
        16, 16, 16, 17, 17, 17, 17, 17, 17, 17,
        };
    uint8 mcstable_majorrev4[71] = {16, 16, 16, 0, 0, 0, 0, 0,
        16, 16, 16, 0, 0, 0, 0, 0,
        16, 16, 16, 0, 0, 0, 0, 0, 0, 0,
        16, 16, 16, 0, 0, 0, 0, 0,
        16, 16, 16, 0, 0, 0, 0, 0,
        16, 16, 16, 0, 0, 0, 0, 0, 0, 0,
        0,
        16, 16, 16, 0, 0, 0, 0, 0,
        16, 16, 16, 0, 0, 0, 0, 0, 0, 0,
        };
    uint8 mcstable_majorrev4_53574[71] = {16, 16, 16, 16, 16, 16, 0, 0,
        16, 16, 16, 16, 16, 0, 0, 0,
        16, 16, 16, 16, 16, 0, 0, 0, 0, 0,
        16, 16, 16, 16, 16, 16, 0, 0,
        16, 16, 16, 16, 16, 0, 0, 0,
        16, 16, 16, 16, 16, 0, 0, 0, 0, 0,
        16,
        16, 16, 16, 16, 16, 16, 0, 0,
        16, 16, 16, 16, 16, 0, 0, 0, 0, 0,
        };

    uint8 i, fdss_level[2];
    uint8 breakpoint_list_20[5] = {0, 3, 17, 48, 62};
    uint8 breakpoint_list_40[5] = {0, 6, 34, 96, 124};
    uint8 breakpoint_list_80[5] = {0, 12, 68, 192, 248};
    uint8 breakpoint_list_interp_20[2] = {47, 61};
    uint8 breakpoint_list_interp_40[2] = {97, 123};
    uint8 breakpoint_list_interp_80[2] = {191, 247};

    /* introducing new fdss table for 4359 */
    uint8 fdss_scale_level[5][5] = {{128, 128, 128, 128, 128},
        {128, 128, 128, 128, 128},
        {164, 146, 104, 146, 164}, /* Mild, meets older +1, -3 dB flatness limits */
        {180, 128, 72, 128, 180}, /* Extreme, meets older +3, -5 dB flatness limits */
        {170, 146, 85, 146, 170}  /* intermediate fdss coeff for 4359 */        };
    int16 fdss_scale_level_interp_20[5][5] = {{0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {-683, -338, 0, 338, 683},
        {-2219, -512, 0, 512, 2219},
        {-683, -338, 0, 338, 683}};
    int16 fdss_scale_level_interp_40[5][5] = {{0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {-341, -169, 0, 169, 341},
        {-1109, -256, 0, 256, 1109},
        {-341, -169, 0, 169, 341}};
    int16 fdss_scale_level_interp_80[5][5] = {{0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {-171, -86, 0, 86, 171},
        {-555, -128, 0, 128, 555},
        {-171, -86, 0, 86, 171}};
    uint8 fdss_scale_level_adjust_20[5] = {128, 128, 132, 128, 128};
    uint8 fdss_scale_level_adjust_40[5] = {128, 128, 132, 128, 128};
    uint8 fdss_scale_level_adjust_80[5] = {128, 128, 134, 128, 128};
    uint8 fdss_scale_level_adjust_interp_20[5] = {128, 128, 132, 128, 128};
    uint8 fdss_scale_level_adjust_interp_40[5] = {128, 128, 131, 128, 128};
    uint8 fdss_scale_level_adjust_interp_80[5] = {128, 128, 134, 128, 128};

    if (CHSPEC_IS2G(pi->radio_chanspec)) {
        fdss_level[0] = pi->fdss_level_2g[0];
        if (pi->fdss_level_2g[1] ==  -1) {
            fdss_level[1] = 0;
        } else {
            fdss_level[1] = pi->fdss_level_2g[1];
        }
    } else {
        fdss_level[0] = pi->fdss_level_5g[0];
        if (pi->fdss_level_5g[1] ==  -1) {
            fdss_level[1] = 0;
        } else {
            fdss_level[1] = pi->fdss_level_5g[1];
        }
    }
    FOREACH_CORE(pi, core) {
        if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_FDSS_MCSINFOTBL(core),
                71, 0, 8, (ROUTER_4349(pi) ?
                mcstable_majorrev4_53574 : mcstable_majorrev4));
        } else {
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_MCSINFOTBL(core), 71, 0, 8, mcstable);
        }
        bkpt_tbl_20 = breakpoint_list_20;
        bkpt_tbl_40 = breakpoint_list_40;
        bkpt_tbl_80 = breakpoint_list_80;

    /* Populate breakpoint and scale tables with the scale values for each BW */
        for (i = 0; i < 2; i++) {
            fdss_tbl = fdss_scale_level[fdss_level[i]];
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_BREAKPOINTSTBL(core), nbkpts, nbkpts*i, 8,
                bkpt_tbl_20);
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_SCALEFACTORSTBL(core), nbkpts, nbkpts*i, 8,
                fdss_tbl);
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_SCALEADJUSTFACTORSTBL(core), 1, i, 8,
                &fdss_scale_level_adjust_20[fdss_level[i]]);

            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_BREAKPOINTSTBL(core), nbkpts,
                (2 * nbkpts) + nbkpts*i, 8,
                bkpt_tbl_40);
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_SCALEFACTORSTBL(core), nbkpts,
                (2 * nbkpts) + nbkpts*i, 8,
                fdss_tbl);
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_SCALEADJUSTFACTORSTBL(core), 1, i+2, 8,
                &fdss_scale_level_adjust_40[fdss_level[i]]);

            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_BREAKPOINTSTBL(core), nbkpts,
                (4 * nbkpts) + nbkpts*i, 8,
                bkpt_tbl_80);
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_SCALEFACTORSTBL(core), nbkpts,
                (4 * nbkpts) + nbkpts*i, 8,
                fdss_tbl);
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_SCALEADJUSTFACTORSTBL(core), 1, i+4, 8,
                &fdss_scale_level_adjust_80[fdss_level[i]]);
        }

    /* Edit  breakpoint table for interpolation case */
        if (pi->fdss_interp_en) {
            for (i = 0; i < 2; i++) {
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_BREAKPOINTSTBL(core),
                    2, 3+5*i, 8, breakpoint_list_interp_20);
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_SCALEFACTORSDELTATBL(core),
                    5, 5*i, 16, fdss_scale_level_interp_20[fdss_level[i]]);
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_SCALEADJUSTFACTORSTBL(core),
                    1, i, 8, &fdss_scale_level_adjust_interp_20[fdss_level[i]]);

                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_BREAKPOINTSTBL(core),
                    2, 13+5*i, 8, breakpoint_list_interp_40);
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_SCALEFACTORSDELTATBL(core),
                    5, 10+5*i, 16, fdss_scale_level_interp_40[fdss_level[i]]);
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_SCALEADJUSTFACTORSTBL(core),
                    1, i+2, 8,
                    &fdss_scale_level_adjust_interp_40[fdss_level[i]]);

                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_BREAKPOINTSTBL(core),
                    2, 23+5*i, 8, breakpoint_list_interp_80);
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_SCALEFACTORSDELTATBL(core),
                    5, 20+5*i, 16, fdss_scale_level_interp_80[fdss_level[i]]);
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_SCALEADJUSTFACTORSTBL(core),
                    1, i+4, 8,
                    &fdss_scale_level_adjust_interp_80[fdss_level[i]]);
            }
        }
        /* CRDOT11ACPHY-2226: Add zero after last breakpoint as workaround
         * of going beyond num_80M_breakpoints
         */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_FDSS_BREAKPOINTSTBL(core),
            1, 5*3*2, 8, &val);
        /* 5: table length; 3: 20MHz 40MHz and 80MHz; 2: low rate and high rate */
    }

}

static void
wlc_phy_set_fdss_table_majrev_ge40(phy_info_t *pi)
{
    uint8 core;
    uint8 nbkpts;
    uint8 *fdss_tbl = NULL;
    uint8 *bkpt_tbl_20 = NULL;
    uint8 *bkpt_tbl_40 = NULL;
    uint8 *bkpt_tbl_80 = NULL;
    uint8 mcstable[71] = {16, 16, 16, 16, 17, 17, 17, 17,
        16, 16, 16, 17, 17, 17, 17, 17,
        16, 16, 16, 17, 17, 17, 17, 17, 17, 17,
        16, 16, 16, 16, 17, 17, 17, 17,
        16, 16, 16, 17, 17, 17, 17, 17,
        16, 16, 16, 17, 17, 17, 17, 17, 17, 17,
        17,
        16, 16, 16, 17, 17, 17, 17, 17,
        16, 16, 16, 17, 17, 17, 17, 17, 17, 17,
        };
    uint8 mcstable_majorrev128[71] = {16, 16, 16, 0, 0, 0, 0, 0,
        16, 16, 16, 0, 0, 0, 0, 0,
        16, 16, 16, 0, 0, 0, 0, 0, 0, 0,
        16, 16, 16, 0, 0, 0, 0, 0,
        16, 16, 16, 0, 0, 0, 0, 0,
        16, 16, 16, 0, 0, 0, 0, 0, 0, 0,
        0,
        16, 16, 16, 0, 0, 0, 0, 0,
        16, 16, 16, 0, 0, 0, 0, 0, 0, 0,
        };

    uint8 i, fdss_level[2];
    uint8 breakpoint_list_20[5] = {0, 3, 17, 48, 62};
    uint8 breakpoint_list_20_6878_fdss6[4] = {0, 17, 33, 48};
    uint8 breakpoint_list_20_6878_fdss7[6] = {0, 9, 17, 36, 48, 56};
    uint8 breakpoint_list_20_6878_fdss8[6] = {0, 9, 17, 36, 48, 56};
    uint8 breakpoint_list_80_6878_fdss9[5] = {0, 38, 85, 171, 219};
    uint8 breakpoint_list_40_6878_fdss9[5] = {0, 19, 44, 85, 110};
    uint8 breakpoint_list_20_6878_fdss9[5] = {0, 9, 18, 47, 56};
    uint8 breakpoint_list_40[5] = {0, 6, 34, 96, 124};
    uint8 breakpoint_list_80[5] = {0, 12, 68, 192, 248};
    uint8 breakpoint_list_interp_20[2] = {47, 61};
    uint8 breakpoint_list_interp_40[2] = {97, 123};
    uint8 breakpoint_list_interp_80[2] = {191, 247};
        /* introducing new fdss table for 4359 */
    uint8 fdss_scale_level[6][5] = {{128, 128, 128, 128, 128},
        {128, 128, 128, 128, 128},
        {164, 146, 104, 146, 164}, /* Mild, meets older +1, -3 dB flatness limits */
        {180, 128, 72, 128, 180}, /* Extreme, meets older +3, -5 dB flatness limits */
        {165, 142, 83, 142, 165},  /* intermediate fdss coeff for 4359 */
        {144, 127, 114, 127, 144} /* intermediate fdss coeff for 4355 */};
    uint8 fdss_scale_level_6878_fdss6[4] = {138, 111, 118, 138};
    uint8 fdss_scale_level_6878_fdss7[6] = {137, 124, 114, 122, 124, 137};
    uint8 fdss_scale_level_6878_fdss8[6] = {141, 128, 126, 115, 128, 141};
    uint8 fdss_scale_level_6878_fdss9[5] = {152, 123, 111, 123, 152};
    uint8 fdss_scale_level_6878_fdss10[5] = {193, 137, 102, 137, 193};
    uint8 fdss_scale_level_6878_fdss11[5] = {166, 143, 106, 143, 166};
    uint8 fdss_scale_level_6878_ch13_fdss6[4] = {138, 111, 118, 138};
    /* boost right side spectrun for channel 13 */
    uint8 fdss_scale_level_6878_ch13_fdss7[6] = {135, 128, 125, 120, 122, 135};
    /* boost fdss scaling on right side for channel 13 */
    uint8 fdss_scale_level_6878_ch13_fdss8[6] = {128, 128, 128, 122, 124, 137};
    uint8 fdss_scale_level_6878_ch1_fdss6[4] = {138, 111, 118, 138};
    /* boost left side spectrun  for channel1 */
    uint8 fdss_scale_level_6878_ch1_fdss7[6] = {133, 120, 110, 140, 133, 133};
    /* boost fdss scaling on left side for channel 1 */
    uint8 fdss_scale_level_6878_ch1_fdss8[6] = {137, 124, 114, 128, 128, 128};
    int16 fdss_scale_level_interp_20[6][5] = {{0, 0, 0, 0, 0},
        {-1700, -362, 0, 362, 1700},
        {-683, -338, 0, 338, 683},
        {-2219, -512, 0, 512, 2219},
        {-683, -338, 0, 338, 683},
        {-1700, -498, 0, 498, 1700}};
    int16 fdss_scale_level_interp_40[5][5] = {{0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {-341, -169, 0, 169, 341},
        {-1109, -256, 0, 256, 1109},
        {-341, -169, 0, 169, 341}};
    int16 fdss_scale_level_interp_80[5][5] = {{0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0},
        {-171, -86, 0, 86, 171},
        {-555, -128, 0, 128, 555},
        {-171, -86, 0, 86, 171}};
    uint8 fdss_scale_level_adjust_20[12] = {128, 128, 132, 128, 128, 128,
        128, 127, 128, 128, 128, 128};
    uint8 fdss_scale_level_adjust_20_ch1[12] = {128, 128, 132, 128, 128, 128,
        128, 127, 128, 128, 128, 128};
    uint8 fdss_scale_level_adjust_20_ch13[12] = {128, 128, 132, 128, 128, 128,
        128, 127, 128, 128, 128, 128};
    uint8* p_fdss_scale_level_adjust_20;
    uint8 fdss_scale_level_adjust_40[12] = {128, 128, 132, 128, 128, 128,
        128, 128, 128, 128, 128, 128};
    uint8 fdss_scale_level_adjust_80[12] = {128, 128, 134, 128, 128, 128,
        128, 128, 128, 128, 128, 128};
    uint8 fdss_scale_level_adjust_interp_20[12] = {128, 128, 132, 128, 128,
        128, 128, 128, 128, 128, 128, 128};
    uint8 fdss_scale_level_adjust_interp_40[12] = {128, 128, 131, 128, 128,
        128, 128, 128, 128, 128, 128, 128};
    uint8 fdss_scale_level_adjust_interp_80[12] = {128, 128, 134, 128, 128,
        128, 128, 128, 128, 128, 128, 128};

    p_fdss_scale_level_adjust_20 = fdss_scale_level_adjust_20;

    if (CHSPEC_IS2G(pi->radio_chanspec)) {
        if (!((CHSPEC_CHANNEL(pi->radio_chanspec) == 13) ||
            (CHSPEC_CHANNEL(pi->radio_chanspec) == 1)) ||
            (pi->fdss_bandedge_2g_en == 0)) {
            fdss_level[0] = pi->fdss_level_2g[0];
            if (pi->fdss_level_2g[1] ==  -1) {
                fdss_level[1] = 0;
            } else {
                fdss_level[1] = pi->fdss_level_2g[1];
            }
        } else if ((CHSPEC_CHANNEL(pi->radio_chanspec) == 13)) {
            fdss_level[0] = pi->fdss_level_2g_ch13[0];
            if (pi->fdss_level_2g_ch13[1] ==  -1) {
                fdss_level[1] = 0;
            } else {
                fdss_level[1] = pi->fdss_level_2g_ch13[1];
            }
            p_fdss_scale_level_adjust_20 = fdss_scale_level_adjust_20_ch13;
        } else {
            fdss_level[0] = pi->fdss_level_2g_ch1[0];
            if (pi->fdss_level_2g_ch1[1] ==  -1) {
                fdss_level[1] = 0;
            } else {
                fdss_level[1] = pi->fdss_level_2g_ch1[1];
            }
            p_fdss_scale_level_adjust_20 = fdss_scale_level_adjust_20_ch1;
        }
    } else {
        fdss_level[0] = pi->fdss_level_5g[0];
        if (pi->fdss_level_5g[1] ==  -1) {
            fdss_level[1] = 0;
        } else {
            fdss_level[1] = pi->fdss_level_5g[1];
        }
    }
    FOREACH_CORE(pi, core) {
        if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
            if (!PHY_IPA(pi) && !ROUTER_4349(pi)) {
                uint8 mcs_value = 16;
                int8 new_mcs_value[77] = { 0 };
                if (CHSPEC_IS2G(pi->radio_chanspec)) {
                    /* Currently enabling only for 20MHz mcs0-5 for ag rates,
                        mcs0-4 for n & ac rates
                    */
                    for (i = 0; i < 5; i++) {
                        new_mcs_value[i] = mcs_value;
                        new_mcs_value[i+8] = mcs_value;
                        new_mcs_value[i+16] = mcs_value;
                    }
                    new_mcs_value[5] = mcs_value;
                } else {
                /* Currently enabling only for mcs0-4 */
                    for (i = 0; i < 5; i++) {
                        new_mcs_value[i] = mcs_value;
                        new_mcs_value[i+8] = mcs_value;
                        new_mcs_value[i+16] = mcs_value;
                    }
                    new_mcs_value[5] = mcs_value;
                    for (i = 0; i < 5; i++) {
                        new_mcs_value[i+28] = mcs_value;
                        new_mcs_value[i+36] = mcs_value;
                        new_mcs_value[i+44] = mcs_value;
                    }
                    new_mcs_value[33] = mcs_value;
                    for (i = 0; i < 5; i++) {
                        new_mcs_value[i+57] = mcs_value;
                        new_mcs_value[i+65] = mcs_value;
                    }
                    new_mcs_value[62] = mcs_value;
                }
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_MCSINFOTBL(core), 77,
                    0, 8, &new_mcs_value);
            } else {
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_MCSINFOTBL(core), 77, 0,
                    8, mcstable_majorrev128);
            }
        } else {
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_MCSINFOTBL(core), 77, 0, 8, mcstable);
        }
        bkpt_tbl_20 = breakpoint_list_20;
        bkpt_tbl_40 = breakpoint_list_40;
        bkpt_tbl_80 = breakpoint_list_80;

        /* Populate breakpoint and scale tables with the scale values for each BW */
        for (i = 0; i < 2; i++) {
            if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
                if (fdss_level[i] == 6) {
                    nbkpts = 4;
                    fdss_tbl = fdss_scale_level_6878_fdss6;
                    bkpt_tbl_20 = breakpoint_list_20_6878_fdss6;
                    if (pi->fdss_bandedge_2g_en) {
                      if (CHSPEC_CHANNEL(pi->radio_chanspec) == 13) {
                        fdss_tbl = fdss_scale_level_6878_ch13_fdss6;
                      } else if (CHSPEC_CHANNEL(pi->radio_chanspec) == 1) {
                        fdss_tbl = fdss_scale_level_6878_ch1_fdss6;
                      }
                    }
                } else if (fdss_level[i] == 7) {
                    nbkpts = 6;
                    fdss_tbl = fdss_scale_level_6878_fdss7;
                    if (pi->fdss_bandedge_2g_en) {
                      if (CHSPEC_CHANNEL(pi->radio_chanspec) == 13) {
                        fdss_tbl = fdss_scale_level_6878_ch13_fdss7;
                      } else if (CHSPEC_CHANNEL(pi->radio_chanspec) == 1) {
                        fdss_tbl = fdss_scale_level_6878_ch1_fdss7;
                      }
                    }
                    bkpt_tbl_20 = breakpoint_list_20_6878_fdss7;
                } else if (fdss_level[i] == 8) {
                    nbkpts = 6;
                    fdss_tbl = fdss_scale_level_6878_fdss8;
                    if (pi->fdss_bandedge_2g_en) {
                      if (CHSPEC_CHANNEL(pi->radio_chanspec) == 13) {
                        fdss_tbl = fdss_scale_level_6878_ch13_fdss8;
                      } else if (CHSPEC_CHANNEL(pi->radio_chanspec) == 1) {
                        fdss_tbl = fdss_scale_level_6878_ch1_fdss8;
                      }
                    }
                    bkpt_tbl_20 = breakpoint_list_20_6878_fdss8;
                } else if (fdss_level[i] == 9) {
                    nbkpts = 5;
                    fdss_tbl = fdss_scale_level_6878_fdss9;
                    bkpt_tbl_80 = breakpoint_list_80_6878_fdss9;
                    bkpt_tbl_40 = breakpoint_list_40_6878_fdss9;
                    bkpt_tbl_20 = breakpoint_list_20_6878_fdss9;
                } else if (fdss_level[i] == 10) {
                    nbkpts = 5;
                    fdss_tbl = fdss_scale_level_6878_fdss10;
                } else if (fdss_level[i] == 11) {
                    nbkpts = 5;
                    breakpoint_list_20[1] = 2;
                    breakpoint_list_20[4] = 63;
                    breakpoint_list_80[1] = 14;
                    breakpoint_list_80[4] = 243;
                    fdss_tbl = fdss_scale_level_6878_fdss11;
                } else {
                    nbkpts = 5;
                    fdss_tbl = fdss_scale_level[fdss_level[i]];
                }
            } else {
                nbkpts = 5;
                fdss_tbl = fdss_scale_level[fdss_level[i]];
            }
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_BREAKPOINTSTBL(core), nbkpts, nbkpts*i, 8,
                bkpt_tbl_20);
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_SCALEFACTORSTBL(core), nbkpts, nbkpts*i, 8,
                fdss_tbl);
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_SCALEADJUSTFACTORSTBL(core), 1, i, 8,
                (p_fdss_scale_level_adjust_20 + fdss_level[i]));

            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_BREAKPOINTSTBL(core), nbkpts,
                (2 * nbkpts) + nbkpts*i, 8,
                bkpt_tbl_40);
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_SCALEFACTORSTBL(core), nbkpts,
                (2 * nbkpts) + nbkpts*i, 8,
                fdss_tbl);
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_SCALEADJUSTFACTORSTBL(core), 1, i+2, 8,
                &fdss_scale_level_adjust_40[fdss_level[i]]);

            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_BREAKPOINTSTBL(core), nbkpts,
                (4 * nbkpts) + nbkpts*i, 8,
                bkpt_tbl_80);
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_SCALEFACTORSTBL(core), nbkpts,
                (4 * nbkpts) + nbkpts*i, 8,
                fdss_tbl);
            wlc_phy_table_write_acphy(pi,
                ACPHY_TBL_ID_FDSS_SCALEADJUSTFACTORSTBL(core), 1, i+4, 8,
                &fdss_scale_level_adjust_80[fdss_level[i]]);
        }
        /* Edit  breakpoint table for interpolation case */

        if (pi->fdss_interp_en) {
            for (i = 0; i < 2; i++) {
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_BREAKPOINTSTBL(core),
                    2, 3+5*i, 8, breakpoint_list_interp_20);
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_SCALEFACTORSDELTATBL(core),
                    5, 5*i, 16, fdss_scale_level_interp_20[fdss_level[i]]);
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_SCALEADJUSTFACTORSTBL(core),
                    1, i, 8, &fdss_scale_level_adjust_interp_20[fdss_level[i]]);

                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_BREAKPOINTSTBL(core),
                    2, 13+5*i, 8, breakpoint_list_interp_40);
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_SCALEFACTORSDELTATBL(core),
                    5, 10+5*i, 16, fdss_scale_level_interp_40[fdss_level[i]]);
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_SCALEADJUSTFACTORSTBL(core),
                    1, i+2, 8,
                    &fdss_scale_level_adjust_interp_40[fdss_level[i]]);

                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_BREAKPOINTSTBL(core),
                    2, 23+5*i, 8, breakpoint_list_interp_80);
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_SCALEFACTORSDELTATBL(core),
                    5, 20+5*i, 16, fdss_scale_level_interp_80[fdss_level[i]]);
                wlc_phy_table_write_acphy(pi,
                    ACPHY_TBL_ID_FDSS_SCALEADJUSTFACTORSTBL(core),
                    1, i+4, 8,
                    &fdss_scale_level_adjust_interp_80[fdss_level[i]]);
            }
        }
    }
}
#endif /* WL_FDSS_DISABLED */

static void
wlc_acphy_load_4349_specific_tbls(phy_info_t *pi)
{
    wlc_acphy_load_radiocrisscross_phyovr_mode(pi);
    wlc_acphy_load_logen_tbl(pi);
}

static void
wlc_acphy_load_radiocrisscross_phyovr_mode(phy_info_t *pi)
{
    uint8 core;
    FOREACH_CORE(pi, core) {
        WRITE_PHYREGCE(pi, AfeClkDivOverrideCtrlN, core, 0x0000);
        WRITE_PHYREGCE(pi, RfctrlAntSwLUTIdxN, core, 0x0000);
        WRITE_PHYREGCE(pi, RfctrlCoreTxPus, core,
            (READ_PHYREGCE(pi, RfctrlCoreTxPus, core) & 0x7DFF));
        WRITE_PHYREGCE(pi, RfctrlOverrideTxPus, core,
            (READ_PHYREGCE(pi, RfctrlOverrideTxPus, core) & 0xF3FF));
    }
}

static void wlc_acphy_load_logen_tbl(phy_info_t *pi)
{
    /* 4349BU */
    if (ACMAJORREV_4(pi->pubpi->phy_rev))
        return;

    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        if (phy_get_phymode(pi) == PHYMODE_MIMO) {
            /* set logen mimodes pu */
            wlc_phy_radio20693_set_reset_table_bits(pi, ACPHY_TBL_ID_RFSEQ,
                0x14d, 1, 1, 0, 16);
            wlc_phy_radio20693_set_reset_table_bits(pi, ACPHY_TBL_ID_RFSEQ,
                0x15d, 1, 1, 1, 16);
            /* Set logen mimosrc pu */
            wlc_phy_radio20693_set_reset_table_bits(pi, ACPHY_TBL_ID_RFSEQ,
                0x14d, 4, 4, 1, 16);
            wlc_phy_radio20693_set_reset_table_bits(pi, ACPHY_TBL_ID_RFSEQ,
                0x15d, 4, 4, 0, 16);
        } else {
            /* set logen mimodes pu */
            wlc_phy_radio20693_set_reset_table_bits(pi, ACPHY_TBL_ID_RFSEQ,
                0x14d, 1, 1, 0, 16);
            wlc_phy_radio20693_set_reset_table_bits(pi, ACPHY_TBL_ID_RFSEQ,
                0x15d, 1, 1, 0, 16);
            /* Set logen mimosrc pu */
            wlc_phy_radio20693_set_reset_table_bits(pi, ACPHY_TBL_ID_RFSEQ,
                0x14d, 4, 4, 0, 16);
            wlc_phy_radio20693_set_reset_table_bits(pi, ACPHY_TBL_ID_RFSEQ,
                0x15d, 4, 4, 0, 16);
        }
    }
}

static void
wlc_phy_set_regtbl_on_band_change_acphy_20693(phy_info_t *pi)
{

    uint8 core = 0;

    ASSERT(RADIOID_IS(pi->pubpi->radioid, BCM20693_ID));

    if (RADIOMAJORREV(pi) == 3) {
        wlc_phy_radio20693_sel_logen_mode(pi);
        return;
    }

    FOREACH_CORE(pi, core)
    {
        if (CHSPEC_IS2G(pi->radio_chanspec))
        {
            phy_utils_write_radioreg(pi, RADIO_REG_20693(pi,
                TX_TOP_2G_OVR_EAST, core), 0x0);
            phy_utils_write_radioreg(pi, RADIO_REG_20693(pi,
                TX_TOP_2G_OVR1_EAST, core), 0x0);
            phy_utils_write_radioreg(pi, RADIO_REG_20693(pi,
                RX_TOP_2G_OVR_EAST, core), 0x0);
            phy_utils_write_radioreg(pi, RADIO_REG_20693(pi,
                RX_TOP_2G_OVR_EAST2, core), 0x0);
            if (PHY_IPA(pi)) {
                phy_utils_write_radioreg(pi, RADIO_REG_20693(pi,
                    BG_TRIM2, core), 0x1937);
            }

            if (RADIOMAJORREV(pi) == 2) {
                MOD_RADIO_REG_20693(pi, RX_TOP_5G_OVR2, core,
                    ovr_mix5g_lobuf_en, 0);
                MOD_RADIO_REG_20693(pi, LNA5G_CFG3, core, mix5g_lobuf_en, 0);
            }
            RADIO_REG_LIST_START
                MOD_RADIO_REG_20693_ENTRY(pi, TIA_CFG8, core,
                    tia_offset_dac_biasadj, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_2G_OVR_EAST2, core,
                    ovr_lna2g_tr_rx_en, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, LNA2G_CFG1, core, lna2g_tr_rx_en, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_5G_OVR, core,
                    ovr_lna5g_tr_rx_en, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, LNA5G_CFG1, core, lna5g_tr_rx_en, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_2G_OVR_EAST, core,
                    ovr_gm2g_auxgm_pwrup, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, LNA2G_CFG2, core, gm2g_auxgm_pwrup, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, LOGEN_CFG2, core, logencore_5g_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, LOGEN_OVR1, core,
                    ovr_logencore_5g_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, TX5G_CFG1, core, tx5g_bias_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR1, core,
                    ovr_tx5g_bias_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, TXMIX5G_CFG4, core, mx5g_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR1, core, ovr_mx5g_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, TXMIX5G_CFG4, core,
                    mx5g_pu_lodc_loop, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR2, core,
                    ovr_mx5g_pu_lodc_loop, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, PA5G_CFG1, core, pa5g_bias_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR1, core,
                    ovr_pa5g_bias_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, PA5G_CFG1, core, pa5g_bias_cas_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR2, core,
                    ovr_pa5g_bias_cas_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, PA5G_CFG4, core, pa5g_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR1, core, ovr_pa5g_pu, 1)
            RADIO_REG_LIST_EXECUTE(pi, core);

            if ((RADIO20693_MAJORREV(pi->pubpi->radiorev) == 1) &&
                (RADIO20693_MINORREV(pi->pubpi->radiorev) == 1)) {
                MOD_RADIO_REG_20693(pi, TRSW5G_CFG1, core, trsw5g_pu, 0);
                MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR1, core, ovr_trsw5g_pu, 1);
            }

            RADIO_REG_LIST_START
                MOD_RADIO_REG_20693_ENTRY(pi, TX_LOGEN5G_CFG1, core,
                    logen5g_tx_enable_5g, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR1, core,
                    ovr_logen5g_tx_enable_5g, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_LOGEN5G_CFG1, core,
                    logen5g_tx_enable_5g_low_band, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_5G_OVR2, core,
                    ovr_logen5g_tx_enable_5g_low_band, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, LNA5G_CFG1, core, lna5g_lna1_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_5G_OVR, core,
                    ovr_lna5g_lna1_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, LNA5G_CFG2, core, lna5g_pu_lna2, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_5G_OVR, core,
                    ovr_gm5g_pwrup, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, LNA5G_RSSI1, core,
                    lna5g_dig_wrssi1_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_5G_OVR, core,
                    ovr_lna5g_dig_wrssi1_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, LNA5G_CFG2, core, lna5g_pu_auxlna2, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_5G_OVR, core,
                    ovr_lna5g_pu_auxlna2, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, LNA5G_CFG3, core, mix5g_en, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_5G_OVR, core, ovr_mix5g_en, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_LPF_CFG2, core,
                    lpf_sel_5g_out_gm, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_LPF_CFG3, core,
                    lpf_sel_2g_5g_cmref_gm, 0)
            RADIO_REG_LIST_EXECUTE(pi, core);
            /* Bimodal settings */
            if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
                RADIO_REG_LIST_START
                    MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_2G_OVR_EAST2, core,
                        ovr_rxmix2g_pu, 1)
                    MOD_RADIO_REG_20693_ENTRY(pi, RXMIX2G_CFG1, core,
                        rxmix2g_pu, 1)
                    MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_2G_OVR_EAST, core,
                        ovr_rxdiv2g_rs, 1)
                    MOD_RADIO_REG_20693_ENTRY(pi, RXRF2G_CFG1, core,
                        rxdiv2g_rs, 0)
                    MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_2G_OVR_EAST, core,
                        ovr_rxdiv2g_pu_bias, 1)
                    MOD_RADIO_REG_20693_ENTRY(pi, RXRF2G_CFG1, core,
                        rxdiv2g_pu_bias, 1)
                    /* Turn off 5g overrides */
                    MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_5G_OVR, core,
                        ovr_mix5g_en, 0)
                    MOD_RADIO_REG_20693_ENTRY(pi, LNA5G_CFG3, core, mix5g_en, 0)
                RADIO_REG_LIST_EXECUTE(pi, core);
                if (!(PHY_IPA(pi)) && (RADIO20693REV(pi->pubpi->radiorev) == 13)) {
                    wlc_phy_set_bias_ipa_as_epa_acphy_20693(pi, core);
                }
            }
        }
        else
        {
            phy_utils_write_radioreg(pi, RADIO_REG_20693(pi, TX_TOP_5G_OVR1, core), 0);
            phy_utils_write_radioreg(pi, RADIO_REG_20693(pi, TX_TOP_5G_OVR2, core), 0);
            if (PHY_IPA(pi) && !(ROUTER_4349(pi))) {
                phy_utils_write_radioreg(pi, RADIO_REG_20693(pi,
                    BG_TRIM2, core), 0x1737);
            }

            if (RADIOMAJORREV(pi) == 2) {
                MOD_RADIO_REG_20693(pi, RX_TOP_5G_OVR2, core,
                    ovr_mix5g_lobuf_en, 1);
                MOD_RADIO_REG_20693(pi, LNA5G_CFG3, core, mix5g_lobuf_en, 1);
            }
            MOD_RADIO_REG_20693(pi, RX_TOP_5G_OVR, core, ovr_lna5g_tr_rx_en,
                (ROUTER_4349(pi) ? 0 : 1));
            MOD_RADIO_REG_20693(pi, LNA5G_CFG1, core, lna5g_tr_rx_en,
                (ROUTER_4349(pi) ? 0 : 1));
            RADIO_REG_LIST_START
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_5G_OVR, core,
                    ovr_lna5g_lna1_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_5G_OVR, core,
                    ovr_gm5g_pwrup, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_5G_OVR, core,
                    ovr_lna5g_dig_wrssi1_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_5G_OVR, core,
                    ovr_lna5g_pu_auxlna2, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, TIA_CFG8, core,
                    tia_offset_dac_biasadj, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_2G_OVR_EAST2, core,
                    ovr_lna2g_tr_rx_en, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, LNA2G_CFG1, core, lna2g_tr_rx_en, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, LOGEN_CFG2, core, logencore_2g_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, LOGEN_OVR1, core,
                    ovr_logencore_2g_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, LNA2G_CFG2, core, gm2g_auxgm_pwrup, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_2G_OVR_EAST, core,
                    ovr_gm2g_auxgm_pwrup, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, LNA2G_CFG2, core, gm2g_pwrup, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_2G_OVR_EAST, core,
                    ovr_gm2g_pwrup, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, TX2G_CFG1, core, tx2g_bias_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_2G_OVR_EAST, core,
                    ovr_tx2g_bias_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, TXMIX2G_CFG2, core, mx2g_bias_en, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_2G_OVR_EAST, core,
                    ovr_mx2g_bias_en, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, PA2G_CFG1, core, pa2g_bias_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_2G_OVR_EAST, core,
                    ovr_pa2g_bias_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, PA2G_CFG1, core, pa2g_2gtx_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_2G_OVR_EAST, core,
                    ovr_pa2g_2gtx_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, PA2G_IDAC2, core, pa2g_bias_cas_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_2G_OVR_EAST, core,
                    ovr_pa2g_bias_cas_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_LOGEN2G_CFG1, core,
                    logen2g_tx_pu_bias, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_2G_OVR_EAST, core,
                    ovr_logen2g_tx_pu_bias, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_LOGEN2G_CFG1, core,
                    logen2g_tx_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_TOP_2G_OVR_EAST, core,
                    ovr_logen2g_tx_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_2G_OVR_EAST2, core,
                    ovr_rxmix2g_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, RXMIX2G_CFG1, core, rxmix2g_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_2G_OVR_EAST, core,
                    ovr_lna2g_dig_wrssi1_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, LNA2G_RSSI1, core,
                    lna2g_dig_wrssi1_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_2G_OVR_EAST2, core,
                    ovr_lna2g_lna1_pu, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, LNA2G_CFG1, core, lna2g_lna1_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, LNA5G_CFG3, core, mix5g_en, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_5G_OVR, core, ovr_mix5g_en, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, LOGEN_OVR1, core,
                    ovr_logencore_5g_pu, 0)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_LPF_CFG2, core,
                    lpf_sel_5g_out_gm, 1)
                MOD_RADIO_REG_20693_ENTRY(pi, TX_LPF_CFG3, core,
                    lpf_sel_2g_5g_cmref_gm, 1)
            RADIO_REG_LIST_EXECUTE(pi, core);
            /* Bimodal settings */
            if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
                RADIO_REG_LIST_START
                    MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_5G_OVR, core,
                        ovr_mix5g_en, 1)
                    MOD_RADIO_REG_20693_ENTRY(pi, LNA5G_CFG3, core, mix5g_en, 1)
                    /* Turn off 2G overrides */
                    MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_2G_OVR_EAST2, core,
                        ovr_rxmix2g_pu, 0)
                    MOD_RADIO_REG_20693_ENTRY(pi, RXMIX2G_CFG1, core,
                        rxmix2g_pu, 0)
                    MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_2G_OVR_EAST, core,
                        ovr_rxdiv2g_rs, 0)
                    MOD_RADIO_REG_20693_ENTRY(pi, RXRF2G_CFG1, core,
                        rxdiv2g_rs, 0)
                    MOD_RADIO_REG_20693_ENTRY(pi, RX_TOP_2G_OVR_EAST, core,
                        ovr_rxdiv2g_pu_bias, 0)
                    MOD_RADIO_REG_20693_ENTRY(pi, RXRF2G_CFG1, core,
                        rxdiv2g_pu_bias, 0)
                RADIO_REG_LIST_EXECUTE(pi, core);
                if (!ROUTER_4349(pi) && PHY_IPA(pi))
                    MOD_RADIO_REG_20693(pi, TXMIX5G_CFG8, core,
                        pad5g_idac_gm, 58);
            }
        } /* band */
    } /* foreach core */

    if (phy_get_phymode(pi) == PHYMODE_MIMO) {
        wlc_phy_radio20693_mimo_core1_pmu_restore_on_bandhcange(pi);
    }

    /* This is applicable only for 4349B0 variants */
    if (PHY_IPA(pi)) {
        if ((RADIO20693REV(pi->pubpi->radiorev) >= 0xE) &&
            (RADIO20693REV(pi->pubpi->radiorev) <= 0x12)) {
            FOREACH_CORE(pi, core) {
                phy_utils_write_radioreg(pi, RADIO_REG_20693(pi, BG_TRIM2, core),
                    (CHSPEC_IS2G(pi->radio_chanspec)) ? 0x1937 : 0x1737);
            }
            /* minipmu_cal */
            wlc_phy_tiny_radio_minipmu_cal(pi);
        }
    }

    /* JIRA: SWWLAN-93908 SWWLAN-94230: MiniPMU settings for Rev3xx 53573 boards.
     * P3xx series boards uses 1.5V input to the MiniPMU against the 1.4V used by the P23x
     * series boards. This changes is required to align the mini pmu LDO voltages to 1.2V with a
     * higher input voltage.
     */
    if (ROUTER_4349(pi) && (((pi->sh->boardrev >> 8) & 0xf) >= 0x3)) {
        FOREACH_CORE(pi, core) {
            MOD_RADIO_REG_20693(pi, BG_TRIM2, core, bg_pmu_vbgtrim, 26);
            MOD_RADIO_REG_20693(pi, PMU_CFG1, core, wlpmu_vrefadj_cbuck, 6);
        }
        /* minipmu_cal */
        wlc_phy_tiny_radio_minipmu_cal(pi);
    }

}

static void
wlc_phy_load_channel_smoothing_tiny(phy_info_t *pi)
{

    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        /* set 64 48-bit entries */
        wlc_phy_table_write_tiny_chnsmth(pi,
            ACPHY_TBL_ID_CORE0CHANSMTH_FLTR,
            CHANSMTH_FLTR_LENGTH, 0, 48, acphy_Smth_tbl_4349);
        if (phy_get_phymode(pi) == PHYMODE_MIMO) {
            wlc_phy_table_write_tiny_chnsmth(pi,
                ACPHY_TBL_ID_CORE1CHANSMTH_FLTR,
                CHANSMTH_FLTR_LENGTH, 0, 48, acphy_Smth_tbl_4349);
        }
    } else {
        const uint16 zero_table[3] = { 0, 0, 0 };
        acphytbl_info_t tbl;
        tbl.tbl_id = ACPHY_TBL_ID_CHANNELSMOOTHING_1x1;
        tbl.tbl_ptr = zero_table;
        tbl.tbl_len = 1;
        tbl.tbl_offset = 0;
        tbl.tbl_width = 48;
        /* clear 1st 128 48-bit entries */
        for (tbl.tbl_offset = 0; tbl.tbl_offset < 128; tbl.tbl_offset++) {
            wlc_phy_table_write_ext_acphy(pi, &tbl);
        }

        /* set next 64 48-bit entries */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_CHANNELSMOOTHING_1x1,
                                  CHANSMTH_FLTR_LENGTH, 128,
                                  tbl.tbl_width, acphy_Smth_tbl_tiny);

        /* clear next 64 48-bit entries */
        for (tbl.tbl_offset = 128 + (ARRAYSIZE(acphy_Smth_tbl_tiny) / 3);
             tbl.tbl_offset < 256;
             tbl.tbl_offset++) {
            wlc_phy_table_write_ext_acphy(pi, &tbl);
        }
    }
}
static void
wlc_phy_set_reg_on_reset_acphy_20693(phy_info_t *pi)
{
    BCM_REFERENCE(pi);
}

static void
wlc_phy_set_spareReg_on_reset_majorrev4(phy_info_t *pi, uint8 phyrxchain)
{
    uint16 spare_reg = READ_PHYREG(pi, SpareReg);

    if (phy_get_phymode(pi) == PHYMODE_MIMO) {
        /* The targeted use case is mimo mode coremask 1 case.
         * Below settings will turn off some of the blocks for core 1
         * and thus resulting in current savings
         */
        if (phyrxchain == 1) {
            /* bit #12: Limit hrp access to core0 alone. Should be
               made 1 before m aking 1 bits 8,9,13 and should
               be made 0 only after bits 8,9,13 are made 0.
               Recommended value: 0x1
             */
            WRITE_PHYREG(pi, SpareReg, (spare_reg & ~(1 << 12)));
            spare_reg = READ_PHYREG(pi, SpareReg);

            /* bit #8: Use core1 clk for second chain like rsdb except div4 clk
               Recommended value: 0x1
             */
            spare_reg &= ~(1 << 8);
            /* bit #9: Turn off core1 divider in phy1rx1 */
            /* Recommended value: 0x1 */
            spare_reg &= ~(1 << 9);
            /* bit #13: Use core1 clk for second chain for div4 clk */
            /* Recommended value: 0x1 */
            spare_reg &= ~(1 << 13);
        }
        /* bit #10: Turn off core1 divider in RX2 */
        /* Recommended value: 0x1 */
        spare_reg &= ~(1 << 10);
    }

    /* bit #6: Duration control of Rx2tx reset to some designs. Enable always */
    spare_reg |= (1 << 6);

    /* bit #11: Turn off RX2 during TX */
    spare_reg |= (1 << 11);

    WRITE_PHYREG(pi, SpareReg, spare_reg);
}

static void
wlc_phy_set_tssiSleep_on_reset(phy_info_t *pi)
{
    uint8 srom_tssisleep_en = pi->u.pi_acphy->chanmgri->cfg->srom_tssisleep_en;

    if (ACMAJORREV_2(pi->pubpi->phy_rev) && PHY_IPA(pi)) {
        if (CHSPEC_IS2G(pi->radio_chanspec)) {
            if (((CHSPEC_BW_LE20(pi->radio_chanspec)) && (srom_tssisleep_en & 0x1)) ||
              ((CHSPEC_IS40(pi->radio_chanspec)) && (srom_tssisleep_en & 0x2))) {
                MOD_PHYREG(pi, AfePuCtrl, tssiSleepEn, 1);
            } else {
                MOD_PHYREG(pi, AfePuCtrl, tssiSleepEn, 0);
            }
        } else {
            if (((CHSPEC_BW_LE20(pi->radio_chanspec)) && (srom_tssisleep_en & 0x4)) ||
              ((CHSPEC_IS40(pi->radio_chanspec)) && (srom_tssisleep_en & 0x8)) ||
              ((CHSPEC_IS80(pi->radio_chanspec)) && (srom_tssisleep_en & 0x10))) {
                MOD_PHYREG(pi, AfePuCtrl, tssiSleepEn, 1);
            } else {
                MOD_PHYREG(pi, AfePuCtrl, tssiSleepEn, 0);
            }
        }
    } else if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, AfePuCtrl, tssiSleepEn, 1);
    } else {
        MOD_PHYREG(pi, AfePuCtrl, tssiSleepEn, 0);
    }
}

static void
wlc_phy_set_mfLessAve_on_reset(phy_info_t *pi)
{
    if (ACREV_GE(pi->pubpi->phy_rev, 32)) {
        uint8 core;

        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            /*  Increase bphy digigain freeze time to 3 us */
            MOD_PHYREG(pi, overideDigiGain1, cckdigigainEnCntValue, 119);
            FOREACH_CORE(pi, core) {
                MOD_PHYREGCE(pi, crsControll, core, mfLessAve, 0);
                MOD_PHYREGCE(pi, crsControlu, core, mfLessAve, 0);
                MOD_PHYREGCE(pi, crsControllSub1, core, mfLessAve, 0);
                MOD_PHYREGCE(pi, crsControluSub1, core, mfLessAve, 0);
            }
        /* Retain reset values for 6878 */
        } else if (!(ACMAJORREV_128(pi->pubpi->phy_rev))) {
            FOREACH_CORE(pi, core) {
                MOD_PHYREGCE(pi, crsControlu, core, mfLessAve, 0);
                if ((wlc_phy_ac_phycap_maxbw(pi) > BW_20MHZ)) {
                    MOD_PHYREGCE(pi, crsControll, core, mfLessAve, 0);
                    MOD_PHYREGCE(pi, crsControllSub1, core, mfLessAve, 0);
                    MOD_PHYREGCE(pi, crsControluSub1, core, mfLessAve, 0);
                }
            }
        }
    } else {
        ACPHY_REG_LIST_START
            MOD_PHYREG_ENTRY(pi, crsControll, mfLessAve, 0)
            MOD_PHYREG_ENTRY(pi, crsControlu, mfLessAve, 0)
            MOD_PHYREG_ENTRY(pi, crsControllSub1, mfLessAve, 0)
            MOD_PHYREG_ENTRY(pi, crsControluSub1, mfLessAve, 0)
        ACPHY_REG_LIST_EXECUTE(pi);
    }
}

static void
wlc_phy_set_peakThresh_on_reset(phy_info_t *pi)
{
    if (!(ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev))) {
        if (ACREV_GE(pi->pubpi->phy_rev, 32)) {
            uint8 core;

            if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                ACMAJORREV_33(pi->pubpi->phy_rev) ||
                (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                !ACMAJORREV_128(pi->pubpi->phy_rev))) {
                FOREACH_CORE(pi, core) {
                    MOD_PHYREGCE(pi, crsThreshold2l, core, peakThresh, 85);
                    MOD_PHYREGCE(pi, crsThreshold2u, core, peakThresh, 85);
                    MOD_PHYREGCE(pi, crsThreshold2lSub1, core, peakThresh, 85);
                    MOD_PHYREGCE(pi, crsThreshold2uSub1, core, peakThresh, 85);
                }
            } else {
                FOREACH_CORE(pi, core) {
                    MOD_PHYREGCE(pi, crsThreshold2u, core, peakThresh, 85);
                    if ((wlc_phy_ac_phycap_maxbw(pi) > BW_20MHZ)) {
                        MOD_PHYREGCE(pi, crsThreshold2l, core,
                            peakThresh, 85);
                        MOD_PHYREGCE(pi, crsThreshold2lSub1, core,
                            peakThresh, 85);
                        MOD_PHYREGCE(pi, crsThreshold2uSub1, core,
                            peakThresh, 85);
                    }
                }
            }
        } else {
            ACPHY_REG_LIST_START
                MOD_PHYREG_ENTRY(pi, crsThreshold2l, peakThresh, 85)
                MOD_PHYREG_ENTRY(pi, crsThreshold2u, peakThresh, 85)
                MOD_PHYREG_ENTRY(pi, crsThreshold2lSub1, peakThresh, 85)
                MOD_PHYREG_ENTRY(pi, crsThreshold2uSub1, peakThresh, 85)
            ACPHY_REG_LIST_EXECUTE(pi);
        }
    } else {
        if (ACMAJORREV_5(pi->pubpi->phy_rev) && CHSPEC_IS20(pi->radio_chanspec)) {
            WRITE_PHYREG(pi, crsThreshold2u, 0x2055);
            WRITE_PHYREG(pi, crsThreshold2l, 0x2055);
        } else {
            WRITE_PHYREG(pi, crsThreshold2u, 0x204d);
            WRITE_PHYREG(pi, crsThreshold2l, 0x204d);
        }
        WRITE_PHYREG(pi, crsThreshold2lSub1, 0x204d);
        WRITE_PHYREG(pi, crsThreshold2uSub1, 0x204d);
    }
}

static void
wlc_phy_set_reg_on_reset_majorrev32_33_37_47_51_129_130_131(phy_info_t *pi)
{
    uint8 core;
    uint16 rxbias, txbias;
    uint32 klm_ulmu[16] = {0x3FE80, 0x36E3B, 0x31424, 0x2DE19,
                           0x2BE13, 0x2AA0F, 0x29A0D, 0x2900B,
                           0x28809, 0x28208, 0x27E08, 0x27A07,
                           0x27606, 0x27406, 0x27005, 0x26E05};
    uint32 klm_ulmu_1x[16] = {0x3E9E5, 0x36AE8, 0x3128E, 0x2DE63,
                              0x3884E, 0x3382E, 0x2FA1F, 0x2D016,
                              0x2B611, 0x2A40E, 0x2960C, 0x28E0A,
                              0x28609, 0x28008, 0x27C07, 0x27807};
    uint16 he_chanupdMutbl[15] = {0x2456, 0x7822, 0x2456, 0x6711, 0x2256,
            0x6710, 0x2256, 0x6633, 0x5666, 0x6622,
            0x5666, 0x4411, 0x4534, 0x4410, 0x4534};

    FOREACH_CORE(pi, core) {
        MOD_PHYREGCE(pi, crshighlowpowThresholdl, core, low2highpowThresh, 69);
        MOD_PHYREGCE(pi, crshighlowpowThresholdu, core, low2highpowThresh, 69);
        MOD_PHYREGCE(pi, crshighlowpowThresholdlSub1, core, low2highpowThresh, 69);
        MOD_PHYREGCE(pi, crshighlowpowThresholduSub1, core, low2highpowThresh, 69);

        // This is to save power, the setting is applicable to all chips.
        MOD_PHYREGCE(pi, forceFront, core, freqCor, 0);
        MOD_PHYREGCE(pi, forceFront, core, freqEst, 0);

        // Old DCC related rf regs used for 4349/4345
        if (RADIOID_IS(pi->pubpi->radioid, BCM20693_ID)) {
            MOD_RADIO_REG_20693(pi, RX_BB_2G_OVR_EAST, core,
                          ovr_tia_offset_comp_pwrup, 1);
            MOD_RADIO_REG_20693(pi, TIA_CFG15, core, tia_offset_comp_pwrup, 1);
        }

        /* No limit for now on max analog gain */
        MOD_PHYREGC(pi, HpFBw, core, maxAnalogGaindb, 100);
        MOD_PHYREGC(pi, DSSScckPktGain, core, dsss_cck_maxAnalogGaindb, 100);

        /* SW RSSI report calculation based on variance (DC is removed) */
        MOD_PHYREGCEE(pi, RxStatPwrOffset, core, use_gainVar_for_rssi, 1);
    }

    if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, SlnaControl, inv_btcx_prisel_polarity, 1);
    }

    /* [4365] need to reduce FSTR threshold by 12dB to resolve
       the hump around -80dB~-90dBm
    */
    MOD_PHYREG(pi, FSTRHiPwrTh, finestr_hiPwrSm_th, 65);
    if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, FSTRHiPwrTh, finestr_hiPwr_th, 57);
    } else {
        MOD_PHYREG(pi, FSTRHiPwrTh, finestr_hiPwr_th, 51);
    }

    // Enable_bcm1_proprietary_256QAM
    MOD_PHYREG(pi, miscSigCtrl, brcm_11n_256qam_support, 0x1);

    // Make this same as TCL
    // 4365 needs to disable bphypredetection,
    // otherwise pktproc stuck at bphy when(AP) 20L is receiving ACK from(STA) 20
    MOD_PHYREG(pi, CRSMiscellaneousParam, bphy_pre_det_en, 0);
    if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, CRSMiscellaneousParam, b_over_ag_falsedet_en, 0x0);
    } else {
        MOD_PHYREG(pi, CRSMiscellaneousParam, b_over_ag_falsedet_en, 0x1);
    }
    MOD_PHYREG(pi, bOverAGParams, bOverAGlog2RhoSqrth, 0x0);

    // FIXME: the following settings need clarification
    MOD_PHYREG(pi, CRSMiscellaneousParam, crsInpHold, 1);
    //MOD_PHYREG(pi, RxStatPwrOffset0, use_gainVar_for_rssi0, 1);

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        // Temporarily disable dac_reset pulse to avoid OOB 8MHz spur
        txbias = 0x0a; rxbias = 0x2c;
    } else {
        txbias = 0x2b; rxbias = 0x28;
    }

    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0xe8, 16, &txbias);
    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0xe7, 16, &rxbias);
    // END of FIXME

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        uint16 val = 0x4;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x36e, 16, &val);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x37e, 16, &val);
    }

    // Tuned for ofdm PER humps
    WRITE_PHYREG(pi, HTAGCWaitCounters, 0x1020);

    // Linear filter compensation in fine timing:
    //   maps to C-model minkept settings for optimal ofdm sensitivity
    WRITE_PHYREG(pi, FSTRCtrl, 0x7aa);

    // Enable fstr auto-adjust
    if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, FSTRCtrl_he4, cp3p2AdjOverride, 0);
        MOD_PHYREG(pi, FSTRCtrl_he4, cp1p6AdjOverride, 0);
    }
    // MAC-aided mode timer setting(constant for all BWs)
    if (ACMAJORREV_47(pi->pubpi->phy_rev) || ACMAJORREV_51_131(pi->pubpi->phy_rev) ||
        ACMAJORREV_129(pi->pubpi->phy_rev)) {
        pi->u.pi_acphy->ul_mac_aided_timing_en = 0;
        if (ACMAJORREV_47(pi->pubpi->phy_rev) || ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
            pi->u.pi_acphy->ul_mac_aided_en = 1;
            wlc_phy_ul_mac_aided(pi);
            wlc_phy_ul_mac_aided_war(pi);
        } else {
            pi->u.pi_acphy->ul_mac_aided_en = 0;
        }

        MOD_PHYREG(pi, Config0_MAC_aided_trig_frame, en_mac_aided_crs, 0);
        MOD_PHYREG(pi, Config0_MAC_aided_trig_frame, en_mac_aided_cstr, 0);
        MOD_PHYREG(pi, Config0_MAC_aided_trig_frame, en_mac_aided_fstr, 0);
        MOD_PHYREG(pi, Config3_MAC_aided_trig_frame, wait_ofdm_time, 970);
        MOD_PHYREG(pi, Config4_MAC_aided_trig_frame, wait_he_time, 1690);
        MOD_PHYREG(pi, Config5_MAC_aided_trig_frame, wait_trig_time, 2170);
        MOD_PHYREG(pi, Config6_MAC_aided_trig_frame, mac_aided_crs_time, 860);
        MOD_PHYREG(pi, Config7_MAC_aided_trig_frame, mac_aided_cstr_time, 1090);
        MOD_PHYREG(pi, Config8_MAC_aided_trig_frame, mac_aided_fstr_time, 1260);
    }

    if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        pi->u.pi_acphy->ul_mac_aided_timing_en = 0;
        pi->u.pi_acphy->ul_mac_aided_en = 1;
        wlc_phy_ul_mac_aided(pi);

        MOD_PHYREG(pi, Config3_MAC_aided_trig_frame, wait_ofdm_time, 970);
        MOD_PHYREG(pi, Config4_MAC_aided_trig_frame, wait_he_time, 1850);
        MOD_PHYREG(pi, Config5_MAC_aided_trig_frame, wait_trig_time, 2170);

        // timing forcing for 3.2us CP
        MOD_PHYREG(pi, Config8_MAC_aided_trig_frame, en_mac_aided_det, 0);
        MOD_PHYREG(pi, Config6_MAC_aided_trig_frame, mac_aided_crs_time, 860);
        MOD_PHYREG(pi, Config7_MAC_aided_trig_frame, mac_aided_cstr_time, 1090);
        MOD_PHYREG(pi, Config8_MAC_aided_trig_frame, mac_aided_fstr_time, 1260);

        // timing forcing for 1.6us CP
        MOD_PHYREG(pi, Config12_MAC_aided_trig_frame, en_mac_aided_cp1p6_det, 0);
        MOD_PHYREG(pi, Config13_MAC_aided_trig_frame, mac_aided_crs_cp1p6_time, 860);
        MOD_PHYREG(pi, Config14_MAC_aided_trig_frame, mac_aided_cstr_cp1p6_time, 1090);
        MOD_PHYREG(pi, Config15_MAC_aided_trig_frame, mac_aided_fstr_cp1p6_time, 1260);
    }

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, FFTSoftReset, lbsdadc_clken_ovr, 1);
        MOD_PHYREG(pi, RxSdFeConfig5, rx_farow_scale_value, 9);
        MOD_PHYREG(pi, RxSdFeConfig5, tiny_bphy20_ADC10_sel, 1);
        MOD_PHYREG(pi, HeRxBePhsCtrl, HeLtfTrackingEn, 1);
    } else {
        // Tiny specific: reason unknown
        MOD_PHYREG(pi, FFTSoftReset, lbsdadc_clken_ovr, 0);
        // Tiny specific: tune conversion gain in 20/40
        MOD_PHYREG(pi, RxSdFeConfig5, rx_farow_scale_value, 7);
        // Tiny specific: disable DVG2 to avoid bphy resampler saturation
        // used to avoid 1mbps performance issues due to DC offset. Required
        MOD_PHYREG(pi, RxSdFeConfig5, tiny_bphy20_ADC10_sel, 0);
    }

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        pi->u.pi_acphy->sromi->srom_low_adc_rate_en) {
        wlc_phy_low_rate_adc_enable_acphy(pi, TRUE);
    }

    if (!ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        // Tiny specific: required for 1mbps performance
        MOD_PHYREG(pi, bphyTest, dccomp, 0);

        // Tiny specific: required for 1mbps performance
        MOD_PHYREG(pi, bphyFiltBypass, bphy_tap_20in20path_from_DVGA_en, 1);
    }

    // TXBF related regs
    //  c_param_wlan_bfe_user_index for implicit TXBF
    WRITE_PHYREG(pi, BfeConfigReg1, 0x1f);
    MOD_PHYREG(pi, BfrMuConfigReg0, bfrMu_delta_snr_mode, 2);

    // PHY capability relate regs
    MOD_PHYREG(pi, RfseqCoreActv2059, DisTx, pi->pubpi->phy_coremask);
    MOD_PHYREG(pi, RfseqCoreActv2059, DisRx, pi->pubpi->phy_coremask);
    MOD_PHYREG(pi, RfseqCoreActv2059, EnTx, pi->pubpi->phy_coremask);
    MOD_PHYREG(pi, RfseqCoreActv2059, EnRx, pi->pubpi->phy_coremask);
    MOD_PHYREG(pi, CoreConfig, CoreMask, pi->pubpi->phy_coremask);
    MOD_PHYREG(pi, CoreConfig, NumRxCores, pi->pubpi->phy_corenum);
    MOD_PHYREG(pi, HTSigTones, support_max_nss, pi->pubpi->phy_corenum);

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        // DCC related regs
        MOD_PHYREG(pi, RfseqTrigger, en_pkt_proc_dcc_ctrl, 1);
        WRITE_PHYREG(pi, femctrl_override_control_reg, 0);

        // mclip related(sparereg_1_for_div_1x1 = mclip_agc_ENABLED)
        MOD_PHYREG(pi, moved_from_sparereg, sparereg_1_for_div_1x1,
                   phy_ac_rxgcrs_get_cap_mclip_agc_en(pi));
    } else {
        MOD_PHYREG(pi, RfseqTrigger, en_pkt_proc_dcc_ctrl, 0);
        MOD_PHYREG(pi, moved_from_sparereg, sparereg_1_for_div_1x1, 0);
    }

    if (ISSIM_ENAB(pi->sh->sih)) {
        MOD_PHYREG(pi, clip_detect_normpwr_var_mux, dont_use_clkdiv4en_for_gaindsmpen, 1);
    }

    /* Don't scale ADC_pwr with dvga, as its casuing some
       clip-failures for 80mhz at high pwrs (> -30dBm)
    */
    MOD_PHYREG(pi, clip_detect_normpwr_var_mux, en_clip_detect_adc, 1);

    if (ACMAJORREV_129_130(pi->pubpi->phy_rev)) {
        /* Include DCfixcorr setting from C-model default for 6710 */
        /* ht_pktgain_adjust = 0, wait_ht_gain_cmd_analog = 8; */
        WRITE_PHYREG(pi, ht_agc_analog_gain, 0x800);
        /* wait_ht_gain_cmd =34
         *  FIXME: 6710 BU team decide if to use this C-model optimal setting
         */
        if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, HTAGCWaitCounters, HTAgcPktgainWait, 0x10);
        } else {
            MOD_PHYREG(pi, HTAGCWaitCounters, HTAgcPktgainWait, 0x22);
        }
        /* bypass_HT_VHT_HE_STF = 1 */
        MOD_PHYREG(pi, DcFiltAddress, bypass_HT_VHT_HE_STF, 0x1);
    }

    //FIXME43684, FIXME63178
    if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        WRITE_PHYREG(pi, SyncPeakCnt, 0x107);

        MOD_PHYREG(pi, Tx11agplcpDelay, plcpdelay11ag, 0x50);
        MOD_PHYREG(pi, TxheplcpDelay, plcpdelayhe, 0x700);

        MOD_PHYREG(pi, cyclicDelayNsts1, enablePreSpatMapCDD_AXTR, 1);
        MOD_PHYREG(pi, cyclicDelayNsts1, enablePreSpatMapCDD_AXRE, 1);
        MOD_PHYREG(pi, cyclicDelayNsts1, enablePreSpatMapCDD_AXMU, 1);
        MOD_PHYREG(pi, cyclicDelayNsts1, enablePreSpatMapCDD_AXSU, 1);

        MOD_PHYREG(pi, RfBiasControl, tx_bg_pulse_val, 0);
        MOD_PHYREG(pi, RfBiasControl, rx_bg_pulse_val, 0);

        WRITE_PHYREG(pi, bfdsConfig1_spatialCoef, 0x380);
        WRITE_PHYREG(pi, bfdsConfig2_spatialCoef, 0x60a);

        /* ENABLE RIFS in 436840, all DV based on RIFS ON */
        MOD_PHYREG(pi, RxControl, RIFSEnable, 1);
    }

    if (ACMAJORREV_129(pi->pubpi->phy_rev))
        WRITE_PHYREG(pi, Rx2Spare, 0);

    if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, Tx11agplcpDelay, plcpdelay11ag, 0x280);

        /* Disable this bit for now (might be responsible for hangs in B0) */
        MOD_PHYREG(pi, demod_low_power, sensitivity_imp_11ax, 0);

        /* Disable lsig length check on 11ax detection path for 43684B0 (vht160 hang) */
        MOD_PHYREG(pi, HEDetectionConfig, lsig_length_min_hedetection, 0);

        MOD_PHYREG(pi, rx1misc_0, rstCmpPwrAftrVht, 0); /* Pulakesh */

        /* Disable rifs for 11ac & 11ax */
        MOD_PHYREG(pi, rx1misc_0, useRifsCntrOnlyVhtHe, 1);
    } else if (ACMAJORREV_51_129_130_131(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, Tx11agplcpDelay, plcpdelay11ag, 0x280);

        /* Disable rifs for 11ac & 11ax */
        MOD_PHYREG(pi, rx1misc_0, useRifsCntrOnlyVhtHe, 1);
    }

    // Temporary disable 11b/ag txerror check, see FWUCODE-3723
    if (ACMAJORREV_47_51_129_130_131(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, phytxerrorMaskReg3, miscmask, 0);
    }

    /* Enable last symbol pilot only phest (only for 20MHz/ru242 HE) */
    /* starting from 6715, i.e. phy_maj_rev >= 130, phyreg default setting is 1 already */
    if (ACMAJORREV_47(pi->pubpi->phy_rev) || ACMAJORREV_51_131(pi->pubpi->phy_rev) ||
        ACMAJORREV_129(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, DemodmodeControl, enPhestPilotOnlyForHeLastSymbol, 1);
    }

    if (ACMAJORREV_129_130(pi->pubpi->phy_rev)) {
        // Select HWRSSI for RxStatus
        MOD_PHYREG(pi, RxFrame_timeout, rxpwrstatus_format_sel, 1);
        if (ISSIM_ENAB(pi->sh->sih) && ACMAJORREV_130(pi->pubpi->phy_rev)) {
            // FIXME:For now following TCL
            MOD_PHYREG(pi, IqestCmd, hw_rssi_mode, 1);
        }
        MOD_PHYREG(pi, IqestCmd, iqMode, 0);
        MOD_PHYREG(pi, rx_iq_ctrl2, Rx_IQImb_use_tbl, 0);

        /* In real operation, this bit should be set by ucode in a dynmaic-way
         * set to 0 means we will not respond to TB frame at all
         */
        if (ISSIM_ENAB(pi->sh->sih) && ACMAJORREV_130(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, Config0_MAC_aided_trig_frame, en_mac_aided_user_info, 1);
        } else {
            MOD_PHYREG(pi, Config0_MAC_aided_trig_frame, en_mac_aided_user_info, 0);
        }

        MOD_PHYREG(pi, HESigSupportCtrlExt1, en_pktext_chk_dlmu_less_ru242, 0);
        MOD_PHYREG(pi, HESigSupportCtrlExt1, en_pktext_chk_re_ru106, 0);
    }

    if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
        /* Change IIR filter shape to solve FLATNESS issue for BW160 */
        WRITE_PHYREG(pi, txfilt160st0a1, 0xfa1);
        WRITE_PHYREG(pi, txfilt160st0a2, 0x34f);
        WRITE_PHYREG(pi, txfilt160st1a1, 0xe5c);
        WRITE_PHYREG(pi, txfilt160st1a2, 0x20c);
        WRITE_PHYREG(pi, txfilt160st2a1, 0xc8d);
        WRITE_PHYREG(pi, txfilt160st2a2, 0x10d);
        WRITE_PHYREG(pi, txfilt160finescale, 0xad);
        /* align 11ag bw160 scaling to 11ac/11ax */
        WRITE_PHYREG(pi, txfilt160dupfinescale, 0x9c);
    }
    if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, scramrandinitCTRL1, PE_scramrandinitEn, 0x1);
        MOD_PHYREG(pi, scramrandinitCTRL1, scramrandinitEn, 0x1);
        MOD_PHYREG(pi, MuMimoStaCtrl_4x4, mumimo_4x4_ml_enable, 0x1);
        WRITE_PHYREG(pi, lesiCrsDetectTimeOut, 0x10);
        MOD_PHYREG(pi, HESigSupportCtrlExt, enable_unsrate_for_ulofdma, 0x2);

        if (ISSIM_ENAB(pi->sh->sih)) {
            MOD_PHYREG(pi, IqestCmd, hw_rssi_mode, 0x1);
        }
        WRITE_PHYREG(pi, HwRssiTROffset0, 0x7);
        WRITE_PHYREG(pi, HwRssiTROffset1, 0x7);
        WRITE_PHYREG(pi, HwRssiTROffset2, 0x7);
        WRITE_PHYREG(pi, HwRssiTROffset3, 0x7);

        /* Let RxStatusWord be latched for every frame */
        MOD_PHYREG(pi, RxStatusLatchCtrl, LatchCtrl1, 0x1);

        /* setup CapTxControlPlcp cc offsets */
        wlc_axphy_dump_txctlplcp_on_reset(pi);

        /* Default wbcal capture fullrate value should be 0 instead for 6715;
           Fixed for future chips
        */
        MOD_PHYREG(pi, SdFeClkStatus, en_wbcal_capture_fullrate, 0x0);

        /* 6715A0 WAR for ULMUMIMO Nss-3 CPE corr performance
           fixed in AP_MASTER post 6715A0
        */
        MOD_PHYREG(pi, ULMU_cpecorr_alpha_scale_nss3_01,
            ULMU_cpecorr_alpha_scale_nss3_0, 0xa);
        MOD_PHYREG(pi, ULMU_cpecorr_alpha_scale_nss3_01,
            ULMU_cpecorr_alpha_scale_nss3_1, 0xa);
        MOD_PHYREG(pi, ULMU_cpecorr_alpha_scale_nss3_23,
            ULMU_cpecorr_alpha_scale_nss3_2, 0xa);
        MOD_PHYREG(pi, ULMU_cpecorr_alpha_scale_nss3_23,
            ULMU_cpecorr_alpha_scale_nss3_3, 0xa);

        /* Update kalman coeffs for high-RSSI ULMUMIMO
           test_klm.m assuming SNR=35dB PN=-45dB
           for 2x/4xLTF
             alpha = [511, 439, 394, 367, 351, 341, 333, 328,
                      324, 321, 319, 317, 315, 314, 312, 311]
             beta  = [128,  59,  36,  25,  19,  15,  13,  11,
                        9,   8,   8,   7,   6,   6,   5,   5]
           for 1xLTF
             alpha = [500, 437, 393, 367, 452, 412, 381, 360,
                      347, 338, 331, 327, 323, 320, 318, 316]
             beta  = [485, 232, 142,  99,  78,  46,  31,  22,
                       17,  14,  12,  10,   9,   8,   7,   7]
        */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PHASETRACKTBL,
                                  16, 198, 32, klm_ulmu);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PHASETRACKTBL,
                                  16, 262, 32, klm_ulmu_1x);

        // Enable HE ChanUpd gear-shift
        MOD_PHYREG(pi, ChanestCDDshift,
            dmd_chupd_use_mod_depend_mu, 0x1);
        MOD_PHYREG(pi, chanUpdate_ChLimitFraction1,
            use_gearshift_he, 0x1);
        /* After enabling dmd_chupd_use_mod_depend_mu to 1,
           Need to update following mu values to the same as zfuA
        */
        WRITE_PHYREG(pi, mu_a_mod_zf_3, 0x2222);
        WRITE_PHYREG(pi, mu_a_mod_zf_4, 0x2222);
        /*
        Update the phytbl for chanupdMutbl (total offsets 0-25),
            especially offset: 1-15
        Offset 1,3,5, and 7 are short,medium,long, and exlong
            for 1K (the first 4 bits of MSB) and
            256 QAM (the second 4 bits of MSB), respectively (ZF method)
            (55->44, 55->44,45->22, 45->22)
        Offset 9,11,13, and 15 are short,medium,long, and exlong
            for 64 (the third 4 bits of MSB) and
            16 QAM (the fourth 4 bits of MSB), respectively (ml method)
            (67->66, 67->66, 56->34, 67->34)
        Offset 8,10,12,and 14 are short,medium,long, and exlong
            for QPSK (the first 4 bits of MSB) and
            BPSK (the second 4 bits of MSB), respectively (ml method)
            (78->66, 78->66, 67->44, 67->44)
        The updated gear-shifting values are applied from the first payload symbol
        The values for offset 16-25 are kept as it is
        Offset 16-17 are for the payload symbol count that gear-shifting happens
        Offset 18-25 are for the mu extended mode for better resolutions of mu value,
            e.g. mu/512, in the future.
        */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_CHANUPDMUTBL,
            15, 1, 16, he_chanupdMutbl);

        phy_ac_rssi_mode_select(pi, 1); // 0 = agc based rssi, 1 is for hwrssi.

        // Enable knoise measure for 6715
        phy_ac_noise_knoise_enable(pi, TRUE, 0); // default mode 0

        /* Align DCOE timing with gain change */
        FOREACH_CORE(pi, core) {
            MOD_PHYREGCE(pi, dccal_control_18, core, dco_dly, 15);
        }

        // Enable drop20s phycrs holding timeout
        MOD_PHYREG(pi, drop20sCtrl4,
            count20sSymCntLimitEn, 0x1);
        MOD_PHYREG(pi, drop20sCtrl4,
            count20sSymCntLimit, 0x14);
    }

    if (ACMAJORREV_130(pi->pubpi->phy_rev) && ISSIM_ENAB(pi->sh->sih)) {
        // Enable OBSS-PD and SRP for 6715 Veloce
        phy_ac_chanmgr_sr_enable(pi, 6);

        // Enable ChUpd gearshift for 6715 Veloce
        WRITE_PHYREG(pi, chanUpdate_ChLimitFraction1, 0x776);
        WRITE_PHYREG(pi, ChanestCDDshift, 0x7600);

        // Enable subband RSSI for 6715 Veloce
        phy_ac_rssi_subbandrssi_setup(pi, 0); // default mode 0
        // Enable subband interference measure for 6715 Veloce
        phy_ac_rssi_subbandintf_enable(pi, TRUE);
        // Enable knoise measure for 6715 Veloce
        phy_ac_noise_knoise_enable(pi, TRUE, 0); // default mode 0
    }

    if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        WRITE_PHYREG(pi, cckshiftbitsRefVar, 0x404e);
        MOD_PHYREG(pi, tx_shaper_common12, txshaper_he_tb_bypass, 0x1);
    } else if (ACMAJORREV_131(pi->pubpi->phy_rev)) {
        // BCAWLAN-226584: Fix dup Tx gain
        WRITE_PHYREG(pi, txfilt160finescale, 0x90);
    }
}

static void
wlc_phy_set_reg_on_reset_majorrev2_5(phy_info_t *pi)
{
    wlc_phy_set_lowpwr_phy_reg_rev3(pi);

    if (ACMAJORREV_5(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, CRSMiscellaneousParam, bphy_pre_det_en, 0);
        if (IS_4364_3x3(pi)) {
            /* fix is to increase cckshiftbitsRefVar by +1.5dB */
            WRITE_PHYREG(pi, cckshiftbitsRefVar, 0x5b0c);
        }
    } else if ((RADIOID_IS(pi->pubpi->radioid, BCM2069_ID)) &&
        (RADIOREV(pi->pubpi->radiorev) == 0x2C) &&
        (PHY_XTAL_IS40M(pi))) {
        /* for 43569A2 use chip default setting for cckshiftbitsRefVar */
        ACPHY_REG_LIST_START
            /* Enable BPHY pre-detect */
            MOD_PHYREG_ENTRY(pi, RxControl, preDetOnlyinCS, 1)
            MOD_PHYREG_ENTRY(pi, dot11acConfig, bphyPreDetTmOutEn, 1)
            /* Disable BPHY pre-detect JIRA:SWWLAN-45198 jammer/ACI performance
            * takes a hit with predetector enabled
            */
            MOD_PHYREG_ENTRY(pi, CRSMiscellaneousParam, bphy_pre_det_en, 0)
            MOD_PHYREG_ENTRY(pi, bphyPreDetectThreshold0, ac_det_1us_min_pwr_0,
                350)
            WRITE_PHYREG_ENTRY(pi, cckshiftbitsRefVar, 32924)
        ACPHY_REG_LIST_EXECUTE(pi);
    } else {
        ACPHY_REG_LIST_START
            /* Enable BPHY pre-detect */
            MOD_PHYREG_ENTRY(pi, RxControl, preDetOnlyinCS, 1)
            MOD_PHYREG_ENTRY(pi, dot11acConfig, bphyPreDetTmOutEn, 0)
            MOD_PHYREG_ENTRY(pi, CRSMiscellaneousParam, bphy_pre_det_en, 0)
            MOD_PHYREG_ENTRY(pi, bphyPreDetectThreshold0, ac_det_1us_min_pwr_0, 350)
            WRITE_PHYREG_ENTRY(pi, cckshiftbitsRefVar, 46422)
        ACPHY_REG_LIST_EXECUTE(pi);
    }

    ACPHY_REG_LIST_START
        MOD_PHYREG_ENTRY(pi, musigb1, mu_sigbmcs6, 0x7)
        MOD_PHYREG_ENTRY(pi, musigb1, mu_sigbmcs5, 0x7)
        MOD_PHYREG_ENTRY(pi, musigb1, mu_sigbmcs4, 0x7)
        MOD_PHYREG_ENTRY(pi, musigb0, mu_sigbmcs3, 0x7)
        MOD_PHYREG_ENTRY(pi, musigb0, mu_sigbmcs2, 0x7)
        MOD_PHYREG_ENTRY(pi, musigb0, mu_sigbmcs1, 0x3)
        MOD_PHYREG_ENTRY(pi, musigb0, mu_sigbmcs0, 0x2)
    ACPHY_REG_LIST_EXECUTE(pi);
}

static void
wlc_phy_set_reg_on_reset_majorrev128(phy_info_t *pi)
{
    uint16 temp;
    uint8 core;

    /* 6878 - forcing CoreConfig to 2x2, RTL default is 4x4 */
    WRITE_PHYREG(pi, CoreConfig, 0x43);
    WRITE_PHYREG(pi, RfseqCoreActv2059, 0x3333);

    /* extra_sym_en is always ON in Aux slice */
    /* 11n packets fail when ampdu is enabled;CRDOT11ACPHY-2066 */
    if (wlapi_si_coreunit(pi->sh->physhim) == DUALMAC_AUX) {
        MOD_PHYREG(pi, ldpc_encoder, extra_sym_en, 0);
    }

    /* disable bphy core-remap */
    MOD_PHYREG(pi, HPFBWovrdigictrl, bphyNoCoreRemap, 1);

    /* extra_sym_en is always ON in Aux slice */
    /* 11n packets fail when ampdu is enabled;CRDOT11ACPHY-2066 */

    if (wlapi_si_coreunit(pi->sh->physhim) == DUALMAC_AUX) {
        MOD_PHYREG(pi, ldpc_encoder, extra_sym_en, 0);
    }

    temp = 0x2c;
    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0xe7, 16, &temp);
    temp = 0x0a;
    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0xe8, 16, &temp);

    temp = 0x4;
    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x36e, 16, &temp);
    temp = 0x4;
    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x37e, 16, &temp);

    if (!ISSIM_ENAB(pi->sh->sih)) {
        MOD_PHYREG(pi, Core1TxControl, iqSwapEnable, 1);
        MOD_PHYREG(pi, Core2TxControl, iqSwapEnable, 1);
    }

    ACPHY_REG_LIST_START
        MOD_PHYREG_ENTRY(pi, FFTSoftReset, lbsdadc_clken_ovr, 0)
        MOD_PHYREG_ENTRY(pi, clip_detect_normpwr_var_mux,
            dont_use_clkdiv4en_for_gaindsmpen, 1)
        MOD_PHYREG_ENTRY(pi, clip_detect_normpwr_var_mux, use_extclkdiv4en_for_fsen, 1)
        MOD_PHYREG_ENTRY(pi, clip_detect_normpwr_var_mux, en_clip_detect_adc, 1)
        MOD_PHYREG_ENTRY(pi, SlnaControl, inv_btcx_prisel_polarity, 1)
        MOD_PHYREG_ENTRY(pi, dccal_common, dcc_method_select, 0)
        MOD_PHYREG_ENTRY(pi, radio_pu_seq, dcc_tia_dac_method_select, 1)
    ACPHY_REG_LIST_EXECUTE(pi);

    FOREACH_CORE(pi, core) {
        MOD_PHYREGCE(pi, dccal_control_16, core, idact_bypass, 1);
        MOD_PHYREGCE(pi, dccal_control_16, core, dcoe_bypass, 1);
        MOD_PHYREGCE(pi, dccal_control_14, core, idacc_bypass, 1);
    }
    if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en) {
        wlc_phy_low_rate_adc_enable_acphy(pi, TRUE);
    }
    MOD_PHYREG(pi, CRSMiscellaneousParam, crsInpHold, 1);

    MOD_PHYREG(pi, HTSigTones, support_max_nss, pi->pubpi->phy_corenum);
    //MOD_PHYREG(pi, miscSigCtrl, brcm_11n_256qam_support, 0x1);

    FOREACH_CORE(pi, core) {
        WRITE_PHYREGCE(pi, crshighlowpowThresholdl, core, 0x454b);
        WRITE_PHYREGCE(pi, crshighlowpowThresholdu, core, 0x454b);
        WRITE_PHYREGCE(pi, crshighlowpowThresholdlSub1, core, 0x454b);
        WRITE_PHYREGCE(pi, crshighlowpowThresholduSub1, core, 0x454b);
        MOD_PHYREGCEE(pi, RxStatPwrOffset, core, use_gainVar_for_rssi, 1);
    }
    ACPHY_REG_LIST_START
        /* Doppler related fix in channel update block */
        MOD_PHYREG_ENTRY(pi, ChanestCDDshift, dmd_chupd_use_mod_depend_mu, 1)
        WRITE_PHYREG_ENTRY(pi, chanupsym2, 0x050)
        WRITE_PHYREG_ENTRY(pi, mu_a_mod_ml_4, 0x4400)
        WRITE_PHYREG_ENTRY(pi, mu_a_mod_ml_5, 0x4444)
    ACPHY_REG_LIST_EXECUTE(pi);
    if (CHSPEC_IS80(pi->radio_chanspec)) {
        ACPHY_REG_LIST_START
            MOD_PHYREG_ENTRY(pi, mu_a_mod_zf_4, mu_a_mod_zf_long_4, 0)
            MOD_PHYREG_ENTRY(pi, mu_a_mod_zf_4, mu_a_mod_zf_long_5, 0)
            MOD_PHYREG_ENTRY(pi, mu_a_mod_zf_5, mu_a_mod_zf_extra_long_4, 4)
            MOD_PHYREG_ENTRY(pi, mu_a_mod_zf_5, mu_a_mod_zf_extra_long_5, 4)
        ACPHY_REG_LIST_EXECUTE(pi);
    }

    ACPHY_REG_LIST_START
        MOD_PHYREG_ENTRY(pi, overideDigiGain1, cckdigigainEnCntValue, 119)
        MOD_PHYREG_ENTRY(pi, ADC_PreClip_Enable, small_sig_gain_indx_decr, 0)
        // Fix for 11Mbps PER hump issue in 4361B0 chips
        MOD_PHYREG_ENTRY(pi, bphyPreDetectThreshold5, log2_rho_sqr_1us_th, 0x80)
    ACPHY_REG_LIST_EXECUTE(pi);

    /* 4357B0/B1: to help reduce m4/c4s1 PER hump seen in -70 to -80 dBm RxPwr range */
    WRITE_PHYREG(pi, FSTRHiPwrTh, 0x453f);

    // Write default value of cckshiftbitsRefVar
    WRITE_PHYREG(pi, cckshiftbitsRefVar,
        ACMAJORREV_128(pi->pubpi->phy_rev) ? CCK_REF_VAR_NOR >> 1 : CCK_REF_VAR_NOR);

    ACPHY_REG_LIST_START
        /* Enabling bphypredetect timer and setting timeout to 25 microsecs */
        MOD_PHYREG_ENTRY(pi, dot11acConfig, bphyPreDetTmOutEn, 1)
        MOD_PHYREG_ENTRY(pi, dot11acConfig, bphyPreDetTmOutNegEdgEn, 0)
        WRITE_PHYREG_ENTRY(pi, bphy_pre_detection_timeout_interval, 0x3e8)
        ACPHY_REG_LIST_EXECUTE(pi);

    /* Seeing LTE inband jammer degradation with low ifs.
     * temporarily disabling it
     */
    MOD_PHYREG(pi, RxControl, RIFSEnable, 0);

    /* Disable core2core sync for 6878 */
    phy_ac_chanmgr_core2core_sync_setup(pi->u.pi_acphy->chanmgri, FALSE);

    MOD_PHYREG(pi, RxStatPwrOffset0, rx_status_pwr_offset0,
        ACMAJORREV_128(pi->pubpi->phy_rev) ? 44 : 46);
    MOD_PHYREG(pi, RxStatPwrOffset1, rx_status_pwr_offset1,
        ACMAJORREV_128(pi->pubpi->phy_rev) ? 44 : 46);

    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* set bphy_tap_20in20path_from_DCnotch_en to 1
         * to improve sensitivity for bphy-rates
         */
        MOD_PHYREG(pi, bphyFiltBypass, bphy_tap_20in20path_from_DCnotch_en, 1);

        /* reduce BPHY TargetVar to reduce input level of BPHY */
        ACPHYREG_BCAST(pi, Core0_BPHY_TargetVar_log2_pt8us, 0x1bf);
    }
}

static void
wlc_phy_set_reg_on_reset_majorrev4(phy_info_t *pi)
{
    uint16 phymode = phy_get_phymode(pi);
    uint16 rxbias, txbias;
    uint8 core;

    ACPHY_REG_LIST_START
        WRITE_PHYREG_ENTRY(pi, miscSigCtrl, 0x003)
        MOD_PHYREG_ENTRY(pi, CRSMiscellaneousParam, crsInpHold, 1)
        MOD_PHYREG_ENTRY(pi, crsThreshold2l, peakThresh, 77)
        MOD_PHYREG_ENTRY(pi, crsThreshold2u, peakThresh, 77)
        MOD_PHYREG_ENTRY(pi, crsThreshold2lSub1, peakThresh, 77)
        MOD_PHYREG_ENTRY(pi, crsThreshold2uSub1, peakThresh, 77)

        MOD_PHYREG_ENTRY(pi, crshighlowpowThresholdl, low2highpowThresh, 69)
        MOD_PHYREG_ENTRY(pi, crshighlowpowThresholdu, low2highpowThresh, 69)
        MOD_PHYREG_ENTRY(pi, crshighlowpowThresholdlSub1, low2highpowThresh, 69)
        MOD_PHYREG_ENTRY(pi, crshighlowpowThresholduSub1, low2highpowThresh, 69)

        /* XXX
         * 4349 WAR for BFE SNR too high by 12dB
         *  - Increase BFE NVAR by 12dB (16*log2 domain)
         *  - Zero out BFE nvar adj table
         *    (not being used for SU mcs recommendation, hurts MU SNR feedback)
         */
        MOD_PHYREG_ENTRY(pi, BfeConfigReg1, bfe_nvar_comp, 64)
        MOD_PHYREG_ENTRY(pi, BfeConfigNvarAdjustTblReg0, bfe_config_lut_noise_var0, 0)
        MOD_PHYREG_ENTRY(pi, BfeConfigNvarAdjustTblReg1, bfe_config_lut_noise_var1, 0)
        MOD_PHYREG_ENTRY(pi, BfeConfigNvarAdjustTblReg2, bfe_config_lut_noise_var2, 0)
        MOD_PHYREG_ENTRY(pi, BfeConfigNvarAdjustTblReg3, bfe_config_lut_noise_var3, 0)
        MOD_PHYREG_ENTRY(pi, BfeConfigNvarAdjustTblReg4, bfe_config_lut_noise_var4, 0)
        MOD_PHYREG_ENTRY(pi, BfeConfigNvarAdjustTblReg5, bfe_config_lut_noise_var5, 0)

        MOD_PHYREG_ENTRY(pi, dot11acConfig, bphyPreDetTmOutEn, 0)
        MOD_PHYREG_ENTRY(pi, bOverAGParams, bOverAGlog2RhoSqrth, 0)
        MOD_PHYREG_ENTRY(pi, CRSMiscellaneousParam, b_over_ag_falsedet_en, 1)
        WRITE_PHYREG_ENTRY(pi, cckshiftbitsRefVar, 46422)
        MOD_PHYREG_ENTRY(pi, RxStatPwrOffset0, use_gainVar_for_rssi0, 1)
        WRITE_PHYREG_ENTRY(pi, HTAGCWaitCounters, 0x1020)
        WRITE_PHYREG_ENTRY(pi, FSTRCtrl, 0x7aa)
        MOD_PHYREG_ENTRY(pi, FFTSoftReset, lbsdadc_clken_ovr, 0)
        MOD_PHYREG_ENTRY(pi, RxSdFeConfig5, rx_farow_scale_value, 7)
        MOD_PHYREG_ENTRY(pi, RxSdFeConfig5, tiny_bphy20_ADC10_sel, 0)
    ACPHY_REG_LIST_EXECUTE(pi);

    txbias = 0x2b;
    rxbias = 0x28;
    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0xe8, 16, &txbias);
    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0xe7, 16, &rxbias);

    MOD_PHYREG(pi, bphyTest, dccomp, 0);

    MOD_PHYREG(pi, RfseqCoreActv2059, DisTx, pi->pubpi->phy_coremask);
    MOD_PHYREG(pi, RfseqCoreActv2059, DisRx, pi->pubpi->phy_coremask);
    MOD_PHYREG(pi, RfseqCoreActv2059, EnTx, pi->pubpi->phy_coremask);
    MOD_PHYREG(pi, RfseqCoreActv2059, EnRx, pi->pubpi->phy_coremask);
    MOD_PHYREG(pi, CoreConfig, CoreMask, pi->pubpi->phy_coremask);
    MOD_PHYREG(pi, CoreConfig, NumRxCores, pi->pubpi->phy_corenum);
    WRITE_PHYREG(pi, HTSigTones, 0x9ee9);
    MOD_PHYREG(pi, HTSigTones, support_max_nss, pi->pubpi->phy_corenum);
    MOD_PHYREG(pi, bphyFiltBypass, bphy_tap_20in20path_from_DVGA_en, 1);
    WRITE_PHYREG(pi, femctrl_override_control_reg, 0x0);

    FOREACH_CORE(pi, core) {
        MOD_RADIO_REG_20693(pi, RX_BB_2G_OVR_EAST, core, ovr_tia_offset_comp_pwrup, 1);
        MOD_RADIO_REG_20693(pi, TIA_CFG15, core, tia_offset_comp_pwrup, 1);
    }

    // DCC related regs
    MOD_PHYREG(pi, RfseqTrigger, en_pkt_proc_dcc_ctrl, 1);

    if (phymode == PHYMODE_MIMO) {
        ACPHY_REG_LIST_START
            MOD_PHYREG_ENTRY(pi, BfeConfigReg1, bfe_config_legacyUserIndex, 0x8)
            MOD_PHYREG_ENTRY(pi, dot11acConfig, bphyPreDetTmOutEn, 1)
            MOD_PHYREG_ENTRY(pi, CRSMiscellaneousParam, crsMfMode, 1)
            WRITE_PHYREG_ENTRY(pi, fineclockgatecontrol, 0x4000)
        ACPHY_REG_LIST_EXECUTE(pi);

        /* disableML if QT and MIMO mode */
        if (ISSIM_ENAB(pi->sh->sih) || !pi->u.pi_acphy->chanmgri->ml_en) {
            MOD_PHYREG(pi, RxControl, MLenable, 0);
        }
    } else if (phymode == PHYMODE_80P80) {
        ACPHY_REG_LIST_START
            MOD_PHYREG_ENTRY(pi, crshighlowpowThresholdl1, low2highpowThresh, 69)
            MOD_PHYREG_ENTRY(pi, crshighlowpowThresholdu1, low2highpowThresh, 69)
            MOD_PHYREG_ENTRY(pi, crshighlowpowThresholdlSub11, low2highpowThresh, 69)
            MOD_PHYREG_ENTRY(pi, crshighlowpowThresholduSub11, low2highpowThresh, 69)
            MOD_PHYREG_ENTRY(pi, crsThreshold2u1, peakThresh, 77)

            MOD_PHYREG_ENTRY(pi, crsControll1, mfLessAve, 0)
            MOD_PHYREG_ENTRY(pi, crsControlu1, mfLessAve, 0)
            MOD_PHYREG_ENTRY(pi, crsControllSub11, mfLessAve, 0)
            MOD_PHYREG_ENTRY(pi, crsControluSub11, mfLessAve, 0)

            WRITE_PHYREG_ENTRY(pi, fineclockgatecontrol, 0x4000)
            MOD_PHYREG_ENTRY(pi, HTSigTones, support_max_nss, 0x1)
        ACPHY_REG_LIST_EXECUTE(pi);
    } else {
        WRITE_PHYREG(pi, fineclockgatecontrol, 0x0);
    }

    FOREACH_CORE(pi, core) {
        MOD_PHYREGCE(pi, forceFront, core, freqCor, 0);
        MOD_PHYREGCE(pi, forceFront, core, freqEst, 0);
    }

    ACPHY_REG_LIST_START
        MOD_PHYREG_ENTRY(pi, RxFeCtrl1, swap_iq1, 1)
        MOD_PHYREG_ENTRY(pi, RxFeCtrl1, swap_iq2, 0)

        /* RfseqMode mixer_1st_dis is set to 1, so mixer 1st is not enabled */
        MOD_PHYREG_ENTRY(pi, RfseqMode, mixer_first_mask_dis, 1)

        /* Doppler related fix in channel update block */
        MOD_PHYREG_ENTRY(pi, ChanestCDDshift, dmd_chupd_use_mod_depend_mu, 1)
        WRITE_PHYREG_ENTRY(pi, chanupsym2, 0x050)
        WRITE_PHYREG_ENTRY(pi, mu_a_mod_ml_4, 0x4400)
        WRITE_PHYREG_ENTRY(pi, mu_a_mod_ml_5, 0x4444)
        /* Doppler related fix in Bphy LMS update block */
        WRITE_PHYREG_ENTRY(pi, CCKLMSStepSize, 0x1)
    ACPHY_REG_LIST_EXECUTE(pi);

    /* BPHY packet gain settle time . Removes Humps. */
    MOD_PHYREG(pi, overideDigiGain1, cckdigigainEnCntValue, 175);
}

/* Initialize chip regs(RW) that get reset with phy_reset */
static void
wlc_phy_set_reg_on_reset_acphy(phy_info_t *pi)
{
    uint8 core;
    phy_ac_chanmgr_info_t *chanmgri = pi->u.pi_acphy->chanmgri;

    /* IQ Swap (revert swap happening in the radio) */
    if (!(ISSIM_ENAB(pi->sh->sih)) &&
            !(ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev)))) {
        phy_utils_or_phyreg(pi, ACPHY_RxFeCtrl1(pi->pubpi->phy_rev), 7 <<
            ACPHY_RxFeCtrl1_swap_iq0_SHIFT(pi->pubpi->phy_rev));
    }

    if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        ACMAJORREV_47(pi->pubpi->phy_rev) ||
        ACMAJORREV_130(pi->pubpi->phy_rev)) {
        if (!ISSIM_ENAB(pi->sh->sih)) {
            MOD_PHYREG(pi, RxFeCtrl1, swap_iq0, 1);
            MOD_PHYREG(pi, RxFeCtrl1, swap_iq1, 1);
            MOD_PHYREG(pi, RxFeCtrl1, swap_iq2, 1);
            MOD_PHYREG(pi, RxFeCtrl1, swap_iq3, 1);

            MOD_PHYREG(pi, Core1TxControl, iqSwapEnable, 1);
            MOD_PHYREG(pi, Core2TxControl, iqSwapEnable, 1);
            MOD_PHYREG(pi, Core3TxControl, iqSwapEnable, 1);
            MOD_PHYREG(pi, Core4TxControl, iqSwapEnable, 1);
        }
    } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
        if (!ISSIM_ENAB(pi->sh->sih)) {
            MOD_PHYREG(pi, RxFeCtrl1, swap_iq0, 1);
            MOD_PHYREG(pi, RxFeCtrl1, swap_iq1, 1);
            MOD_PHYREG(pi, RxFeCtrl1, swap_iq2, 1);

            MOD_PHYREG(pi, Core1TxControl, iqSwapEnable, 1);
            MOD_PHYREG(pi, Core2TxControl, iqSwapEnable, 1);
            MOD_PHYREG(pi, Core3TxControl, iqSwapEnable, 1);
        }
    } else if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
        if (!ISSIM_ENAB(pi->sh->sih)) {
            MOD_PHYREG(pi, RxFeCtrl1, swap_iq0, 1);
            MOD_PHYREG(pi, RxFeCtrl1, swap_iq1, 1);
            MOD_PHYREG(pi, Core1TxControl, iqSwapEnable, 1);
            MOD_PHYREG(pi, Core2TxControl, iqSwapEnable, 1);
        }
    }

    /* Avoid underflow trigger for loopback Farrow */
    MOD_PHYREG(pi, RxFeCtrl1, en_txrx_sdfeFifoReset, 1);

    /* Needed for 6878 */
    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* WAR for CRDOT11ACPHY-2664: keep interlvr clock on */
        /* Needed for 6878, Fixed for phy_maj44 */
        MOD_PHYREG(pi, forceClkOn, forceInterlvClkOn, 1);
        MOD_PHYREG(pi, RX1_Clock_Root_Gating_Control, usePhy1ClkDivAlign, 0);

        /* Disable GF detect logic, causing issue with -8dBm ACI power */
        MOD_PHYREG(pi, miscSigCtrl, force_1st_sigqual_bpsk, 1);

        /* HW4347BU-177 Update bphy predetect settings */
        MOD_PHYREG(pi, RxControl, preDetOnlyinCS, 1);
        MOD_PHYREG(pi, BphyControl8, dinonRstTglScheme, 1);
    }

    if (ACMAJORREV_129(pi->pubpi->phy_rev) &&
               pi->u.pi_acphy->sromi->srom_low_adc_rate_en) {
        MOD_PHYREG(pi, RxFeCtrl1, rxfe_bilge_cnt, 8);
    } else {
        MOD_PHYREG(pi, RxFeCtrl1, rxfe_bilge_cnt, 4);
    }

#ifdef WL_MU_RX
    if (ACMAJORREV_47_129_130(pi->pubpi->phy_rev)) {
        //FIXME43684
        MOD_PHYREG(pi, miscSigCtrl, mu_enable, 0);
        MOD_PHYREG(pi, miscSigCtrl, check_vht_siga_valid_mu, 1);
    } else if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
        //FIXME63178
        MOD_PHYREG(pi, miscSigCtrl, mu_enable, 1);
        MOD_PHYREG(pi, miscSigCtrl, check_vht_siga_valid_mu, 1);
    } else {
        MOD_PHYREG(pi, miscSigCtrl, mu_enable, 1);
        MOD_PHYREG(pi, miscSigCtrl, check_vht_siga_valid_mu, 0);
    }
#endif /* WL_MU_RX */

    MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, 1);
    MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, 0);

    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        /* 4349 */
        phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

        wlc_phy_set_spareReg_on_reset_majorrev4(pi, stf_shdata->phyrxchain);

        MOD_PHYREG(pi, overideDigiGain1, cckdigigainEnCntValue, 0x6E);
    }

    /* Enable 6-bit Carrier Sense Match Filter Mode for 4335C0 and 43602A0 */
    if ((ACMAJORREV_2(pi->pubpi->phy_rev) && !ACMINORREV_0(pi)) ||
        ACMAJORREV_5(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, CRSMiscellaneousParam, crsMfMode, 1);
    }

    WRITE_PHYREG(pi, dot11acphycrsTxExtension, chanmgri->crsTxExtension);

    /* Currently PA turns on 1us before first DAC sample. Decrease that gap to 0.5us */
    if ((ACMAJORREV_0(pi->pubpi->phy_rev)) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
            WRITE_PHYREG(pi, TxRealFrameDelay, 146);
    }

    /* This number combined with MAC RIFS results in 2.0us RIFS air time */
    WRITE_PHYREG(pi, TxRifsFrameDelay, 48);
    si_core_cflags(pi->sh->sih, SICF_MPCLKE, SICF_MPCLKE);

    if (RADIOID_IS(pi->pubpi->radioid, BCM20693_ID)) {
        wlc_phy_force_rfseq_acphy(pi, ACPHY_RFSEQ_RESET2RX);
    }

    /* allow TSSI loopback path to turn off */
    wlc_phy_set_tssiSleep_on_reset(pi);

    /* In event of high power spurs/interference that causes crs-glitches,
       stay in WAIT_ENERGY_DROP for 1 clk20 instead of default 1 ms.
       This way, we get back to CARRIER_SEARCH quickly and will less likely to miss
       actual packets. PS: this is actually one settings for ACI
    */
    /* WRITE_PHYREG(pi, ACPHY_energydroptimeoutLen, 0x2); */

    /* Upon Reception of a High Tone/Tx Spur, the default 40MHz MF settings causes ton of
       glitches. Set the MF settings similar to 20MHz uniformly. Provides Robustness for
       tones (on-chip, on-platform, accidential loft coming from other devices)
    */
    wlc_phy_set_mfLessAve_on_reset(pi);

    wlc_phy_set_peakThresh_on_reset(pi);

    if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        /* 4365 */
        wlc_phy_set_reg_on_reset_majorrev32_33_37_47_51_129_130_131(pi);
    } else if (ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
        /* 4350 and 43602 */
        wlc_phy_set_reg_on_reset_majorrev2_5(pi);
    } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* 6878 */
        FOREACH_CORE(pi, core) {
            /* No limit for now on max analog gain */
            MOD_PHYREGC(pi, HpFBw, core, maxAnalogGaindb, 100);
            MOD_PHYREGC(pi, DSSScckPktGain, core, dsss_cck_maxAnalogGaindb, 100);
        }
    }

    /*  Write 0x0 to RfseqMode to turn off both CoreActv_override
     * (to give control to Tx control word) and Trigger_override (to give
     * control to rfseq)
     *
     * Now we are done with all rfseq INIT.
     * PR109510 : don't need to touch rfpll, pllldo in reset2rx (bit 2, powerOnReset2RxSeq)
    */
    WRITE_PHYREG(pi, RfseqMode, 0);

    /* Disable Viterbi cache-hit low power featre for 4360
     * since it is hard to meet 320 MHz timing
     */
    MOD_PHYREG(pi, ViterbiControl0, CacheHitEn, ACMAJORREV_0(pi->pubpi->phy_rev)
                ? 0 : 1);

    /* WAR: Enable timeout for 6878 for paydecode pktproc hang state */
    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        ACPHY_REG_LIST_START
            WRITE_PHYREG_ENTRY(pi, ofdmpaydecodetimeoutlen, 0x7d0)
            /* 2000 * 25 us = 50 ms */
            WRITE_PHYREG_ENTRY(pi, cckpaydecodetimeoutlen, 0x7d0)
            /* 2000 * 25 us = 50 ms */
            WRITE_PHYREG_ENTRY(pi, nonpaydecodetimeoutlen, 0x20)
            /* 32 * 25 us = 800 us */
            WRITE_PHYREG_ENTRY(pi, timeoutEn, 0x81f)
            /* ofdmpaydecodetimeoutEn = 1
            * cckpaydecodetimeoutEn = 1
            * nonpaydecodetimeoutEn = 1
            * resetCCAontimeout = 1
            * resetRxontimeout = 1
            */
        ACPHY_REG_LIST_EXECUTE(pi);
    }
    /* and eventually crashes in reset2rx spin wait. resetcca is getting called as */
    /* part of set channel */
    if (!IS_4364_3x3(pi)) {
        /* Reset pktproc state and force RESET2RX sequence */
        wlc_phy_resetcca_acphy(pi);
    }

    /* Make TSSI to select Q-rail */
    MOD_PHYREG(pi, TSSIMode, tssiADCSel,
        (ACMAJORREV_GE40(pi->pubpi->phy_rev) &&
        pi->u.pi_acphy->sromi->srom_low_adc_rate_en) ? 0 : 1);

    MOD_PHYREG(pi, HTSigTones, support_gf, 0);

    /* JIRA-CRDOT11ACPHY-273: SIG errror check For number of VHT symbols calculated */
    MOD_PHYREG(pi, partialAIDCountDown, check_vht_siga_length, 1);

    MOD_PHYREG(pi, DmdCtrlConfig, check_vhtsiga_rsvd_bit, 0);

    FOREACH_CORE(pi, core) {
        MOD_PHYREGCE(pi, forceFront, core, freqCor, 1);
        MOD_PHYREGCE(pi, forceFront, core, freqEst, 1);
        /* Use Gain variance for RSSI estimation */
        MOD_PHYREGCEE(pi, RxStatPwrOffset, core, use_gainVar_for_rssi, 1);
    }

#ifdef OCL
    if (TINY_RADIO(pi)) {
        if (PHY_OCL_ENAB(pi->sh->physhim)) {
            uint16 regval = 0x82c0;
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x15b, 16, &regval);
        }
    }
#endif
    if (ROUTER_4349(pi)) {
        /* JIRA: SWWLAN-91943
         * Issue: pktgainSettleLen of 0x30 (1.2us) is introducing PER humps at ~-62dBm
         * Fix: Increasing the pktgainSettleLen to 0x40 (1.6us)
         */
        WRITE_PHYREG(pi, pktgainSettleLen, 0x40);
    } else if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
        /* BCAWLAN-205955: [47622] PER spike seen at sliceB core1 at -38dBm RSSI */
        /* Keep pktgainSettleLen default value */
        WRITE_PHYREG(pi, pktgainSettleLen, 0x40);
    } else {
        /* PR108248 : 1.6us pktgainSettleLen causes late/missed STR and pkt end in
         * glitches (esp for 20in80, 40in 80, single core),
         * 1 us should be enough time for pktgain to settle
         */
        WRITE_PHYREG(pi, pktgainSettleLen, 0x30);
    }

    /* tkip macdelay & mac holdoff */
    if (ACMAJORREV_47_129(pi->pubpi->phy_rev)) {
        WRITE_PHYREG(pi, TxMacIfHoldOff, TXMAC_IFHOLDOFF_43684B0);
        WRITE_PHYREG(pi, TxMacDelay, TXMAC_MACDELAY_DEFAULT);
    } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, TxMacIfHoldOff, holdoffval, TXMAC_IFHOLDOFF_6715);
        WRITE_PHYREG(pi, TxMacDelay, TXMAC_MACDELAY_DEFAULT);
    } else if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
        WRITE_PHYREG(pi, TxMacIfHoldOff, TXMAC_IFHOLDOFF_63178);
        WRITE_PHYREG(pi, TxMacDelay, TXMAC_MACDELAY_DEFAULT);
    } else if (!ACMAJORREV_0(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        WRITE_PHYREG(pi, TxMacIfHoldOff, TXMAC_IFHOLDOFF_DEFAULT);
        WRITE_PHYREG(pi, TxMacDelay, TXMAC_MACDELAY_DEFAULT);
    }

    /* tiny radio specific processing */
    if (TINY_RADIO(pi)) {
        if (RADIOID_IS(pi->pubpi->radioid, BCM20693_ID))
            wlc_phy_set_reg_on_reset_acphy_20693(pi);
    }

    wlc_phy_mlua_adjust_acphy(pi, phy_btcx_is_btactive(pi->btcxi));
#ifndef WLC_DISABLE_ACI
    /* Setup HW_ACI block */

    if (!ACPHY_ENABLE_FCBS_HWACI(pi)) {
        if (((pi->sh->interference_mode_2G & ACPHY_ACI_HWACI_PKTGAINLMT) != 0) ||
            ((pi->sh->interference_mode_5G & ACPHY_ACI_HWACI_PKTGAINLMT) != 0) ||
            ((((pi->sh->interference_mode_2G & ACPHY_HWACI_MITIGATION) != 0) ||
             ((pi->sh->interference_mode_5G & ACPHY_HWACI_MITIGATION) != 0)) &&
                !(ACMAJORREV_32(pi->pubpi->phy_rev) ||
                ACMAJORREV_33(pi->pubpi->phy_rev) ||
                (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                !ACMAJORREV_128(pi->pubpi->phy_rev)))))
            wlc_phy_hwaci_setup_acphy(pi, FALSE, TRUE);
        else
            wlc_phy_hwaci_setup_acphy(pi, FALSE, FALSE);
    }
#endif /* !WLC_DISABLE_ACI */

    /* 43602: C-Model Parameters setting */
    if (ACMAJORREV_5(pi->pubpi->phy_rev)) {
        ACPHY_REG_LIST_START
            /* Turn ON 11n 256 QAM in 2.4G */
            WRITE_PHYREG_ENTRY(pi, miscSigCtrl, 0x203)
            WRITE_PHYREG_ENTRY(pi, HTAGCWaitCounters, 0x1028)

            /* WRITE_PHYREG_ENTRY(pi, bfeConfigReg1, 0x8) */

            WRITE_PHYREG_ENTRY(pi, crsThreshold2lSub1, 0x204d)
            WRITE_PHYREG_ENTRY(pi, crsThreshold2uSub1, 0x204d)

            /* Fine timing optimization for linear filter */
            WRITE_PHYREG_ENTRY(pi, FSTRCtrl, 0x7aa)
        ACPHY_REG_LIST_EXECUTE(pi);
    }

    if (ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
        /* Low_power settings */
        WRITE_PHYREG(pi, RxFeTesMmuxCtrl, 0x60);
        /* Commenting out this low-power feature. Seen performance hit because of it.  */
        /* FOREACH_CORE(pi, core) { */
        /*     MOD_PHYREGCE(pi, forceFront, core, freqCor, 0); */
        /*     MOD_PHYREGCE(pi, forceFront, core, freqEst, 0); */
        /* } */
    }

    if (ACMAJORREV_2(pi->pubpi->phy_rev)) {
        /* Helps for OFDM high-end hump due to W1 clamping */
        /* WRITE_PHYREG(pi, pktgainSettleLen, 0x33); */
        /* WRITE_PHYREG(pi, defer_setClip2_CtrLen, 13); */
        WRITE_PHYREG(pi, dssscckgainSettleLen, 0x65);
    }

    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        wlc_phy_set_reg_on_reset_majorrev128(pi);
    }

    /* enable fix for bphy loft calibration issue CRDOT11ACPHY-378 */
    if (ACREV_GE(pi->pubpi->phy_rev, 6))
        MOD_PHYREG(pi, bphyTest, bphyTxfiltTrunc, 0);

    ACPHY_REG_LIST_START
        /* for: http://jira.broadcom.com/browse/SWWFA-10  */
        WRITE_PHYREG_ENTRY(pi, drop20sCtrl1, 0xc07f)

        /* phyrcs20S drop threshold -110 dBm */
        WRITE_PHYREG_ENTRY(pi, drop20sCtrl2, 0x64)
    ACPHY_REG_LIST_EXECUTE(pi);

        /* phyrcs40S drop threshold -110 dBm */
    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        MOD_PHYREG(pi, drop20sCtrl3, phycrs40SpwrTh, 0x64);
    } else {
        WRITE_PHYREG(pi, drop20sCtrl3, 0x64);
    }

    /* phyrcs20s symbol count threshold */
    if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, drop20sCtrl4, count20sSymCntLimitEn, 0x1);
        MOD_PHYREG(pi, drop20sCtrl4, count20sSymCntLimit, 0x14);
    }

    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        /* 4349 */
        wlc_phy_set_reg_on_reset_majorrev4(pi);
    }

    if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        /* phy_tx_active_cores {0 1 2 3} */
        MOD_PHYREG(pi, CoreConfig, CoreMask,   pi->pubpi->phy_coremask);
        MOD_PHYREG(pi, CoreConfig, NumRxCores, pi->pubpi->phy_corenum);

        /* RfseqMode mixer_1st_dis is set to 1, so mixer 1st is not enabled */
        MOD_PHYREG(pi, RfseqMode, mixer_first_mask_dis, 1);
    }

    /* 4360 & 43602 & 4365 & 7271 */
    if (ACMAJORREV_0(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev) ||
        ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        /* Increase timing search timeout to reduce number of glitches under 64k,
           if glitches > 64k, they are wrapped around to a low number. 12 us --> 13 us
        */
            WRITE_PHYREG(pi, timingsearchtimeoutLen, 520);   /* 13 / 0.025 */
    }

    if (IS_4364_3x3(pi) && CHSPEC_IS2G(pi->radio_chanspec))
        MOD_PHYREG(pi, crsThreshold1u, autoThresh, 240);

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* JIRA:CRDOT11ACPHY-2437
           Do not reset ADC clock divider at reset2rx as it can lead to a deadlock,
           where Rx FIFO is empty, generating Rx stall and ADC is not filling RxFIFO
           due to divider held in reset
        */
        MOD_PHYREG(pi, Logen_AfeDiv_reset_select, rst2rx_afediv_arst_val, 0);
    }

#ifdef WL_MU_RX
    if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
        ACPHY_REG_LIST_START
            MOD_PHYREG_ENTRY(pi, mu_a_mumimo_ltrn_dependent,
                    mu_a_mumimo_ltrn_dependent_log2_1, 0x6)
            MOD_PHYREG_ENTRY(pi, mu_a_mumimo_ltrn_dependent,
                    mu_a_mumimo_ltrn_dependent_log2_0, 0x7)
            MOD_PHYREG_ENTRY(pi, miscSigCtrl, mu_enable, 1)
            MOD_PHYREG_ENTRY(pi, miscSigCtrl, check_vht_siga_valid_mu, 1)
        ACPHY_REG_LIST_EXECUTE(pi);
    } else if (ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        /* MU-MIMO STA related stuff */
        if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
            //FIXME43684
            MOD_PHYREG(pi, mu_a_mumimo_ltrn_dependent,
                mu_a_mumimo_ltrn_dependent_log2_1, 0x6);
            MOD_PHYREG(pi, mu_a_mumimo_ltrn_dependent,
                mu_a_mumimo_ltrn_dependent_log2_0, 0x7);
        } else {
            MOD_PHYREG(pi, mu_a_mumimo_ltrn_dependent,
                mu_a_mumimo_ltrn_dependent_log2_1, 0x0);
            MOD_PHYREG(pi, mu_a_mumimo_ltrn_dependent,
                mu_a_mumimo_ltrn_dependent_log2_0, 0x0);
        }
        ACPHY_REG_LIST_START
            /* XXX Enable MU MIMO rx capability, disable MU-specific SIGA decode
             * checks
             */
            MOD_PHYREG_ENTRY(pi, miscSigCtrl, mu_enable, 1)
            MOD_PHYREG_ENTRY(pi, miscSigCtrl, check_vht_siga_valid_mu, 0)
            /* XXX Improved MU-STA phy settings on 4365 for non-ZF AP
             * JIRA SWWLAN-115593.
             */
            MOD_PHYREG_ENTRY(pi, MuSumThreshold4x4_0, IntSumThresh2, 0x80)
            MOD_PHYREG_ENTRY(pi, MuMimoStaCtrl_4x4,
                mumimo_4x4_dummystreamdetect_enable, 0x1)
            MOD_PHYREG_ENTRY(pi, MuMimoStaCtrl_4x4, mumimo_4x4_all_stream_equalize, 0x1)
            MOD_PHYREG_ENTRY(pi, MuMimoStaCtrl_4x4, mumimo_skip_datasc_for_phest, 0x1)
        ACPHY_REG_LIST_EXECUTE(pi);
        if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
            //FIXME43684
            MOD_PHYREG(pi, miscSigCtrl, mu_enable, 0);
            MOD_PHYREG(pi, miscSigCtrl, check_vht_siga_valid_mu, 1);
        }
    }
#endif /* WL_MU_RX */

#if defined(WL_MU_TX) && defined(WLVASIP)
    /* Initialize MUMIMO related phyreg.
     * 4366 initialize them in VASIP firmware
     */
    if (ACMAJORREV_47_51_129_130_131(pi->pubpi->phy_rev)) {
        WRITE_PHYREG(pi, m2v_svmpaddr0,
            (uint16)SVMPADDR2VC(wlapi_get_svmp_addr_m2v_buf(pi->sh->physhim, 0)));
        WRITE_PHYREG(pi, m2v_svmpaddr1,
            (uint16)SVMPADDR2VC(wlapi_get_svmp_addr_m2v_buf(pi->sh->physhim, 1)));
        WRITE_PHYREG(pi, bfdrBaseAddr,
            (uint16)SVMPADDR2VC(wlapi_get_svmp_addr_bfd_buffer(pi->sh->physhim, 0)));
        WRITE_PHYREG(pi, bfdrCqiBaseAddr,
            (uint16)SVMPADDR2VC(wlapi_get_svmp_addr_bfd_buffer(pi->sh->physhim, 1)));
        WRITE_PHYREG(pi, bfdsLogAddr,
            (uint16)SVMPADDR2VC(wlapi_get_svmp_addr_bfd_buffer(pi->sh->physhim, 2)));
        WRITE_PHYREG(pi, bfdsMlbfBaseAddr,
            (uint16)SVMPADDR2VC(wlapi_get_svmp_addr_bfd_buffer(pi->sh->physhim, 3)));
        WRITE_PHYREG(pi, vasip_wdog_timeout_H, 0x40);

    }
    if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        /* Initialize hwsch log addr */
        WRITE_PHYREG(pi, schLogAddr,
            (uint16)SVMPADDR2VC(wlapi_get_svmp_addr_hwsch_cmd_log(pi->sh->physhim)));

        /* Initialize hwsch v2m response addr */
        WRITE_PHYREG(pi, schdestAddr,
            (uint16)SVMPADDR2VC(wlapi_get_svmp_addr_hwsch_v2m_buf(pi->sh->physhim)));
    }
#endif /* WL_MU_TX && WLVASIP */

    if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
        /* XXX
        * SWWLAN-185611:[43684A0/B0] WAR for bug HW43684-520
        * - Clock-gating bug in pre-correction block (UL OFDMA STA) can cause
        *   core2core sync to break when precorrection is enabled
        */
        if (ACMINORREV_LE(pi, 2)) { // Fixed in C0
            MOD_PHYREG(pi, phase_word_3p2_hi, upconv_11ax_forceClkOn, 1);
        }

        MOD_PHYREG(pi, bfdrConfig, numSlots, 0xFF);
        MOD_PHYREG(pi, bfdsConfig, enableWiderNDP, 1);
        /* Enable MRC SIG QUAL for 43684B0 */
        if (!ACMINORREV_0(pi))
            MOD_PHYREG(pi, MrcSigQualControl0, enableMrcSigQual, 0x1);
    } else if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
        /* Enable MRC SIG QUAL */
        MOD_PHYREG(pi, MrcSigQualControl0, enableMrcSigQual, 0x1);
    } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
        /* Enable Wider NDP feature */
        MOD_PHYREG(pi, bfdsConfig, enableWiderNDP, 1);

        MOD_PHYREG(pi, MrcSigQualControl0, enableMrcSigQual, 0x1);

        // Enable SOI centric mclip AGC
        MOD_PHYREG(pi, norm_var_hyst_th_pt8us, mClpSOICentric, 1);

        // Reduce bphy targetVar to avoid PER humps
        ACPHYREG_BCAST(pi, Core0_BPHY_TargetVar_log2_pt8us, 0x19f);
    } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, bfdsConfig, enableWiderNDP, 1);

        // Enable MrcSigQual for 6715
        MOD_PHYREG(pi, MrcSigQualControl0, enableMrcSigQual, 0x1);

        // Enable SOI centric mclip AGC
        MOD_PHYREG(pi, norm_var_hyst_th_pt8us, mClpSOICentric, 1);

        // Reduce bphy targetVar to avoid PER humps
        ACPHYREG_BCAST(pi, Core0_BPHY_TargetVar_log2_pt8us, 0x19f);

        // FIX for - rxchain change(rx2tx & tx2rx) causing long settling(high DC)
        FOREACH_CORE(pi, core) {
          MOD_PHYREGCE(pi, RfctrlCoreTxPus, core, lpf_bq2_pu, 1);
          MOD_PHYREGCE(pi, RfctrlOverrideTxPus, core, lpf_bq2_pu, 1);
          MOD_PHYREGCE(pi, RfctrlCoreLogenBias, core, tia_bias_pu, 1);
          MOD_PHYREGCE(pi, RfctrlOverrideLogenBias, core, tia_bias_pu, 1);
          MOD_PHYREGCE(pi, RfctrlCoreTxPus, core, lpf_bq1_pu, 1);
          MOD_PHYREGCE(pi, RfctrlOverrideTxPus, core, lpf_bq1_pu, 1);
        }
    }

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, bfdsCmdSeq, use_bfd_spexp, 1);
        MOD_PHYREG(pi, dacClkCtrl, vasipClkEn, 1);
    }

    // disable HeSigA check, enable only BW check; FWUCODE-3404
    if (ACMAJORREV_47_51_129_130_131(pi->pubpi->phy_rev)) {
        WRITE_PHYREG(pi, Config1_MAC_aided_trig_frame, 0x3);
    }

    // change the CSD for 2SS case to [0 3 1 5]
    if (ACMAJORREV_47_51_129_130_131(pi->pubpi->phy_rev)) {
        WRITE_PHYREG(pi, bfdsConfig3_spatialCoef, 0xC0);
        WRITE_PHYREG(pi, bfdsConfig4_spatialCoef, 0x141);
        WRITE_PHYREG(pi, bfdsConfig5_spatialCoef, 0xC0);
        WRITE_PHYREG(pi, bfdsConfig6_spatialCoef, 0x141);
    }

    /* change per_user_rssi regs */
    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev) && !ACMAJORREV_130(pi->pubpi->phy_rev)) {
        /* RTL default value was off by 3dB  for 40MHz */
        WRITE_PHYREG(pi, per_user_rssi_bw_offset_dB, 0x369);
        /* constant_qdB = 187
           disable digGain_comp for 43684 and 63178 due to HW bug
           6710 and 6715 have the bug fixed, can enable digGain_comp seperately
        */
        WRITE_PHYREG(pi, per_user_rssi_constant_qdB, 0xBB);
    } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        /* constant_qdB = 187 based on real chip expr;
           enabled digGain_comp by default for 6715
           Note: need to be tuned later.
        */
        MOD_PHYREG(pi, per_user_rssi_constant_qdB, per_user_rssi_constant_qdB, 0xBB);
    }

    /* IPA heat transient issue: keep the PAD online all the time to improve TxEVM floor
     * Require to reset PAD settings after wl up.
     */
    if (PHY_PAPDEN(pi) && phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->keep_pad_on) {
        phy_ac_chanmgr_pad_online_enable(pi, TRUE, FALSE);
    }
}

/* Initialize chip tbls(reg-based) that get reset with phy_reset */
static void
wlc_phy_set_tbl_on_reset_acphy(phy_info_t *pi)
{
    uint8 stall_val, ctr, sz;
    bool low_rate;
    phy_info_acphy_t *pi_ac;
    phy_ac_radio_data_t *rdata;
    uint16 adc_war_val = 0x20, pablowup_war_val = 120;
    uint8 core;
    uint16 gmult20, gmult40, gmult80;
    uint16 rfseq_bundle_tssisleep48[3];
    uint16 rfseq_bundle_48[3];
    const void *data, *dly;
    uint16 x;
    uint8 zeros[ZEROS_TBL_SZ * 4];
    uint16 rfseq_lpf_pu_val = 0x500e, rfseq_lpf_pu_addr[] = {0x14d, 0x15d, 0x16d, 0x48d};
    uint16 rfseq_dacdiode_val = 0x49, rfseq_dacdiode_addr[] = {0x3cb, 0x3db, 0x3eb, 0x56b};
    uint16 bq2_gain = 2; // according to RF's recommendation
    uint16 lpf_bq2_gaincomb_addr[] = {0x17e, 0x18e, 0x19e, 0x51e};
    uint16 lpf_bq2_gaincomb_val = (bq2_gain | bq2_gain << 3 | bq2_gain << 6);
#ifdef WLVASIP
    int idx;
    uint32 tmp32;
    uint32 svmp_addr, mem_addr;
    uint32 bfdLut32[4];
    uint16 bfdLut16[2];
    uint16 mem_size;
    uint16 num_cqi_rpts, bfdLut_cqi_step, num_decmp_rpts;
    const uint32 bfdLut_128slots[] = {0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
    const uint32 bfdLut_255slots[] = {0xfffffffe, 0xffffffff, 0xffffffff, 0xffffffff,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff};
#endif /* WLVASIP */

    /* uint16 AFEdiv_read_val = 0x0000; */

    bool ext_pa_ana_2g =  ((BOARDFLAGS2(GENERIC_PHY_INFO(pi)->boardflags2) &
        BFL2_SROM11_ANAPACTRL_2G) != 0);
    bool ext_pa_ana_5g =  ((BOARDFLAGS2(GENERIC_PHY_INFO(pi)->boardflags2) &
        BFL2_SROM11_ANAPACTRL_5G) != 0);

    /* DEBUG: TEST CODE FOR SS PTW70 DEBUG */
    uint8 phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;
    BCM_REFERENCE(phyrxchain);

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    ACPHY_DISABLE_STALL(pi);
    pi_ac = pi->u.pi_acphy;
    rdata = phy_ac_radio_get_data(pi_ac->radioi);
    /* Load settings related to antswctrl if not on QT */
    if (!ISSIM_ENAB(pi->sh->sih)) {
        wlc_phy_set_regtbl_on_femctrl(pi);
    }
    /* Quickturn only init */
    if (ISSIM_ENAB(pi->sh->sih)) {
        uint8 ncore_idx;
        uint16 val;
        uint16 init_gain_code_A = 0x16a, init_gain_code_B = 0x24;

        if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
            val = 64;
        } else {
           /* changing to TCL value */
           val = 50;
           if (CHSPEC_IS20(pi->radio_chanspec)) {
              MOD_PHYREG(pi, DcFiltAddress, dcBypass, 1);
           }
        }

        FOREACH_CORE(pi, ncore_idx) {
            wlc_phy_set_tx_bbmult_acphy(pi, &val, ncore_idx);
        }

        /* dummy call to satisfy compiler */
        wlc_phy_get_tx_bbmult_acphy(pi, &val, 0);

        /* on QT: force the init gain to allow noise_var not limiting 256QAM performance */
        ACPHYREG_BCAST(pi, Core0InitGainCodeA, init_gain_code_A);
        ACPHYREG_BCAST(pi, Core0InitGainCodeB, init_gain_code_B);

        FOREACH_ACTV_CORE(pi, phyrxchain, core) {
            if (core == 3) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                    0x509, 16, &qt_rfseq_val1[core]);
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                    0x506, 16, &qt_rfseq_val2[core]);
            } else {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                    0xf9 + core, 16, &qt_rfseq_val1[core]);
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                    0xf6 + core, 16, &qt_rfseq_val2[core]);
            }
        }
    }

    /* Update gmult, dacbuf after radio init */
    /* Tx Filters */
    if (!(ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        /* 7271A0 has these not connected via RFSEQ */
        wlc_phy_set_analog_tx_lpf(pi, 0x1ff, -1, -1, -1, rdata->rccal_gmult,
                                  rdata->rccal_gmult_rc, -1);
        wlc_phy_set_tx_afe_dacbuf_cap(pi, 0x1ff, rdata->rccal_dacbuf, -1, -1);
    }

    /* Rx Filters */
    if (ACMAJORREV_0(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
        /* 4360 (tighten rx analog filters). Note than 80mhz filter cutoff
           was speced at 39mhz (should have been 38.5)
           C-model desired bw : {9, 18.5, 38.5}  @ 3dB cutoff
           lab-desired (freq offset + 5%PVT): {9.5, 20, 41}
           with gmult = 193 (in 2069_procs.tcl), we get {11, 23.9, 48.857}
           Reduce bw by factor : {9.5/11, 20/23.9, 41/48.857} = {0.863, 0.837, 0.839}
        */
        gmult20 = (rdata->rccal_gmult * 221) >> 8;     /* gmult * 0.863 */
        gmult40 = (rdata->rccal_gmult * 215) >> 8;     /* gmult * 0.839 (~ 0.837) */
        gmult80 = (rdata->rccal_gmult * 215) >> 8;     /* gmult * 0.839 */
    } else {
        gmult20 = rdata->rccal_gmult;
        gmult40 = rdata->rccal_gmult;
        gmult80 = rdata->rccal_gmult;
    }

    if (!(ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        wlc_phy_set_analog_rx_lpf(pi, 1, -1, -1, -1, gmult20,
            rdata->rccal_gmult_rc, -1);
    }

    wlc_phy_set_analog_rx_lpf(pi, 2, -1, -1, -1, gmult40, rdata->rccal_gmult_rc, -1);
    wlc_phy_set_analog_rx_lpf(pi, 4, -1, -1, -1, gmult80, rdata->rccal_gmult_rc, -1);

    /* Reset2rx sequence */
    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        data = rfseq_majrev4_reset2rx_cmd;
        dly = rfseq_majrev4_reset2rx_dly;
    } else if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
        data = rfseq_majrev32_reset2rx_cmd;
        dly = rfseq_majrev32_reset2rx_dly;
    } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        data = rfseq_majrev128_reset2rx_cmd;
        dly = rfseq_majrev128_reset2rx_dly;
    } else if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        data = rfseq_majrev47_reset2rx_cmd;
        dly = rfseq_majrev47_reset2rx_dly;
    } else {
        data = rfseq_reset2rx_cmd;
        dly = rfseq_reset2rx_dly;
    }
    if (data && dly) {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x20, 16, data);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x90, 16, dly);
    }

    if (!(ACMAJORREV_32(pi->pubpi->phy_rev)) &&
        !(ACMAJORREV_33(pi->pubpi->phy_rev)) &&
        !(ACMAJORREV_GE40(pi->pubpi->phy_rev))) {
        /* during updateGainL make sure the lpf/tia hpc corner is set
            properly to optimum setting
        */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 2, 0x121, 16,
                rfseq_updl_lpf_hpc_ml);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 2, 0x131, 16,
                rfseq_updl_lpf_hpc_ml);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 2, 0x124, 16,
                rfseq_updl_tia_hpc_ml);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 2, 0x137, 16,
                rfseq_updl_tia_hpc_ml);
    }

    /* tx2rx/rx2tx: Remove SELECT_RFPLL_AFE_CLKDIV/RESUME as we are not in boost mode */
    if (ACMAJORREV_2(pi->pubpi->phy_rev) && PHY_IPA(pi)) {
        if ((CHSPEC_IS2G(pi->radio_chanspec) &&
            (((CHSPEC_IS20(pi->radio_chanspec)) &&
            (pi->u.pi_acphy->chanmgri->cfg->srom_tssisleep_en & 0x1)) ||
            ((CHSPEC_IS40(pi->radio_chanspec)) &&
            (pi->u.pi_acphy->chanmgri->cfg->srom_tssisleep_en & 0x2)))) ||
            (CHSPEC_ISPHY5G6G(pi->radio_chanspec) &&
            (((CHSPEC_IS20(pi->radio_chanspec)) &&
            (pi->u.pi_acphy->chanmgri->cfg->srom_tssisleep_en & 0x4)) ||
            ((CHSPEC_IS40(pi->radio_chanspec)) &&
            (pi->u.pi_acphy->chanmgri->cfg->srom_tssisleep_en & 0x8)) ||
            ((CHSPEC_IS80(pi->radio_chanspec)) &&
            (pi->u.pi_acphy->chanmgri->cfg->srom_tssisleep_en & 0x10))))) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
                    16, rfseq_rx2tx_cmd_withtssisleep);
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x70,
                    16, rfseq_rx2tx_dly_withtssisleep);
                MOD_PHYREG(pi, RfBiasControl, tssi_sleep_bg_pulse_val, 1);
                MOD_PHYREG(pi, AfePuCtrl, tssiSleepEn, 1);
                rfseq_bundle_tssisleep48[0] = 0x0000;
                rfseq_bundle_tssisleep48[1] = 0x20;
                rfseq_bundle_tssisleep48[2] = 0x0;
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 0, 48,
                    rfseq_bundle_tssisleep48);
        } else {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
                16, rfseq_rx2tx_cmd);
        }
    } else if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00, 16,
                                  tiny_rfseq_rx2tx_tssi_sleep_cmd);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 112, 16,
                                  tiny_rfseq_rx2tx_tssi_sleep_dly);
        MOD_PHYREG(pi, AfePuCtrl, tssiSleepEn, 1);
    } else if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00, 16,
                                  rfseq_majrev32_rx2tx_cmd);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x70, 16,
                                  rfseq_majrev32_rx2tx_dly);
    } else if (ACMAJORREV_2(pi->pubpi->phy_rev) && !(PHY_IPA(pi))) {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
            16, rfseq_rx2tx_cmd_noafeclkswitch);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x70,
            16, rfseq_rx2tx_cmd_noafeclkswitch_dly);
    }  else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
      /* Switch ON dac before radio to avoid dac-_pu spurs */
      MOD_PHYREG(pi, Logen_AfeDiv_reset_select, bundleDacDiodePwrupEn, 0);
      rfseq_bundle_48[0] = 0x0030;
      rfseq_bundle_48[1] = 0x0;
      rfseq_bundle_48[2] = 0x400;
      wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 24, 48, rfseq_bundle_48);
      wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
                                16, rfseq_majrev130_rx2tx_cmd);
      wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x70,
                                16, rfseq_majrev130_rx2tx_dly);
    }  else if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
            16, rfseq_majrev47_rx2tx_cmd);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x70,
            16, rfseq_majrev47_rx2tx_dly);
    }  else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
            16, rfseq_majrev128_rx2tx_cmd);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x70,
            16, rfseq_majrev128_rx2tx_dly);
    } else {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
            16, rfseq_rx2tx_cmd);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x70,
            16, rfseq_rx2tx_dly);
    }

    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x10, 16,
                                  rfseq_majrev4_tx2rx_cmd);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 128, 16,
                                  rfseq_majrev4_tx2rx_dly);
    } else if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x10, 16,
                                  rfseq_majrev32_tx2rx_cmd);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x80, 16,
                                  rfseq_majrev32_tx2rx_dly);
    } else if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* FIXME63178: lifting with 43684 until we have to differ */
        /* set rfseq_bundle_tbl {0x01C703C 0x01C7014 0 0} */
        /* FIXME43684: for second PLL we should probably use 0x02C700E 0x02C702C
           for 0x82 and 0x83. acphy_write_table RfseqBundle $rfseq_bundle_tbl 0
        */

        MOD_PHYREG(pi, RfctrlCmd, bundleScheme2, rfseq_majrev47_bundleScheme2);

        rfseq_bundle_48[0] = 0x703C;
        rfseq_bundle_48[1] = 0x1c;
        rfseq_bundle_48[2] = 0x0;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 0, 48,
                                  rfseq_bundle_48);
        rfseq_bundle_48[0] = 0x7014;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 1, 48,
                                  rfseq_bundle_48);
        rfseq_bundle_48[0] = 0x0;
        rfseq_bundle_48[1] = 0x0;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 2, 48,
                                  rfseq_bundle_48);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 3, 48,
                                  rfseq_bundle_48);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 4, 48,
                                  rfseq_bundle_48);
        /* in low rate adc mode, add bundle cmd 0x85 to reset rxfarrow at
         * the beginning of tx2rx sequence, add cmd 0x86 to release reset
         * at the end of tx2rx sequence
         */
        rfseq_bundle_48[0] = 0x400;
        rfseq_bundle_48[1] = 0x0;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 5, 48,
                rfseq_bundle_48);
        rfseq_bundle_48[0] = 0x0;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 6, 48,
                rfseq_bundle_48);

        if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x10, 16,
                                      rfseq_majrev129_tx2rx_cmd);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x80, 16,
                                      rfseq_majrev129_tx2rx_dly);
            low_rate = pi->u.pi_acphy->sromi->srom_low_adc_rate_en;
            phy_ac_chanmgr_low_rate_tssi_rfseq_fiforst_dly(pi, low_rate);
        } if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x10, 16,
                                      rfseq_majrev130_tx2rx_cmd);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x80, 16,
                                      rfseq_majrev130_tx2rx_dly);
        } else {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x10, 16,
                                      rfseq_majrev47_tx2rx_cmd);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x80, 16,
                                      rfseq_majrev47_tx2rx_dly);
        }

        if (!ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
            bzero(zeros, sizeof(zeros));
            for (x = 0; x < 512; x += ZEROS_TBL_SZ) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_NVNOISESHAPINGTBL,
                                          ZEROS_TBL_SZ, x, 32, zeros);
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_NVNOISESHAPING11AXTBL,
                                          ZEROS_TBL_SZ, x, 32, zeros);
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_NVRXEVMSHAPINGTBL,
                                          ZEROS_TBL_SZ, x, 8, zeros);
            }
        }

        /* update mclip clip2 table */
        if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
            FOREACH_CORE(pi, core) {
                uint32 tbl = ACPHY_TBL_ID_MCLPAGCCLIP2TBL0 + 32 * core;
                sz = (uint8) ARRAYSIZE(clip2_tbl_maj129);
                wlc_phy_table_write_acphy(pi, tbl, sz, 0x1, 32, clip2_tbl_maj129);
            }
        } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            FOREACH_CORE(pi, core) {
                uint32 tbl = ACPHY_TBL_ID_MCLPAGCCLIP2TBL0 + 32 * core;
                sz = (uint8) ARRAYSIZE(clip2_tbl_maj130);
                // Change Clip2Tbl of "clip Init-gain" to Improved ACI Performacne
                // RB174018
                wlc_phy_table_write_acphy(pi, tbl, sz, 0x0, 32, clip2_tbl_maj130);
            }
        } else if (ACMAJORREV_131(pi->pubpi->phy_rev)) {
            FOREACH_CORE(pi, core) {
                uint32 tbl = ACPHY_TBL_ID_MCLPAGCCLIP2TBL0 + 32 * core;
                wlc_phy_table_write_acphy(pi, tbl, 6, 0x1, 32, clip2_tbl_maj131);
            }
        } else if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
            FOREACH_CORE(pi, core) {
                uint32 tbl = ACPHY_TBL_ID_MCLPAGCCLIP2TBL0 + 32 * core;
                wlc_phy_table_write_acphy(pi, tbl, 6, 0x1, 32, clip2_tbl_maj51);
            }
        } else {
            FOREACH_CORE(pi, core) {
                uint32 tbl = ACPHY_TBL_ID_MCLPAGCCLIP2TBL0 + 32 * core;
                wlc_phy_table_write_acphy(pi, tbl, 6, 0x1, 32, clip2_tbl_maj47_rX);
            }
        }
    } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* set rfseq_bundle_tbl {0x01C703C 0x01C7014 0x02C703E 0x02C701C} */
        /* acphy_write_table RfseqBundle $rfseq_bundle_tbl 0 */
        rfseq_bundle_48[0] = 0x703C;
        rfseq_bundle_48[1] = 0x1c;
        rfseq_bundle_48[2] = 0x0;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 0, 48,
                                  rfseq_bundle_48);
        rfseq_bundle_48[0] = 0x7014;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 1, 48,
                                  rfseq_bundle_48);
        rfseq_bundle_48[0] = 0x700e;
        rfseq_bundle_48[1] = 0x2c;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 2, 48,
                                  rfseq_bundle_48);
        rfseq_bundle_48[0] = 0x702c;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 3, 48,
                                  rfseq_bundle_48);
        rfseq_bundle_48[0] = 0x6020;
        rfseq_bundle_48[1] = 0x20;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 4, 48,
                                  rfseq_bundle_48);
        /* in low rate adc mode, add bundle cmd 0x85 to reset rxfarrow at
         * the beginning of tx2rx sequence, add cmd 0x86 to release reset
         * at the end of tx2rx sequence
         */
        rfseq_bundle_48[0] = 0x400;
        rfseq_bundle_48[1] = 0x0;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 5, 48,
                rfseq_bundle_48);
        rfseq_bundle_48[0] = 0x0;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 6, 48,
                rfseq_bundle_48);
        if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x10, 16,
                    rfseq_majrev128_tx2rx_cmd);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x80, 16,
                    rfseq_majrev128_tx2rx_dly);
        }
        /* update mclip clip2 table */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_MCLPAGCCLIP2TBL0, 6, 0x1, 32, clip2_tbl);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_MCLPAGCCLIP2TBL1, 6, 0x1, 32, clip2_tbl);
    } else if (ACMAJORREV_2(pi->pubpi->phy_rev) && !(PHY_IPA(pi))) {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x10,
            16, rfseq_tx2rx_cmd_noafeclkswitch);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x80,
            16, rfseq_tx2rx_dly_noafeclkswitch);
    } else {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x10, 16, rfseq_tx2rx_cmd);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x80, 16, rfseq_tx2rx_dly);
    }

    /* This was to keep the adc-clock buffer powered up even if adc is powered down
       for non-tiny radio. But for tiny radio this is not required.
    */
    if (!(ACMAJORREV_4(pi->pubpi->phy_rev) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
          ACMAJORREV_33(pi->pubpi->phy_rev) ||
          ACMAJORREV_GE40(pi->pubpi->phy_rev))) {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x3c6, 16, &adc_war_val);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x3c7, 16, &adc_war_val);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x3d6, 16, &adc_war_val);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x3d7, 16, &adc_war_val);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x3e6, 16, &adc_war_val);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x3e7, 16, &adc_war_val);
    }

    /* do this during fem table load for 43602a0 */
    if (((CHSPEC_IS2G(pi->radio_chanspec) && ext_pa_ana_2g) ||
        (CHSPEC_ISPHY5G6G(pi->radio_chanspec) && ext_pa_ana_5g)) &&
        !(ACMAJORREV_5(pi->pubpi->phy_rev) && ACMINORREV_0(pi))) {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x80, 16, &pablowup_war_val);
    }

    /* 4360 and 43602 */
    if (ACMAJORREV_0(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
        /* load the txv for spatial expansion */
        acphy_load_txv_for_spexp(pi);
    }

    if ((RADIOID_IS(pi->pubpi->radioid, BCM2069_ID)) &&
        ((RADIO2069_MAJORREV(pi->pubpi->radiorev) > 0) ||
            ((RADIO2069_MINORREV(pi->pubpi->radiorev) == 16) ||
            (RADIO2069_MINORREV(pi->pubpi->radiorev) == 17)))) {
        /* 11n_20 */
        wlc_phy_set_analog_tx_lpf(pi, 0x2, -1, 5, 5, -1, -1, -1);
        /* 11ag_11ac_20 */
        wlc_phy_set_analog_tx_lpf(pi, 0x4, -1, 5, 5, -1, -1, -1);
        /* 11n_40 */
        wlc_phy_set_analog_tx_lpf(pi, 0x10, -1, 5, 5, -1, -1, -1);
        /* 11ag_11ac_40 */
        wlc_phy_set_analog_tx_lpf(pi, 0x20, -1, 5, 5, -1, -1, -1);
        /* 11n_11ag_11ac_80 */
        wlc_phy_set_analog_tx_lpf(pi, 0x80, -1, 6, 6, -1, -1, -1);
    } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* 11b_20 11n_20 11ag_11ac_20 */
        wlc_phy_set_analog_tx_lpf(pi, 0x7, -1, 4, -1, -1, -1, -1);
        /* 11n_40 11ag_11ac_40 */
        wlc_phy_set_analog_tx_lpf(pi, 0x30, -1, 5, -1, -1, -1, -1);
        /* 11n_11ag_11ac_80 */
        wlc_phy_set_analog_tx_lpf(pi, 0x80, -1, 6, -1, -1, -1, -1);
    }

    /* tiny radio specific processing */
    if (TINY_RADIO(pi)) {
        uint16 regval;
        const uint32 NvAdjTbl[64] = { 0x000000, 0x400844, 0x300633, 0x200422,
            0x100211, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
            0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
            0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000100,
            0x000200, 0x000311, 0x000422, 0x100533, 0x200644, 0x300700,
            0x400800, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
            0x000000, 0x000000, 0x400800, 0x300700, 0x200644, 0x100533,
            0x000422, 0x000311, 0x000200, 0x000100, 0x000000, 0x000000,
            0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
            0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
            0x000000, 0x000000, 0x100211, 0x200422, 0x300633, 0x400844};

        const uint32 phasetracktbl[22] = { 0x06af56cd, 0x059acc7b,
            0x04ce6652, 0x02b15819, 0x02b15819, 0x02b15819, 0x02b15819,
            0x02b15819, 0x02b15819, 0x02b15819, 0x02b15819, 0x06af56cd,
            0x059acc7b, 0x04ce6652, 0x02b15819, 0x02b15819, 0x02b15819,
            0x02b15819, 0x02b15819, 0x02b15819, 0x02b15819, 0x02b15819};

        /* Tiny NvAdjTbl */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_NVADJTBL, 64, 0, 32, NvAdjTbl);

        if (!(ACMAJORREV_4(pi->pubpi->phy_rev) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev))) {
            /* Tiny phasetrack */
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PHASETRACKTBL_1X1, 22, 0, 32,
            phasetracktbl);
        }

        /* Channels Smoothing */
        if ((!ACMINORREV_0(pi) || ACMAJORREV_4(pi->pubpi->phy_rev)) &&
           (!ACMAJORREV_32(pi->pubpi->phy_rev)) &&
           (!ACMAJORREV_33(pi->pubpi->phy_rev)))
            wlc_phy_load_channel_smoothing_tiny(pi);

        /* program tx, rx bias reset to avoid clock stalls */
        regval = 0x2b;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0xe8, 16, &regval);
        regval = 0x28;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0xe7, 16, &regval);

        /* #Keep lpf_pu @ 0 for rx since lpf_pu controls tx lpf exclusively */
        regval = 0x82c0;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x14b, 16, &regval);

        /* Magic rfseqbundle writes to make TX->Rx turnaround work */
        /* set rfseq_bundle_tbl {0x4000 0x0000 } */
        /* acphy_write_table RfseqBundle $rfseq_bundle_tbl 4 */
        if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
            rfseq_bundle_48[0] = 0x6000;
        } else {
            rfseq_bundle_48[0] = 0x4000;
        }
        rfseq_bundle_48[1] = 0x0;
        rfseq_bundle_48[2] = 0x0;

        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 4, 48,
                                  rfseq_bundle_48);
        rfseq_bundle_48[0] = 0x0000;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 5, 48,
                                  rfseq_bundle_48);

        /* set rfseq_bundle_tbl {0x3000C 0x20000 0x30034 0x20000} */
        /* acphy_write_table RfseqBundle $rfseq_bundle_tbl 0 */
        rfseq_bundle_48[0] = 0x0000;
        rfseq_bundle_48[1] = 0x2;
        rfseq_bundle_48[2] = 0x0;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 0, 48,
                                  rfseq_bundle_48);
        rfseq_bundle_48[0] = 0x0034;
        rfseq_bundle_48[1] = 0x3;
        rfseq_bundle_48[2] = 0x0;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 1, 48,
                                  rfseq_bundle_48);
        rfseq_bundle_48[0] = 0x0000;
        rfseq_bundle_48[1] = 0x2;
        rfseq_bundle_48[2] = 0x0;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 2, 48,
                                  rfseq_bundle_48);
        rfseq_bundle_48[0] = 0x000c;
        rfseq_bundle_48[1] = 0x3;
        rfseq_bundle_48[2] = 0x0;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 3, 48,
                                  rfseq_bundle_48);
    }

    /* Increase phase track loop BW to improve PER floor, */
    /*   Phase noise  seems higher. Needs further investigation */
    if (ACMAJORREV_2(pi->pubpi->phy_rev)) {
        uint32 phasetracktbl[22] = { 0x6AF5700, 0x59ACC9A,
            0x4CE6666, 0x4422064, 0x4422064, 0x4422064,    0x4422064,
            0x4422064, 0x4422064, 0x4422064, 0x4422064, 0x6AF5700,
            0x59ACC9A, 0x4CE6666, 0x4422064, 0x4422064, 0x4422064,
            0x4422064, 0x4422064, 0x4422064, 0x4422064, 0x4422064};
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PHASETRACKTBL, 22, 0, 32,
                                  phasetracktbl);
    }

    /* initialize the fixed spatial expansion matrix */
    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
        phy_ac_chanmgr_set_spexp_matrix(pi);
    }

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        for (ctr = 0; ctr < 4; ctr++) {
            /* TX, lpf_bq2_pu=1, lpf_bq1_pu=0 */
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                                      rfseq_lpf_pu_addr[ctr], 16,
                                      &rfseq_lpf_pu_val);
            /* dac_diode_pwrup = 1 */
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                                      rfseq_dacdiode_addr[ctr],
                                      16, &rfseq_dacdiode_val);
        }
    }

    if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        lpf_bq2_gaincomb_val = 0;
        /* For linearity considerations, leave LPF gain to 0 for now.
         * The LPF has been moved away from the 6715 gaintables.
         */
        for (ctr = 0; ctr < 4; ctr++) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                    lpf_bq2_gaincomb_addr[ctr], 16,
                    &lpf_bq2_gaincomb_val);
        }
    }

#ifdef WLSMC
    if (ACMAJORREV_47(pi->pubpi->phy_rev) || ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
        uint8 offset;
        if (pi->smci->download) {
            for (offset = 0; offset < 9; offset++) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPTINYTBL, 8,
                    0x8a0 + offset*8, 32, &Ndbps_ru_LUT[offset]);
            }
            for (offset = 0; offset < 3; offset++) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPTINYTBL, 6,
                    0x980 + offset*8, 32, &n_cbps_ru_LUT[offset]);
            }
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPTINYTBL, 18,
                    0, 32, &dummy_rxctrl[0]);
        }
    } else if (ACMAJORREV_129_130(pi->pubpi->phy_rev)) {
        uint8 offset;
        if (pi->smci->download) {
            for (offset = 0; offset < 9; offset++) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPTINYTBL, 8,
                    0x8a0 + offset*8, 32, &Ndbps_ru_LUT[offset]);
            }
            for (offset = 0; offset < 3; offset++) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPTINYTBL, 6,
                    0x980 + offset*8, 32, &n_cbps_ru_LUT[offset]);
            }

        }
    }

    if (ACMAJORREV_47_51_129_130_131(pi->pubpi->phy_rev)) {
        phy_smc_reset(pi, FALSE);
        phy_smc_reset(pi, TRUE);
        OSL_DELAY(1);
        if (ACMAJORREV_47(pi->pubpi->phy_rev) || ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, mac2smc_controls, mac2smc_interrupt, 0x1);
        } else if (ACMAJORREV_129_130(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, SMCClkResetControl, SMCclkEn, 0);
        }
        OSL_DELAY(6);
    }
#endif /* WLSMC */

#ifdef WLVASIP
    if (ACMAJORREV_47_51(pi->pubpi->phy_rev) || ACMAJORREV_GE129(pi->pubpi->phy_rev)) {
        /* initialize phy rev dependent variables */
        num_cqi_rpts = 46;
        if (ACMAJORREV_47_130(pi->pubpi->phy_rev)) {
            bfdLut_cqi_step = 0xb;
            num_decmp_rpts = 4;
        } else if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
            bfdLut_cqi_step = 0x4;
            num_decmp_rpts = 2;
        } else if (ACMAJORREV_131(pi->pubpi->phy_rev)) {
            bfdLut_cqi_step = 0x6;
            num_decmp_rpts = 2;
        } else { // if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
            bfdLut_cqi_step = 0x5;
            num_decmp_rpts = 3;
        }

        /* initialize decompressed report related lut */
        svmp_addr = wlapi_get_svmp_addr_txv_decompressed_report(pi->sh->physhim);
        mem_size = wlapi_get_svmp_size_txv_decompressed_report(pi->sh->physhim)
            / num_decmp_rpts;
        bfdLut32[2] = SVMPADDR2VC(svmp_addr)
            + (SVMPADDR2VC(svmp_addr + mem_size) << 16);
        if (ACMAJORREV_47_130(pi->pubpi->phy_rev)) {
            bfdLut32[3] = SVMPADDR2VC(svmp_addr + mem_size * 2)
                + (SVMPADDR2VC(svmp_addr + mem_size * 3) << 16);
        } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
            bfdLut32[3] = SVMPADDR2VC(svmp_addr + mem_size * 2);
        }
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_BFDINDEXLUT,
            (num_decmp_rpts+1) >> 1, 130, 32, &bfdLut32[2]);

        /* initialize cqi related lut */
        for (idx = 0; idx < num_cqi_rpts; idx += 2) {
            tmp32 = (idx + ((idx+1) << 16)) * bfdLut_cqi_step;
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_BFDINDEXLUT, 1,
                160+(idx>>1), 32, &tmp32);
        }
        tmp32 = (uint32) num_cqi_rpts - 1;
        tmp32 = (tmp32 + (tmp32 << 16)) * bfdLut_cqi_step;
        for (idx = (num_cqi_rpts >> 1) << 1;
            idx < NUM_BFD_CQI_INDEX; idx += 2) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_BFDINDEXLUT, 1,
                160+(idx>>1), 32, &tmp32);
        }

        /* initialize implicit beamforming related lut */
        svmp_addr = wlapi_get_svmp_addr_imbf_rpt(pi->sh->physhim);
        mem_addr = SVMPADDR2VC(svmp_addr);
        bfdLut32[0] = mem_addr + (mem_addr << 16);
        bfdLut16[0] = (uint16)mem_addr;
        /* for implicit beamforing, steering idx 127 is used
         * which will point to svmp 0x3f00 for 43684
         */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_BFDINDEXLUT, 1,
            159, 32, &bfdLut32[0]);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_IBFERPTADDRLUT, 1,
            63, 16, &bfdLut16[0]);

        if (pi->vasipi->active) {
            svmp_addr = wlapi_get_svmp_addr_mu_output(pi->sh->physhim, 0);
            mem_addr  = wlapi_get_svmp_addr_mu_output(pi->sh->physhim, 1);
            bfdLut32[0] = SVMPADDR2VC(svmp_addr) + (SVMPADDR2VC(mem_addr) << 16);
            svmp_addr = wlapi_get_svmp_addr_mu_output(pi->sh->physhim, 2);
            mem_addr  = wlapi_get_svmp_addr_mu_output(pi->sh->physhim, 3);
            bfdLut32[1] = SVMPADDR2VC(svmp_addr) + (SVMPADDR2VC(mem_addr) << 16);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_BFDINDEXLUT, 2,
                128, 32, &bfdLut32[0]);
        }
    }

    if (ACMAJORREV_47_51_129_130_131(pi->pubpi->phy_rev)) {
        /* initialize csi related lut */
        bfdLut16[0] = SVMPADDR2VC(wlapi_get_svmp_addr_csi_rpt(pi->sh->physhim, 0));
        bfdLut16[1] = SVMPADDR2VC(wlapi_get_svmp_addr_csi_rpt(pi->sh->physhim, 1));
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_IBFERPTADDRLUT, 2,
            0, 16, &bfdLut16[0]);
    }

    /* After 43684B1, by default phy reset won't trigger vasip reset */
    if (ACMAJORREV_47(pi->pubpi->phy_rev) && ACMINORREV_LE(pi, 1)) {
        phy_vasip_reset_proc(pi, TRUE);
        phy_vasip_reset_proc(pi, FALSE);
    }

    if (ACMAJORREV_51(pi->pubpi->phy_rev) || ACMAJORREV_GE130(pi->pubpi->phy_rev)) {
        /* programming the bit mask to indicate all 128 memory slots available */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_BFDINDEXLUT, 4,
            224, 32, &bfdLut_128slots[0]);
    } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
        /* programming the bit mask to indicate all 255 memory slots available */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_BFDINDEXLUT, 8,
            224, 32, &bfdLut_255slots[0]);
    }
#endif /* WLVASIP */

#ifdef WLWARENG
    // No table to be loaded for WAR-engine for now
#endif /* WLWARENG */

    ACPHY_ENABLE_STALL(pi, stall_val);

}

/*
JIRA:BCAWLAN-206637: Adjust TxRX rfseq to make low_rate_tssi work.
Extend tx2rx (farrow_reset) such that it extends length of
WAIT_NCLKS = crsTxExtension + sgi-extention
crsTxExtension + (9*16) - 40 - 50 = crsTxExtension + 54;
9*16 --> 9 sgi symbols, 16 -> 16/40mhz = 0.4 us. 40 --> 1 us,
as bilge_cnt gives some margin before raddr>=waddr.
50 --> delay already in TX2RX before farrow_reset (0x85)
*/
void phy_ac_chanmgr_low_rate_tssi_rfseq_fiforst_dly(phy_info_t *pi, bool enable)
{
    phy_ac_chanmgr_info_t *chanmgri = pi->u.pi_acphy->chanmgri;
    uint16 fifo_rst_dly[1];

    if (enable) {
        fifo_rst_dly[0] = chanmgri->crsTxExtension + 54;
    } else {
        fifo_rst_dly[0] = 4;
    }

    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x8d, 16,
                              fifo_rst_dly);
}

static void
wlc_phy_set_regtbl_on_band_change_majrev_ge40(phy_info_t *pi)
{
    uint8 core, phy_coremask = pi->pubpi->phy_coremask;
    uint sicoreunit;
    int32 rd;

    PHY_INFORM(("%s: band=%d coremask = 0x%x\n", __FUNCTION__,
        (CHSPEC_IS2G(pi->radio_chanspec))?2:5, phy_coremask));

    sicoreunit = wlapi_si_coreunit(pi->sh->physhim);

    FOREACH_CORE(pi, core) {
        if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
                MOD_RADIO_REG_20709(pi, RX2G_REG4, core,
                        rx_ldo_pu, 0x1);
        }
        MOD_PHYREG(pi, dyn_radioa0, dyn_radio_ovr0, 0x1);
        MOD_PHYREG(pi, dyn_radioa1, dyn_radio_ovr1, 0x1);
        if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
            //set phyreg(dyn_radioa$core.dyn_radio_ovr_val_idac_main$core) 0x2a;
            MOD_PHYREG(pi, dyn_radioa0, dyn_radio_ovr_val_idac_main0, 0x2a);
            MOD_PHYREG(pi, dyn_radioa1, dyn_radio_ovr_val_idac_main1, 0x2a);
            //set phyreg(radio_connection_direct_pad_idac.pad_idac_gm) 0xa;
            MOD_PHYREG(pi, radio_connection_direct_pad_idac, pad_idac_gm, 0xa);
            //set phyreg(radio_connection_direct_mx_bbdc.mx5g_idac_bbdc) 0xa;
            MOD_PHYREG(pi, radio_connection_direct_mx_bbdc,
                mx5g_idac_bbdc, 0xa);
            //set phyreg(radio_connection_direct_pad_idac.pad_idac_pmos) 0x14;
            MOD_PHYREG(pi, radio_connection_direct_pad_idac,
                pad_idac_pmos, 0x14);
            //set phyreg(radio_connection_direct_mx_bbdc.mx2g_idac_bbdc) 0x14;
            MOD_PHYREG(pi, radio_connection_direct_mx_bbdc,
                mx2g_idac_bbdc, 0x14);
        } else {
            wlc_phy_set_papdlut_dynradioregtbl_majrev_ge40(pi);
        }

        if (PHY_COREMASK_SISO(phy_coremask)) {
            /* SISO */
            if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
                PHY_INFORM(("%s: SISO 5G core= %d\n", __FUNCTION__, core));
                WRITE_PHYREGCE(pi, Dac_gain, core, 0xd670);
                WRITE_PHYREGCE(pi, pllLogenMaskCtrl, core, 0x1f);
            } else {
                PHY_INFORM(("%s: SISO 2G core= %d\n", __FUNCTION__, core));
                WRITE_PHYREGCE(pi, Dac_gain, core, 0x7870);
                WRITE_PHYREGCE(pi, pllLogenMaskCtrl, core, 0x3f);
            }
        } else {
            /* MIMO */
            if (core == 1) {
                if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
                    PHY_INFORM(("wl%d %s: MIMO 5G core= %d\n",
                        PI_INSTANCE(pi), __FUNCTION__, core));
                    sicoreunit = wlapi_si_coreunit(pi->sh->physhim);
                    WRITE_PHYREGCE(pi, Dac_gain, core, 0xc770);
                    if (sicoreunit == DUALMAC_MAIN) {
                        PHY_INFORM(("wl%d %s: MIMO 5G MAIN\n",
                            PI_INSTANCE(pi), __FUNCTION__));
                        WRITE_PHYREGCE(pi, pllLogenMaskCtrl, core,
                            0x2fb);
                    } else if (sicoreunit == DUALMAC_AUX) {
                        PHY_INFORM(("wl%d %s: MIMO 5G AUX\n",
                            PI_INSTANCE(pi), __FUNCTION__));
                        WRITE_PHYREGCE(pi, pllLogenMaskCtrl, core,
                            0x2ff);
                    }
                    WRITE_PHYREGCE(pi, Extra2AfeClkDivOverrideCtrl28nm,
                        core, 0x20);
                } else {
                    PHY_INFORM(("wl%d %s: MIMO 2G core= %d\n",
                        PI_INSTANCE(pi), __FUNCTION__, core));
                    WRITE_PHYREGCE(pi, Dac_gain, core, 0x6170);
                    WRITE_PHYREGCE(pi, pllLogenMaskCtrl, core, 0xff);
                    WRITE_PHYREGCE(pi, Extra2AfeClkDivOverrideCtrl28nm,
                        core, 0x10);
                }
            } else if (core == 0) {
                if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
                    PHY_INFORM(("wl%d %s: MIMO 5G core= %d\n",
                        PI_INSTANCE(pi), __FUNCTION__, core));
                    WRITE_PHYREGCE(pi, Dac_gain, core, 0xc770);
                    WRITE_PHYREGCE(pi, pllLogenMaskCtrl, core, 0x77f);
                    WRITE_PHYREGCE(pi, Extra2AfeClkDivOverrideCtrl28nm,
                        core, 0x20);
                } else {
                    PHY_INFORM(("wl%d %s: MIMO 2G core= %d\n",
                        PI_INSTANCE(pi), __FUNCTION__, core));
                    WRITE_PHYREGCE(pi, Dac_gain, core, 0x6170);
                    WRITE_PHYREGCE(pi, pllLogenMaskCtrl, core, 0x1ff);
                    WRITE_PHYREGCE(pi, Extra2AfeClkDivOverrideCtrl28nm,
                        core, 0x10);
                }
            }
        }
    }
    if (CHSPEC_IS2G(pi->radio_chanspec)) {
        /* CRDOT11ACPHY-2555 lesi and bphy_pre_det set in aux block bphy */
        phy_ac_rxgcrs_iovar_get_lesi(pi->u.pi_acphy->rxgcrsi, &rd);
        /* skip bphy predetector for 6878 */
        if ((rd && wlapi_si_coreunit(pi->sh->physhim) == DUALMAC_AUX) ||
            ACMAJORREV_128(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, CRSMiscellaneousParam, bphy_pre_det_en, 0);
        }
        else {
            MOD_PHYREG(pi, CRSMiscellaneousParam, bphy_pre_det_en, 1);
        }
    } else {
        MOD_PHYREG(pi, CRSMiscellaneousParam, bphy_pre_det_en, 0);
    }

    MOD_PHYREG(pi, fineRxclockgatecontrol, EncodeGainClkEn, 1);
}

static void
wlc_phy_set_regtbl_on_band_change_acphy(phy_info_t *pi)
{
    uint8 stall_val, core;
    uint16 bq1_gain_core1 = 0x49;
    uint8 pdet_range_id;
    int8 loftshift;
#ifndef WLC_DISABLE_ACI
    bool hwaci_on;
#endif /* !WLC_DISABLE_ACI */
    bool w2_on;
    txcal_coeffs_t txcal_cache[PHY_CORE_MAX];
    rxcal_coeffs_t rxcal_cache[PHY_CORE_MAX];
#ifdef PHYCAL_CACHING
    ch_calcache_t *ctx;
    bool ctx_valid;
#endif /* PHYCAL_CACHING */
    phy_ac_chanmgr_info_t *chanmgri = pi->u.pi_acphy->chanmgri;
    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    ACPHY_DISABLE_STALL(pi);

    wlc_phy_cfg_energydrop_timeout(pi);

    if (!(ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        wlc_phy_bphymrc_hwconfig(pi);
    }

    if (CHSPEC_IS2G(pi->radio_chanspec)) {
        /* write to bphy regs only when in 2g */
        WRITE_PHYREG(pi, DsssStep, 0x668);      /* gives slight improvement */

        if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))
            WRITE_PHYREG(pi, bphymrcCtrl, 0x60);    /* disable bphy mrc for now  */
    }

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        /* 6878 has to follow 43684/63178 in rfseqext */
        wlc_phy_set_rfseqext_tbl(pi, pi->u.pi_acphy->sromi->srom_low_adc_rate_en);
    } else if (ACMAJORREV_GE40(pi->pubpi->phy_rev)) {
        wlc_phy_set_regtbl_on_band_change_majrev_ge40(pi);
    }

    if (ACMAJORREV_5(pi->pubpi->phy_rev) ||
        ACMAJORREV_2(pi->pubpi->phy_rev) ||
        ACMAJORREV_128(pi->pubpi->phy_rev)) {
        if (CHSPEC_IS2G(pi->radio_chanspec)) {
            MOD_PHYREG(pi, bOverAGParams, bOverAGlog2RhoSqrth, 120);
            MOD_PHYREG(pi, CRSMiscellaneousParam, b_over_ag_falsedet_en, 1);
        } else {
            MOD_PHYREG(pi, bOverAGParams, bOverAGlog2RhoSqrth, 255);
            MOD_PHYREG(pi, CRSMiscellaneousParam, b_over_ag_falsedet_en, 0);
        }
    }

    if (ACMAJORREV_2(pi->pubpi->phy_rev) && !ACMINORREV_0(pi)) {
        FOREACH_CORE(pi, core) {
            /* Reduces 20in80 humps in 5G */
            WRITE_PHYREGC(pi, Clip2Threshold, core, 0xa04e);
            if (CHSPEC_IS2G(pi->radio_chanspec))
              WRITE_PHYREGC(pi, Clip2Threshold, core, 0x804e);
        }
    }

    if (ACMAJORREV_4(pi->pubpi->phy_rev) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) || (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev) && !ACMAJORREV_129(pi->pubpi->phy_rev) &&
        !ACMAJORREV_131(pi->pubpi->phy_rev))) {
        MOD_PHYREG(pi, clip_detect_normpwr_var_mux, use_norm_var_for_clip_detect, 1);
    }

    /* Load tx gain table */
    wlc_phy_ac_gains_load(pi->u.pi_acphy->tbli);

    if (ACREV_IS(pi->pubpi->phy_rev, 0)) {
        wlc_phy_tx_gm_gain_boost(pi);
    }

    pdet_range_id = phy_tpc_get_5g_pdrange_id(pi->tpci);
    if ((pdet_range_id == 9 || pdet_range_id == 16) && !ACMAJORREV_32(pi->pubpi->phy_rev) &&
        !ACMAJORREV_33(pi->pubpi->phy_rev) &&
        !(ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        bq1_gain_core1 = (CHSPEC_ISPHY5G6G(pi->radio_chanspec))? 0x49 : 0;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x18e, 16, &bq1_gain_core1);
    }

    if (IS_4364_3x3(pi)) {
        wlc_phy_ac_shared_ant_femctrl_master(pi);
    } else {
#if (!defined(WL_SISOCHIP) && defined(SWCTRL_TO_BT_IN_COEX))
        /* Write FEMCTRL mask to shmem ; let ucode write them to FEMCTRL register */
        wlc_phy_ac_femctrl_mask_on_band_change_btcx(pi->u.pi_acphy->btcxi);
#else
        wlc_phy_ac_femctrl_mask_on_band_change(pi);
#endif
    }

    if (RADIOID_IS(pi->pubpi->radioid, BCM20693_ID)) {
        wlc_phy_set_regtbl_on_band_change_acphy_20693(pi);

        /* 4349 specific chspec initializations */
        if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
            if (CCT_INIT(pi->u.pi_acphy) ||
                CCT_BAND_CHG(pi->u.pi_acphy) || CCT_BW_CHG(pi->u.pi_acphy)) {
                wlc_acphy_load_4349_specific_tbls(pi);
                wlc_acphy_dyn_papd_cfg_20693(pi);
                wlc_phy_config_bias_settings_20693(pi);
                acphy_set_lpmode(pi, ACPHY_LP_PHY_LVL_OPT);
            }
        }

    }

    /* 2g/5g band can have different aci modes */
    if (!ACPHY_ENABLE_FCBS_HWACI(pi)) {
#ifndef WLC_DISABLE_ACI
        hwaci_on = ((pi->sh->interference_mode & ACPHY_ACI_HWACI_PKTGAINLMT) != 0) ||
        (((pi->sh->interference_mode & ACPHY_HWACI_MITIGATION) != 0) &&
        !(ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)));
        wlc_phy_hwaci_setup_acphy(pi, hwaci_on, FALSE);
#endif /* !WLC_DISABLE_ACI */
        w2_on = ((pi->sh->interference_mode & ACPHY_ACI_W2NB_PKTGAINLMT) != 0) ||
            (((pi->sh->interference_mode & ACPHY_HWACI_MITIGATION) != 0) &&
            !(ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)));
        wlc_phy_aci_w2nb_setup_acphy(pi, w2_on);
    }

    if (PHY_PAPDEN(pi)) {

        if (!ACMAJORREV_4(pi->pubpi->phy_rev))
            OSL_DELAY(100);

        if (TINY_RADIO(pi))
        {
#ifdef PHYCAL_CACHING
            PHY_CAL(("wlc_phy_set_regtbl_on_band_change_acphy:"
                " wlc_phy_get_chanctx\n"));
            ctx = wlc_phy_get_chanctx(pi, pi->radio_chanspec);
            ctx_valid = (ctx != NULL) ? ctx->valid : FALSE;

            /* allow reprogramming rfpwrlut if ctx is not available or
             * ctx is available but invalid
             */
            if (!ctx_valid)
#endif /* PHYCAL_CACHING */
                wlc_phy_papd_set_rfpwrlut_tiny(pi);
        } else if (ACMAJORREV_51_129_130_131(pi->pubpi->phy_rev) ||
                ACMAJORREV_128(pi->pubpi->phy_rev)) {
            wlc_phy_papd_set_rfpwrlut_tiny(pi);
        } else {
            wlc_phy_papd_set_rfpwrlut(pi);
        }
    }

    /* For 4350C0, bphy is turned off when in 5G. Need to disable the predetector. */
    if (ACMAJORREV_2(pi->pubpi->phy_rev) && !ACMINORREV_0(pi)) {
        if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
            MOD_PHYREG(pi, CRSMiscellaneousParam, bphy_pre_det_en, 0);
        } else {
            /* Disable BPHY pre-detect JIRA:SWWLAN-45198 jammer/ACI performance
            * takes a hit with predetector enabled
            */
            MOD_PHYREG(pi, CRSMiscellaneousParam, bphy_pre_det_en, 0);
        }
    }

    /* Turn ON 11n 256 QAM in 2.4G */
    if (ACMAJORREV_4(pi->pubpi->phy_rev) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        bool enable = (CHSPEC_IS2G(pi->radio_chanspec) && CHSPEC_IS20(pi->radio_chanspec));

        if (!ACMAJORREV_32(pi->pubpi->phy_rev) &&
            !ACMAJORREV_33(pi->pubpi->phy_rev) &&
            !ACMAJORREV_GE47(pi->pubpi->phy_rev))
            WRITE_PHYREG(pi, miscSigCtrl, enable ? 0x203 : 0x3);

        if (ACMAJORREV_47_51_129_131(pi->pubpi->phy_rev)) {
        /* http://bca-jira.broadcom.com/browse/CRBCAD11PHY-3906
           avoid phytxerr under HT proprietary rate when bfd_spexp is used
        */
            wlapi_11n_proprietary_rates_enable(pi->sh->physhim, DISABLE);
        } else {
        /* Already fixed for 6715 */
            wlapi_11n_proprietary_rates_enable(pi->sh->physhim, enable);
        }
        PHY_INFORM(("wl%d %s: 11n turbo QAM %s\n",
            PI_INSTANCE(pi), __FUNCTION__,
            enable ? "enabled" : "disabled"));

        /* Loading Tx specific radio settings  */
        if (ACMAJORREV_4(pi->pubpi->phy_rev) &&
            RADIOID_IS(pi->pubpi->radioid, BCM20693_ID))
            wlc_phy_config_bias_settings_20693(pi);
    }

    /* Increase LOFT comp range to avoid LOFT failure */
    /* Issue BCAWLAN-219228 was reported on multiple BCA chips,
     * all chips should be included here.
     */
    if (ACMAJORREV_47(pi->pubpi->phy_rev) || ACMAJORREV_129(pi->pubpi->phy_rev) ||
        ACMAJORREV_130(pi->pubpi->phy_rev)) {
        /* 0:1x loft comp, 1: 2x loft comp */
        MOD_PHYREG(pi, SlnaControl, inv_btcx_prisel_polarity, 1);
        loftshift = (chanmgri->loftcompshift_ovrd == -1) ? 1 : chanmgri->loftcompshift_ovrd;
        if ((pi->sh->boardflags4 & BFL4_SROM18_LOFT_COMP_2X) != 0 &&
            (chanmgri->loftcompshift_ovrd == -1)) {
            MOD_PHYREG(pi, Core1TxControl, loft_comp_shift, 2);
            MOD_PHYREG(pi, Core2TxControl, loft_comp_shift, 2);
            MOD_PHYREG(pi, Core3TxControl, loft_comp_shift, 2);
            if (!ACMAJORREV_129(pi->pubpi->phy_rev))
                MOD_PHYREG(pi, Core4TxControl, loft_comp_shift, 2);
        } else {
            MOD_PHYREG(pi, Core1TxControl, loft_comp_shift, loftshift);
            MOD_PHYREG(pi, Core2TxControl, loft_comp_shift, loftshift);
            MOD_PHYREG(pi, Core3TxControl, loft_comp_shift, loftshift);
            if (!ACMAJORREV_129(pi->pubpi->phy_rev))
                MOD_PHYREG(pi, Core4TxControl, loft_comp_shift, loftshift);
        }
    } else if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
        loftshift = (chanmgri->loftcompshift_ovrd == -1) ? 2 : chanmgri->loftcompshift_ovrd;
        MOD_PHYREG(pi, Core1TxControl, loft_comp_shift, loftshift);
        MOD_PHYREG(pi, Core2TxControl, loft_comp_shift, loftshift);
    } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        loftshift = (chanmgri->loftcompshift_ovrd == -1) ? 1 : chanmgri->loftcompshift_ovrd;
        /* 6878 is based on old PHY and supports only 1-bit shift */
        MOD_PHYREG(pi, Core1TxControl, loft_comp_shift, loftshift);
        MOD_PHYREG(pi, Core2TxControl, loft_comp_shift, loftshift);
    }

#ifdef WL_EAP_NOISE_MEASUREMENTS
    /* knoise rxgain override value initializaiton */
    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
        if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
            wlapi_bmac_write_shm(pi->sh->physhim, M_NCAL_RXGAIN_HI(pi), RxGAIN_HI_5G);
            wlapi_bmac_write_shm(pi->sh->physhim, M_NCAL_RXGAIN_LO(pi), RxGAIN_LO_5G);
        } else {
            wlapi_bmac_write_shm(pi->sh->physhim, M_NCAL_RXGAIN_HI(pi), RxGAIN_HI_2G);
            wlapi_bmac_write_shm(pi->sh->physhim, M_NCAL_RXGAIN_LO(pi), RxGAIN_LO_2G);
        }
    }
#ifdef WL_EAP_BCM43570
    else if (IS_43570(pi)) {
        if (CHSPEC_IS2G(pi->radio_chanspec)) {
            /* Compensate for 2G's low LNA2 gains */
            if (BF_ELNA_2G(pi->u.pi_acphy)) {
                wlapi_bmac_write_shm(pi->sh->physhim, M_NCAL_LPFGAIN_HI(pi), 0x28);
                wlapi_bmac_write_shm(pi->sh->physhim, M_NCAL_LPFGAIN_LO(pi), 0x9);
            }
            else {
                wlapi_bmac_write_shm(pi->sh->physhim, M_NCAL_LPFGAIN_HI(pi), 0x28);
                wlapi_bmac_write_shm(pi->sh->physhim, M_NCAL_LPFGAIN_LO(pi), 0x0);
            }
        /* Else 5G */
        } else {
            /* Compensate for 5G's eLNA-less high mixer gain */
            wlapi_bmac_write_shm(pi->sh->physhim, M_NCAL_LPFGAIN_HI(pi), 0x32);
            wlapi_bmac_write_shm(pi->sh->physhim, M_NCAL_LPFGAIN_LO(pi), 0x3c);
        }
    }
#endif /* WL_EAP_BCM43570 */

#endif /* WL_EAP_NOISE_MEASUREMENTS */
    /* need to zero out cal coeffs on band change */
    bzero(txcal_cache, sizeof(txcal_cache));
    bzero(rxcal_cache, sizeof(rxcal_cache));
    wlc_phy_txcal_coeffs_upd(pi, txcal_cache);
    wlc_phy_rxcal_coeffs_upd(pi, rxcal_cache);
    wlc_phy_rxiq_gtbl_comp_acphy(pi, FALSE, NULL);

    if (!ACMAJORREV_32(pi->pubpi->phy_rev) && !ACMAJORREV_33(pi->pubpi->phy_rev)) {
        wlc_acphy_paldo_change(pi);
    }
    ACPHY_ENABLE_STALL(pi, stall_val);

    /* gpioX = bandsel */
    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        phy_ac_chanmgr_set_bandsel_on_gpio(pi, CHSPEC_IS2G(pi->radio_chanspec), -1);
    }
#ifdef WLC_TXSHAPER
    if (!SCAN_RM_IN_PROGRESS(pi)) {
        if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            phy_ac_txss_txshaper_init(chanmgri->aci->txssi);
        }
    }
#endif
}

static void
wlc_phy_set_regtbl_on_bw_change_acphy(phy_info_t *pi)
{
    int sp_tx_bw = 0;
    uint8 stall_val, core, nbclip_cnt_4360 = 15;
    uint8 rxevm20p[] = {8, 6, 4}, rxevm20n[] = {4, 6, 8};
    uint8 rxevm0[] = {0, 0, 0}, rxevm_len = 3;
    uint32 epa_turnon_time;
    uint32 NvAdjAXVal;
    uint8 lpf_txbq1[] = {0, 0, 0, 0}, lpf_txbq2[] = {4, 5, 6, 7};
    uint8 lpf_rxbq1[] = {0, 1, 2, 3}, lpf_rxbq2[] = {0, 1, 2, 3};
    uint16 x, NvAdjAXStartTone, NvAdjAXEndTone, val;
    uint8 zeros[ZEROS_TBL_SZ * 4];
#ifndef BCMQT
#ifdef WLVASIP
    uint32 ruAllocParams;
    uint32 svmp_addr, mem_idx, mem_addr;
    uint16 num_cqi_rpts, bfdLut_cqi_step;
    uint32 zero_word = 0;
    uint16 rpt_size_list[] = {3, 4, 6, 11};
    uint8 ru_end_list[] = {8, 17, 36, 73};
    /* 11ax 20MHz 4x4 CQI report header with ru from 0 to 0 */
    uint32 cqi_rpt_header[] = {0x5b600000, 0x00000008, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000};
#endif /* WLVASIP */
#endif /* BCMQT */

    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    ACPHY_DISABLE_STALL(pi);

    if (HW_PHYRESET_ON_BW_CHANGE == 0)
        wlc_phy_set_reg_on_bw_change_acphy_noHWPhyReset(pi);

    if (CHSPEC_IS80(pi->radio_chanspec) ||
        PHY_AS_80P80(pi, pi->radio_chanspec)) {
        /* 80mhz */
        if (ACMAJORREV_0(pi->pubpi->phy_rev))
            sp_tx_bw = 5;
        else
            sp_tx_bw = 6;

        nbclip_cnt_4360 *= 4;
    } else if (CHSPEC_IS160(pi->radio_chanspec)) {
        /* true 160mhz */
        sp_tx_bw = 7;
        if (ACMAJORREV_47_129_130(pi->pubpi->phy_rev)) {
            nbclip_cnt_4360 *= 8;
        } else {
            nbclip_cnt_4360 *= 4;
        }
    } else if (CHSPEC_IS40(pi->radio_chanspec)) {
        /* 40mhz */
        if (ACMAJORREV_0(pi->pubpi->phy_rev))
            sp_tx_bw = 4;
        else
            sp_tx_bw = 5;

        nbclip_cnt_4360 *= 2;
    } else if (CHSPEC_BW_LE20(pi->radio_chanspec)) {
        /* 20mhz */
        if (ACMAJORREV_0(pi->pubpi->phy_rev))
            sp_tx_bw = 3;
        else if (ACMAJORREV_GE37(pi->pubpi->phy_rev)) {
            sp_tx_bw = 4;
        } else {
            sp_tx_bw = 5;
        }
    } else {
        PHY_ERROR(("%s: No primary channel settings for bw=%d\n",
                   __FUNCTION__, CHSPEC_BW(pi->radio_chanspec)));
    }

    /* reduce NB clip CNT thresholds */
    FOREACH_CORE(pi, core) {
        MOD_PHYREGC(pi, FastAgcClipCntTh, core, fastAgcNbClipCntTh,
            nbclip_cnt_4360);
    }

    if (!(ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        wlc_phy_set_analog_tx_lpf(pi, 0x100, -1, sp_tx_bw, sp_tx_bw, -1, -1, -1);
    }

    if (ACMAJORREV_131(pi->pubpi->phy_rev)) {
        /* Reduce RSSI error for low signal level in BW160. See BCAWLAN-222508 */
        FOREACH_CORE(pi, core) {
            if (CHSPEC_IS160(pi->radio_chanspec)) {
                MOD_PHYREGCEE(pi, RxStatPwrOffset, core, use_gainVar_for_rssi, 0);
            } else {
                MOD_PHYREGCEE(pi, RxStatPwrOffset, core, use_gainVar_for_rssi, 1);
            }
        }
    }

    if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
        wlc_phy_radio20698_set_tx_notch(pi);
    } else if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
        wlc_phy_radio20704_set_tx_notch(pi);
    } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        wlc_phy_radio20709_set_tx_notch(pi);
    } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
        //FIXME6710
        wlc_phy_radio20707_set_tx_notch(pi);
    } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        wlc_phy_radio20708_set_tx_notch(pi);
    } else if (ACMAJORREV_131(pi->pubpi->phy_rev)) {
        wlc_phy_radio20710_set_tx_notch(pi);
    }

    /* change the barelyclipgainbackoff to 6 for 80Mhz due to some PER issue for 4360A0 CHIP */
    if (ACREV_IS(pi->pubpi->phy_rev, 0)) {
      if (CHSPEC_IS80(pi->radio_chanspec)) {
          ACPHYREG_BCAST(pi, Core0computeGainInfo, 0xcc0);
      } else {
          ACPHYREG_BCAST(pi, Core0computeGainInfo, 0xc60);
      }
    }

#ifndef WL_FDSS_DISABLED
    /* Enable FDSS */
    if (TINY_RADIO(pi) && !ACMAJORREV_32(pi->pubpi->phy_rev) &&
        !ACMAJORREV_33(pi->pubpi->phy_rev) &&
        ((CHSPEC_IS2G(pi->radio_chanspec) && (pi->fdss_level_2g[0] != -1)) ||
        (CHSPEC_ISPHY5G6G(pi->radio_chanspec) && (pi->fdss_level_5g[0] != -1))))  {
        wlc_phy_fdss_init(pi);
        if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
            wlc_phy_set_fdss_table_majrev_ge40(pi);
        } else {
            wlc_phy_set_fdss_table(pi);
        }
    }
#endif /* WL_FDSS_DISABLED */

    if (ACMAJORREV_2(pi->pubpi->phy_rev) && (ACMINORREV_1(pi) || ACMINORREV_3(pi))) {
        FOREACH_CORE(pi, core) {
            /* Reduces 54Mbps humps */
            MOD_PHYREGC(pi, computeGainInfo, core, gainBackoffValue, 1);
            /* Reduces 20in80 humps */
            WRITE_PHYREGC(pi, Clip2Threshold, core, 0xa04e);
        }
    }

    if (IS_4364_3x3(pi)) {
        FOREACH_CORE(pi, core) {
            /* Reduces 54Mbps humps */
            MOD_PHYREGC(pi, computeGainInfo, core, gainBackoffValue, 1);
        }
    }

    /* Shape rxevm table due to hit on near DC_tones */
    if (ACMAJORREV_0(pi->pubpi->phy_rev) || ACMAJORREV_2(pi->pubpi->phy_rev) ||
        ACMAJORREV_5(pi->pubpi->phy_rev)) {
        if (CHSPEC_IS20(pi->radio_chanspec)) {
            /* Freq Bins {1 2 3} = {8 6 4} dB */
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_NVRXEVMSHAPINGTBL,
                                      rxevm_len, 1, 8, rxevm20p);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_NVRXEVMSHAPINGTBL,
                                      rxevm_len, 64 - rxevm_len, 8, rxevm20n);
        } else {
            /* Reset the 20mhz entries to 0 */
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_NVRXEVMSHAPINGTBL,
                                      rxevm_len, 1, 8, rxevm0);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_NVRXEVMSHAPINGTBL,
                                      rxevm_len, 64 - rxevm_len, 8, rxevm0);
        }
    }

    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
        uint8 phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;
        BCM_REFERENCE(phyrxchain);
        FOREACH_ACTV_CORE(pi, phyrxchain, core) {
            MOD_PHYREGCE(pi, RfctrlCoreAfeCfg2, core, afe_iqadc_clamp_en, 1);
            MOD_PHYREGCE(pi, RfctrlOverrideAfeCfg, core, afe_iqadc_clamp_en, 1);
        }

        /* updategainH : increase clamp_en off delay to 16 */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0x30, 16, rf_updh_cmd_clamp);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0xa0, 16, rf_updh_dly_clamp);

        /* updategainL : increase clamp_en off delay to 16 */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0x40, 16, rf_updl_cmd_clamp);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0xb0, 16, rf_updl_dly_clamp);

        /* updategainU : increase clamp_en off delay to 16 */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0x50, 16, rf_updu_cmd_clamp);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0xc0, 16, rf_updu_dly_clamp);
    }

    /* [4365]resovle DVGA stuck high - htagc and gainreset during wait_energy_drop collides */
    if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev)))
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0xb0, 16, rf_updl_dly_dvga);

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* 240 * 25 us = 6 ms @ 20 MHz, scale for other PHY-BW's */
        uint16 paydecodetimeout = CHSPEC_IS20(pi->radio_chanspec) ? 240 :
            CHSPEC_IS40(pi->radio_chanspec) ? 480 :
            CHSPEC_IS80(pi->radio_chanspec) ? 960 : 1920;
        WRITE_PHYREG(pi, ofdmpaydecodetimeoutlen, paydecodetimeout);

        /* 4095 bytes @ 1Mbps is 33 ms. 1400 * 25 us = 35 ms */
        paydecodetimeout = CHSPEC_IS20(pi->radio_chanspec) ? 1400 : 2800;
        WRITE_PHYREG(pi, cckpaydecodetimeoutlen, paydecodetimeout);
        /* 32 * 25 us = 800 us */
        WRITE_PHYREG(pi, nonpaydecodetimeoutlen, 0x20);

        if (CHSPEC_IS160(pi->radio_chanspec)) {
            MOD_PHYREG(pi, timeoutEn, resetRxontimeout, 1);
        } else {
            MOD_PHYREG(pi, timeoutEn, resetRxontimeout, 0);
        }
        if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
            // BCAWLAN-224883: enable non-payload
            ACPHY_REG_LIST_START
            MOD_PHYREG_ENTRY(pi, timeoutEn, resetCCAontimeout, 1)
            MOD_PHYREG_ENTRY(pi, timeoutEn, nonpaydecodetimeoutEn, 1)
            MOD_PHYREG_ENTRY(pi, timeoutEn, cckpaydecodetimeoutEn, 1)
            MOD_PHYREG_ENTRY(pi, timeoutEn, ofdmpaydecodetimeoutEn, 1)
            ACPHY_REG_LIST_EXECUTE(pi);
        } else {
            ACPHY_REG_LIST_START
            MOD_PHYREG_ENTRY(pi, timeoutEn, resetCCAontimeout, 1)
            MOD_PHYREG_ENTRY(pi, timeoutEn, nonpaydecodetimeoutEn, 0)
            MOD_PHYREG_ENTRY(pi, timeoutEn, cckpaydecodetimeoutEn, 0)
            MOD_PHYREG_ENTRY(pi, timeoutEn, ofdmpaydecodetimeoutEn, 1)
            ACPHY_REG_LIST_EXECUTE(pi);
        }
    }

    if (TINY_RADIO(pi)) {
            if (CHSPEC_BW_LE20(pi->radio_chanspec)) {
                ACPHY_REG_LIST_START
                    WRITE_PHYREG_ENTRY(pi, nonpaydecodetimeoutlen, 1)
                    MOD_PHYREG_ENTRY(pi, timeoutEn, resetCCAontimeout, 1)
                    MOD_PHYREG_ENTRY(pi, timeoutEn, nonpaydecodetimeoutEn, 1)
                ACPHY_REG_LIST_EXECUTE(pi);
            } else {
                ACPHY_REG_LIST_START
                    WRITE_PHYREG_ENTRY(pi, nonpaydecodetimeoutlen, 32)
                    MOD_PHYREG_ENTRY(pi, timeoutEn, resetCCAontimeout, 0)
                    MOD_PHYREG_ENTRY(pi, timeoutEn, nonpaydecodetimeoutEn, 0)
                ACPHY_REG_LIST_EXECUTE(pi);
            }
    }

    /* 4360, 4350 */
    if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
        if (CHSPEC_IS20(pi->radio_chanspec)) {
            /* reduce clip2 len, helps with humps due to late clip2 */
            WRITE_PHYREG(pi, defer_setClip1_CtrLen, 40);
            WRITE_PHYREG(pi, defer_setClip2_CtrLen, 20);
        } else if (CHSPEC_IS160(pi->radio_chanspec)) {
            /* increase clip1 len. Needed for 20in160 cases */
            WRITE_PHYREG(pi, defer_setClip1_CtrLen, 45);
            WRITE_PHYREG(pi, defer_setClip2_CtrLen, 40);
        } else {
            /* Use default value */
            WRITE_PHYREG(pi, defer_setClip1_CtrLen, 40);
            WRITE_PHYREG(pi, defer_setClip2_CtrLen, 40);
        }

        /* 43684 changes the pktgainSettleLen for a 160MHz/TDSFO-WAR */
        /* Increase settling len to prevent PER spike @ ~-62dB for 160MHz */
        if (CHSPEC_IS160(pi->radio_chanspec)) {
            WRITE_PHYREG(pi, pktgainSettleLen, 0x40);
        } else {
            WRITE_PHYREG(pi, pktgainSettleLen, 0x30);
        }
    } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        WRITE_PHYREG(pi, defer_setClip2_CtrLen, 20);
    } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
        WRITE_PHYREG(pi, defer_setClip2_CtrLen, 20);
    } else if (ACMAJORREV_51_131(pi->pubpi->phy_rev) ||
        ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* reduce clip2 len, helps with humps due to late clip2 */
        WRITE_PHYREG(pi, defer_setClip2_CtrLen, 16);
    } else  {
        if (CHSPEC_BW_LE20(pi->radio_chanspec)) {
            /* reduce clip2 len, helps with humps due to late clip2 */
            WRITE_PHYREG(pi, defer_setClip1_CtrLen, 20);
            WRITE_PHYREG(pi, defer_setClip2_CtrLen, 16);
        } else {
            /* increase clip1 len. Needed for 20in80, 40in80 cases */
            WRITE_PHYREG(pi, defer_setClip1_CtrLen, 30);
            WRITE_PHYREG(pi, defer_setClip2_CtrLen, 20);
        }
    }

    if (ACMAJORREV_2(pi->pubpi->phy_rev)) {
        WRITE_PHYREG(pi, crsThreshold2u, 0x204d);
        WRITE_PHYREG(pi, crsThreshold2l, 0x204d);
        WRITE_PHYREG(pi, crsThreshold2lSub1, 0x204d);
        WRITE_PHYREG(pi, crsThreshold2uSub1, 0x204d);
    }

    if (ACMAJORREV_5(pi->pubpi->phy_rev) ||
        ACMAJORREV_4(pi->pubpi->phy_rev)) {
        /* CRS. 6bit MF */
        /* BPHY pre-detect is disabled by default. No writes here. */
        if (CHSPEC_IS20(pi->radio_chanspec)) {
            WRITE_PHYREG(pi, crsThreshold2u, 0x2055);
            WRITE_PHYREG(pi, crsThreshold2l, 0x2055);
        } else {
            WRITE_PHYREG(pi, crsThreshold2u, 0x204d);
            WRITE_PHYREG(pi, crsThreshold2l, 0x204d);
        }
        WRITE_PHYREG(pi, crsThreshold2lSub1, 0x204d);
        WRITE_PHYREG(pi, crsThreshold2uSub1, 0x204d);
    }

    if (ACMAJORREV_5(pi->pubpi->phy_rev)) {
        /* Spur canceller */
        if (CHSPEC_IS20(pi->radio_chanspec))
            WRITE_PHYREG(pi, spur_can_phy_bw_mhz, 0x14);
        else if (CHSPEC_IS40(pi->radio_chanspec))
            WRITE_PHYREG(pi, spur_can_phy_bw_mhz, 0x280);
        else
            WRITE_PHYREG(pi, spur_can_phy_bw_mhz, 0x50);
    }

    if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {

        uint16 phymode = phy_get_phymode(pi);

        if (phymode == PHYMODE_MIMO) {
            if (CHSPEC_IS20(pi->radio_chanspec)) {
                MOD_PHYREG(pi, CRSMiscellaneousParam, crsMfFlipCoef, 0);
                ACPHYREG_BCAST(pi, crsThreshold2u0, 0x2055);
                ACPHYREG_BCAST(pi, crsThreshold2l0, 0x2055);
            } else {
                MOD_PHYREG(pi, CRSMiscellaneousParam, crsMfFlipCoef, 1);
            }
        }
    }

    if (PHY_IPA(pi) && ACMAJORREV_2(pi->pubpi->phy_rev) &&
        (ACMINORREV_3(pi) || ACMINORREV_5(pi))) {
        /* 4354a1_ipa, to decrease LOFT, move TSSI_CONFIG & extra delay before IPA_PU. Need
           to move in TSSI_CONFIG, otherwise only delaying IPA_PU would delay TSSI_CONFIG
           ;80MHz alone this change is backed out..
        */
        if (CHSPEC_IS20(pi->radio_chanspec)) {
            if (((CHSPEC_IS2G(pi->radio_chanspec)) &&
                (pi->u.pi_acphy->chanmgri->cfg->srom_tssisleep_en & 0x1)) ||
                ((CHSPEC_ISPHY5G6G(pi->radio_chanspec)) &&
                (pi->u.pi_acphy->chanmgri->cfg->srom_tssisleep_en & 0x4))) {
                    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
                        16, rfseq_rx2tx_cmd_rev15_ipa_withtssisleep);
            } else {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
                                 16, rfseq_rx2tx_cmd_rev15_ipa);
            }
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x70,
                                     16, rfseq_rx2tx_dly_rev15_ipa20);
        } else if (CHSPEC_IS40(pi->radio_chanspec)) {
            if (((CHSPEC_IS2G(pi->radio_chanspec)) &&
                (pi->u.pi_acphy->chanmgri->cfg->srom_tssisleep_en & 0x2)) ||
                ((CHSPEC_ISPHY5G6G(pi->radio_chanspec)) &&
                (pi->u.pi_acphy->chanmgri->cfg->srom_tssisleep_en & 0x8))) {
                    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
                        16, rfseq_rx2tx_cmd_rev15_ipa_withtssisleep);
            } else {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
                                 16, rfseq_rx2tx_cmd_rev15_ipa);
            }
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x70,
                                      16, rfseq_rx2tx_dly_rev15_ipa40);
        }
    }

    /* R8000 - atlas has different PA turn on timing */
    if (ACMAJORREV_0(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
        epa_turnon_time = (BOARDFLAGS(GENERIC_PHY_INFO(pi)->boardflags) &
                           BFL_SROM11_EPA_TURNON_TIME) >> BFL_SROM11_EPA_TURNON_TIME_SHIFT;
        if (epa_turnon_time == 1) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
                                      16, rfseq_rx2tx_cmd);
            if (CHSPEC_IS20(pi->radio_chanspec)) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x70,
                                          16, rfseq_rx2tx_dly_epa1_20);
            } else if (CHSPEC_IS40(pi->radio_chanspec)) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x70,
                                          16, rfseq_rx2tx_dly_epa1_40);
            } else {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x70,
                                          16, rfseq_rx2tx_dly_epa1_80);
            }
        }
    }

    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        wlc_phy_ac_lpf_cfg(pi);
    }

    if (ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        if (PHY_AS_80P80(pi, pi->radio_chanspec)) {
            phy_ac_chanmgr_switch_phymode_acphy(pi, PHYMODE_80P80);
        } else {
            phy_ac_chanmgr_switch_phymode_acphy(pi, PHYMODE_MIMO);
        }
    }

    if ((ACMAJORREV_33(pi->pubpi->phy_rev) || ACMAJORREV_47(pi->pubpi->phy_rev)) &&
            PHY_AS_80P80(pi, pi->radio_chanspec)) {
        uint32 read_val[22];
        wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_PHASETRACKTBL,
            22, 0x0, 32, &read_val);

        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PHASETRACKTBL_B,
            22, 0x0, 32, &read_val);
    }

    // JIRA SWWLAN-113796. sgi-bug
    if (ACMAJORREV_33(pi->pubpi->phy_rev)) {
        if (CHSPEC_IS20(pi->radio_chanspec))
            MOD_PHYREG(pi, FSTRCtrl, sgiLtrnAdjMax, 7);
        else
            MOD_PHYREG(pi, FSTRCtrl, sgiLtrnAdjMax, 3);
    }

    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
        /* Disable VHT proprietary MCS and 1024-QAM for phybw=20MHz */
        if (CHSPEC_IS20(pi->radio_chanspec)) {
            MOD_PHYREG(pi, HTSigTones, ldpc_proprietary_mcs_vht, 0);
            MOD_PHYREG(pi, miscSigCtrl, brcm_vht_1024qam_support, 0);
        } else {
            MOD_PHYREG(pi, HTSigTones, ldpc_proprietary_mcs_vht, 1);
            MOD_PHYREG(pi, miscSigCtrl, brcm_vht_1024qam_support, 1);
        }
    }

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        wlc_phy_set_analog_lpf(pi, lpf_txbq1, lpf_txbq2, lpf_rxbq1, lpf_rxbq2);
        MOD_PHYREG(pi, bfdsConfig, phyBandwidth, CHSPEC_BW_LE20(pi->radio_chanspec) ?
        0x0 : CHSPEC_IS40(pi->radio_chanspec) ? 0x1 : CHSPEC_IS80(pi->radio_chanspec) ?
        0x2 : 0x3);
        if (CHSPEC_IS160(pi->radio_chanspec)) {
            val = ACMAJORREV_130(pi->pubpi->phy_rev) ||
                         ACMAJORREV_131(pi->pubpi->phy_rev) ? 0 : 1;
            FOREACH_CORE(pi, core) {
                MOD_PHYREGCE(pi, sarAfeCompCtrl, core, sarAfePhaseSel, val);
            }
        }
#ifndef BCMQT
#ifdef WLVASIP
        if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, schConfig, cqiSlotSize, 1);
            ruAllocParams = (pi->radio_chanspec & WL_CHANSPEC_BW_MASK) >>
                WL_CHANSPEC_BW_SHIFT;
            ruAllocParams -= (WL_CHANSPEC_BW_20 >> WL_CHANSPEC_BW_SHIFT);
            cqi_rpt_header[0] |=
                (rpt_size_list[ruAllocParams] << CQI_RPT_HEADER_RPTSIZE_SHIFT) |
                (ruAllocParams << CQI_RPT_HEADER_PKTBW_SHIFT) |
                (ruAllocParams << CQI_RPT_HEADER_PHYBW_SHIFT);
            cqi_rpt_header[2] |= ru_end_list[ruAllocParams] <<
                CQI_RPT_HEADER_RUEND_SHIFT;

            /* get last cqi report address */
            num_cqi_rpts = 46;
            bfdLut_cqi_step = 0xb;
            mem_idx = 1;
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1,
                0x8000, 32, &mem_idx);
            mem_addr = (uint32) READ_PHYREG(pi, bfdrCqiBaseAddr);
            mem_addr += bfdLut_cqi_step* (num_cqi_rpts-1);
            mem_addr <<= 3;

            /* modify cqi report header */
            for (x = 0; x < 8; x++) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1,
                    mem_addr+x, 32, &cqi_rpt_header[x]);
            }
            /* fake all zero report */
            for (x = 8; x < bfdLut_cqi_step*16/2; x++) {
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1,
                    mem_addr+x, 32, &zero_word);
            }
        } else {
            ruAllocParams = (pi->radio_chanspec & WL_CHANSPEC_BW_MASK) >>
                WL_CHANSPEC_BW_SHIFT;
            ruAllocParams -= (WL_CHANSPEC_BW_20 >> WL_CHANSPEC_BW_SHIFT);
            ruAllocParams += CHSPEC_SB(pi->radio_chanspec) << 16;
            svmp_addr = wlapi_get_svmp_addr_sch_phybw(pi->sh->physhim);
            mem_idx = SVMPADDR2MEMIDX(svmp_addr);
            mem_addr = SVMPADDR2MEMADDR(svmp_addr);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1,
                0x8000, 32, &mem_idx);
            // write phybw lsb and prisb msb to SVMP
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1,
                mem_addr, 32, &ruAllocParams);
        }
#endif /* WLVASIP */
#endif /* BCMQT */

        /* add these registers on channel bw change: no longer going to
            be running bw_reset, rather will do a phy_reset, and so
            need to determine what bw dependent registers are needed to be set
        */
        MOD_PHYREG(pi, TssiEnRate, StrobeRateOverride, 1);
        MOD_PHYREG(pi, IqestWaitTime, waitTime,
                   CHSPEC_IS20(pi->radio_chanspec) ? 20 : 40);

        /* subband Classifer (helps in eliminating flase detect due to ACPR leakage)
           i.e 20in40/80 is not false detected as 40/80
           (these regs get reset on a bw change)
        */

        WRITE_PHYREG(pi, ClassifierLogAC1, 0x90a);
        MOD_PHYREG(pi, ClassifierCtrl6, logACDelta2, 9);

        /* these registers were explicitly set in phy_ac_tssical.c */
        MOD_PHYREG(pi, RxSdFeConfig1, farrow_rshift_tx, 0x0);
        MOD_PHYREG(pi, RxSdFeConfig1, farrow_rshift_force, 0x1);

        // This one is BW specific, and needs to set to 85 for all BWs
        FOREACH_CORE(pi, core)
            MOD_PHYREGCE(pi, crsThreshold2u, core, peakThresh, 85);
    } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        wlc_phy_set_analog_lpf(pi, lpf_txbq1, lpf_txbq2, lpf_rxbq1, lpf_rxbq2);
    }

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* Disable ChanUpd for 43684B0. HW bug for BW160 */
        if (ACMINORREV_1(pi)) {
            if (pi->u.pi_acphy->chanmgri->chanup_ovrd == -1) {
                if (CHSPEC_IS160(pi->radio_chanspec)) {
                    WRITE_PHYREG(pi, mluA, 0x0004);
                    WRITE_PHYREG(pi, zfuA, 0x0004);
                } else {
                    WRITE_PHYREG(pi, mluA, 0x2024);
                    WRITE_PHYREG(pi, zfuA, 0x2224);
                }
            } else {
                phy_ac_chanmgr_iovar_set_chanup_ovrd(pi,
                  pi->u.pi_acphy->chanmgri->chanup_ovrd);
            }
        } else {
            if (pi->u.pi_acphy->chanmgri->chanup_ovrd != -1) {
                phy_ac_chanmgr_iovar_set_chanup_ovrd(pi,
                  pi->u.pi_acphy->chanmgri->chanup_ovrd);
            }
        }

        /* 11ax has 3~5 dc gaps for phybw <= 80M,
         * to get good enough EVM for 1028QAM
         * fine tune EVM shaping table per phybw
         */
        if (CHSPEC_IS20(pi->radio_chanspec)) {
            NvAdjAXStartTone = 2;
            NvAdjAXEndTone = 10;
        } else if ((CHSPEC_IS40(pi->radio_chanspec)) ||
                (CHSPEC_IS80(pi->radio_chanspec))) {
            NvAdjAXStartTone = 3;
            NvAdjAXEndTone = 11;
        } else {
            NvAdjAXStartTone = 2;
            NvAdjAXEndTone = 11;
        }

        /* Zero out the table first */
        bzero(zeros, sizeof(zeros));
        for (x = 0; x < 256; x += ZEROS_TBL_SZ) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_NVADJ11AXTBL,
                                      ZEROS_TBL_SZ, x, 32, zeros);
        }
        for (x = NvAdjAXStartTone; x < NvAdjAXEndTone; x++) {
            if (CHSPEC_IS160(pi->radio_chanspec)) {
                NvAdjAXVal = 0;
            } else {
                NvAdjAXVal = NvAdjAXEndTone-x;
                NvAdjAXVal = NvAdjAXVal + (NvAdjAXVal << 6);
            }
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_NVADJ11AXTBL,
                1, x, 32, &NvAdjAXVal);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_NVADJ11AXTBL,
                1, 256-x, 32, &NvAdjAXVal);
        }

        // BW specific BW regs
        phy_ax_dccal_digcorr_bwspecific(pi);

        if (!ACMAJORREV_130(pi->pubpi->phy_rev)) {
            // Enable fft-in shift with BW utilization <= 12.5 %
            if (CHSPEC_IS160(pi->radio_chanspec)) {
                WRITE_PHYREG(pi, fft_backoff_cont_0, 0x1011);
                WRITE_PHYREG(pi, fft_backoff_cont_1, 0x1100);
            } else {
                WRITE_PHYREG(pi, fft_backoff_cont_0, 0x0);
                WRITE_PHYREG(pi, fft_backoff_cont_1, 0x0);
            }
        }

        if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            WRITE_PHYREG(pi, HwRssiLtfAdj,
                         CHSPEC_IS20(pi->radio_chanspec) ? 0x10d :
                         CHSPEC_IS40(pi->radio_chanspec) ? 0x49 : 0x1);
        }

        /* Avoid humps at -40 dBm */
        if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            val = CHSPEC_IS20(pi->radio_chanspec) ? 0x2108 : 0x2018;
        } else {
            val = CHSPEC_IS20(pi->radio_chanspec) ? 0x3108 : 0x2018;
        }
        FOREACH_CORE(pi, core)
            WRITE_PHYREGC(pi, Clip2Threshold, core, val);
    }

    if (ACMAJORREV_47(pi->pubpi->phy_rev) || ACMAJORREV_129(pi->pubpi->phy_rev)) {
        /* Reduce the ldpc max iteration for 43684B0 and after
           HW bug for BW160 ULOFDMA on register file overflow within LDPC decoder
        */
        if (CHSPEC_IS160(pi->radio_chanspec)) {
            MOD_PHYREG(pi, LDPCtermControl, max_iteration, 25);
        } else {
            MOD_PHYREG(pi, LDPCtermControl, max_iteration, 50);
        }
    }

    // SIFS adjustment on MAC and PHY side
    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        WRITE_PHYREG(pi, TxRealFrameDelay, 0xe2);
        wlapi_bmac_upd_ifs(pi->sh->physhim);
    }

    ACPHY_ENABLE_STALL(pi, stall_val);
}

static void
wlc_phy_set_sfo_on_chan_change_acphy(phy_info_t *pi, chanspec_t chanspec)
{
    int fc = 0;
    const uint16 *val_ptr = NULL;
    const uint16 *val_ptr1 = NULL;
    uint8 ch;

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));
    if (!TINY_RADIO(pi)) {
        if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
            const chan_info_radio20698_rffe_t *chan_info;
            fc = wlc_phy_chan2freq_20698(pi, chanspec, &chan_info);
        } else if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
            const chan_info_radio20704_rffe_t *chan_info;
            fc = wlc_phy_chan2freq_20704(pi, chanspec, &chan_info);
        } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
            const chan_info_radio20709_rffe_t *chan_info;
            fc = wlc_phy_chan2freq_20709(pi, chanspec, &chan_info);
        } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
            const chan_info_radio20707_rffe_t *chan_info;
            fc = wlc_phy_chan2freq_20707(pi, chanspec, &chan_info);
        } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            const chan_info_radio20708_rffe_t *chan_info;
            fc = wlc_phy_chan2freq_20708(pi, chanspec, &chan_info);
        } else if (ACMAJORREV_131(pi->pubpi->phy_rev)) {
            const chan_info_radio20710_rffe_t *chan_info;
            fc = wlc_phy_chan2freq_20710(pi, chanspec, &chan_info);
        } else {
            const void *chan_info;
            ch = CHSPEC_CHANNEL(chanspec);
            fc = wlc_phy_chan2freq_acphy(pi, ch, &chan_info);
        }

        if (fc >= 0) {
            wlc_phy_write_sfo_params_acphy(pi, (uint16)fc);
        }
    } else {
        ch = CHSPEC_CHANNEL(chanspec);
        if (ACMAJORREV_4(pi->pubpi->phy_rev) ||
                ACMAJORREV_32(pi->pubpi->phy_rev) ||
                ACMAJORREV_33(pi->pubpi->phy_rev)) {
            const chan_info_radio20693_pll_t *pll_tbl;
            const chan_info_radio20693_rffe_t *rffe_tbl;
            const chan_info_radio20693_pll_wave2_t *pll_tbl_wave2;

            if (phy_get_phymode(pi) != PHYMODE_80P80) {
                if (PHY_AS_80P80(pi, pi->radio_chanspec)) {
                    /* For First freq segment */
                    uint8 chans[NUM_CHANS_IN_CHAN_BONDING];
                    wf_chspec_get_80p80_channels(pi->radio_chanspec, chans);

                    if (wlc_phy_chan2freq_20693(pi, chans[0], &pll_tbl,
                            &rffe_tbl, &pll_tbl_wave2) >= 0) {
                        val_ptr = &(pll_tbl_wave2->PHY_BW1a);
                    }
                    /* For second freq segment */
                    if (wlc_phy_chan2freq_20693(pi, chans[1], &pll_tbl,
                            &rffe_tbl, &pll_tbl_wave2) >= 0) {
                        val_ptr1 = &(pll_tbl_wave2->PHY_BW1a);
                    }
                    if (val_ptr != NULL && val_ptr1 != NULL) {
                    wlc_phy_write_sfo_80p80_params_acphy(pi, val_ptr, val_ptr1);
                    } else {
                        PHY_ERROR(("wl%d: %s: CFO/SFO settings fails!\n",
                                pi->sh->unit, __FUNCTION__));
                        ASSERT(0);
                    }
                } else {
                    fc = wlc_phy_chan2freq_20693(pi, ch, &pll_tbl, &rffe_tbl,
                        &pll_tbl_wave2);
                    if (fc >= 0) {
                        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                            ACMAJORREV_33(pi->pubpi->phy_rev)) {
                            val_ptr = &(pll_tbl_wave2->PHY_BW1a);
                            wlc_phy_write_sfo_params_acphy_wave2(pi,
                                val_ptr);
                        } else {
                            wlc_phy_write_sfo_params_acphy(pi,
                                (uint16)fc);
                        }
                    }
                }
            } else {
                /* For First freq segment */
                ch = wf_chspec_primary80_channel(pi->radio_chanspec);
                fc = wlc_phy_chan2freq_20693(pi, ch, &pll_tbl, &rffe_tbl,
                    &pll_tbl_wave2);
                if (fc >= 0) {
                    if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                        ACMAJORREV_33(pi->pubpi->phy_rev)) {
                        val_ptr = &(pll_tbl_wave2->PHY_BW1a);
                        wlc_phy_write_sfo_params_acphy_wave2(pi, val_ptr);
                    } else {
                        wlc_phy_write_sfo_params_acphy(pi, (uint16)fc);
                    }
                }
                /* For second freq segment */
                ch = wf_chspec_secondary80_channel(pi->radio_chanspec);
                if (wlc_phy_chan2freq_20693(pi, ch, &pll_tbl, &rffe_tbl,
                    &pll_tbl_wave2) >= 0) {
                    if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                        ACMAJORREV_33(pi->pubpi->phy_rev)) {
                        val_ptr = &(pll_tbl_wave2->PHY_BW1a);
                    } else {
                        val_ptr = &(pll_tbl->PHY_BW1a);
                    }
                    wlc_phy_write_sfo_80p80_params_acphy(pi, val_ptr, NULL);
                }
            }
        }
    }

}

static void
wlc_phy_write_sfo_params_acphy(phy_info_t *pi, uint16 fc)
{
    uint16 phy_bw;
    uint32 tmp;
#define SFO_UNITY_FACTOR    2621440UL    /* 2.5 x 2^20 */
#define SFO_UNITY_FACTOR_ADDBITS    41943040UL    /* 2.5 x 2^24 */

    /*
     * sfo_chan_center_Ts20 = round([ fc-10e6  fc   fc+10e6] / 20e6 * 8), fc in Hz
     *                      = round([$fc-10   $fc  $fc+10] * 0.4), $fc in MHz
     */

    if (!TINY_RADIO(pi) && PHYCORENUM(pi->pubpi->phy_corenum) > 1) {
        /* BW1a */
        phy_bw = (((fc + 10) * 4) + 5) / 10;
        WRITE_PHYREG(pi, BW1a, phy_bw);

        /* BW3 */
        phy_bw = (((fc - 10) * 4) + 5) / 10;
        WRITE_PHYREG(pi, BW3, phy_bw);
    }

    /* BW2 */
    phy_bw = ((fc * 4) + 5) / 10;
    WRITE_PHYREG(pi, BW2, phy_bw);

    /*
     * sfo_chan_center_factor = round(2^17 / ([fc-10e6 fc fc+10e6]/20e6)/(ten_mhz+1)), fc in Hz
     *                        = round(2621440 ./ [$fc-10 $fc $fc+10]/($ten_mhz+1)), $fc in MHz
     * 6715 with 4 added bits:
     * sfo_chan_center_factor = round(2^21 / ([fc-10e6 fc fc+10e6]/20e6)/(ten_mhz+1)), fc in Hz
     */

    if (!TINY_RADIO(pi) && PHYCORENUM(pi->pubpi->phy_corenum) > 1) {
        /* BW4 */
        tmp = fc + 10;
        if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            phy_bw = (uint16)((SFO_UNITY_FACTOR_ADDBITS + tmp / 2) / tmp);
        } else {
            phy_bw = (uint16)((SFO_UNITY_FACTOR + tmp / 2) / tmp);
        }
        WRITE_PHYREG(pi, BW4, phy_bw);

        /* BW6 */
        tmp = fc - 10;
        if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            phy_bw = (uint16)((SFO_UNITY_FACTOR_ADDBITS + tmp / 2) / tmp);
        } else {
            phy_bw = (uint16)((SFO_UNITY_FACTOR + tmp / 2) / tmp);
        }
        WRITE_PHYREG(pi, BW6, phy_bw);
    }

    /* BW5 */
    tmp = fc;
    if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        phy_bw = (uint16)((SFO_UNITY_FACTOR_ADDBITS + tmp / 2) / tmp);
    } else {
        phy_bw = (uint16)((SFO_UNITY_FACTOR + tmp / 2) / tmp);
    }
    WRITE_PHYREG(pi, BW5, phy_bw);
}

static void
wlc_phy_write_sfo_params_acphy_wave2(phy_info_t *pi, const uint16 *val_ptr)
{
    ASSERT(val_ptr != NULL);
    if (val_ptr != NULL) {
        /* set SFO parameters */
        WRITE_PHYREG(pi, BW1a, val_ptr[0]);
        WRITE_PHYREG(pi, BW2, val_ptr[1]);
        WRITE_PHYREG(pi, BW3, val_ptr[2]);
        /* Set sfo_chan_center_factor */
        WRITE_PHYREG(pi, BW4, val_ptr[3]);
        WRITE_PHYREG(pi, BW5, val_ptr[4]);
        WRITE_PHYREG(pi, BW6, val_ptr[5]);
    }
}

static void
wlc_phy_write_sfo_80p80_params_acphy(phy_info_t *pi, const uint16 *val_ptr, const uint16 *val_ptr1)
{
    ASSERT(val_ptr != NULL);
    if (val_ptr != NULL) {
        if (ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            ASSERT(val_ptr1 != NULL);
            /* set SFO parameters */
            WRITE_PHYREG(pi, BW1a0, val_ptr[0]);
            WRITE_PHYREG(pi, BW1a1, val_ptr[0]);
            WRITE_PHYREG(pi, BW1a2, val_ptr1[0]);
            WRITE_PHYREG(pi, BW1a3, val_ptr1[0]);
            WRITE_PHYREG(pi, BW20, val_ptr[1]);
            WRITE_PHYREG(pi, BW21, val_ptr[1]);
            WRITE_PHYREG(pi, BW22, val_ptr1[1]);
            WRITE_PHYREG(pi, BW23, val_ptr1[1]);
            WRITE_PHYREG(pi, BW30, val_ptr[2]);
            WRITE_PHYREG(pi, BW31, val_ptr[2]);
            WRITE_PHYREG(pi, BW32, val_ptr1[2]);
            WRITE_PHYREG(pi, BW33, val_ptr1[2]);
            /* Set sfo_chan_center_factor */
            WRITE_PHYREG(pi, BW40, val_ptr[3]);
            WRITE_PHYREG(pi, BW41, val_ptr[3]);
            WRITE_PHYREG(pi, BW42, val_ptr1[3]);
            WRITE_PHYREG(pi, BW43, val_ptr1[3]);
            WRITE_PHYREG(pi, BW50, val_ptr[4]);
            WRITE_PHYREG(pi, BW51, val_ptr[4]);
            WRITE_PHYREG(pi, BW52, val_ptr1[4]);
            WRITE_PHYREG(pi, BW53, val_ptr1[4]);
            WRITE_PHYREG(pi, BW60, val_ptr[5]);
            WRITE_PHYREG(pi, BW61, val_ptr[5]);
            WRITE_PHYREG(pi, BW62, val_ptr1[5]);
            WRITE_PHYREG(pi, BW63, val_ptr1[5]);
        } else {
            /* set SFO parameters */
            WRITE_PHYREG(pi, BW1a1, val_ptr[0]);
            WRITE_PHYREG(pi, BW21, val_ptr[1]);
            WRITE_PHYREG(pi, BW31, val_ptr[2]);
            /* Set sfo_chan_center_factor */
            WRITE_PHYREG(pi, BW41, val_ptr[3]);
            WRITE_PHYREG(pi, BW51, val_ptr[4]);
            WRITE_PHYREG(pi, BW61, val_ptr[5]);
        }
    }
}

static void
chanspec_set_primary_chan(phy_info_t * pi)
{
    if (CHSPEC_IS8080(pi->radio_chanspec) || CHSPEC_IS160(pi->radio_chanspec)) {
        /* 80P80 */
        uint16 param1 = 0;
        uint16 param2 = 0;

        switch (CHSPEC_CTL_SB(pi->radio_chanspec)) {
        case WL_CHANSPEC_CTL_SB_LLL:
            /* param1 initialized to 0 */
            /* param2 initialized to 0 */
            break;
        case WL_CHANSPEC_CTL_SB_LLU:
            /* param1 initialized to 0 */
            param2 = 1;
            break;
        case WL_CHANSPEC_CTL_SB_LUL:
            /* param1 initialized to 0 */
            param2 = 2;
            break;
        case WL_CHANSPEC_CTL_SB_LUU:
            /* param1 initialized to 0 */
            param2 = 3;
            break;
        case WL_CHANSPEC_CTL_SB_ULL:
            param1 = 1;
            /* param2 initialized to 0 */
            break;
        case WL_CHANSPEC_CTL_SB_ULU:
            param1 = 1;
            param2 = 1;
            break;
        case WL_CHANSPEC_CTL_SB_UUL:
            param1 = 1;
            param2 = 2;
            break;
        case WL_CHANSPEC_CTL_SB_UUU:
            param1 = 1;
            param2 = 3;
            break;
        default:
            PHY_ERROR(("%s: No primary channel settings for CTL_SB=%d\n",
                       __FUNCTION__, CHSPEC_CTL_SB(pi->radio_chanspec)));
            ASSERT(0);
        }
        MOD_PHYREG(pi, ClassifierCtrl_80p80, prim_sel_hi, param1);
        MOD_PHYREG(pi, ClassifierCtrl2, prim_sel, param2);

    } else if (CHSPEC_IS80(pi->radio_chanspec)) {
        /* 80mhz */
        uint16 param = 0;

        switch (CHSPEC_CTL_SB(pi->radio_chanspec)) {
        case WL_CHANSPEC_CTL_SB_LL:
            /* param initialized to 0 */
            break;
        case WL_CHANSPEC_CTL_SB_LU:
            param = 1;
            break;
        case WL_CHANSPEC_CTL_SB_UL:
            param = 2;
            break;
        case WL_CHANSPEC_CTL_SB_UU:
            param = 3;
            break;
        default:
            PHY_ERROR(("%s: No primary channel settings for CTL_SB=%d\n",
                       __FUNCTION__, CHSPEC_CTL_SB(pi->radio_chanspec)));
            ASSERT(0);
        }
        MOD_PHYREG(pi, ClassifierCtrl2, prim_sel, param);

    } else if (CHSPEC_IS40(pi->radio_chanspec)) {
        /* 40mhz */
        uint16 param;
        if (CHSPEC_SB_UPPER(pi->radio_chanspec)) {
            param = 1;
        } else {
            param = 0;
        }
        MOD_PHYREG(pi, RxControl, bphy_band_sel, param);
        MOD_PHYREG(pi, ClassifierCtrl2, prim_sel, param);

    } else if (CHSPEC_BW_LE20(pi->radio_chanspec)) {
        /* 20mhz */
        MOD_PHYREG(pi, RxControl, bphy_band_sel, 0);
        MOD_PHYREG(pi, ClassifierCtrl2, prim_sel, 0);
    } else {
        PHY_ERROR(("%s: No primary channel settings for bw=%d\n",
                   __FUNCTION__, CHSPEC_BW(pi->radio_chanspec)));
        ASSERT(0);
    }
}

/*
 * making IIR filter gaussian like for BPHY to improve ACPR ACMajor 40
 */
static void
chanspec_make_iir_filter_gaussian_acmajor_40_128(phy_info_t *pi)
{
    uint8 bphy_testmode_val = (0x3F & READ_PHYREGFLD(pi, bphyTest, testMode));
    bphy_testmode_val = bphy_testmode_val |
        ((pi->u.pi_acphy->chanmgri->acphy_cck_dig_filt_type & 0x2)  << 5);
    MOD_PHYREG(pi, bphyTest, testMode, bphy_testmode_val);
    MOD_PHYREG(pi, bphyTest, FiltSel2,
        ((pi->u.pi_acphy->chanmgri->acphy_cck_dig_filt_type & 0x4) >> 2));
    if ((pi->u.pi_acphy->chanmgri->acphy_cck_dig_filt_type == 0) ||
        (pi->u.pi_acphy->chanmgri->acphy_cck_dig_filt_type == 2)) {
        wlc_phy_set_tx_iir_coeffs(pi, 1, 4);
        if (pi->u.pi_acphy->chanmgri->acphy_cck_dig_filt_type == 0) {
            MOD_PHYREG(pi, BphyControl3, bphyScale20MHz, 0x2F);
        } else {
            MOD_PHYREG(pi, BphyControl3, bphyScale20MHz, 0x2A);
        }
    } else {
        wlc_phy_set_tx_iir_coeffs(pi, 1, 5);
        MOD_PHYREG(pi, BphyControl3, bphyScale20MHz, 0x37);
    }
}

/*
 * making IIR filter gaussian like for BPHY to improve ACPR ACMajor 4
 */
static void
chanspec_make_iir_filter_gaussian_acmajor_4(phy_info_t *pi, int fc)
{

    {
        uint16 bphy_testmode_val;
        uint16 param2;
        if (!PHY_IPA(pi) && !ROUTER_4349(pi)) {
            bphy_testmode_val = (0x3F & READ_PHYREGFLD(pi, bphyTest, testMode));
            param2 = 0u;
        } else {
            bphy_testmode_val = 0u;
            param2 = 1u;
        }
        MOD_PHYREG(pi, bphyTest, testMode, bphy_testmode_val);
        MOD_PHYREG(pi, bphyTest, FiltSel2, param2);
    }

    wlc_phy_set_tx_iir_coeffs(pi, 1, pi->sromi->cckfilttype);
    wlc_phy_set_tx_iir_coeffs(pi, 0, 0); /* default setting for ofdm */

    /* JIRA: SW4349-1379
     * FOR ePA chips, to improve CCK spectral mask margins, bphy scale
     * is increased to 0x5F. this improves SM margins by >2dB, with an
     * acceptable degradation in the EVM. This change also mandates the
     * use of separate loop for cck TPC (target power control)
     */
    {
        uint16 bphy_scale_val;
        if (!PHY_IPA(pi) && !ROUTER_4349(pi)) {
            bphy_scale_val = 0x4Du;
        } else if (ROUTER_4349(pi)) {
            /* bphyScale to equalize gain between cck and ofdm frames */
            bphy_scale_val = 0x23u;
        } else {
            bphy_scale_val = 0x3Bu;
        }
        MOD_PHYREG(pi, BphyControl3, bphyScale20MHz, bphy_scale_val);
    }

    if (((fc == 2412) || (fc == 2462) || (fc == 2467) ||
            (fc == 2472)) &&
        (pi->sromi->ofdmfilttype_2g != 127)) {
        wlc_phy_set_tx_iir_coeffs(pi, 0,
                pi->sromi->ofdmfilttype_2g);
    } else if (((fc == 5240) || (fc == 5260) || (fc == 5580) ||
            (fc == 5660)) &&
        (pi->sromi->ofdmfilttype != 127)) {
        wlc_phy_set_tx_iir_coeffs(pi, 0, pi->sromi->ofdmfilttype);
    }
}

/*
 * making IIR filter gaussian like for BPHY to improve ACPR
 */
static void
chanspec_make_iir_filter_gaussian(phy_info_t *pi, int fc)
{
    /* making IIR filter gaussian like for BPHY to improve ACPR */

    /* set RRC filter alpha
     FiltSel2 is 11 bit which msb, bphyTest's 6th bit is lsb
     These 2 bits control alpha
     bits 11 & 6    Resulting filter
      -----------    ----------------
          00         alpha=0.35 - default
          01         alpha=0.75 - alternate
          10         alpha=0.2  - for use in Japan on channel 14
          11         no TX filter
    */

    uint8 cck_filt_type;
    uint8 bphy_testmode_val = (0x3F & READ_PHYREGFLD(pi, bphyTest, testMode));
    bool cck_dig_filt_bflg4 =
        ((pi->sh->boardflags4 & BFL4_SROM13_CCK_DIG_FILT) != 0);
    if ((fc == 2484) && (!CHSPEC_IS8080(pi->radio_chanspec))) {
        MOD_PHYREG(pi, bphyTest, testMode, bphy_testmode_val);
        MOD_PHYREG(pi, bphyTest, FiltSel2, 1);
        /* Load default filter */
        wlc_phy_set_tx_iir_coeffs(pi, 1, 0);
    } else {
        if (ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, bphyTest, FiltSel2, 0);
            if (PHY_IPA(pi)) {
                wlc_phy_set_tx_iir_coeffs(pi, 1, 2);
            } else {
                wlc_phy_set_tx_iir_coeffs(pi, 1, 1);
            }
        } else if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                   ACMAJORREV_33(pi->pubpi->phy_rev) ||
                   ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
            if (cck_dig_filt_bflg4 == TRUE) {
                cck_filt_type = 1;
            } else {
                cck_filt_type = 2;
            }

            bphy_testmode_val |= (cck_filt_type & 0x2)  << 5;
            MOD_PHYREG(pi, bphyTest, testMode, bphy_testmode_val);
            MOD_PHYREG(pi, bphyTest, FiltSel2, 0);
            wlc_phy_set_tx_iir_coeffs(pi, 1, cck_filt_type);
            if (fc == 2472) {
                /* Narrower filter for ch13 - ofdm */
                wlc_phy_set_tx_iir_coeffs(pi, 0, 4);
            }
        } else if ACMAJORREV_4(pi->pubpi->phy_rev) {
            chanspec_make_iir_filter_gaussian_acmajor_4(pi, fc);
        } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
            chanspec_make_iir_filter_gaussian_acmajor_40_128(pi);
        } else {
            bphy_testmode_val = bphy_testmode_val |
                ((pi->u.pi_acphy->chanmgri->acphy_cck_dig_filt_type & 0x2)  << 5);
            MOD_PHYREG(pi, bphyTest, testMode, bphy_testmode_val);
            MOD_PHYREG(pi, bphyTest, FiltSel2,
                ((pi->u.pi_acphy->chanmgri->acphy_cck_dig_filt_type & 0x4) >> 2));
            /* Load filter with Gaussian shaping */
            wlc_phy_set_tx_iir_coeffs(pi, 1,
                (pi->u.pi_acphy->chanmgri->acphy_cck_dig_filt_type & 0x1));
        }
    }
}

static void
wlc_phy_farrow_setup_nontiny(phy_info_t * pi)
{
    uint8 use_ovr = (ISSIM_ENAB(pi->sh->sih) && !ACMAJORREV_131(pi->pubpi->phy_rev)) ? 1 : 0;

    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        wlc_phy_farrow_setup_28nm(pi, 1 /* DAC rate mode */ );
    } else if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
        wlc_phy_farrow_setup_28nm(pi, 1 /* DAC rate mode */ );
        /* Configure AFE div */
        wlc_phy_radio20698_afe_div_ratio(pi, use_ovr, 0, 0);
    } else if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
        wlc_phy_farrow_setup_28nm(pi, 1 /* DAC rate mode */ );
        /* Configure AFE div */
        wlc_phy_radio20704_afe_div_ratio(pi, use_ovr);
    } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
        wlc_phy_farrow_setup_28nm(pi, 1 /* DAC rate mode */ );
        /* Configure AFE div */
        wlc_phy_radio20707_afe_div_ratio(pi, use_ovr);
    } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        wlc_phy_farrow_setup_28nm(pi, 1 /* DAC rate mode */ );
        /* Configure AFE div */
        wlc_phy_radio20708_afe_div_ratio(pi, 1, 0, 0, 0, 1, 0);
    } else if (ACMAJORREV_131(pi->pubpi->phy_rev)) {
        wlc_phy_farrow_setup_28nm(pi, 1 /* DAC rate mode */ );
        /* Configure AFE div */
        wlc_phy_radio20710_afe_div_ratio(pi, use_ovr, 0);
    } else {
        wlc_phy_farrow_setup_acphy(pi, pi->radio_chanspec);
    }
}

/*
 * JIRA (HW11ACRADIO-30)
 *
 * clamp_en needs to be high for ~1us for clipped pkts (80mhz)
 */
static void
war_jira_hw11acradio_30(phy_info_t * pi, int fc)
{
    uint8 core = 0;

    if ((CHSPEC_IS80(pi->radio_chanspec) ||
         (CHSPEC_IS40(pi->radio_chanspec) && (RADIO2069_MAJORREV(pi->pubpi->radiorev) == 2) &&
          !PHY_IPA(pi) && (fc == 5190))) &&
        !TINY_RADIO(pi) &&
        !ACMAJORREV_GE37(pi->pubpi->phy_rev)) {

        uint16 rfseq_bundle_adcrst48[3];
        uint16 rfseq_bundle_adcrst49[3];
        uint16 rfseq_bundle_adcrst50[3];
        uint8 phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;

        BCM_REFERENCE(phyrxchain);
        FOREACH_ACTV_CORE(pi, phyrxchain, core) {
            MOD_PHYREGCE(pi, RfctrlCoreAfeCfg2, core, afe_iqadc_clamp_en, 1);
            MOD_PHYREGCE(pi, RfctrlOverrideAfeCfg, core, afe_iqadc_clamp_en, 1);
        }

        /* FIXME: For now only 80mhz. Later enable it for 20/40 also. Issue iqadc reset
           along with clamp_en as clamp_en is not enough to take adc out of oscillation
           Replace sd_adc_assert/deassert with this bundle command for reseting the adc
        */
        rfseq_bundle_adcrst48[2]  = 0;
        rfseq_bundle_adcrst49[2]  = 0;
        rfseq_bundle_adcrst50[2]  = 0;
        if (CHSPEC_IS20(pi->radio_chanspec)) {
            rfseq_bundle_adcrst48[0] = 0xef52;
            rfseq_bundle_adcrst48[1] = 0x94;
            rfseq_bundle_adcrst49[0] = 0xef42;
            rfseq_bundle_adcrst49[1] = 0x84;
            rfseq_bundle_adcrst50[0] = 0xef52;
            rfseq_bundle_adcrst50[1] = 0x84;
        } else if (! CHSPEC_IS40(pi->radio_chanspec) ||
                ((RADIO2069_MAJORREV(pi->pubpi->radiorev) == 2) && !PHY_IPA(pi) &&
                (fc == 5190))) {
            rfseq_bundle_adcrst48[0] = 0x0fd2;
            rfseq_bundle_adcrst48[1] = 0x96;
            rfseq_bundle_adcrst49[0] = 0x0fc2;
            rfseq_bundle_adcrst49[1] = 0x86;
            rfseq_bundle_adcrst50[0] = 0x0fd2;
            rfseq_bundle_adcrst50[1] = 0x86;
        } else {
            rfseq_bundle_adcrst48[0] = 0x4f52;
            rfseq_bundle_adcrst48[1] = 0x94;
            rfseq_bundle_adcrst49[0] = 0x4f42;
            rfseq_bundle_adcrst49[1] = 0x84;
            rfseq_bundle_adcrst50[0] = 0x4f52;
            rfseq_bundle_adcrst50[1] = 0x84;
        }

        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 48, 48,
                                  rfseq_bundle_adcrst48);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 49, 48,
                                  rfseq_bundle_adcrst49);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 50, 48,
                                  rfseq_bundle_adcrst50);
        /* reduce the adc reset time from 250ns to 50ns for 43602 as it caused CSTR failure
        * when ADC clips during clip gain
        */
        if (ACMAJORREV_5(pi->pubpi->phy_rev)) {
            rf_updh_dly_adcrst[4] = 0x2;
            rf_updl_dly_adcrst[4] = 0x2;
        }
        /* updategainH : issue adc reset for 250ns */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0x30, 16, rf_updh_cmd_adcrst);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0xa0, 16, rf_updh_dly_adcrst);

        /* updategainL : issue adc reset for 250ns */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0x40, 16, rf_updl_cmd_adcrst);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0xb0, 16, rf_updl_dly_adcrst);

        /* updategainU : issue adc reset for 250n */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0x50, 16, rf_updu_cmd_adcrst);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0xc0, 16, rf_updu_dly_adcrst);
    } else {
        if (!ACMAJORREV_32(pi->pubpi->phy_rev) && !ACMAJORREV_33(pi->pubpi->phy_rev)) {
            uint8 phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;
            BCM_REFERENCE(phyrxchain);
            FOREACH_ACTV_CORE(pi, phyrxchain, core) {
                /*
                 * 4360A0 : SD-ADC was not monotonic for 1st revision
                 * but is fixed now
                 */
                if (ACREV_IS(pi->pubpi->phy_rev, 0)) {
                    MOD_PHYREGCE(pi, RfctrlCoreAfeCfg2, core,
                        afe_iqadc_clamp_en, 0);
                } else {
                    MOD_PHYREGCE(pi, RfctrlCoreAfeCfg2, core,
                        afe_iqadc_clamp_en, 1);
                }
                MOD_PHYREGCE(pi, RfctrlOverrideAfeCfg, core, afe_iqadc_clamp_en, 1);
            }

            if (pi->pubpi->phy_rev < 40) {
                /* updategainH : increase clamp_en off delay to 16 */
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0x30,
                16, rf_updh_cmd_clamp);
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0xa0,
                16, rf_updh_dly_clamp);

                /* updategainL : increase clamp_en off delay to 16 */
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0x40,
                16, rf_updl_cmd_clamp);
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0xb0,
                16, rf_updl_dly_clamp);

                /* updategainU : increase clamp_en off delay to 16 */
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0x50,
                16, rf_updu_cmd_clamp);
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8, 0xc0,
                16, rf_updu_dly_clamp);
            }
        }
    }
}

/*
 * AFE clk and Harmonic of 40 MHz crystal causes a spur at 417 Khz
 */
static void
war_4350_spur_417mhz(phy_info_t * pi, int fc)
{
    uint32 rx_afediv_sel, tx_afediv_sel;
    uint32 read_val[2], write_val[2];
    phy_ac_radio_data_t *radio_data = phy_ac_radio_get_data(pi->u.pi_acphy->radioi);

    /* AFE clk and Harmonic of 40 MHz crystal causes a spur at 417 Khz */
    if (CHSPEC_IS20(pi->radio_chanspec)) {
        if ((radio_data->srom_txnospurmod2g == 0) &&
            (CHSPEC_IS2G(pi->radio_chanspec)) && !PHY_IPA(pi)) {
            /* AFE divider of 4.5 */
            /* i_iqadc_adc_bias 2 */
            /* iqmode 20 */
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 6, 60,
                                     &read_val);
            rx_afediv_sel = (read_val[0] & ~(0x7 << 14) & ~(0x3 << 11) &
                            ~(0x3) & 0xfffff) |
                (0x2 << 14 | 0x2 << 11 | 0x3);
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 0, 60,
                                     &read_val);
            tx_afediv_sel = (read_val[0] & ~(0x7 << 14) & ~(0x3 << 11) &
                             ~(0x3) & 0xfffff) |
                (0x2 << 14 | 0x2 << 11 | 0x3);
        } else if ((CHSPEC_IS2G(pi->radio_chanspec) &&
            ((fc != 2412 && fc != 2467) || (pi->xtalfreq == 40000000) ||
            (ACMAJORREV_2(pi->pubpi->phy_rev) &&
            (ACMINORREV_1(pi) ||
            ACMINORREV_3(pi) ||
            ACMINORREV_5(pi)) &&
            pi->xtalfreq == 37400000 && PHY_ILNA(pi)))) ||
            (fc == 5745) || (fc == 5765) || (fc == 5825 && !PHY_IPA(pi)) ||
            ((fc == 5180) && ((((RADIOMINORREV(pi) == 4) ||
            (RADIOMINORREV(pi) == 10) ||
            (RADIOMINORREV(pi) == 11) ||
            (RADIOMINORREV(pi) == 13)) &&
            (pi->sh->chippkg == 2)) ||
            (RADIOMINORREV(pi) == 7) ||
            (RADIOMINORREV(pi) == 9) ||
            (RADIOMINORREV(pi) == 8) ||
            (RADIOMINORREV(pi) == 12)) &&
            (pi->xtalfreq == 37400000))) {
            /* if AFE divider of 8 is used for 20 MHz channel 149,153, */
            /* or any channel in 2GHz when xtalfreq=40MHz, */
            /* or any 2Ghz channel except 2467 when xtalfreq=37.4MHz */
            /* so change divider ratio to 9 */
            /* i_iqadc_adc_bias 2 */
            /* iqmode 20 */
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 6, 60,
                                     &read_val);
            rx_afediv_sel = (read_val[0] & ~(0x7 << 14) & ~(0x3 << 11) &
                             ~(0x3) & 0xfffff) |
                (0x4 << 14 | 0x2 << 11 | 0x3);
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 0, 60,
                                     &read_val);
            tx_afediv_sel = (read_val[0] & ~(0x7 << 14) & ~(0x3 << 11) &
                             ~(0x3) & 0xfffff) |
                (0x4 << 14 | 0x2 << 11 | 0x3);
        } else {
            /* AFE divider of 8 */
            /* i_iqadc_adc_bias 2 */
            /* iqmode 20 */
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 6, 60,
                                     &read_val);
            rx_afediv_sel = (read_val[0] & ~(0x7 << 14) & ~(0x3 << 11) &
                             ~(0x3) & 0xfffff) |
                (0x3 << 14 | 0x2 << 11 | 0x3);
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 0, 60,
                                     &read_val);
            tx_afediv_sel = (read_val[0] & ~(0x7 << 14) & ~(0x3 << 11) &
                             ~(0x3) & 0xfffff) |
                (0x3 << 14 | 0x2 << 11 | 0x3);
        }
        if (!PHY_IPA(pi) && CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
            if ((((fc == 5180) && (pi->sh->chippkg != 2)) ||
                 ((fc >= 5200) && (fc <= 5320)) ||
                 ((fc >= 5745) && (fc <= 5825)))) {
                /* AFE div 5 for tx/rx  (bits 13:15) */
                /* i_iqadc_adc_bias 0 (bits 11:12) for stability */
                /* iqmode 40 (bits 0:2) to fix TSSI issue */
                wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 6,
                                         60, &read_val);
                rx_afediv_sel = (read_val[0] & ~(0x7 << 14) & ~(0x3 << 11) &
                                 ~(0x3) & 0xfffff) |
                    (0x7 << 14 | 0x0 << 11 | 0x2);
                wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 0,
                                         60, &read_val);
                tx_afediv_sel = (read_val[0] & ~(0x7 << 14) & ~(0x3 << 11) &
                                 ~(0x3) & 0xfffff) |
                    (0x7 << 14 | 0x0 << 11 | 0x2);
            } else {
                /* AFE div 8 for tx/rx */
                /* i_iqadc_adc_bias 2 */
                /* iqmode 20 */
                wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 6,
                                         60, &read_val);
                rx_afediv_sel = (read_val[0] & ~(0x7 << 14) & ~(0x3 << 11) &
                                 ~(0x3) & 0xfffff) |
                    (0x3 << 14 | 0x2 << 11 | 0x3);
                wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 0,
                                         60, &read_val);
                tx_afediv_sel = (read_val[0] & ~(0x7 << 14) & ~(0x3 << 11) &
                                 ~(0x3) & 0xfffff) |
                    (0x3 << 14 | 0x2 << 11 | 0x3);
            }
        }
        /* RX_SD_ADC_PU_VAL bw20 */
        write_val[0] = ((rx_afediv_sel & 0xfff) << 20) | rx_afediv_sel;
        write_val[1] = (rx_afediv_sel << 8) | (rx_afediv_sel >> 12);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 6, 60,
                                  write_val);
        /* bw20_HighspeedMode1 */
        write_val[0] = ((tx_afediv_sel & 0xfff) << 20) | tx_afediv_sel;
        write_val[1] = (tx_afediv_sel << 8) | (tx_afediv_sel >> 12);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 0, 60,
                                  write_val);
        wlc_phy_force_rfseq_noLoleakage_acphy(pi);
    } else if (CHSPEC_IS40(pi->radio_chanspec)) {
        /* if AFE divider of 4 is used for 40 MHz channel 151m,
         * so change divider ratio to 4.5
         */
        if (CHSPEC_ISPHY5G6G(pi->radio_chanspec) &&
            !PHY_IPA(pi) && (fc != 5190)) {
            /* AFE div 5 for tx/rx */
            rx_afediv_sel = (sdadc_cfg40 & ~(0x7 << 14) & 0xfffff) |
                (0x7 << 14) | (0x3 << 17);
            tx_afediv_sel = (sdadc_cfg40 & ~(0x7 << 14) & 0xfffff) |
                (0x7 << 14);
        } else if (CHSPEC_ISPHY5G6G(pi->radio_chanspec) &&
                   !PHY_IPA(pi) && (fc == 5190)) {
            /* AFE div 3 for tx/rx, bw80 mode */
            rx_afediv_sel = (sdadc_cfg80 & ~(0x7 << 14) & 0xfffff) |
                (0x0 << 14) | (0x3 << 17);
            tx_afediv_sel = (sdadc_cfg80 & ~(0x7 << 14) & 0xfffff) |
                (0x0 << 14);
        } else if ((CHSPEC_IS2G(pi->radio_chanspec)) || (fc == 5755) ||
            (fc == 5550 && pi->xtalfreq == 40000000) ||
            (fc == 5310 && pi->xtalfreq == 37400000 && PHY_IPA(pi))) {
            /* AFE div 4.5 for tx/rx */
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 7, 60,
                                     &read_val);
            rx_afediv_sel = (read_val[0] & ~(0x7 << 14) & 0xfffff) |
                (0x2 << 14);
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 1, 60,
                                     &read_val);
            tx_afediv_sel = (read_val[0] & ~(0x7 << 14) & 0xfffff) |
                    (0x2 << 14);
        } else {
            /* AFE div 4 for tx/rx */
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 7, 60,
                                     &read_val);
            rx_afediv_sel = (read_val[0] & ~(0x7 << 14) & 0xfffff) |
                (0x1 << 14);
            wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 1, 60,
                                     &read_val);
            tx_afediv_sel = (read_val[0] & ~(0x7 << 14) & 0xfffff) |
                (0x1 << 14);
        }
        /* RX_SD_ADC_PU_VAL bw40 */
        write_val[0] = ((rx_afediv_sel & 0xfff) << 20) | rx_afediv_sel;
        write_val[1] = (rx_afediv_sel << 8) | (rx_afediv_sel >> 12);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 7, 60,
                                  write_val);
        /* bw40_HighspeedMode1 */
        write_val[0] = ((tx_afediv_sel & 0xfff) << 20) | tx_afediv_sel;
        write_val[1] = (tx_afediv_sel << 8) | (tx_afediv_sel >> 12);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 1, 60,
                                  write_val);
        wlc_phy_force_rfseq_noLoleakage_acphy(pi);
    }
}

static void
chanspec_setup_regtbl_on_chan_change(phy_info_t *pi)
{
    bool suspend = 0;
    uint8 stall_val = 0, orig_rxfectrl1 = 0;
    chanspec_t chspec[NUM_CHANS_IN_CHAN_BONDING];
    uint8 core = 0;
    phy_ac_radio_data_t *radio_data = phy_ac_radio_get_data(pi->u.pi_acphy->radioi);

    /* get the center freq */
    int fc;

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
    /* see BCAWLAN-214648, using chip-nonspecific method for obtaining FC */
        fc = wf_channel2mhz(CHSPEC_CHANNEL(pi->radio_chanspec),
            CHSPEC_IS2G(pi->radio_chanspec) ? WF_CHAN_FACTOR_2_4_G
            : CHSPEC_IS5G(pi->radio_chanspec) ? WF_CHAN_FACTOR_5_G
            : WF_CHAN_FACTOR_6_G);
    } else {
        fc = radio_data->fc;
    }

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    phy_ac_chanmgr_get_operating_chanspecs(pi, chspec);

    /* -ve freq means channel not found in tuning table */
    if (fc < 0) {
        return;
    }

    PHY_CHANLOG(pi, __FUNCTION__, TS_ENTER, 0);

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) && CHSPEC_IS6G(pi->radio_chanspec)) {
        /* Need to call rfseqext on chan change since 6G rfseqext is channel dependent */
        wlc_phy_set_rfseqext_tbl(pi, pi->u.pi_acphy->sromi->srom_low_adc_rate_en);
    }

    /* Setup the Tx/Rx Farrow resampler */
    if (TINY_RADIO(pi)) {
        wlc_phy_farrow_setup_tiny(pi, pi->radio_chanspec);
    } else {
        wlc_phy_farrow_setup_nontiny(pi);
    }

    /* Load Pdet related settings */
    wlc_phy_set_pdet_on_reset_acphy(pi);

    if (!ACMAJORREV_32(pi->pubpi->phy_rev) &&
        !ACMAJORREV_33(pi->pubpi->phy_rev)) {
        suspend = !(R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC);
        if (!suspend)
            wlapi_suspend_mac_and_wait(pi->sh->physhim);

        /* Disable stalls and hold FIFOs in reset */
        stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
        orig_rxfectrl1 = READ_PHYREGFLD(pi, RxFeCtrl1, soft_sdfeFifoReset);

        ACPHY_DISABLE_STALL(pi);
        MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, 1);
    }

    /* 4350A0 radio */
    if ((RADIOID_IS(pi->pubpi->radioid, BCM2069_ID)) &&
        (RADIO2069_MAJORREV(pi->pubpi->radiorev) == 2) &&
        !(ISSIM_ENAB(pi->sh->sih))) {
        war_4350_spur_417mhz(pi, fc);
    }

    /* JIRA (HW11ACRADIO-30) - clamp_en needs to be high for ~1us for clipped pkts (80mhz) */
    war_jira_hw11acradio_30(pi, fc);

    if (ACMAJORREV_2(pi->pubpi->phy_rev) && !(PHY_IPA(pi))) {
        uint16 rfseq_bundle_adcdacoff51[3];
        /* Add AFE Power down to RFSeq */

        if (CHSPEC_IS2G(pi->radio_chanspec)) {
            rfseq_bundle_adcdacoff51[0] = 0xef72;
            rfseq_bundle_adcdacoff51[1] = 0x84;
        } else {
            if (CHSPEC_IS20(pi->radio_chanspec)) {
                if (((fc >= 5180) && (fc <= 5320)) ||
                ((fc >= 5745) && (fc <= 5825))) {
                    rfseq_bundle_adcdacoff51[0] = 0x8f72;
                    rfseq_bundle_adcdacoff51[1] = 0x84;
                } else {
                    rfseq_bundle_adcdacoff51[0] = 0xef72;
                    rfseq_bundle_adcdacoff51[1] = 0x84;
                }
            } else if (CHSPEC_IS40(pi->radio_chanspec)) {
                if (fc == 5190) {
                    rfseq_bundle_adcdacoff51[0] = 0x0ff2;
                    rfseq_bundle_adcdacoff51[1] = 0x86;
                } else {
                    rfseq_bundle_adcdacoff51[0] = 0x8f72;
                    rfseq_bundle_adcdacoff51[1] = 0x84;
                }
            } else {
                rfseq_bundle_adcdacoff51[0] = 0x0ff2;
                rfseq_bundle_adcdacoff51[1] = 0x86;
            }
        }
        /* Below bundle shuts off all DACs at the beginning of TX2RX sequence */
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQBUNDLE, 1, 51, 48,
        rfseq_bundle_adcdacoff51);

        wlc_phy_set_sdadc_pd_val_per_core_acphy(pi);
    }

    if (!ACMAJORREV_32(pi->pubpi->phy_rev) && !ACMAJORREV_33(pi->pubpi->phy_rev)) {
        if (!suspend)
            wlapi_enable_mac(pi->sh->physhim);

        /* Restore FIFO reset and Stalls */
        ACPHY_ENABLE_STALL(pi, stall_val);
        MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, orig_rxfectrl1);
    }

    wlc_phy_set_sfo_on_chan_change_acphy(pi, chspec[0]);

    /* Set the correct primary channel */
    chanspec_set_primary_chan(pi);

    /* set aci thresholds */
    wlc_phy_set_aci_regs_acphy(pi);

    /* making IIR filter gaussian like for BPHY to improve ACPR */
    chanspec_make_iir_filter_gaussian(pi, fc);

    /* if it's 2x2 or 3x3 design, populate the reciprocity compensation coeff */
    if (ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) || ACMAJORREV_GE37(pi->pubpi->phy_rev)) {
            wlc_phy_populate_recipcoeffs_acphy(pi);
    }

#ifndef MACOSX
    /* XXX FIXME: SWWLAN-31395 causes long channel switch times for 4360
     * So disable implicit TXBF for 4360 and 43602 for Macos for now
     */
    if (ACMAJORREV_0(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev))
        wlc_phy_populate_recipcoeffs_acphy(pi);
#endif /* MACOSX */

    /* 4354 wlipa 2GHz xtal spur war */
    if (ACMAJORREV_2(pi->pubpi->phy_rev) && (ACMINORREV_1(pi) ||
        ACMINORREV_3(pi)) && PHY_ILNA(pi)) {
        ACPHY_REG_LIST_START
            MOD_RADIO_REG_ENTRY(pi, RFP, PLL_XTAL2, xtal_pu_RCCAL1, 0)
            MOD_RADIO_REG_ENTRY(pi, RFP, GE16_OVR27, ovr_xtal_outbufBBstrg, 1)
            MOD_RADIO_REG_ENTRY(pi, RFP, PLL_XTAL4, xtal_outbufBBstrg, 0)
            MOD_RADIO_REG_ENTRY(pi, RFP, PLL_XTAL4, xtal_outbufcalstrg, 0)
            MOD_RADIO_REG_ENTRY(pi, RFP, GE16_OVR27, ovr_xtal_outbufstrg, 1)
            MOD_RADIO_REG_ENTRY(pi, RFP, PLL_XTAL4, xtal_outbufstrg, 2)
            MOD_RADIO_REG_ENTRY(pi, RFP, PLL_XTAL5, xtal_sel_BT, 1)
            MOD_RADIO_REG_ENTRY(pi, RFP, PLL_XTAL5, xtal_bufstrg_BT, 2)
        ACPHY_REG_LIST_EXECUTE(pi);
    }

    if (BFCTL(pi->u.pi_acphy) == 3) {
        if (fc == 5180 || fc == 5190 || fc == 5310 ||
            fc == 5320 || fc == 5500 || fc == 5510) {
            MOD_RADIO_REG(pi, RFP, PLL_CP4, rfpll_cp_ioff, 0xA0);
        }
    }

    /* WAR for 4365 */
    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
        MOD_RADIO_ALLREG_20693(pi, RX_TOP_5G_OVR, ovr_lna5g_nap, 1);
        MOD_RADIO_ALLREG_20693(pi, LNA5G_CFG2, lna5g_nap, 0);
        MOD_RADIO_ALLREG_20693(pi, RX_TOP_2G_OVR_EAST, ovr_lna2g_nap, 1);
        MOD_RADIO_ALLREG_20693(pi, LNA2G_CFG2, lna2g_nap, 0);
    }

    if (ACMAJORREV_47(pi->pubpi->phy_rev) ||
        ACMAJORREV_51_131(pi->pubpi->phy_rev) || ACMAJORREV_129(pi->pubpi->phy_rev) ||
        ACMAJORREV_130(pi->pubpi->phy_rev)) {
        /* Enable TD-SFO for phybw = 80M or 160M */
        if (CHSPEC_IS80(pi->radio_chanspec) || CHSPEC_IS160(pi->radio_chanspec)) {
            pi->u.pi_acphy->td_sfo_corr_en = 2;
        } else {
            pi->u.pi_acphy->td_sfo_corr_en = 0;
        }
        wlc_phy_td_sfo_eco(pi);
    }

    if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        /* set afediv_tssi_pu_reset = 1 for core0 to resolve the lowrate tssi delay
         * problem when core0 is inactive
         */
        uint16 addr = 0x3cc;
        uint16 val  = 0x13;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ,
                1, addr, 16, &val);
    }

    /* 53573A0: 5G Tx Low Power Radio Optimizations */
    /* proc 20693_tx5g_set_bias_ipa_opt_sv */
    if (ROUTER_4349(pi) && CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
        if (PHY_IPA(pi)) {
            FOREACH_CORE(pi, core)
            {
                bool isB0, isB0_BW40, isB0_BW80;
                bool isACR = (((pi->sh->boardrev >> 12) & 0xF) == 2);
                isB0 = (RADIO20693REV(pi->pubpi->radiorev) == 23);
                isB0_BW40 = (RADIO20693REV(pi->pubpi->radiorev) == 23) &&
                    (CHSPEC_IS40(pi->radio_chanspec));
                isB0_BW80 = (RADIO20693REV(pi->pubpi->radiorev) == 23) &&
                (CHSPEC_IS80(pi->radio_chanspec) ||
                 CHSPEC_IS8080(pi->radio_chanspec));

                MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR1, core,
                    ovr_trsw5g_pu, 0x1);
                MOD_RADIO_REG_20693(pi, TRSW5G_CFG1, core,
                    trsw5g_pu, 0x0);
                MOD_RADIO_REG_20693(pi, SPARE_CFG7, core,
                    swcap_sec_gate_off_5g, 0xf);
                MOD_RADIO_REG_20693(pi, SPARE_CFG7, core,
                    swcap_sec_sd_on_5g, 0x10);
                MOD_RADIO_REG_20693(pi, SPARE_CFG6, core,
                    swcap_pri_pd_5g, 0x1);
                MOD_RADIO_REG_20693(pi, SPARE_CFG6, core,
                    swcap_pri_5g, 0x0);
                MOD_RADIO_REG_20693(pi, SPARE_CFG6, core,
                    swcap_pri_gate_off_5g, 0x0);
                MOD_RADIO_REG_20693(pi, SPARE_CFG6, core,
                    swcap_pri_sd_on_5g, 0x0);

                MOD_RADIO_REG_20693(pi, PA5G_CFG3, core,
                    pa5g_ptat_slope_main, 0x0);

                MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR2, core,
                    ovr_pa5g_idac_incap_compen_main, 0x1);
                MOD_RADIO_REG_20693(pi, PA5G_INCAP, core,
                    pa5g_idac_incap_compen_main,
                    (isACR && (isB0_BW40 || isB0_BW80))? 0xc :
                    (isACR && isB0)? 0x3a : 0x16);
                MOD_RADIO_REG_20693(pi, PA5G_IDAC3, core,
                    pa5g_idac_tuning_bias, 0x0);

                MOD_RADIO_REG_20693(pi, TXMIX5G_CFG8, core, pad5g_idac_gm,
                    (isACR && (isB0_BW40 || isB0_BW80)) ? 0x3f :
                    (isACR && isB0) ? 0x26 :
                    (isB0) ? 0x1a : 0x18);
                MOD_RADIO_REG_20693(pi, TXGM5G_CFG1, core, pad5g_idac_cascode,
                    (isACR && (isB0_BW40 || isB0_BW80)) ? 0xf :
                    (isACR && isB0) ? 0xe :
                    (isB0) ? 0xd : 0xb);
                MOD_RADIO_REG_20693(pi, SPARE_CFG10, core,
                    pad5g_idac_cascode2, 0x0);
                MOD_RADIO_REG_20693(pi, PA5G_INCAP, core, pad5g_idac_pmos,
                    (isACR && (isB0_BW40 || isB0_BW80)) ? 0xa : 0x1c);

                MOD_RADIO_REG_20693(pi, PA5G_IDAC3, core,
                    pad5g_idac_tuning_bias, 0xd);
                MOD_RADIO_REG_20693(pi, TXMIX5G_CFG6, core,
                    mx5g_ptat_slope_cascode, 0x0);
                MOD_RADIO_REG_20693(pi, TXMIX5G_CFG6, core,
                    mx5g_ptat_slope_lodc, 0x2);
                MOD_RADIO_REG_20693(pi, TXMIX5G_CFG4, core,
                    mx5g_idac_cascode, 0xf);
                MOD_RADIO_REG_20693(pi, TXMIX5G_CFG5, core,
                    mx5g_idac_lodc, 0xa);
                MOD_RADIO_REG_20693(pi, TXMIX5G_CFG5, core, mx5g_idac_bbdc,
                    (isB0_BW40 && isACR) ? 0x2 : (isB0)? 0xc : 0x8);

                MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR2, core,
                    ovr_pa5g_idac_main, 1);
                MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR3, core,
                    ovr_pa5g_idac_cas, 1);

                if (isB0_BW80) {
                    MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                        pa5g_idac_main, (isACR) ? 0x38 : 0x3a);
                    MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                        pa5g_idac_cas, (isACR) ? 0x14 : 0x15);
                } else if (isB0_BW40) {
                    MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                        pa5g_idac_main, (isACR) ? 0x34 : 0x28);
                    MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                        pa5g_idac_cas, (isACR) ? 0x14 : 0x13);
                } else if (isB0) {
                    MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                        pa5g_idac_main, (isACR) ? 0x20 : 0x1a);
                    MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                        pa5g_idac_cas, (isACR) ? 0x12 : 0x11);
                } else {
                    if (CHSPEC_IS40(pi->radio_chanspec) ||
                        CHSPEC_IS80(pi->radio_chanspec) ||
                        CHSPEC_IS8080(pi->radio_chanspec)) {
                        /* if this 'current-boosting' tuning is adopted,
                         * remember to increase PAPD cal index (65)
                         * since the RF gain is significantly boosted.
                         */
                        MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                            pa5g_idac_main, 0x2c);
                        MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                            pa5g_idac_cas, 0x16);
                    } else {
                        /* 20Mhz */
                        if (fc >= 5745) {
                            MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                                pa5g_idac_main, 0x14);
                            MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                                pa5g_idac_cas, 0x11);
                        } else {
                            MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                                pa5g_idac_main, 0x20);
                            MOD_RADIO_REG_20693(pi, PA5G_IDAC1, core,
                                pa5g_idac_cas, 0x14);
                        }
                    }
                }

                MOD_RADIO_REG_20693(pi, TXMIX5G_CFG3, core,
                    mx5g_pu_bleed, 0x0);
                MOD_RADIO_REG_20693(pi, TXMIX5G_CFG1, core,
                    mx5g_idac_bleed_bias, 0x0);
                MOD_RADIO_REG_20693(pi, TXMIX5G_CFG4, core,
                    mx5g_idac_tuning_bias, 0xd);
            }
        }

        /* EVM Ramp: TxBias5G & Pad5G on */
        FOREACH_CORE(pi, core)
        {
            MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR1, core, ovr_tx5g_bias_pu, 0x1);
            MOD_RADIO_REG_20693(pi, TX5G_CFG1, core, tx5g_bias_pu, 0x1);
            MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR3, core, ovr_pad5g_bias_cas_pu, 0x1);
            MOD_RADIO_REG_20693(pi, TX5G_CFG1, core, pad5g_bias_cas_pu, 0x1);
            MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR2, core, ovr_pad5g_pu, 0x1);
            MOD_RADIO_REG_20693(pi, TX5G_CFG1, core, pad5g_pu, 0x1);
        }
    }

    if (ROUTER_4349(pi) && PHY_IPA(pi) && CHSPEC_IS2G(pi->radio_chanspec)) {
        FOREACH_CORE(pi, core)
        {
            MOD_RADIO_REG_20693(pi, TX_TOP_2G_OVR1_EAST, core,
                    ovr_pa2g_idac_main, 0x1);
            MOD_RADIO_REG_20693(pi, TX_TOP_2G_OVR1_EAST, core,
                    ovr_pa2g_idac_cas, 0x1);
            if (CHSPEC_IS40(pi->radio_chanspec)) {
                MOD_RADIO_REG_20693(pi, PA2G_IDAC1, core,
                        pa2g_idac_main, 0x1e);
                MOD_RADIO_REG_20693(pi, PA2G_IDAC1, core,
                        pa2g_idac_cas, 0x21);
            } else {
                MOD_RADIO_REG_20693(pi, PA2G_IDAC1, core,
                        pa2g_idac_main, 0x19);
                MOD_RADIO_REG_20693(pi, PA2G_IDAC1, core,
                        pa2g_idac_cas, 0x24);
            }

            MOD_RADIO_REG_20693(pi, SPARE_CFG10, core,
                    pa2g_incap_pmos_src_sel_gnd, 0x0);
            MOD_RADIO_REG_20693(pi, TX_TOP_2G_OVR1_EAST, core,
                    ovr_pa2g_idac_incap_compen_main, 0x1);
            MOD_RADIO_REG_20693(pi, PA2G_INCAP, core,
                    pa2g_idac_incap_compen_main, 0x34);
        }
    }

    /* IPA heat transient issue: keep the PAD online all the time to improve TxEVM floor */
    if (PHY_PAPDEN(pi) && phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->keep_pad_on) {
        phy_ac_chanmgr_pad_online_enable(pi, TRUE, FALSE);
    }

    PHY_CHANLOG(pi, __FUNCTION__, TS_EXIT, 0);
}

/*
gmult_rc (24:17), gmult(16:9), bq1_bw(8:6), rc_bw(5:3), bq0_bw(2:0)
LO: (15:0), HI (24:16)
mode_mask = bits[0:8] = 11b_20, 11n_20, 11ag_11ac_20, 11b_40, 11n_40, 11ag_11ac_40, 11b_80,
11n_11ag_11ac_80, samp_play
*/
static void
wlc_phy_set_analog_tx_lpf(phy_info_t *pi, uint16 mode_mask, int bq0_bw, int bq1_bw,
                       int rc_bw, int gmult, int gmult_rc, int core_num)
{
    uint8 ctr, core, max_modes = 9;
    uint16 addr_lo_offs[] = {0x142, 0x152, 0x162, 0x482};
    uint16 addr_hi_offs[] = {0x362, 0x372, 0x382, 0x552};
    uint16 rxlpfbw[] = {0, 0, 0, 1, 1, 1, 2, 2, 1};
    uint16 addr_lo_base, addr_hi_base, addr_lo, addr_hi;
    uint16 val_lo, val_hi;
    uint32 val;
    uint8 stall_val;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

    BCM_REFERENCE(stf_shdata);

    /* This proc does not impact 4349, so return without doing anything */
    if (ACMAJORREV_4(pi->pubpi->phy_rev) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev)) {
        return;
    }

    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    ACPHY_DISABLE_STALL(pi);
    /* core_num = -1 ==> all cores */
    FOREACH_ACTV_CORE(pi, stf_shdata->phytxchain, core) {
        if ((core_num == -1) || (core_num == core)) {
            ASSERT(core < ARRAYSIZE(addr_lo_offs));
            addr_lo_base = addr_lo_offs[core];
            addr_hi_base = addr_hi_offs[core];
            for (ctr = 0; ctr < max_modes; ctr++) {
                if ((mode_mask >> ctr) & 1) {
                    addr_lo = addr_lo_base + ctr;
                    addr_hi = addr_hi_base + ctr;
                    wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ,
                                             1, addr_lo, 16, &val_lo);
                    wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ,
                                             1, addr_hi, 16, &val_hi);
                    val = (val_hi << 16) | val_lo;
                    if (ACMAJORREV_GE37(pi->pubpi->phy_rev)) {
                        /* Only bq1/bq2 (= TIA/LPF) bandwidth is controlled
                         * by direct control. The biquad control fields for
                         * Rx are in the Rx2Tx table entries and are
                         * initialized here too.
                         * For Tx bq2/bq1 control is in bit 5:2/1:0, however
                         * only bq2 is in the Tx path
                         * For Rx bq2/bq1 control is in bit 11:8/7:6
                         * Note: in this function bq2/bq1 are referred to
                         *       as bq1/bq0
                         */
                        if (bq0_bw >= 0) {
                            val = (val & 0x1fffffc) | (bq0_bw << 0);
                        }
                        if (bq1_bw >= 0) {
                            val = (val & 0x1ffffc3) | (bq1_bw << 2);
                        }
                        val = (val & 0x1fff03f) | (rxlpfbw[ctr] << 6) |
                        (rxlpfbw[ctr] << 8);
                    } else {
                        if (bq0_bw >= 0) {
                            val = (val & 0x1fffff8) | (bq0_bw << 0);
                            }
                        if (rc_bw >= 0) {
                            val = (val & 0x1ffffc7) | (rc_bw << 3);
                        }
                        if (bq1_bw >= 0) {
                            val = (val & 0x1fffe3f) | (bq1_bw << 6);
                        }
                        if (gmult >= 0) {
                            val = (val & 0x1fe01ff) | (gmult << 9);
                        }
                        if (gmult_rc >= 0) {
                            val = (val & 0x001ffff) | (gmult_rc << 17);
                        }
                    }

                    val_lo = val & 0xffff;
                    val_hi = (val >> 16) & 0x1ff;
                    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ,
                                              1, addr_lo, 16, &val_lo);
                    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ,
                                              1, addr_hi, 16, &val_hi);
                }
            }
        }
    }
    ACPHY_ENABLE_STALL(pi, stall_val);
}

/*
dacbuf_fixed_cap[5], dacbuf_cap[4:0]
mode_mask = bits[0:8] = 11b_20, 11n_20, 11ag_11ac_20, 11b_40, 11n_40, 11ag_11ac_40, 11b_80,
11n_11ag_11ac_80, samp_play
*/
static void
wlc_phy_set_tx_afe_dacbuf_cap(phy_info_t *pi, uint16 mode_mask, int dacbuf_cap,
                           int dacbuf_fixed_cap, int core_num)
{
    uint8 ctr, core, max_modes = 9;
    uint16 core_base[] = {0x3f0, 0x60, 0xd0, 0x570};
    uint8 offset[] = {0xb, 0xb, 0xc, 0xc, 0xe, 0xe, 0xf, 0xf, 0xa};
    uint8 shift[] = {0, 6, 0, 6, 0, 6, 0, 6, 0};
    uint16 addr, read_val, val;
    uint8 stall_val;
    uint8 phyrxchain;

    BCM_REFERENCE(phyrxchain);

    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    ACPHY_DISABLE_STALL(pi);
    /* core_num = -1 ==> all cores */
    phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;
    FOREACH_ACTV_CORE(pi, phyrxchain, core) {
        if ((core_num == -1) || (core_num == core)) {
            for (ctr = 0; ctr < max_modes; ctr++) {
                if ((mode_mask >> ctr) & 1) {
                    addr = core_base[core] + offset[ctr];
                    wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ,
                                             1, addr, 16, &read_val);
                    val = (read_val >> shift[ctr]) & 0x3f;

                    if (dacbuf_cap >= 0) {
                            val = (val & 0x20) | dacbuf_cap;
                    }
                    if (dacbuf_fixed_cap >= 0) {
                        val = (val & 0x1f) |
                                (dacbuf_fixed_cap << 5);
                    }

                    if (shift[ctr] == 0) {
                        val = (read_val & 0xfc0) | val;
                    } else {
                        val = (read_val & 0x3f) | (val << 6);
                    }

                    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ,
                                              1, addr, 16, &val);
                }
            }
        }
    }
    ACPHY_ENABLE_STALL(pi, stall_val);
}

/*
gmult_rc (24:17), rc_bw(16:14), gmult(13:6), bq1_bw(5:3), bq0_bw(2:0)
LO: (15:0), HI (24:16)
mode_mask = bits[0:2] = 20, 40, 80
*/
static void
wlc_phy_set_analog_rx_lpf(phy_info_t *pi, uint8 mode_mask, int bq0_bw, int bq1_bw,
                  int rc_bw, int gmult, int gmult_rc, int core_num)
{
    uint8 ctr, core, max_modes = 3;
    uint16 addr20_lo_offs[] = {0x140, 0x150, 0x160};
    uint16 addr20_hi_offs[] = {0x360, 0x370, 0x380};
    uint16 addr40_lo_offs[] = {0x141, 0x151, 0x161};
    uint16 addr40_hi_offs[] = {0x361, 0x371, 0x381};
    uint16 addr80_lo_offs[] = {0x441, 0x443, 0x445};
    uint16 addr80_hi_offs[] = {0x440, 0x442, 0x444};
    uint16 addr_lo, addr_hi;
    uint16 val_lo, val_hi;
    uint32 val;
    uint8 stall_val;
    uint8 phyrxchain;

    BCM_REFERENCE(phyrxchain);

    /* This proc does not impact 4349, so return without doing anything */
    if (ACMAJORREV_4(pi->pubpi->phy_rev) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) || ACMAJORREV_GE40(pi->pubpi->phy_rev))
        return;

    ASSERT(PHYCORENUM(pi->pubpi->phy_corenum) < 4);

    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    ACPHY_DISABLE_STALL(pi);
    /* core_num = -1 ==> all cores */
    phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;
    FOREACH_ACTV_CORE(pi, phyrxchain, core) {
        if ((core_num == -1) || (core_num == core)) {
            for (ctr = 0; ctr < max_modes; ctr++) {
                if ((mode_mask >> ctr) & 1) {
                    if (ctr == 0) {
                        addr_lo = addr20_lo_offs[core];
                        addr_hi = addr20_hi_offs[core];
                    }
                    else if (ctr == 1) {
                        addr_lo = addr40_lo_offs[core];
                        addr_hi = addr40_hi_offs[core];
                    } else {
                        addr_lo = addr80_lo_offs[core];
                        addr_hi = addr80_hi_offs[core];
                    }

                    wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ,
                                             1, addr_lo, 16, &val_lo);
                    wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ,
                                             1, addr_hi, 16, &val_hi);
                    val = (val_hi << 16) | val_lo;

                    if (bq0_bw >= 0) {
                        val = (val & 0x1fffff8) | (bq0_bw << 0);
                    }
                    if (bq1_bw >= 0) {
                        val = (val & 0x1ffffc7) | (bq1_bw << 3);
                    }
                    if (gmult >= 0) {
                        val = (val & 0x1ffc03f) | (gmult << 6);
                    }
                    if (rc_bw >= 0) {
                        val = (val & 0x1fe3fff) | (rc_bw << 14);
                    }
                    if (gmult_rc >= 0) {
                        val = (val & 0x001ffff) | (gmult_rc << 17);
                    }

                    val_lo = val & 0xffff;
                    val_hi = (val >> 16) & 0x1ff;
                    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                                              addr_lo, 16, &val_lo);
                    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1,
                                              addr_hi, 16, &val_hi);
                }
            }
        }
    }
    ACPHY_ENABLE_STALL(pi, stall_val);
}

static void
wlc_phy_set_analog_lpf(phy_info_t *pi, uint8 *tx_bq1, uint8 *tx_bq2, uint8 *rx_bq1, uint8 *rx_bq2)
{
    uint8 ctr,  core, stall_val;
    uint16 addrs[] = {0x142, 0x152, 0x162, 0x482};  // cores 0-3(20/40/80 mhz)
    uint16 addr160 = 0x5c0;                             // not per core table
    uint16 val_bw[4], vals[9];

    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    ACPHY_DISABLE_STALL(pi);

    // loop over 20/40/80/160mhz
    for (ctr = 0; ctr < 4; ctr++)
        val_bw[ctr] = (rx_bq2[ctr] << 8) | (rx_bq1[ctr] << 6) |
            (tx_bq2[ctr] << 2) | tx_bq1[ctr];

    /* $val20 $val20 $val20 $val40 $val40 $val40 $val80 $val80 $valsmp */
    vals[0] = vals[1] = vals[2] = val_bw[0];
    vals[3] = vals[4] = vals[5] = val_bw[1];
    vals[6] = vals[7] = val_bw[2];
    vals[8] = CHSPEC_IS160(pi->radio_chanspec) ?
        val_bw[3] : (CHSPEC_IS80(pi->radio_chanspec) ?
                     val_bw[2] : (CHSPEC_IS40(pi->radio_chanspec) ? val_bw[1] : val_bw[0]));
    FOREACH_CORE(pi, core) {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 9, addrs[core], 16, &vals);
    }

    /* 160mhz entries are not per core */
    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, addr160,   16, &val_bw[3]);
    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, addr160+1, 16, &val_bw[3]);

    ACPHY_ENABLE_STALL(pi, stall_val);
}

static void
acphy_load_txv_for_spexp(phy_info_t *pi)
{
    uint32 len = 243, offset = 1220;

    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_BFMUSERINDEX,
                              len, offset, 32, acphy_txv_for_spexp);
}

static void
wlc_phy_cfg_energydrop_timeout(phy_info_t *pi)
{
    if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
        /* Fine timing mod to have more overlap(~10dB) between low and high SNR regimes
         * change to 0x8 to prevent the radar to trigger the fine timing
         */
        MOD_PHYREG(pi, FSTRMetricTh, hiPwr_min_metric_th, 0x8);
        /* change it to 40000 for radar detection */
        WRITE_PHYREG(pi, energydroptimeoutLen, 0x9c40);
    } else {
        /* Fine timing mod to have more overlap(~10dB) between low and high SNR regimes */
        MOD_PHYREG(pi, FSTRMetricTh, hiPwr_min_metric_th, 0xf);
        /* In event of high power spurs/interference that causes crs-glitches,
         * stay in WAIT_ENERGY_DROP for 1 clk20 instead of default 1 ms.
         * This way, we get back to CARRIER_SEARCH quickly and will less likely to miss
         * actual packets. PS: this is actually one settings for ACI
         */
        WRITE_PHYREG(pi, energydroptimeoutLen, 0x2);
    }
}

static void
wlc_phy_set_reg_on_bw_change_acphy_noHWPhyReset(phy_info_t *pi)
{
    uint8 core;
    uint16 regval, vht, mm;
    const bool chspec_is20 = CHSPEC_BW_LE20(pi->radio_chanspec);
    const bool chspec_is40 = CHSPEC_IS40(pi->radio_chanspec);
    const bool chspec_is80 = CHSPEC_IS80(pi->radio_chanspec);
    const bool chspec_is160 = CHSPEC_IS160(pi->radio_chanspec);

    if (TINY_RADIO(pi) || IS_28NM_RADIO(pi) || IS_16NM_RADIO(pi)) {
        MOD_PHYREG(pi, TssiEnRate, StrobeRateOverride, 1);
    }

    /* Following change necessary if there is no TI-ADC to make
     * strobe rate same as 43684. 6710 is not doing this
     * because TSSI was characterized before this finding:
     * MOD_PHYREG(pi, TssiEnRate, StrobeRate, chspec_is20?
     * 0x2 : chspec_is40 ? 0x3 : 0x4)
     */
    MOD_PHYREG(pi, TssiEnRate, StrobeRate, chspec_is20 ? 0x1 : chspec_is40 ? 0x2 :
               chspec_is80 ? 0x3 : 0x4);
    MOD_PHYREG(pi, ClassifierCtrl, mac_bphy_band_sel, chspec_is20 ? 0x1 : 0x0);
    MOD_PHYREG(pi, RxControl, bphy_band_sel, chspec_is20 ? 0x1 : 0x0);
    MOD_PHYREG(pi, DcFiltAddress, dcCoef0, chspec_is20 ? 0x15 : chspec_is40 ? 0xb : 0x5);

    MOD_PHYREG(pi, IqestWaitTime, waitTime, chspec_is20 ? 0x14  : 0x28);

    if (ACMAJORREV_4(pi->pubpi->phy_rev) ||    ACMAJORREV_128(pi->pubpi->phy_rev)) {
        if (!ACMAJORREV_4(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, CRSMiscellaneousParam, crsMfFlipCoef,
                       chspec_is20 ? 0x0 : 0x1);
        }

        MOD_PHYREG(pi, iqest_input_control, dc_accum_wait_vht,
                   chspec_is20 ? 0xc :
                   chspec_is40 ? 0x1d : 0x3b);
        MOD_PHYREG(pi, iqest_input_control, dc_accum_wait_mm,
                   chspec_is20 ? 0xb :
                   chspec_is40 ? 0x1b : 0x37);
    }

    if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, nvcfg3, noisevar_rxevm_lim_qdb, chspec_is20 ?
                   0x97 : chspec_is40 ? 0x8b :
                   chspec_is80 ? 0x97 : 0x97);

        vht = chspec_is20 ? 12 : chspec_is40 ? 29 : chspec_is80 ? 59 : 86;
        mm = chspec_is20 ? 11 : chspec_is40 ? 27 : chspec_is80 ? 55 : 77;
        if (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, dc_fix_filter_config, dc_accum_wait_vht, vht);
            MOD_PHYREG(pi, dc_fix_filter_config, dc_accum_wait_mm, mm);
        } else {
            MOD_PHYREG(pi, iqest_input_control, dc_accum_wait_vht, vht);
            MOD_PHYREG(pi, iqest_input_control, dc_accum_wait_mm, mm);
        }
        if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            vht = chspec_is20 ? 13 : chspec_is40 ? 31 : chspec_is80 ? 63 : 93;
            mm = chspec_is20 ? 12 : chspec_is40 ? 29 : chspec_is80 ? 59 : 84;
            MOD_PHYREG(pi, dc_fix_filter_config_lesi, dc_accum_wait_vht, vht);
            MOD_PHYREG(pi, dc_fix_filter_config_lesi, dc_accum_wait_mm, mm);
        }

        if (!ACMAJORREV_32(pi->pubpi->phy_rev) && !ACMAJORREV_33(pi->pubpi->phy_rev)) {
            WRITE_PHYREG(pi, ACIBrwdfCoef0,
                         chspec_is20 ? 0xdc31 :
                         chspec_is40 ? 0xc32b :
                         chspec_is80 ? 0xba28 : 0xb626);
            WRITE_PHYREG(pi, ACIBrwdfCoef1,
                         chspec_is20 ? 0x0000 :
                         chspec_is40 ? 0x0000 :
                         chspec_is80 ? 0x00f7 : 0xfcf1);
            WRITE_PHYREG(pi, ACIBrwdfCoef2,
                         chspec_is20 ? 0x008d :
                         chspec_is40 ? 0xee80 :
                         chspec_is80 ? 0xe179 : 0xdd74);
        }
    }

    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        wlc_phy_set_reg_on_bw_change_acphy_majorrev128(pi);
    }

    if (!TINY_RADIO(pi) && !IS_28NM_RADIO(pi) && !IS_16NM_RADIO(pi)) {
        MOD_PHYREG(pi, RxFilt40Num00, RxFilt40Num00, chspec_is20 ?
            0x146 : chspec_is40 ? 0x181 : 0x17a);
        MOD_PHYREG(pi, RxFilt40Num01, RxFilt40Num01, chspec_is20 ?
            0x88 : chspec_is40 ? 0x5a : 0x9e);
        MOD_PHYREG(pi, RxFilt40Num02, RxFilt40Num02, chspec_is20 ?
            0x146 : chspec_is40 ? 0x181 : 0x17a);
        MOD_PHYREG(pi, RxFilt40Den00, RxFilt40Den00, chspec_is20 ?
            0x76e : chspec_is40 ? 0x793 : 0x7ca);
        MOD_PHYREG(pi, RxFilt40Den01, RxFilt40Den01, chspec_is20 ?
            0x1a8 : chspec_is40 ? 0x1b7 : 0x1b2);
        MOD_PHYREG(pi, RxFilt40Num10, RxFilt40Num10, chspec_is20 ?
            0xa3 : chspec_is40 ? 0xc1 : 0xbd);
        MOD_PHYREG(pi, RxFilt40Num11, RxFilt40Num11, chspec_is20 ?
            0xf4 : chspec_is40 ? 0x102 : 0x114);
        MOD_PHYREG(pi, RxFilt40Num12, RxFilt40Num12, chspec_is20 ?
            0xa3 : chspec_is40 ? 0xc1 : 0xbd);
        MOD_PHYREG(pi, RxFilt40Den10, RxFilt40Den10, chspec_is20 ?
            0x684 : chspec_is40 ? 0x6c0 : 0x6d6);
        MOD_PHYREG(pi, RxFilt40Den11, RxFilt40Den11, chspec_is20 ?
            0xad : chspec_is40 ? 0xa9 : 0xa2);
        MOD_PHYREG(pi, RxStrnFilt40Num00, RxStrnFilt40Num00,
            chspec_is20 ? 0xe5 : chspec_is40 ? 0x162 : 0x16c);
        MOD_PHYREG(pi, RxStrnFilt40Num01, RxStrnFilt40Num01,
            chspec_is20 ? 0x68 : chspec_is40 ? 0x42 : 0x6f);
        MOD_PHYREG(pi, RxStrnFilt40Num02, RxStrnFilt40Num02,
            chspec_is20 ? 0xe5 : chspec_is40 ? 0x162 : 0x16c);
        MOD_PHYREG(pi, RxStrnFilt40Den00, RxStrnFilt40Den00,
            chspec_is20 ? 0x6be : chspec_is40 ? 0x75c : 0x793);
        MOD_PHYREG(pi, RxStrnFilt40Den01, RxStrnFilt40Den01,
            chspec_is20 ? 0x19e : chspec_is40 ? 0x1b3 : 0x1b2);
        MOD_PHYREG(pi, RxStrnFilt40Num10, RxStrnFilt40Num10,
            chspec_is20 ? 0x73 : chspec_is40 ? 0xb1 : 0xb6);
        MOD_PHYREG(pi, RxStrnFilt40Num11, RxStrnFilt40Num11,
            chspec_is20 ? 0xb2 : chspec_is40 ? 0xed : 0xff);
        MOD_PHYREG(pi, RxStrnFilt40Num12, RxStrnFilt40Num12,
            chspec_is20 ? 0x73 : chspec_is40 ? 0xb1 : 0xb6);
        MOD_PHYREG(pi, RxStrnFilt40Den10, RxStrnFilt40Den10,
            chspec_is20 ? 0x5fe : chspec_is40 ? 0x692 : 0x6b4);
        MOD_PHYREG(pi, RxStrnFilt40Den11, RxStrnFilt40Den11,
            chspec_is20 ? 0xcc : chspec_is40 ? 0xaf : 0xa8);
    }

    if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, RxFilt40Num00, RxFilt40Num00, chspec_is20 ?
            0x146 : chspec_is40 ? 0x181 : 0x17a);
        MOD_PHYREG(pi, RxFilt40Num01, RxFilt40Num01, chspec_is20 ?
            0x88 : chspec_is40 ? 0x5a : 0x9e);
        MOD_PHYREG(pi, RxFilt40Num02, RxFilt40Num02, chspec_is20 ?
            0x146 : chspec_is40 ? 0x181 : 0x17a);
        MOD_PHYREG(pi, RxFilt40Den00, RxFilt40Den00, chspec_is20 ?
            0x76e : chspec_is40 ? 0x793 : 0x7ca);
        MOD_PHYREG(pi, RxFilt40Den01, RxFilt40Den01, chspec_is20 ?
            0x1a8 : chspec_is40 ? 0x1b7 : 0x1b2);
        MOD_PHYREG(pi, RxFilt40Num10, RxFilt40Num10, chspec_is20 ?
            0xa3 : chspec_is40 ? 0xc1 : 0xbd);
        MOD_PHYREG(pi, RxFilt40Num11, RxFilt40Num11, chspec_is20 ?
            0xf4 : chspec_is40 ? 0x102 : 0x114);
        MOD_PHYREG(pi, RxFilt40Num12, RxFilt40Num12, chspec_is20 ?
            0xa3 : chspec_is40 ? 0xc1 : 0xbd);
        MOD_PHYREG(pi, RxFilt40Den10, RxFilt40Den10, chspec_is20 ?
            0x684 : chspec_is40 ? 0x6c0 : 0x6d6);
        MOD_PHYREG(pi, RxFilt40Den11, RxFilt40Den11, chspec_is20 ?
            0xad : chspec_is40 ? 0xa9 : 0xa2);
        MOD_PHYREG(pi, HwRssiSampleCount, NumSampToCol, chspec_is20 ?
            0x50 : chspec_is40 ? 0xa0 : 0x140);
        MOD_PHYREG(pi, HwRssiSampleCountLTF, NumSampToColLTF, chspec_is20 ?
            0x50 : chspec_is40 ? 0xa0 : 0x140);
        MOD_PHYREG(pi, norm_var_hyst_th_pt8us, dis_upd_on_GainDsmpEnOut, chspec_is20 ?
            0x0 : chspec_is40 ? 0x0 : 0x1);
        MOD_PHYREG(pi, ClassifierLogACPrimary0, logACDelta1P, chspec_is20 ?
            0x13 : chspec_is40 ? 0x13 : 0x9);
        MOD_PHYREG(pi, ClassifierLogACPrimary1, logACDelta2P, chspec_is20 ?
            0x13 : chspec_is40 ? 0x13 : 0x9);
        MOD_PHYREG(pi, rx1misc_1, rxPwrVarCntAdj, chspec_is20 ?
            0x1 : chspec_is40 ? 0x1 : 0x0);
        MOD_PHYREG(pi, rx1misc_1, LogAcAlignmentCount, chspec_is20 ?
            0x0 : chspec_is40 ? 0x0 : 0x6);
        MOD_PHYREG(pi, td_sfo_rst_ext, dly_match_count, chspec_is20 ?
            0xd : chspec_is40 ? 0x1c : 0x39);
        MOD_PHYREG(pi, Core0Adcclip_aci, adc_clip_cnt_th, chspec_is20 ?
            0xa : chspec_is40 ? 0x14 : 0x14);
        MOD_PHYREG(pi, Core1Adcclip_aci, adc_clip_cnt_th, chspec_is20 ?
            0xa : chspec_is40 ? 0x14 : 0x14);
        MOD_PHYREG(pi, Core2Adcclip_aci, adc_clip_cnt_th, chspec_is20 ?
            0xa : chspec_is40 ? 0x14 : 0x14);
        MOD_PHYREG(pi, Core0Adcclip_macaided, adc_clip_cnt_th, chspec_is20 ?
            0xa : chspec_is40 ? 0x14 : 0x14);
        MOD_PHYREG(pi, Core1Adcclip_macaided, adc_clip_cnt_th, chspec_is20 ?
            0xa : chspec_is40 ? 0x14 : 0x14);
        MOD_PHYREG(pi, Core2Adcclip_macaided, adc_clip_cnt_th, chspec_is20 ?
            0xa : chspec_is40 ? 0x14 : 0x14);
    }

    if (ACMAJORREV_33(pi->pubpi->phy_rev) || ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        // 20-in-160 vs. 20-in-80 vs. 20-in-40 hw-obss filter settings
        WRITE_PHYREG(pi, obssreg0_c0, chspec_is160 ? 0x321 : chspec_is80 ? 0x340 : 0x37f);
        WRITE_PHYREG(pi, obssreg0_c10, chspec_is160 ? 0x236 : chspec_is80 ? 0x286 : 0x360);
        WRITE_PHYREG(pi, obssreg0_c11, chspec_is160 ? 0xdb : chspec_is80 ? 0xbd : 0x96);
        WRITE_PHYREG(pi, obssreg0_c20, chspec_is160 ? 0x228 : chspec_is80 ? 0x287 : 0x3b3);
        WRITE_PHYREG(pi, obssreg0_c21, chspec_is160 ? 0xf7 : chspec_is80 ? 0xf0 : 0xe8);
        if (ACMAJORREV_GE130(pi->pubpi->phy_rev)) {
            // 40-in-160 vs. 40-in-80 hw-obss filter settings
            WRITE_PHYREG(pi, obssreg1_c0, chspec_is160 ? 0x34a : 0x38c);
            WRITE_PHYREG(pi, obssreg1_c10, chspec_is160 ? 0x295 : 0x386);
            WRITE_PHYREG(pi, obssreg1_c11, chspec_is160 ? 0xbf : 0x97);
            WRITE_PHYREG(pi, obssreg1_c20, chspec_is160 ? 0x291 : 0x3da);
            WRITE_PHYREG(pi, obssreg1_c21_, chspec_is160 ? 0xf3 : 0xeb);
        }
    }

    if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        wlc_phy_set_reg_on_bw_change_acphy_majorrev130(pi);
    }

    if (CHSPEC_ISPHY5G6G(pi->radio_chanspec) && !(ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) || (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev)))) {
        MOD_PHYREG(pi, RadarBlankCtrl, radarBlankingInterval, chspec_is20 ? 0x19 : 0x32);
        MOD_PHYREG(pi, RadarT3BelowMin, Count, chspec_is20 ? 0x14 : 0x28);
        MOD_PHYREG(pi, RadarT3Timeout, Timeout, chspec_is20 ? 0xc8 : 0x190);
        MOD_PHYREG(pi, RadarResetBlankingDelay, Count, chspec_is20 ? 0x19 : 0x32);
    }

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* Other radar regs */
        if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
            regval = chspec_is20 ? 0x19 : 0x32;
            MOD_PHYREG(pi, RadarBlankCtrl, radarBlankingInterval, regval);
            MOD_PHYREG(pi, RadarBlankCtrl_SC, radarBlankingInterval, regval);

            regval = chspec_is20 ? 0x8 : chspec_is40 ? 0x10 : chspec_is80 ? 0x20 : 0;
            MOD_PHYREG(pi, RadarMaLength, maLength, regval);
            MOD_PHYREG(pi, RadarMaLength_SC, maLength, regval);

            regval = chspec_is20 ? 0x14 : 0x28;
            MOD_PHYREG(pi, RadarT3BelowMin, Count, regval);
            MOD_PHYREG(pi, RadarT3BelowMin_SC, Count, regval);

            regval = chspec_is20 ? 200 : chspec_is40 ? 400 : chspec_is80 ? 400 : 960;
            MOD_PHYREG(pi, RadarT3Timeout, Timeout, regval);
            MOD_PHYREG(pi, RadarT3Timeout_SC, Timeout, regval);

            regval = chspec_is20 ? 0x19 : 0x32;
            MOD_PHYREG(pi, RadarResetBlankingDelay, Count, regval);
            MOD_PHYREG(pi, RadarResetBlankingDelay_SC, Count, regval);

            regval = chspec_is20 ? 0xa : chspec_is40 ? 0xf : chspec_is80 ? 0xc : 0xa;
            MOD_PHYREG(pi, RadarSubBandConfig1, close_to_DC_thresh, regval);
            MOD_PHYREG(pi, RadarSubBandConfig1_SC, close_to_DC_thresh, regval);

            regval = chspec_is20 ? 0xa : chspec_is40 ? 0x10: chspec_is80 ? 0x5 : 0x3;
            MOD_PHYREG(pi, RadarSubBandConfig2, stat_thresh_close_DC, regval);
            MOD_PHYREG(pi, RadarSubBandConfig2_SC, stat_thresh_close_DC, regval);

            regval = chspec_is20 ? 0x9 : chspec_is40 ? 0x9: chspec_is80 ? 0x3 : 0x0;
            MOD_PHYREG(pi, RadarSubBandConfig2, phase_est_stable_thresh, regval);
            MOD_PHYREG(pi, RadarSubBandConfig2_SC, phase_est_stable_thresh, regval);

            regval = chspec_is20 ? 0x1 : chspec_is40 ? 0x2: 0x4;
            MOD_PHYREG(pi, RadarSubBandConfig5, not_radar_pw_thresh, regval);
            MOD_PHYREG(pi, RadarSubBandConfig5_SC, not_radar_pw_thresh, regval);

            regval =  chspec_is160 ? 0x2 : 7;
            MOD_PHYREG(pi, RadarSubBandConfig5, not_radar_stat_thresh, regval);
            MOD_PHYREG(pi, RadarSubBandConfig5_SC, not_radar_stat_thresh, regval);

            regval = chspec_is160 ? 48 : 64;
            MOD_PHYREG(pi, RadarThresh1, radarThd1, regval);
            MOD_PHYREG(pi, RadarThresh1_SC, radarThd1, regval);
            MOD_PHYREG(pi, RadarThresh1_core1, radarThd1_core1, regval);
            MOD_PHYREG(pi, RadarThresh1_tx, radarThd1_tx, regval);
        }

        MOD_PHYREG(pi, fdiqi_main_tap_dly, rx_fdiq_mode, chspec_is160 ? 1 : 0);
        MOD_PHYREG(pi, RxFrontendCommon_HE_0, useDelayedAddDropForDmd, chspec_is160 ?
                   1 : 0);

        /* subband Classifer (helps in eliminating flase detect due to ACPR leakage)
        i.e 20in40/80 is not false detected
        as 40/80 (these regs get reset on a bw change)
        */
        MOD_PHYREG(pi, ClassifierLogAC1, logACDelta1,
                   chspec_is20 ? 0x13 :
                   chspec_is40 ? 0x13 : 0x9);
        MOD_PHYREG(pi, ClassifierCtrl6, logACDelta2,
                   chspec_is20 ? 0x13 :
                   chspec_is40 ? 0x13 : 0x9);

        WRITE_PHYREG(pi, DcFiltAddressExt,
                     chspec_is20 ? 0x1800 :
                     chspec_is40 ? 0x3000 :
                     chspec_is80 ? 0x6000 : 0xc024);
        WRITE_PHYREG(pi, DcFiltAddressExt1,
                     chspec_is20 ? 0x1f10 :
                     chspec_is40 ? 0x3f20 :
                     chspec_is80 ? 0x7f40 : 0xff80);
        MOD_PHYREG(pi, DcFiltAddressExt2, dc_accum_wait_extention_he,
                   chspec_is160 ? 0x25 : 0);
        MOD_PHYREG(pi, LtrnOffset, min_ltrn_offset,
                   chspec_is20 ? 0x9 : 0xd);
        MOD_PHYREG(pi, LtrnOffset, max_ltrn_offset,
                   chspec_is20 ? 0xd : 0x11);

        ACPHYREG_BCAST(pi, a_coef10, chspec_is20 ? 0x37e :
                       chspec_is40 ? 0x3bc : 0x3dd);
        ACPHYREG_BCAST(pi, a_coef20, chspec_is20 ? 0x3a7 :
                       chspec_is40 ? 0x3d2 : 0x3e8);
        ACPHYREG_BCAST(pi, a_coef30, chspec_is20 ? 0x3b4 :
                       chspec_is40 ? 0x3d8 : 0x3ed);
        regval = chspec_is20 ? 0x48 : chspec_is40 ? 0x24 : chspec_is80 ? 0x12 : 0x9;
        ACPHYREG_BCAST(pi, b_coef10, regval);
        ACPHYREG_BCAST(pi, b_coef20, regval);
        ACPHYREG_BCAST(pi, b_coef30, regval);

        WRITE_PHYREG(pi, abortstatedwelltimeLen,  chspec_is40 ? 4 : 2);
        MOD_PHYREG(pi, advnapCtrl, nap_gate_on_cnt, chspec_is20 ? 0x8 :
                   chspec_is40 ? 0x11 : chspec_is80 ? 0x1c : 0x32);
        MOD_PHYREG(pi, spur_can_phy_bw_mhz, spur_can_phy_bw_mhz,
                   chspec_is20 ? 20 : chspec_is40 ? 40 :
                   chspec_is80 ? 80  : 160);

        WRITE_PHYREG(pi, IqestSampleCount, chspec_is20 ? 0x30 : 0x140);
        WRITE_PHYREG(pi, HwRssiIqestSampleWaitLTF, chspec_is20 ? 0x50 :
                     chspec_is40 ? 0xa0 : chspec_is80 ? 0x140 : 0x280);
        WRITE_PHYREG(pi, HwRssiIqestSampleWaitHTF, chspec_is20 ? 0x10 :
                     chspec_is40 ? 0x20 : chspec_is80 ? 0x40 : 0x80);

        MOD_PHYREG(pi, finstr_adj_neg_neg_limit_0, fine_str_sgi_adj_neg_limit_vals,
                   chspec_is40 ? 0xf8 : 0x80);
        MOD_PHYREG(pi, finstr_adj_neg_neg_limit_1, fine_str_cp1p6_adj_neg_limit_vals,
                   chspec_is40 ? 0xf0 : 0x80);
        MOD_PHYREG(pi, finstr_adj_neg_neg_limit_2, fine_str_cp3p2_adj_neg_limit_vals,
                   chspec_is40 ? 0xd0 : 0x80);

        if (!ACMAJORREV_130(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, chnsmCtr21, chnsm_delay_pilot_rdy_cntr, chspec_is20 ? 0xa :
                       chspec_is40 ? 0x71 : chspec_is80 ? 0x104 : 0xa0);
        } else {
            MOD_PHYREG(pi, chnsmCtr21, chnsm_delay_pilot_rdy_cntr, chspec_is20 ? 0xa :
                       chspec_is40 ? 0x71 : chspec_is80 ? 0x104 : 0x140);
        }
        if (ACMAJORREV_129_130(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, RxFrontendCommon_HE_0, heNdpShortNdpCnt,
                chspec_is20 ? 0x52 : chspec_is40 ? 0xa4 : 0x148);
        } else {
            MOD_PHYREG(pi, RxFrontendCommon_HE_0, heNdpShortNdpCnt,
                chspec_is20 ? 0x3f : chspec_is40 ? 0x7e : 0xfc);
        }
        // although bphy setting does not matter for 5G, still include the setting for 6715
        // so that phyreg comparsion between tcl and drv can be cleaner
        if (CHSPEC_IS2G(pi->radio_chanspec) || ACMAJORREV_130(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, bphyaciThresh0, bphyaciThresh0, chspec_is20 ? 0x1e : 0x12c);
            MOD_PHYREG(pi, bphyaciThresh1, bphyaciThresh1, chspec_is20 ? 0x55 : 0x145);
            MOD_PHYREG(pi, bphyaciThresh2, bphyaciThresh2, chspec_is20 ? 0x80 : 0x15e);
            MOD_PHYREG(pi, bphyaciThresh3, bphyaciThresh3, chspec_is20 ? 0xaa : 0x177);
            MOD_PHYREG(pi, bphyaciPwrThresh0, bphyaciPwrThresh0, chspec_is20 ?
                       0x2bc : 0x320);
            MOD_PHYREG(pi, bphyaciPwrThresh1, bphyaciPwrThresh1, chspec_is20 ?
                       0x320 : 0x384);
            MOD_PHYREG(pi, bphyaciPwrThresh2, bphyaciPwrThresh2, chspec_is20 ?
                       0x384 : 0x3b6);
            MOD_PHYREG(pi, bphyPreDetectThreshold6, ac_det_1us_aci_th,
                       chspec_is20 ? 0x80 : 0x200);
        }

        FOREACH_CORE(pi, core) {
            MOD_PHYREGC(pi, RssiClipMuxSel, core, fastAgcW0ClipCntTh,
                        chspec_is20 ? 0x0f :
                        chspec_is40 ? 0x1f :
                        chspec_is80 ? 0x3f : 0x7f);
            MOD_PHYREGCE(pi, crsThreshold2u, core, peakThresh, chspec_is20 ?
                         0x55 : 0x4d);
            MOD_PHYREGCE(pi, PREMPT_ofdm_nominal_clip_cnt_th, core,
                         ofdm_nominal_clip_cnt_th, chspec_is20 ? 0x10 :
                         chspec_is40 ? 0x20 : chspec_is80 ? 0x40 : 0x80);
            if (CHSPEC_IS2G(pi->radio_chanspec)) {
                MOD_PHYREGCE(pi, PREMPT_cck_nominal_clip_cnt_th, core,
                             cck_nominal_clip_cnt_th, chspec_is20 ? 0x18 :
                             chspec_is40 ? 0x30 : chspec_is80 ? 0x60 : 0xc0);
            }
            /* EANBLE time interleaved ADC for BW160 and DISABLE for others */
            if (chspec_is160) {
                MOD_PHYREGCE(pi, sarAfeCompCtrl, core, sarAfePhaseSel,
                    ACMAJORREV_130(pi->pubpi->phy_rev) ||
                    ACMAJORREV_131(pi->pubpi->phy_rev) ? 0: 1);
                MOD_PHYREGCE(pi, sarAfeCompCtrl, core, sarAfeCompMode, 1);
            } else {
                MOD_PHYREGCE(pi, sarAfeCompCtrl, core, sarAfePhaseSel, 0);
                MOD_PHYREGCE(pi, sarAfeCompCtrl, core, sarAfeCompMode, 0);
            }
        }
    } else {
        MOD_PHYREG(pi, ClassifierCtrl6, logACDelta2, chspec_is20 ?
                   0x13 : chspec_is40 ? 0x13 : 0x9);
        MOD_PHYREG(pi, ClassifierLogAC1, logACDelta1, chspec_is20 ?
                   0x13 : chspec_is40 ? 0x13 : 0x9);
        if (CHSPEC_IS2G(pi->radio_chanspec)) {
            if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
                MOD_PHYREG(pi, bphyPreDetectThreshold6, ac_det_1us_aci_th,
                           chspec_is20 ? 0x80 : 0x200);
            }
        }
    }

    regval = chspec_is20 ? 0x0e17 : chspec_is40 ? 0x162a : chspec_is80 ? 0x2c54 : 0x5854;
    FOREACH_CORE(pi, core) {
        MOD_PHYREGC(pi, Adcclip, core, adc_clip_cnt_th, chspec_is20 ? 0xa : 0x14);
        WRITE_PHYREGC(pi, FastAgcClipCntTh, core, regval);
        if (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))
            WRITE_PHYREGC(pi, FastAgcClipCntTh_aci, core, regval);
        if (ACMAJORREV_130(pi->pubpi->phy_rev))
            WRITE_PHYREGC(pi, FastAgcClipCntTh_macaided, core, regval);
    }

    // MAC-aided mode timer setting(BW specific)
    if (CHSPEC_IS2G(pi->radio_chanspec)) {
        // only has measured data for 20MHz. For 40MHz use 5G number for now.
        regval = chspec_is20 ? 79 : 36;
    } else {
        regval = chspec_is20 ? 72 : chspec_is40 ? 36 : chspec_is80 ? 20 : 1;
    }
    // FIXME: need to revisit the wait_deaf_time values for 6715
    if (ACMAJORREV_47(pi->pubpi->phy_rev) || ACMAJORREV_51_131(pi->pubpi->phy_rev) ||
            ACMAJORREV_129_130(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, Config2_MAC_ided_trig_frame, wait_deaf_time, regval);
    }

    if (!ACMAJORREV_0(pi->pubpi->phy_rev) &&
        !(ACMAJORREV_2(pi->pubpi->phy_rev) && ACMINORREV_0(pi))) {
        MOD_PHYREG(pi, CRSMiscellaneousParam, crsMfFlipCoef, chspec_is20 ? 0 : 1);
    }

    /* FIX ME : Currently setting only for 4350, Other phy revs should
     * check with RTL folks and set accordingly
     */
    if (ACMAJORREV_2(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, FSTRCtrl, fineStrSgiVldCntVal, chspec_is20 ?
            0x9 : 0xa);
        MOD_PHYREG(pi, FSTRCtrl, fineStrVldCntVal, chspec_is20 ?
            0x9 : 0xa);
    }
}

static void
wlc_phy_set_reg_on_bw_change_acphy_majorrev128(phy_info_t *pi)
{
    const bool chspec_is20 = CHSPEC_IS20(pi->radio_chanspec);
    const bool chspec_is40 = CHSPEC_IS40(pi->radio_chanspec);

    WRITE_PHYREG(pi, lesiCrsTypRxPowerPerCore,
        CHSPEC_IS20(pi->radio_chanspec) ?
        0x15b : CHSPEC_IS40(pi->radio_chanspec) ? 0x228 : 0x184);
    WRITE_PHYREG(pi, lesiCrsHighRxPowerPerCore,
        CHSPEC_IS20(pi->radio_chanspec) ?
        0x76e : CHSPEC_IS40(pi->radio_chanspec) ? 0x9c1 : 0x551);
    WRITE_PHYREG(pi, lesiCrsMinRxPowerPerCore,
        CHSPEC_IS20(pi->radio_chanspec) ?
        0x5 : CHSPEC_IS40(pi->radio_chanspec) ? 0x18 : 0x2c);

    WRITE_PHYREG(pi, lesiCrs1stDetThreshold_1,
        CHSPEC_IS20(pi->radio_chanspec) ?
        0x3344 : CHSPEC_IS40(pi->radio_chanspec) ? 0x2633 : 0x1e26);

    WRITE_PHYREG(pi, lesiCrs2ndDetThreshold_1,
        CHSPEC_IS20(pi->radio_chanspec) ?
        0x3344 : CHSPEC_IS40(pi->radio_chanspec) ? 0x2633 : 0x1e26);

    MOD_PHYREG(pi, lesiFstrControl3, cCrsFftInpAdj, CHSPEC_IS20(pi->radio_chanspec) ?
        0x1 : CHSPEC_IS40(pi->radio_chanspec) ? 0x1 : 0x3);
    MOD_PHYREG(pi, lesiFstrControl3, lCrsFftInpAdj, CHSPEC_IS20(pi->radio_chanspec) ?
        9: CHSPEC_IS40(pi->radio_chanspec) ? 19 : 40);

    MOD_PHYREG(pi, LesiFstrFdNoisePower0, noi_pow, CHSPEC_IS20(pi->radio_chanspec) ?
        175: CHSPEC_IS40(pi->radio_chanspec) ? 175 : 100);
    MOD_PHYREG(pi, LesiFstrFdNoisePower1, noi_pow, CHSPEC_IS20(pi->radio_chanspec) ?
        175: CHSPEC_IS40(pi->radio_chanspec) ? 175 : 100);

    WRITE_PHYREG(pi, ACIBrwdfCoef0,
        CHSPEC_IS20(pi->radio_chanspec) ? 0xdc31 :
        CHSPEC_IS40(pi->radio_chanspec) ? 0xc32b : 0xba28);
    WRITE_PHYREG(pi, ACIBrwdfCoef1,
        CHSPEC_IS20(pi->radio_chanspec) ? 0x0000 :
        CHSPEC_IS40(pi->radio_chanspec) ? 0x0000 : 0x00f7);
    WRITE_PHYREG(pi, ACIBrwdfCoef2,
        CHSPEC_IS20(pi->radio_chanspec) ? 0x008d :
        CHSPEC_IS40(pi->radio_chanspec) ? 0xee80 : 0xe179);
    MOD_PHYREG(pi, DcFiltAddressExt, dc_s2_bnd_start,
        chspec_is20 ? 0x10 :
        chspec_is40 ? 0x20 : 0x40);
    WRITE_PHYREG(pi, DcFiltAddressExt1,
        CHSPEC_IS20(pi->radio_chanspec) ? 0x1f30 :
        CHSPEC_IS40(pi->radio_chanspec) ? 0x3f60 : 0x7fc0);
    WRITE_PHYREG(pi, LtrnOffset,
        CHSPEC_IS20(pi->radio_chanspec) ? 0x090d : 0x0d11);
    WRITE_PHYREG(pi, MrcSigQualControl3,
        CHSPEC_IS20(pi->radio_chanspec) ? 0x071c : 0x0c30);

    /* ucflag to turn on/off lpf_spare WAR */
}

static void
wlc_phy_set_reg_on_bw_change_acphy_majorrev130(phy_info_t *pi)
{
    uint8 core;
    const bool chspec_is20 = CHSPEC_IS20(pi->radio_chanspec);
    const bool chspec_is40 = CHSPEC_IS40(pi->radio_chanspec);
    const bool chspec_is80 = CHSPEC_IS80(pi->radio_chanspec);
    const bool chspec_is160 = CHSPEC_IS160(pi->radio_chanspec);

    MOD_PHYREG(pi, HwRssiSampleCount, NumSampToCol, chspec_is20 ?
        0x50 : chspec_is40 ? 0xa0 : chspec_is80 ? 0x140 : 0x280);
    MOD_PHYREG(pi, HwRssiSampleCountLTF, NumSampToColLTF, chspec_is20 ?
        0x50 : chspec_is40 ? 0xa0 : chspec_is80 ? 0x140 : 0x280);
    WRITE_PHYREG(pi, HwRssiSampleCount_2xhltf, chspec_is20 ?
        0x50 : chspec_is40 ? 0xa0 : chspec_is80 ? 0x140 : 0x280);
    WRITE_PHYREG(pi, HwRssiSampleCount_4xhltf, chspec_is20 ?
        0x50 : chspec_is40 ? 0xa0 : chspec_is80 ? 0x140 : 0x280);
    MOD_PHYREG(pi, norm_var_hyst_th_pt8us, dis_upd_on_GainDsmpEnOut, chspec_is80 ?
        0x1 : 0x0);
    MOD_PHYREG(pi, ClassifierLogACPrimary0, logACDelta1P, chspec_is20 ?
        0x13 : chspec_is40 ? 0x13 : 0x9);
    MOD_PHYREG(pi, ClassifierLogACPrimary1, logACDelta2P, chspec_is20 ?
        0x13 : chspec_is40 ? 0x13 : 0x9);
    MOD_PHYREG(pi, rx1misc_1, rxPwrVarCntAdj, chspec_is20 ?
        0x1 : chspec_is40 ? 0x1 : 0x0);
    MOD_PHYREG(pi, rx1misc_1, LogAcAlignmentCount, chspec_is20 ?
        0x0 : chspec_is40 ? 0x0 : 0x0);
    MOD_PHYREG(pi, td_sfo_rst_ext, td_sfo_rst_ext, chspec_is160 ?
        0x10 : 0x4);
    MOD_PHYREG(pi, td_sfo_rst_ext, dly_match_count, chspec_is20 ?
        0xd : chspec_is40 ? 0x1c : chspec_is80 ? 0x39 : 0x72);
    FOREACH_CORE(pi, core) {
        MOD_PHYREGC(pi, Adcclip_aci, core, adc_clip_cnt_th, chspec_is20 ?
            0xa : 0x14);
        MOD_PHYREGC(pi, Adcclip_macaided, core, adc_clip_cnt_th, chspec_is20 ?
            0xa : 0x14);
    }

    /* To Improve BW160 Edge-Subband Sensitivity */
    if (RADIOMAJORREV(pi) > 1) {
        FOREACH_CORE(pi, core) {
            if (chspec_is160) {
                MOD_PHYREGC(pi, _TargetVar_log2, core, targetVar_log2, 437);
            } else {
                MOD_PHYREGC(pi, _TargetVar_log2, core, targetVar_log2, 448);
            }
        }
    }

    // mClp: to revisit when rxgcrs is brought up properly for 6715
    FOREACH_CORE(pi, core) {
        WRITE_PHYREGC(pi, mClpAgcW1ClipCntTh, core,
            chspec_is20 ? 0x0e0e :
            chspec_is40 ? 0x1c1c :
            chspec_is80 ? 0x3838 : 0x7070);
        WRITE_PHYREGC(pi, mClpAgcW3ClipCntTh, core,
            chspec_is20 ? 0x1717 :
            chspec_is40 ? 0x2e2e :
            chspec_is80 ? 0x5c5c : 0x5c5c);
        WRITE_PHYREGC(pi, mClpAgcNbClipCntTh, core,
            chspec_is20 ? 0x1717 :
            chspec_is40 ? 0x2e2e :
            chspec_is80 ? 0x5c5c : 0x5c5c);
        WRITE_PHYREGC(pi, mClpAgcMDClipCntTh1, core,
            chspec_is20 ? 0x1717 :
            chspec_is40 ? 0x2e2e :
            chspec_is80 ? 0x5c5c : 0x5c5c);
        MOD_PHYREGC(pi, mClpAgcMDClipCntTh2, core, mClpAgcW1ClipCntThMd,
            chspec_is20 ? 0x0e : chspec_is40 ? 0x1c : chspec_is80 ? 0x38 : 0x70);
        WRITE_PHYREGC(pi, mClpAgcW1ClipCntTh_aci, core,
            chspec_is20 ? 0x0e0e :
            chspec_is40 ? 0x1c1c :
            chspec_is80 ? 0x3838 : 0x7070);
        WRITE_PHYREGC(pi, mClpAgcW3ClipCntTh_aci, core,
            chspec_is20 ? 0x1717 :
            chspec_is40 ? 0x2e2e :
            chspec_is80 ? 0x5c5c : 0x5c5c);
        WRITE_PHYREGC(pi, mClpAgcNbClipCntTh_aci, core,
            chspec_is20 ? 0x1717 :
            chspec_is40 ? 0x2e2e :
            chspec_is80 ? 0x5c5c : 0x5c5c);
        WRITE_PHYREGC(pi, mClpAgcMDClipCntTh1_aci, core,
            chspec_is20 ? 0x1717 :
            chspec_is40 ? 0x2e2e :
            chspec_is80 ? 0x5c5c : 0x5c5c);
        MOD_PHYREGC(pi, mClpAgcMDClipCntTh2_aci, core, mClpAgcW1ClipCntThMd,
            chspec_is20 ? 0x0e : chspec_is40 ? 0x1c : chspec_is80 ? 0x38 : 0x70);
        WRITE_PHYREGC(pi, mClpAgcW1ClipCntTh_macaided, core,
            chspec_is20 ? 0x0e0e :
            chspec_is40 ? 0x1c1c :
            chspec_is80 ? 0x3838 : 0x7070);
        WRITE_PHYREGC(pi, mClpAgcW3ClipCntTh_macaided, core,
            chspec_is20 ? 0x1717 :
            chspec_is40 ? 0x2e2e :
            chspec_is80 ? 0x5c5c : 0x5c5c);
        WRITE_PHYREGC(pi, mClpAgcNbClipCntTh_macaided, core,
            chspec_is20 ? 0x1717 :
            chspec_is40 ? 0x2e2e :
            chspec_is80 ? 0x5c5c : 0x5c5c);
        WRITE_PHYREGC(pi, mClpAgcMDClipCntTh1_macaided, core,
            chspec_is20 ? 0x1717 :
            chspec_is40 ? 0x2e2e :
            chspec_is80 ? 0x5c5c : 0x5c5c);
        MOD_PHYREGC(pi, mClpAgcMDClipCntTh2_macaided, core, mClpAgcW1ClipCntThMd,
            chspec_is20 ? 0x0e : chspec_is40 ? 0x1c : chspec_is80 ? 0x38 : 0x70);
    }

    MOD_PHYREG(pi, RxFrontendCommon_HE_1, heTbNdpShortTbNdpCnt,
        chspec_is20 ? 0x96 : chspec_is40 ? 0x12c : chspec_is80 ? 0x26c : 0x1cc);

    // FFT input shift
    MOD_PHYREG(pi, fft_backoff_cont_0, fft_enable_input_shift_threshold_vht,
        chspec_is80 ? 0x2 : chspec_is160 ? 0x1 : 0x0);
    MOD_PHYREG(pi, fft_backoff_cont_0, fft_enable_input_shift_threshold_he_su,
        chspec_is80 ? 0x2 : chspec_is160 ? 0x1 : 0x0);
    MOD_PHYREG(pi, fft_backoff_cont_1, fft_enable_input_shift_threshold_he_dlofdma,
        chspec_is80 ? 0x1 : chspec_is160 ? 0x1 : 0x0);
    MOD_PHYREG(pi, fft_backoff_cont_1, fft_enable_input_shift_threshold_he_trig,
        chspec_is80 ? 0x1 : chspec_is160 ? 0x1 : 0x0);
    MOD_PHYREG(pi, fft_backoff_cont_1, fft_enable_input_shift_threshold_he_dlmumimo,
        chspec_is160 ? 0x1 : 0x0);
    MOD_PHYREG(pi, fft_backoff_cont_1, fft_enable_input_shift_threshold_he_tbndp,
        chspec_is20 ? 0x1 : chspec_is40 ? 0x2 : chspec_is80 ? 0x4 : 0x8);
    MOD_PHYREG(pi, fft_backoff_cont_2, fft_enable_input_shift_RUthreshold_he_trig,
        chspec_is80 ? 0x9 : chspec_is160 ? 0x9 : 0x0);
    WRITE_PHYREG(pi, target_var_log2_ul_fft_shift_0,
        chspec_is20 ? 0x1c0 : chspec_is40 ? 0x1c0 : chspec_is80 ? 0x180 : 0x141);
    WRITE_PHYREG(pi, target_var_log2_ul_fft_shift_1, chspec_is160 ? 0x180 : 0x1c0);
    WRITE_PHYREG(pi, target_var_log2_dl_fft_shift_0,
        chspec_is20 ? 0x1c0 : chspec_is40 ? 0x1c0 : chspec_is80 ? 0x180 : 0x141);
    WRITE_PHYREG(pi, target_var_log2_dl_fft_shift_1, chspec_is160 ? 0x180 : 0x1c0);

    MOD_PHYREG(pi, cpBufLengthPreAdj1, lengthadj1xLTF,
        chspec_is20 ? 0xa : chspec_is40 ? 0x1e : chspec_is80 ? 0x2a : 0x2a);

    // LESI
    MOD_PHYREG(pi, lesi_control_2, dcRotMeanCompensateEn,
        chspec_is20 ? 0x0 : 0x1);
    /* below phyregs are bw-dependent but written by rxgcrs_lesi function
    WRITE_PHYREG(pi, lesiCrsTypRxPowerPerCore,
        chspec_is20 ? 0x15b : chspec_is40 ? 0x228 : chspec_is80 ? 0x184 : 0x27e);
    WRITE_PHYREG(pi, lesiCrsHighRxPowerPerCore,
        chspec_is20 ? 0x76e : chspec_is40 ? 0x9c1 : chspec_is80 ? 0x551 : 0x64b);
    WRITE_PHYREG(pi, lesiCrsMinRxPowerPerCore,
        chspec_is20 ? 0x5 : chspec_is40 ? 0x18 : chspec_is80 ? 0x2c : 0x58);
    WRITE_PHYREG(pi, lesiCrs1stDetThreshold_1,
        chspec_is20 ? 0x2a3b : chspec_is40 ? 0x1d2a : chspec_is80 ? 0x151d : 0x0e15);
    WRITE_PHYREG(pi, lesiCrs1stDetThreshold_2,
        chspec_is20 ? 0x1d22 : chspec_is40 ? 0x1518 : chspec_is80 ? 0x0e11 : 0x0a0c);
    WRITE_PHYREG(pi, lesiCrs2ndDetThreshold_1,
        chspec_is20 ? 0x2a3b : chspec_is40 ? 0x1d2a : chspec_is80 ? 0x151d : 0x0e15);
    WRITE_PHYREG(pi, lesiCrs2ndDetThreshold_2,
        chspec_is20 ? 0x1d22 : chspec_is40 ? 0x1518 : chspec_is80 ? 0x0e11 : 0x0a0c);
    MOD_PHYREG(pi, lesiFstrControl3, cCrsFftInpAdj,
        chspec_is20 ? 0x0 : chspec_is40 ? 0x1 : chspec_is80 ? 0x3 : 0x7);
    WRITE_PHYREG(pi, lesiFstrModeSwitchHiPower,
        chspec_is20 ? 0x239 : chspec_is40 ? 0x33d : chspec_is80 ? 0x211 : 0x308);
    WRITE_PHYREG(pi, lesiFstrModeSwitchLoPower,
        chspec_is20 ? 0xec : chspec_is40 ? 0x19d : chspec_is80 ? 0x13f : 0x239);
    MOD_PHYREG(pi, lesiFstrControl4, lCrsFftOp1Adj,
        chspec_is20 ? 0x1 : chspec_is40 ? 0x2 : chspec_is80 ? 0x4 : 0x8);
    MOD_PHYREG(pi, lesiFstrControl5, lCrsFftInpAdj,
        chspec_is20 ? 0x9 : chspec_is40 ? 0x13 : chspec_is80 ? 0x28 : 0x51);
    ACPHYREG_BCAST(pi, lesiFstrClassifierEqualizationFactor0_0, chspec_is80 ?
        0x2030 : 0x2020);
    ACPHYREG_BCAST(pi, lesiFstrClassifierEqualizationFactor1_0, chspec_is80 ?
        0x3020 : 0x2020);
    ACPHYREG_BCAST(pi, LesiFstrFdNoisePower0,
        chspec_is20 ? 0xaf : chspec_is40 ? 0xaf : 0x64);
    */

    // Preemption
    /* if-operator might change depending on 6G channel */
    ACPHYREG_BCAST(pi, PREMPT_cck_nominal_clip_cnt_th0,
        chspec_is20 ? 0x18 : chspec_is40 ? 0x30 : chspec_is80 ? 0x60 : 0xc0);
}

/* Load pdet related Rfseq on reset */
static void
wlc_phy_set_pdet_on_reset_acphy(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;
    uint8 core, pdet_range_id, subband_idx, ant, core_freq_segment_map;
    uint16 offset, tmp_val, val_av, val_vmid;
    uint8 bands[NUM_CHANS_IN_CHAN_BONDING];
    uint8 av[4] = {0, 0, 0, 0};
    uint8 vmid[4] = {0, 0, 0, 0};
    uint8 stall_val;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);
    bool flag2rangeon =
        ((CHSPEC_IS2G(pi->radio_chanspec) && phy_tpc_get_tworangetssi2g(pi->tpci)) ||
        (CHSPEC_ISPHY5G6G(pi->radio_chanspec) && phy_tpc_get_tworangetssi5g(pi->tpci))) &&
        PHY_IPA(pi);

    BCM_REFERENCE(stf_shdata);

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));
    if (CHSPEC_IS2G(pi->radio_chanspec)) {
        pdet_range_id = phy_tpc_get_2g_pdrange_id(pi->tpci);
    } else {
        pdet_range_id = phy_tpc_get_5g_pdrange_id(pi->tpci);
    }

    FOREACH_CORE(pi, core) {
        /* core_freq_segment_map is only required for 80P80 mode
         For other modes, it is ignored
        */
        if (PHY_AS_80P80(pi, pi->radio_chanspec)) {
            phy_ac_chanmgr_get_chan_freq_range_80p80(pi, 0, bands);
            if (ACMAJORREV_33(pi->pubpi->phy_rev) ||
                ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
                subband_idx = (core <= 1) ? bands[0] : bands[1];
            } else {
                subband_idx = bands[0];
                ASSERT(0); // FIXME
            }
        } else {
            core_freq_segment_map = pi_ac->chanmgri->data->core_freq_mapping[core];
            subband_idx = phy_ac_chanmgr_get_chan_freq_range(pi, 0,
                core_freq_segment_map);
        }
        ant = phy_get_rsdbbrd_corenum(pi, core);
        if (BF3_AVVMID_FROM_NVRAM(pi->u.pi_acphy)) {
            av[core] = pi_ac->sromi->avvmid_set_from_nvram[ant][subband_idx][0];
            vmid[core] = pi_ac->sromi->avvmid_set_from_nvram[ant][subband_idx][1];
        } else {
            if (ACMAJORREV_0(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
                /* 4360 and 43602 */
                av[core] = avvmid_set[pdet_range_id][subband_idx][ant];
                vmid[core] = avvmid_set[pdet_range_id][subband_idx][ant+3];
            } else if (ACMAJORREV_2(pi->pubpi->phy_rev)) {
                av[core] = avvmid_set2[pdet_range_id][subband_idx][ant];
                vmid[core] = avvmid_set2[pdet_range_id][subband_idx][ant+2];
            } else if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
                av[core] = avvmid_set4[pdet_range_id][subband_idx][ant];
                vmid[core] =
                        avvmid_set4[pdet_range_id][subband_idx][ant+2];
            } else if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                ACMAJORREV_33(pi->pubpi->phy_rev)) {
                av[core] = avvmid_set32[pdet_range_id][subband_idx][ant];
                vmid[core] = avvmid_set32[pdet_range_id][subband_idx][ant+4];
            } else if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
                av[core] =
                    avvmid_set47_1[pdet_range_id][subband_idx][ant];
                vmid[core] =
                    avvmid_set47_1[pdet_range_id][subband_idx][ant+4];
                if (pdet_range_id == 0 && pi->sh->boardrev >= 5123) {
                //different avvmid for MCM P403 or later, 0x1403 = 5123
                    av[core] =
                    avvmid_set47_2[pdet_range_id][subband_idx][ant];
                    vmid[core] =
                    avvmid_set47_2[pdet_range_id][subband_idx][ant+4];
                }
            } else if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
                av[core] = avvmid_set47[pdet_range_id][subband_idx][ant];
                vmid[core] = avvmid_set47[pdet_range_id][subband_idx][ant+4];
            } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
                av[core] = avvmid_set129[pdet_range_id][subband_idx][ant];
                vmid[core] = avvmid_set129[pdet_range_id][subband_idx][ant+4];
            } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
                av[core] = avvmid_set130[pdet_range_id][subband_idx][ant];
                vmid[core] = avvmid_set130[pdet_range_id][subband_idx][ant+4];
            } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
                uint8 *p_avvmid_set =
                    get_avvmid_set_128(pi, pdet_range_id, subband_idx);
                av[core] = p_avvmid_set[ant];
                vmid[core] = p_avvmid_set[ant+3];
            }
        }
    }
    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    ACPHY_DISABLE_STALL(pi);

    FOREACH_ACTV_CORE(pi, stf_shdata->hw_phyrxchain, core) {
        if (core == 3)
            offset = 0x560 + 0xd;
        else
            offset = 0x3c0 + 0xd + core*0x10;
        wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, offset, 16, &tmp_val);
        val_av = (tmp_val & 0x1ff8) | (av[core]&0x7);
        val_vmid = (val_av & 0x7) | ((vmid[core]&0x3ff)<<3);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, offset, 16, &val_vmid);

        if (((ACMAJORREV_2(pi->pubpi->phy_rev) ||
            ACMAJORREV_4(pi->pubpi->phy_rev)) &&
            BF3_TSSI_DIV_WAR(pi->u.pi_acphy)) ||
            flag2rangeon) {
            offset = 0x3c0 + 0xe + core*0x10;
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, offset, 16,
                &val_vmid);
        }
    }
    ACPHY_ENABLE_STALL(pi, stall_val);
}

static void
wlc_phy_program_bphy_txfilt_coeffs(phy_info_t *pi, uint8 filter_type)
{
    /* supported filter type: 0, 1, 2 */
    if (filter_type <= 2) {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_TXFILTTBL, 4, 0, 32,
            bphy_filt_coefs_rev130[filter_type]);
    }
}

static void
wlc_phy_set_tx_iir_coeffs(phy_info_t *pi, bool cck, uint8 filter_type)
{
    if (cck == FALSE) {
        if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
            /* Default filters */
            if (filter_type == 0) {
                if (CHSPEC_IS2G(pi->radio_chanspec) &&
                        !ROUTER_4349(pi) && !PHY_IPA(pi)) {
                    ACPHY_REG_LIST_START
                    /* Default Chebyshev ~10.5MHz cutoff */
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st0a1, 0x0056)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st0a2, 0x02fb)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st0n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st1a1, 0x0f3d)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st1a2, 0x0169)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st1n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st2a1, 0x0e23)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st2a2, 0x0068)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st2n, 0x0002)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20finescale, 0x00E9)
                    ACPHY_REG_LIST_EXECUTE(pi);
                } else {
                    ACPHY_REG_LIST_START
                    /* Default Chebyshev ~10.5MHz cutoff */
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st0a1, 0x0056)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st0a2, 0x02fb)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st0n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st1a1, 0x0f3d)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st1a2, 0x0169)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st1n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st2a1, 0x0e23)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st2a2, 0x0068)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st2n, 0x0002)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20finescale, 0x00a6)
                    ACPHY_REG_LIST_EXECUTE(pi);
                }
            } else if (filter_type == 1) {
                ACPHY_REG_LIST_START
                     /* Chebyshev ~8.8MHz cutoff (FCC -26dBr BW) */
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st0a1, 0x0e73)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st0a2, 0x033d)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st0n, 0x0002)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st1a1, 0x0d5f)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st1a2, 0x0205)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st1n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st2a1, 0x0c39)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st2a2, 0x011e)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20st2n, 0x0002)
                    WRITE_PHYREG_ENTRY(pi, txfilt20in20finescale, 0x001a)
                ACPHY_REG_LIST_EXECUTE(pi);
            }
        } else if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
            if (filter_type == 4) { /* narrower filter - cheby1: 6, 0.1, 9.1/20 */
                WRITE_PHYREG(pi, txfilt20in20st0a1, 0x0f6b);
                WRITE_PHYREG(pi, txfilt20in20st0a2, 0x0339);
                WRITE_PHYREG(pi, txfilt20in20st0n,  0x0002);
                WRITE_PHYREG(pi, txfilt20in20st1a1, 0x0e29);
                WRITE_PHYREG(pi, txfilt20in20st1a2, 0x01e5);
                WRITE_PHYREG(pi, txfilt20in20st1n,  0x0003);
                WRITE_PHYREG(pi, txfilt20in20st2a1, 0x0cb2);
                WRITE_PHYREG(pi, txfilt20in20st2a2, 0x00f0);
                WRITE_PHYREG(pi, txfilt20in20st2n,  0x0003);
                WRITE_PHYREG(pi, txfilt20in20finescale, 0x0054);
            }
        }
    } else {
        /* Tx filters in PHY REV 3, PHY REV 6 and later operate at 1/2 the sampling
         * rate of previous revs
         */
        if ((ACMAJORREV_0(pi->pubpi->phy_rev) && (ACMINORREV_0(pi) || ACMINORREV_1(pi))) ||
            ACMAJORREV_4(pi->pubpi->phy_rev)) {
        if (filter_type == 0) {
            ACPHY_REG_LIST_START
                /* Default filter */
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a1, 0x0a94)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a2, 0x0373)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0n, 0x0005)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a1, 0x0a93)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a2, 0x0298)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1n, 0x0004)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a1, 0x0a52)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a2, 0x021d)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2n, 0x0004)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20finescale, 0x0080)
            ACPHY_REG_LIST_EXECUTE(pi);
        } else if (filter_type == 1) {
            ACPHY_REG_LIST_START
                /* Gaussian  shaping filter */
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a1, 0x0b54)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a2, 0x0290)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0n, 0x0004)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a1, 0x0a40)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a2, 0x0290)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1n, 0x0005)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a1, 0x0a06)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a2, 0x0240)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2n, 0x0005)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20finescale, 0x0080)
            ACPHY_REG_LIST_EXECUTE(pi);
        } else if (filter_type == 4) {
            if (phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->dac_mode == 1) {
                ACPHY_REG_LIST_START
                    /* Gaussian shaping filter for TINY_A0, dac_rate_mode 1 */
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a1, -80)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a2, 369)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0n, 3)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a1, -757)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a2, 369)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1n, 3)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a1, -1007)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a2, 256)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2n, 3)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20finescale, 120)
                ACPHY_REG_LIST_EXECUTE(pi);
            } else if (phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->dac_mode == 2) {
                ACPHY_REG_LIST_START
                    /* Gaussian shaping filter for TINY_A0, dac_rate_mode 2 */
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in80st0a1, -1852)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in80st0a2, 892)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in80st0n, 7)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in80st1a1, -1890)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in80st1a2, 892)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in80st1n, 7)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in80st2a1, -1877)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in80st2a2, 860)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in80st2n, 7)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in80finescale, 65)
                ACPHY_REG_LIST_EXECUTE(pi);
            } else {
                ACPHY_REG_LIST_START
                    /* Gaussian shaping filter for TINY_A0, dac_rate_mode 3 */
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in40st0a1, -1714)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in40st0a2, 829)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in40st0n, 6)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in40st1a1, -1796)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in40st1a2, 829)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in40st1n, 6)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in40st2a1, -1790)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in40st2a2, 784)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in40st2n, 6)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in40finescale, 54)
                ACPHY_REG_LIST_EXECUTE(pi);
            }
        } else if (filter_type == 5) {
            ACPHY_REG_LIST_START
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a1, -48)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a2, 1)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0n, 3)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a1, -75)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a2, 23)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1n, 3)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a1, -504)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a2, 64)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2n, 3)
                WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20finescale, 175)
            ACPHY_REG_LIST_EXECUTE(pi);
        }
    } else if (ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev) ||
        ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        ACMAJORREV_GE47(pi->pubpi->phy_rev)) {

        // for 6715, tx iir filter coeffs is setup through PHY TABLE
        if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            wlc_phy_program_bphy_txfilt_coeffs(pi, filter_type);
        } else {
            if (filter_type == 0) {
                ACPHY_REG_LIST_START
                    /* Default filter */
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a1, 0x0f6b)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a2, 0x0339)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a1, 0x0e29)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a2, 0x01e5)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1n, 0x0002)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a1, 0x0cb2)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a2, 0x00f0)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20finescale, 0x00b3)
                ACPHY_REG_LIST_EXECUTE(pi);
            } else if (filter_type == 1) {
                if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                    ACMAJORREV_33(pi->pubpi->phy_rev) ||
                    ACMAJORREV_47(pi->pubpi->phy_rev) ||
                    ACMAJORREV_51(pi->pubpi->phy_rev) ||
                    ACMAJORREV_128(pi->pubpi->phy_rev) ||
                    ACMAJORREV_129(pi->pubpi->phy_rev) ||
                    ACMAJORREV_131(pi->pubpi->phy_rev)) {
                    ACPHY_REG_LIST_START
                    /* Gaussian shaping filter (-0.5 dB Tx Power)
                    * Fine tune scaling to align pwr_vs_idx with ofdm
                    */
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a1, 0x0edb)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a2, 0x01cb)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a1, 0x0d1d)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a2, 0x0192)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a1, 0x0c33)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a2, 0x00f3)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20finescale, 0x0084)
                    ACPHY_REG_LIST_EXECUTE(pi);
                } else {
                    ACPHY_REG_LIST_START
                    /* Gaussian shaping filter (-0.5 dB Tx Power) */
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a1, 0x0edb)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a2, 0x01cb)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a1, 0x0d1d)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a2, 0x0192)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a1, 0x0c33)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a2, 0x00f3)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20finescale, 0x0076)
                    ACPHY_REG_LIST_EXECUTE(pi);
                }
            } else if (filter_type == 2) {
                ACPHY_REG_LIST_START
                    /* Tweaked Gaussian for 4335 iPA CCk margin */
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a1, 0x0edb)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a2, 0x01ab)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a1, 0x0d1d)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a2, 0x0172)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a1, 0x0c77)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a2, 0x00a9)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20finescale, 0x0082)
                ACPHY_REG_LIST_EXECUTE(pi);
            } else if (filter_type == 4) {
                /* Tweaked Gaussian for 43012 CCk margin */
                ACPHY_REG_LIST_START
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a1, 0x0edb)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a2, 0x01ab)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a1, 0x0d1d)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a2, 0x0172)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a1, 0x0c77)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a2, 0x00a9)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20finescale, 0x0082)
                ACPHY_REG_LIST_EXECUTE(pi);
            }
        }

        } else {
            if (filter_type == 0) {
                ACPHY_REG_LIST_START
                    /* Default filter */
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a1, 0x0f6b)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0a2, 0x0339)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st0n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a1, 0x0e29)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1a2, 0x01e5)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st1n, 0x0002)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a1, 0x0cb2)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2a2, 0x00f0)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20st2n, 0x0003)
                    WRITE_PHYREG_ENTRY(pi, txfiltbphy20in20finescale, 0x00b3)
                ACPHY_REG_LIST_EXECUTE(pi);
            } else if (filter_type == 1) {
                /* TBD */
            }
        }
    }
}

static void
wlc_phy_set_sdadc_pd_val_per_core_acphy(phy_info_t *pi)
{
    if (ACMAJORREV_2(pi->pubpi->phy_rev) && !(PHY_IPA(pi))) {
        bool suspend;
        uint8 stall_val, orig_rxfectrl1;
        uint16 rx_sd_adc_pd_val[2] = {0x20, 0x20};
        uint16 rx_sd_adc_pd_cfg[2] = {0x00, 0x00};
        uint8 core;
        uint8 phyrxchain;

        int fc;
        const void *chan_info = NULL;
        uint8 ch = CHSPEC_CHANNEL(pi->radio_chanspec);
        phy_ac_radio_data_t *rd = phy_ac_radio_get_data(pi->u.pi_acphy->radioi);

        BCM_REFERENCE(phyrxchain);

        suspend = !(R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC);
        if (!suspend)
            wlapi_suspend_mac_and_wait(pi->sh->physhim);
        /* Disable stalls and hold FIFOs in reset */
        stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
        orig_rxfectrl1 = READ_PHYREGFLD(pi, RxFeCtrl1, soft_sdfeFifoReset);
        if (stall_val == 0)
            ACPHY_DISABLE_STALL(pi);
        MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, 1);

        ASSERT(RADIOID_IS(pi->pubpi->radioid, BCM2069_ID));
        fc = wlc_phy_chan2freq_acphy(pi, ch, &chan_info);

            phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;
            FOREACH_ACTV_CORE(pi, phyrxchain, core) {

            if (CHSPEC_IS2G(pi->radio_chanspec)) {
                if (rd->srom_txnospurmod2g
                        == 0) {
                    rx_sd_adc_pd_val[core] = 0x3d;
                    rx_sd_adc_pd_cfg[core] = 0x1a53;
                } else {
                    if ((fc == 2412) || (fc == 2467)) {
                        rx_sd_adc_pd_val[core] = 0x3d;
                        rx_sd_adc_pd_cfg[core] = 0x1b53;
                    } else {
                        rx_sd_adc_pd_val[core] = 0x3d;
                        rx_sd_adc_pd_cfg[core] = 0x1c53;
                    }
                }
            } else {
                if (CHSPEC_IS20(pi->radio_chanspec)) {
                    if (((fc >= 5180) && (fc <= 5320)) ||
                    ((fc >= 5745) && (fc <= 5825))) {
                        rx_sd_adc_pd_val[core] = 0x3d;
                        rx_sd_adc_pd_cfg[core] = 0x1f12;
                    } else {
                        rx_sd_adc_pd_val[core] = 0x3d;
                        rx_sd_adc_pd_cfg[core] = 0x1B53;
                    }
                } else if (CHSPEC_IS40(pi->radio_chanspec)) {
                    if (fc == 5190) {
                        rx_sd_adc_pd_val[core] = 0x3f;
                        rx_sd_adc_pd_cfg[core] = 0x1818;
                    } else {
                        rx_sd_adc_pd_val[core] = 0x3d;
                        rx_sd_adc_pd_cfg[core] = 0x1f12;
                    }
                } else {
                    rx_sd_adc_pd_val[core] = 0x3f;
                    rx_sd_adc_pd_cfg[core] = 0x1818;
                }
            }

        }

        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x3c6, 16,
            &rx_sd_adc_pd_val[0]);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x3c9, 16,
            &rx_sd_adc_pd_cfg[0]);

        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x3d6, 16,
            &rx_sd_adc_pd_val[1]);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x3d9, 16,
            &rx_sd_adc_pd_cfg[1]);

        /* Restore FIFO reset and Stalls */
        if (!suspend)
            wlapi_enable_mac(pi->sh->physhim);
        ACPHY_ENABLE_STALL(pi, stall_val);
        MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, orig_rxfectrl1);

    }
}

static void
wlc_phy_tx_gm_gain_boost(phy_info_t *pi)
{
    uint8 core;

    ASSERT(RADIOID_IS(pi->pubpi->radioid, BCM2069_ID));

    if (CHSPEC_IS2G(pi->radio_chanspec)) {
        uint8 phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;
        BCM_REFERENCE(phyrxchain);
        FOREACH_ACTV_CORE(pi, phyrxchain, core) {
            MOD_RADIO_REGC(pi, TXGM_CFG1, core, gc_res, 0x1);
        }
    } else {
        if (BF_SROM11_GAINBOOSTA01(pi->u.pi_acphy)) {
            /* Boost A0/1 radio gain */
            FOREACH_CORE(pi, core) {
                MOD_RADIO_REGC(pi, TXMIX5G_CFG1, core, gainboost, 0x6);
                MOD_RADIO_REGC(pi, PGA5G_CFG1, core, gainboost, 0x6);
            }
        }
        if (RADIO2069REV(pi->pubpi->radiorev) <= 3) {
            /* Boost A2 radio gain */
            core = 2;
            MOD_RADIO_REGC(pi, TXMIX5G_CFG1, core, gainboost, 0x6);
            MOD_RADIO_REGC(pi, PGA5G_CFG1, core, gainboost, 0x6);
        }
    }
}

static void
wlc_phy_write_rx_farrow_pre_tiny(phy_info_t *pi, chan_info_rx_farrow *rx_farrow,
    chanspec_t chanspec)
{
    uint16 deltaphase_lo, deltaphase_hi;
    uint16 drift_period, farrow_ctrl;

#ifdef ACPHY_1X1_ONLY
    uint8 channel = CHSPEC_CHANNEL(chanspec);
    uint32 deltaphase;

    if (channel <= 14) {
        if (CHSPEC_IS20(chanspec))
            drift_period = 5120; /* 40x32x4 */
        else if (CHSPEC_IS40(chanspec))
            drift_period = 5120; /* 40x32x4 */
        else
            drift_period = 1280; /* 160x4x2 */
    } else {
        if (CHSPEC_IS20(chanspec))
            drift_period = 3840; /* 40x24x4 */
        else if (CHSPEC_IS40(chanspec))
            drift_period = 3840; /* 40x24x4 */
        else
            drift_period = 2880; /* 160x9x2 */
    }

    if (CHSPEC_IS80(chanspec)) {
        deltaphase = rx_farrow->deltaphase_80;
        farrow_ctrl = rx_farrow->farrow_ctrl_80;
    } else {
        deltaphase = rx_farrow->deltaphase_20_40;
        farrow_ctrl = rx_farrow->farrow_ctrl_20_40;
    }

    deltaphase_lo = deltaphase & 0xffff;
    deltaphase_hi = (deltaphase >> 16) & 0xff;

#else  /* ACPHY_1X1_ONLY */
    UNUSED_PARAMETER(chanspec);

    /* Setup the Rx Farrow */
    deltaphase_lo = rx_farrow->deltaphase_lo;
    deltaphase_hi = rx_farrow->deltaphase_hi;
    drift_period = rx_farrow->drift_period;
    farrow_ctrl = rx_farrow->farrow_ctrl;

#endif  /* ACPHY_1X1_ONLY */
    /* Setup the Rx Farrow */
    WRITE_PHYREG(pi, rxFarrowDeltaPhase_lo, deltaphase_lo);
    WRITE_PHYREG(pi, rxFarrowDeltaPhase_hi, deltaphase_hi);
    WRITE_PHYREG(pi, rxFarrowDriftPeriod, drift_period);
    WRITE_PHYREG(pi, rxFarrowCtrl, farrow_ctrl);

    /* Use the same settings for the loopback Farrow */
    WRITE_PHYREG(pi, lbFarrowDeltaPhase_lo, deltaphase_lo);
    WRITE_PHYREG(pi, lbFarrowDeltaPhase_hi, deltaphase_hi);
    WRITE_PHYREG(pi, lbFarrowDriftPeriod, drift_period);
    WRITE_PHYREG(pi, lbFarrowCtrl, farrow_ctrl);
}

static void
BCMATTACHFN(phy_ac_chanmgr_nvram_attach)(phy_ac_chanmgr_info_t *ac_info)
{
    uint8 i;
    uint8 csml;
#ifndef BOARD_FLAGS3
    uint32 bfl3; /* boardflags3 */
#endif
    phy_info_t *pi = ac_info->pi;
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

    csml = (uint8)PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_csml, 0x11);

    ac_info->ml_en =  (csml & 0xF);
    ac_info->chsm_en =  (csml & 0xF0) >> 4;

    if (ACMAJORREV_128(pi->pubpi->phy_rev) || ACMAJORREV_131(pi->pubpi->phy_rev)) {
        /* Save LESI enable/disable flag per band from NVRAM file that will be used */
        /* during band change */
        /* 2G - default on */
        ac_info->lesi_perband[0] = (uint8)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
            rstr_lesi_en, 0, 1);
        /* 5G - default on */
        ac_info->lesi_perband[1] = (uint8)PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
            rstr_lesi_en, 1, 1);
    }
    pi->sh->bphymrc_en = (uint8)PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_bphymrc_en, 0);

    if ((PHY_GETVAR_SLICE(pi, rstr_cckdigfilttype)) != NULL) {
        ac_info->acphy_cck_dig_filt_type = (uint8)PHY_GETINTVAR_SLICE(pi,
            rstr_cckdigfilttype);
    } else {
        /* bit0 is gaussian shaping and bit1 & 2 are for RRC alpha */
        ac_info->acphy_cck_dig_filt_type = 0x01;
    }

    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* low mcs gamma and gain values for PAPRR */
        for (i = 0; i < NUM_MCS_PAPRR_GAMMA; i++) {
            pi->paprrmcsgamma2g[i] = (int16) (PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
                rstr_paprrmcsgamma2g, i, -1));
            pi->paprrmcsgain2g[i] = (uint8) (PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
                rstr_paprrmcsgain2g, i, 128));
            pi->paprrmcsgamma5g20[i] = (int16) (PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
                rstr_paprrmcsgamma5g20, i, -1));
            pi->paprrmcsgain5g20[i] = (uint8) (PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
                rstr_paprrmcsgain5g20, i, 128));
            pi->paprrmcsgamma5g40[i] = (int16) (PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
                rstr_paprrmcsgamma5g40, i, -1));
            pi->paprrmcsgain5g40[i] = (uint8) (PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
                rstr_paprrmcsgain5g40, i, 128));
            pi->paprrmcsgamma5g80[i] = (int16) (PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
                rstr_paprrmcsgamma5g80, i, -1));
            pi->paprrmcsgain5g80[i] = (uint8) (PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
                rstr_paprrmcsgain5g80, i, 128));
        }
    }
#ifndef WL_FDSS_DISABLED
    pi->fdss_bandedge_2g_en = (uint8) (PHY_GETINTVAR_DEFAULT_SLICE(pi,
        rstr_fdss_bandedge_2g_en, 0));
    pi->fdss_interp_en = (uint8) (PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_fdss_interp_en, 1));
    for (i = 0; i < PHY_CORE_MAX; i++) {
        pi->fdss_level_2g[i] = (int8) (PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
            rstr_fdss_level_2g, i, -1));
        pi->fdss_level_5g[i] = (int8) (PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
            rstr_fdss_level_5g, i, -1));
        pi->fdss_level_2g_ch13[i] = (int8) (PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
            rstr_fdss_level_2g_ch13, i, (pi->fdss_level_2g[i])));
        pi->fdss_level_2g_ch1[i] = (int8) (PHY_GETINTVAR_ARRAY_DEFAULT_SLICE(pi,
            rstr_fdss_level_2g_ch1, i, (pi->fdss_level_2g[i])));
    }
#endif /* !WL_FDSS_DISABLED */
    ac_info->cfg->srom_paprdis = (bool)PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_paprdis, FALSE);
    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        ac_info->cfg->srom_paprdis = (bool)PHY_GETINTVAR_DEFAULT_SLICE(pi,
            rstr_paprdis, TRUE);
    } else {
        ac_info->cfg->srom_paprdis = (bool)PHY_GETINTVAR_DEFAULT_SLICE(pi,
            rstr_paprdis, FALSE);
    }
    ac_info->cfg->srom_papdwar = (int8)PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_papdwar, -1);
    ac_info->cfg->srom_txnoBW80ClkSwitch = (uint8)PHY_GETINTVAR_DEFAULT_SLICE(pi,
        rstr_txnoBW80ClkSwitch, 0);
    ac_info->cfg->srom_nonbf_logen_mode_en = (uint8)PHY_GETINTVAR_DEFAULT_SLICE
        (pi, rstr_nonbf_logen_mode_en, 0);
    ac_info->cfg->srom_tssisleep_en =
        (uint)PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_tssisleep_en, 0);
#ifndef BOARD_FLAGS
    BF_SROM11_GAINBOOSTA01(pi_ac) = ((BOARDFLAGS(GENERIC_PHY_INFO(pi)->boardflags) &
        BFL_SROM11_GAINBOOSTA01) != 0);
#endif /* BOARD_FLAGS */

#ifndef BOARD_FLAGS2
    BF2_SROM11_APLL_WAR(pi_ac) = ((BOARDFLAGS2(GENERIC_PHY_INFO(pi)->boardflags2) &
        BFL2_SROM11_APLL_WAR) != 0);
    BF2_2G_SPUR_WAR(pi_ac) = ((BOARDFLAGS2(GENERIC_PHY_INFO(pi)->boardflags2) &
        BFL2_2G_SPUR_WAR) != 0);
    BF2_DAC_SPUR_IMPROVEMENT(pi_ac) = (BOARDFLAGS2(GENERIC_PHY_INFO(pi)->boardflags2) &
        BFL2_DAC_SPUR_IMPROVEMENT) != 0;
#endif /* BOARD_FLAGS2 */

#ifndef BOARD_FLAGS3
    if ((PHY_GETVAR_SLICE(pi, rstr_boardflags3)) != NULL) {
        bfl3 = (uint32)PHY_GETINTVAR_SLICE(pi, rstr_boardflags3);
        BF3_AVVMID_FROM_NVRAM(pi_ac) = (bfl3 & BFL3_AVVMID_FROM_NVRAM)
            >> BFL3_AVVMID_FROM_NVRAM_SHIFT;
        BF3_BBPLL_SPR_MODE_DIS(pi_ac) = ((bfl3 & BFL3_BBPLL_SPR_MODE_DIS) != 0);
        BF3_PHASETRACK_MAX_ALPHABETA(pi_ac) = (bfl3 & BFL3_PHASETRACK_MAX_ALPHABETA) >>
            BFL3_PHASETRACK_MAX_ALPHABETA_SHIFT;
        BF3_ACPHY_LPMODE_2G(pi_ac) = (bfl3 & BFL3_ACPHY_LPMODE_2G) >>
            BFL3_ACPHY_LPMODE_2G_SHIFT;
        BF3_ACPHY_LPMODE_5G(pi_ac) = (bfl3 & BFL3_ACPHY_LPMODE_5G) >>
            BFL3_ACPHY_LPMODE_5G_SHIFT;
        BF3_RSDB_1x1_BOARD(pi_ac) = (bfl3 & BFL3_1X1_RSDB_ANT) >>
            BFL3_1X1_RSDB_ANT_SHIFT;
        BF3_5G_SPUR_WAR(pi_ac) = ((bfl3 & BFL3_5G_SPUR_WAR) != 0);
    } else {
        BF3_BBPLL_SPR_MODE_DIS(pi_ac) = 0;
        BF3_PHASETRACK_MAX_ALPHABETA(pi_ac) = 0;
        BF3_ACPHY_LPMODE_2G(pi_ac) = 0;
        BF3_ACPHY_LPMODE_5G(pi_ac) = 0;
        BF3_RSDB_1x1_BOARD(pi_ac) = 0;
        BF3_5G_SPUR_WAR(pi_ac) = 0;
    }
#endif /* BOARD_FLAGS3 */

    pi->sromi->ofdmfilttype = (uint8)PHY_GETINTVAR_DEFAULT_SLICE(pi,
        rstr_ofdmfilttype_5gbe, 127);
    pi->sromi->ofdmfilttype_2g = (uint8)PHY_GETINTVAR_DEFAULT_SLICE(pi,
        rstr_ofdmfilttype_2gbe, 127);

    if ((PHY_GETVAR_SLICE(pi, ed_thresh2g)) != NULL) {
        pi_ac->sromi->ed_thresh2g = (int32)PHY_GETINTVAR_SLICE(pi, ed_thresh2g);
    } else {
        pi_ac->sromi->ed_thresh2g = 0;
    }

    if ((PHY_GETVAR_SLICE(pi, ed_thresh5g)) != NULL) {
        pi_ac->sromi->ed_thresh5g = (int32)PHY_GETINTVAR_SLICE(pi, ed_thresh5g);
    } else {
        pi_ac->sromi->ed_thresh5g = 0;
    }

    if ((PHY_GETVAR_SLICE(pi, "eu_edthresh2g")) != NULL) {
        pi->srom_eu_edthresh2g = (int8)PHY_GETINTVAR_SLICE(pi, "eu_edthresh2g");
    } else {
        pi->srom_eu_edthresh2g = 0;
    }
    if ((PHY_GETVAR_SLICE(pi, "eu_edthresh5g")) != NULL) {
        pi->srom_eu_edthresh5g = (int8)PHY_GETINTVAR_SLICE(pi, "eu_edthresh5g");
    } else {
        pi->srom_eu_edthresh5g = 0;
    }

#ifdef CONFIG_TENDA_PRIVATE_WLAN
    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* -65, -71 (based on formula), though higher on chip */
        pi_ac->sromi->ed_thresh_default = STATIC_ED_THRESH_DEFAULT;     /*Avoid sta connection failure when the interference is large*/
    } else {
        /* -69, -75 (based on formula), though higher on chip */
        pi_ac->sromi->ed_thresh_default = STATIC_ED_THRESH_DEFAULT;    /*Avoid sta connection failure when the interference is large*/
    }
#else
    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* -65, -71 (based on formula), though higher on chip */
        pi_ac->sromi->ed_thresh_default = -65;
    } else {
        /* -69, -75 (based on formula), though higher on chip */
        pi_ac->sromi->ed_thresh_default = -69;
    }
#endif /* CONFIG_TENDA_PRIVATE_WLAN */

    if ((PHY_GETVAR_SLICE(pi, "hwaci_sw_mitigation")) != NULL) {
        pi_ac->sromi->hwaci_sw_mitigation =
            (int8)PHY_GETINTVAR_SLICE(pi, "hwaci_sw_mitigation");
    } else {
        pi_ac->sromi->hwaci_sw_mitigation = 0;
    }

    pi->sromi->lpflags = (uint32)PHY_GETINTVAR_DEFAULT(pi, rstr_lpflags, 0);
    pi->sromi->subband5Gver =
        (uint8)PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_subband5gver, PHY_SUBBAND_4BAND);
    pi->sromi->dacdiv10_2g = (uint8) PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_dacdiv10_2g, 0);
    pi->sromi->dssf_dis_ch138 = (uint8)PHY_GETINTVAR_DEFAULT_SLICE(pi, rstr_dssf_dis_ch138, 1);
}

/* ********************************************* */
/*                External Definitions                    */
/* ********************************************* */

void
phy_ac_chanmgr_write_rx_farrow_tiny(phy_info_t *pi, chanspec_t chanspec,
                             chanspec_t chanspec_sc, int sc_mode)
{
    uint8 ch, num, den, bw, M, vco_div, core;
    uint8 chans[NUM_CHANS_IN_CHAN_BONDING];
    uint32 fcw, fcw1, tmp_low = 0, tmp_high = 0;
    uint32 fc, fc1;
    chanspec_t chanspec_sel = chanspec;
    bool vco_12GHz = pi->u.pi_acphy->chanmgri->vco_12GHz;
    if (sc_mode == 1) {
        chanspec_sel = chanspec_sc;
    }
    bw = CHSPEC_IS20(chanspec_sel) ? PHYBW_20: CHSPEC_IS40(chanspec_sel) ? PHYBW_40 :
        (CHSPEC_IS80(chanspec_sel) || PHY_AS_80P80(pi, chanspec_sel)) ? PHYBW_80 :
        PHYBW_160;

    if (CHSPEC_IS2G(chanspec_sel)) {
        if (!vco_12GHz) {
            num = 3;
            den = 2;
        } else {
            num = 4;
            den = 1;
        }
    } else {
        if (!vco_12GHz) {
            num = 2;
            den = 3;
        } else {
            num = 2;
            den = 1;
        }
    }

    if (vco_12GHz) {
        if ((pi->u.pi_acphy->chanmgri->data->fast_adc_en) ||
            (ACMAJORREV_4(pi->pubpi->phy_rev) && CHSPEC_IS8080(chanspec))) {
            M = SIPO_DIV_FAST * PHYBW_80 / bw;
            vco_div = AFE_DIV_FAST * ADC_DIV_FAST;
        } else {
            M = SIPO_DIV_SLOW;
            vco_div = AFE_DIV_BW(bw) * ADC_DIV_SLOW;
        }
    } else {
        if (CHSPEC_IS20(chanspec_sel)) {
            M = SIPO_DIV_SLOW;
            vco_div = 6;
        } else if (CHSPEC_IS40(chanspec_sel)) {
            M = SIPO_DIV_SLOW;
            vco_div = 3;
        } else if (CHSPEC_IS80(chanspec_sel) ||
                PHY_AS_80P80(pi, chanspec_sel)) {
            M = SIPO_DIV_FAST;
            vco_div = 1;
        } else if (CHSPEC_IS160(chanspec_sel)) {
            M = SIPO_DIV_FAST;
            vco_div = 1;
            ASSERT(0); // FIXME
        } else {
            M = SIPO_DIV_FAST;
            vco_div = 1;
        }
    }

    if (RADIOID_IS(pi->pubpi->radioid, BCM20693_ID)) {
        const uint8 afeclkdiv_arr[] = {2, 16, 4, 8, 3, 24, 6, 12};
        const uint8 adcclkdiv_arr[] = {1, 2, 3, 6};
        const uint8 sipodiv_arr[] = {12, 8};
        const chan_info_radio20693_altclkplan_t *altclkpln = altclkpln_radio20693;
        int row;
        if (ROUTER_4349(pi)) {
            altclkpln = altclkpln_radio20693_router4349;
        }
        row = wlc_phy_radio20693_altclkpln_get_chan_row(pi);
        if ((row >= 0) && (pi->u.pi_acphy->chanmgri->data->fast_adc_en == 0)) {
            num = CHSPEC_IS2G(pi->radio_chanspec) ? 4 : 2;
            M = sipodiv_arr[altclkpln[row].sipodiv];
            den = 1;
            vco_div = afeclkdiv_arr[altclkpln[row].afeclkdiv] *
                adcclkdiv_arr[altclkpln[row].adcclkdiv];
        }
    }
    /* bits_in_mu = 24 */
    /*
    fcw = (num * phy_utils_channel2freq(ch) * (((uint32)(1<<31))/
        (den * vco_div * 2 * M * bw)))>> 7;
    */
    if (CHSPEC_IS8080(chanspec) && !(ACMAJORREV_33(pi->pubpi->phy_rev))) {
        FOREACH_CORE(pi, core) {
            if (core == 0) {
                ch = wf_chspec_primary80_channel(chanspec);
                fc = wf_channel2mhz(ch, WF_CHAN_FACTOR_5_G);

                math_uint64_multiple_add(&tmp_high, &tmp_low, fc * num, 1 << 24, 0);
                math_uint64_divide(&fcw, tmp_high, tmp_low,
                    (uint32) (den * vco_div * 2 * M * bw));

                PHY_INFORM(("%s: fcw 0x%0x ch %d freq %d vco_div %d bw %d\n",
                    __FUNCTION__, fcw, ch, phy_utils_channel2freq(ch),
                    vco_div, bw));

                MOD_PHYREG(pi, RxSdFeConfig20, fcw_value_lo, fcw & 0xffff);
                MOD_PHYREG(pi, RxSdFeConfig30, fcw_value_hi,
                    (fcw >> 16) & 0xffff);
                MOD_PHYREG(pi, RxSdFeConfig30, fast_ADC_en,
                    (pi->u.pi_acphy->chanmgri->data->fast_adc_en & 0x1));
            } else if (core == 1) {
                ch = wf_chspec_secondary80_channel(chanspec);
                fc = wf_channel2mhz(ch, WF_CHAN_FACTOR_5_G);

                math_uint64_multiple_add(&tmp_high, &tmp_low, fc * num, 1 << 24, 0);
                math_uint64_divide(&fcw, tmp_high, tmp_low,
                    (uint32) (den * vco_div * 2 * M * bw));

                PHY_INFORM(("%s: fcw 0x%0x ch %d freq %d vco_div %d bw %d\n",
                    __FUNCTION__, fcw, ch, phy_utils_channel2freq(ch),
                    vco_div, bw));

                MOD_PHYREG(pi, RxSdFeConfig21, fcw_value_lo, fcw & 0xffff);
                MOD_PHYREG(pi, RxSdFeConfig31, fcw_value_hi,
                    (fcw >> 16) & 0xffff);
                MOD_PHYREG(pi, RxSdFeConfig31, fast_ADC_en,
                    (pi->u.pi_acphy->chanmgri->data->fast_adc_en & 0x1));
            }
        }
    } else {
        //ch = CHSPEC_CHANNEL(chanspec);
        //fc = wf_channel2mhz(ch, CHSPEC_IS2G(pi->radio_chanspec) ? WF_CHAN_FACTOR_2_4_G
        //: WF_CHAN_FACTOR_5_G);
        if (ACMAJORREV_33(pi->pubpi->phy_rev) &&
                (PHY_AS_80P80(pi, chanspec_sel))) {
            wf_chspec_get_80p80_channels(chanspec, chans);
            fc  = wf_channel2mhz(chans[0], WF_CHAN_FACTOR_5_G);
            fc1 = wf_channel2mhz(chans[1], WF_CHAN_FACTOR_5_G);

            math_uint64_multiple_add(&tmp_high, &tmp_low, fc * num, 1 << 24, 0);
            math_uint64_divide(&fcw, tmp_high, tmp_low,
                    (uint32) (den * vco_div * 2 * M * bw));

            math_uint64_multiple_add(&tmp_high, &tmp_low, fc1 * num, 1 << 24, 0);
            math_uint64_divide(&fcw1, tmp_high, tmp_low,
                    (uint32) (den * vco_div * 2 * M * bw));

            PHY_INFORM(("%s: fcw0 0x%0x ch0 %d fc %d freq0 %d vco_div %d bw %d\n",
                __FUNCTION__, fcw, chans[0], fc,
                phy_utils_channel2freq((uint)chans[0]), vco_div, bw));
            PHY_INFORM(("%s: fcw1 0x%0x ch1 %d fc %d freq1 %d vco_div %d bw %d\n",
                __FUNCTION__, fcw1, chans[1], fc1,
                phy_utils_channel2freq((uint)chans[1]), vco_div, bw));
        } else {
            ch = CHSPEC_CHANNEL(chanspec_sel);
            fc = wf_channel2mhz(ch, CHSPEC_IS2G(chanspec_sel) ? WF_CHAN_FACTOR_2_4_G
                : WF_CHAN_FACTOR_5_G);

            math_uint64_multiple_add(&tmp_high, &tmp_low, fc * num, 1 << 24, 0);
            math_uint64_divide(&fcw, tmp_high, tmp_low,
                (uint32) (den * vco_div * 2 * M * bw));
            fcw1 = fcw;
            PHY_INFORM(("%s: fcw 0x%0x ch %d freq %d vco_div %d bw %d\n",
                __FUNCTION__, fcw, ch, phy_utils_channel2freq(ch), vco_div, bw));
        }

        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev)) {
            if (sc_mode == 1) {
                if (CHSPEC_BW(chanspec) != CHSPEC_BW(chanspec_sc)) {
                    printf("NOT SUPPORT SC CORE BW != NORMAL CORE BW !!! \n");
                    ASSERT(0);
                }
                MOD_PHYREG(pi, RxSdFeConfig2_path3, fcw_value_lo, fcw & 0xffff);
                MOD_PHYREG(pi, RxSdFeConfig3_path3, fcw_value_hi,
                    (fcw >> 16) & 0xffff);
                MOD_PHYREG(pi, RxSdFeConfig3_path3, fast_ADC_en,
                    (pi->u.pi_acphy->chanmgri->data->fast_adc_en & 0x1));
            } else {
                MOD_PHYREG(pi, RxSdFeConfig2_path0, fcw_value_lo, fcw & 0xffff);
                MOD_PHYREG(pi, RxSdFeConfig2_path1, fcw_value_lo, fcw & 0xffff);
                MOD_PHYREG(pi, RxSdFeConfig2_path2, fcw_value_lo, fcw1 & 0xffff);
                MOD_PHYREG(pi, RxSdFeConfig2_path3, fcw_value_lo, fcw1 & 0xffff);
                MOD_PHYREG(pi, RxSdFeConfig3_path0, fcw_value_hi,
                    (fcw >> 16) & 0xffff);
                MOD_PHYREG(pi, RxSdFeConfig3_path1, fcw_value_hi,
                    (fcw >> 16) & 0xffff);
                MOD_PHYREG(pi, RxSdFeConfig3_path2, fcw_value_hi,
                    (fcw1 >> 16) & 0xffff);
                MOD_PHYREG(pi, RxSdFeConfig3_path3, fcw_value_hi,
                    (fcw1 >> 16) & 0xffff);
                MOD_PHYREG(pi, RxSdFeConfig3_path0, fast_ADC_en,
                    (pi->u.pi_acphy->chanmgri->data->fast_adc_en & 0x1));
                MOD_PHYREG(pi, RxSdFeConfig3_path1, fast_ADC_en,
                    (pi->u.pi_acphy->chanmgri->data->fast_adc_en & 0x1));
                MOD_PHYREG(pi, RxSdFeConfig3_path2, fast_ADC_en,
                    (pi->u.pi_acphy->chanmgri->data->fast_adc_en & 0x1));
                MOD_PHYREG(pi, RxSdFeConfig3_path3, fast_ADC_en,
                    (pi->u.pi_acphy->chanmgri->data->fast_adc_en & 0x1));
            }
        } else {
            MOD_PHYREG(pi, RxSdFeConfig2, fcw_value_lo, fcw & 0xffff);
            MOD_PHYREG(pi, RxSdFeConfig3, fcw_value_hi, (fcw >> 16) & 0xffff);
            MOD_PHYREG(pi, RxSdFeConfig3, fast_ADC_en,
                       (pi->u.pi_acphy->chanmgri->data->fast_adc_en & 0x1));
        }
    }
}

void
wlc_phy_farrow_setup_acphy(phy_info_t *pi, chanspec_t chanspec)
{
#ifdef ACPHY_1X1_ONLY
    uint32 dac_resamp_fcw;
    uint16 MuDelta_l, MuDelta_u;
    uint16 MuDeltaInit_l, MuDeltaInit_u;
#endif
    uint16 channel = CHSPEC_CHANNEL(chanspec);
    const uint16 *resamp_set = NULL;
    chan_info_tx_farrow *tx_farrow = NULL;
    chan_info_rx_farrow *rx_farrow;
    int bw_idx = 0;
    int tbl_idx = 0;

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));
    if (ISSIM_ENAB(pi->sh->sih)) {
        /* Use channel 7(2g)/151(5g) settings for Quickturn */
        if (CHSPEC_IS2G(chanspec)) {
            channel = 7;
        } else {
            channel = 155;
        }
    }

    /* China 40M Spur WAR */
    if (ACMAJORREV_0(pi->pubpi->phy_rev)) {
        uint8 core;
        /* Cleanup Overrides */
        MOD_PHYREG(pi, AfeClkDivOverrideCtrl, afediv_sel_div_ovr, 0);
        MOD_PHYREG(pi, AfeClkDivOverrideCtrl, afediv_sel_div, 0x0);
        pi->sdadc_config_override = FALSE;

        FOREACH_CORE(pi, core) {
            MOD_PHYREGCE(pi, RfctrlCoreAfeCfg2, core, afe_iqadc_flashhspd, 0);
            MOD_PHYREGCE(pi, RfctrlOverrideAfeCfg, core, afe_iqadc_flashhspd, 0);
            MOD_PHYREGCE(pi, RfctrlCoreAfeCfg2, core, afe_ctrl_flash17lvl, 0);
            MOD_PHYREGCE(pi, RfctrlOverrideAfeCfg, core, afe_ctrl_flash17lvl, 0);
            MOD_PHYREGCE(pi, RfctrlCoreAfeCfg2, core, afe_iqadc_mode, 0);
            MOD_PHYREGCE(pi, RfctrlOverrideAfeCfg, core, afe_iqadc_mode, 0);
        }
    }

#ifdef ACPHY_1X1_ONLY
    bw_idx = 0;
#else /* ACPHY_1X1_ONLY */
    bw_idx = CHSPEC_BW_LE20(chanspec)? 0 : (CHSPEC_IS40(chanspec)? 1 : 2);
#endif /* ACPHY_1X1_ONLY */
    if (ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
        /* Compute rx farrow setup */
        wlc_phy_write_rx_farrow_acphy(pi->u.pi_acphy->chanmgri, chanspec);
    } else {
        ASSERT(pi->u.pi_acphy->chanmgri->rx_farrow != NULL);
        /* Find the Rx Farrow settings in the table for the specific b/w and channel */
        for (tbl_idx = 0; tbl_idx < ACPHY_NUM_CHANS; tbl_idx++) {
            rx_farrow = &pi->u.pi_acphy->chanmgri->rx_farrow[bw_idx][tbl_idx];
            if (rx_farrow->chan == channel) {
                wlc_phy_write_rx_farrow_pre_tiny(pi, rx_farrow, chanspec);
                break;
            }
        }

        /*
         * No need to iterate through the Tx Farrow table, since the channels have the same
         * order as the Rx Farrow table.
         */

        if (tbl_idx == ACPHY_NUM_CHANS) {
            PHY_ERROR(("wl%d: %s: Failed to find Farrow settings"
                   " for bw=%d, channel=%d\n",
                   pi->sh->unit, __FUNCTION__, CHSPEC_BW(chanspec), channel));
            return;
        }
    }

    ASSERT(pi->u.pi_acphy->chanmgri->tx_farrow != NULL);
#ifdef ACPHY_1X1_ONLY
    ASSERT(phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->dac_mode == 1);
    tx_farrow = &pi->u.pi_acphy->chanmgri->tx_farrow[0][tbl_idx];
    dac_resamp_fcw = tx_farrow->dac_resamp_fcw;

    if (CHSPEC_IS80(chanspec))
    {
        dac_resamp_fcw += (dac_resamp_fcw >> 1);
    }

    dac_resamp_fcw = (dac_resamp_fcw + 32) >> 6;

    MuDelta_l = (dac_resamp_fcw & 0xFFFF);
    MuDelta_u = (dac_resamp_fcw & 0xFF0000) >> 16;
    MuDeltaInit_l = (dac_resamp_fcw & 0xFFFF);
    MuDeltaInit_u = (dac_resamp_fcw & 0xFF0000) >> 16;

    wlc_phy_tx_farrow_mu_setup(pi, MuDelta_l, MuDelta_u, MuDeltaInit_l, MuDeltaInit_u);
#else /* ACPHY_1X1_ONLY */
    if (ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
        /* Compute tx farrow setup */
        wlc_phy_write_tx_farrow_acphy(pi->u.pi_acphy->chanmgri, chanspec);
    } else {
        tx_farrow = &pi->u.pi_acphy->chanmgri->tx_farrow[bw_idx][tbl_idx];
        wlc_phy_tx_farrow_mu_setup(pi, tx_farrow->MuDelta_l, tx_farrow->MuDelta_u,
            tx_farrow->MuDeltaInit_l, tx_farrow->MuDeltaInit_u);
    }
#endif /* ACPHY_1X1_ONLY */

    /* China 40M Spur WAR */
    if (ACMAJORREV_0(pi->pubpi->phy_rev) &&
        (pi->afe_override) && CHSPEC_IS40(pi->radio_chanspec)) {
        uint16 fc;
        if (CHSPEC_CHANNEL(pi->radio_chanspec) > 14)
            fc = CHAN5G_FREQ(CHSPEC_CHANNEL(pi->radio_chanspec));
        else
            fc = CHAN2G_FREQ(CHSPEC_CHANNEL(pi->radio_chanspec));

        /* AFE Settings */
        if (fc == 5310) {
            uint8 core;
            MOD_PHYREG(pi, AfeClkDivOverrideCtrl, afediv_sel_div_ovr, 0x1);
            MOD_PHYREG(pi, AfeClkDivOverrideCtrl, afediv_sel_div, 0x0);

            FOREACH_CORE(pi, core) {
                MOD_PHYREGCE(pi, RfctrlCoreAfeCfg2, core, afe_iqadc_flashhspd, 1);
                MOD_PHYREGCE(pi, RfctrlOverrideAfeCfg, core,
                             afe_iqadc_flashhspd, 1);
                MOD_PHYREGCE(pi, RfctrlCoreAfeCfg2, core, afe_ctrl_flash17lvl, 0);
                MOD_PHYREGCE(pi, RfctrlOverrideAfeCfg, core,
                             afe_ctrl_flash17lvl, 1);
                MOD_PHYREGCE(pi, RfctrlCoreAfeCfg2, core, afe_iqadc_mode, 1);
                MOD_PHYREGCE(pi, RfctrlOverrideAfeCfg, core, afe_iqadc_mode, 1);
            }

            ACPHY_REG_LIST_START
                MOD_RADIO_REG_ENTRY(pi, RF0, AFEDIV1, afediv_main_driver_size, 8)
                MOD_RADIO_REG_ENTRY(pi, RF0, AFEDIV2, afediv_repeater1_dsize, 8)
                MOD_RADIO_REG_ENTRY(pi, RF0, AFEDIV2, afediv_repeater2_dsize, 8)
            ACPHY_REG_LIST_EXECUTE(pi);

            /* Set Override variable to pick up correct settings during cals */
            pi->sdadc_config_override = TRUE;
        } else if (fc == 5270) {
            MOD_PHYREG(pi, AfeClkDivOverrideCtrl, afediv_sel_div_ovr, 0x1);
            MOD_PHYREG(pi, AfeClkDivOverrideCtrl, afediv_sel_div, 0x2);
        }

        /* Resampler Settings */
        if (fc == 5270)
            resamp_set = resamp_cnwar_5270;
        else if (fc == 5310)
            resamp_set = resamp_cnwar_5310;

        if (resamp_set != NULL) {
            WRITE_PHYREG(pi, rxFarrowDeltaPhase_lo, resamp_set[0]);
            WRITE_PHYREG(pi, rxFarrowDeltaPhase_hi, resamp_set[1]);
            WRITE_PHYREG(pi, rxFarrowDriftPeriod, resamp_set[2]);
            WRITE_PHYREG(pi, lbFarrowDeltaPhase_lo, resamp_set[3]);
            WRITE_PHYREG(pi, lbFarrowDeltaPhase_hi, resamp_set[4]);
            WRITE_PHYREG(pi, lbFarrowDriftPeriod, resamp_set[5]);
            ACPHYREG_BCAST(pi, TxResamplerMuDelta0l, resamp_set[6]);
            ACPHYREG_BCAST(pi, TxResamplerMuDelta0u, resamp_set[7]);
            ACPHYREG_BCAST(pi, TxResamplerMuDeltaInit0l, resamp_set[8]);
            ACPHYREG_BCAST(pi, TxResamplerMuDeltaInit0u, resamp_set[9]);
        }
    }

    if (ACMAJORREV_4(pi->pubpi->phy_rev))
        MOD_PHYREG(pi, AfeClkDivOverrideCtrl, afediv_sel_div_ovr, 0x1);
}

#define QT_2G_DEFAULT_28NM 2465
#define QT_5G_DEFAULT_28NM 5807
#define VELOCE_2G_DEFAULT_28NM 2472
#define VELOCE_5G_DEFAULT_28NM 5760

void
wlc_phy_tx_farrow_setup_28nm(phy_info_t *pi, uint16 dac_rate_mode, bool wbcal)
{
    uint16 bits_in_mu = 23;
    uint16 mu_extra_bits = 0;
    uint16 mu_extra_bits_mask = 0;
    uint16 fc;
    uint16 dac_div_idx = 0, bw_idx, bw;
    uint16 dac_div_ratio;
    // lut entries [2G20 2G40 2G80 2G160 5G20 5G40 5G80 5G160]
#ifdef REGULATORY_DEBUG
    uint8 p_afediv_lut[] = {48, 48, 32, 16, 96, 96, 64, 28};
    uint8 p_afediv_lut_6GL[] = {240, 120, 72, 32};
    uint8 p_afediv_lut_6GL_rev51[] = {192, 120, 72, 32};
    uint8 p_afediv_lut_6GU[] = {240, 120, 72, 36};
    uint8 p_afediv_lut_rev130[] = {48, 48, 32, 16, 96, 96, 64, 40};
    uint8 p_afediv_lut_6GL_rev130[] = {240, 120, 72, 40};
    uint8 p_afediv_lut_6GU_rev130[] = {240, 120, 72, 48};
    uint16 p_afediv_lut_mode2_rev130[] = {128, 64, 32, 16, 288, 144, 64, 32, 256, 128, 64, 32};
#else
    uint8 p_afediv_lut[] = {96, 48, 32, 16, 192, 96, 64, 28};
    uint8 p_afediv_lut_6GL[] = {240, 120, 72, 32};
    uint8 p_afediv_lut_6GL_rev51[] = {192, 120, 72, 32};
    uint8 p_afediv_lut_6GU[] = {240, 120, 72, 36};
    uint8 p_afediv_lut_rev130[] = {96, 48, 32, 16, 192, 96, 64, 40};
    uint8 p_afediv_lut_6GL_rev130[] = {240, 120, 72, 40};
    uint8 p_afediv_lut_6GU_rev130[] = {240, 120, 72, 48};
    uint16 p_afediv_lut_mode2_rev130[] = {128, 64, 32, 16, 288, 144, 64, 32, 256, 128, 64, 32};
#endif // REGULATORY_DEBUG
    uint32 dac_resamp_fcw, numerator_hi, numerator_lo;
    uint64 numerator;
    uint16 mu_delta_l, mu_delta_u;
    uint16 regval;
    uint32 resamp_fcw_msb = 0;

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));
    bw_idx = CHSPEC_BW_LE20(pi->radio_chanspec)? 0 :
        (CHSPEC_IS40(pi->radio_chanspec)? 1 : CHSPEC_IS80(pi->radio_chanspec)? 2 : 3);
    bw = 10*(1<<(bw_idx+1));

#ifndef REGULATORY_DEBUG
    /* Override DAC DIV for 2G to be 10 instead of 12 */
    if (pi->sromi->dacdiv10_2g) {
        if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
            p_afediv_lut[0] = 80;
        } else {
            PHY_ERROR(("%s: ERROR: No DAC DIV 10 support yet for ACMAJORREV: %d\n",
                __FUNCTION__, pi->pubpi->phy_rev));
            ASSERT(0);
        }
    }
#endif

    if (ISSIM_ENAB(pi->sh->sih)) {
        if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev)) {
            p_afediv_lut[7] = 32;    /* 5g 160mhz */
            fc = (CHSPEC_IS2G(pi->radio_chanspec)) ?
                VELOCE_2G_DEFAULT_28NM : VELOCE_5G_DEFAULT_28NM;
        } else {
            /* 6878 QT AFE CLK MODEL */
            fc = (CHSPEC_IS2G(pi->radio_chanspec)) ?
                QT_2G_DEFAULT_28NM : QT_5G_DEFAULT_28NM;
        }
    } else {
        switch (CHSPEC_BAND(pi->radio_chanspec)) {
        case WL_CHANSPEC_BAND_2G:
            fc = CHAN2G_FREQ(CHSPEC_CHANNEL(pi->radio_chanspec));
            break;
        case WL_CHANSPEC_BAND_5G:
            fc = CHAN5G_FREQ(CHSPEC_CHANNEL(pi->radio_chanspec));
            break;
        case WL_CHANSPEC_BAND_6G:
            fc = CHAN6G_FREQ(CHSPEC_CHANNEL(pi->radio_chanspec));
            break;
        default:
            fc = 0;
            ASSERT(FALSE);
        }
    }
    if (dac_rate_mode == 2) {
        if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            if (wbcal && fc > 5320) {
                p_afediv_lut_mode2_rev130[4] = 320; // 5G mid/high 20M
                p_afediv_lut_mode2_rev130[5] = 160; // 5G mid/high 40M
            }
            dac_div_idx = CHSPEC_IS2G(pi->radio_chanspec) ? bw_idx : bw_idx + 4;
            dac_div_ratio = p_afediv_lut_mode2_rev130[dac_div_idx];
        } else {
            dac_div_ratio = CHSPEC_IS2G(pi->radio_chanspec) ? 128 : 256;
            if (CHSPEC_IS40(pi->radio_chanspec)) dac_div_ratio >>= 1;
            else if (CHSPEC_IS80(pi->radio_chanspec)) dac_div_ratio >>= 2;
            else if (CHSPEC_IS160(pi->radio_chanspec)) dac_div_ratio >>= 3;
        }
    } else { /* dac_rate_mode = 1 */
        if (ACMAJORREV_47_51_129_131(pi->pubpi->phy_rev)) {
            if (fc >= 6535) {
                dac_div_idx = bw_idx;
                dac_div_ratio = p_afediv_lut_6GU[dac_div_idx];
            } else if (fc >= 5900) {
                dac_div_idx = bw_idx;
                if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
                    dac_div_ratio = p_afediv_lut_6GL_rev51[dac_div_idx];
                } else {
                    dac_div_ratio = p_afediv_lut_6GL[dac_div_idx];
                }
            } else {
                dac_div_idx = CHSPEC_IS2G(pi->radio_chanspec) ? bw_idx : bw_idx + 4;
                dac_div_ratio = p_afediv_lut[dac_div_idx];
            }
        } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            if (fc >= 6250) {
                dac_div_idx = bw_idx;
                dac_div_ratio = p_afediv_lut_6GU_rev130[dac_div_idx];
            } else if (fc >= 5855) {
                dac_div_idx = bw_idx;
                dac_div_ratio = p_afediv_lut_6GL_rev130[dac_div_idx];
            } else {
                if (wbcal) {
                    if (fc <= 5320) {
                        /* 5G low 80M uses dac_div = 9 */
                        p_afediv_lut_rev130[6] = 72;
                    } else {
                        /* 5G mid/high 80M uses dac_div = 10 */
                        p_afediv_lut_rev130[6] = 80;
                    }
                    p_afediv_lut_rev130[7] = 40; // 5G 160M uses dac_div = 5
                }
                dac_div_idx = CHSPEC_IS2G(pi->radio_chanspec) ? bw_idx : bw_idx + 4;
                dac_div_ratio = p_afediv_lut_rev130[dac_div_idx];
            }
        } else {
            dac_div_idx = CHSPEC_IS2G(pi->radio_chanspec) ? bw_idx : bw_idx + 4;
            dac_div_ratio = p_afediv_lut[dac_div_idx];
        }
    }

    /* Assign more extra bits for DAC Resampler ratio */
    if (ACMAJORREV_51(pi->pubpi->phy_rev) || ACMAJORREV_128(pi->pubpi->phy_rev)) {
        mu_extra_bits = 3;
    } else if (ACMAJORREV_GE129(pi->pubpi->phy_rev)) {
        mu_extra_bits = 8;
    }
    bits_in_mu += mu_extra_bits;
    mu_extra_bits_mask = (1 << mu_extra_bits) - 1;

    /* Extra 1 bit left shift for rounding */
    numerator = ((uint64)bw << (bits_in_mu + 1)) * dac_div_ratio;
    numerator_hi = (uint32)(numerator >> 32);
    numerator_lo = (uint32)(numerator & 0xFFFFFFFF);

    /* for 6715 and future chips, where we have high dac clk, dac_resamp_fcw before
     * rounding can go beyond 32 bits, hence need some special handing
     */
    if (ACMAJORREV_130(pi->pubpi->phy_rev) && !ISSIM_ENAB(pi->sh->sih)) {
        if (numerator >= ((uint64)fc << 32)) {
            resamp_fcw_msb = 1;
            numerator -= (uint64)fc << 32;
        }
    }
    math_uint64_divide(&dac_resamp_fcw, numerator_hi, numerator_lo, (uint32)fc);

    /* Add 1 and right shift by 1 to round */
    if (ACMAJORREV_130(pi->pubpi->phy_rev) && ISSIM_ENAB(pi->sh->sih) &&
        CHSPEC_IS160(pi->radio_chanspec)) {
        /* special handling for 160MHz Veloce clk 1288MHz */
        dac_resamp_fcw = 2134145240;
    } else {
        dac_resamp_fcw = (dac_resamp_fcw + 1) >> 1;
    }
    dac_resamp_fcw += (resamp_fcw_msb << 31);

    /* extra LSB are stripped and appended in front of mu_delta_u */
    mu_delta_l = (uint16)((dac_resamp_fcw >> mu_extra_bits) & 0xFFFF);
    mu_delta_u = (uint16)(((dac_resamp_fcw >> (16 + mu_extra_bits)) & 0xFF) |
            ((dac_resamp_fcw & mu_extra_bits_mask) << 8));

    ACPHYREG_BCAST(pi, TxResamplerMuDelta0l, mu_delta_l);
    ACPHYREG_BCAST(pi, TxResamplerMuDelta0u, mu_delta_u);
    ACPHYREG_BCAST(pi, TxResamplerMuDeltaInit0l, mu_delta_l);
    ACPHYREG_BCAST(pi, TxResamplerMuDeltaInit0u, mu_delta_u);

    PHY_INFORM(("wl%d %s: band=%d fc=%d fcw=0x%x%x%x%x\n", PI_INSTANCE(pi),
        __FUNCTION__, (CHSPEC_IS2G(pi->radio_chanspec))?2:5, fc,
        (dac_resamp_fcw >> 24) & 0xFF, (dac_resamp_fcw >> 16) & 0xFF,
        (dac_resamp_fcw >> 8) & 0xFF, dac_resamp_fcw & 0xFF));
    if (pi->u.pi_acphy->sromi->srom_low_adc_rate_en &&
        ACMAJORREV_GE40(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, sdfeClkGatingCtrl, disableRxStallonTx, 0x1);
    }
    /* Enable the Tx resampler on all cores */
    regval = READ_PHYREG(pi, TxResamplerEnable0);
    regval |= (1 << ACPHY_TxResamplerEnable0_enable_tx_SHIFT(pi->pubpi->phy_rev));
    ACPHYREG_BCAST(pi, TxResamplerEnable0,  regval);
}

void
wlc_phy_rx_farrow_setup_28nm(phy_info_t *pi, chanspec_t chanspec_sc, uint8 sc_mode)
{
    uint16 fc, afe_div_ratio, fcw_hi, fcw_lo;
    uint16 drift_period, bw_idx, bw;
    uint16 afe_div_idx;
    uint32 fcw;
    uint32 numerator_hi, numerator_lo;
    uint64 numerator;
    uint8 adc_rate_mode;
    uint8 p_afediv_lut[] = {96, 48, 28, 14, 192, 96, 64, 32};
    uint8 p_afediv_lut_rev130_2G[] = {108, 48, 28, 14, 216, 96, 64, 32};
    uint8 p_afediv_lut_special[] = {96, 48, 28, 14, 192, 96, 64, 34};
    uint8 p_afediv_lut_6GL[] = {216, 108, 72, 36};
    uint8 p_afediv_lut_6GL_special[] = {216, 108, 72, 34};
    uint8 p_afediv_lut_6GU[] = {240, 120, 80, 40};
    uint8 p_afediv_lut_6GUU[] = {240, 120, 80, 42};
    uint16 p_afediv_lut_bm[] = {112, 56, 96, 224, 112, 192};
    uint16 p_afediv_lut_bm_5G[] = {256, 128, 192};
    uint16 p_afediv_lut_bm_6GL[] = {288, 144, 216};
    uint16 p_afediv_lut_bm_6GU[] = {320, 160, 240};
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    if ((ACMAJORREV_130(pi->pubpi->phy_rev)) && (PHY_COREMASK_SISO(stf_shdata->phyrxchain))) {
        p_afediv_lut_rev130_2G[0] = 96;
        p_afediv_lut_rev130_2G[4] = 192;
    }

    if (sc_mode == 0) {
        bw_idx = CHSPEC_BW_LE20(pi->radio_chanspec) ? 0 :
            CHSPEC_IS40(pi->radio_chanspec) ? 1 :
            CHSPEC_IS80(pi->radio_chanspec) ? 2 : 3;
        bw = 10*(1<<(bw_idx+1));

        if (ISSIM_ENAB(pi->sh->sih)) {
            if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                !ACMAJORREV_128(pi->pubpi->phy_rev)) {
                fc = (CHSPEC_IS2G(pi->radio_chanspec)) ?
                VELOCE_2G_DEFAULT_28NM : VELOCE_5G_DEFAULT_28NM;
            } else {
                /* 6878 QT AFE CLK MODEL */
                fc = (CHSPEC_IS2G(pi->radio_chanspec)) ?
                QT_2G_DEFAULT_28NM : QT_5G_DEFAULT_28NM;
            }
        } else {
            switch (CHSPEC_BAND(pi->radio_chanspec)) {
            case WL_CHANSPEC_BAND_2G:
                fc = CHAN2G_FREQ(CHSPEC_CHANNEL(pi->radio_chanspec));
                break;
            case WL_CHANSPEC_BAND_5G:
                fc = CHAN5G_FREQ(CHSPEC_CHANNEL(pi->radio_chanspec));
                break;
            case WL_CHANSPEC_BAND_6G:
                fc = CHAN6G_FREQ(CHSPEC_CHANNEL(pi->radio_chanspec));
                break;
            default:
                fc = 0;
                ASSERT(FALSE);
            }
        }
    } else {
        bw_idx = CHSPEC_BW_LE20(chanspec_sc)? 0 : CHSPEC_IS40(chanspec_sc)? 1 :
        CHSPEC_IS80(chanspec_sc)? 2 : 3;

        bw = 10*(1<<(bw_idx+1));
        fc = wf_channel2mhz(CHSPEC_CHANNEL(chanspec_sc), CHSPEC_IS2G(chanspec_sc) ?
            WF_CHAN_FACTOR_2_4_G : WF_CHAN_FACTOR_5_G);
    }

    if (ACMAJORREV_47_51_129_131(pi->pubpi->phy_rev)) {
        if (fc >= 6535) {
            afe_div_idx = bw_idx;
            afe_div_ratio = p_afediv_lut_6GU[afe_div_idx];
        } else if (fc == 6025) {
            afe_div_idx = bw_idx;
            afe_div_ratio = p_afediv_lut_6GL_special[afe_div_idx];
        } else if (fc >= 5900) {
            afe_div_idx = bw_idx;
            afe_div_ratio = p_afediv_lut_6GL[afe_div_idx];
        } else if (fc == 5815 && ACMAJORREV_131(pi->pubpi->phy_rev)) {
            afe_div_idx = bw_idx + 4;
            afe_div_ratio = p_afediv_lut_special[afe_div_idx];
        } else {
            if (sc_mode == 0) {
                afe_div_idx = CHSPEC_IS2G(pi->radio_chanspec) ? bw_idx : bw_idx + 4;
            } else {
                afe_div_idx = CHSPEC_IS2G(chanspec_sc) ? bw_idx : bw_idx + 4;
            }
            afe_div_ratio = p_afediv_lut[afe_div_idx];
        }
    } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        if (fc >= 6985) {
            afe_div_idx = bw_idx;
            afe_div_ratio = p_afediv_lut_6GUU[afe_div_idx];
        } else if (fc >= 6535) {
            afe_div_idx = bw_idx;
            afe_div_ratio = p_afediv_lut_6GU[afe_div_idx];
        } else if (fc >= 5855) {
            afe_div_idx = bw_idx;
            afe_div_ratio = p_afediv_lut_6GL[afe_div_idx];
            if (fc == 6025 && bw_idx == 3) {
                afe_div_ratio = 34;
            }
        } else {
            if (sc_mode == 0) {
                afe_div_idx = CHSPEC_IS2G(pi->radio_chanspec) ? bw_idx : bw_idx + 4;
            } else {
                afe_div_idx = CHSPEC_IS2G(chanspec_sc) ? bw_idx : bw_idx + 4;
            }
            if (fc == 5815 && bw_idx == 3) {
                afe_div_ratio = 34;
            } else {
                if (fc < 3000) {
                    afe_div_ratio = p_afediv_lut_rev130_2G[afe_div_idx];
                } else {
                    afe_div_ratio = p_afediv_lut[afe_div_idx];
                }
            }
        }
        adc_rate_mode = READ_PHYREG(pi, AdcboostCtrl) & 0x3;
        if (adc_rate_mode > 1) {
            if (adc_rate_mode == 3) {
                bw_idx = 2;
            }
            if (fc >= 6535) {
                afe_div_idx = bw_idx;
                afe_div_ratio = p_afediv_lut_bm_6GU[afe_div_idx];
            } else if (fc >= 5855) {
                afe_div_idx = bw_idx;
                afe_div_ratio = p_afediv_lut_bm_6GL[afe_div_idx];
            } else if (fc >= 5180) {
                afe_div_idx = bw_idx;
                afe_div_ratio = p_afediv_lut_bm_5G[afe_div_idx];
            } else {
                if (sc_mode == 0) {
                    afe_div_idx = CHSPEC_IS2G(pi->radio_chanspec) ?
                        bw_idx : bw_idx + 3;
                } else {
                    afe_div_idx = CHSPEC_IS2G(chanspec_sc) ?
                        bw_idx : bw_idx + 3;
                }
                afe_div_ratio = p_afediv_lut_bm[afe_div_idx];
            }
        }
    } else {
        if (sc_mode == 0) {
            afe_div_idx = CHSPEC_IS2G(pi->radio_chanspec) ? bw_idx : bw_idx + 4;
        } else {
            afe_div_idx = CHSPEC_IS2G(chanspec_sc) ? bw_idx : bw_idx + 4;
        }
        afe_div_ratio = p_afediv_lut[afe_div_idx];
    }

    drift_period = afe_div_ratio * bw;
    /* Extra 1 bit left shift to help in rounding */
    numerator = (uint64)(fc) << (N_BITS_RX_FARR + 1);
    numerator_hi = (uint32)(numerator >> 32);
    numerator_lo = (uint32)(numerator & 0xFFFFFFFF);
    math_uint64_divide(&fcw, numerator_hi, numerator_lo, (uint32)drift_period);

    /* Extra 1 bit left shift to help in rounding */
    fcw -= (1 << (N_BITS_RX_FARR + 1));

    /* Add 1 and right shift by 1 for rounding */
    if (ACMAJORREV_130(pi->pubpi->phy_rev) && ISSIM_ENAB(pi->sh->sih) &&
        CHSPEC_IS160(pi->radio_chanspec)) {
        /* special handelling for 160MHz Veloce clk 1288MHz */
        fcw = 104858;
    } else {
        fcw = ((fcw + 1) >> 1);
    }

    fcw_lo = (uint16)(fcw & 0xFFFF);
    fcw_hi = (uint16)((fcw >> 16) & 0xFF);

    if (sc_mode == 0) {
        ACPHYREG_BCAST(pi, rxFarrowDeltaPhase_lo, fcw_lo);
        ACPHYREG_BCAST(pi, rxFarrowDeltaPhase_hi, fcw_hi);
        WRITE_PHYREG(pi, rxFarrowDriftPeriod, drift_period);
        ACPHYREG_BCAST(pi, rxFarrowCtrl, 0xb3);

        PHY_INFORM(("wl%d %s: band=%d fc=%d fcw=0x%x%x\n", PI_INSTANCE(pi), __FUNCTION__,
            (CHSPEC_IS2G(pi->radio_chanspec))?2:5, fc, fcw_hi, fcw_lo));

        ACPHYREG_BCAST(pi, lbFarrowDeltaPhase_lo, fcw_lo);
        ACPHYREG_BCAST(pi, lbFarrowDeltaPhase_hi, fcw_hi);
        WRITE_PHYREG(pi, lbFarrowDriftPeriod, drift_period);
        ACPHYREG_BCAST(pi, lbFarrowCtrl, 0xb3);
    } else {
        phy_utils_write_phyreg_p1c(pi, ACPHY_rxFarrowDeltaPhase_lo(pi->pubpi->phy_rev),
            fcw_lo);
        phy_utils_write_phyreg_p1c(pi, ACPHY_rxFarrowDeltaPhase_hi(pi->pubpi->phy_rev),
            fcw_hi);
        phy_utils_write_phyreg_p1c(pi, ACPHY_rxFarrowDriftPeriod(pi->pubpi->phy_rev),
            drift_period);
        phy_utils_write_phyreg_p1c(pi, ACPHY_rxFarrowCtrl(pi->pubpi->phy_rev), 0xb3);

        PHY_INFORM(("wl%d %s: band=%d fc=%d fcw=0x%x%x\n", PI_INSTANCE(pi), __FUNCTION__,
            (CHSPEC_IS2G(chanspec_sc))?2:5, fc, fcw_hi, fcw_lo));

        phy_utils_write_phyreg_p1c(pi, ACPHY_lbFarrowDeltaPhase_lo(pi->pubpi->phy_rev),
            fcw_lo);
        phy_utils_write_phyreg_p1c(pi, ACPHY_lbFarrowDeltaPhase_hi(pi->pubpi->phy_rev),
            fcw_hi);
        phy_utils_write_phyreg_p1c(pi, ACPHY_lbFarrowDriftPeriod(pi->pubpi->phy_rev),
            drift_period);
        phy_utils_write_phyreg_p1c(pi, ACPHY_lbFarrowCtrl(pi->pubpi->phy_rev), 0xb3);
    }
}

void
wlc_phy_farrow_setup_28nm(phy_info_t *pi, uint16 dac_rate_mode)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));
    wlc_phy_rx_farrow_setup_28nm(pi, 0, 0);
    wlc_phy_tx_farrow_setup_28nm(pi, dac_rate_mode, 0);

    if (ISSIM_ENAB(pi->sh->sih) && (!pi_ac->chanmgri->veloce_farrow_db)) {
        /* Disable Clock Stalling */
        ACPHY_ENABLE_STALL(pi, 1);
    } else {
        /* Enable Clock Stalling */
        ACPHY_ENABLE_STALL(pi, 0);
    }
}

void
wlc_phy_set_lowpwr_phy_reg_rev3(phy_info_t *pi)
{
    ACPHY_REG_LIST_START
        MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet1, vco_en_alc, 0x0)
        MOD_PHYREG_ENTRY(pi, radio_rxrf_lna5g, lna5g_lna1_bias_idac, 0x8)
        MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet4, vco_tempco_dcadj_1p2, 0x9)
        MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet2, vco_vctrl_buf_ical, 0x3)
        MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet4, vco_ib_bias_opamp, 0x6)
        MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet4, vco_ib_bias_opamp_fastsettle, 0xf)
        MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet1, vco_bypass_vctrl_buf, 0x0)
        MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet3, vco_HDRM_CAL, 0x2)
        MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet2, vco_ICAL, 0x16)
        MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet3, vco_ICAL_1p2, 0xc)
        MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet1, vco_USE_2p5V, 0x1)
    ACPHY_REG_LIST_EXECUTE(pi);
    if (ACMAJORREV_2(pi->pubpi->phy_rev) && ACMINORREV_1(pi)) {
        MOD_PHYREG(pi, radio_logen2gN5g, idac_mix, 0x4);
    }
}

void
wlc_phy_set_lowpwr_phy_reg(phy_info_t *pi)
{
    /* These guys not required for tiny based phys */
    if (!TINY_RADIO(pi)) {
        ACPHY_REG_LIST_START
            MOD_PHYREG_ENTRY(pi, radio_logen2g, idac_gm, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_logen2g, idac_gm_2nd, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_logen2g, idac_qb, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_logen2g, idac_qb_2nd, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_logen2g, idac_qtx, 0x4)
            MOD_PHYREG_ENTRY(pi, radio_logen2gN5g, idac_itx, 0x4)
            MOD_PHYREG_ENTRY(pi, radio_logen2gN5g, idac_qrx, 0x4)
            MOD_PHYREG_ENTRY(pi, radio_logen2gN5g, idac_irx, 0x4)
            MOD_PHYREG_ENTRY(pi, radio_logen2gN5g, idac_buf, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_logen2gN5g, idac_mix, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_logen5g, idac_div, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_logen5g, idac_vcob, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_logen5gbufs, idac_bufb, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_logen5g, idac_mixb, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_logen5g, idac_load, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_logen5gbufs, idac_buf2, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_logen5gbufs, idac_bufb2, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_logen5gbufs, idac_buf1, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_logen5gbufs, idac_bufb1, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_logen5gQI, idac_qtx, 0x4)
            MOD_PHYREG_ENTRY(pi, radio_logen5gQI, idac_itx, 0x4)
            MOD_PHYREG_ENTRY(pi, radio_logen5gQI, idac_qrx, 0x4)
            MOD_PHYREG_ENTRY(pi, radio_logen5gQI, idac_irx, 0x4)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcocal, vcocal_rstn, 0x1)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcocal, vcocal_force_caps, 0x0)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcocal, vcocal_force_caps_val, 0x40)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet1, vco_ALC_ref_ctrl, 0xd)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet1, vco_bias_mode, 0x1)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet1, vco_cvar_extra, 0xb)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet1, vco_cvar, 0xf)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet1, vco_en_alc, 0x0)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet2, vco_tempco_dcadj, 0xe)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet2, vco_tempco, 0xb)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet3, vco_cal_en, 0x1)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet3, vco_cal_en_empco, 0x1)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet3, vco_cap_mode, 0x0)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet4, vco_ib_ctrl, 0x0)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet3, vco_por, 0x0)
            MOD_PHYREG_ENTRY(pi, radio_pll_lf_r1, lf_r1, 0x0)
            MOD_PHYREG_ENTRY(pi, radio_pll_lf_r2r3, lf_r2, 0xc)
            MOD_PHYREG_ENTRY(pi, radio_pll_lf_r2r3, lf_r3, 0xc)
            MOD_PHYREG_ENTRY(pi, radio_pll_lf_cm, lf_rs_cm, 0xff)
            MOD_PHYREG_ENTRY(pi, radio_pll_lf_cm, lf_rf_cm, 0xc)
            MOD_PHYREG_ENTRY(pi, radio_pll_lf_cSet1, lf_c1, 0x99)
            MOD_PHYREG_ENTRY(pi, radio_pll_lf_cSet1, lf_c2, 0x8b)
            MOD_PHYREG_ENTRY(pi, radio_pll_lf_cSet2, lf_c3, 0x8b)
            MOD_PHYREG_ENTRY(pi, radio_pll_lf_cSet2, lf_c4, 0x8f)
            MOD_PHYREG_ENTRY(pi, radio_pll_cp, cp_kpd_scale, 0x34)
            MOD_PHYREG_ENTRY(pi, radio_pll_cp, cp_ioff, 0x60)
            MOD_PHYREG_ENTRY(pi, radio_ldo, ldo_1p2_xtalldo1p2_lowquiescenten, 0x0)
            MOD_PHYREG_ENTRY(pi, radio_ldo, ldo_2p5_lowpwren_VCO, 0x0)
            MOD_PHYREG_ENTRY(pi, radio_ldo, ldo_2p5_lowquiescenten_VCO_aux, 0x0)
            MOD_PHYREG_ENTRY(pi, radio_ldo, ldo_2p5_lowpwren_VCO_aux, 0x0)
            MOD_PHYREG_ENTRY(pi, radio_ldo, ldo_2p5_lowquiescenten_CP, 0x0)
            MOD_PHYREG_ENTRY(pi, radio_ldo, ldo_2p5_lowquiescenten_VCO, 0x0)
            MOD_PHYREG_ENTRY(pi, radio_rxrf_lna2g, lna2g_lna1_bias_idac, 0x2)
            MOD_PHYREG_ENTRY(pi, radio_rxrf_lna2g, lna2g_lna2_aux_bias_idac, 0x8)
            MOD_PHYREG_ENTRY(pi, radio_rxrf_lna2g, lna2g_lna2_main_bias_idac, 0x8)
            MOD_PHYREG_ENTRY(pi, radio_rxrf_lna5g, lna5g_lna1_bias_idac, 0x8)
            MOD_PHYREG_ENTRY(pi, radio_rxrf_lna5g, lna5g_lna2_aux_bias_idac, 0x7)
            MOD_PHYREG_ENTRY(pi, radio_rxrf_lna5g, lna5g_lna2_main_bias_idac, 0x4)
            MOD_PHYREG_ENTRY(pi, radio_rxrf_rxmix, rxmix2g_aux_bias_idac, 0x8)
            MOD_PHYREG_ENTRY(pi, radio_rxrf_rxmix, rxmix2g_main_bias_idac, 0x8)
            MOD_PHYREG_ENTRY(pi, radio_rxrf_rxmix, rxmix5g_gm_aux_bias_idac_i, 0x8)
            MOD_PHYREG_ENTRY(pi, radio_rxrf_rxmix, rxmix5g_gm_main_bias_idac_i, 0x8)
            MOD_PHYREG_ENTRY(pi, radio_rxbb_tia, tia_DC_Ib1, 0x6)
            MOD_PHYREG_ENTRY(pi, radio_rxbb_tia, tia_DC_Ib2, 0x6)
            MOD_PHYREG_ENTRY(pi, radio_rxbb_tia, tia_Ib_I, 0x6)
            MOD_PHYREG_ENTRY(pi, radio_rxbb_tia, tia_Ib_Q, 0x6)
            MOD_PHYREG_ENTRY(pi, radio_rxbb_bias12, lpf_bias_level1, 0x4)
            MOD_PHYREG_ENTRY(pi, radio_rxbb_bias12, lpf_bias_level2, 0x8)
            MOD_PHYREG_ENTRY(pi, radio_rxbb_bias34, lpf_bias_level3, 0x10)
            MOD_PHYREG_ENTRY(pi, radio_rxbb_bias34, lpf_bias_level4, 0x20)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet4, vco_tempco_dcadj_1p2, 0x9)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet2, vco_vctrl_buf_ical, 0x3)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet4, vco_ib_bias_opamp, 0x6)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet4, vco_ib_bias_opamp_fastsettle, 0xf)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet1, vco_bypass_vctrl_buf, 0x0)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet3, vco_HDRM_CAL, 0x2)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet2, vco_ICAL, 0x16)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet3, vco_ICAL_1p2, 0xc)
            MOD_PHYREG_ENTRY(pi, radio_pll_vcoSet1, vco_USE_2p5V, 0x1)
        ACPHY_REG_LIST_EXECUTE(pi);
    }
}

/** Tx implicit beamforming. Ingress and outgress channels are assumed to have reprocity. */
void
wlc_phy_populate_recipcoeffs_acphy(phy_info_t *pi)
{
    phy_ac_chanmgr_info_t *chanmgri = pi->u.pi_acphy->chanmgri;
    uint16 start_words[][3] = {
        {0x005B, 0x0000, 0x0000},
        {0x8250, 0x0000, 0x0000},
        {0xC338, 0x0000, 0x0000},
        {0x4527, 0x0001, 0x0000},
        {0xA6A1, 0x0001, 0x0000},
        {0x081B, 0x0002, 0x0000},
        {0x8A18, 0x0002, 0x0000},
        {0x2C96, 0x0003, 0x0000},
        {0x8E17, 0x0003, 0x0000},
        {0x101B, 0x0004, 0x0000},
        {0x0020, 0x0000, 0x0000},
        {0x0020, 0x0000, 0x0000}
    };

    uint16 packed_word[6] = {0, 0, 0, 0, 0, 0};
    uint16 zero_word[3] = {0, 0, 0};

    uint16 ang_tmp = 0, ang_tmp1 = 0;
    uint16 subband_idx, k, i;
    uint16 theta[2];
    uint32 packed;
    uint16 nwords_start = 12, nwords_pad = 4, nwords_recip, nbanks;
    uint8  stall_val;
    uint8 bands[NUM_CHANS_IN_CHAN_BONDING];

    bool is_caled = wlc_phy_is_txbfcal(pi);

    if (phy_stf_get_data(pi->stfi)->hw_phytxchain <= 1 || !(is_caled)) {
        return;
    }

    /* 1. obtain angles from SROM */
    if (PHY_AS_80P80(pi, pi->radio_chanspec)) {
        phy_ac_chanmgr_get_chan_freq_range_80p80(pi, 0, bands);
        subband_idx = bands[0];
    } else {
        subband_idx = phy_ac_chanmgr_get_chan_freq_range(pi, 0, PRIMARY_FREQ_SEGMENT);
    }

    switch (subband_idx) {
    case WL_CHAN_FREQ_RANGE_2G:
#ifdef WLTXBF_2G_DISABLED
        ang_tmp = 0; ang_tmp1 = 0;
#else
        ang_tmp = pi->u.pi_acphy->sromi->rpcal2g;
        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            ang_tmp1 = pi->sromi->rpcal2gcore3;
        }
        if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
            uint16 ambiguity_angle = wlc_phy_get_recip_LO_div_phase(pi);
            uint16 cal_ambiguity = pi->u.pi_acphy->sromi->rpcal_phase2g;
            if (ambiguity_angle != cal_ambiguity) {
                /* ambiguity angle either is 0 or 180 degrees for rev51/131
                ** When it differs from the cal ambiguity,
                ** 8th LSB of ang_tmp is inverted
                */
                ang_tmp ^= (0x80);
            }
        }
#endif /* WLTXBF_2G_DISABLED */
        break;
    case WL_CHAN_FREQ_RANGE_5G_BAND0:
        ang_tmp = pi->u.pi_acphy->sromi->rpcal5gb0;
        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            ang_tmp1 = pi->sromi->rpcal5gb0core3;
        }
        break;
    case WL_CHAN_FREQ_RANGE_5G_BAND1:
        ang_tmp = pi->u.pi_acphy->sromi->rpcal5gb1;
        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            ang_tmp1 = pi->sromi->rpcal5gb1core3;
        }
        break;
    case WL_CHAN_FREQ_RANGE_5G_BAND2:
        ang_tmp = pi->u.pi_acphy->sromi->rpcal5gb2;
        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            ang_tmp1 = pi->sromi->rpcal5gb2core3;
        }
        break;
    case WL_CHAN_FREQ_RANGE_5G_BAND3:
        ang_tmp = pi->u.pi_acphy->sromi->rpcal5gb3;
        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            ang_tmp1 = pi->sromi->rpcal5gb3core3;
        }
        break;
    default:
        ang_tmp = pi->u.pi_acphy->sromi->rpcal2g;
        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            ang_tmp1 = pi->sromi->rpcal2gcore3;
        }
        break;
    }

    if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        ang_tmp1 = ((ang_tmp >> 8) & 0xff) + ((ang_tmp1 & 0x00ff) << 8);
        ang_tmp  = (ang_tmp & 0xff) << 8;
    }

    /* 2. generate packed word */
    if (ACMAJORREV_0(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)) {

        math_cmplx_angle_to_phasor_lut(ang_tmp, packed_word);

        /* printf("reciprocity packed_word: %x%x%x\n",
        packed_word[2], packed_word[1], packed_word[0]);
        */

    } else if ((ACMAJORREV_2(pi->pubpi->phy_rev)) || (ACMAJORREV_4(pi->pubpi->phy_rev)) ||
        (ACMAJORREV_128(pi->pubpi->phy_rev))) {
        theta[0] = (uint8) (ang_tmp & 0xFF);
        theta[1] = (uint8) ((ang_tmp >> 8) & 0xFF);
        /* printf("---- theta1 = %d, theta2 = %d\n", theta[0], theta[1]); */

        /* every 4 tones are packed into 1 word */
        packed = (theta[0] | (theta[0] << 8) | (theta[0] << 16) | (theta[0] << 24));

        /* printf("reciprocity packedWideWord: %x\n", packed); */
    } else if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        if (!PHY_AS_80P80(pi, pi->radio_chanspec)) {
            math_cmplx_angle_to_phasor_lut(ang_tmp, &(packed_word[0]));
            math_cmplx_angle_to_phasor_lut(ang_tmp1, &(packed_word[3]));
        }
    }

    /* Disable stalls and hold FIFOs in reset */
    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    ACPHY_DISABLE_STALL(pi);

    /* 3. write to table */
    /* 4360 and 43602 */
    if (ACMAJORREV_0(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
        nwords_recip = 64 + 128 + 256;

        for (k = 0; k < nwords_start; k++) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_BFECONFIG,
            1, k, 48, start_words[k]);
        }

        for (k = 0; k < nwords_recip; k++) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_BFECONFIG,
            1, nwords_start + k, 48, packed_word);
        }

        for (k = 0; k < nwords_pad; k++) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_BFECONFIG,
            1, nwords_start + nwords_recip + k, 48, zero_word);
        }
    } else if ((ACMAJORREV_2(pi->pubpi->phy_rev)) || (ACMAJORREV_4(pi->pubpi->phy_rev)) ||
        (ACMAJORREV_128(pi->pubpi->phy_rev))) {
        /* 4 tones are packed into one word */
        nwords_recip = (256 >> 2);

        for (k = 0; k < nwords_recip; k++) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_BFECONFIG2X2TBL,
            1, k, 32, &packed);
        }
    } else if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        /* total 512 or 1024 words, and partitioned into 4 mem banks */
        nwords_recip = (ACMAJORREV_47(pi->pubpi->phy_rev) ||
                ACMAJORREV_51_131(pi->pubpi->phy_rev)) ? (1024 >> 2): (512 >> 2);

        for (k = 0; k < nwords_recip; k++) {
            if (k % 2 == 0 || ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
                chanmgri->recip_packed_majrev32_33_37[k][0] = packed_word[0];
                chanmgri->recip_packed_majrev32_33_37[k][1] = packed_word[1];
                chanmgri->recip_packed_majrev32_33_37[k][2] = packed_word[2];
            } else {
                chanmgri->recip_packed_majrev32_33_37[k][0] = packed_word[3];
                chanmgri->recip_packed_majrev32_33_37[k][1] = packed_word[4];
                chanmgri->recip_packed_majrev32_33_37[k][2] = packed_word[5];
            }
        }
        nbanks = (ACMAJORREV_51(pi->pubpi->phy_rev)) ? 1 :
            ((ACMAJORREV_131(pi->pubpi->phy_rev)) ? 2 : 4);
        for (i = 0; i < nbanks; i++) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_BFECONFIG,
            nwords_recip, i*nwords_recip, 48, chanmgri->recip_packed_majrev32_33_37);
        }
    }

    ACPHY_ENABLE_STALL(pi, stall_val);
}

/* get the two 80p80 complex freq. chanspec must be 80p80 or 160 */
void
phy_ac_chanmgr_get_chan_freq_range_80p80(phy_info_t *pi, chanspec_t chanspec, uint8 *freq)
{
    chanspec_t pri, sec;

    if (chanspec == 0) {
        chanspec = pi->radio_chanspec;
    }

    ASSERT(PHY_AS_80P80(pi, chanspec) || CHSPEC_IS160(chanspec));

    pri = wf_chspec_primary80_chspec(chanspec);
    sec = wf_chspec_secondary80_chspec(chanspec);

    PHY_TRACE(("wl%d: %s: chanspec1=0x%04x chanspec2=0x%04x\n", pi->sh->unit,
            __FUNCTION__, pri, sec));

    freq[0] = phy_ac_chanmgr_chan2freq_range(pi, pri);
    freq[1] = phy_ac_chanmgr_chan2freq_range(pi, sec);

}

/* get the complex freq. if chan==0, use default radio channel */
uint8
phy_ac_chanmgr_get_chan_freq_range(phy_info_t *pi, chanspec_t chanspec, uint8 core_segment_mapping)
{
    if (chanspec == 0) {
        chanspec = pi->radio_chanspec;
    }

    if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

        if (PHY_AS_80P80(pi, chanspec)) {
            PHY_INFORM(("wl%d: FIXME %s\n", pi->sh->unit, __FUNCTION__));
            chanspec = wf_chspec_primary80_chspec(chanspec);
        }
    } else {
        if (phy_get_phymode(pi) == PHYMODE_80P80) {
            if (CHSPEC_BW(chanspec) == WL_CHANSPEC_BW_8080) {
                if (PRIMARY_FREQ_SEGMENT == core_segment_mapping)
                    chanspec = wf_chspec_primary80_chspec(chanspec);

                if (SECONDARY_FREQ_SEGMENT == core_segment_mapping)
                    chanspec = wf_chspec_secondary80_chspec(chanspec);
            }
        }
    }

    return phy_ac_chanmgr_chan2freq_range(pi, chanspec);

}

/* Internal function to get frequency for given channel
 * if channel==0, use default channel
 */
static int
phy_ac_chanmgr_chan2freq(phy_info_t *pi, chanspec_t chanspec)
{
    uint8 channel;
    int freq = 0;

    if (chanspec == 0) {
        chanspec = pi->radio_chanspec;
    }

    channel = CHSPEC_CHANNEL(chanspec);

    if (RADIOID_IS(pi->pubpi->radioid, BCM2069_ID)) {
        const void *chan_info;
        freq = wlc_phy_chan2freq_acphy(pi, channel, &chan_info);
    } else if (RADIOID_IS(pi->pubpi->radioid, BCM20693_ID)) {
        const chan_info_radio20693_pll_t *chan_info_pll;
        const chan_info_radio20693_rffe_t *chan_info_rffe;
        const chan_info_radio20693_pll_wave2_t *chan_info_pll_wave2;
        freq = wlc_phy_chan2freq_20693(pi, channel, &chan_info_pll, &chan_info_rffe,
            &chan_info_pll_wave2);
    } else if (RADIOID_IS(pi->pubpi->radioid, BCM20698_ID)) {
        const chan_info_radio20698_rffe_t *chan_info;
        freq = wlc_phy_chan2freq_20698(pi, chanspec, &chan_info);
    } else if (RADIOID_IS(pi->pubpi->radioid, BCM20704_ID)) {
        const chan_info_radio20704_rffe_t *chan_info;
        freq = wlc_phy_chan2freq_20704(pi, chanspec, &chan_info);
    } else if (RADIOID_IS(pi->pubpi->radioid, BCM20707_ID)) {
        const chan_info_radio20707_rffe_t *chan_info;
        freq = wlc_phy_chan2freq_20707(pi, chanspec, &chan_info);
    } else if (RADIOID_IS(pi->pubpi->radioid, BCM20708_ID)) {
        const chan_info_radio20708_rffe_t *chan_info;
        freq = wlc_phy_chan2freq_20708(pi, chanspec, &chan_info);
    } else if (RADIOID_IS(pi->pubpi->radioid, BCM20709_ID)) {
        const chan_info_radio20709_rffe_t *chan_info;
        freq = wlc_phy_chan2freq_20709(pi, chanspec, &chan_info);
    } else if (RADIOID_IS(pi->pubpi->radioid, BCM20710_ID)) {
        const chan_info_radio20710_rffe_t *chan_info;
        freq = wlc_phy_chan2freq_20710(pi, chanspec, &chan_info);
    } else {
        ASSERT(0);
        freq = -1;
    }

    return freq;
}

/* Get the subband index (WL_CHAN_FREQ_RANGE_[25]G_*) for the given channel
 * according to the SROMREV<12 definitions, i.e. 4 subbands in 5G
 * reuse 4 subbands in 6G, each one covers 320MHz BW
 * if chan==0, use default radio channel
 */
static uint8
phy_ac_chanmgr_chan2freq_range(phy_info_t *pi, chanspec_t chanspec)
{
    int freq = phy_ac_chanmgr_chan2freq(pi, chanspec);
    bool is_6715A0MCH5 = FALSE;
    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));
    if (CHSPEC_IS6G(chanspec)) {
        if ((freq >= PHY_MAXNUM_6GSUBBANDS_BAND0) &&
            (freq < PHY_MAXNUM_6GSUBBANDS_BAND2))
            return WL_CHAN_FREQ_RANGE_5G_BAND0;
        else if ((freq >= PHY_MAXNUM_6GSUBBANDS_BAND2) &&
            (freq < PHY_MAXNUM_6GSUBBANDS_BAND4))
            return WL_CHAN_FREQ_RANGE_5G_BAND1;
        else if ((freq >= PHY_MAXNUM_6GSUBBANDS_BAND4) &&
            (freq < PHY_MAXNUM_6GSUBBANDS_BAND6))
            return WL_CHAN_FREQ_RANGE_5G_BAND2;
        else
            return WL_CHAN_FREQ_RANGE_5G_BAND3;
    }

    // Note: 6715MCH5 uses 5gSubbandVer = 6, but earlier drv code
    // is buggy hence falling into 3 subband Av/Vmid. In order to
    // keep the old behavior for 6715A0 MCH5(pdgain = 2) because its
    // DVT txpwrctrl test already clean, we make an exception here for this board.
    is_6715A0MCH5 = ACMAJORREV_130(pi->pubpi->phy_rev) && ACMINORREV_0(pi) &&
        CHSPEC_IS5G(chanspec) && (phy_tpc_get_5g_pdrange_id(pi->tpci) == 2);

    if (CHSPEC_IS2G(chanspec) || freq < 0) {
        return WL_CHAN_FREQ_RANGE_2G;
    } else if ((pi->sromi->subband5Gver == PHY_MAXNUM_5GSUBBANDS) ||
               (pi->sromi->subband5Gver == PHY_MAXNUM_5GSUBBANDS_V6 && (!is_6715A0MCH5)) ||
               (pi->sromi->subband5Gver == PHY_SUBBAND_4BAND)) {
        if ((freq >= PHY_SUBBAND_4BAND_BAND0) &&
            (freq < PHY_SUBBAND_4BAND_BAND1))
            return WL_CHAN_FREQ_RANGE_5G_BAND0;
        else if ((freq >= PHY_SUBBAND_4BAND_BAND1) &&
            (freq < PHY_SUBBAND_4BAND_BAND2))
            return WL_CHAN_FREQ_RANGE_5G_BAND1;
        else if ((freq >= PHY_SUBBAND_4BAND_BAND2) &&
            (freq < PHY_SUBBAND_4BAND_BAND3))
            return WL_CHAN_FREQ_RANGE_5G_BAND2;
        else
            return WL_CHAN_FREQ_RANGE_5G_BAND3;
    } else if (pi->sromi->subband5Gver == PHY_SUBBAND_3BAND_EMBDDED) {
        if ((freq >= EMBEDDED_LOW_5G_CHAN) && (freq < EMBEDDED_MID_5G_CHAN)) {
            return WL_CHAN_FREQ_RANGE_5GL;
        } else if ((freq >= EMBEDDED_MID_5G_CHAN) &&
                   (freq < EMBEDDED_HIGH_5G_CHAN)) {
            return WL_CHAN_FREQ_RANGE_5GM;
        } else {
            return WL_CHAN_FREQ_RANGE_5GH;
        }
    } else if (pi->sromi->subband5Gver == PHY_SUBBAND_3BAND_HIGHPWR) {
        if ((freq >= HIGHPWR_LOW_5G_CHAN) && (freq < HIGHPWR_MID_5G_CHAN)) {
            return WL_CHAN_FREQ_RANGE_5GL;
        } else if ((freq >= HIGHPWR_MID_5G_CHAN) && (freq < HIGHPWR_HIGH_5G_CHAN)) {
            return WL_CHAN_FREQ_RANGE_5GM;
        } else {
            return WL_CHAN_FREQ_RANGE_5GH;
        }
    } else { /* Default PPR Subband subband5Gver = 7 */
        if ((freq >= JAPAN_LOW_5G_CHAN) && (freq < JAPAN_MID_5G_CHAN)) {
            return WL_CHAN_FREQ_RANGE_5GL;
        } else if ((freq >= JAPAN_MID_5G_CHAN) && (freq < JAPAN_HIGH_5G_CHAN)) {
            return WL_CHAN_FREQ_RANGE_5GM;
        } else {
            return WL_CHAN_FREQ_RANGE_5GH;
        }
    }
}

/* get the complex freq for 80p80 channels. if chan==0, use default radio channel */
void
phy_ac_chanmgr_get_chan_freq_range_80p80_srom12(phy_info_t *pi, chanspec_t chanspec, uint8 *freq)
{
    chanspec_t pri, sec;

    ASSERT(SROMREV(pi->sh->sromrev) >= 12);

    if (chanspec == 0) {
        chanspec = pi->radio_chanspec;
    }

    ASSERT(PHY_AS_80P80(pi, chanspec) || CHSPEC_IS160(chanspec));

    pri = wf_chspec_primary80_chspec(chanspec);
    sec = wf_chspec_secondary80_chspec(chanspec);

    PHY_TRACE(("wl%d: %s: chanspec1=0x%04x chanspec2=0x%04x\n", pi->sh->unit,
            __FUNCTION__, pri, sec));

    freq[0] = phy_ac_chanmgr_chan2freq_range_srom12(pi, pri);
    freq[1] = phy_ac_chanmgr_chan2freq_range_srom12(pi, sec);
}

/* get the complex freq. if chan==0, use default radio channel */
uint8
phy_ac_chanmgr_get_chan_freq_range_srom12(phy_info_t *pi, chanspec_t chanspec)
{
    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    ASSERT(SROMREV(pi->sh->sromrev) >= 12);

    if (chanspec == 0) {
        chanspec = pi->radio_chanspec;
    }

    return phy_ac_chanmgr_chan2freq_range_srom12(pi, chanspec);
}

/* Get the subband index (WL_CHAN_FREQ_RANGE_[25]G_*) for the given channel
 * according to the SROMREV>=12 definitions, i.e. subband5gver=5 or 5 band in 5GHz
 * if chan==0, use default radio channel
 */
uint8
phy_ac_chanmgr_chan2freq_range_srom12(phy_info_t *pi, chanspec_t chanspec)
{
    int freq = phy_ac_chanmgr_chan2freq(pi, chanspec);

    ASSERT(SROMREV(pi->sh->sromrev) >= 12);
    if (CHSPEC_ISPHY5G6G(chanspec)) {
        ASSERT(pi->sromi->subband5Gver == PHY_MAXNUM_5GSUBBANDS ||
                SUBBANB5GV6(pi));
    }

    if (CHSPEC_IS2G(chanspec) || freq < PHY_MAXNUM_5GSUBBANDS_BAND0) {
        if (CHSPEC_IS40(chanspec))
            return WL_CHAN_FREQ_RANGE_2G_40;
        else
            return WL_CHAN_FREQ_RANGE_2G;
    } else if (freq < PHY_MAXNUM_6GSUBBANDS_BAND0) {
        /* 5G band definition */
        if ((freq >= PHY_MAXNUM_5GSUBBANDS_BAND0) &&
            (freq < PHY_MAXNUM_5GSUBBANDS_BAND1)) {
            if (CHSPEC_IS40(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND0_40;
            else if (CHSPEC_IS80(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND0_80;
            else if (CHSPEC_IS8080(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND0_80;
            else if (CHSPEC_IS160(chanspec) && PHY_AS_80P80(pi, chanspec)) {
                return WL_CHAN_FREQ_RANGE_5G_BAND0_80;
            } else
                return WL_CHAN_FREQ_RANGE_5G_BAND0;
        } else if ((freq >= PHY_MAXNUM_5GSUBBANDS_BAND1) &&
            (freq < PHY_MAXNUM_5GSUBBANDS_BAND2)) {
            if (CHSPEC_IS40(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND1_40;
            else if (CHSPEC_IS80(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND1_80;
            else if (CHSPEC_IS8080(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND1_80;
            else if (CHSPEC_IS160(chanspec)) {
                if (PHY_AS_80P80(pi, chanspec)) {
                    return WL_CHAN_FREQ_RANGE_5G_BAND1_80;
                } else {
                    if (freq == PHY_MAXNUM_5GSUBBANDS_BAND1) {
                        return WL_CHAN_FREQ_RANGE_5G_BAND0_160;
                    } else {
                        return WL_CHAN_FREQ_RANGE_5G_BAND1_160;
                    }
                }
            } else
                return WL_CHAN_FREQ_RANGE_5G_BAND1;
        } else if ((freq >= PHY_MAXNUM_5GSUBBANDS_BAND2) &&
            (freq < PHY_MAXNUM_5GSUBBANDS_BAND3)) {
            if (CHSPEC_IS40(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND2_40;
            else if (CHSPEC_IS80(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND2_80;
            else if (CHSPEC_IS8080(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND2_80;
            else if (CHSPEC_IS160(chanspec)) {
                if (PHY_AS_80P80(pi, chanspec)) {
                    return WL_CHAN_FREQ_RANGE_5G_BAND2_80;
                } else {
                    return WL_CHAN_FREQ_RANGE_5G_BAND2_160;
                }
            } else
                return WL_CHAN_FREQ_RANGE_5G_BAND2;
        } else if ((freq >= PHY_MAXNUM_5GSUBBANDS_BAND3) &&
            (freq < PHY_MAXNUM_5GSUBBANDS_BAND4)) {
            if (CHSPEC_IS40(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND3_40;
            else if (CHSPEC_IS80(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND3_80;
            else if (CHSPEC_IS8080(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND3_80;
            else if (PHY_AS_80P80(pi, chanspec)) {
                //no 160 channel in band3
                return WL_CHAN_FREQ_RANGE_5G_BAND3;
            } else
                return WL_CHAN_FREQ_RANGE_5G_BAND3;
#if BAND6G
        /* 5G subband5 will share 6G band5 SROM when subband5gver=6
         * BAND6G flag must be enabled, otherwise subband4 is used
         */
        } else if (SUBBANB5GV6(pi) && (freq >= PHY_MAXNUM_5GSUBBANDS_BAND5)) {
            PHY_TXPWR(("%s: Using chan freq range for 5g band5\n",
                    __FUNCTION__));
            if (CHSPEC_IS40(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND5_40;
            else if (CHSPEC_IS80(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND5_80;
            else if (CHSPEC_IS8080(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND5_80;
            else if (CHSPEC_IS160(chanspec)) {
                // should not go here as there is no valid channel
                return WL_CHAN_FREQ_RANGE_5G_BAND3_160;
            } else
                return WL_CHAN_FREQ_RANGE_5G_BAND5;
#endif /* BAND6G */
        } else {
            if (CHSPEC_IS40(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND4_40;
            else if (CHSPEC_IS80(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND4_80;
            else if (CHSPEC_IS8080(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND4_80;
            else if (CHSPEC_IS160(chanspec)) {
                if (PHY_AS_80P80(pi, chanspec)) {
                    return WL_CHAN_FREQ_RANGE_5G_BAND4_80;
                } else {
                    return WL_CHAN_FREQ_RANGE_5G_BAND3_160;
                }
            } else
                return WL_CHAN_FREQ_RANGE_5G_BAND4;
        }
    } else {
        /* 6G band definition */
        if ((freq >= PHY_MAXNUM_6GSUBBANDS_BAND0) &&
            (freq < PHY_MAXNUM_6GSUBBANDS_BAND1)) {
            if (CHSPEC_IS40(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND0_40;
            else if (CHSPEC_IS80(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND0_80;
            else if (CHSPEC_IS160(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND0_160;
            else
                return WL_CHAN_FREQ_RANGE_5G_BAND0;
        } else if ((freq >= PHY_MAXNUM_6GSUBBANDS_BAND1) &&
            (freq < PHY_MAXNUM_6GSUBBANDS_BAND2)) {
            if (CHSPEC_IS40(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND1_40;
            else if (CHSPEC_IS80(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND1_80;
            else if (CHSPEC_IS160(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND1_160;
            else
                return WL_CHAN_FREQ_RANGE_5G_BAND1;
        } else if ((freq >= PHY_MAXNUM_6GSUBBANDS_BAND2) &&
            (freq < PHY_MAXNUM_6GSUBBANDS_BAND3)) {
            if (CHSPEC_IS40(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND2_40;
            else if (CHSPEC_IS80(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND2_80;
            else if (CHSPEC_IS160(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND2_160;
            else
                return WL_CHAN_FREQ_RANGE_5G_BAND2;
        } else if ((freq >= PHY_MAXNUM_6GSUBBANDS_BAND3) &&
            (freq < PHY_MAXNUM_6GSUBBANDS_BAND4)) {
            if (CHSPEC_IS40(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND3_40;
            else if (CHSPEC_IS80(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND3_80;
            else if (CHSPEC_IS160(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND3_160;
            else
                return WL_CHAN_FREQ_RANGE_5G_BAND3;
        } else  if ((freq >= PHY_MAXNUM_6GSUBBANDS_BAND4) &&
            (freq < PHY_MAXNUM_6GSUBBANDS_BAND5)) {
            if (CHSPEC_IS40(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND4_40;
            else if (CHSPEC_IS80(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND4_80;
            else if (CHSPEC_IS160(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND4_160;
            else
                return WL_CHAN_FREQ_RANGE_5G_BAND4;
        } else  if ((freq >= PHY_MAXNUM_6GSUBBANDS_BAND5) &&
            (freq < PHY_MAXNUM_6GSUBBANDS_BAND6)) {
            if (CHSPEC_IS40(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND5_40;
            else if (CHSPEC_IS80(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND5_80;
            else if (CHSPEC_IS160(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND5_160;
            else
                return WL_CHAN_FREQ_RANGE_5G_BAND5;
        } else {
            if (CHSPEC_IS40(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND6_40;
            else if (CHSPEC_IS80(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND6_80;
            else if (CHSPEC_IS160(chanspec))
                return WL_CHAN_FREQ_RANGE_5G_BAND6_160;
            else
                return WL_CHAN_FREQ_RANGE_5G_BAND6;
        }
    }
}

static bool
phy_ac_chanmgr_is_txbfcal(phy_type_chanmgr_ctx_t *ctx)
{
    phy_ac_chanmgr_info_t *info = (phy_ac_chanmgr_info_t *)ctx;
    phy_info_t *pi = info->pi;
    uint8  subband_idx;
    uint8  chans[NUM_CHANS_IN_CHAN_BONDING];
    uint16 rpcal_val, rpcal_val1 = 0;
    bool   is_caled;

#ifdef MACOSX
    /* XXX FIXME: SWWLAN-31395 causes long channel switch times for 4360/43602
     * So disable implicit TXBF for 4360 for Macos for now
     */
    if (ACMAJORREV_0(pi->pubpi->phy_rev)|| ACMAJORREV_5(pi->pubpi->phy_rev))
        return FALSE;
#endif /* MACOSX */

    if (ACMAJORREV_33(pi->pubpi->phy_rev) && PHY_AS_80P80(pi, pi->radio_chanspec)) {
        phy_ac_chanmgr_get_chan_freq_range_80p80(pi, pi->radio_chanspec, chans);
        subband_idx = chans[0];
        PHY_INFORM(("wl%d: %s: FIXME for 80P80\n", pi->sh->unit, __FUNCTION__));
        // FIXME - core0/1: chans[0], core2/3 chans[1]
    } else {
        subband_idx = phy_ac_chanmgr_get_chan_freq_range(pi, 0, PRIMARY_FREQ_SEGMENT);
    }

    switch (subband_idx) {
    case WL_CHAN_FREQ_RANGE_2G:
        rpcal_val = pi->u.pi_acphy->sromi->rpcal2g;
        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            rpcal_val1 = pi->sromi->rpcal2gcore3;
        }
        break;
    case WL_CHAN_FREQ_RANGE_5G_BAND0:
        rpcal_val = pi->u.pi_acphy->sromi->rpcal5gb0;
        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            rpcal_val1 = pi->sromi->rpcal5gb0core3;
        }
        break;
    case WL_CHAN_FREQ_RANGE_5G_BAND1:
        rpcal_val = pi->u.pi_acphy->sromi->rpcal5gb1;
        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            rpcal_val1 = pi->sromi->rpcal5gb1core3;
        }
        break;
    case WL_CHAN_FREQ_RANGE_5G_BAND2:
        rpcal_val = pi->u.pi_acphy->sromi->rpcal5gb2;
        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            rpcal_val1 = pi->sromi->rpcal5gb2core3;
        }
        break;
    case WL_CHAN_FREQ_RANGE_5G_BAND3:
        rpcal_val = pi->u.pi_acphy->sromi->rpcal5gb3;
        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            rpcal_val1 = pi->sromi->rpcal5gb3core3;
        }
        break;
    default:
        PHY_ERROR(("wl%d: %s: Invalid chan_freq_range %d\n",
                   pi->sh->unit, __FUNCTION__, subband_idx));
        rpcal_val = pi->u.pi_acphy->sromi->rpcal2g;
        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            rpcal_val1 = pi->sromi->rpcal2gcore3;
        }
        break;
    }

    is_caled = (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) ?
            !(rpcal_val == 0 && rpcal_val1 == 0) : (rpcal_val != 0);

    return is_caled;
}

void
wlc_phy_smth(phy_info_t *pi, int8 enable_smth, int8 dump_mode)
{
#ifdef WL_PROXDETECT
    if (phy_ac_tof_forced_smth(pi->u.pi_acphy->tofi))
        return;
#endif

    if (ACMAJORREV_5(pi->pubpi->phy_rev) || ACMAJORREV_4(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        ACMAJORREV_GE40(pi->pubpi->phy_rev)) {
        phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
        uint16 SmthReg0, SmthReg1;

        ACPHY_REG_LIST_START
            /* Set the SigB to the default values */
            MOD_PHYREG_ENTRY(pi, musigb2, mu_sigbmcs9, 0x7)
            MOD_PHYREG_ENTRY(pi, musigb2, mu_sigbmcs8, 0x7)
            MOD_PHYREG_ENTRY(pi, musigb1, mu_sigbmcs7, 0x7)
            MOD_PHYREG_ENTRY(pi, musigb1, mu_sigbmcs6, 0x7)
            MOD_PHYREG_ENTRY(pi, musigb1, mu_sigbmcs5, 0x7)
            MOD_PHYREG_ENTRY(pi, musigb1, mu_sigbmcs4, 0x7)
            MOD_PHYREG_ENTRY(pi, musigb0, mu_sigbmcs3, 0x7)
            MOD_PHYREG_ENTRY(pi, musigb0, mu_sigbmcs2, 0x7)
            MOD_PHYREG_ENTRY(pi, musigb0, mu_sigbmcs1, 0x3)
            MOD_PHYREG_ENTRY(pi, musigb0, mu_sigbmcs0, 0x2)
        ACPHY_REG_LIST_EXECUTE(pi);

        pi_ac->chanmgri->acphy_smth_dump_mode = SMTH_NODUMP;

        switch (enable_smth) {
        case SMTH_DISABLE:
            /* Disable Smoothing and Enable SigB */
            SmthReg0 = READ_PHYREG(pi, chnsmCtrl0) & 0xFFFE;
            SmthReg1 = READ_PHYREG(pi, chnsmCtrl1);
            break;
        case SMTH_ENABLE:
            /* Enable Smoothing With all modes ON */
            /* This is the default config in which Smth is enabled for */
            /* SISO pkts and HT TxBF case. Use SigB for VHT-TxBF */
            SmthReg0 = 0x33F;
            SmthReg1 = 0x2C0;
            if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
                SmthReg0 |=
                    ACPHY_chnsmCtrl0_mte_pilot_enable_MASK(pi->pubpi->phy_rev);
            }
            if (ACMAJORREV_33(pi->pubpi->phy_rev) ||
                (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                !ACMAJORREV_128(pi->pubpi->phy_rev))) {
              SmthReg0 = 0x37f;
              SmthReg1 = 0x5eC0;
            }
            if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
                SmthReg0 = 0x377;
                SmthReg1 = 0x5EC0;
            }
            pi_ac->chanmgri->acphy_smth_dump_mode = dump_mode;
            switch (dump_mode)
            {
            case SMTH_FREQDUMP:
            /* Enable Freq-domain dumping (Raw Channel Estimates) */
            SmthReg0 &= ~(
                ACPHY_chnsmCtrl0_nw_whiten_enable_MASK(pi->pubpi->phy_rev) |
                ACPHY_chnsmCtrl0_group_delay_enable_MASK(pi->pubpi->phy_rev) |
                ACPHY_chnsmCtrl0_mte_enable_MASK(pi->pubpi->phy_rev) |
                ACPHY_chnsmCtrl0_window_enable_MASK(pi->pubpi->phy_rev) |
                ACPHY_chnsmCtrl0_fft_enable_MASK(pi->pubpi->phy_rev));
            SmthReg1 &= ~(
                ACPHY_chnsmCtrl1_ifft_enable_MASK(pi->pubpi->phy_rev) |
                ACPHY_chnsmCtrl1_output_enable_MASK(pi->pubpi->phy_rev));
            break;

            case SMTH_FREQDUMP_AFTER_NW:
            /* Enable Freq-domain dumping (After NW Filtering) */
            SmthReg0 &= ~(
                ACPHY_chnsmCtrl0_group_delay_enable_MASK(pi->pubpi->phy_rev) |
                ACPHY_chnsmCtrl0_mte_enable_MASK(pi->pubpi->phy_rev) |
                ACPHY_chnsmCtrl0_window_enable_MASK(pi->pubpi->phy_rev) |
                ACPHY_chnsmCtrl0_fft_enable_MASK(pi->pubpi->phy_rev));
            SmthReg1 &= ~(
                ACPHY_chnsmCtrl1_ifft_enable_MASK(pi->pubpi->phy_rev) |
                ACPHY_chnsmCtrl1_output_enable_MASK(pi->pubpi->phy_rev));
            break;

            case SMTH_FREQDUMP_AFTER_GD:
            /* Enable Freq-domain dumping (After GD Compensation) */
            SmthReg0 &= ~(
                ACPHY_chnsmCtrl0_mte_enable_MASK(pi->pubpi->phy_rev) |
                ACPHY_chnsmCtrl0_window_enable_MASK(pi->pubpi->phy_rev) |
                ACPHY_chnsmCtrl0_fft_enable_MASK(pi->pubpi->phy_rev));
            SmthReg1 &= ~(
                ACPHY_chnsmCtrl1_ifft_enable_MASK(pi->pubpi->phy_rev) |
                ACPHY_chnsmCtrl1_output_enable_MASK(pi->pubpi->phy_rev));
            break;

            case SMTH_FREQDUMP_AFTER_MTE:
            /* Enable Freq-domain dumping (After MTE) */
            SmthReg0 &= ~(
                ACPHY_chnsmCtrl0_window_enable_MASK(pi->pubpi->phy_rev) |
                ACPHY_chnsmCtrl0_fft_enable_MASK(pi->pubpi->phy_rev));
            SmthReg1 &= ~(
                ACPHY_chnsmCtrl1_ifft_enable_MASK(pi->pubpi->phy_rev) |
                ACPHY_chnsmCtrl1_output_enable_MASK(pi->pubpi->phy_rev));
            break;

            case SMTH_TIMEDUMP_AFTER_IFFT:
            /* Enable Time-domain dumping (After IFFT) */
            SmthReg0 &= ~(
                ACPHY_chnsmCtrl0_window_enable_MASK(pi->pubpi->phy_rev) |
                ACPHY_chnsmCtrl0_fft_enable_MASK(pi->pubpi->phy_rev));
            SmthReg1 &= ~ACPHY_chnsmCtrl1_output_enable_MASK(pi->pubpi->phy_rev);
            break;

            case SMTH_TIMEDUMP_AFTER_WIN:
                /* Enable Time-domain dumping (After Windowing) */
            SmthReg0 &= ~ACPHY_chnsmCtrl0_fft_enable_MASK(pi->pubpi->phy_rev);
            SmthReg1 &= ~ACPHY_chnsmCtrl1_output_enable_MASK(pi->pubpi->phy_rev);
            break;

            case SMTH_FREQDUMP_AFTER_FFT:
            /* Enable Freq-domain dumping (After FFT) */
            SmthReg1 &= ~ACPHY_chnsmCtrl1_output_enable_MASK(pi->pubpi->phy_rev);
            break;
            }
            break;
        case SMTH_ENABLE_NO_NW:
            /* Enable Smoothing With all modes ON Except NW Filter */
            SmthReg0 = 0x337;
            SmthReg1 = 0x2C0;
            if (ACMAJORREV_33(pi->pubpi->phy_rev) ||
                (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                !ACMAJORREV_128(pi->pubpi->phy_rev))) {
              SmthReg0 = 0x377;
              SmthReg1 = 0x5eC0;
            }
            break;
        case SMTH_ENABLE_NO_NW_GD:
            /* Enable Smoothing With all modes ON Except NW and GD  */
            SmthReg0 = 0x327;
            SmthReg1 = 0x2C0;
            if (ACMAJORREV_33(pi->pubpi->phy_rev) ||
                (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                !ACMAJORREV_128(pi->pubpi->phy_rev))) {
              SmthReg0 = 0x367;
              SmthReg1 = 0x5eC0;
            }
            break;
        case SMTH_ENABLE_NO_NW_GD_MTE:
            /* Enable Smoothing With all modes ON Except NW, GD and  MTE */
            SmthReg0 = 0x307;
            SmthReg1 = 0x2C0;
            if (ACMAJORREV_33(pi->pubpi->phy_rev) ||
                (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                !ACMAJORREV_128(pi->pubpi->phy_rev))) {
              SmthReg0 = 0x307;
              SmthReg1 = 0x5eC0;
            }
            break;
        case DISABLE_SIGB_AND_SMTH:
            /* Disable Smoothing and SigB */
            SmthReg0 = 0x33E;
            SmthReg1 = 0x0C0;
            if (ACMAJORREV_33(pi->pubpi->phy_rev) ||
                (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                !ACMAJORREV_128(pi->pubpi->phy_rev))) {
              SmthReg0 = 0x37E;
              SmthReg1 = 0x0C0;
            }
            ACPHY_REG_LIST_START
                MOD_PHYREG_ENTRY(pi, musigb2, mu_sigbmcs9, 0x0)
                MOD_PHYREG_ENTRY(pi, musigb2, mu_sigbmcs8, 0x0)
                MOD_PHYREG_ENTRY(pi, musigb1, mu_sigbmcs7, 0x0)
                MOD_PHYREG_ENTRY(pi, musigb1, mu_sigbmcs6, 0x0)
                MOD_PHYREG_ENTRY(pi, musigb1, mu_sigbmcs5, 0x0)
                MOD_PHYREG_ENTRY(pi, musigb1, mu_sigbmcs4, 0x0)
                MOD_PHYREG_ENTRY(pi, musigb0, mu_sigbmcs3, 0x0)
                MOD_PHYREG_ENTRY(pi, musigb0, mu_sigbmcs2, 0x0)
                MOD_PHYREG_ENTRY(pi, musigb0, mu_sigbmcs1, 0x0)
                MOD_PHYREG_ENTRY(pi, musigb0, mu_sigbmcs0, 0x0)
            ACPHY_REG_LIST_EXECUTE(pi);
            break;
        case SMTH_FOR_TXBF:
            /* Enable Smoothing for TxBF using Smth for HT and VHT */
            SmthReg0 = 0x33F;
            SmthReg1 = 0x6C0;
            if (ACMAJORREV_33(pi->pubpi->phy_rev) ||
                (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
                !ACMAJORREV_128(pi->pubpi->phy_rev))) {
              SmthReg0 = 0x37F;
              SmthReg1 = 0x5EC0;
            }
            break;
        default:
            PHY_ERROR(("wl%d: %s: Unrecognized smoothing mode: %d\n",
                      pi->sh->unit, __FUNCTION__, enable_smth));
            return;
        }
        WRITE_PHYREG(pi, chnsmCtrl0, SmthReg0);
        WRITE_PHYREG(pi, chnsmCtrl1, SmthReg1);
        pi_ac->chanmgri->acphy_enable_smth = enable_smth;

        if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
            /* 4349 specific setting */
            if (enable_smth == SMTH_ENABLE) {
                /* output_enable_new = 0x0 no output
                 * output_enable_new = 0x1 only legacy channel is smoothed
                 * output_enable_new = 0x2 only HT/VHT channel is smoothed
                 * output_enable_new = 0x3 both legacy and HT/VHT are smoothed
                 */
                /* 0x2 since TXBF doesn't work if legacy smoothing is enabled */
                MOD_PHYREG(pi, chnsmCtrl1, output_enable_new, 0x2);
            } else {
                MOD_PHYREG(pi, chnsmCtrl1, output_enable_new, 0x0);
            }
            if ((phy_get_phymode(pi) == PHYMODE_MIMO) &&
                    (phy_stf_get_data(pi->stfi)->phyrxchain == 0x3)) {
                MOD_PHYREG(pi, chnsmCtrl1, disable_2rx_nvar_calc, 0x0);
            } else {
                MOD_PHYREG(pi, chnsmCtrl1, disable_2rx_nvar_calc, 0x1);
            }
            MOD_PHYREG(pi, nvcfg3, unity_gain_for_2x2_coremask2, 0x1);
        } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, chnsmCtrl0, mte_pilot_enable, 0x1);
            MOD_PHYREG(pi, chnsmCtrl1, ignore_VHT_txbf_bit, 0x1);
            if (enable_smth == SMTH_ENABLE) {
                /* output_enable_new = 0x0 no output
                 * output_enable_new = 0x1 only legacy channel is smoothed
                 * output_enable_new = 0x2 only HT/VHT channel is smoothed
                 * output_enable_new = 0x3 both legacy and HT/VHT are smoothed
                 */
                MOD_PHYREG(pi, chnsmCtrl1, output_enable_new, 0x3);
            } else {
                MOD_PHYREG(pi, chnsmCtrl1, output_enable_new, 0x0);
            }
        }

        if (ACMAJORREV_GE37(pi->pubpi->phy_rev) &&
            !ACMAJORREV_130(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, nvcfg3, unity_gain_for_2x2_coremask2, 0x1);
        }
    }

    /* Disable channel smoothing for 43684 B0 in 160MHz.
     * Cause PER hump with LESI on. JIRA: HW43684-541
     * Issue is fixed in 43684 C0
     */
    if (ACMAJORREV_47(pi->pubpi->phy_rev) && ACMINORREV_LE(pi, 2))
        phy_ac_chanmgr_tdcs_160m_rev47(pi);
}

static bool
phy_ac_chanmgr_is_smth_en(phy_type_chanmgr_ctx_t *ctx, bool ishe)
{
    phy_ac_chanmgr_info_t *info = (phy_ac_chanmgr_info_t *)ctx;
    phy_info_t *pi = info->pi;
    bool is_smth_en;
    if (ishe == TRUE) {
        /* 11ax tdcs is supported from 6715 */
        if (ACMAJORREV_GE130(pi->pubpi->phy_rev)) {
            is_smth_en = (pi->u.pi_acphy->chanmgri->acphy_enable_smth == 1) ?
                TRUE : FALSE;
            return is_smth_en;
        } else {
            return FALSE;
        }
    } else {
        /* 11ac tdcs  */
        if (ACMAJORREV_5(pi->pubpi->phy_rev) || ACMAJORREV_4(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) || ACMAJORREV_GE40(pi->pubpi->phy_rev)) {
            is_smth_en = (pi->u.pi_acphy->chanmgri->acphy_enable_smth == 1) ?
                TRUE : FALSE;
            return is_smth_en;
        } else {
            return FALSE;
        }
    }
}

static void
phy_ac_chanmgr_smth_per_nss_SUTxbf(phy_type_chanmgr_ctx_t *ctx, uint8 uint_val)
{
    uint8 mac_suspend;
    phy_ac_chanmgr_info_t *info = (phy_ac_chanmgr_info_t *)ctx;
    phy_info_t *pi = info->pi;

    if (!pi->sh->clk) {
        return;
    }

    mac_suspend = (R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC);

    if (mac_suspend) wlapi_suspend_mac_and_wait(pi->sh->physhim);

    if (ACMAJORREV_33(pi->pubpi->phy_rev) || ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, chnsmCtr21, chnsm_en_per_nss_SUTxbf, uint_val);
    }

    if (mac_suspend) wlapi_enable_mac(pi->sh->physhim);
}

void
wlc_phy_ac_shared_ant_femctrl_master(phy_info_t *pi)
{
    PHY_ERROR(("%s: Multi Slice femctrl not supported\n", __FUNCTION__));
}

#if (defined(WL_SISOCHIP) || !defined(SWCTRL_TO_BT_IN_COEX))
static void
wlc_phy_ac_femctrl_mask_on_band_change(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
    if (!ACMAJORREV_0(pi->pubpi->phy_rev)) {
        /* When WLAN is in 5G, WLAN table should control the FEM lines */
        /* and BT should not have any access permissions */
        if (CHSPEC_ISPHY5G6G(pi->radio_chanspec)) {
            /* disable BT Fem control table accesses */
            MOD_PHYREG(pi, FemCtrl, enBtSignalsToFEMLut, 0x0);
            if (!ACPHY_FEMCTRL_ACTIVE(pi)) {
                if (ACMAJORREV_4(pi->pubpi->phy_rev) &&
                    !BF3_RSDB_1x1_BOARD(pi_ac))  {
                    if (phy_get_phymode(pi) == PHYMODE_MIMO) {
                        /* writes to both cores */
                        MOD_PHYREG(pi, FemCtrl, femCtrlMask, 0x3ff);
                        /* now write to only core0 */
                        wlapi_exclusive_reg_access_core0(
                            pi->sh->physhim, 1);
                        MOD_PHYREG(pi, FemCtrl, femCtrlMask,
                            pi_ac->sromi->femctrlmask_5g);
                        wlapi_exclusive_reg_access_core0(
                            pi->sh->physhim, 0);
                    } else if (phy_get_phymode(pi) == PHYMODE_RSDB &&
                        phy_get_current_core(pi) == PHY_RSBD_PI_IDX_CORE1) {
                        MOD_PHYREG(pi, FemCtrl, femCtrlMask, 0x3ff);
                    } else {
                        MOD_PHYREG(pi, FemCtrl, femCtrlMask,
                            pi_ac->sromi->femctrlmask_5g);
                    }
                } else {
                    MOD_PHYREG(pi, FemCtrl, femCtrlMask,
                        pi_ac->sromi->femctrlmask_5g);
                }
            } else {
                if (ACMAJORREV_2(pi->pubpi->phy_rev) ||
                           ACMAJORREV_5(pi->pubpi->phy_rev)) {
                    if (BFCTL(pi_ac) == 10) {
                        if (BF3_FEMCTRL_SUB(pi_ac) == 0) {
                            MOD_PHYREG(pi, FemCtrl, femCtrlMask,
                                       0x317);
                        } else if (BF3_FEMCTRL_SUB(pi_ac) == 1) {
                            MOD_PHYREG(pi, FemCtrl, femCtrlMask,
                                       0x347);
                        } else if (BF3_FEMCTRL_SUB(pi_ac) == 2) {
                            MOD_PHYREG(pi, FemCtrl, femCtrlMask,
                                       0x303);
                        } else if (BF3_FEMCTRL_SUB(pi_ac) == 3) {
                            MOD_PHYREG(pi, FemCtrl, femCtrlMask,
                                       0x307);
                        } else if (BF3_FEMCTRL_SUB(pi_ac) == 4) {
                            MOD_PHYREG(pi, FemCtrl, femCtrlMask,
                                       0x309);
                        } else if (BF3_FEMCTRL_SUB(pi_ac) == 5) {
                            MOD_PHYREG(pi, FemCtrl, femCtrlMask,
                                       0x3c7);
                        } else {
                            MOD_PHYREG(pi, FemCtrl, femCtrlMask,
                                       0x3ff);
                        }
                    } else if (pi->u.pi_acphy->sromi->femctrl == 2) {
                      if (pi->u.pi_acphy->sromi->femctrl_sub == 5)
                        si_pmu_switch_off_PARLDO(pi->sh->sih, pi->sh->osh);
                      if (pi->u.pi_acphy->sromi->femctrl_sub == 6)
                        si_pmu_switch_on_PARLDO(pi->sh->sih, pi->sh->osh);
                    } else {
                        MOD_PHYREG(pi, FemCtrl, femCtrlMask, 0x3ff);
                    }
                } else if (TINY_RADIO(pi)) {
                    MOD_PHYREG(pi, FemCtrl, femCtrlMask, 0x3ff);
                }
            }
        } else { /* When WLAN is in 2G, BT controls should be allowed to go through */
            /* BT should also be able to control FEM Control Table */
            if ((!(CHIPID(pi->sh->chip) == BCM43602_CHIP_ID ||
                CHIPID(pi->sh->chip) == BCM43462_CHIP_ID)) &&
                BF_SROM11_BTCOEX(pi_ac) &&
                (BOARDFLAGS(GENERIC_PHY_INFO(pi)->boardflags) & BFL_FEM_BT)) {
                if (BOARDFLAGS(GENERIC_PHY_INFO(pi)->boardflags2) &
                    BFL2_BT_SHARE_ANT0) {
                    MOD_PHYREG(pi, FemCtrl, enBtSignalsToFEMLut, 0x1);
                } else {
                    MOD_PHYREG(pi, FemCtrl, enBtSignalsToFEMLut, 0x0);
                }
            } else {
                MOD_PHYREG(pi, FemCtrl, enBtSignalsToFEMLut, 0x0);
            }

            MOD_PHYREG(pi, FemCtrl, femCtrlMask, 0x3ff);

            if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
                MOD_PHYREG(pi, FemCtrl, femCtrlMask,
                    pi_ac->sromi->femctrlmask_2g);
                if (!(BOARDFLAGS(GENERIC_PHY_INFO(pi)->boardflags) & BFL_FEM_BT)) {
                    MOD_PHYREG(pi, FemCtrl, enBtSignalsToFEMLut, 0x0);
                } else {
                    if (BF3_RSDB_1x1_BOARD(pi_ac)) {
                        MOD_PHYREG(pi, FemCtrl, enBtSignalsToFEMLut, 0x1);
                    } else {
                        if (phy_get_phymode(pi) == PHYMODE_MIMO) {
                        /* writes to both cores */
                        MOD_PHYREG(pi, FemCtrl, enBtSignalsToFEMLut, 0x0);
                        wlapi_exclusive_reg_access_core0(
                            pi->sh->physhim, 1);
                        /* writes to only core0 */
                        MOD_PHYREG(pi, FemCtrl, enBtSignalsToFEMLut, 0x1);
                        wlapi_exclusive_reg_access_core0(
                            pi->sh->physhim, 0);
                        } else if (phy_get_phymode(pi) == PHYMODE_RSDB)  {
                            if (phy_get_current_core(pi) == 0) {
                                MOD_PHYREG(pi, FemCtrl,
                                    enBtSignalsToFEMLut, 0x1);
                            } else {
                                MOD_PHYREG(pi, FemCtrl,
                                    enBtSignalsToFEMLut, 0x0);
                            }
                        }
                    }
                }
            }

            if (ACMAJORREV_5(pi->pubpi->phy_rev) &&
                pi->u.pi_acphy->sromi->femctrl == 2) {
                if (pi->u.pi_acphy->sromi->femctrl_sub == 5)
                    si_pmu_switch_on_PARLDO(pi->sh->sih, pi->sh->osh);
                if (pi->u.pi_acphy->sromi->femctrl_sub == 6)
                    si_pmu_switch_off_PARLDO(pi->sh->sih, pi->sh->osh);
            }
        }
    }
}
#endif /* (defined(WL_SISOCHIP) || !defined(SWCTRL_TO_BT_IN_COEX)) */

void
phy_ac_chanmgr_core2core_sync_setup(phy_ac_chanmgr_info_t *chanmgri, bool enable)
{
    phy_info_t *pi = chanmgri->pi;
    uint8 core;
    uint8 val =  (enable ? 1 : 0);
    bool  suspend = !(R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC);

    //FIXME43684, suggest to turn core2core syncup on
    if (!((ACMAJORREV_32(pi->pubpi->phy_rev) && ACMINORREV_2(pi)) ||
          ACMAJORREV_33(pi->pubpi->phy_rev) || pi->pubpi->phy_rev >= 40)) {
        return;
    }

    PHY_TRACE(("%s: enable = %d\n", __FUNCTION__, (uint8)enable));

    if (!suspend)
        wlapi_suspend_mac_and_wait(pi->sh->physhim);

    FOREACH_CORE(pi, core) {
        if (enable) {
            MOD_PHYREGCE(pi, RfctrlCoreAfeCfg1, core, afe_iqdac_pwrup, 1);
            MOD_PHYREGCE(pi, RfctrlCoreAfeCfg2, core, afe_iqadc_clamp_en, 1);
            MOD_PHYREGCE(pi, RfctrlCoreAfeCfg2, core, afe_iqadc_flashhspd, 1);
        }
        MOD_PHYREGCE(pi, RfctrlOverrideAfeCfg, core, afe_iqdac_pwrup, val);
        MOD_PHYREGCE(pi, RfctrlOverrideAfeCfg, core, afe_iqadc_clamp_en, val);
        MOD_PHYREGCE(pi, RfctrlOverrideAfeCfg, core, afe_iqadc_flashhspd, val);
    }

    if (enable &&
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        /* Additional radio settings to keep clocks powered on (sequence matters!) */

        /* Set RfctrlIntc to bypass mode to fix spur before CAC */
        FOREACH_CORE(pi, core) {
            WRITE_PHYREGCE(pi, RfctrlIntc, core, 0x1c00);
        }

        /* Setup */
        phy_ac_chanmgr_core2core_sync_dac_clks(pi->u.pi_acphy->chanmgri, TRUE);

        /* Restore RfctrlIntc settings */
        FOREACH_CORE(pi, core) {
            WRITE_PHYREGCE(pi, RfctrlIntc, core, 0x0);
        }

        /* Additional PHY setting to avoid farrow txunderflow for core2core sync ON */
        MOD_PHYREG(pi, fineclockgatecontrol, forcetxlbClkEn, 1);
    }

    if (ACMAJORREV_32(pi->pubpi->phy_rev) && ACMINORREV_2(pi)) {
        /* Sparereg setting needed for 4365B1 */
        uint16 sparereg;
        sparereg = READ_PHYREG(pi, SpareReg);
        if (enable) {
            MOD_PHYREG(pi, SpareReg, spareReg, sparereg | 0x0040);
        } else {
            MOD_PHYREG(pi, SpareReg, spareReg, sparereg & 0xffbf);
        }
    }

    FOREACH_CORE(pi, core) {
        MOD_PHYREGCE(pi, TxResamplerEnable, core, txfe_baseband_enfree, val);
        MOD_PHYREGCE(pi, TxResamplerEnable, core, txfetoptxfrreseten, val);
        MOD_PHYREGCE(pi, TxResamplerEnable, core, mufreeWren, val);
    }

    MOD_PHYREG(pi, dacClkCtrl, txcore2corefrclken, val);
    MOD_PHYREG(pi, dacClkCtrl, txcore2corefrdacclken, val);
    MOD_PHYREG(pi, dacClkCtrl, gateafeclocksoveren, val);

    if ((ACMAJORREV_32(pi->pubpi->phy_rev) && ACMINORREV_2(pi)) ||
        ACMAJORREV_128(pi->pubpi->phy_rev)) {
        /* 4365B1 & 6878 */
        MOD_PHYREG(pi, dacClkCtrl, txfarrowresetfreeen, val);
    } else if (ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        /* 4365C0, 43684 */
        uint8 valc =  (enable ? 0 : 1);
        MOD_PHYREG(pi, dacClkCtrl, dacpuoren, val);
        MOD_PHYREG(pi, dacClkCtrl, txframeoreden, val);
        MOD_PHYREG(pi, dacClkCtrl, endacratiochgvld, 0);
        FOREACH_CORE(pi, core) {
            /* Protect core2core sync against resetcca */
            MOD_PHYREGCE(pi, TxResamplerEnable, core, tx_fifo_resetccaEn, valc);
            if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
                /* Prevent txfarrow reset at end of frame */
                MOD_PHYREGCE(pi, TxResamplerEnable, core, tx_fifo_txresetEn, valc);
            }
        }
        MOD_PHYREG(pi, dacClkCtrl, txfarrowresetfreeen, val);
        /* WAR for core2core sync (EVM jaggedness/ PER floor) */
        MOD_PHYREG(pi, bphytestcontrol, bphytestAntselect, 0xf);
    }

    if (!enable) {
        /* Resetcca when disabling core2core sync */
        uint8 stall_val;
        stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
        ACPHY_DISABLE_STALL(pi);
        wlc_phy_resetcca_acphy(pi);
        ACPHY_ENABLE_STALL(pi, stall_val);

        if (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)) {
            /* should be done after resetcca (SWWLAN-191901) */
            phy_ac_chanmgr_core2core_sync_dac_clks(pi->u.pi_acphy->chanmgri, FALSE);
            MOD_PHYREG(pi, fineclockgatecontrol, forcetxlbClkEn, 0);
        }
    }

    if (!suspend)
        wlapi_enable_mac(pi->sh->physhim);

}

void
phy_ac_chanmgr_core2core_sync_dac_clks(phy_ac_chanmgr_info_t *chanmgri, bool enable)
{
    phy_info_t *pi = chanmgri->pi;
    uint8 core;
    uint8 pll_num = 0;
    uint8 restore_ext_5g_papu[PHY_CORE_MAX];
    uint8 restore_override_ext_pa[PHY_CORE_MAX];

    /* Disable PA during rfseq setting */
    FOREACH_CORE(pi, core) {
        restore_ext_5g_papu[core] = READ_PHYREGFLDCE(pi, RfctrlIntc,
                core, ext_5g_papu);
        restore_override_ext_pa[core] = READ_PHYREGFLDCE(pi, RfctrlIntc,
                core, override_ext_pa);
        MOD_PHYREGCE(pi, RfctrlIntc, core, override_ext_pa, 1);
        MOD_PHYREGCE(pi, RfctrlIntc, core, ext_5g_papu, 0);
        MOD_PHYREGCE(pi, RfctrlIntc, core, tr_sw_tx_pu, 0);
        MOD_PHYREGCE(pi, RfctrlIntc, core, tr_sw_rx_pu, 1);
        MOD_PHYREGCE(pi, RfctrlIntc, core, override_tr_sw, 1);
    }

    if (RADIOID_IS(pi->pubpi->radioid, BCM20708_ID)) {
        pll_num = pi->u.pi_acphy->radioi->maincore_on_pll1 ? 1 : 0;
    }

    /* Additional radio settings to keep DAC clocks powered on (sequence matters!) */
    if (enable) {
        pi->u.pi_acphy->c2csync_dac_clks_on = TRUE;
        /* Setup */
        if (!ACMAJORREV_130(pi->pubpi->phy_rev)) {
            FOREACH_CORE(pi, core) {
                /* First overwrite dac_div_pu's then force div_resets low
                 * to avoid glitch
                 */
                if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
                    MOD_RADIO_REG_20698(pi, AFEDIV_REG0, core,
                                        afediv_dac_div_pu, 1);
                    MOD_RADIO_REG_20698(pi, AFEDIV_CFG1_OVR, core,
                                        ovr_afediv_dac_div_pu, 1);
                } else if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
                    MOD_RADIO_REG_20704(pi, AFEDIV_REG0, core,
                                        afediv_dac_div_pu, 1);
                    MOD_RADIO_REG_20704(pi, AFEDIV_CFG1_OVR, core,
                                        ovr_afediv_dac_div_pu, 1);
                } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
                    MOD_RADIO_REG_20707(pi, AFEDIV_REG0, core,
                                        afediv_dac_div_pu, 1);
                    MOD_RADIO_REG_20707(pi, AFEDIV_CFG1_OVR, core,
                                        ovr_afediv_dac_div_pu, 1);
                } else if (ACMAJORREV_131(pi->pubpi->phy_rev)) {
                    MOD_RADIO_REG_20710(pi, AFEDIV_REG0, core,
                                        afediv_dac_div_pu, 1);
                    MOD_RADIO_REG_20710(pi, AFEDIV_CFG1_OVR, core,
                                        ovr_afediv_dac_div_pu, 1);
                }

                wlc_phy_force_rfseq_acphy(pi, ACPHY_RFSEQ_RX2TX);
                OSL_DELAY(10);
                wlc_phy_force_rfseq_acphy(pi, ACPHY_RFSEQ_TX2RX);

                /* ensure dac outbuf always on via override
                 * (inbuf always on via override by default)
                 */
                if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
                    MOD_RADIO_REG_20698(pi, AFEDIV_REG0, core,
                                        afediv_dac_div_reset, 0);
                    MOD_RADIO_REG_20698(pi, AFEDIV_CFG1_OVR, core,
                                        ovr_afediv_dac_div_reset, 1);
                    MOD_RADIO_REG_20698(pi, AFEDIV_REG0, core,
                                        afediv_outbuf_dac_pu, 1);
                    MOD_RADIO_REG_20698(pi, AFEDIV_CFG1_OVR, core,
                                        ovr_afediv_outbuf_dac_pu, 1);
                } else if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
                    MOD_RADIO_REG_20704(pi, AFEDIV_REG0, core,
                                        afediv_dac_div_reset, 0);
                    MOD_RADIO_REG_20704(pi, AFEDIV_CFG1_OVR, core,
                                        ovr_afediv_dac_div_reset, 1);
                    MOD_RADIO_REG_20704(pi, AFEDIV_REG0, core,
                                        afediv_outbuf_dac_pu, 1);
                    MOD_RADIO_REG_20704(pi, AFEDIV_CFG1_OVR, core,
                                        ovr_afediv_outbuf_dac_pu, 1);
                } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
                    MOD_RADIO_REG_20707(pi, AFEDIV_REG0, core,
                                        afediv_dac_div_reset, 0);
                    MOD_RADIO_REG_20707(pi, AFEDIV_CFG1_OVR, core,
                                        ovr_afediv_dac_div_reset, 1);
                    MOD_RADIO_REG_20707(pi, AFEDIV_REG0, core,
                                        afediv_outbuf_dac_pu, 1);
                    MOD_RADIO_REG_20707(pi, AFEDIV_CFG1_OVR, core,
                                        ovr_afediv_outbuf_dac_pu, 1);
                } else if (ACMAJORREV_131(pi->pubpi->phy_rev)) {
                    MOD_RADIO_REG_20710(pi, AFEDIV_REG0, core,
                                        afediv_dac_div_reset, 0);
                    MOD_RADIO_REG_20710(pi, AFEDIV_CFG1_OVR, core,
                                        ovr_afediv_dac_div_reset, 1);
                    MOD_RADIO_REG_20710(pi, AFEDIV_REG0, core,
                                        afediv_outbuf_dac_pu, 1);
                    MOD_RADIO_REG_20710(pi, AFEDIV_CFG1_OVR, core,
                                        ovr_afediv_outbuf_dac_pu, 1);
                }
            }
        } else {
            /* First overwrite dac_div_pu's then force div_resets low
             * to avoid glitch
             */
            MOD_RADIO_PLLREG_20708(pi, AFEDIV_CFG1_OVR, pll_num,
                    ovr_afediv_dac_div_pu, 1);
            MOD_RADIO_PLLREG_20708(pi, AFEDIV_REG0, pll_num,
                    afediv_dac_div_pu, 1);

            wlc_phy_force_rfseq_acphy(pi, ACPHY_RFSEQ_RX2TX);
            OSL_DELAY(10);
            wlc_phy_force_rfseq_acphy(pi, ACPHY_RFSEQ_TX2RX);

            MOD_RADIO_PLLREG_20708(pi, AFEDIV_CFG1_OVR, pll_num,
                    ovr_afediv_div0_reset, 1);
            MOD_RADIO_PLLREG_20708(pi, AFEDIV_REG3, pll_num,
                    afediv_div0_reset, 0);
            MOD_RADIO_PLLREG_20708(pi, AFEDIV_CFG1_OVR, pll_num,
                    ovr_afediv_pu_clk_buffers, 1);
            MOD_RADIO_PLLREG_20708(pi, AFEDIV_REG2, pll_num,
                    afediv_pu_clk_buffers, 1);

            /* ensure dac outbuf always on via override
             * (inbuf always on via override by default)
             */
            // FIXME - need to review with TCL/Radio team
            MOD_RADIO_PLLREG_20708(pi, AFEDIV_CFG1_OVR, pll_num,
                    ovr_afediv_outbuf_dac_pu, 1);
            MOD_RADIO_PLLREG_20708(pi, AFEDIV_REG0, pll_num,
                    afediv_pu_outbuf_dac, 1);
            MOD_RADIO_PLLREG_20708(pi, AFEDIV_CFG1_OVR, pll_num,
                    ovr_afediv_pu_inbuf, 1);
            MOD_RADIO_PLLREG_20708(pi, AFEDIV_REG0, pll_num,
                    afediv_pu_inbuf, 1);
        }

    } else {
        pi->u.pi_acphy->c2csync_dac_clks_on = FALSE;
        /* 43684 specific settings
         *  - Remove radioreg overrides for keeping DAC clocks ON
         *  - should be done after resetcca (SWWLAN-191901)
         */
        if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
            FOREACH_CORE(pi, core) {
                /* Remove dac outbuf overrides */
                MOD_RADIO_REG_20698(pi, AFEDIV_CFG1_OVR, core,
                                    ovr_afediv_outbuf_dac_pu, 0);
                MOD_RADIO_REG_20698(pi, AFEDIV_REG0, core,
                                    afediv_outbuf_dac_pu, 0);

                /* Remove dac_div pu and reset overrides */
                MOD_RADIO_REG_20698(pi, AFEDIV_CFG1_OVR, core,
                                    ovr_afediv_dac_div_pu, 0);
                MOD_RADIO_REG_20698(pi, AFEDIV_REG0, core, afediv_dac_div_pu, 0);
                MOD_RADIO_REG_20698(pi, AFEDIV_CFG1_OVR, core,
                                    ovr_afediv_dac_div_reset, 0);
                MOD_RADIO_REG_20698(pi, AFEDIV_REG0, core,
                                    afediv_dac_div_reset, 0);
            }
        } else if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
            FOREACH_CORE(pi, core) {
                /* Remove dac outbuf overrides */
                MOD_RADIO_REG_20704(pi, AFEDIV_CFG1_OVR, core,
                                    ovr_afediv_outbuf_dac_pu, 0);
                MOD_RADIO_REG_20704(pi, AFEDIV_REG0, core,
                                    afediv_outbuf_dac_pu, 0);

                /* Remove dac_div pu and reset overrides */
                MOD_RADIO_REG_20704(pi, AFEDIV_CFG1_OVR, core,
                                    ovr_afediv_dac_div_pu, 0);
                MOD_RADIO_REG_20704(pi, AFEDIV_REG0, core, afediv_dac_div_pu, 0);
                MOD_RADIO_REG_20704(pi, AFEDIV_CFG1_OVR, core,
                                    ovr_afediv_dac_div_reset, 0);
                MOD_RADIO_REG_20704(pi, AFEDIV_REG0, core,
                                    afediv_dac_div_reset, 0);
            }
        } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
            FOREACH_CORE(pi, core) {
                /* Remove dac outbuf overrides */
                MOD_RADIO_REG_20707(pi, AFEDIV_CFG1_OVR, core,
                                    ovr_afediv_outbuf_dac_pu, 0);
                MOD_RADIO_REG_20707(pi, AFEDIV_REG0, core,
                                    afediv_outbuf_dac_pu, 0);

                /* Remove dac_div pu and reset overrides */
                MOD_RADIO_REG_20707(pi, AFEDIV_CFG1_OVR, core,
                                    ovr_afediv_dac_div_pu, 0);
                MOD_RADIO_REG_20707(pi, AFEDIV_REG0, core, afediv_dac_div_pu, 0);
                MOD_RADIO_REG_20707(pi, AFEDIV_CFG1_OVR, core,
                                    ovr_afediv_dac_div_reset, 0);
                MOD_RADIO_REG_20707(pi, AFEDIV_REG0, core,
                                    afediv_dac_div_reset, 0);
            }
        } else if (ACMAJORREV_131(pi->pubpi->phy_rev)) {
            FOREACH_CORE(pi, core) {
                /* Remove dac outbuf overrides */
                MOD_RADIO_REG_20710(pi, AFEDIV_CFG1_OVR, core,
                                    ovr_afediv_outbuf_dac_pu, 0);
                MOD_RADIO_REG_20710(pi, AFEDIV_REG0, core,
                                    afediv_outbuf_dac_pu, 0);

                /* Remove dac_div pu and reset overrides */
                MOD_RADIO_REG_20710(pi, AFEDIV_CFG1_OVR, core,
                                    ovr_afediv_dac_div_pu, 0);
                MOD_RADIO_REG_20710(pi, AFEDIV_REG0, core, afediv_dac_div_pu, 0);
                MOD_RADIO_REG_20710(pi, AFEDIV_CFG1_OVR, core,
                                    ovr_afediv_dac_div_reset, 0);
                MOD_RADIO_REG_20710(pi, AFEDIV_REG0, core,
                                    afediv_dac_div_reset, 0);
            }
        }
    }
    /* Restore PA reg value after reseq setting */
    FOREACH_CORE(pi, core) {
        MOD_PHYREGCE(pi, RfctrlIntc,
                core, ext_5g_papu, restore_ext_5g_papu[core]);
        MOD_PHYREGCE(pi, RfctrlIntc,
                core, override_ext_pa, restore_override_ext_pa[core]);
        MOD_PHYREGCE(pi, RfctrlIntc, core, tr_sw_tx_pu, 0);
        MOD_PHYREGCE(pi, RfctrlIntc, core, tr_sw_rx_pu, 0);
        MOD_PHYREGCE(pi, RfctrlIntc, core, override_tr_sw, 0);
    }
}

void
phy_ac_chanmgr_hwobss(phy_ac_chanmgr_info_t *chanmgri, bool enable_hwobss)
{
    phy_info_t *pi = chanmgri->pi;
    if (ACMAJORREV_33(pi->pubpi->phy_rev)) {
        if (enable_hwobss) {
            WRITE_PHYREG(pi, obss_control, 0x7800);
            WRITE_PHYREG(pi, drop20sCtrl1, 0x0b0); /* drop2nd needed for obss */
            WRITE_PHYREG(pi, drop20sCtrl2, 0x37f);
            WRITE_PHYREG(pi, drop20sCtrl3, 0x3340);
            /* use table based AGC for HT header */
            WRITE_PHYREG(pi, TableBasedAGCcntrlA, 0x1c84);
        } else {
        /* disable obss */
            WRITE_PHYREG(pi, obss_control, 0x7000);
        }
    } else if (ACMAJORREV_47_51_129_130_131(pi->pubpi->phy_rev)) {
        if (enable_hwobss) {
            WRITE_PHYREG(pi, obss_control, 0x7800);
            WRITE_PHYREG(pi, drop20sCtrl1, 0x80c4); /* drop2nd needed for obss */
            WRITE_PHYREG(pi, drop20sCtrl2, 0x37f);
            WRITE_PHYREG(pi, drop20sCtrl3, 0x3340);

            /* new improvements 43684B0
             * enable exit on strong SOI
             * but keep abort_sec_en=0 (it would violate the 11ac wifi certification)
             */
            WRITE_PHYREG(pi, obss_param_extra, 0x48);

            if (ACMAJORREV_131(pi->pubpi->phy_rev)) {
            } else {
                /* Need to use normal (non-gated) pktgain clk rate
                 * Otherwise it's too slow for bw 40M
                 */
                MOD_PHYREG(pi, fineRxclockgatecontrol, EncodeGainClkEn4obss, 0);
            }

            /* Increase obss detection threshold to avoid trigger from aci */
            WRITE_PHYREG(pi, obss_param0, 0x200);
            WRITE_PHYREG(pi, obss_param13, 0x280);

            /* skip abort secondary when in UL-OFDMA Rx mode */
            if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
                MOD_PHYREG(pi, obss_filt_override, obss_mit_dis_when_mac_aided, 1);
                MOD_PHYREG(pi, obss_filt_override, obss_mit_dis_when_lesi,      0);
                MOD_PHYREG(pi, obss_filt_override, obss_mit_dis_when_sig_aided, 0);
                MOD_PHYREG(pi, obss_param_extra, obss_abort_sec_skip_mac_aided, 1);
                MOD_PHYREG(pi, obss_param_extra, obss_abort_sec_skip_lesi,      1);
                MOD_PHYREG(pi, obss_param_extra, obss_abort_sec_skip_sig_aided, 1);
            }
        } else {
        /* disable obss */
            WRITE_PHYREG(pi, obss_control, 0x7000);
            WRITE_PHYREG(pi, obss_param_extra, 0x3f);
            WRITE_PHYREG(pi, drop20sCtrl1, 0xc07f);
            WRITE_PHYREG(pi, drop20sCtrl2, 0x64);
            MOD_PHYREG(pi, drop20sCtrl3, phycrs40SpwrTh, 0x64);
        }
        /* MAC will use following values to enable hwobss */
        wlapi_bmac_write_shm(pi->sh->physhim, M_PHYREG_HWOBSS_VAL(pi),
            READ_PHYREG(pi, obss_param_extra));
    } else if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        if (enable_hwobss) {
            WRITE_PHYREG(pi, obss_control, 0x7800);
            WRITE_PHYREG(pi, drop20sCtrl1, 0x0b0); /* drop2nd needed for obss */
            WRITE_PHYREG(pi, drop20sCtrl2, 0x37f);
            WRITE_PHYREG(pi, drop20sCtrl3, 0x3340);

            /* Need to use normal pktgain clk rate
               Otherwise it's too slow for bw 40M
             */
            MOD_PHYREG(pi, fineRxclockgatecontrol, EncodeGainClkEn4obss, 0);

            /* Increase obss detection threshold to avoid trigger from aci */
            WRITE_PHYREG(pi, obss_param0, 0x200);
            WRITE_PHYREG(pi, obss_param13, 0x280);
        } else {
            /* disable obss */
            WRITE_PHYREG(pi, obss_control, 0x7000);
            WRITE_PHYREG(pi, drop20sCtrl1, 0xc07f);
            WRITE_PHYREG(pi, drop20sCtrl2, 0x64);
            MOD_PHYREG(pi, drop20sCtrl3, phycrs40SpwrTh, 0x64);
        }
    }
}

void
wlc_phy_rxcore_setstate_acphy(phy_info_t *pi, uint8 rxcore_bitmask, uint8 phytxchain)
{
    uint16 rfseqCoreActv_DisRx_save;
    uint16 rfseqMode_save;
    uint8 stall_val = 0, core, coremask;
    uint8 orig_rxfectrl1 = 0;
    uint16 classifier_state = 0;
    uint16 edThreshold_save[PHY_MAX_CORES] = {0};
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

    ASSERT((rxcore_bitmask > 0) && (rxcore_bitmask <= 15));
    if (!ACMAJORREV_32(pi->pubpi->phy_rev) &&
        !ACMAJORREV_33(pi->pubpi->phy_rev) &&
        !(ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        if ((stf_shdata->phyrxchain == rxcore_bitmask) && !pi->u.pi_acphy->init &&
                !CCT_INIT(pi->u.pi_acphy) &&
                !pi->u.pi_acphy->chanmgri->data->rxchain_hw_notset)
            return;
    }
    phy_stf_set_phyrxchain(pi->stfi, rxcore_bitmask);
    phy_ac_chanmgr_set_both_txchain_rxchain(pi->u.pi_acphy->chanmgri,
        rxcore_bitmask, stf_shdata->phytxchain);

    if (!pi->sh->clk) {
        pi->u.pi_acphy->chanmgri->data->rxchain_hw_notset = TRUE;
        return;
    }
    pi->u.pi_acphy->chanmgri->data->rxchain_hw_notset = FALSE;

    wlapi_suspend_mac_and_wait(pi->sh->physhim);
    pi->u.pi_acphy->chanmgri->data->phyrxchain_old = READ_PHYREGFLD(pi, CoreConfig, CoreMask);

    if (ACMAJORREV_4(pi->pubpi->phy_rev) || ACMAJORREV_128(pi->pubpi->phy_rev)) {
#ifdef OCL
        if (PHY_OCL_ENAB(pi->sh->physhim)) {
            if ((pi->u.pi_acphy->chanmgri->data->phyrxchain_old == 3) &&
                (stf_shdata->phyrxchain != 3)) {
                phy_ocl_disable_req_set(pi, OCL_DISABLED_SISO,
                                            TRUE, WLC_OCL_REQ_RXCHAIN);
            } else if (stf_shdata->phyrxchain == 3) {
                phy_ocl_disable_req_set(pi, OCL_DISABLED_SISO,
                                            FALSE, WLC_OCL_REQ_RXCHAIN);
            }
        }
#endif /* OCL */
    }

    if (ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev) ||
        ACMAJORREV_4(pi->pubpi->phy_rev) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        ACMAJORREV_GE37(pi->pubpi->phy_rev)) {
        /* Disable classifier */
        classifier_state = READ_PHYREG(pi, ClassifierCtrl);
        phy_rxgcrs_sel_classifier(pi, 4);

        /* Disable stalls and hold FIFOs in reset */
        stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
        orig_rxfectrl1 = READ_PHYREGFLD(pi, RxFeCtrl1, soft_sdfeFifoReset);
        ACPHY_DISABLE_STALL(pi);
        MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, 1);
    }

    /* Save Registers */
    rfseqCoreActv_DisRx_save = READ_PHYREGFLD(pi, RfseqCoreActv2059, DisRx);
    rfseqMode_save = READ_PHYREG(pi, RfseqMode);

    // Reset PHY. some bad state of inactive cores causes trouble in active cores.
    wlc_phy_resetcca_acphy(pi);

    /* JIRA: CRDOT11ACPHY-909; The fix is to max out the edThreshold to prevent
    * the unused core stuck at wait_energy_drop during coremask setting
    */
    FOREACH_CORE(pi, core) {
        edThreshold_save[core] = READ_PHYREGC(pi, edThreshold, core);
        WRITE_PHYREGC(pi, edThreshold, core, 0xffff);
    }
    /* delay to allow the edThreshold setting take effect */
    OSL_DELAY(10);

    /* Indicate to PHY of the Inactive Core */
    MOD_PHYREG(pi, CoreConfig, CoreMask, rxcore_bitmask);
    /* Indicate to RFSeq of the Inactive Core */
    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, RfseqCoreActv2059, EnRx,
            stf_shdata->hw_phyrxchain);
    } else {
        MOD_PHYREG(pi, RfseqCoreActv2059, EnRx, rxcore_bitmask);
    }

    /* Make sure Rx Chain gets shut off in Rx2Tx Sequence */
    MOD_PHYREG(pi, RfseqCoreActv2059, DisRx, 15);
    /* Make sure Tx Chain doesn't get turned off during this function */
    MOD_PHYREG(pi, RfseqCoreActv2059, EnTx, 0);
    MOD_PHYREG(pi, RfseqMode, CoreActv_override, 1);
    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        acphy_set_lpmode(pi, ACPHY_LP_RADIO_LVL_OPT);
    }

    wlc_phy_force_rfseq_noLoleakage_acphy(pi);

    /* Make TxEn chains point to phytxchain */
    /* Needed for X51A for Assymetric TX /RX mode */
    if (ACMAJORREV_33(pi->pubpi->phy_rev))
        MOD_PHYREG(pi, RfseqCoreActv2059, EnTx,    stf_shdata->hw_phytxchain);
    else
        MOD_PHYREG(pi, RfseqCoreActv2059, EnTx, phytxchain);
    /*  Restore Register */
    MOD_PHYREG(pi, RfseqCoreActv2059, DisRx, rfseqCoreActv_DisRx_save);
    WRITE_PHYREG(pi, RfseqMode, rfseqMode_save);

    if ((pi->sh->interference_mode & ACPHY_ACI_ELNABYPASS) != 0) {
        /* Use the lowest active core for elnabypass on w0 detection */
        coremask = rxcore_bitmask; core = 0;
        while ((coremask & 1) == 0) {
            coremask = coremask >> 1;
            core++;
        }
        MOD_PHYREG(pi, RssiStatusControl, coreSel, core);
    }

    if (ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev) ||
        ACMAJORREV_4(pi->pubpi->phy_rev) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) || ACMAJORREV_GE37(pi->pubpi->phy_rev)) {
        if (pi->pubpi->phy_rev >= 33) {
            /* Reset PHY */
            wlc_phy_resetcca_acphy(pi);
        }

        /* Restore FIFO reset and Stalls */
        ACPHY_ENABLE_STALL(pi, stall_val);
        MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, orig_rxfectrl1);
        OSL_DELAY(1);

        /* Restore classifier */
        WRITE_PHYREG(pi, ClassifierCtrl, classifier_state);
        OSL_DELAY(1);

        if (pi->pubpi->phy_rev < 33)
            wlc_phy_resetcca_acphy(pi);
    }

    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        /* 4349 Channel Smoothing related changes */
        if ((phy_get_phymode(pi) == PHYMODE_MIMO) && (stf_shdata->phyrxchain == 0x3)) {
            MOD_PHYREG(pi, chnsmCtrl1, disable_2rx_nvar_calc, 0x0);
        } else {
            MOD_PHYREG(pi, chnsmCtrl1, disable_2rx_nvar_calc, 0x1);
        }
    }
    /* Restore edThreshold */
    FOREACH_CORE(pi, core) {
        WRITE_PHYREGC(pi, edThreshold, core, edThreshold_save[core]);
    }

    wlc_phy_set_sdadc_pd_val_per_core_acphy(pi);
    /* fix to make sure ed_crs does not fire on the inactive core */
    /* RB: http://wlan-rb.sj.broadcom.com/r/105578/ */
    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        if (rxcore_bitmask == 1) {
            WRITE_PHYREG(pi, ed_crsEn, 0x00f);
        }
        if (rxcore_bitmask == 2) {
            WRITE_PHYREG(pi, ed_crsEn, 0x0f0);
        }
        if (rxcore_bitmask == 3) {
            WRITE_PHYREG(pi, ed_crsEn, 0x0ff);
        }
    }

    /* To enable TSSI clk to make it indepenant to rxcore setting */
    if ((ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        MOD_PHYREG(pi, RxFeCtrl1, forceSdFeClkEn, phytxchain);
    }

    wlc_phy_set_regtbl_on_rxchain(pi, TRUE);

    wlapi_enable_mac(pi->sh->physhim);
}

void
wlc_phy_update_rxchains(wlc_phy_t *pih, uint8 *rxcore_bitmask, uint8 *txcore_bitmask,
        uint8 phyrxchain, uint8 phytxchain)
{
    phy_info_t *pi = (phy_info_t*)pih;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

    /* Local copy of phyrxchains before overwrite */
    *rxcore_bitmask = 0;
    /* Local copy of EnTx bits from RfseqCoreActv.EnTx */
    *txcore_bitmask = 0;

    if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        /* Save and overwrite Rx chains */
        *rxcore_bitmask = stf_shdata->phyrxchain;
        *txcore_bitmask = READ_PHYREGFLD(pi, RfseqCoreActv2059, EnTx);
        phy_stf_set_phyrxchain(pi->stfi, stf_shdata->hw_phyrxchain);
        wlc_phy_rxcore_setstate_acphy(pi, stf_shdata->hw_phyrxchain,
            phytxchain);
    } else if (!PHY_COREMASK_SISO(pi->pubpi->phy_coremask) &&
        (phy_get_phymode(pi) != PHYMODE_RSDB)) {
        /* Save and overwrite Rx chains */
        wlapi_suspend_mac_and_wait(pi->sh->physhim);
        *rxcore_bitmask = stf_shdata->phyrxchain;
        *txcore_bitmask = READ_PHYREGFLD(pi, RfseqCoreActv2059, EnTx);
        wlapi_enable_mac(pi->sh->physhim);
        wlc_phy_rxcore_setstate_acphy(pi, phyrxchain,
            phytxchain);
    }
}

void
wlc_phy_restore_rxchains(wlc_phy_t *pih, uint8 enRx, uint8 enTx)
{
    phy_info_t *pi = (phy_info_t*)pih;

    if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        /* Restore Rx chains */
        wlc_phy_rxcore_setstate_acphy(pi, enRx, enTx);
        MOD_PHYREG(pi, RfseqCoreActv2059, EnTx, enTx);
    } else if (!PHY_COREMASK_SISO(pi->pubpi->phy_coremask) &&
        (phy_get_phymode(pi) != PHYMODE_RSDB)) {
        /* Restore Rx chains */
        wlc_phy_rxcore_setstate_acphy(pi, enRx, enTx);
    }
}

uint8
wlc_phy_rxcore_getstate_acphy(wlc_phy_t *pih)
{
    uint16 rxen_bits;
    phy_info_t *pi = (phy_info_t*)pih;

    rxen_bits = READ_PHYREGFLD(pi, RfseqCoreActv2059, EnRx);

    ASSERT(phy_stf_get_data(pi->stfi)->phyrxchain == rxen_bits);

    return ((uint8) rxen_bits);
}

bool
wlc_phy_is_scan_chan_acphy(phy_info_t *pi)
{
    return (SCAN_RM_IN_PROGRESS(pi) &&
            (pi->interf->home_chanspec != pi->radio_chanspec));
}

void
wlc_phy_resetcca_acphy(phy_info_t *pi)
{
    uint16 phy_ctl_reg_val = 0;
    uint16 clkgatests_reg_val = 0;
    uint8 stall_val = 0;

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    /* SAVE PHY_CTL value */
    phy_ctl_reg_val = R_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi));
    /* MAC should be suspended before calling this function */
    ASSERT((R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC) == 0);

    /* If MACPhy clock not enabled (Bit1), wait for 1us */
    if ((phy_ctl_reg_val & 0x2) == 0) {
        OSL_DELAY(1);
    }

    if (PHY_MAC_REV_CHECK(pi, 36)) {
        /* Save ClkGateSts register */
        clkgatests_reg_val = R_REG(pi->sh->osh, D11_CLK_GATE_STS(pi));

        /* Set ForceMacPhyClockRequest bit in ClkGateSts register : SWWLAN-101393 */
        W_REG(pi->sh->osh, D11_CLK_GATE_STS(pi),
            (uint16)(clkgatests_reg_val | (1 << 4)));
    }

    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, sampleCmd, enable, 1);
    }

    /* bilge count sequence fix */
    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        wlapi_bmac_phyclk_fgc(pi->sh->physhim, ON);

        MOD_PHYREG(pi, BBConfig, resetCCA, 1);
        OSL_DELAY(1);
        if (!TINY_RADIO(pi) && !ACMAJORREV_128(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, RxFeCtrl1, rxfe_bilge_cnt, 0);
            OSL_DELAY(1);
        }
        MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, 1);
        OSL_DELAY(1);
        wlapi_bmac_phyclk_fgc(pi->sh->physhim, OFF);
        OSL_DELAY(1);
        MOD_PHYREG(pi, BBConfig, resetCCA, 0);
        OSL_DELAY(1);

        SPINWAIT(READ_PHYREGFLD(pi, RfseqStatus0, reset2rx), ACPHY_SPINWAIT_RESET2RX);
        if (READ_PHYREGFLD(pi, RfseqStatus0, reset2rx)) {
            PHY_FATAL_ERROR_MESG((" %s: SPINWAIT ERROR : Reset to Rx failed \n",
            __FUNCTION__));
            PHY_FATAL_ERROR(pi, PHY_RC_RESET2RX_FAILED);
        }
        MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, 0);
    } else if (IS_4364_3x3(pi)) {
        /* Force gated clocks on */
        wlapi_bmac_phyclk_fgc(pi->sh->physhim, ON);
        W_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi), (uint16)0x6); /* set reg(PHY_CTL) 0x6 */

        /* Disable Stalls */
        stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
        if (stall_val == 0)
            ACPHY_DISABLE_STALL(pi);

        /* Hold FIFO's in Reset */
        MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, 1);
        OSL_DELAY(1);

        /* Do the Reset */
        MOD_PHYREG(pi, BBConfig, resetCCA, 1);
        OSL_DELAY(1);
        MOD_PHYREG(pi, BBConfig, resetCCA, 0);

        /* Wait for reset2rx finish, which is triggered by resetcca in hw */
        SPINWAIT(READ_PHYREGFLD(pi, RfseqStatus0, reset2rx), ACPHY_SPINWAIT_RESET2RX);
        if (READ_PHYREGFLD(pi, RfseqStatus0, reset2rx)) {
            PHY_FATAL_ERROR_MESG((" %s: SPINWAIT ERROR : Reset to Rx failed \n",
            __FUNCTION__));
            PHY_FATAL_ERROR(pi, PHY_RC_RESET2RX_FAILED);
        }

        /* Make sure pktproc came out of reset */
        SPINWAIT((READ_PHYREGFLD(pi, pktprocdebug, pktprocstate) == 0),
                ACPHY_SPINWAIT_PKTPROC_STATE);
        if (READ_PHYREGFLD(pi, pktprocdebug, pktprocstate) == 0) {
            PHY_FATAL_ERROR_MESG((" %s: SPINWAIT ERROR : PKTPROC was in PKTRESET \n",
            __FUNCTION__));
            PHY_FATAL_ERROR(pi, PHY_RC_PKTPROC_RESET_FAILED);
        }

        /* Undo Stalls and SDFEFIFO Reset */
        MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, 0);
        ACPHY_ENABLE_STALL(pi, stall_val);
        OSL_DELAY(1);

        /* Force gated clocks off */
        W_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi), (uint16)0x2); /* set reg(PHY_CTL) 0x2 */
        wlapi_bmac_phyclk_fgc(pi->sh->physhim, OFF);
    } else if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        wlapi_bmac_phyclk_fgc(pi->sh->physhim, ON);
        OSL_DELAY(1);

        /* # force gated clock on */
        W_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi), (uint16)0x6); /* set reg(PHY_CTL) 0x6 */
        OSL_DELAY(1);

        wlc_phy_toggle_resetcca_hwbit_acphy(pi);

        W_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi), (uint16)0x2); /* set reg(PHY_CTL) 0x2 */

        wlapi_bmac_phyclk_fgc(pi->sh->physhim, OFF);
    } else {
        wlapi_bmac_phyclk_fgc(pi->sh->physhim, ON);

        /* # force gated clock on */
        W_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi), (uint16)0x6); /* set reg(PHY_CTL) 0x6 */
        W_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi), (uint16)0xe); /* MacPhyResetCCA = 1 */
        OSL_DELAY(1);
        W_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi), (uint16)0x6); /* MacPhyResetCCA = 0 */
        W_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi), (uint16)0x2); /* set reg(PHY_CTL) 0x2 */

        wlapi_bmac_phyclk_fgc(pi->sh->physhim, OFF);
    }

    /* wait for reset2rx finish, which is triggered by resetcca in hw */
    OSL_DELAY(2);

    /* Restore PHY_CTL register */
    W_REG(pi->sh->osh, D11_PSM_PHY_CTL(pi), phy_ctl_reg_val);

    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, sampleCmd, enable, 0);
    }

    if (PHY_MAC_REV_CHECK(pi, 36)) {
        /* Restore ClkGateSts register */
        W_REG(pi->sh->osh, D11_CLK_GATE_STS(pi), clkgatests_reg_val);
    }

    if (ACMAJORREV_4(pi->pubpi->phy_rev)|| ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        MOD_PHYREG(pi, RfseqMode, Trigger_override, 0);
    }
}

void
wlc_phy_toggle_resetcca_hwbit_acphy(phy_info_t *pi)
{
    uint32 pll;

    bool reduce_clk_freq = FALSE;

    /* AVS: Chips which need reduced clock freq during resertcca toggle */
    if (ACMAJORREV_129(pi->pubpi->phy_rev) || ACMAJORREV_GE131(pi->pubpi->phy_rev))
        reduce_clk_freq = TRUE;

    /* Backend, BP & dot11mac clock = divide freq */
    if (reduce_clk_freq)
        phy_utils_pmu_bbpll_freq_switch(pi, &pll, 0);

    /* TOGGLE HW resetcca bit */
    MOD_PHYREG(pi, BBConfig, resetCCA, 1);
    OSL_DELAY(1);
    MOD_PHYREG(pi, BBConfig, resetCCA, 0);

    /* Add wait-time for:
     * 1) reset2rx finish, which is triggered by resetcca in hw
     * 2) pktproc to come out of reset
     */
    OSL_DELAY(10);

    /* Restore back normal clk freqs */
    if (reduce_clk_freq)
        phy_utils_pmu_bbpll_freq_switch(pi, &pll, 1);
}

/* 20693_dyn_papd_cfg */
static void
wlc_acphy_dyn_papd_cfg_20693(phy_info_t *pi)
{
    uint8 core;
    FOREACH_CORE(pi, core) {
        if (core == 0) {
            MOD_PHYREG(pi, dyn_radioa0, dyn_radio_ovr0, 0);
        } else {
            MOD_PHYREG(pi, dyn_radioa1, dyn_radio_ovr1, 0);
        }
        if (CHSPEC_IS2G(pi->radio_chanspec)) {
            MOD_RADIO_REG_20693(pi, TX_TOP_2G_OVR1_EAST, core,
                ovr_pa2g_idac_cas, 1);
            MOD_RADIO_REG_20693(pi, TX_TOP_2G_OVR1_EAST, core,
                ovr_pa2g_idac_incap_compen_main, 1);
            MOD_RADIO_REG_20693(pi, TX_TOP_2G_OVR1_EAST, core,
                ovr_pa2g_idac_main, 1);
        } else {
            MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR3, core,
                ovr_pa5g_idac_cas, 1);
            MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR2, core,
                ovr_pa5g_idac_incap_compen_main, 1);
            MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR2, core,
                ovr_pa5g_idac_main, 1);
        }
    }
}

static void
wlc_phy_set_bias_ipa_as_epa_acphy_20693(phy_info_t *pi, uint8 core)
{
    MOD_RADIO_REG_20693(pi, SPARE_CFG2, core,
        pa2g_bias_bw_main, 0);
    MOD_RADIO_REG_20693(pi, SPARE_CFG2, core,
        pa2g_bias_bw_cas, 0);
    MOD_RADIO_REG_20693(pi, SPARE_CFG2, core,
        pa2g_bias_bw_pmos, 0);
    MOD_RADIO_REG_20693(pi, TX_TOP_2G_OVR1_EAST, core,
        ovr_pa2g_idac_main, 1);
    MOD_RADIO_REG_20693(pi, PA2G_IDAC1, core,
        pa2g_idac_main, 0x24);
    MOD_RADIO_REG_20693(pi, TX_TOP_2G_OVR1_EAST, core,
        ovr_pa2g_idac_cas, 1);
    MOD_RADIO_REG_20693(pi, PA2G_IDAC1, core,
        pa2g_idac_cas, 0x22);
    MOD_RADIO_REG_20693(pi, TX_TOP_2G_OVR1_EAST, core,
        ovr_pa2g_idac_incap_compen_main, 1);
    MOD_RADIO_REG_20693(pi, PA2G_INCAP, core,
        pa2g_idac_incap_compen_main, 0x2d);
    MOD_RADIO_REG_20693(pi, TX_TOP_2G_OVR1_EAST, core,
        ovr_mx2g_idac_bbdc, 1);
    MOD_RADIO_REG_20693(pi, TXMIX2G_CFG6, core,
        mx2g_idac_bbdc, 0x1c);
    MOD_RADIO_REG_20693(pi, TXMIX2G_CFG2, core,
        mx2g_idac_cascode, 0x13);
}

void wlc_phy_radio20693_sel_logen_mode(phy_info_t *pi)
{
    uint16 phymode = phy_get_phymode(pi);
#ifdef WL_AIR_IQ
    if (phymode == PHYMODE_BGDFS) {
        wlc_phy_radio20693_setup_logen_3plus1(pi, pi->radio_chanspec,
                pi->u.pi_acphy->chanmgri->radio_chanspec_sc);
        return;
    }
#endif /* WL_AIR_IQ */
    if (CHSPEC_IS2G(pi->radio_chanspec)) {
        wlc_phy_radio20693_sel_logen_5g_mode(pi, 5);
        wlc_phy_radio20693_sel_logen_2g_mode(pi, 0);
    } else {
        wlc_phy_radio20693_sel_logen_2g_mode(pi, 2);
        if (phymode == PHYMODE_BGDFS) {
          wlc_phy_radio20693_sel_logen_5g_mode(pi, 2);
        } else if (PHY_AS_80P80(pi, pi->radio_chanspec)) {
          wlc_phy_radio20693_sel_logen_5g_mode(pi, 1);
        } else if (CHSPEC_IS160(pi->radio_chanspec)) {
          wlc_phy_radio20693_sel_logen_5g_mode(pi, 0);
          ASSERT(0); // FIXME
        } else {
          wlc_phy_radio20693_sel_logen_5g_mode(pi, 0);
        }
    }
}

#ifdef WL_AIR_IQ
static void
wlc_phy_radio20693_set_tia(phy_info_t *pi, uint8 core, bool enable)
{
    MOD_RADIO_REG_20693(pi, RX_BB_2G_OVR_EAST, core, ovr_tia_pwrup_resstring, 1);
    MOD_RADIO_REG_20693(pi, RX_BB_2G_OVR_EAST, core, ovr_tia_amp1_pwrup, 1);
    MOD_RADIO_REG_20693(pi, RX_BB_2G_OVR_EAST, core, ovr_tia_pwrup_amp2, 1);
    MOD_RADIO_REG_20693(pi, RX_BB_2G_OVR_EAST, core, ovr_tia_amp2_bypass, 1);
    MOD_RADIO_REG_20693(pi, RX_BB_2G_OVR_EAST, core, ovr_tia_enable_st1, 1);
    MOD_RADIO_REG_20693(pi, RX_BB_2G_OVR_EAST, core, ovr_tia_offset_dac_pwrup, 1);
    MOD_RADIO_REG_20693(pi, TIA_CFG6, core, tia_pwrup_resstring, enable);
    MOD_RADIO_REG_20693(pi, TIA_CFG5, core, tia_amp1_pwrup, enable);
    MOD_RADIO_REG_20693(pi, TIA_CFG7, core, tia_pwrup_amp2, 0);
    MOD_RADIO_REG_20693(pi, TIA_CFG1, core, tia_amp2_bypass, 3);
    MOD_RADIO_REG_20693(pi, TIA_CFG1, core, tia_enable_st1, 0);
    MOD_RADIO_REG_20693(pi, TIA_CFG8, core, tia_offset_dac_pwrup, enable);
}
static void
wlc_phy_radio20693_set_rxmix_2g(phy_info_t *pi, uint8 core, bool enable)
{
    bool enable_inv = !enable;
    MOD_RADIO_REG_20693(pi, RX_TOP_2G_OVR_EAST, core, ovr_rxmix2g_lobuf_pu, 1);
    MOD_RADIO_REG_20693(pi, RX_TOP_2G_OVR_EAST2, core, ovr_rxmix2g_pu, 1);
    MOD_RADIO_REG_20693(pi, RX_TOP_2G_OVR_EAST2, core, ovr_rf2g_mix1st_en, 1);
    MOD_RADIO_REG_20693(pi, RXMIX2G_CFG1, core, rxmix2g_lobuf_pu, enable);
    MOD_RADIO_REG_20693(pi, RXMIX2G_CFG1, core, rxmix2g_pu, enable);
    MOD_RADIO_REG_20693(pi, RXMIX2G_CFG1, core, rf2g_mix1st_en, enable_inv);
    MOD_RADIO_REG_20693(pi, LOGEN2G_RCCR, core, rx2g_iqbias_short,  enable);
}
static void
wlc_phy_radio20693_set_lna2_2g(phy_info_t *pi, uint8 core, bool enable)
{
    MOD_RADIO_REG_20693(pi, RX_TOP_2G_OVR_EAST, core, ovr_gm2g_pwrup, 1);
    MOD_RADIO_REG_20693(pi, RX_TOP_2G_OVR_EAST, core, ovr_lna2g_nap, 1);
    MOD_RADIO_REG_20693(pi, LNA2G_CFG2, core, gm2g_pwrup, enable);
    MOD_RADIO_REG_20693(pi, LNA2G_CFG2, core, lna2g_nap, enable);
}
static void
wlc_phy_radio20693_set_lna1_2g(phy_info_t *pi, uint8 core, bool enable)
{
    bool enable_inv = !enable;
    MOD_RADIO_REG_20693(pi, RX_TOP_2G_OVR_EAST2, core, ovr_lna2g_lna1_pu, 1);
    MOD_RADIO_REG_20693(pi, RX_TOP_2G_OVR_EAST2, core, ovr_lna2g_tr_rx_en, 1);
    MOD_RADIO_REG_20693(pi, RX_TOP_2G_OVR_EAST2, core, ovr_lna2g_lna1_out_short_pu, 1);
    MOD_RADIO_REG_20693(pi, LNA2G_CFG1, core, lna2g_lna1_pu, enable);
    MOD_RADIO_REG_20693(pi, LNA2G_CFG1, core, lna2g_tr_rx_en, enable);
    MOD_RADIO_REG_20693(pi, LNA2G_CFG1, core, lna2g_lna1_out_short_pu, enable_inv);
}
static void
wlc_phy_radio20693_set_rx2g(phy_info_t *pi, uint8 core, bool enable)
{
    wlc_phy_radio20693_set_lna1_2g(pi, core, enable);
    wlc_phy_radio20693_set_lna2_2g(pi, core, enable);
    wlc_phy_radio20693_set_rxmix_2g(pi, core, enable);
    MOD_RADIO_REG_20693(pi, RXIQMIX_CFG1, core, rxmix2g_lobuf_pu, enable);
    MOD_RADIO_REG_20693(pi, RXIQMIX_CFG1, core, rxiq2g_coupler_pu, 0);
    MOD_RADIO_REG_20693(pi, LNA2G_CFG1, core, lna2g_lna1_bypass, 0);
    MOD_RADIO_REG_20693(pi, RX_TOP_2G_OVR_EAST2, core, ovr_lna2g_lna1_bypass, 1);
    MOD_RADIO_REG_20693(pi, LNA2G_CFG1, core, lna2g_lna1_bypass_hiip3, 0);
    MOD_RADIO_REG_20693(pi, RXRF2G_CFG2, core, lna2g_epapd_en, 0);
    MOD_RADIO_REG_20693(pi, LNA2G_CFG2, core, gm2g_auxgm_pwrup, 0);
    wlc_phy_radio20693_set_tia(pi, core, enable);
}
static void
wlc_phy_radio20693_set_rxmix_5g(phy_info_t *pi, uint8 core, bool enable)
{
    MOD_RADIO_REG_20693(pi, RX_TOP_5G_OVR, core, ovr_mix5g_en, 1);
    MOD_RADIO_REG_20693(pi, RX_TOP_5G_OVR, core, ovr_rxmix5g_lobuf_en, 1);
    MOD_RADIO_REG_20693(pi, LNA5G_CFG3, core, mix5g_en, enable);
    MOD_RADIO_REG_20693(pi, LNA5G_CFG4, core, mix5g_en_lb, enable);
    MOD_RADIO_REG_20693(pi, LOGEN5G_RCCR, core, logen5g_rx_iqbias_short, enable);
    MOD_RADIO_REG_20693(pi, LOGEN5G_RCCR, core, lobuf_en,  enable);
}
static void
wlc_phy_radio20693_set_lna2_5g(phy_info_t *pi, uint8 core, bool enable)
{
    MOD_RADIO_REG_20693(pi, RX_TOP_5G_OVR, core, ovr_gm5g_pwrup, 1);
    MOD_RADIO_REG_20693(pi, RX_TOP_5G_OVR, core, ovr_lna5g_nap, 1);
    MOD_RADIO_REG_20693(pi, LNA5G_CFG2, core, gm5g_pwrup, enable);
    MOD_RADIO_REG_20693(pi, LNA5G_CFG2, core, lna5g_nap, enable);
}
static void
wlc_phy_radio20693_set_lna1_5g(phy_info_t *pi, uint8 core, bool enable)
{
    bool enable_inv = !enable;
    MOD_RADIO_REG_20693(pi, RX_TOP_5G_OVR, core, ovr_lna5g_lna1_pu, 1);
    MOD_RADIO_REG_20693(pi, RX_TOP_5G_OVR, core, ovr_lna5g_tr_rx_en, 1);
    MOD_RADIO_REG_20693(pi, RX_TOP_5G_OVR, core, ovr_lna5g_lna1_out_short_pu, 1);
    MOD_RADIO_REG_20693(pi, LNA5G_CFG1, core, lna5g_lna1_pu, enable);
    MOD_RADIO_REG_20693(pi, LNA5G_CFG1, core, lna5g_tr_rx_en, enable);
    MOD_RADIO_REG_20693(pi, LNA5G_CFG1, core, lna5g_lna1_out_short_pu, enable_inv);
}
static void
wlc_phy_radio20693_set_rx5g(phy_info_t *pi, uint8 core, bool enable)
{
    wlc_phy_radio20693_set_lna1_5g(pi, core, enable);
    wlc_phy_radio20693_set_lna2_5g(pi, core, enable);
    wlc_phy_radio20693_set_rxmix_5g(pi, core, enable);
    MOD_RADIO_REG_20693(pi, LOGEN5G_EPAPD, core, loopback5g_cal_pu, 0);
    MOD_RADIO_REG_20693(pi, LNA5G_CFG1, core, lna5g_lna1_bypass, 0);
    MOD_RADIO_REG_20693(pi, RX_TOP_5G_OVR, core, ovr_lna5g_lna1_bypass, 1);
    MOD_RADIO_REG_20693(pi, LOGEN5G_EPAPD, core, epapd_en, 0);
    MOD_RADIO_REG_20693(pi, LNA5G_CFG2, core, auxgm_pwrup, 0);
    MOD_RADIO_REG_20693(pi, LOGEN5G_EPAPD, core, rxiq5g_coupler_pu, 0);
    wlc_phy_radio20693_set_tia(pi, core, enable);
}
static void
wlc_phy_radio20693_save_3plus1(phy_info_t *pi, uint8 core, uint8 restore)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
    phy_ac_chanmgr_info_t *chnmgri = pi_ac->chanmgri;
    phy_ac_chanmgr_20693_3plus1_regs_t *r = &chnmgri->regs_save_20693_3plus1;
    uint16 ct;
    uint16 radio_regs[] = {
        RADIO_REG_20693(pi, LNA2G_CFG1, core),
        RADIO_REG_20693(pi, LNA2G_CFG2, core),
        RADIO_REG_20693(pi, LNA5G_CFG1, core),
        RADIO_REG_20693(pi, LNA5G_CFG2, core),
        RADIO_REG_20693(pi, LNA5G_CFG3, core),
        RADIO_REG_20693(pi, LNA5G_CFG4, core),
        RADIO_REG_20693(pi, LOGEN2G_RCCR, core),
        RADIO_REG_20693(pi, LOGEN5G_EPAPD, core),
        RADIO_REG_20693(pi, LOGEN5G_RCCR, core),
        RADIO_REG_20693(pi, RX_BB_2G_OVR_EAST, core),
        RADIO_REG_20693(pi, RXIQMIX_CFG1, core),
        RADIO_REG_20693(pi, RXMIX2G_CFG1, core),
        RADIO_REG_20693(pi, RXRF2G_CFG2, core),
        RADIO_REG_20693(pi, RX_TOP_2G_OVR_EAST2, core),
        RADIO_REG_20693(pi, RX_TOP_2G_OVR_EAST, core),
        RADIO_REG_20693(pi, RX_TOP_5G_OVR, core),
        RADIO_REG_20693(pi, TIA_CFG1, core),
        RADIO_REG_20693(pi, TIA_CFG5, core),
        RADIO_REG_20693(pi, TIA_CFG6, core),
        RADIO_REG_20693(pi, TIA_CFG7, core),
        RADIO_REG_20693(pi, TIA_CFG8, core),
        RADIO_PLLREG_20693(pi, LO2G_LOGEN0_CFG1, 0),
        RADIO_PLLREG_20693(pi, LO2G_LOGEN1_CFG1, 0),
        RADIO_PLLREG_20693(pi, LO2G_VCO_DRV_CFG1, 0),
        RADIO_PLLREG_20693(pi, LO5G_CORE0_CFG1, 0),
        RADIO_PLLREG_20693(pi, LO5G_CORE1_CFG1, 0),
        RADIO_PLLREG_20693(pi, LO5G_CORE2_CFG1, 0)
    };
    ASSERT(chnmgri);
    r = &chnmgri->regs_save_20693_3plus1;
    PHY_TRACE(("%s: saving core=%d cnt=%d.\n", __FUNCTION__, core, r->save_count));
    ASSERT(ARRAYSIZE(radio_regs) == ARRAYSIZE(r->regs));
    if (restore) {
        r->save_count--;
        for (ct = 0; ct < ARRAYSIZE(radio_regs); ct++) {
            phy_utils_write_radioreg(pi, radio_regs[ct], r->regs[ct]);
        }
    } else {
        r->save_count++;
        for (ct = 0; ct < ARRAYSIZE(radio_regs); ct++) {
            r->regs[ct] = _READ_RADIO_REG(pi, radio_regs[ct]);
        }
    }
}
static void
wlc_phy_radio20693_setup_logen_3plus1(phy_info_t *pi, chanspec_t chanspec, chanspec_t chanspec_sc)
{
    int ct;
    uint16 pll_regs_bit_vals_2g_5g[][3] = {
        RADIO_PLLREGC_FLD_20693(pi, LO2G_LOGEN1_CFG1, 0, lo2g_logen1_pu, 0),
        RADIO_PLLREGC_FLD_20693(pi, LO2G_VCO_DRV_CFG1, 0, logen1_main_inv_pu, 1),
        RADIO_PLLREGC_FLD_20693(pi, LO2G_LOGEN1_CFG1, 0, logen1_sel, 1),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE0_CFG1, 0, logen0_pu, 0),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE0_CFG1, 0, core0_gm_pu, 0),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE1_CFG1, 0, core1_gm_pu, 0),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE2_CFG1, 0, core2_gm_pu, 0)
    };
    uint16 pll_regs_bit_vals_5g_2g[][3] = {
        RADIO_PLLREGC_FLD_20693(pi, LO2G_LOGEN0_CFG1, 0, lo2g_logen0_pu, 0),
    };
    if (CHSPEC_IS2G(chanspec_sc)) {
        if (CHSPEC_IS2G(chanspec)) {
            wlc_phy_radio20693_sel_logen_5g_mode(pi, 5);
            wlc_phy_radio20693_sel_logen_2g_mode(pi, 1);
        } else {
            wlc_phy_radio20693_sel_logen_5g_mode(pi, 4);
            wlc_phy_radio20693_sel_logen_2g_mode(pi, 1);
            for (ct = 0; ct < ARRAYSIZE(pll_regs_bit_vals_5g_2g); ct++) {
                phy_utils_mod_radioreg(pi, pll_regs_bit_vals_5g_2g[ct][0],
                    pll_regs_bit_vals_5g_2g[ct][1],
                    pll_regs_bit_vals_5g_2g[ct][2]);
            }
        }
        wlc_phy_radio20693_set_rx5g(pi, 3, FALSE);
        wlc_phy_radio20693_set_rx2g(pi, 3, TRUE);
    } else {
        if (CHSPEC_IS2G(chanspec)) {
            wlc_phy_radio20693_sel_logen_2g_mode(pi, 0);
            for (ct = 0; ct < ARRAYSIZE(pll_regs_bit_vals_2g_5g); ct++) {
                phy_utils_mod_radioreg(pi, pll_regs_bit_vals_2g_5g[ct][0],
                    pll_regs_bit_vals_2g_5g[ct][1],
                    pll_regs_bit_vals_2g_5g[ct][2]);
            }
        } else {
            wlc_phy_radio20693_sel_logen_2g_mode(pi, 2);
        }
        wlc_phy_radio20693_sel_logen_5g_mode(pi, 2);
        wlc_phy_radio20693_set_rx2g(pi, 3, FALSE);
        wlc_phy_radio20693_set_rx5g(pi, 3, TRUE);
    }
}
#endif /* WL_AIR_IQ */
void wlc_phy_radio20693_sel_logen_5g_mode(phy_info_t *pi, int mode)
{
    int ct;
    uint16 logen0_5g_inv_pu_val[] =    {1, 1, 1, 1, 1, 0};
    uint16 logen0_pu_val[]          =    {1, 1, 1, 1, 1, 0};
    uint16 core0_gm_pu_val[]      =    {1, 1, 1, 1, 1, 0};
    uint16 core1_gm_pu_val[]      =    {1, 1, 1, 1, 1, 0};
    uint16 logen1_5g_inv_pu_val[] = {0, 1, 1, 1, 0, 0};
    uint16 logen1_pu_val[]          =    {0, 1, 1, 1, 0, 0};
    uint16 logen1_gm_pu_val[]      = {0, 1, 0, 1, 0, 0};
    uint16 core2_gm_pu_val[]      =    {1, 0, 1, 0, 1, 0};
    uint16 core2_lc_pu_val[]      =    {1, 1, 1, 0, 1, 0};
    uint16 core2_mux_sel_val[]      =    {0, 1, 0, 0, 0, 0};
    uint16 core3_gm_pu_val[]      =    {1, 1, 1, 1, 0, 0};
    //uint16 core3_lc_pu_val[]      =    {1, 1, 1, 1, 0, 0};
    uint16 core3_mux_pu_val[]      = {1, 1, 1, 1, 0, 0};
    uint16 core3_mux_sel_val[]      =    {0, 1, 1, 1, 0, 0};
    uint16 bias0_pu_val[]         =    {1, 1, 1, 1, 1, 0};
    uint16 bias1_pu_val[]         =    {1, 1, 1, 1, 1, 0};
    uint16 bias2_pu_val[]          =    {1, 1, 1, 1, 1, 0};
    uint16 bias3_pu_val[]          = {1, 1, 1, 1, 1, 0};
    uint16 logen1_vco_inv_pu_val[]  = {0, 1, 1, 1, 0, 0};
    uint16 logen1_main_inv_pu_val[] = {0, 1, 1, 1, 0, 0};

    uint16 pll_regs_bit_vals[][3] = {
        RADIO_PLLREGC_FLD_20693(pi, LO2G_VCO_DRV_CFG1, 0, logen0_5g_inv_pu,
        logen0_5g_inv_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE0_CFG1, 0, logen0_pu,
        logen0_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO2G_VCO_DRV_CFG1, 0, logen1_5g_inv_pu,
        logen1_5g_inv_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE1_CFG1, 0, logen1_pu,
        logen1_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE1_CFG1, 0, logen1_gm_pu,
        logen1_gm_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE0_CFG1, 0, core0_gm_pu,
        core0_gm_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE1_CFG1, 0, core1_gm_pu,
        core1_gm_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE2_CFG1, 0, core2_gm_pu,
        core2_gm_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE3_CFG1, 0, core3_gm_pu,
        core3_gm_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE2_CFG1, 0, core2_lc_pu,
        core2_lc_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE2_CFG1, 0, core2_mux_sel,
        core2_mux_sel_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE3_CFG1, 0, core3_lc_pu,
        core3_mux_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE3_CFG1, 0, core3_mux_sel,
        core3_mux_sel_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE0_IDAC2, 0, bias0_pu,
        bias0_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE1_IDAC2, 0, bias1_pu,
        bias1_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE2_IDAC2, 0, bias2_pu,
        bias2_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO5G_CORE3_IDAC2, 0, bias3_pu,
        bias3_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO2G_VCO_DRV_CFG1, 0, logen1_vco_inv_pu,
        logen1_vco_inv_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO2G_VCO_DRV_CFG1, 0, logen1_main_inv_pu,
        logen1_main_inv_pu_val[mode])
    };

    for (ct = 0; ct < ARRAYSIZE(pll_regs_bit_vals); ct++) {
        phy_utils_mod_radioreg(pi, pll_regs_bit_vals[ct][0],
                               pll_regs_bit_vals[ct][1], pll_regs_bit_vals[ct][2]);
    }
}

void wlc_phy_radio20693_sel_logen_2g_mode(phy_info_t *pi, int mode)
{
    int ct;
    uint16 logen0_pu_val[]              = {1, 1, 0};
    uint16 logen1_pu_val[]              = {1, 1, 0};
    uint16 logen1_sel_val[]                = {0, 1, 0};
    uint16 logen1_vco_inv_pu_val[]      = {0, 1, 0};
    uint16 logen1_main_inv_pu_val[]     = {0, 1, 0};
    uint16 logen1_div3_en_val[]         = {0, 1, 0};
    uint16 logen1_div4_en_val[]         = {0, 0, 0};
    uint16 logen1_idac_cklc_bias_val[]  = {0, 3, 0};
    uint16 logen1_idac_cklc_qb_val[]    = {0, 4, 0};

    uint16 pll_regs_bit_vals[][3] = {
        RADIO_PLLREGC_FLD_20693(pi, LO2G_LOGEN0_CFG1, 0, lo2g_logen0_pu,
        logen0_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO2G_LOGEN1_CFG1, 0, lo2g_logen1_pu,
        logen1_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO2G_LOGEN1_CFG1, 0, logen1_sel,
        logen1_sel_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO2G_VCO_DRV_CFG1, 0, logen1_vco_inv_pu,
        logen1_vco_inv_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO2G_VCO_DRV_CFG1, 0, logen1_main_inv_pu,
        logen1_main_inv_pu_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO2G_SPARE0, 0, lo2g_1_div3_en,
        logen1_div3_en_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi, LO2G_SPARE0, 0, lo2g_1_div4_en,
        logen1_div4_en_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi,  LO2G_LOGEN1_IDAC1, 0, logen1_idac_cklc_bias,
        logen1_idac_cklc_bias_val[mode]),
        RADIO_PLLREGC_FLD_20693(pi,  LO2G_LOGEN1_IDAC1, 0, logen1_idac_cklc_qb,
        logen1_idac_cklc_qb_val[mode])
    };

    for (ct = 0; ct < ARRAYSIZE(pll_regs_bit_vals); ct++) {
            phy_utils_mod_radioreg(pi, pll_regs_bit_vals[ct][0],
            pll_regs_bit_vals[ct][1], pll_regs_bit_vals[ct][2]);
    }
}

void wlc_phy_radio20693_afe_clkdistribtion_mode(phy_info_t *pi, int mode)
{
    MOD_RADIO_PLLREG_20693(pi, AFECLK_DIV_CFG1, 0, afeclk_mode, mode);
}

void wlc_phy_radio20693_force_dacbuf_setting(phy_info_t *pi)
{
    uint8 core;

    FOREACH_CORE(pi, core) {
        RADIO_REG_LIST_START
            MOD_RADIO_REG_20693_ENTRY(pi, TX_DAC_CFG5, core, DACbuf_fixed_cap, 0)
            MOD_RADIO_REG_20693_ENTRY(pi, TX_BB_OVR1, core, ovr_DACbuf_fixed_cap, 1)
            MOD_RADIO_REG_20693_ENTRY(pi, TX_DAC_CFG5, core, DACbuf_Cap, 0x6)
            MOD_RADIO_REG_20693_ENTRY(pi, TX_BB_OVR1, core, ovr_DACbuf_Cap, 1)
        RADIO_REG_LIST_EXECUTE(pi, core);
    }

}

/*
 * derive the operating chanspecs from the current configured chanspec specifically
 * dealing with radios that provide 160MHz using consecutive 80+80 operation.
 */
void
phy_ac_chanmgr_get_operating_chanspecs(phy_info_t *pi, chanspec_t chspec[2])
{
    bool is_80p80 = FALSE;
    uint8 core;
    uint16 phymode = phy_get_phymode(pi);
    phy_ac_chanmgr_data_t *data = pi->u.pi_acphy->chanmgri->data;
    BCM_REFERENCE(is_80p80);
    for (core = 0; core < PHY_CORE_MAX; core++) {
        data->core_freq_mapping[core] = PRIMARY_FREQ_SEGMENT;
    }

    /* RSDB family has 80p80, need to handle carefully */
    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        if (phymode == PHYMODE_80P80) {
            chspec[0] = wf_chspec_primary80_chspec(pi->radio_chanspec);
            chspec[1] = wf_chspec_secondary80_chspec(pi->radio_chanspec);
            data->core_freq_mapping[0] = PRIMARY_FREQ_SEGMENT;
            data->core_freq_mapping[1] = SECONDARY_FREQ_SEGMENT;
            is_80p80 = TRUE;
        } else {
            chspec[0] = pi->radio_chanspec;
            chspec[1] = chspec[0];
        }
    } else {
        if (PHY_AS_80P80(pi, pi->radio_chanspec) &&
        (CHSPEC_IS160(pi->radio_chanspec) || CHSPEC_IS8080(pi->radio_chanspec))) {
            chspec[0] = wf_chspec_primary80_chspec(pi->radio_chanspec);
            chspec[1] = wf_chspec_secondary80_chspec(pi->radio_chanspec);
            is_80p80 = TRUE;
        } else {
            chspec[0] = pi->radio_chanspec;
            chspec[1] = 0;
        }
    }

    PHY_INFORM(("wl%d: %s chanspecs (0x%04x, 0x%04x) | %s\n", PI_INSTANCE(pi), __FUNCTION__,
        chspec[0], is_80p80 ? chspec[1] : chspec[0],
        is_80p80 ? "chan bonded" : "not 80p80, single chan"));
}

static void
chanspec_tune_phy_ACMAJORREV_128(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
    bool elna_present = (CHSPEC_IS2G(pi->radio_chanspec)) ? BF_ELNA_2G(pi_ac) :
        BF_ELNA_5G(pi_ac);

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    chanspec_tune_phy_dccal(pi, FALSE);

    if (!SCAN_RM_IN_PROGRESS(pi)) {
        /* Switch Spur Canceller on/off */
        phy_ac_spurcan(pi_ac->rxspuri, !elna_present);

        if (CCT_INIT(pi_ac) || CCT_BW_CHG(pi_ac) || CCT_BAND_CHG(pi_ac)) {
            wlc_phy_smth(pi, pi_ac->chanmgri->acphy_enable_smth,
                pi_ac->chanmgri->acphy_smth_dump_mode);

            /* Enable LESI for 6878 from the NVRAM file */
            if (CHSPEC_IS2G(pi->radio_chanspec)) {
                if (pi_ac->chanmgri->lesi_perband[0] == 1) {
                    phy_ac_rxgcrs_lesi(pi_ac->rxgcrsi, TRUE, 0);
                } else {
                    phy_ac_rxgcrs_lesi(pi_ac->rxgcrsi, FALSE, 0);
                }
            } else {
                if (pi_ac->chanmgri->lesi_perband[1] == 1) {
                    phy_ac_rxgcrs_lesi(pi_ac->rxgcrsi, TRUE, 0);
                } else {
                    phy_ac_rxgcrs_lesi(pi_ac->rxgcrsi, FALSE, 0);
                }
            }
        }
    }
}

static void
chanspec_tune_phy_ACMAJORREV_47(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    chanspec_tune_phy_dccal(pi, FALSE);

    if (CCT_INIT(pi_ac) || CCT_BW_CHG(pi_ac) || CCT_BAND_CHG(pi_ac)) {
        wlc_phy_smth(pi, pi_ac->chanmgri->acphy_enable_smth,
            pi_ac->chanmgri->acphy_smth_dump_mode);
    }

    /* Update classfier (160->20/40/80 or different 160 primary */
    phy_ac_chanmgr_class_rev47(pi);
}

static void
chanspec_tune_phy_ACMAJORREV_129(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
    bool elna_present = (CHSPEC_IS2G(pi->radio_chanspec)) ? BF_ELNA_2G(pi_ac) :
        BF_ELNA_5G(pi_ac);

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    chanspec_tune_phy_dccal(pi, FALSE);

    if (!SCAN_RM_IN_PROGRESS(pi)) {
        /* Switch Spur Canceller on/off */
        phy_ac_spurcan(pi_ac->rxspuri, !elna_present);
    }

    if (CCT_INIT(pi_ac) || CCT_BW_CHG(pi_ac) || CCT_BAND_CHG(pi_ac)) {
        wlc_phy_smth(pi, pi_ac->chanmgri->acphy_enable_smth,
            pi_ac->chanmgri->acphy_smth_dump_mode);
    }

}

static void
chanspec_tune_phy_ACMAJORREV_130(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
    //bool elna_present = ISSIM_ENAB(pi->sh->sih) ? FALSE :
    //    (CHSPEC_IS2G(pi->radio_chanspec)) ? BF_ELNA_2G(pi_ac) :
    //    BF_ELNA_5G(pi_ac);
    /* spurcan is enabled only if the channel is 6G
     * becase of no impact on DFS
     */
    bool enable = CHSPEC_IS6G(pi->radio_chanspec) ? TRUE : FALSE;

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    if (!ISSIM_ENAB(pi->sh->sih)) {
        chanspec_tune_phy_dccal(pi, FALSE);
    }

    if (!SCAN_RM_IN_PROGRESS(pi)) {
        /* Switch Spur Canceller on/off */
        /* FIXME: spur can be enabled, but be off
         * tentatively until further notice (DFS-related)
         */
        phy_ac_spurcan(pi_ac->rxspuri, enable);
    }

#ifdef WLC_TXSHAPER
    if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        phy_ac_txss_txshaper_chan_set(pi->u.pi_acphy->txssi);
    }
#endif /* WLC_TXSHAPER */

    if (CCT_INIT(pi_ac) || CCT_BW_CHG(pi_ac) || CCT_BAND_CHG(pi_ac)) {
        wlc_phy_smth(pi, pi_ac->chanmgri->acphy_enable_smth,
            pi_ac->chanmgri->acphy_smth_dump_mode);
    }

}

static void
chanspec_tune_phy_ACMAJORREV_51_131(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
    bool lesi_enable = FALSE;
    bool elna_present = (CHSPEC_IS2G(pi->radio_chanspec)) ? BF_ELNA_2G(pi_ac) :
        BF_ELNA_5G(pi_ac);

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    chanspec_tune_phy_dccal(pi, FALSE);

    if (!SCAN_RM_IN_PROGRESS(pi)) {
        /* Switch Spur Canceller on/off */
        phy_ac_spurcan(pi_ac->rxspuri, !elna_present);
    }

    if (CCT_INIT(pi_ac) || CCT_BW_CHG(pi_ac) || CCT_BAND_CHG(pi_ac)) {
        wlc_phy_smth(pi, pi_ac->chanmgri->acphy_enable_smth,
            pi_ac->chanmgri->acphy_smth_dump_mode);

        if (ACMAJORREV_131(pi->pubpi->phy_rev)) {
            /* Enable LESI from the NVRAM file */
            lesi_enable = (bool)(pi_ac->chanmgri->lesi_perband[
                CHSPEC_IS2G(pi->radio_chanspec) ? 0 : 1]);
            phy_ac_rxgcrs_lesi(pi_ac->rxgcrsi, lesi_enable, 0);
        }
    }

}

static void
chanspec_tune_phy_ACMAJORREV_32(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
    uint8 core;
    bool elna_present = (CHSPEC_IS2G(pi->radio_chanspec)) ? BF_ELNA_2G(pi_ac) :
        BF_ELNA_5G(pi_ac);

    /* setup DCC parameters */
    if (CCT_INIT(pi_ac) || CCT_BW_CHG(pi_ac) || CCT_BAND_CHG(pi_ac)) {
        if (ACMAJORREV_32(pi->pubpi->phy_rev) && ACMINORREV_0(pi)) {
            /* Disable the old DCC */
            MOD_PHYREG(pi, RfseqTrigger, en_pkt_proc_dcc_ctrl, 0);
            FOREACH_CORE(pi, core) {
                MOD_RADIO_REG_TINY(pi, RX_BB_2G_OVR_EAST, core,
                    ovr_tia_offset_dac, 1);
            }
        } else {
            MOD_PHYREG(pi, RfseqTrigger, en_pkt_proc_dcc_ctrl, 1);
            wlc_tiny_setup_coarse_dcc(pi);
        }
    }

    phy_ac_spurcan(pi_ac->rxspuri, !elna_present);

    if ((CCT_INIT(pi_ac) || CCT_BW_CHG(pi_ac) || CCT_BAND_CHG(pi_ac)) &&
        ACMAJORREV_33(pi->pubpi->phy_rev)) {
        wlc_phy_smth(pi, pi_ac->chanmgri->acphy_enable_smth,
            pi_ac->chanmgri->acphy_smth_dump_mode);
    }

    if (ISSIM_ENAB(pi->sh->sih)) {
        ACPHY_DISABLE_STALL(pi);

        MOD_PHYREG(pi, RxFeCtrl1, swap_iq0, 0x0);
        MOD_PHYREG(pi, RxFeCtrl1, swap_iq1, 0x0);
        MOD_PHYREG(pi, RxFeCtrl1, swap_iq2, 0x0);
        MOD_PHYREG(pi, RxFeCtrl1, swap_iq3, 0x0);
        MOD_PHYREG(pi, Core1TxControl, iqSwapEnable, 0x0);
        MOD_PHYREG(pi, Core2TxControl, iqSwapEnable, 0x0);
        MOD_PHYREG(pi, Core3TxControl, iqSwapEnable, 0x0);
        MOD_PHYREG(pi, Core4TxControl, iqSwapEnable, 0x0);
    }

    wlc_dcc_fsm_reset(pi);

    if (ACMAJORREV_33(pi->pubpi->phy_rev)) {
        if (PHY_AS_80P80(pi, pi->radio_chanspec)) {
            /* Disable MRC SIG QUAL for BW80p80 for abnormal RTL behavior */
            MOD_PHYREG(pi, MrcSigQualControl0, enableMrcSigQual, 0x0);
        } else {
            /* Enable MRC SIG QUAL */
            MOD_PHYREG(pi, MrcSigQualControl0, enableMrcSigQual, 0x1);
        }
    }
}

static void
chanspec_tune_phy_ACMAJORREV_5(phy_info_t *pi)
{
    BCM_REFERENCE(pi);
}

static void
chanspec_tune_phy_ACMAJORREV_4(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

    bool elna_present = (CHSPEC_IS2G(pi->radio_chanspec)) ? BF_ELNA_2G(pi_ac) :
        BF_ELNA_5G(pi_ac);

    /* setup DCC parameters */
    if (CCT_INIT(pi_ac) || CCT_BW_CHG(pi_ac) || CCT_BAND_CHG(pi_ac))
        wlc_tiny_setup_coarse_dcc(pi);

    phy_ac_spurcan(pi_ac->rxspuri, !elna_present);

    if (CCT_INIT(pi_ac) || CCT_BW_CHG(pi_ac) || CCT_BAND_CHG(pi_ac)) {
        wlc_phy_smth(pi, pi_ac->chanmgri->acphy_enable_smth,
            pi_ac->chanmgri->acphy_smth_dump_mode);
    }

    /* 4349A0: in quickturn, disable stalls and swap iq */
    if (ISSIM_ENAB(pi->sh->sih)) {
        ACPHY_REG_LIST_START
            ACPHY_DISABLE_STALL_ENTRY(pi)

            MOD_PHYREG_ENTRY(pi, RxFeCtrl1, swap_iq0, 0x0)
            MOD_PHYREG_ENTRY(pi, RxFeCtrl1, swap_iq1, 0x0)
            MOD_PHYREG_ENTRY(pi, Core1TxControl, iqSwapEnable, 0x0)
        ACPHY_REG_LIST_EXECUTE(pi);

        if (phy_get_phymode(pi) != PHYMODE_RSDB)
            MOD_PHYREG(pi, Core2TxControl, iqSwapEnable, 0x0);
    }

    if (ROUTER_4349(pi)) {
        /* JIRA: SWWLAN-90220
         * DDR frequency of 392MHz is introducing spur at 2432MHz.
         * JIRA: SWWLAN-76882 SWWLAN-90228 affects only RSDB mode
         * Channel    fc   SpurFreq fspur-fc Tone#    AffectedTones (Location in Table)
         *    9    2452 2457.778 5.778    18.4896    18,19
         *    10    2457 2457.778 0.778    2.4896    2,3
         *    11    2462 2457.778 -4.222   -13.5104    -14,-13    (50,51)
         *    12    2467 2457.778 -9.222   -29.5104    -30,-29    (34,35)
         *    5l,9u    2442 2457.778 15.778   50.4896    50,51
         *    6l,10u    2447 2457.778 10.778   34.4896    34,35
         *    7l,11u    2452 2457.778 5.778    18.4896    18,19
         *    8l,12u    2457 2457.778 0.778    2.4896    2,3
         *    9l,13u    2462 2457.778 -4.222   -13.5104    -14,-13    (114,115)
         */
        acphy_router_4349_nvshptbl_t
            router_4349_nvshptbl[ACPHY_NUM_SPUR_CHANS_ROUTER4349] = {
                { 2427, 0,  16, 1, 0, RSDB_SPUR | MIMO_SPUR},
                { 2437, 0,  48, 1, 0, RSDB_SPUR | MIMO_SPUR},
                { 2422, 1,  32, 1, 0, RSDB_SPUR | MIMO_SPUR},
                { 2427, 1,  16, 1, 0, RSDB_SPUR | MIMO_SPUR},
                { 2437, 1, 112, 1, 0, RSDB_SPUR | MIMO_SPUR},
                { 2442, 1,  96, 1, 0, RSDB_SPUR | MIMO_SPUR},
                { 2447, 1,  80, 1, 0, RSDB_SPUR | MIMO_SPUR},
                { 2452, 0,  18, 2, 9, RSDB_SPUR},
                { 2457, 0,   2, 2, 9, RSDB_SPUR},
                { 2462, 0,  50, 2, 9, RSDB_SPUR},
                { 2467, 0,  34, 2, 9, RSDB_SPUR},
                { 2442, 1,  50, 2, 9, RSDB_SPUR},
                { 2447, 1,  34, 2, 9, RSDB_SPUR},
                { 2452, 1,  18, 2, 9, RSDB_SPUR},
                { 2457, 1,   2, 2, 9, RSDB_SPUR},
                { 2462, 1, 114, 2, 9, RSDB_SPUR}
        };
        uint32 NvShpTbl[ACPHY_NSHAPETBL_MAX_TONES_ROUTER4349] = {0};
        uint16 channel = CHSPEC_CHANNEL(pi->radio_chanspec);
        uint16 freq_mhz = (uint16)wf_channel2mhz(channel, WF_CHAN_FACTOR_2_4_G);
        uint8 offset = 0;
        uint8 bw = CHSPEC_IS40(pi->radio_chanspec);
        uint8 cnt, num_tones, i, spur_mode, core;

        /* Reset the entries */
        for (cnt = 0; cnt < ACPHY_NUM_SPUR_CHANS_ROUTER4349; ++cnt) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_NVNOISESHAPINGTBL,
                MIN(ACPHY_NSHAPETBL_MAX_TONES_ROUTER4349,
                router_4349_nvshptbl[cnt].num_tones),
                router_4349_nvshptbl[cnt].offset, 32, NvShpTbl);
        }

        for (cnt = 0; cnt < ACPHY_NUM_SPUR_CHANS_ROUTER4349; ++cnt) {
            /* If the chan is spur affected, update its NvShpTbl */
            if (freq_mhz == router_4349_nvshptbl[cnt].freq &&
                bw == router_4349_nvshptbl[cnt].bw) {
                offset = router_4349_nvshptbl[cnt].offset;
                spur_mode = router_4349_nvshptbl[cnt].spur_mode;
                num_tones = MIN(ACPHY_NSHAPETBL_MAX_TONES_ROUTER4349,
                    router_4349_nvshptbl[cnt].num_tones);

                /* Update the noise variance in the NvShpTbl */
                for (i = 0; i < num_tones; ++i) {
                    NvShpTbl[i] =
                        router_4349_nvshptbl[cnt].nv_val;
                    /* Writing the same Nv for all the cores */
                    FOREACH_CORE(pi, core) {
                        NvShpTbl[i] |= (NvShpTbl[i] << (8 * core));
                    }
                }

                if (phy_get_phymode(pi) == PHYMODE_RSDB) {
                    if (spur_mode & RSDB_SPUR) {
                        wlc_phy_table_write_acphy(pi,
                            ACPHY_TBL_ID_NVNOISESHAPINGTBL, num_tones,
                            offset, 32, NvShpTbl);
                    }
                } else if (phy_get_phymode(pi) == PHYMODE_MIMO) {
                    if (spur_mode & MIMO_SPUR) {
                        wlc_phy_table_write_acphy(pi,
                            ACPHY_TBL_ID_NVNOISESHAPINGTBL, num_tones,
                            offset, 32, NvShpTbl);
                    }
                }
            }
        }

        /* JIRAs: SWWLAN-69184,SWWLAN-70731: Fix for BCC 2-stream
         * 11n/11ac rate failures in 53573
         */
        MOD_PHYREG(pi, DemodSoftreset, demod_reset_on_pktprocreset, 0x1);
        OSL_DELAY(10);
        MOD_PHYREG(pi, DemodSoftreset, demod_reset_on_pktprocreset, 0x0);

        /* JIRA: SWWLAN-87252: Fix for PER Humps at ~-35dBm in 5G 20 20in40 20in80
         * Issue: High-Gain is having PER hump at ~-35dBm
         * Fix: Moving the NbClip Thsh in such a way that Mid gain will be applied
         */
        if (CHSPEC_ISPHY5G6G(pi->radio_chanspec) && !BF_ELNA_5G(pi->u.pi_acphy)) {
            FOREACH_CORE(pi, core) {
                MOD_PHYREGC(pi, FastAgcClipCntTh, core, fastAgcNbClipCntTh,
                    CHSPEC_BW_LE20(pi->radio_chanspec) ? 0x0a :
                    CHSPEC_IS40(pi->radio_chanspec) ? 0x14 : 0x28);
            }
        }
    }

    wlc_dcc_fsm_reset(pi);
}

static void
chanspec_tune_phy_ACMAJORREV_2(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

    if ((!ACMINORREV_0(pi) && !ACMINORREV_2(pi)) &&
        CHSPEC_IS2G(pi->radio_chanspec) && (BF2_2G_SPUR_WAR(pi_ac) == 1)) {
        phy_ac_dssfB(pi_ac->rxspuri, TRUE);
    }

    /* Spur war for 4350 */
    if (BF2_2G_SPUR_WAR(pi_ac) == 1)
        wlc_phy_spurwar_nvshp_acphy(pi, CCT_BW_CHG(pi_ac), TRUE, FALSE);
}

static void
chanspec_tune_phy_ACMAJORREV_0(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

    /* Update ucode settings based on current band/bw */
    if (CCT_INIT(pi_ac) || CCT_BAND_CHG(pi_ac) || CCT_BW_CHG(pi_ac))
        phy_ac_hirssi_set_ucode_params(pi);
}

static void
phy_ac_chanmgr_dccal_force(phy_info_t *pi)
{
    uint8 mac_suspend;

    if (!pi->sh->up) return;
    if (!(ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) return;

    mac_suspend = (R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC);
    if (mac_suspend) wlapi_suspend_mac_and_wait(pi->sh->physhim);

    chanspec_tune_phy_dccal(pi, TRUE);

    if (mac_suspend) wlapi_enable_mac(pi->sh->physhim);
}

static void
chanspec_tune_phy_dccal(phy_info_t *pi, bool force)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    /* force mode was added to do forced dccal at the end of scan, currently not needed */
    if (!SCAN_RM_IN_PROGRESS(pi) || force) {
        /* setup DCC parameters */
        if (CCT_INIT(pi_ac) || force) {
            phy_ac_dccal_init(pi);
            phy_ac_load_gmap_tbl(pi);
        } else if (CCT_BAND_CHG(pi_ac)) {
            /* Load IDAC GMAP table */
            phy_ac_load_gmap_tbl(pi);
        }

        /* Bring chip in known good state before issuing dccal,
         as this function is called in the middle on chansepc_set
        */
        wlc_phy_resetcca_acphy(pi);

        /* Cal DC cal in channel change */
        phy_ac_dccal(pi);
    }
}

int
wlc_phy_femctrl_clb_prio_2g_acphy(phy_info_t *pi, bool set, uint32 val)
{
    int ret = 0;

    if (set) {
        femctrl_clb_majrev_ge40(pi, 1, val);
    } else {
        ret = phy_get_femctrl_clb_prio_2g_acphy(pi);
    }
    return ret;
}

int
wlc_phy_femctrl_clb_prio_5g_acphy(phy_info_t *pi, bool set, uint32 val)
{
    int ret = 0;

    if (set) {
        femctrl_clb_majrev_ge40(pi, 0, val);
    } else {
        ret = phy_get_femctrl_clb_prio_5g_acphy(pi);
    }

    return ret;
}

static void
femctrl_clb_majrev_ge40(phy_info_t *pi, int band_is_2g, int slice)
{

    uint8 core;
    int cur_val, mask;

    #define CLBMASK 0x3ff
    #define CLBCORE0_SHIFT 16
    #define CLBCORE1_SHIFT 20

    FOREACH_CORE(pi, core) {

        if (band_is_2g) {
            mask = pi->u.pi_acphy->sromi->nvram_femctrl_clb.map_2g[slice][core];
            phy_set_femctrl_clb_prio_2g_acphy(pi, slice);
        } else {
            mask = pi->u.pi_acphy->sromi->nvram_femctrl_clb.map_5g[slice][core];
            phy_set_femctrl_clb_prio_5g_acphy(pi, slice);
        }
        if (core == 0) {
            /* clb_swctrl_smask_coresel_ant0 */
            cur_val = si_gci_chipcontrol(pi->sh->sih, CC_GCI_CHIPCTRL_09, 0, 0);
            cur_val &= (CLBMASK<<CLBCORE0_SHIFT);

            if (slice) {
                si_gci_chipcontrol(pi->sh->sih, CC_GCI_CHIPCTRL_09,
                    CLBMASK<<CLBCORE0_SHIFT, ~(mask<<CLBCORE0_SHIFT) & cur_val);
            } else {
                si_gci_chipcontrol(pi->sh->sih, CC_GCI_CHIPCTRL_09,
                    CLBMASK<<CLBCORE0_SHIFT, (mask<<CLBCORE0_SHIFT) | cur_val);
            }
        }

        if (core == 1) {
            /* clb_swctrl_smask_coresel_ant1 */
            cur_val = si_gci_chipcontrol(pi->sh->sih, CC_GCI_CHIPCTRL_10, 0, 0);
            cur_val &= (CLBMASK<<CLBCORE1_SHIFT);

            if (slice) {
                si_gci_chipcontrol(pi->sh->sih, CC_GCI_CHIPCTRL_10,
                    CLBMASK<<CLBCORE1_SHIFT, ~(mask<<CLBCORE1_SHIFT) & cur_val);
            } else {
                si_gci_chipcontrol(pi->sh->sih, CC_GCI_CHIPCTRL_10,
                    CLBMASK<<CLBCORE1_SHIFT, (mask<<CLBCORE1_SHIFT) | cur_val);
            }
        }
    }

}

static void
chanspec_setup_phy_ACMAJORREV_128(phy_info_t *pi)
{
    uint8 mac_suspend = 1;

    mac_suspend = (R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC);
    if (mac_suspend) {
        wlapi_suspend_mac_and_wait(pi->sh->physhim);
    }

    MOD_PHYREG(pi, BT_SwControl, inv_btcx_prisel, 0x1);
    if (mac_suspend) {
        wlapi_enable_mac(pi->sh->physhim);
    }
}

static void
wlc_phy_bphymrc_hwconfig(phy_info_t *pi)
{
    if (ACPHY_bphymrcCtrl(pi->pubpi->phy_rev) != INVALID_ADDRESS) {
        if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
            if (pi->sh->bphymrc_en && CHSPEC_IS2G(pi->radio_chanspec)) {
                MOD_PHYREG(pi, HPFBWovrdigictrl, bphyNoCoreRemap, 1);
                WRITE_PHYREG(pi, bphymrcCtrl, 0x272);
            } else {
                MOD_PHYREG(pi, HPFBWovrdigictrl, bphyNoCoreRemap, 0);
                WRITE_PHYREG(pi, bphymrcCtrl, 0);
            }
        } else {
            PHY_TRACE(("wl%d: %s: BPHY MRC settings are NOT updated\n",
                pi->sh->unit, __FUNCTION__));
        }
    }
}

static void
wlc_phy_tx_precorr_sfo_setup(phy_info_t *pi)
{
    uint32    chanfactor;
    uint32    fc;
    uint8    nf;
    //int chanidx;

    //chanfactor = 2^35/3.2e-6/(fc*e6) = SFO_CHAN_SCALE/fc
    //SFO_CHAN_SCALE = 2^35/3.2
    #define SFO_CHAN_SCALE 10737418240
    // 6GHz channel are alredy defined in wf_channel2mhz
    // src/shared/bcmwifi/src/bcmwifi_channels.c
    fc = wf_channel2mhz(CHSPEC_CHANNEL(pi->radio_chanspec),
                                CHSPEC_IS2G(pi->radio_chanspec) ? WF_CHAN_FACTOR_2_4_G
                                : CHSPEC_IS5G(pi->radio_chanspec) ? WF_CHAN_FACTOR_5_G
                                                                  : WF_CHAN_FACTOR_6_G);

    nf = math_fp_div_64(SFO_CHAN_SCALE, fc, 0, 0, &chanfactor);
    chanfactor = math_fp_round_32(chanfactor, nf);

    MOD_PHYREG(pi, currentChannelSFO, precorr_chanfactor_reg_hi,
                                ((chanfactor & 0x7f0000) >> 16));
    MOD_PHYREG(pi, currentChannelSFOLO, precorr_chanfactor_reg_lo, (chanfactor & 0xffff));

    /* verify the calculation
    // 2G
    for (chanidx = 1; chanidx < 14; chanidx++) {
        fc = chanidx*5+2407;
        nf = math_fp_div_64(SFO_CHAN_SCALE, fc, 0, 0, &chanfactor);
        printf("zd_debug: fc = %d chanfactor = %d nf = %d \n", fc, chanfactor, nf);
        chanfactor = math_fp_round_32(chanfactor, nf);
        printf("zd_debug:chanfactor=%d \n", chanfactor);
    }
    //5G
    for (chanidx = 1; chanidx < 201; chanidx++) {
        fc = chanidx*5+5000;
        nf = math_fp_div_64(SFO_CHAN_SCALE, fc, 0, 0, &chanfactor);
        printf("zd_debug: fc = %d chanfactor = %d nf = %d \n", fc, chanfactor, nf);
        chanfactor = math_fp_round_32(chanfactor, nf);
        printf("zd_debug:chanfactor=%d \n", chanfactor);
    }
    // 6G
    for (chanidx = 1; chanidx < 254; chanidx++) {
        fc = chanidx*5+5940;
        nf = math_fp_div_64(SFO_CHAN_SCALE, fc, 0, 0, &chanfactor);
        printf("zd_debug: fc = %d chanfactor = %d nf = %d \n", fc, chanfactor, nf);
        chanfactor = math_fp_round_32(chanfactor, nf);
        printf("zd_debug:chanfactor=%d \n", chanfactor);
    }
    */
}

static void
chanspec_setup_phy_ACMAJORREV_GE47(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

    if (CCT_INIT(pi_ac) || CCT_BW_CHG(pi_ac) || CCT_BAND_CHG(pi_ac)) {
        /* Initially switch LESI on, if globally enabled in pi_ac->rxgcrsi.
         * NOTE: phy_ac_rxgcrs_lesi() will only enable LESI if the global enable
         *       is on.
         * After the initial setting, enabling/disabling may be controlled by desense.
         */
        phy_ac_rxgcrs_lesi(pi_ac->rxgcrsi, TRUE, 0);

        /* enable ChanSmooth */
        wlc_phy_smth(pi, pi_ac->chanmgri->acphy_enable_smth,
            pi_ac->chanmgri->acphy_smth_dump_mode);
    }
    /* Reset the TxPwrCtrl HW during the setup */
    MOD_PHYREG(pi, TxPwrCtrlCmd, txpwrctrlReset, 1);
    OSL_DELAY(10);
    MOD_PHYREG(pi, TxPwrCtrlCmd, txpwrctrlReset, 0);

    if ((ACMAJORREV_47(pi->pubpi->phy_rev) && ACMINORREV_GE(pi, 3)) ||
            ACMAJORREV_GE130(pi->pubpi->phy_rev)) {
        /* 43684C0 remove the LUT of currentChanneel for 6 GHz channel band support
        http://bca-jira.broadcom.com/browse/CRBCAD11PHY-3880
        ToDo: 6715 is using the same method, add in condition line
        */
        wlc_phy_tx_precorr_sfo_setup(pi);
    } else {
        /* JIRA: CRDOT11ACPHY-3012: The register "currentChannelSFO" has to be set for
        * proper functionality of tx Pre corr
        */
        WRITE_PHYREG(pi, currentChannelSFO, CHSPEC_CHANNEL(pi->radio_chanspec));
    }
}

static void
chanspec_setup_phy_ACMAJORREV_51_131(phy_info_t *pi)
{
    /* Reset the TxPwrCtrl HW during the setup */
    MOD_PHYREG(pi, TxPwrCtrlCmd, txpwrctrlReset, 1);
    OSL_DELAY(10);
    MOD_PHYREG(pi, TxPwrCtrlCmd, txpwrctrlReset, 0);

    if (ACMAJORREV_131(pi->pubpi->phy_rev)) {
        /* remove the LUT of currentChanneel for 6 GHz channel band support
        http://bca-jira.broadcom.com/browse/CRBCAD11PHY-3880
        */
        wlc_phy_tx_precorr_sfo_setup(pi);
    } else {
        /* JIRA: CRDOT11ACPHY-3012: The register "currentChannelSFO" has to be set for
        * proper functionality of tx Pre corr
        */
        WRITE_PHYREG(pi, currentChannelSFO, CHSPEC_CHANNEL(pi->radio_chanspec));
    }
}

static void
chanspec_setup_phy_ACMAJORREV_32(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

    if (CCT_INIT(pi_ac))
        MOD_PHYREG(pi, BT_SwControl, inv_btcx_prisel, 0x1);

    /* Reset the TxPwrCtrl HW during the setup */
    MOD_PHYREG(pi, TxPwrCtrlCmd, txpwrctrlReset, 1);
    OSL_DELAY(10);
    MOD_PHYREG(pi, TxPwrCtrlCmd, txpwrctrlReset, 0);

    // Enable LESI for 4365
    phy_ac_rxgcrs_lesi(pi_ac->rxgcrsi, TRUE, 0);
}

static void
chanspec_setup_phy_ACMAJORREV_5(phy_info_t *pi)
{
    MOD_PHYREG(pi, BT_SwControl, inv_btcx_prisel, 0x1);
}

static void
chanspec_setup_phy_ACMAJORREV_4(phy_info_t *pi)
{

    MOD_PHYREG(pi, BT_SwControl, inv_btcx_prisel, 0x1);

    /* Reset the TxPwrCtrl HW during the setup */
    MOD_PHYREG(pi, TxPwrCtrlCmd, txpwrctrlReset, 1);
    OSL_DELAY(10);
    MOD_PHYREG(pi, TxPwrCtrlCmd, txpwrctrlReset, 0);
}

static void
chanspec_setup_phy_ACMAJORREV_2(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

    MOD_PHYREG(pi, BT_SwControl, inv_btcx_prisel, 0x1);

    if (CCT_INIT(pi_ac) && (ACMINORREV_1(pi) || ACMINORREV_3(pi)) && PHY_ILNA(pi)) {
        si_gci_chipcontrol(pi->sh->sih, CC_GCI_CHIPCTRL_06, CC_GCI_XTAL_BUFSTRG_NFC,
            (0x1 << 12));
    }
}

static void
chanspec_setup_phy_ACMAJORREV_0(phy_info_t *pi)
{
    /* store/clear the hirssi(shmem) info of previous channel */
    if (phy_ac_hirssi_shmem_read_clear(pi)) {
        /* Check for previous channel */
        phy_ac_hirssi_set_timer(pi);
    }
}

static int
phy_ac_chanmgr_get_chanspec_bandrange(phy_type_chanmgr_ctx_t *ctx, chanspec_t chanspec)
{
    phy_ac_chanmgr_info_t *info = (phy_ac_chanmgr_info_t *)ctx;
    phy_info_t *pi = info->pi;
    if (SROMREV(pi->sh->sromrev) < 12) {
        return phy_ac_chanmgr_get_chan_freq_range(pi, chanspec, PRIMARY_FREQ_SEGMENT);
    } else {
        return phy_ac_chanmgr_get_chan_freq_range_srom12(pi, chanspec);
    }
}

static void
chanspec_setup_phy(phy_info_t *pi)
{
    if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
        chanspec_setup_phy_ACMAJORREV_128(pi);
    } else if (ACMAJORREV_51_131(pi->pubpi->phy_rev))
        chanspec_setup_phy_ACMAJORREV_51_131(pi);
    else if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))
        chanspec_setup_phy_ACMAJORREV_GE47(pi);
    else if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev))
        chanspec_setup_phy_ACMAJORREV_32(pi);
    else if (ACMAJORREV_5(pi->pubpi->phy_rev))
        chanspec_setup_phy_ACMAJORREV_5(pi);
    else if (ACMAJORREV_4(pi->pubpi->phy_rev))
        chanspec_setup_phy_ACMAJORREV_4(pi);
    else if (ACMAJORREV_2(pi->pubpi->phy_rev))
        chanspec_setup_phy_ACMAJORREV_2(pi);
    else if (ACMAJORREV_0(pi->pubpi->phy_rev))
        chanspec_setup_phy_ACMAJORREV_0(pi);
    else {
        PHY_ERROR(("wl%d %s: Invalid ACMAJORREV %d!\n",
            PI_INSTANCE(pi), __FUNCTION__, pi->pubpi->phy_rev));
        ASSERT(0);
    }
}

static void
chanspec_setup_cmn(phy_info_t *pi)
{
    uint8 max_rxchain;
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
    bool rxcore_called = FALSE;

    if (CCT_INIT(pi_ac)) {
        phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);

        phy_ac_antdiv_chanspec(pi_ac->antdivi);
        wlc_phy_set_reg_on_reset_acphy(pi);
        wlc_phy_set_tbl_on_reset_acphy(pi);

        /* If any rx cores were disabled before phy_init,
         * disable them again since phy_init enables all rx cores
         * Also make RfseqCoreActv2059.EnTx = phytxchain except
         * for cals where it is set to hw_phytxchain
         */
        max_rxchain =  (1 << pi->pubpi->phy_corenum) - 1;
        if ((stf_shdata->phyrxchain != max_rxchain) ||
            (stf_shdata->hw_phytxchain != max_rxchain)) {
            wlc_phy_rxcore_setstate_acphy(pi, stf_shdata->phyrxchain,
                stf_shdata->phytxchain);
            rxcore_called = TRUE;
        }
    }

    if (!rxcore_called)
        wlc_phy_set_regtbl_on_rxchain(pi, FALSE);

    if (CCT_INIT(pi_ac) || CCT_BAND_CHG(pi_ac)) {
        wlc_phy_set_regtbl_on_band_change_acphy(pi);

#ifdef WL_ETMODE
        if (ET_ENAB(pi)) {
            phy_ac_et(pi);
        }
#endif /* WL_ETMODE */
    }

    if (CCT_INIT(pi_ac) || CCT_BW_CHG(pi_ac))
        wlc_phy_set_regtbl_on_bw_change_acphy(pi);

    chanspec_setup_regtbl_on_chan_change(pi);
    phy_ac_antdiv_regtbl_fc_from_nvram(pi_ac->antdivi);
    chanspec_prefcbs_init(pi);
#ifdef OCL
    if (PHY_OCL_ENAB(pi->sh->physhim)) {
        phy_ocl_disable_req_set(pi, OCL_DISABLED_CHANSWITCH,
                                    TRUE, WLC_OCL_REQ_CHANSWITCH);
    }
#endif /* OCL */
}

static void
chanspec_cleanup(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

    /* Restore FIFO reset and Stalls */
    MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, pi_ac->chanmgri->FifoReset);

    if (pi_ac->c2c_sync_en) {
        phy_ac_chanmgr_core2core_sync_setup(pi->u.pi_acphy->chanmgri, TRUE);
    }

    /* reset RX */
    wlc_phy_resetcca_acphy(pi);

    /* return from Deaf */
    phy_rxgcrs_stay_in_carriersearch(pi->rxgcrsi, FALSE);

    /* clear Chspec Call Trace */
    CCT_CLR(pi_ac);

    /* Clear the chanest dump counter */
    pi->phy_chanest_dump_ctr = 0;
}

/* see chanspec_cleanup which restores some of the setup params */
static void
chanspec_setup(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

    if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        PHY_TRACE(("wl%d: %s chan = %d\n", pi->sh->unit, __FUNCTION__,
            CHSPEC_CHANNEL(pi->radio_chanspec)));
        PHY_CHANLOG(pi, __FUNCTION__, TS_ENTER, 0);
    }

    /* 7271 does not follow this, see HWJUSTY-263 */
    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        /* Disable core2core sync */
        phy_ac_chanmgr_core2core_sync_setup(pi->u.pi_acphy->chanmgri, FALSE);
    }

    /* Hold FIFOs in reset before changing channels */
    pi_ac->chanmgri->FifoReset = READ_PHYREGFLD(pi, RxFeCtrl1, soft_sdfeFifoReset);
    MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, 1);

    /* update corenum and coremask state variables */
    if (ACMAJORREV_4(pi->pubpi->phy_rev) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        phy_ac_update_phycorestate(pi);
    }

    /* BAND CHANGED ? */
    if (CCT_INIT(pi_ac) ||
        (pi_ac->chanmgri->data->curr_band2g != CHSPEC_IS2G(pi->radio_chanspec))) {

        if (!ACMAJORREV_32(pi->pubpi->phy_rev) && !ACMAJORREV_33(pi->pubpi->phy_rev))
            chanspec_setup_hirssi_ucode_cap(pi);

        pi_ac->chanmgri->data->curr_band2g = CHSPEC_IS2G(pi->radio_chanspec);

        /* indicate band change to control flow */
        mboolset(pi_ac->CCTrace, CALLED_ON_BAND_CHG);
    }

    /* BW CHANGED ? */
    if (CCT_INIT(pi_ac) || (pi_ac->curr_bw != CHSPEC_BW(pi->radio_chanspec))) {
        pi_ac->curr_bw = CHSPEC_BW(pi->radio_chanspec);

        /* Set the phy BW as dictated by the chspec (also calls phy_reset) */
        wlapi_bmac_bw_set(pi->sh->physhim, CHSPEC_BW(pi->radio_chanspec));

        /* bw change  do not need a phy_reset when HW_PHYRESET_ON_BW_CHANGE = 0 */
        if (HW_PHYRESET_ON_BW_CHANGE == 1) {
            /* indicate phy reset, follow init path to control flow */
            mboolset(pi_ac->CCTrace, CALLED_ON_INIT);
        } else {
            chanspec_sparereg_war(pi);
        }

        OSL_DELAY(2);

        /* indicate bw change to control flow */
        mboolset(pi_ac->CCTrace, CALLED_ON_BW_CHG);
        if (PHY_AS_80P80(pi, pi_ac->curr_bw) ||
            PHY_AS_80P80(pi, pi->radio_chanspec)) {
            mboolset(pi_ac->CCTrace, CALLED_ON_BW_CHG_80P80);
        }
    }

    /* Some PHY have issue with accessing wide tables */
    chanspec_phy_table_access_war(pi);

    /* Change the band bit. Do this after phy_reset */
    if (CHSPEC_IS2G(pi->radio_chanspec))
        MOD_PHYREG(pi, ChannelControl, currentBand, 0);
    else
        MOD_PHYREG(pi, ChannelControl, currentBand, 1);

}

static void
chanspec_tune_phy(phy_info_t *pi)
{
    if (ACMAJORREV_5(pi->pubpi->phy_rev))
        chanspec_tune_phy_ACMAJORREV_5(pi);
    else if (ACMAJORREV_4(pi->pubpi->phy_rev))
        chanspec_tune_phy_ACMAJORREV_4(pi);
    else if (ACMAJORREV_2(pi->pubpi->phy_rev))
        chanspec_tune_phy_ACMAJORREV_2(pi);
    else if (ACMAJORREV_0(pi->pubpi->phy_rev))
        chanspec_tune_phy_ACMAJORREV_0(pi);
    else if (ACMAJORREV_128(pi->pubpi->phy_rev))
        chanspec_tune_phy_ACMAJORREV_128(pi);
    else if (ACMAJORREV_47(pi->pubpi->phy_rev))
        chanspec_tune_phy_ACMAJORREV_47(pi);
    else if (ACMAJORREV_129(pi->pubpi->phy_rev))
        chanspec_tune_phy_ACMAJORREV_129(pi);
    else if (ACMAJORREV_130(pi->pubpi->phy_rev))
        chanspec_tune_phy_ACMAJORREV_130(pi);
    else if (ACMAJORREV_51_131(pi->pubpi->phy_rev))
        chanspec_tune_phy_ACMAJORREV_51_131(pi);
    else if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev))
        chanspec_tune_phy_ACMAJORREV_32(pi);
    else {
        PHY_ERROR(("wl%d %s: Invalid ACMAJORREV!\n", PI_INSTANCE(pi), __FUNCTION__));
        ASSERT(0);
    }
}

/* ******************** WARs ********************* */

static void
chanspec_phy_table_access_war(phy_info_t *pi)
{
    /* PHY table read reliability WAR.
     * 4365:
     * Latest DVT JIRAS: SWWLAN-154664/SWWLAN-154666/SWWLAN-154668/SWWLAN-154685
     * exposed that '15' is a problematic setting, and further confirmed by RTL
     * group through chipsim simulations. The newest setting '10' is obtained
     * through experiments, i.e.
     *  [15:12]  does not work
     *  [11:1]   works
     *  [0]      hang
     */
    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, TableAccessCnt, TableAccessCnt_ClkIdle,
                CHSPEC_ISLE20(pi->radio_chanspec) ? 10 : 7);
    }
}

static void
chanspec_setup_hirssi_ucode_cap(phy_info_t *pi)
{
    BCM_REFERENCE(pi);
}

static void
chanspec_sparereg_war(phy_info_t *pi)
{
}

static bool
chanspec_papr_enable(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
    bool enable = FALSE;
    int freq;
    uint8 ch = CHSPEC_CHANNEL(pi->radio_chanspec);

    if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
        /* Enable PAPR only for 6710 2G iPA boards */
        enable = (PHY_IPA(pi) && CHSPEC_IS2G(pi->radio_chanspec)) ? TRUE : FALSE;
    } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        enable = FALSE;
    }
    if (!pi_ac->chanmgri->cfg->srom_paprdis) {
        if (PHY_IPA(pi) && ACMAJORREV_2(pi->pubpi->phy_rev) &&
            (ACMINORREV_1(pi) || ACMINORREV_3(pi))) {
            enable = TRUE;
        } else if (!PHY_IPA(pi) && ACMAJORREV_2(pi->pubpi->phy_rev) && (ACMINORREV_1(pi))) {
            const void *chan_info = NULL;
            freq = wlc_phy_chan2freq_acphy(pi, ch, &chan_info);
            if (freq == 2472) {
                enable = FALSE;
            } else {
                enable = TRUE;
            }
        } else if (!PHY_IPA(pi) && ACMAJORREV_128(pi->pubpi->phy_rev)) {
            enable = TRUE;
        }
    }
    if ((ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) &&
        CHSPEC_ISPHY5G6G(pi->radio_chanspec))
        enable = TRUE;
    if (ACMAJORREV_47_130(pi->pubpi->phy_rev) &&
        CHSPEC_IS160(pi->radio_chanspec))
        enable = TRUE;
    return enable;
}

static void
chanspec_tune_rxpath(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

    if (!ACMAJORREV_32(pi->pubpi->phy_rev) &&
        !ACMAJORREV_33(pi->pubpi->phy_rev) &&
        !(ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev) &&
        !(ACMAJORREV_47(pi->pubpi->phy_rev) && !pi->sromi->dssf_dis_ch138))) {
        /* DSSF for 4335C0 & 4345 */
        phy_ac_dssf(pi_ac->rxspuri, TRUE);
    }

    phy_ac_rssi_init_gain_err(pi_ac->rssii);
}

static void
chanspec_tune_txpath(phy_info_t *pi)
{
    uint8 tx_pwr_ctrl_state = PHY_TPC_HW_OFF;
    uint8 tx_pwr_idx_store[PHY_CORE_MAX];
    int freq = 0;
    uint8 ch = CHSPEC_CHANNEL(pi->radio_chanspec);
    uint8 chans[NUM_CHANS_IN_CHAN_BONDING];
    uint8 core;
    uint wf_chan_factor;
    uint32    fc;

    switch (CHSPEC_BAND(pi->radio_chanspec)) {
    case WL_CHANSPEC_BAND_2G:
        wf_chan_factor = WF_CHAN_FACTOR_2_4_G;
        break;
    case WL_CHANSPEC_BAND_5G:
        wf_chan_factor = WF_CHAN_FACTOR_5_G;
        break;
    case WL_CHANSPEC_BAND_6G:
        wf_chan_factor = WF_CHAN_FACTOR_6_G;
        break;
    default:
        ASSERT(0);
        return;
    }
    fc = wf_channel2mhz(ch, wf_chan_factor);

    if (RADIOID_IS(pi->pubpi->radioid, BCM2069_ID)) {
        const void *chan_info = NULL;
        freq = wlc_phy_chan2freq_acphy(pi, ch, &chan_info);
    } else if (RADIOID_IS(pi->pubpi->radioid, BCM20693_ID)) {
        const chan_info_radio20693_pll_t *chan_info_pll;
        const chan_info_radio20693_rffe_t *chan_info_rffe;
        const chan_info_radio20693_pll_wave2_t *pll_tbl_wave2;

        if (ACMAJORREV_33(pi->pubpi->phy_rev)) {
            if (CHSPEC_IS160(pi->radio_chanspec) || CHSPEC_IS8080(pi->radio_chanspec)) {
                wf_chspec_get_80p80_channels(pi->radio_chanspec, chans);
                PHY_INFORM(("wl%d: %s: BW 160MHz chan1 = %d, chan2 = %d\n",
                    pi->sh->unit, __FUNCTION__, chans[0], chans[1]));
            } else {
                chans[0] = CHSPEC_CHANNEL(pi->radio_chanspec);
                chans[1] = 0;
                PHY_TRACE(("wl%d: %s chan = %d\n", pi->sh->unit, __FUNCTION__,
                    chans[0]));
            }
            freq = wlc_phy_chan2freq_20693(pi, chans[0], &chan_info_pll,
                &chan_info_rffe, &pll_tbl_wave2);
        } else {
            freq = wlc_phy_chan2freq_20693(pi, ch, &chan_info_pll, &chan_info_rffe,
                    &pll_tbl_wave2);
        }
    } else if (RADIOID_IS(pi->pubpi->radioid, BCM20698_ID)) {
        const chan_info_radio20698_rffe_t *chan_info;
        freq = wlc_phy_chan2freq_20698(pi, pi->radio_chanspec, &chan_info);
    } else if (RADIOID_IS(pi->pubpi->radioid, BCM20704_ID)) {
        const chan_info_radio20704_rffe_t *chan_info;
        freq = wlc_phy_chan2freq_20704(pi, pi->radio_chanspec, &chan_info);
    } else if (RADIOID_IS(pi->pubpi->radioid, BCM20707_ID)) {
        const chan_info_radio20707_rffe_t *chan_info;
        freq = wlc_phy_chan2freq_20707(pi, pi->radio_chanspec, &chan_info);
    } else if (RADIOID_IS(pi->pubpi->radioid, BCM20708_ID)) {
        const chan_info_radio20708_rffe_t *chan_info;
        freq = wlc_phy_chan2freq_20708(pi, pi->radio_chanspec, &chan_info);
    } else if (RADIOID_IS(pi->pubpi->radioid, BCM20709_ID)) {
        const chan_info_radio20709_rffe_t *chan_info;
        freq = wlc_phy_chan2freq_20709(pi, pi->radio_chanspec, &chan_info);
    } else if (RADIOID_IS(pi->pubpi->radioid, BCM20710_ID)) {
        const chan_info_radio20710_rffe_t *chan_info;
        freq = wlc_phy_chan2freq_20710(pi, pi->radio_chanspec, &chan_info);
    } else {
        ASSERT(0);
    }

#if defined(WLC_TXCAL)
    phy_ac_tssical_set_olpc_threshold(pi);
#endif    /* WLC_TXCAL */

    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev))
        MOD_PHYREG(pi, BT_SwControl, inv_btcx_prisel, 0x1);

    /* set txgain in case txpwrctrl is disabled */
    if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        uint8 phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;
        BCM_REFERENCE(phyrxchain);
        FOREACH_ACTV_CORE(pi, phyrxchain, core) {
                tx_pwr_idx_store[core] = pi->u.pi_acphy->txpwrindex[core];
        }
        FOREACH_ACTV_CORE(pi, phyrxchain, core) {
            pi->u.pi_acphy->txpwrindex[core] = (fc < 5500) ? 48 : 64;
        }
    }
    wlc_phy_txpwr_fixpower_acphy(pi);

    /* Disable TxPwrCtrl */
    tx_pwr_ctrl_state = pi->txpwrctrl;
    wlc_phy_txpwrctrl_enable_acphy(pi, PHY_TPC_HW_OFF);

    /* Set the TSSI visibility limits for 4360 A0/B0 */

    /* Temporary fix for UTF failure for X51 SWWLAN-93602
     * Always set TSSI visibility threshold
     */
    wlc_phy_set_tssisens_lim_acphy(pi, TRUE);

    /* Enable TxPwrCtrl & Restore TxPwrCtrl and TxPwrIdx */
    if (!((ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev) &&
            !ACMAJORREV_130(pi->pubpi->phy_rev))) &&
            ((BOARDFLAGS2(GENERIC_PHY_INFO(pi)->boardflags2) &
            BFL2_TXPWRCTRL_EN) == 0))) {
        wlc_phy_txpwrctrl_enable_acphy(pi, tx_pwr_ctrl_state);
        if (tx_pwr_ctrl_state == PHY_TPC_HW_OFF) {
            uint8 phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;
            BCM_REFERENCE(phyrxchain);
            FOREACH_ACTV_CORE(pi, phyrxchain, core) {
                pi->u.pi_acphy->txpwrindex[core] = tx_pwr_idx_store[core];
            }
        }
    }

#ifdef WLC_TXCAL
    phy_tssical_compute_olpc_idx(pi->tssicali);
#endif /* WLC_TXCAL */
    chanspec_setup_papr(pi, 0, 0);
    if (ACMAJORREV_2(pi->pubpi->phy_rev) && (!PHY_IPA(pi)) &&
            (pi->u.pi_acphy->chanmgri->cfg->srom_txnoBW80ClkSwitch == 0)) {
        wlc_phy_afeclkswitch_sifs_delay(pi);
        if (!(freq == 5210 || freq == 5290) && (CHSPEC_IS80(pi->radio_chanspec)))
            wlc_phy_modify_txafediv_acphy(pi, 9);
        else if (CHSPEC_IS80(pi->radio_chanspec))
            wlc_phy_modify_txafediv_acphy(pi, 6);
    }
}

static void
chanspec_prefcbs_init(phy_info_t *pi)
{
#ifdef ENABLE_FCBS
    int chanidx, chanidx_current;
    chanidx = 0;
    chanidx_current = 0;

    if (IS_FCBS(pi)) {

        chanidx_current = wlc_phy_channelindicator_obtain(pi);

        for (chanidx = 0; chanidx < MAX_FCBS_CHANS; chanidx++) {
            if ((chanidx != chanidx_current) &&
            (!(pi->phy_fcbs.initialized[chanidx]))) {

                wlc_phy_prefcbsinit_acphy(pi, chanidx);

                if (CCT_INIT(pi_ac)) {
                    wlc_phy_set_reg_on_reset_acphy(pi);
                    wlc_phy_set_tbl_on_reset_acphy(pi);
                }

                if (CCT_BAND_CHG(pi_ac))
                    wlc_phy_set_regtbl_on_band_change_acphy(pi);

                if (CCT_BW_CHG(pi_ac))
                    wlc_phy_set_regtbl_on_bw_change_acphy(pi);
            }
        }

        wlc_phy_prefcbsinit_acphy(pi, chanidx_current);
    }
#endif /* ENABLE_FCBS */
}

/* features and WARs enable */
static void
chanspec_fw_enab(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

    /* min_res_mask = 0, max_res_mask = 0, clk_ctl_st = 0 */
    uint32 bbpll_parr_in[3] = {0, 0, 0};

    if (D11REV_IS(pi->sh->corerev, 48)) {
        wlapi_bmac_write_shm(pi->sh->physhim, M_PAPDOFF_MCS(pi),
                             pi_ac->chanmgri->cfg->srom_papdwar);
    }

    if (!ACMAJORREV_32(pi->pubpi->phy_rev) &&
        !ACMAJORREV_33(pi->pubpi->phy_rev) &&
        !ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        /* Toggle */
        chanspec_bbpll_parr(pi_ac->rxspuri, bbpll_parr_in, OFF);
        chanspec_bbpll_parr(pi_ac->rxspuri, bbpll_parr_in, ON);
    } else if (ACMAJORREV_51_131(pi->pubpi->phy_rev) ||
            ACMAJORREV_128(pi->pubpi->phy_rev)) {
        phy_rxspur_change_bbpll(pi);
    }

#if (defined(WLOLPC) && !defined(WLOLPC_DISABLED)) || defined(BCMDBG) || \
    defined(WLTEST)
    chanspec_clr_olpc_dbg_mode(pi_ac->tpci);
#endif /* ((WLOLPC) && !(WLOLPC_DISABLED)) || (BCMDBG) || (WLTEST) */

    /* Enable antenna diversity */
    if (wlc_phy_check_antdiv_enable_acphy(pi) &&
        (CCT_INIT(pi_ac) || CCT_BW_CHG(pi_ac) || CCT_BAND_CHG(pi_ac)) &&
        pi->sh->rx_antdiv) {
        wlc_phy_antdiv_acphy(pi, pi->sh->rx_antdiv);
    }

#ifdef WLC_SW_DIVERSITY
    if (PHYSWDIV_ENAB(pi))
        wlc_phy_swdiv_antmap_init(pi->u.pi_acphy->antdivi);
#endif

    /* 6878 QT BBMULT settings */
    if (ACMAJORREV_128(pi->pubpi->phy_rev) && ISSIM_ENAB(pi->sh->sih)) {
        uint8 ncore_idx;
        uint16 val = 50;

        FOREACH_CORE(pi, ncore_idx) {
            wlc_phy_set_tx_bbmult_acphy(pi, &val, ncore_idx);
        }
    }

#ifdef WL_DSI
    /* Update FCBS dynamic sequence
       This is for DS0
    */
    ds0_radio_seq_update(pi);
#endif /* WL_DSI */
#ifdef OCL
    if (PHY_OCL_ENAB(pi->sh->physhim)) {
        if ((CCT_INIT(pi_ac) || CCT_BW_CHG(pi_ac) || CCT_BAND_CHG(pi_ac))) {
                phy_ac_ocl_config(pi_ac->ocli);
        }
        phy_ocl_disable_req_set(pi, OCL_DISABLED_CHANSWITCH,
                FALSE, WLC_OCL_REQ_CHANSWITCH);
    }
#endif /* OCL */
}

void
wlc_phy_preemption_abort_during_timing_search(phy_info_t *pi, bool enable)
{
    if (enable) {
        ACPHYREG_BCAST(pi, PktAbortSupportedStates, 0x2fff);
    } else {
        ACPHYREG_BCAST(pi, PktAbortSupportedStates, 0x2ffe);
    }
}

void
wlc_phy_chanspec_set_acphy(phy_info_t *pi, chanspec_t chanspec)
{
    chanspec_module_t *module = get_chanspec_module_list();

    PHY_CHANLOG(pi, __FUNCTION__, TS_ENTER, 0);
    /* sync pi->radio_chanspec with incoming chanspec */
    wlc_phy_chanspec_radio_set(pi, chanspec);

    /* CHANSPEC DISPATCH */
    do {
        (*module)(pi);
        ++module;
    } while (*module != NULL);

    PHY_CHANLOG(pi, __FUNCTION__, TS_EXIT, 0);
    /* Note: !!! DO NOT ADD ANYTHING HERE !!!
     * All changes to acphy chanspec should go to respective module list above
     */
}

static void
phy_ac_chanmgr_chanspec_set(phy_type_chanmgr_ctx_t *ctx, chanspec_t chanspec)
{
    phy_ac_chanmgr_info_t *info = (phy_ac_chanmgr_info_t *)ctx;
    phy_info_t *pi = info->pi;

    wlc_phy_chanspec_set_acphy(pi, chanspec);

    /* reset the phynoise_state (/ac/misc/phy_ac_misc) */
    /* RB - http://wlan-rb.sj.broadcom.com/r/105244/ */
    pi->phynoise_state = 0;
}

static void
phy_ac_chanmgr_upd_interf_mode(phy_type_chanmgr_ctx_t *ctx, chanspec_t chanspec)
{
    phy_ac_chanmgr_info_t *info = (phy_ac_chanmgr_info_t *)ctx;
    phy_info_t *pi = info->pi;
    if (pi->sh->interference_mode_override == TRUE) {
        pi->sh->interference_mode = CHSPEC_IS2G(chanspec) ?
                pi->sh->interference_mode_2G_override :
                pi->sh->interference_mode_5G_override;
    } else {
        pi->sh->interference_mode = CHSPEC_IS2G(chanspec) ?
                pi->sh->interference_mode_2G :
                pi->sh->interference_mode_5G;
    }
}

void
wlc_phy_modify_txafediv_acphy(phy_info_t *pi, uint16 a)
{
    uint16 rfseqExtReg_bw40 = 0x5ea;
    uint16 rfseqExtReg_bw80 = 0x7f8;
    uint8    ch = CHSPEC_CHANNEL(pi->radio_chanspec), afe_clk_num = 3, afe_clk_den = 2;
    uint16    b = 640, lb_b = 320;
    uint32    fcw, lb_fcw, tmp_low = 0, tmp_high = 0;
    uint32  deltaphase;
    uint16  deltaphase_lo, deltaphase_hi;
    uint16  farrow_downsamp;
    uint32    fc = wf_channel2mhz(ch, CHSPEC_IS2G(pi->radio_chanspec) ? WF_CHAN_FACTOR_2_4_G
                    : WF_CHAN_FACTOR_5_G);
    uint32 tx_afediv_sel;
    uint32 write_val[2];
    uint8 stall_val, orig_rxfectrl1;

    if ((pi->sh->chippkg == 2) && (fc == 5290))
        return;

    wlapi_suspend_mac_and_wait(pi->sh->physhim);
    phy_utils_phyreg_enter(pi);

    math_uint64_multiple_add(&tmp_high, &tmp_low, a * afe_clk_num * b,
        1 << 23, (fc * afe_clk_den) >> 1);
    math_uint64_divide(&fcw, tmp_high, tmp_low, fc * afe_clk_den);
    wlc_phy_tx_farrow_mu_setup(pi, fcw & 0xffff, (fcw & 0xff0000) >> 16, fcw & 0xffff,
        (fcw & 0xff0000) >> 16);
    math_uint64_multiple_add(&tmp_high, &tmp_low, fc * afe_clk_den,
        1 << 25, 0);
    math_uint64_divide(&lb_fcw, tmp_high, tmp_low, a * afe_clk_num * lb_b);
    deltaphase = (lb_fcw - 33554431) >> 1;
    deltaphase_lo = deltaphase & 0xffff;
    deltaphase_hi = (deltaphase >> 16) & 0xff;
    farrow_downsamp = fc * afe_clk_den / (a * afe_clk_num * lb_b);
    WRITE_PHYREG(pi, lbFarrowDeltaPhase_lo, deltaphase_lo);
    WRITE_PHYREG(pi, lbFarrowDeltaPhase_hi, deltaphase_hi);
    if (a == 9) {
        WRITE_PHYREG(pi, lbFarrowDriftPeriod, 4320);
    } else {
        WRITE_PHYREG(pi, lbFarrowDriftPeriod, 2880);
    }
    MOD_PHYREG(pi, lbFarrowCtrl, lb_farrow_outShift, 0);
    MOD_PHYREG(pi, lbFarrowCtrl, lb_decimator_output_shift, 0);
    MOD_PHYREG(pi, lbFarrowCtrl, lb_farrow_outScale, 1);
    MOD_PHYREG(pi, lbFarrowCtrl, lb_farrow_downsampfactor, farrow_downsamp);
    if (RADIO2069_MAJORREV(pi->pubpi->radiorev) == 2 &&
        !(ISSIM_ENAB(pi->sh->sih))) {

        /* Disable stalls and hold FIFOs in reset */
        stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
        orig_rxfectrl1 = READ_PHYREGFLD(pi, RxFeCtrl1, soft_sdfeFifoReset);

        if (stall_val == 0)
            ACPHY_DISABLE_STALL(pi);

        MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, 1);
        if (a == 9) {
            tx_afediv_sel = (rfseqExtReg_bw40 & ~(0x7 << 14) & 0xfffff) |
                (0x2 << 14);
        } else {
            tx_afediv_sel = (rfseqExtReg_bw80 & ~(0x7 << 14) & 0xfffff) |
                (0x0 << 14);
        }

        write_val[0] = ((tx_afediv_sel & 0xfff) << 20) | tx_afediv_sel;
        write_val[1] = (tx_afediv_sel << 8) | (tx_afediv_sel >> 12);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1, 2, 60,
            write_val);
        wlc_phy_force_rfseq_noLoleakage_acphy(pi);

        /* Restore FIFO reset and Stalls */
        ACPHY_ENABLE_STALL(pi, stall_val);
        MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, orig_rxfectrl1);
    }

    phy_utils_phyreg_exit(pi);
    wlapi_enable_mac(pi->sh->physhim);
}

void
wlc_phy_afeclkswitch_sifs_delay(phy_info_t *pi)
{
    uint16 sifs_rx_tx_tx = 0x3e3e, sifs_nav_tx = 0x23e;
    uint8 stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);

    if (stall_val == 0)
        ACPHY_DISABLE_STALL(pi);

    if (CHSPEC_IS80(pi->radio_chanspec)) {
        uint16 epaPwrupDly;

        W_REG(pi->sh->osh, D11_IFS_SIFS_RX_TX_TX(pi),
              (uint16)(sifs_rx_tx_tx - ((TXDELAY_BW80 *8)<<8 | (TXDELAY_BW80 *8))));
        W_REG(pi->sh->osh, D11_IFS_SIFS_NAV_TX(pi),
              (uint16)(sifs_nav_tx - (TXDELAY_BW80 *8)));
        WRITE_PHYREG(pi, TxRealFrameDelay, 186 + TXDELAY_BW80 * 80);
        WRITE_PHYREG(pi, TxMacIfHoldOff, TXMAC_IFHOLDOFF_DEFAULT + TXDELAY_BW80 * 2);
        WRITE_PHYREG(pi, TxMacDelay, TXMAC_MACDELAY_DEFAULT + TXDELAY_BW80 * 80);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
              16, rfseq_rx2tx_cmd_afeclkswitch);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x70,
              16, rfseq_rx2tx_cmd_afeclkswitch_dly);
        wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x79, 16, &epaPwrupDly);
        epaPwrupDly = epaPwrupDly + TXDELAY_BW80 * 1000/25;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x79,
              16, &epaPwrupDly);
    } else {
        W_REG(pi->sh->osh, D11_IFS_SIFS_RX_TX_TX(pi), sifs_rx_tx_tx);
        W_REG(pi->sh->osh, D11_IFS_SIFS_NAV_TX(pi), sifs_nav_tx);
        WRITE_PHYREG(pi, TxRealFrameDelay, 186);
        WRITE_PHYREG(pi, TxMacIfHoldOff, TXMAC_IFHOLDOFF_DEFAULT);
        WRITE_PHYREG(pi, TxMacDelay, TXMAC_MACDELAY_DEFAULT);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x00,
              16, rfseq_rx2tx_cmd_noafeclkswitch);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 16, 0x70,
              16, rfseq_rx2tx_cmd_noafeclkswitch_dly);
    }
    ACPHY_ENABLE_STALL(pi, stall_val);
}
static void
wlc_phy_ac_lpf_cfg(phy_info_t *pi)
{
    uint16 data1, data2, offset;
    uint8 core;

    FOREACH_CORE(pi, core) {
        /* modify cRB_lpf_ctl_LUT_en_tx_offset to turn off LPF in tx */
        offset = 0x14d + core * 0x10;
        wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, offset, 16, &data1);
        /* 0x2000 is for enaling dac_rc switch for ofdm */
        data2 = (data1 | 0x2000) & 0x29FE;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, offset, 16, &data2);
        /* modify cRB_lpf_sw_en_tx_offset to bypass LPF in tx */
        offset = 0x36b + core * 0x10;
        wlc_phy_table_read_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, offset, 16, &data1);
        /* 0x40 is for enaling dac_rc switch for cck */
        data2 = (data1 | 0x40) & 0xFE53;
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, offset, 16, &data2);
    }
}

uint8 *
BCMRAMFN(get_avvmid_set_36)(phy_info_t *pi, uint16 pdet_range_id, uint16 subband_id)
{
    BCM_REFERENCE(pi);
    return &avvmid_set_maj36[pdet_range_id][subband_id][0];
}

uint8 *
BCMRAMFN(get_avvmid_set_128)(phy_info_t *pi, uint16 pdet_range_id, uint16 subband_id)
{
    BCM_REFERENCE(pi);
    /* 6878 TODO: Temporary WAR to fix AvVmid */
    return &avvmid_set_maj128[pdet_range_id][subband_id][0];
}

void
wlc_phy_tx_farrow_mu_setup(phy_info_t *pi, uint16 MuDelta_l, uint16 MuDelta_u, uint16 MuDeltaInit_l,
    uint16 MuDeltaInit_u)
{
    ACPHYREG_BCAST(pi, TxResamplerMuDelta0l, MuDelta_l);
    ACPHYREG_BCAST(pi, TxResamplerMuDelta0u, MuDelta_u);
    ACPHYREG_BCAST(pi, TxResamplerMuDeltaInit0l, MuDeltaInit_l);
    ACPHYREG_BCAST(pi, TxResamplerMuDeltaInit0u, MuDeltaInit_u);
}

static void
phy_ac_chanmgr_write_tx_farrow_tiny(phy_info_t *pi, chanspec_t chanspec,
    chanspec_t chanspec_sc, int sc_mode)
{
    uint8    ch, afe_clk_num, afe_clk_den, core;
    uint16    a, b;
    uint32    fcw, tmp_low = 0, tmp_high = 0;
    uint32    fc;
    chanspec_t chanspec_sel;
    phy_ac_chanmgr_info_t *chanmgri = pi->u.pi_acphy->chanmgri;
    bool vco_12GHz_in5G = (chanmgri->vco_12GHz &&
        CHSPEC_ISPHY5G6G(pi->radio_chanspec));
    bool  suspend = !(R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC);
    uint8 restore_pa5g_pu[PHY_CORE_MAX];
    uint8 restore_over_pa5g_pu[PHY_CORE_MAX];
    uint8 restore_ext_5g_papu[PHY_CORE_MAX];
    uint8 restore_override_ext_pa[PHY_CORE_MAX];

    if (sc_mode == 1) {
        chanspec_sel = chanspec_sc;
    } else {
        chanspec_sel = chanspec;
    }

    fc = wf_channel2mhz(CHSPEC_CHANNEL(chanspec_sel), CHSPEC_IS2G(chanspec_sel) ?
                        WF_CHAN_FACTOR_2_4_G : WF_CHAN_FACTOR_5_G);

    if ((!suspend) && (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev)))
        wlapi_suspend_mac_and_wait(pi->sh->physhim);
    if (phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->dac_mode == 1) {
        if (CHSPEC_IS20(chanspec_sel)) {
            if ((phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->vcodivmode & 0x1) ||
                vco_12GHz_in5G) {
                a = 16;
            } else if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                ACMAJORREV_33(pi->pubpi->phy_rev)) {
                if (fc <= 5320 && fc >= 5180) {
                    /* dac_div_ratio = 12 */
                    a = 12;
#ifdef PHY_CORE2CORESYC //WAR: core2core requires to fix dac_div_ratio
                } else if (fc <= 5825 && fc >= 5745) {
                    /* dac_div_ratio = 15 */
                    a = 15;
#endif
                } else {
                    /* dac_div_ratio = 16 */
                    a = 16;
                }
            } else
                a = 18;
            b = 160;
        } else if (CHSPEC_IS40(chanspec_sel)) {
            if ((phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->vcodivmode & 0x2) ||
                vco_12GHz_in5G) {
                a = 8;
            } else if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                ACMAJORREV_33(pi->pubpi->phy_rev)) {
#ifdef PHY_CORE2CORESYC
                if ((fc == 5190 || fc == 5230)) {
                    /* dac_div_ratio = 9 */
                    a = 9;
                } else
#endif
                {
                    /* dac_div_ratio = 8 */
                    a = 8;
                }
            } else
                a = 9;
            b = 320;
        } else if (CHSPEC_IS80(chanspec_sel) ||
                PHY_AS_80P80(pi, pi->radio_chanspec)) {
            a = 6;
            if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                ACMAJORREV_33(pi->pubpi->phy_rev)) {
#ifdef PHY_CORE2CORESYC
                if (fc == 5210) {
                    /* dac_div_ratio = 7 */
                    a = 7;
                } else
#endif
                {
                    /* dac_div_ratio = 6 */
                    a = 6;
                }
            }
            b = 640;
        } else if (CHSPEC_IS160(chanspec_sel)) {
            a = 6;
            b = 640;
            ASSERT(0); // FIXME
        } else {
            a = 6;
            b = 640;
        }

        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev)) {
            if (sc_mode == 1) {
                MOD_RADIO_REG_20693(pi, CLK_DIV_OVR1, 3, ovr_afe_div_dac, 1);
                MOD_RADIO_REG_20693(pi, CLK_DIV_CFG1, 3, sel_dac_div, a);
            } else {
                MOD_RADIO_ALLREG_20693(pi, CLK_DIV_OVR1, ovr_afe_div_dac, 1);
                MOD_RADIO_ALLREG_20693(pi, CLK_DIV_CFG1, sel_dac_div, a);
            }
        }
    } else if (phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->dac_mode == 2) {
        a = 6;
        b = 640;
    } else {
        if (CHSPEC_IS80(chanspec_sel) || (CHSPEC_IS8080(chanspec_sel) &&
            !ACMAJORREV_33(pi->pubpi->phy_rev))) {
            a = 6;
            b = 640;
        } else {
            a = 8;
            b = 320;
        }
    }
    if (CHSPEC_IS2G(chanspec_sel)) {
        afe_clk_num = 2;
        afe_clk_den = 3;
    } else {
        afe_clk_num = 3;
        afe_clk_den = 2;
#if !defined(MACOSX)
        if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
            if (RADIOID_IS(pi->pubpi->radioid, BCM20693_ID) &&
                (CHSPEC_IS20(pi->radio_chanspec) ||
                CHSPEC_IS40(pi->radio_chanspec)) &&
                !PHY_IPA(pi) && !ROUTER_4349(pi)) {
                afe_clk_den = 3;
            }
        }
#endif /* MACOSX */
    }
    if (RADIOID_IS(pi->pubpi->radioid, BCM20693_ID)) {
        const uint8 afeclkdiv_arr[] = {2, 16, 4, 8, 3, 24, 6, 12};
        const uint8 dacclkdiv_arr[] = {6, 8, 9, 16, 18, 32, 64, 10};
        const uint8 dacdiv_arr[] = {1, 2, 3, 4};
        const chan_info_radio20693_altclkplan_t *altclkpln = altclkpln_radio20693;
        int row;
        if (ROUTER_4349(pi)) {
            altclkpln = altclkpln_radio20693_router4349;
        }
        row = wlc_phy_radio20693_altclkpln_get_chan_row(pi);

        if ((row >= 0) && (pi->u.pi_acphy->chanmgri->data->fast_adc_en == 0)) {
            a = 1;
            afe_clk_num = afeclkdiv_arr[altclkpln[row].afeclkdiv] *
                dacclkdiv_arr[altclkpln[row].dacclkdiv] *
                dacdiv_arr[altclkpln[row].dacdiv];
            afe_clk_den = CHSPEC_IS2G(pi->radio_chanspec) ? 8 : 4;
        }
    }
    /* bits_in_mu = 23 */
    if (ACMAJORREV_33(pi->pubpi->phy_rev) && PHY_AS_80P80(pi, chanspec)) {
        uint8 chans[NUM_CHANS_IN_CHAN_BONDING];

        wf_chspec_get_80p80_channels(chanspec, chans);

        FOREACH_CORE(pi, core) {
            /* core 0/1: 80L, core 2/3 80U */
            ch = (core <= 1) ? chans[0] : chans[1];
            fc = wf_channel2mhz(ch, WF_CHAN_FACTOR_5_G);
            PHY_INFORM(("%s: core=%d, fc=%d\n", __FUNCTION__, core, fc));

            math_uint64_multiple_add(&tmp_high, &tmp_low, a * afe_clk_num * b,
                1 << 23, (fc * afe_clk_den) >> 1);
            math_uint64_divide(&fcw, tmp_high, tmp_low, fc * afe_clk_den);

            switch (core) {
            case 0:
                WRITE_PHYREG(pi, TxResamplerMuDelta0l, fcw & 0xffff);
                WRITE_PHYREG(pi, TxResamplerMuDelta0u,
                        (fcw & 0xff0000) >> 16);
                WRITE_PHYREG(pi, TxResamplerMuDeltaInit0l, fcw & 0xffff);
                WRITE_PHYREG(pi, TxResamplerMuDeltaInit0u,
                        (fcw & 0xff0000) >> 16);
                break;
            case 1:
                WRITE_PHYREG(pi, TxResamplerMuDelta1l, fcw & 0xffff);
                WRITE_PHYREG(pi, TxResamplerMuDelta1u,
                        (fcw & 0xff0000) >> 16);
                WRITE_PHYREG(pi, TxResamplerMuDeltaInit1l, fcw & 0xffff);
                WRITE_PHYREG(pi, TxResamplerMuDeltaInit1u,
                        (fcw & 0xff0000) >> 16);
                break;
            case 2:
                WRITE_PHYREG(pi, TxResamplerMuDelta2l, fcw & 0xffff);
                WRITE_PHYREG(pi, TxResamplerMuDelta2u,
                        (fcw & 0xff0000) >> 16);
                WRITE_PHYREG(pi, TxResamplerMuDeltaInit2l, fcw & 0xffff);
                WRITE_PHYREG(pi, TxResamplerMuDeltaInit2u,
                        (fcw & 0xff0000) >> 16);
                break;
            case 3:
                WRITE_PHYREG(pi, TxResamplerMuDelta3l, fcw & 0xffff);
                WRITE_PHYREG(pi, TxResamplerMuDelta3u,
                        (fcw & 0xff0000) >> 16);
                WRITE_PHYREG(pi, TxResamplerMuDeltaInit3l, fcw & 0xffff);
                WRITE_PHYREG(pi, TxResamplerMuDeltaInit3u,
                        (fcw & 0xff0000) >> 16);
                break;
            default:
                PHY_ERROR(("wl%d: %s: Max 4 cores only!\n",
                        pi->sh->unit, __FUNCTION__));
                ASSERT(0);
            }
        }
    } else if (CHSPEC_IS8080(chanspec)) {
        FOREACH_CORE(pi, core) {
            if (core == 0) {
                ch = wf_chspec_primary80_channel(chanspec);
                fc = wf_channel2mhz(ch, WF_CHAN_FACTOR_5_G);

                math_uint64_multiple_add(&tmp_high, &tmp_low, a * afe_clk_num * b,
                    1 << 23, (fc * afe_clk_den) >> 1);
                math_uint64_divide(&fcw, tmp_high, tmp_low, fc * afe_clk_den);

                WRITE_PHYREG(pi, TxResamplerMuDelta0l, fcw & 0xffff);
                WRITE_PHYREG(pi, TxResamplerMuDelta0u, (fcw & 0xff0000) >> 16);
                WRITE_PHYREG(pi, TxResamplerMuDeltaInit0l, fcw & 0xffff);
                WRITE_PHYREG(pi, TxResamplerMuDeltaInit0u, (fcw & 0xff0000) >> 16);
            } else if (core == 1) {
                ch = wf_chspec_secondary80_channel(chanspec);
                fc = wf_channel2mhz(ch, WF_CHAN_FACTOR_5_G);

                math_uint64_multiple_add(&tmp_high, &tmp_low, a * afe_clk_num * b,
                    1 << 23, (fc * afe_clk_den) >> 1);
                math_uint64_divide(&fcw, tmp_high, tmp_low, fc * afe_clk_den);

                WRITE_PHYREG(pi, TxResamplerMuDelta1l, fcw & 0xffff);
                WRITE_PHYREG(pi, TxResamplerMuDelta1u, (fcw & 0xff0000) >> 16);
                WRITE_PHYREG(pi, TxResamplerMuDeltaInit1l, fcw & 0xffff);
                WRITE_PHYREG(pi, TxResamplerMuDeltaInit1u, (fcw & 0xff0000) >> 16);
            }
        }
    } else {
        if (sc_mode == 1) {
            ch = CHSPEC_CHANNEL(chanspec_sel);
            fc = wf_channel2mhz(ch, CHSPEC_IS2G(chanmgri->radio_chanspec_sc) ?
                WF_CHAN_FACTOR_2_4_G : WF_CHAN_FACTOR_5_G);
        } else {
            ch = CHSPEC_CHANNEL(chanspec_sel);
            fc = wf_channel2mhz(ch, CHSPEC_IS2G(pi->radio_chanspec) ?
                WF_CHAN_FACTOR_2_4_G : WF_CHAN_FACTOR_5_G);
        }
        //ch = CHSPEC_CHANNEL(chanspec);
        //fc = wf_channel2mhz(ch, CHSPEC_IS2G(pi->radio_chanspec) ?
        //    WF_CHAN_FACTOR_2_4_G : WF_CHAN_FACTOR_5_G);
        math_uint64_multiple_add(&tmp_high, &tmp_low, a * afe_clk_num * b,
            1 << 23, (fc * afe_clk_den) >> 1);
        math_uint64_divide(&fcw, tmp_high, tmp_low, fc * afe_clk_den);
        if (sc_mode == 1) {
            WRITE_PHYREG(pi, TxResamplerMuDelta3l, fcw & 0xffff);
            WRITE_PHYREG(pi, TxResamplerMuDelta3u, (fcw & 0xff0000) >> 16);
            WRITE_PHYREG(pi, TxResamplerMuDeltaInit3l, fcw & 0xffff);
            WRITE_PHYREG(pi, TxResamplerMuDeltaInit3u, (fcw & 0xff0000) >> 16);
        } else {
            wlc_phy_tx_farrow_mu_setup(pi, fcw & 0xffff, (fcw & 0xff0000) >> 16,
                fcw & 0xffff, (fcw & 0xff0000) >> 16);
        }
    }

    if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
        ACMAJORREV_33(pi->pubpi->phy_rev)) {
        /* Disable PA during rfseq setting */
        FOREACH_CORE(pi, core) {
            restore_over_pa5g_pu[core] = READ_RADIO_REGFLD_20693(pi, TX_TOP_5G_OVR1,
                    core, ovr_pa5g_pu);
            restore_pa5g_pu[core] = READ_RADIO_REGFLD_20693(pi, PA5G_CFG1,
                    core, pa5g_pu);
            restore_ext_5g_papu[core] = READ_PHYREGFLDCE(pi, RfctrlIntc,
                    core, ext_5g_papu);
            restore_override_ext_pa[core] = READ_PHYREGFLDCE(pi, RfctrlIntc,
                    core, override_ext_pa);
            MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR1, core, ovr_pa5g_pu, 1);
            MOD_RADIO_REG_20693(pi, PA5G_CFG1, core, pa5g_pu, 0);
            MOD_PHYREGCE(pi, RfctrlIntc, core, override_ext_pa, 1);
            MOD_PHYREGCE(pi, RfctrlIntc, core, ext_5g_papu, 0);
        }

        wlc_phy_force_rfseq_acphy(pi, ACPHY_RFSEQ_RX2TX);
        wlc_phy_force_rfseq_acphy(pi, ACPHY_RFSEQ_TX2RX);

        /* Restore PA reg value after reseq setting */
        FOREACH_CORE(pi, core) {
            MOD_RADIO_REG_20693(pi, TX_TOP_5G_OVR1,
                    core, ovr_pa5g_pu, restore_over_pa5g_pu[core]);
            MOD_RADIO_REG_20693(pi, PA5G_CFG1,
                    core, pa5g_pu, restore_pa5g_pu[core]);
            MOD_PHYREGCE(pi, RfctrlIntc,
                    core, ext_5g_papu, restore_ext_5g_papu[core]);
            MOD_PHYREGCE(pi, RfctrlIntc,
                    core, override_ext_pa, restore_override_ext_pa[core]);
        }

        wlc_phy_resetcca_acphy(pi);
        if (!suspend)
            wlapi_enable_mac(pi->sh->physhim);
    }
}

static bool
BCMATTACHFN(phy_ac_chanmgr_attach_paprr)(phy_ac_chanmgr_info_t *ac_info)
{
    phy_info_t *pi = ac_info->pi;
    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));

    if ((pi->paprrmcsgain2g = phy_malloc(pi, (sizeof(uint8) * NUM_MCS_PAPRR_GAMMA))) == NULL) {
        PHY_ERROR(("%s: paprrmcsgain2g malloc failed\n", __FUNCTION__));
        return FALSE;
    }
    if ((pi->paprrmcsgain5g20 = phy_malloc(pi,
            (sizeof(uint8) * NUM_MCS_PAPRR_GAMMA))) == NULL) {
        PHY_ERROR(("%s: paprrmcsgain5g20 malloc failed\n", __FUNCTION__));
        return FALSE;
    }
    if ((pi->paprrmcsgain5g40 = phy_malloc(pi,
            (sizeof(uint8) * NUM_MCS_PAPRR_GAMMA))) == NULL) {
        PHY_ERROR(("%s: paprrmcsgain5g40 malloc failed\n", __FUNCTION__));
        return FALSE;
    }
    if ((pi->paprrmcsgain5g80 = phy_malloc(pi,
            (sizeof(uint8) * NUM_MCS_PAPRR_GAMMA))) == NULL) {
        PHY_ERROR(("%s: paprrmcsgain5g80 malloc failed\n", __FUNCTION__));
        return FALSE;
    }
    if ((pi->paprrmcsgamma2g = phy_malloc(pi, (sizeof(int16) * NUM_MCS_PAPRR_GAMMA))) == NULL) {
        PHY_ERROR(("%s: paprrmcsgamma2g malloc failed\n", __FUNCTION__));
        return FALSE;
    }
    if ((pi->paprrmcsgamma5g20 = phy_malloc(pi,
            (sizeof(int16) * NUM_MCS_PAPRR_GAMMA))) == NULL) {
        PHY_ERROR(("%s: paprrmcsgamma5g20 malloc failed\n", __FUNCTION__));
        return FALSE;
    }
    if ((pi->paprrmcsgamma5g40 = phy_malloc(pi,
            (sizeof(int16) * NUM_MCS_PAPRR_GAMMA))) == NULL) {
        PHY_ERROR(("%s: paprrmcsgamma5g40 malloc failed\n", __FUNCTION__));
        return FALSE;
    }
    if ((pi->paprrmcsgamma5g80 = phy_malloc(pi,
            (sizeof(int16) * NUM_MCS_PAPRR_GAMMA))) == NULL) {
        PHY_ERROR(("%s: paprrmcsgamma5g80 malloc failed\n", __FUNCTION__));
        return FALSE;
    }
    if ((pi->paprrmcsgamma2g_ch13 = phy_malloc(pi,
            (sizeof(int16) * NUM_MCS_PAPRR_GAMMA))) == NULL) {
        PHY_ERROR(("%s: paprrmcsgamma2g_ch13 malloc failed\n", __FUNCTION__));
        return FALSE;
    }
    if ((pi->paprrmcsgamma2g_ch1 = phy_malloc(pi,
            (sizeof(int16) * NUM_MCS_PAPRR_GAMMA))) == NULL) {
        PHY_ERROR(("%s: paprrmcsgamma2g_ch1 malloc failed\n", __FUNCTION__));
        return FALSE;
    }
    if ((pi->paprrmcsgain2g_ch13 = phy_malloc(pi,
            (sizeof(int16) * NUM_MCS_PAPRR_GAMMA))) == NULL) {
        PHY_ERROR(("%s: paprrmcsgain2g_ch13 malloc failed\n", __FUNCTION__));
        return FALSE;
    }
    if ((pi->paprrmcsgain2g_ch1 = phy_malloc(pi,
            (sizeof(int16) * NUM_MCS_PAPRR_GAMMA))) == NULL) {
        PHY_ERROR(("%s: paprrmcsgain2g_ch1 malloc failed\n", __FUNCTION__));
        return FALSE;
    }
    return TRUE;
}
static bool
BCMATTACHFN(phy_ac_chanmgr_attach_farrow)(phy_ac_chanmgr_info_t *ci)
{
    int num_bw;
    phy_info_t *pi = ci->pi;
    const chan_info_tx_farrow(*tx_farrow) [ACPHY_NUM_CHANS];

    PHY_TRACE(("wl%d: %s\n", pi->sh->unit, __FUNCTION__));
    ci->tx_farrow = NULL;
    ci->rx_farrow = NULL;

    /* these revs do not use farrow tables, they instead calculate them */
    if (TINY_RADIO(pi) || ACMAJORREV_128(pi->pubpi->phy_rev)) {
        return TRUE;
    }

#ifdef ACPHY_1X1_ONLY
    num_bw = 1;
#else
    num_bw = ACPHY_NUM_BW;
#endif

    if ((ci->tx_farrow =
         phy_malloc(pi, num_bw * sizeof(chan_info_tx_farrow[ACPHY_NUM_CHANS]))) == NULL) {
        PHY_ERROR(("wl%d: %s: out of memory, malloced %d bytes\n", pi->sh->unit,
                   __FUNCTION__, MALLOCED(pi->sh->osh)));
        return FALSE;
    }

    if ((ci->rx_farrow =
         phy_malloc(pi, num_bw * sizeof(chan_info_rx_farrow[ACPHY_NUM_CHANS]))) == NULL) {
        PHY_ERROR(("wl%d: %s: out of memory, malloced %d bytes\n", pi->sh->unit,
                   __FUNCTION__, MALLOCED(pi->sh->osh)));
        return FALSE;
    }

    memcpy(ci->rx_farrow, rx_farrow_tbl,
           ACPHY_NUM_CHANS * num_bw * sizeof(chan_info_rx_farrow));

#ifdef ACPHY_1X1_ONLY
    ASSERT(phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->dac_mode == 1);
    tx_farrow = tx_farrow_dac1_tbl;
#else /* ACPHY_1X1_ONLY */
    switch (phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->dac_mode) {
    case 2:
        tx_farrow = tx_farrow_dac2_tbl;
        break;
    case 3:
        tx_farrow = tx_farrow_dac3_tbl;
        break;
    case 1:
    default:
        /* default to dac_mode 1 */
        tx_farrow = tx_farrow_dac1_tbl;
        break;
    }
#endif /* ACPHY_1X1_ONLY */
    memcpy(ci->tx_farrow, tx_farrow, ACPHY_NUM_CHANS * num_bw * sizeof(chan_info_tx_farrow));

    return TRUE;
}

#define TINY_GET_ADC_MODE(pi, chanspec)        \
    ((CHSPEC_IS20(chanspec) || CHSPEC_IS40(chanspec)) ?    \
    (pi->u.pi_acphy->chanmgri->use_fast_adc_20_40) : 1)
void
wlc_phy_farrow_setup_tiny(phy_info_t *pi, chanspec_t chanspec)
{
    /* Setup adc mode based on BW */
    pi->u.pi_acphy->chanmgri->data->fast_adc_en = TINY_GET_ADC_MODE(pi, chanspec);

    phy_ac_chanmgr_write_tx_farrow_tiny(pi, chanspec, 0, 0);
    phy_ac_chanmgr_write_rx_farrow_tiny(pi, chanspec, 0, 0);

    /* Enable the Tx resampler on all cores */
    MOD_PHYREG(pi, TxResamplerEnable0, enable_tx, 1);
}

void
phy_ac_chanmgr_set_phymode(phy_info_t *pi, chanspec_t chanspec, chanspec_t chanspec_sc,
    uint16 phymode)
{
    uint8 ch[NUM_CHANS_IN_CHAN_BONDING];
    uint8 stall_val = 0;
    uint16 classifier_state = 0;
    uint8 orig_rxfectrl1 = 0;
#ifndef ATE_BUILD
    uint8 rx_coremask, tx_coremask;
    uint8 bwbit = 2;
#endif /* !ATE_BUILD */
    uint8 bwidx = 0;
    uint16 gpio13_mask = 0x2000;
    uint32 pmu_chipctrl1 = 0;
    phy_radar_info_t *ri = pi->radari;
    phy_radar_st_t *st = phy_radar_get_st(ri);
    phy_ac_chanmgr_info_t *chanmgri = pi->u.pi_acphy->chanmgri;

    bool is43684mch2 = ACMAJORREV_47(pi->pubpi->phy_rev) && CHSPEC_IS2G(chanspec) &&
        CHSPEC_ISPHY5G6G(chanspec_sc) && !BF_ELNA_5G(pi->u.pi_acphy);
    bool is6715mch2 = ACMAJORREV_130(pi->pubpi->phy_rev) && CHSPEC_IS2G(chanspec) &&
        CHSPEC_ISPHY5G6G(chanspec_sc) && !BF_ELNA_5G(pi->u.pi_acphy);

    /* when upgrade from 3+1, phymode is set to 0 twice */
    if (PHY_SUPPORT_SCANCORE(pi) &&
        READ_PHYREGFLD(pi, AntDivConfig2059, Trigger_override_per_core) == 0 &&
        phymode == 0) {
        goto avoid_settwice;
    }
    wlapi_suspend_mac_and_wait(pi->sh->physhim);
    /* Disable classifier */
    classifier_state = READ_PHYREG(pi, ClassifierCtrl);
    phy_rxgcrs_sel_classifier(pi, 4);

    /* Disable stalls and hold FIFOs in reset */
    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    orig_rxfectrl1 = READ_PHYREGFLD(pi, RxFeCtrl1, soft_sdfeFifoReset);
    ACPHY_DISABLE_STALL(pi);
    MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, 1);

    // Disable core2core sync and Enable it after switch
    if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))
        phy_ac_chanmgr_core2core_sync_setup(pi->u.pi_acphy->chanmgri, FALSE);
    switch (phymode) {
    case  PHYMODE_BGDFS:
        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev)) { /* 4365 */
            ch[0] = CHSPEC_CHANNEL(chanspec);
            ch[1] = CHSPEC_CHANNEL(chanspec_sc);
            wlc_phy_radio20693_afe_clkdistribtion_mode(pi, 2);
            phy_ac_radio_20693_pmu_pll_config_wave2(pi, 5);
            phy_ac_radio_20693_chanspec_setup(pi, ch[0], 0, 0, 0);
            phy_ac_radio_20693_chanspec_setup(pi, ch[1], 0, 1, 0);
            wlc_phy_radio2069x_vcocal_isdone(pi, FALSE, FALSE);
#ifdef WL_AIR_IQ
            wlc_phy_radio20693_sel_logen_mode(pi);
#else
            wlc_phy_radio20693_sel_logen_5g_mode(pi, 2);
#endif /* WL_AIR_IQ */
            /* Setup adc mode based on BW */
            pi->u.pi_acphy->chanmgri->data->fast_adc_en =
                TINY_GET_ADC_MODE(pi, chanspec);
            phy_ac_chanmgr_write_tx_farrow_tiny(pi, chanspec, chanspec_sc, 1);
            phy_ac_chanmgr_write_rx_farrow_tiny(pi, chanspec, chanspec_sc, 1);
            /* Enable the Tx resampler on all cores */
            MOD_PHYREG(pi, TxResamplerEnable3, enable_tx, 1);
        } else if (ACMAJORREV_47(pi->pubpi->phy_rev)) { /* 43684 */
            ch[0] = CHSPEC_CHANNEL(chanspec);
            ch[1] = CHSPEC_CHANNEL(chanspec_sc);
            wlc_phy_radio20698_powerup_RFP1(pi, 1);
            wlc_phy_radio20698_sel_logen_mode(pi, 4);
            wlc_phy_chanspec_radio20698_setup(pi, chanspec_sc, 1, 4);
#ifdef WL_AIR_IQ
            if ((CHSPEC_IS2G(chanspec) && CHSPEC_ISPHY5G6G(chanspec_sc)) ||
                (CHSPEC_IS2G(chanspec) && CHSPEC_IS2G(chanspec_sc)) ||
                (CHSPEC_ISPHY5G6G(chanspec)&& CHSPEC_IS2G(chanspec_sc))) {
                /* different band */
                wlc_phy_radio20698_pu_rx_core(pi, 3, ch[1], 0);
            }
#else
            if ((CHSPEC_IS2G(chanspec) && CHSPEC_ISPHY5G6G(chanspec_sc)) ||
                (CHSPEC_ISPHY5G6G(chanspec)&& CHSPEC_IS2G(chanspec_sc))) {
                /* different band */
                wlc_phy_radio20698_pu_rx_core(pi, 3, ch[1], 0);
            }
#endif
#ifndef ATE_BUILD
            /* enable p1c */
            bwbit = CHSPEC_BW_LE20(chanspec_sc)? 2 : CHSPEC_IS40(chanspec_sc)? 4 :
                CHSPEC_IS80(chanspec_sc)? 6 : 8;
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)(0x10 + bwbit));
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)(0x11 + bwbit));
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)(0x91 + bwbit));
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)(0x11 + bwbit));
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)(0x01 + bwbit));
            MOD_PHYREG(pi, dacClkCtrl, vasipClkEn, 1);
            MOD_PHYREG(pi, dacClkCtrl, vasipAutoClkEn, 1);
#endif /* !ATE_BUILD */
            MOD_PHYREG(pi, TxResamplerEnable3, tx_stall_disable, 1);
#ifndef ATE_BUILD
            wlc_phy_rx_farrow_setup_28nm(pi, chanspec_sc, 1);
#endif /* !ATE_BUILD */
            bwidx = CHSPEC_BW_LE20(chanspec_sc)? 0 : CHSPEC_IS40(chanspec_sc)? 1 :
                CHSPEC_IS80(chanspec_sc)? 2 : 3;
            MOD_PHYREG(pi, RfctrlOverrideLpfCT3, lpf_bq1_bw, 1);
            MOD_PHYREG(pi, RfctrlOverrideLpfCT3, lpf_bq2_bw, 1);
            MOD_PHYREG(pi, RfctrlCoreLpfCT3, lpf_bq1_bw, bwidx);
            MOD_PHYREG(pi, RfctrlCoreLpfCT3, lpf_bq2_bw, bwidx);
            wlc_phy_radio20698_afe_div_ratio(pi, 1, chanspec_sc, 1);
            /* different band for 43684 MC board */
            phy_ac_chanmgr_set_bandsel_on_gpio(pi, CHSPEC_IS2G(chanspec), 9);
            phy_ac_chanmgr_set_bandsel_on_gpio(pi, CHSPEC_IS2G(chanspec_sc), 11);
            if (is43684mch2) {
                /* gpio13=1 to enable HW input blanking when phymode 31 */
                si_gpioout(pi->sh->sih, gpio13_mask, (1 << 13), GPIO_DRV_PRIORITY);
                si_gpioouten(pi->sh->sih, gpio13_mask, gpio13_mask,
                    GPIO_DRV_PRIORITY);
                si_gpiocontrol(pi->sh->sih, gpio13_mask, 0, GPIO_DRV_PRIORITY);
            }
            if (CHSPEC_IS2G(chanspec) && CHSPEC_ISPHY5G6G(chanspec_sc)) {
                MOD_PHYREG(pi, FemOutputOvrCtrl3, femCtrlOutputOvr, 1);
                MOD_PHYREG(pi, FemOutputOvrCtrl3, femCtrlOutput, 6);
            } else if (CHSPEC_ISPHY5G6G(chanspec) && CHSPEC_IS2G(chanspec_sc)) {
                MOD_PHYREG(pi, FemOutputOvrCtrl3, femCtrlOutputOvr, 1);
                MOD_PHYREG(pi, FemOutputOvrCtrl3, femCtrlOutput, 4);
            }
            if (CHSPEC_ISPHY5G6G(chanspec) && CHSPEC_ISPHY5G6G(chanspec_sc) &&
                !ACMINORREV_0(pi) && (ch[1] == 52)) {
                // small fm on 5g+5g plc CH52 due to RB149974 5g lna1 gain change
                chanmgri->orig_min_fm_lp = st->rparams.radar_args.min_fm_lp;
                st->rparams.radar_args.min_fm_lp = 132;
            } else if (is43684mch2) {
                chanmgri->orig_min_fm_lp = st->rparams.radar_args.min_fm_lp;
                chanmgri->orig_max_span_lp = st->rparams.radar_args.max_span_lp;
                chanmgri->orig_st_level_time = st->rparams.radar_args.st_level_time;
                // FCC-5 small fm on plc bw20/bw40
                // set pw thrsh to 610 to improve ETSI-4 detection on bw20/bw40
                st->rparams.radar_args.min_fm_lp = CHSPEC_IS20(chanspec_sc) ? 0 :
                    (CHSPEC_IS40(chanspec_sc) && ch[1] <=
                    WL_THRESHOLD_LO_BAND) ? 27 : CHSPEC_IS40(chanspec_sc) ?
                    100 : CHSPEC_IS80(chanspec_sc) ? 133 :
                    chanmgri->orig_min_fm_lp;
                st->rparams.radar_args.max_span_lp = CHSPEC_IS20(chanspec_sc) ?
                    47116 : chanmgri->orig_max_span_lp;
                st->rparams.radar_args.st_level_time = (CHSPEC_IS20(chanspec_sc) ||
                    CHSPEC_IS40(chanspec_sc)) ? 0x8262 :
                    chanmgri->orig_st_level_time;
            }
            if (CHSPEC_IS80(chanspec) && (ch[0] <= WL_THRESHOLD_LO_BAND)) {
                chanmgri->origSigFld1Decode = READ_PHYREGFLD(pi, RadarBlankCtrl2,
                    radarSigDecode1BlankEn);
                MOD_PHYREG(pi, RadarBlankCtrl2, radarSigDecode1BlankEn, 0);
            }
            /* disable SDFE clk on P1C to fix ping issue due to RB155334 */
            chanmgri->origSdFeClkEn = READ_PHYREGFLD(pi, RxFeCtrl1, forceSdFeClkEn);
            MOD_PHYREG(pi, RxFeCtrl1, forceSdFeClkEn, (chanmgri->origSdFeClkEn & 0x7));
            /* disable ed_crs and ed_Crs80u on core3 to avoid PSM watchdog fired */
            chanmgri->origEdCrsEn = READ_PHYREG(pi, ed_crsEn);
            chanmgri->origEdCrsEn80u = READ_PHYREG(pi, ed_crsEn_80u);
            WRITE_PHYREG(pi, ed_crsEn, chanmgri->origEdCrsEn & 0xfff);
            WRITE_PHYREG(pi, ed_crsEn_80u, chanmgri->origEdCrsEn80u & 0xfff);
        } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) { /* 6715 */
            ch[0] = CHSPEC_CHANNEL(chanspec);
            ch[1] = CHSPEC_CHANNEL(chanspec_sc);
            if (pi->u.pi_acphy->radioi->maincore_on_pll1 == 0) {
                wlc_phy_radio20708_powerup_RFPll(pi, 1, 1);
                wlc_phy_radio20708_sel_logen_mode(pi, 2);
                wlc_phy_radio20708_afediv_control(pi, 2);
                wlc_phy_radio20708_logen_core_setup(pi, chanspec_sc, 1, 0);
                wlc_phy_chanspec_radio20708_setup(pi, chanspec_sc, 1, 1, 2);
            } else { /* maincore_on_pll1 == 1 */
                wlc_phy_radio20708_powerup_RFPll(pi, 0, 1);
                wlc_phy_radio20708_sel_logen_mode(pi, 5);
                wlc_phy_radio20708_afediv_control(pi, 5);
                wlc_phy_radio20708_logen_core_setup(pi, chanspec_sc, 1, 0);
                wlc_phy_chanspec_radio20708_setup(pi, chanspec_sc, 1, 0, 5);
            }
#ifdef WL_AIR_IQ
            if ((CHSPEC_IS2G(chanspec) && CHSPEC_ISPHY5G6G(chanspec_sc)) ||
                (CHSPEC_IS2G(chanspec) && CHSPEC_IS2G(chanspec_sc)) ||
                (CHSPEC_ISPHY5G6G(chanspec)&& CHSPEC_IS2G(chanspec_sc))) {
                /* different band */
                wlc_phy_radio20708_pu_rx_core(pi, 3, ch[1], 0);
            }
#else
            if ((CHSPEC_IS2G(chanspec) && CHSPEC_ISPHY5G6G(chanspec_sc)) ||
                (CHSPEC_ISPHY5G6G(chanspec)&& CHSPEC_IS2G(chanspec_sc))) {
                /* different band */
                wlc_phy_radio20708_pu_rx_core(pi, 3, ch[1], 0);
            }
#endif
#ifndef ATE_BUILD
            /* enable p1c */
            bwbit = CHSPEC_BW_LE20(chanspec_sc)? 2 : CHSPEC_IS40(chanspec_sc)? 4 :
                CHSPEC_IS80(chanspec_sc)? 6 : 8;
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)(0x10 + bwbit));
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)(0x11 + bwbit));
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)(0x91 + bwbit));
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)(0x11 + bwbit));
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)(0x01 + bwbit));
            MOD_PHYREG(pi, dacClkCtrl, vasipClkEn, 1);
            MOD_PHYREG(pi, dacClkCtrl, vasipAutoClkEn, 1);
#endif /* !ATE_BUILD */
            MOD_PHYREG(pi, TxResamplerEnable3, tx_stall_disable, 1);
#ifndef ATE_BUILD
            wlc_phy_rx_farrow_setup_28nm(pi, chanspec_sc, 1);
#endif /* !ATE_BUILD */
            bwidx = CHSPEC_BW_LE20(chanspec_sc)? 0 : CHSPEC_IS40(chanspec_sc)? 1 :
                CHSPEC_IS80(chanspec_sc)? 2 : 3;
            MOD_PHYREG(pi, RfctrlOverrideLpfCT3, lpf_bq1_bw, 1);
            MOD_PHYREG(pi, RfctrlOverrideLpfCT3, lpf_bq2_bw, 1);
            MOD_PHYREG(pi, RfctrlCoreLpfCT3, lpf_bq1_bw, bwidx);
            MOD_PHYREG(pi, RfctrlCoreLpfCT3, lpf_bq2_bw, bwidx);
            wlc_phy_radio20708_afe_div_ratio(pi, 1, chanspec_sc, 1, 0, 1, 0);
            /* different band for 6715 MC board */
            phy_ac_chanmgr_set_bandsel_on_gpio(pi, CHSPEC_IS2G(chanspec), 9);
            phy_ac_chanmgr_set_bandsel_on_gpio(pi, CHSPEC_IS2G(chanspec_sc), 11);

            if (CHSPEC_IS2G(chanspec) && CHSPEC_ISPHY5G6G(chanspec_sc)) {
                /* 6715MCH2: enable gpio15 HW input blanking via chip control */
                pmu_chipctrl1 = si_pmu_chipcontrol(pi->sh->sih, 1, 0, 0);
                si_pmu_chipcontrol(pi->sh->sih, 1, 0xFFFFFFFF,
                    (pmu_chipctrl1 | 0x10000000));
                MOD_PHYREG(pi, FemOutputOvrCtrl3, femCtrlOutputOvr, 1);
                MOD_PHYREG(pi, FemOutputOvrCtrl3, femCtrlOutput, 6);
            } else if (CHSPEC_ISPHY5G6G(chanspec) && CHSPEC_IS2G(chanspec_sc)) {
                MOD_PHYREG(pi, FemOutputOvrCtrl3, femCtrlOutputOvr, 1);
                MOD_PHYREG(pi, FemOutputOvrCtrl3, femCtrlOutput, 4);
            }

            if (is6715mch2) {
                chanmgri->orig_min_fm_lp = st->rparams.radar_args.min_fm_lp;
                chanmgri->orig_st_level_time = st->rparams.radar_args.st_level_time;

                st->rparams.radar_args.min_fm_lp = (CHSPEC_IS20(chanspec_sc) ||
                    CHSPEC_IS40(chanspec_sc)) ? 40 : chanmgri->orig_min_fm_lp;
                st->rparams.radar_args.st_level_time = CHSPEC_IS20(chanspec_sc) ?
                    ((chanmgri->orig_st_level_time & 0xfff) | 0x7000) :
                    chanmgri->orig_st_level_time;
                /* override rxgain to improve FCC-5 radar fm estimation */
                phy_ac_rxgcrs_iovar_set_force_rxgain(pi->u.pi_acphy->rxgcrsi,
                    0x5fb8);
            }

            /* disable ed_crs and ed_Crs80u on core3 to avoid PSM watchdog fired */
            chanmgri->origEdCrsEn = READ_PHYREG(pi, ed_crsEn);
            chanmgri->origEdCrsEn80u = READ_PHYREG(pi, ed_crsEn_80u);
            WRITE_PHYREG(pi, ed_crsEn, chanmgri->origEdCrsEn & 0xfff);
            WRITE_PHYREG(pi, ed_crsEn_80u, chanmgri->origEdCrsEn80u & 0xfff);
        } else {
            PHY_ERROR(("wl%d: %s: Unsupported PHY revision for BGDFS\n",
                     pi->sh->unit, __FUNCTION__));
            break;
        }
#ifndef ATE_BUILD
        MOD_PHYREG(pi, RfseqMode, CoreActv_override_percore, 8);
        /* Enable RX on core-3, disable Tx */
        rx_coremask = 0x8 | phy_stf_get_data(pi->stfi)->phyrxchain;
        tx_coremask = 0x7 & phy_stf_get_data(pi->stfi)->phytxchain;

        MOD_PHYREG(pi, RfseqCoreActv2059, EnRx, rx_coremask);
        MOD_PHYREG(pi, RfseqCoreActv2059, EnTx, tx_coremask);
        MOD_PHYREG(pi, CoreConfig, CoreMask, tx_coremask);
#ifdef WL_AIR_IQ
        MOD_PHYREG(pi, CoreConfig, pasvRxSampCapOn, 1);
#else
        MOD_PHYREG(pi, CoreConfig, pasvRxSampCapOn, 0);
#endif /* WL_AIR_IQ */
        if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, AntDivConfig2059, Trigger_override_per_core, 8);
        } else {
            /* 43684: if it is set p1c bw < normal bw cannot detect radar */
            MOD_PHYREG(pi, CoreConfig, pasvRxCoreMask, 8);
        }

        if (CHSPEC_CHANNEL(chanspec_sc) <= WL_THRESHOLD_LO_BAND) {
            if (CHSPEC_IS20(chanspec_sc)) {
                st->rparams.radar_args.thresh0_sc=
                    st->rparams.radar_thrs2.thresh0_sc_20_lo;
                st->rparams.radar_args.thresh1_sc =
                    st->rparams.radar_thrs2.thresh1_sc_20_lo;
            } else if (CHSPEC_IS40(chanspec_sc)) {
                st->rparams.radar_args.thresh0_sc =
                    st->rparams.radar_thrs2.thresh0_sc_40_lo;
                st->rparams.radar_args.thresh1_sc =
                    st->rparams.radar_thrs2.thresh1_sc_40_lo;
            } else if (CHSPEC_IS80(chanspec_sc) || PHY_AS_80P80(pi, chanspec_sc)) {
                st->rparams.radar_args.thresh0_sc =
                    st->rparams.radar_thrs2.thresh0_sc_80_lo;
                st->rparams.radar_args.thresh1_sc =
                    st->rparams.radar_thrs2.thresh1_sc_80_lo;
            } else if (CHSPEC_IS160(chanspec_sc) &&
                ACMAJORREV_47_130(pi->pubpi->phy_rev)) {
                st->rparams.radar_args.thresh0_sc =
                    st->rparams.radar_thrs2.thresh0_sc_160_lo;
                st->rparams.radar_args.thresh1_sc =
                    st->rparams.radar_thrs2.thresh1_sc_160_lo;
            }
        } else {
            if (CHSPEC_IS20(chanspec_sc)) {
                st->rparams.radar_args.thresh0_sc=
                    st->rparams.radar_thrs2.thresh0_sc_20_hi;
                st->rparams.radar_args.thresh1_sc =
                    st->rparams.radar_thrs2.thresh1_sc_20_hi;
            } else if (CHSPEC_IS40(chanspec_sc)) {
                st->rparams.radar_args.thresh0_sc =
                    st->rparams.radar_thrs2.thresh0_sc_40_hi;
                st->rparams.radar_args.thresh1_sc =
                    st->rparams.radar_thrs2.thresh1_sc_40_hi;
            } else if (CHSPEC_IS80(chanspec_sc) || PHY_AS_80P80(pi, chanspec_sc)) {
                st->rparams.radar_args.thresh0_sc =
                    st->rparams.radar_thrs2.thresh0_sc_80_hi;
                st->rparams.radar_args.thresh1_sc =
                    st->rparams.radar_thrs2.thresh1_sc_80_hi;
            } else if (CHSPEC_IS160(chanspec_sc) &&
                ACMAJORREV_47_130(pi->pubpi->phy_rev)) {
                st->rparams.radar_args.thresh0_sc =
                    st->rparams.radar_thrs2.thresh0_sc_160_hi;
                st->rparams.radar_args.thresh1_sc =
                    st->rparams.radar_thrs2.thresh1_sc_160_hi;
            }
        }
        phy_utils_write_phyreg(pi, ACPHY_RadarThresh0_SC(pi->pubpi->phy_rev),
        (uint16)((int16)st->rparams.radar_args.thresh0_sc));
        phy_utils_write_phyreg(pi, ACPHY_RadarThresh1_SC(pi->pubpi->phy_rev),
        (uint16)((int16)st->rparams.radar_args.thresh1_sc));
#endif /* !ATE_BUILD */
        break;
    case 0:
        if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev)) { /* 4365 */
            MOD_RADIO_PLLREG_20693(pi, PLL_CFG1, 1, rfpll_synth_pu, 0);
            MOD_RADIO_PLLREG_20693(pi, PLL_CP1, 1, rfpll_cp_pu, 0);
            MOD_RADIO_PLLREG_20693(pi, PLL_CFG1, 1, rfpll_vco_pu, 0);
            MOD_RADIO_PLLREG_20693(pi, PLL_CFG1, 1, rfpll_vco_buf_pu, 0);
            MOD_RADIO_PLLREG_20693(pi, PLL_CFG1, 1, rfpll_monitor_pu, 0);
            MOD_RADIO_PLLREG_20693(pi, PLL_LF6, 1, rfpll_lf_cm_pu, 0);
            MOD_RADIO_PLLREG_20693(pi, PLL_CFG1, 1, rfpll_pfd_en, 0);
            MOD_RADIO_PLLREG_20693(pi, RFPLL_OVR1, 1, ovr_rfpll_synth_pu, 1);
            MOD_RADIO_PLLREG_20693(pi, RFPLL_OVR1, 1, ovr_rfpll_cp_pu, 1);
            MOD_RADIO_PLLREG_20693(pi, RFPLL_OVR1, 1, ovr_rfpll_vco_pu, 1);
            MOD_RADIO_PLLREG_20693(pi, RFPLL_OVR1, 1, ovr_rfpll_vco_buf_pu, 1);
            MOD_RADIO_PLLREG_20693(pi, PLL_HVLDO1, 1, ldo_2p5_pu_ldo_CP, 0);
            MOD_RADIO_PLLREG_20693(pi, PLL_HVLDO1, 1, ldo_2p5_pu_ldo_VCO, 0);
            MOD_RADIO_PLLREG_20693(pi, RFPLL_OVR1, 1, ovr_ldo_2p5_pu_ldo_CP, 1);
            MOD_RADIO_PLLREG_20693(pi, RFPLL_OVR1, 1, ovr_ldo_2p5_pu_ldo_VCO, 1);
            wlc_phy_radio20693_afe_clkdistribtion_mode(pi, 0);
#ifdef WL_AIR_IQ
            wlc_phy_radio20693_sel_logen_mode(pi);
#else
            wlc_phy_radio20693_sel_logen_5g_mode(pi, 0);
#endif /* WL_AIR_IQ */

            /* Setup adc mode based on BW */
            pi->u.pi_acphy->chanmgri->data->fast_adc_en =
                TINY_GET_ADC_MODE(pi, chanspec);
            phy_ac_chanmgr_write_tx_farrow_tiny(pi, chanspec, chanspec_sc, 0);
            phy_ac_chanmgr_write_rx_farrow_tiny(pi, chanspec, chanspec_sc, 0);
            /* Enable the Tx resampler on all cores */
            MOD_PHYREG(pi, TxResamplerEnable0, enable_tx, 1);
        } else if (ACMAJORREV_47(pi->pubpi->phy_rev)) { /* 43684 */
            ch[0] = CHSPEC_CHANNEL(chanspec);
            ch[1] = CHSPEC_CHANNEL(chanspec_sc);
            wlc_phy_radio20698_powerup_RFP1(pi, 0);
            wlc_phy_radio20698_sel_logen_mode(pi, 0);
            wlc_phy_chanspec_radio20698_setup(pi, chanspec, 1, 0);
            wlc_phy_radio20698_vcocal_isdone(pi, FALSE);

            if (CHSPEC_IS2G(chanspec) && CHSPEC_ISPHY5G6G(chanspec_sc)) {
                /* different band */
                MOD_RADIO_REG_20698(pi, RX5G_REG1, 3, rx5g_lna_tune, 0xf);
                MOD_RADIO_REG_20698(pi, RX5G_REG5, 3, rx5g_mix_Cin_tune, 0);
                wlc_phy_radio20698_pu_rx_core(pi, 3, ch[0], 1);
            } else if (CHSPEC_ISPHY5G6G(chanspec) && CHSPEC_IS2G(chanspec_sc)) {
                wlc_phy_radio20698_pu_rx_core(pi, 3, ch[0], 1);
            }
#ifndef ATE_BUILD
            /* reset p1c phyreg first before turn it off */
            MOD_PHYREG_p1c(pi, RadarSearchCtrl, radarEnable, 0);
            /* disable p1c */
            bwbit = CHSPEC_BW_LE20(chanspec_sc)? 2 : CHSPEC_IS40(chanspec_sc)? 4 :
                CHSPEC_IS80(chanspec_sc)? 6 : 8;
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)(0x11 + bwbit));
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)(0x10 + bwbit));
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)0x02);
#endif /* !ATE_BUILD */
            MOD_PHYREG(pi, TxResamplerEnable3, tx_stall_disable, 0);
#ifndef ATE_BUILD
            wlc_phy_rx_farrow_setup_28nm(pi, chanspec_sc, 0);
#endif /* !ATE_BUILD */
            MOD_PHYREG(pi, RfctrlOverrideLpfCT3, lpf_bq1_bw, 0);
            MOD_PHYREG(pi, RfctrlOverrideLpfCT3, lpf_bq2_bw, 0);
            wlc_phy_radio20698_afe_div_ratio(pi, 0, 0, 0);

            /* different band for 43684 MC board */
            phy_ac_chanmgr_set_bandsel_on_gpio(pi, CHSPEC_IS2G(chanspec), 9);
            phy_ac_chanmgr_set_bandsel_on_gpio(pi, CHSPEC_IS2G(chanspec), 11);
            if (is43684mch2) {
                /* gpio13=0 to disable HW input blanking when phymode 0 */
                si_gpioout(pi->sh->sih, gpio13_mask, (0 << 13), GPIO_DRV_PRIORITY);
                si_gpioouten(pi->sh->sih, gpio13_mask, gpio13_mask,
                    GPIO_DRV_PRIORITY);
                si_gpiocontrol(pi->sh->sih, gpio13_mask, 0, GPIO_DRV_PRIORITY);
            }
            if ((CHSPEC_IS2G(chanspec) && CHSPEC_ISPHY5G6G(chanspec_sc)) ||
                (CHSPEC_ISPHY5G6G(chanspec)&& CHSPEC_IS2G(chanspec_sc))) {
                MOD_PHYREG(pi, FemOutputOvrCtrl3, femCtrlOutput, 0);
                MOD_PHYREG(pi, FemOutputOvrCtrl3, femCtrlOutputOvr, 0);
            }
            /* restore min_fm_lp/st_level_time when phymode 0 */
            if (CHSPEC_ISPHY5G6G(chanspec) && CHSPEC_ISPHY5G6G(chanspec_sc) &&
                !ACMINORREV_0(pi) && (ch[1] == 52) &&
                (chanmgri->orig_min_fm_lp != 0)) {
                st->rparams.radar_args.min_fm_lp = chanmgri->orig_min_fm_lp;
            } else if (is43684mch2) {
                if (chanmgri->orig_min_fm_lp != 0 && (CHSPEC_IS20(chanspec_sc) ||
                    CHSPEC_IS40(chanspec_sc)))
                    st->rparams.radar_args.min_fm_lp = chanmgri->orig_min_fm_lp;
                if (chanmgri->orig_max_span_lp != 0 && CHSPEC_IS20(chanspec_sc))
                    st->rparams.radar_args.max_span_lp =
                        chanmgri->orig_max_span_lp;
                if (chanmgri->orig_st_level_time != 0 &&
                    (CHSPEC_IS20(chanspec_sc) || CHSPEC_IS40(chanspec_sc)))
                    st->rparams.radar_args.st_level_time =
                        chanmgri->orig_st_level_time;
            }
            if (CHSPEC_IS80(chanspec) && (ch[0] <= WL_THRESHOLD_LO_BAND) &&
                (chanmgri->origSigFld1Decode != 0)) {
                MOD_PHYREG(pi, RadarBlankCtrl2, radarSigDecode1BlankEn,
                    chanmgri->origSigFld1Decode);
            }
            if (chanmgri->origSdFeClkEn != 0)
                MOD_PHYREG(pi, RxFeCtrl1, forceSdFeClkEn, chanmgri->origSdFeClkEn);
            if (chanmgri->origEdCrsEn != 0)
                WRITE_PHYREG(pi, ed_crsEn, chanmgri->origEdCrsEn);
            if (chanmgri->origEdCrsEn80u != 0)
                WRITE_PHYREG(pi, ed_crsEn_80u, chanmgri->origEdCrsEn80u);
        } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) { /* 6715 */
            ch[0] = CHSPEC_CHANNEL(chanspec);
            ch[1] = CHSPEC_CHANNEL(chanspec_sc);
            if (pi->u.pi_acphy->radioi->maincore_on_pll1 == 0) {
                wlc_phy_radio20708_powerup_RFPll(pi, 1, 0);
                wlc_phy_radio20708_sel_logen_mode(pi, 1);
                wlc_phy_radio20708_afediv_control(pi, 1);
                wlc_phy_radio20708_logen_core_setup(pi, chanspec_sc, 1, 1);
                wlc_phy_chanspec_radio20708_setup(pi, chanspec, 1, 0, 0);
            } else { /* maincore_on_pll1 == 1 */
                wlc_phy_radio20708_powerup_RFPll(pi, 0, 0);
                wlc_phy_radio20708_sel_logen_mode(pi, 4);
                wlc_phy_radio20708_afediv_control(pi, 4);
                wlc_phy_radio20708_logen_core_setup(pi, chanspec_sc, 1, 1);
                wlc_phy_chanspec_radio20708_setup(pi, chanspec, 1, 1, 0);
            }
#ifdef WL_AIR_IQ
            if ((CHSPEC_IS2G(chanspec) && CHSPEC_ISPHY5G6G(chanspec_sc)) ||
                (CHSPEC_IS2G(chanspec) && CHSPEC_IS2G(chanspec_sc)) ||
                (CHSPEC_ISPHY5G6G(chanspec)&& CHSPEC_IS2G(chanspec_sc))) {
                /* different band */
                wlc_phy_radio20708_pu_rx_core(pi, 3, ch[0], 1);
            }
#else
            if ((CHSPEC_IS2G(chanspec) && CHSPEC_ISPHY5G6G(chanspec_sc)) ||
                (CHSPEC_ISPHY5G6G(chanspec) && CHSPEC_IS2G(chanspec_sc))) {
                /* different band */
                wlc_phy_radio20708_pu_rx_core(pi, 3, ch[0], 1);
            }
#endif
#ifndef ATE_BUILD
            /* reset p1c phyreg first before turn it off */
            MOD_PHYREG_p1c(pi, RadarSearchCtrl, radarEnable, 0);
            /* disable p1c */
            bwbit = CHSPEC_BW_LE20(chanspec_sc)? 2 : CHSPEC_IS40(chanspec_sc)? 4 :
                CHSPEC_IS80(chanspec_sc)? 6 : 8;
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)(0x11 + bwbit));
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)(0x10 + bwbit));
            W_REG(pi->sh->osh, D11_PHYPLUS1CTL(pi), (uint16)0x02);
#endif /* !ATE_BUILD */
            MOD_PHYREG(pi, TxResamplerEnable3, tx_stall_disable, 0);
#ifndef ATE_BUILD
            wlc_phy_rx_farrow_setup_28nm(pi, chanspec_sc, 0);
#endif /* !ATE_BUILD */
            MOD_PHYREG(pi, RfctrlOverrideLpfCT3, lpf_bq1_bw, 0);
            MOD_PHYREG(pi, RfctrlOverrideLpfCT3, lpf_bq2_bw, 0);
            wlc_phy_radio20708_afe_div_ratio(pi, 1, 0, 0, 1, 1, 0);

            /* different band for 43684 MC board */
            phy_ac_chanmgr_set_bandsel_on_gpio(pi, CHSPEC_IS2G(chanspec), 9);
            phy_ac_chanmgr_set_bandsel_on_gpio(pi, CHSPEC_IS2G(chanspec), 11);

            if (CHSPEC_IS2G(chanspec) && CHSPEC_ISPHY5G6G(chanspec_sc)) {
                /* 6715MCH2: disable gpio15 HW input blanking via chip control */
                pmu_chipctrl1 = si_pmu_chipcontrol(pi->sh->sih, 1, 0, 0);
                si_pmu_chipcontrol(pi->sh->sih, 1, 0xFFFFFFFF,
                    (pmu_chipctrl1 & 0xefffffff));
            }

            if (is6715mch2) {
                if (chanmgri->orig_min_fm_lp != 0 && (CHSPEC_IS20(chanspec_sc) ||
                    CHSPEC_IS40(chanspec_sc)))
                    st->rparams.radar_args.min_fm_lp = chanmgri->orig_min_fm_lp;
                if (chanmgri->orig_st_level_time != 0 && CHSPEC_IS20(chanspec_sc))
                    st->rparams.radar_args.st_level_time =
                        chanmgri->orig_st_level_time;
                /* remove rxgain override */
                phy_ac_rxgcrs_iovar_set_force_rxgain(pi->u.pi_acphy->rxgcrsi,
                    0xffff);
            }

            if (chanmgri->origEdCrsEn != 0)
                WRITE_PHYREG(pi, ed_crsEn, chanmgri->origEdCrsEn);
            if (chanmgri->origEdCrsEn80u != 0)
                WRITE_PHYREG(pi, ed_crsEn_80u, chanmgri->origEdCrsEn80u);
        } else {
            PHY_ERROR(("wl%d: %s: Unsupported PHY revision for BGDFS\n",
                     pi->sh->unit, __FUNCTION__));
            break;
        }
#ifndef ATE_BUILD
        if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, TxPwrCtrlCmd, txPwrCtrl_en, 0);
            wlc_phy_txpwrctrl_enable_acphy(pi, PHY_TPC_HW_OFF);
            wlc_phy_txpwr_by_index_acphy(pi, (1 << 3), 64);
        }
        MOD_PHYREG(pi, RfseqMode, CoreActv_override_percore, 0);
        /* Enable RX and TX on core-3 */
        rx_coremask = 0x8 | phy_stf_get_data(pi->stfi)->phyrxchain;
        tx_coremask = 0x8 | phy_stf_get_data(pi->stfi)->phytxchain;

        MOD_PHYREG(pi, RfseqCoreActv2059, EnRx, rx_coremask);
        MOD_PHYREG(pi, RfseqCoreActv2059, EnTx, tx_coremask);
        MOD_PHYREG(pi, CoreConfig, CoreMask, tx_coremask);
        MOD_PHYREG(pi, CoreConfig, pasvRxSampCapOn, 0);
        MOD_PHYREG(pi, CoreConfig, pasvRxCoreMask, 0);
        if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
            wlc_phy_txpwrctrl_enable_acphy(pi, PHY_TPC_HW_ON);
        }
#endif /* !ATE_BUILD */
        break;
    default:
        PHY_ERROR(("wl%d: %s: Unsupported radio revision %d\n",
            pi->sh->unit, __FUNCTION__, RADIOREV(pi->pubpi->radiorev)));
        ASSERT(0);
    }
    /* Restore FIFO reset and Stalls */
    ACPHY_ENABLE_STALL(pi, stall_val);
    MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, orig_rxfectrl1);
    OSL_DELAY(1);

    /* Restore classifier */
    WRITE_PHYREG(pi, ClassifierCtrl, classifier_state);
    OSL_DELAY(1);

    /* Reset PHY */
    wlc_phy_resetcca_acphy(pi);
    /* Enable Core2Core Sync */
    if ((ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev)) && pi->u.pi_acphy->c2c_sync_en)
        phy_ac_chanmgr_core2core_sync_setup(pi->u.pi_acphy->chanmgri, TRUE);
    wlapi_enable_mac(pi->sh->physhim);

    avoid_settwice:
    switch (phymode) {
        case  PHYMODE_BGDFS:
            if (ACMAJORREV_32(pi->pubpi->phy_rev) ||
                ACMAJORREV_33(pi->pubpi->phy_rev)) {
                MOD_PHYREG(pi, AntDivConfig2059, Trigger_override_per_core, 8);
                MOD_PHYREG(pi, RadarSearchCtrl_SC, radarEnable, 1);
                MOD_PHYREG(pi, RadarDetectConfig3_SC, scan_mode, 1);
                MOD_PHYREG(pi, RadarDetectConfig3_SC, gain_override_en, 1);
                MOD_PHYREG(pi, RadarBlankCtrl2_SC, blank_mode, 1);
            } else if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
#ifndef ATE_BUILD
                MOD_PHYREG(pi, AntDivConfig2059, Trigger_override_per_core, 8);
                /* disable_stalls=0 for real chip and 1 for self_test */
                MOD_PHYREG_p1c(pi, RxFeCtrl1, disable_stalls, 0);
                MOD_PHYREG_p1c(pi, RxFeCtrl1, swap_iq0, 1);
                MOD_PHYREG_p1c(pi, sarAfeCompCtrl0, sarAfePhaseSel,
                    CHSPEC_IS160(chanspec_sc) ? 1 : 0);
                MOD_PHYREG_p1c(pi, RadarTx2NclksBlank, tx2nclks_BlankEn, 1);
                MOD_PHYREG_p1c(pi, RadarTx2NclksBlank, tx2nclks_BlankExtTime, 0);
                MOD_PHYREG_p1c(pi, RfseqMode, CoreActv_override_percore, 1);
                MOD_PHYREG_p1c(pi, RfseqCoreActv2059, EnRx, 1);
                MOD_PHYREG_p1c(pi, RfseqCoreActv2059, EnTx, 0);
                MOD_PHYREG_p1c(pi, CoreConfig, CoreMask, 0);
                MOD_PHYREG_p1c(pi, CoreConfig, NumRxCores, 1);
                MOD_PHYREG_p1c(pi, CoreConfig, pasvRxCoreMask, 1);
                MOD_PHYREG_p1c(pi, AntDivConfig2059, Trigger_override_per_core, 1);
                MOD_PHYREG_p1c(pi, RadarDetectConfig3, gain_override_en, 1);
                if (is43684mch2) {
                    MOD_PHYREG_p1c(pi, RadarGainOverride,
                        gain_override_val, 0x36);
                } else { /* set gain_override_val to 0x3d to fix A1 p1c detection */
                    MOD_PHYREG_p1c(pi, RadarGainOverride,
                        gain_override_val, 0x3d);
                }
                MOD_PHYREG_p1c(pi, RadarDetectConfig3, scan_mode, 1);
                MOD_PHYREG_p1c(pi, RadarSearchCtrl, radarEnable, 1);
                phy_utils_write_phyreg_p1c(pi,
                    ACPHY_RadarBlankCtrl(pi->pubpi->phy_rev),
                    CHSPEC_IS20(chanspec_sc) ? 0x8019 :
                    CHSPEC_IS40(chanspec_sc) ? 0x8032 :
                    CHSPEC_IS80(chanspec_sc) ? 0x8064 : 0x80c8);
                if (CHSPEC_IS2G(chanspec) && CHSPEC_ISPHY5G6G(chanspec_sc)) {
                    phy_utils_write_phyreg_p1c(pi,
                        ACPHY_RadarBlankCtrl2(pi->pubpi->phy_rev), 0x8000);
                } else {
                    phy_utils_write_phyreg_p1c(pi,
                        ACPHY_RadarBlankCtrl2(pi->pubpi->phy_rev), 0xa000);
                }
                phy_utils_write_phyreg_p1c(pi,
                    ACPHY_RadarThresh0(pi->pubpi->phy_rev),
                    (uint16)((int16)st->rparams.radar_args.thresh0_sc));
                phy_utils_write_phyreg_p1c(pi,
                    ACPHY_RadarThresh1(pi->pubpi->phy_rev),
                    (uint16)((int16)st->rparams.radar_args.thresh1_sc));
                phy_utils_write_phyreg_p1c(pi,
                    ACPHY_RadarT3BelowMin(pi->pubpi->phy_rev), 0x14);
                phy_utils_write_phyreg_p1c(pi,
                    ACPHY_RadarT3Timeout(pi->pubpi->phy_rev),
                    CHSPEC_IS20(chanspec_sc) ? 0x258 :
                    CHSPEC_IS40(chanspec_sc) ? 0x4b0 :
                    CHSPEC_IS80(chanspec_sc) ? 0x960 : 0x12c0);
                phy_utils_write_phyreg_p1c(pi,
                    ACPHY_RadarResetBlankingDelay(pi->pubpi->phy_rev),
                    CHSPEC_IS20(chanspec_sc) ? 0x19 : CHSPEC_IS40(chanspec_sc) ?
                    0x32 : CHSPEC_IS80(chanspec_sc) ? 0x64 : 0xc8);
                MOD_PHYREG_p1c(pi, DcFiltAddress, dcCoef0,
                    CHSPEC_IS20(chanspec_sc) ?
                    0xa : CHSPEC_IS40(chanspec_sc) ? 0x5 :
                    CHSPEC_IS80(chanspec_sc) ? 0x2 : 0x1);
#endif /* !ATE_BUILD */
            } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
#ifndef ATE_BUILD
                MOD_PHYREG(pi, AntDivConfig2059, Trigger_override_per_core, 8);
                /* disable_stalls=0 for real chip and 1 for self_test */
                MOD_PHYREG_p1c(pi, RxFeCtrl1, disable_stalls, 0);
                MOD_PHYREG_p1c(pi, RxFeCtrl1, swap_iq0, 1);
                /* sarAfePhaseSel has to be set to 0 for 6715 160MHz */
                MOD_PHYREG_p1c(pi, sarAfeCompCtrl0, sarAfePhaseSel, 0);
                MOD_PHYREG_p1c(pi, RadarTx2NclksBlank, tx2nclks_BlankEn, 1);
                MOD_PHYREG_p1c(pi, RadarTx2NclksBlank, tx2nclks_BlankExtTime, 0);
                MOD_PHYREG_p1c(pi, RfseqMode, CoreActv_override_percore, 1);
                MOD_PHYREG_p1c(pi, RfseqCoreActv2059, EnRx, 1);
                MOD_PHYREG_p1c(pi, RfseqCoreActv2059, EnTx, 0);
                MOD_PHYREG_p1c(pi, CoreConfig, CoreMask, 0);
                MOD_PHYREG_p1c(pi, CoreConfig, NumRxCores, 1);
                MOD_PHYREG_p1c(pi, CoreConfig, pasvRxCoreMask, 1);
                MOD_PHYREG_p1c(pi, AntDivConfig2059, Trigger_override_per_core, 1);
                MOD_PHYREG_p1c(pi, RadarDetectConfig3, gain_override_en, 1);
                if (is6715mch2) {
                    MOD_PHYREG_p1c(pi, RadarGainOverride,
                        gain_override_val, 0x38);
                } else {
                    MOD_PHYREG_p1c(pi, RadarGainOverride,
                        gain_override_val, 0x3d);
                }
                MOD_PHYREG_p1c(pi, RadarDetectConfig3, scan_mode, 1);
                MOD_PHYREG_p1c(pi, RadarSearchCtrl, radarEnable, 1);
                MOD_PHYREG_p1c(pi, RadarSubBandConfig2, stat_thresh, 0x2);
                phy_utils_write_phyreg_p1c(pi,
                    ACPHY_RadarBlankCtrl(pi->pubpi->phy_rev),
                    CHSPEC_IS20(chanspec_sc) ? 0x8019 :
                    CHSPEC_IS40(chanspec_sc) ? 0x8032 :
                    CHSPEC_IS80(chanspec_sc) ? 0x8064 : 0x80c8);
                if (CHSPEC_IS2G(chanspec) && CHSPEC_ISPHY5G6G(chanspec_sc)) {
                    phy_utils_write_phyreg_p1c(pi,
                        ACPHY_RadarBlankCtrl2(pi->pubpi->phy_rev), 0x8000);
                    MOD_PHYREG_p1c(pi, RadarSearchCtrl, ExternalBlankEn, 1);
                } else {
                    phy_utils_write_phyreg_p1c(pi,
                        ACPHY_RadarBlankCtrl2(pi->pubpi->phy_rev), 0xa000);
                }
                phy_utils_write_phyreg_p1c(pi,
                    ACPHY_RadarThresh0(pi->pubpi->phy_rev),
                    (uint16)((int16)st->rparams.radar_args.thresh0_sc));
                phy_utils_write_phyreg_p1c(pi,
                    ACPHY_RadarThresh1(pi->pubpi->phy_rev),
                    (uint16)((int16)st->rparams.radar_args.thresh1_sc));
                phy_utils_write_phyreg_p1c(pi,
                    ACPHY_RadarT3BelowMin(pi->pubpi->phy_rev), 0x14);
                phy_utils_write_phyreg_p1c(pi,
                    ACPHY_RadarT3Timeout(pi->pubpi->phy_rev),
                    CHSPEC_IS20(chanspec_sc) ? 0x258 :
                    CHSPEC_IS40(chanspec_sc) ? 0x4b0 :
                    CHSPEC_IS80(chanspec_sc) ? 0x960 : 0x12c0);
                phy_utils_write_phyreg_p1c(pi,
                    ACPHY_RadarResetBlankingDelay(pi->pubpi->phy_rev),
                    CHSPEC_IS20(chanspec_sc) ? 0x19 : CHSPEC_IS40(chanspec_sc) ?
                    0x32 : CHSPEC_IS80(chanspec_sc) ? 0x64 : 0xc8);
#endif /* !ATE_BUILD */
            } else {
                PHY_ERROR(("wl%d: %s: Unsupported PHY revision for BGDFS\n",
                    pi->sh->unit, __FUNCTION__));
            }
            break;
        case 0:
#ifndef ATE_BUILD
            MOD_PHYREG(pi, AntDivConfig2059, Trigger_override_per_core, 0);
            MOD_PHYREG(pi, RadarSearchCtrl_SC, radarEnable, 0);
            MOD_PHYREG(pi, RadarDetectConfig3_SC, scan_mode, 0);
            MOD_PHYREG(pi, RadarDetectConfig3_SC, gain_override_en, 0);
            MOD_PHYREG(pi, RadarBlankCtrl2_SC, blank_mode, 0);
#endif /* !ATE_BUILD */
            break;
        default:
            PHY_ERROR(("wl%d: %s: Unsupported radio revision %d\n",
                     pi->sh->unit, __FUNCTION__, RADIOREV(pi->pubpi->radiorev)));
            ASSERT(0);
    }

}

void
BCMATTACHFN(wlc_phy_nvram_avvmid_read)(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = (phy_info_acphy_t *)pi->u.pi_acphy;
    uint8 i, j, ant;
    uint8 core;
    char phy_var_name[20];
    /*    phy_info_acphy_t *pi_ac = pi->u.pi_acphy; */
    FOREACH_CORE(pi, core) {
        ant = phy_get_rsdbbrd_corenum(pi, core);
        (void)snprintf(phy_var_name, sizeof(phy_var_name), rstr_AvVmid_cD, ant);
        if ((PHY_GETVAR_SLICE(pi, phy_var_name)) != NULL) {
            for (i = 0; i < ACPHY_NUM_BANDS; i++) {
                for (j = 0; j < ACPHY_AVVMID_NVRAM_PARAMS; j++) {
                    pi_ac->sromi->avvmid_set_from_nvram[ant][i][j] =
                        (uint8) PHY_GETINTVAR_ARRAY_SLICE(pi, phy_var_name,
                        (ACPHY_AVVMID_NVRAM_PARAMS*i +j));
                }
            }
#ifndef BOARD_FLAGS3
            /* If the AV VMID values are populated in the NVRAM, use those values,
            even if boadflag is not set
            */
            if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
                BF3_AVVMID_FROM_NVRAM(pi_ac) = 1;
            }
#endif
        }
    }
}

#if defined(WLTEST)
void wlc_phy_get_avvmid_acphy(phy_info_t *pi, int32 *ret_int_ptr, wlc_avvmid_t avvmid_type,
        uint8 *core_sub_band)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
    uint8 avvmid_idx = 0;
    uint8 band_idx = core_sub_band[1];
    uint8 core = core_sub_band[0];
    avvmid_idx = (avvmid_type == AV) ? 0 : 1;
    *ret_int_ptr = (int32)(pi_ac->sromi->avvmid_set_from_nvram[core][band_idx][avvmid_idx]);
    return;
}

void wlc_phy_set_avvmid_acphy(phy_info_t *pi, uint8 *avvmid, wlc_avvmid_t avvmid_type)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
    uint8 core, sub_band_idx;
    uint8 avvmid_idx = 0;
    avvmid_idx = (avvmid_type == AV) ? 0 : 1;
    core = avvmid[0];
    sub_band_idx = avvmid[1];
    pi_ac->sromi->avvmid_set_from_nvram[core][sub_band_idx][avvmid_idx] = avvmid[2];
    /* Load Pdet related settings */
    wlc_phy_set_pdet_on_reset_acphy(pi);
}
#endif

void BCMATTACHFN(wlc_phy_nvram_vlin_params_read)(phy_info_t *pi)
{

    char phy_var_name2[20], phy_var_name3[20];
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
    uint8 core, ant;
    FOREACH_CORE(pi, core) {
        ant = phy_get_rsdbbrd_corenum(pi, core);
        if ((!TINY_RADIO(pi)) && BF3_VLIN_EN_FROM_NVRAM(pi_ac)) {
            (void)snprintf(phy_var_name2, sizeof(phy_var_name2),
                rstr_VlinPwr2g_cD, ant);
            if ((PHY_GETVAR_SLICE(pi, phy_var_name2)) != NULL) {
                pi_ac->chanmgri->cfg->vlinpwr2g_from_nvram =
                    (uint8) PHY_GETINTVAR_SLICE(pi, phy_var_name2);
            }
            (void)snprintf(phy_var_name3, sizeof(phy_var_name3),
                rstr_Vlinmask2g_cD, ant);
            if ((PHY_GETVAR_SLICE(pi, phy_var_name3)) != NULL) {
                pi_ac->chanmgri->data->vlinmask2g_from_nvram =
                    (uint8) PHY_GETINTVAR_SLICE(pi, phy_var_name3);
            }

            if (PHY_BAND5G_ENAB(pi)) {
                (void)snprintf(phy_var_name2, sizeof(phy_var_name2),
                    rstr_VlinPwr5g_cD, ant);
                if ((PHY_GETVAR_SLICE(pi, phy_var_name2)) != NULL) {
                    pi_ac->chanmgri->cfg->vlinpwr5g_from_nvram =
                        (uint8) PHY_GETINTVAR_SLICE(pi, phy_var_name2);
                }
                (void)snprintf(phy_var_name3, sizeof(phy_var_name3),
                    rstr_Vlinmask5g_cD, ant);
                if ((PHY_GETVAR_SLICE(pi, phy_var_name3)) != NULL) {
                    pi_ac->chanmgri->data->vlinmask5g_from_nvram =
                        (uint8) PHY_GETINTVAR_SLICE(pi, phy_var_name3);
                }
            }
        }
    }
}

static void
wlc_tiny_setup_coarse_dcc(phy_info_t *pi)
{
    uint8 phybw;
    uint8 core;

    /*
     * Settings required to use the RFSeq to trigger the coarse DCC
     * 4345TC Not used.
     * 4345A0 offset comparator has hysteresis and dc offset but is adequate for 5G
     * 4365-analog DCC
     */

    if ((!ACMAJORREV_4(pi->pubpi->phy_rev)) && (!ACMAJORREV_32(pi->pubpi->phy_rev)) &&
        (!ACMAJORREV_33(pi->pubpi->phy_rev))) {
        wlc_tiny_dc_static_WAR(pi);
    }

    /* DCC FSM Defaults */
    MOD_PHYREG(pi, BBConfig, dcc_wakeup_restart_en, 0);
    MOD_PHYREG(pi, BBConfig, dcc_wakeup_restart_delay, 10);

    /* Control via pktproc, instead of RFSEQ */
    MOD_PHYREG(pi, RfseqTrigger, en_pkt_proc_dcc_ctrl,  1);

    FOREACH_CORE(pi, core) {

        /* Disable overrides that may have been set during 2G cal */
        MOD_RADIO_REG_TINY(pi, RX_BB_2G_OVR_EAST, core, ovr_tia_offset_dac_pwrup, 0);
        MOD_RADIO_REG_TINY(pi, RX_BB_2G_OVR_EAST, core, ovr_tia_offset_dac, 0);
        if (ACMAJORREV_4(pi->pubpi->phy_rev) || ACMAJORREV_32(pi->pubpi->phy_rev) ||
            ACMAJORREV_33(pi->pubpi->phy_rev)) {
            MOD_RADIO_REG_20693(pi, RX_BB_2G_OVR_EAST, core,
                ovr_tia_offset_comp_pwrup, 0);
        } else {
            MOD_RADIO_REG_TINY(pi, RX_BB_2G_OVR_NORTH, core,
                ovr_tia_offset_comp_pwrup, 0);
        }
        MOD_RADIO_REG_TINY(pi, RX_BB_2G_OVR_EAST, core, ovr_tia_offset_dac, 0);
        MOD_RADIO_REG_TINY(pi, TIA_CFG8, core, tia_offset_comp_drive_strength, 1);

        /* Set idac LSB to (50nA * 4) ~ 0.2uA for 2G, (50nA * 12) ~ 0.6 uA for 5G */
        /* changed biasadj to 1 as coupled d.c. in loop is very less. */
        if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
            MOD_RADIO_REG_TINY(pi, TIA_CFG8, core, tia_offset_dac_biasadj,
            (CHSPEC_IS2G(pi->radio_chanspec)) ? 1 : 1);
        } else if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
            MOD_RADIO_REG_TINY(pi, TIA_CFG8, core, tia_offset_dac_biasadj,
            (CHSPEC_IS2G(pi->radio_chanspec)) ? 12 : 4);
        } else {
            MOD_RADIO_REG_TINY(pi, TIA_CFG8, core, tia_offset_dac_biasadj,
            (CHSPEC_IS2G(pi->radio_chanspec)) ? 4 : 12);
        }
    }
    if (ACMAJORREV_4(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, rx_tia_dc_loop_0, dac_sign, 1);
        MOD_PHYREG(pi, rx_tia_dc_loop_0, en_lock, 1);
        if (phy_get_phymode(pi) != PHYMODE_RSDB) {
            ACPHY_REG_LIST_START
                MOD_PHYREG_ENTRY(pi, rx_tia_dc_loop_1, dac_sign, 1)
                MOD_PHYREG_ENTRY(pi, rx_tia_dc_loop_1, en_lock, 1)
                MOD_PHYREG_ENTRY(pi, rx_tia_dc_loop_1, restart_gear, 6)
            ACPHY_REG_LIST_EXECUTE(pi);
        }
    }
    /* Set minimum idle gain incase of restart */
    MOD_PHYREG(pi, rx_tia_dc_loop_0, restart_gear, 6);

    /* 4365-analog DCC: loop through the cores */
    if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
        /* Loop through cores */
        FOREACH_CORE(pi, core) {
            MOD_PHYREGCE(pi, rx_tia_dc_loop_, core, dac_sign, 1);
            MOD_PHYREGCE(pi, rx_tia_dc_loop_, core, en_lock, 1);
            MOD_PHYREGCE(pi, rx_tia_dc_loop_, core, restart_gear, 6);
        }
    }

    if (IS20MHZ(pi))
        phybw = 0;
    else if (IS40MHZ(pi))
        phybw = 1;
    else
        phybw = 2;

    /*
     * Because FSM clock is PHY_BW dependant scale gear gain and loop count.
     *
     * Settings below assume:
     *    9 DCC FSM clock cycle latency and single pole  RC filter >=2MHz ala 4345B0.
     * (Valid also for 4345A0).
     */
    MOD_PHYREG(pi, rx_tia_dc_loop_gain_0, loop_gain_0, 15); /* disable */
    MOD_PHYREG(pi, rx_tia_dc_loop_gain_1, loop_gain_1, 2 + phybw);
    MOD_PHYREG(pi, rx_tia_dc_loop_gain_2, loop_gain_2, 4 + phybw);
    MOD_PHYREG(pi, rx_tia_dc_loop_gain_3, loop_gain_3, 5 + phybw);
    MOD_PHYREG(pi, rx_tia_dc_loop_gain_4, loop_gain_4, 6 + phybw);
    MOD_PHYREG(pi, rx_tia_dc_loop_gain_5, loop_gain_5, 8 + phybw);
    /* making Loop count of gear 1 because of CRDOT11ACPHY-1530 */
    MOD_PHYREG(pi, rx_tia_dc_loop_count_0, loop_count_0, 1); /* disable */
    MOD_PHYREG(pi, rx_tia_dc_loop_count_1, loop_count_1, (phybw > 1) ? 255 : (80 << phybw));
    MOD_PHYREG(pi, rx_tia_dc_loop_count_2, loop_count_2, (phybw > 1) ? 255 : (80 << phybw));
    MOD_PHYREG(pi, rx_tia_dc_loop_count_3, loop_count_3, (phybw > 1) ? 255 : (80 << phybw));
    MOD_PHYREG(pi, rx_tia_dc_loop_count_4, loop_count_4, (phybw > 1) ? 255 : (80 << phybw));
    MOD_PHYREG(pi, rx_tia_dc_loop_count_5, loop_count_5, (20 << phybw));
}

void
wlc_phy_mlua_adjust_acphy(phy_info_t *pi, bool btactive)
{
    uint8 zfuA1, zfuA1_log2, zfuA2, zfuA2_log2;
    uint8 mluA1, mluA1_log2, mluA2, mluA2_log2;
    uint8 zfuA0 = 0, zfuA3 = 0;
    uint8 mluA0 = 0, mluA3 = 0;

    /* Disable adjust if chanup is forced to disable */
    /* With auto mode, Disable ChanUpd for 43684B0. HW bug for BW160 */
    if ((pi->u.pi_acphy->chanmgri->chanup_ovrd == 0) ||
        ((pi->u.pi_acphy->chanmgri->chanup_ovrd == -1) && ACMAJORREV_47(pi->pubpi->phy_rev) &&
         ACMINORREV_1(pi) && CHSPEC_IS160(pi->radio_chanspec))) {
        return;
    }

    /* Disable this for now, there is some issue with BTcoex */
    if (btactive) {
        if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            mluA1 = 2; mluA1_log2 = 1; mluA2 = 2; mluA2_log2 = 0, mluA0 = 2, mluA3 = 2;
            zfuA1 = 2; zfuA1_log2 = 1; zfuA2 = 2; zfuA2_log2 = 1, zfuA0 = 2; zfuA3 = 2;
        } else {
            mluA1 = 2; mluA1_log2 = 1; mluA2 = 0; mluA2_log2 = 0;
            zfuA1 = 2; zfuA1_log2 = 1; zfuA2 = 2; zfuA2_log2 = 1;
        }
    } else {
        if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
            (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
            mluA1 = 4; mluA1_log2 = 2; mluA2 = 4; mluA2_log2 = 2; mluA0 = 4; mluA3 = 2;
            zfuA1 = 4; zfuA1_log2 = 2; zfuA2 = 4; zfuA2_log2 = 2; zfuA0 = 4; zfuA3 = 2;
        } else {
            mluA1 = 4; mluA1_log2 = 2; mluA2 = 4; mluA2_log2 = 2;
            zfuA1 = 4; zfuA1_log2 = 2; zfuA2 = 4; zfuA2_log2 = 2;
            if (ACMAJORREV_2(pi->pubpi->phy_rev) &&
                (RADIOID_IS(pi->pubpi->radioid, BCM2069_ID)) &&
                (RADIOREV(pi->pubpi->radiorev) == 0x2C) && PHY_XTAL_IS40M(pi)) {
                /* see http://confluence.broadcom.com/x/AljxEQ */
                mluA2_log2 = 3;
                mluA1_log2 = 3;
            }
        }
        if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
            !ACMAJORREV_128(pi->pubpi->phy_rev)) {
            mluA1 = 2; mluA2 = 0; zfuA1 = 2; zfuA2 = 2;
        }
    }

    /* Increase Channel Update ML mu */
    if (ACMAJORREV_0(pi->pubpi->phy_rev) && (ACMINORREV_0(pi) || ACMINORREV_1(pi))) {
        /* 4360 a0,b0 */
        MOD_PHYREG(pi, mluA, mluA1, mluA1);
        MOD_PHYREG(pi, mluA, mluA2, mluA2);
        /* zfuA register used to update channel for 256 QAM */
        MOD_PHYREG(pi, zfuA, zfuA1, zfuA1);
        MOD_PHYREG(pi, zfuA, zfuA2, zfuA2);
    } else if (ACMAJORREV_2(pi->pubpi->phy_rev) || ACMAJORREV_5(pi->pubpi->phy_rev)) {
        /* 4350 a0,b0 (log domain) */
        MOD_PHYREG(pi, mluA, mluA1, mluA1_log2);
        MOD_PHYREG(pi, mluA, mluA2, mluA2_log2);
        /* zfuA register used to update channel for 256 QAM */
        MOD_PHYREG(pi, zfuA, zfuA1, zfuA1_log2);
        MOD_PHYREG(pi, zfuA, zfuA2, zfuA2_log2);
    } else if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev) ||
               (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        MOD_PHYREG(pi, mluA, mluA0, mluA0);
        MOD_PHYREG(pi, mluA, mluA1, mluA1);
        MOD_PHYREG(pi, mluA, mluA2, mluA2);
        MOD_PHYREG(pi, mluA, mluA3, mluA3);
        /* zfuA register used to update channel for 256 QAM */
        MOD_PHYREG(pi, zfuA, zfuA0, zfuA0);
        MOD_PHYREG(pi, zfuA, zfuA1, zfuA1);
        MOD_PHYREG(pi, zfuA, zfuA2, zfuA2);
        MOD_PHYREG(pi, zfuA, zfuA3, zfuA3);
    } else {
    }
}

void
phy_ac_chanmgr_cal_init(phy_info_t *pi)
{
    if (ACMAJORREV_2(pi->pubpi->phy_rev) && (!PHY_IPA(pi)) &&
        (CHSPEC_IS80(pi->radio_chanspec)) &&
        (pi->u.pi_acphy->chanmgri->cfg->srom_txnoBW80ClkSwitch == 0)) {
            wlc_phy_modify_txafediv_acphy(pi, 6);
    }
}

void
phy_ac_chanmgr_cal_reset(phy_info_t *pi)
{
    uint8 ch = CHSPEC_CHANNEL(pi->radio_chanspec);
    uint32 fc = wf_channel2mhz(ch, CHSPEC_ISPHY5G6G(pi->radio_chanspec)
                ? WF_CHAN_FACTOR_5_G : WF_CHAN_FACTOR_2_4_G);
    if (ACMAJORREV_2(pi->pubpi->phy_rev) && (!PHY_IPA(pi)) &&
        (pi->u.pi_acphy->chanmgri->cfg->srom_txnoBW80ClkSwitch == 0)) {
        wlc_phy_afeclkswitch_sifs_delay(pi);
        if (!(fc == 5210 || fc == 5290) && (CHSPEC_IS80(pi->radio_chanspec)))
            wlc_phy_modify_txafediv_acphy(pi, 9);
        else if (CHSPEC_IS80(pi->radio_chanspec))
            wlc_phy_modify_txafediv_acphy(pi, 6);
    }
}

static void
phy_ac_chanmgr_bypass_itssi(phy_type_chanmgr_ctx_t *ctx, bool force)
{
    phy_ac_chanmgr_info_t *ci = (phy_ac_chanmgr_info_t *)ctx;
    ci->bypass_idle_tssi = force;
}

#if defined(WLTEST)
static int
phy_ac_chanmgr_get_smth(phy_type_chanmgr_ctx_t *ctx, int32* ret_int_ptr)
{
    phy_ac_chanmgr_info_t *ci = (phy_ac_chanmgr_info_t *)ctx;
    if (ACMAJORREV_4(ci->pi->pubpi->phy_rev) || ACMAJORREV_33(ci->pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(ci->pi->pubpi->phy_rev) && !ACMAJORREV_128(ci->pi->pubpi->phy_rev))) {
        *ret_int_ptr = ci->acphy_enable_smth;
        return BCME_OK;
    } else {
        PHY_ERROR(("Smth is not supported for this chip \n"));
        return BCME_UNSUPPORTED;
    }
}

static int
phy_ac_chanmgr_set_smth(phy_type_chanmgr_ctx_t *ctx, int8 int_val)
{
    phy_ac_chanmgr_info_t *ci = (phy_ac_chanmgr_info_t *)ctx;
    if (ACMAJORREV_4(ci->pi->pubpi->phy_rev) || ACMAJORREV_33(ci->pi->pubpi->phy_rev) ||
        (ACMAJORREV_GE47(ci->pi->pubpi->phy_rev) && !ACMAJORREV_128(ci->pi->pubpi->phy_rev))) {
        if (((int_val > SMTH_FOR_TXBF) || (int_val < SMTH_DISABLE)) ||
            PHY_AS_80P80(ci->pi, ci->pi->radio_chanspec)) {
            PHY_ERROR(("Smth %d is not supported \n", (uint16)int_val));
            return BCME_UNSUPPORTED;
        } else {
            wlc_phy_smth(ci->pi, int_val, SMTH_NODUMP);
            return BCME_OK;
        }
    } else {
        PHY_ERROR(("Smth is not supported for this chip \n"));
        return BCME_UNSUPPORTED;
    }
}
#endif /* defined(WLTEST) */

bool
phy_ac_chanmgr_get_val_nonbf_logen_mode(phy_ac_chanmgr_info_t *chanmgri)
{
    return chanmgri->cfg->srom_nonbf_logen_mode_en;
}

/* sets chanspec of scan core; returns error status */
int
phy_ac_chanmgr_set_val_sc_chspec(phy_ac_chanmgr_info_t *chanmgri, int32 set_val)
{
    chanmgri->radio_chanspec_sc = (chanspec_t)set_val;

    return BCME_OK;
}

/* gets scan core chanspec in pointer ret_val parameter passed; returns error status */
int
phy_ac_chanmgr_get_val_sc_chspec(phy_ac_chanmgr_info_t *chanmgri, int32 *ret_val)
{
    *ret_val = (int32)chanmgri->radio_chanspec_sc;
    return BCME_OK;
}

/* sets phymode; eg. to 3+1 or 4x4; returns error status */
int
phy_ac_chanmgr_set_val_phymode(phy_ac_chanmgr_info_t *chanmgri, int32 set_val)
{
    phy_info_t *pi = chanmgri->pi;
    if (ACMAJORREV_51_129_131(pi->pubpi->phy_rev)) {
        return BCME_UNSUPPORTED;
    }

#ifdef WL_AIR_IQ
    if ((phy_get_phymode(pi) == PHYMODE_BGDFS && set_val != PHYMODE_BGDFS) ||
        (phy_get_phymode(pi) != PHYMODE_BGDFS && set_val == PHYMODE_BGDFS)) {
        wlc_phy_save_regs_3plus1(pi, set_val);
    }
#endif /* WL_AIR_IQ */
    phy_set_phymode(pi, (uint16) set_val);
    phy_ac_chanmgr_set_phymode(pi, pi->radio_chanspec,
            chanmgri->radio_chanspec_sc, (uint16) set_val);
    return BCME_OK;
}

/* gets phymode in pointer ret_val parameter passed; returns error status */
int
phy_ac_chanmgr_get_val_phymode(phy_ac_chanmgr_info_t *chanmgri, int32 *ret_val)
{
    phy_info_t *pi = chanmgri->pi;

    *ret_val = (int32) phy_get_phymode(pi);
    return BCME_OK;
}

static void
phy_ac_chanmgr_set_spexp_matrix(phy_info_t *pi)
{
    uint32 svmp_start_addr = 0x1000;
    uint32 svmp_m4_idx = 0x4;
    uint32 spexp_offs[3] = {  0, 680, 456+680};
    //uint32 spexp_size[3] = {680, 456, 344};
    uint32 txv_config[3][3] = {{0x1000700, 0x2004008, 0x0000055},
        {0x1000680, 0x2004008, 0x0000039},
        {0x1000480, 0x2004008, 0x000002B}};
    uint32 zeros[5] = {0, 0, 0, 0, 0};
    int16 Q43[2*12] = {591, 0, 591, 0, 591, 0, 591, 0, 591, 0, 0, -591,
        -591, 0, 0, 591, 591, 0, -591, 0, 591, 0, -591, 0};
    //int16 Q43[2*12] = {836, 0, 0, 0, 836, 0, 0, 0, 0, 0, 836, 0,
    //    0, 0, 836, 0, 591, 0, -591, 0, -591, 0, 591, 0};
    int16 Q42[2*8]  = {724, 0, 724, 0, 724, 0, 724, 0, 724, 0, 0, 724, -724, 0, 0, -724};
    int16 Q32[2*6]  = {887, 0, 0, 0, 887, 0, 512, 0, 1024, 0, -512, 0};
    int16 csd_phasor[2*16] = {1024, 0, 946, 392, 724, 724, 392, 946, 0, 1024, -392, 946,
        -724, 724, -946, 392, -1024, 0, -946, -392, -724, -724, -392, -946, 0, -1024,
        392, -946, 724, -724, 946, -392};
    int16 k, m, n, j, ncol, nrow, ntones = 56;
    int16 Qr, Qi, *Q = NULL, csd_idx[3];
    uint32 Qcsd[12], svmp_addr[2] = {0x22552200, 0x0000228E};

    // steering matrix is of size S1.14
    //  Q_4x3 = 1/sqrt(3)*[1, 1, 1; 1, -j, -1; 1, -1, 1; 1, j, -1];
    //  Q_4x2 = 1/sqrt(2)*[1, 1; 1, j; 1, -1; 1, -j];
    //  Q_3x2 = [sqrt(3/4), sqrt(1/4); 0, 1; sqrt(3/4), -sqrt(1/4)];

    // These 3 special spacial expansion matrix are stored in M4
    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 1,
            0x8000, 32, &svmp_m4_idx);

    for (k = 0; k < 3; k++) {
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 3,
            svmp_start_addr + spexp_offs[k], 32, &(txv_config[k][0]));
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, 5,
            svmp_start_addr + spexp_offs[k] + 3, 32, &zeros);

        switch (k) {
        case 0:
            ncol = 3; nrow = 4; Q = Q43;
            break;
        case 1:
            ncol = 2; nrow = 4; Q = Q42;
            break;
        case 2:
            ncol = 2; nrow = 3; Q = Q32;
            break;
        }

        // initialize csd_idx to start from tone -28
        csd_idx[1] = -28;
        for (j = 0; j < ntones; j++) {
            // csd
            csd_idx[1] = (csd_idx[1]+64) & 0xf;
            csd_idx[0] = (csd_idx[1] << 1) & 0xf;
            csd_idx[2] = (csd_idx[0] + csd_idx[1]) & 0xf;
            for (m = 0; m < nrow; m++) {
                for (n = 0; n < ncol; n++) {
                    if (m == 0) {
                        Qr = Q[(n*nrow+m)*2];
                        Qi = Q[(n*nrow+m)*2 + 1];
                    } else {
                        Qr = (((Q[(n*nrow+m)*2]*csd_phasor[csd_idx[m-1]*2] -
                        Q[(n*nrow+m)*2 + 1]*csd_phasor[csd_idx[m-1]*2+1])
                        >> 9) + 1) >> 1;
                        Qi = (((Q[(n*nrow+m)*2]*csd_phasor[csd_idx[m-1]*2+1]
                        + Q[(n*nrow+m)*2+1]*csd_phasor[csd_idx[m-1]*2])
                        >> 9) + 1) >> 1;
                    }
                    Qr = (Qr > 0)? Qr : (Qr + (1 << 12));
                    Qi = (Qi > 0)? Qi : (Qi + (1 << 12));
                    Qcsd[n*nrow + m] = ((Qr & 0xfff) << 4) |
                        ((Qi & 0xfff) << 20);
                }
            }
            csd_idx[1] += (j == ((ntones >> 1) - 1))? 2: 1;

            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_SVMPMEMS, ncol*nrow,
            svmp_start_addr + spexp_offs[k] + 8 + ncol*nrow*j, 32, &Qcsd);
        }
    }

    // writing txbf_user index 0x60, 0x61, 0x62 svmp address
    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_BFMUSERINDEX, 2, 0x1030, 32, svmp_addr);
}

static int
phy_ac_chanmgr_switch_phymode_acphy(phy_info_t *pi, uint32 phymode)
{
    si_t *sih = pi->sh->sih;
    uint32 prev_phymode = (si_core_cflags(sih, 0, 0) & SICF_PHYMODE) >> SICF_PHYMODE_SHIFT;

    if (phymode != prev_phymode)
        si_core_cflags(sih, SICF_PHYMODE, phymode << SICF_PHYMODE_SHIFT);

    return BCME_OK;
}

/* report virtual core related capabilities. It only hosts 80p80 capability now */
int
phy_ac_chanmgr_get_val_phy_vcore(phy_ac_chanmgr_info_t *chanmgri, int32 *ret_val)
{
    phy_info_t *pi = chanmgri->pi;
    phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);
    bool en_sw_txrxchain_mask =
        ((pi->sh->boardflags4 & BFL4_SROM13_EN_SW_TXRXCHAIN_MASK) != 0);

    *ret_val = ((uint16)(PHY_AS_80P80_CAP(pi)) << 12);
    if (ACMAJORREV_33(pi->pubpi->phy_rev) || (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
        !ACMAJORREV_128(pi->pubpi->phy_rev))) {
        *ret_val |= (pi->pubpi->phy_corenum << 8) +
            ((stf_shdata->hw_phytxchain & (en_sw_txrxchain_mask ?
                pi->sromi->sw_txchain_mask:0xf)) << 4) +
            (stf_shdata->hw_phyrxchain & (en_sw_txrxchain_mask ?
                pi->sromi->sw_rxchain_mask:0xf));
    } else {
        /* 4349 and other phyrevs with 80p80/160mhz mode can add code here */
    }

    return BCME_OK;
}

void wlc_phy_set_rfseqext_tbl(phy_info_t *pi, uint8 mode)
{
    /* TCL: ocl_rfseq_from_rfseqext */
    uint8 core;
    uint8 idx;
    uint32 write_val[2];
    uint32 rfseqext[2][11][2];

    uint32 fc = wf_channel2mhz(CHSPEC_CHANNEL(pi->radio_chanspec),
                               CHSPEC_IS6G(pi->radio_chanspec) ? WF_CHAN_FACTOR_6_G
                             : CHSPEC_IS5G(pi->radio_chanspec) ? WF_CHAN_FACTOR_5_G
                                                              :  WF_CHAN_FACTOR_2_4_G);

    if (ISSIM_ENAB(pi->sh->sih)) {
        if (CHSPEC_IS2G(pi->radio_chanspec)) {
            memcpy(rfseqext, rfseqext_veloce_2g, sizeof(rfseqext));
        } else {
            memcpy(rfseqext, rfseqext_veloce_5g, sizeof(rfseqext));
        }
    } else if (pi->sromi->dacdiv10_2g && CHSPEC_IS2G(pi->radio_chanspec)) {
        if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
            memcpy(rfseqext, rfseqext_rev51_2G_dacdiv10, sizeof(rfseqext));
        } else {
            PHY_ERROR(("%s: ERROR: No DAC DIV 10 support yet for ACMAJORREV: %d\n",
                __FUNCTION__, pi->pubpi->phy_rev));
            ASSERT(0);
        }
    } else {
        if (ACMAJORREV_47_129(pi->pubpi->phy_rev)) {
            if (fc >= 6535) {
                memcpy(rfseqext, rfseqext_rev47_6gu, sizeof(rfseqext));
            } else if (fc == 6025) {
                memcpy(rfseqext, rfseqext_rev47_6gl_special, sizeof(rfseqext));
            } else if (fc >= 5900) {
                memcpy(rfseqext, rfseqext_rev47_6gl, sizeof(rfseqext));
            } else {
                memcpy(rfseqext, rfseqext_rev47_2g5g, sizeof(rfseqext));
            }
        } else if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            memcpy(rfseqext, rfseqext_rev130_2g5g, sizeof(rfseqext));
        } else if (ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
            if (fc >= 6535) {
                memcpy(rfseqext, rfseqext_rev51_6gu, sizeof(rfseqext));
            } else if (fc == 6025) {
                memcpy(rfseqext, rfseqext_rev51_6gl_special, sizeof(rfseqext));
            } else if (fc >= 5900) {
                memcpy(rfseqext, rfseqext_rev51_6gl, sizeof(rfseqext));
            } else if (fc == 5815) {
                memcpy(rfseqext, rfseqext_rev51_2g5g_special, sizeof(rfseqext));
            } else {
                memcpy(rfseqext, rfseqext_rev51_2g5g, sizeof(rfseqext));
            }
        } else {
            memcpy(rfseqext, rfseqext_rev47_2g5g, sizeof(rfseqext));
        }
    }

    /* low_adc_rate_en=0/1 */
    if (mode >= 2) {
        PHY_ERROR(("%s: ERROR: Invalid low_adc_rate_en value (%d) failed\n",
            __FUNCTION__, mode));
        ASSERT(0);
        return;
    }

    FOREACH_CORE(pi, core) {
        /* update rfseqext table for afediv configuration */
        if (pi->pubpi->slice == DUALMAC_MAIN) {
            /* Setingt the bw 20MHz DAC clock for dac_clk_x2_mode */
            for (idx = 0; idx < RFSEQEXT_TBL_SZ_PER_CORE_28NM; idx++) {
                write_val[0] = rfseqext[mode][idx][1];
                write_val[1] = rfseqext[mode][idx][0];
                wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQEXT, 1,
                                          core*RFSEQEXT_TBL_SZ_PER_CORE_28NM+idx,
                                          60, write_val);
            }
        }
    }
}

static void wlc_phy_td_sfo_eco(phy_info_t *pi)
{
    uint64 rxfarr_ratio;
    uint32 td_sfo_be;
    uint16 rf_updh_cmd_en_maj47[] = {0x85, 0x2a, 0x07, 0x0a, 0x00, 0x08, 0x2b, 0x86, 0x1f};
    uint16 rf_updh_dly_en_maj47[] = {0x02, 0x01, 0x02, 0x02, 0x02, 0x10, 0x01, 0x02, 0x01};
    uint16 rf_updh_cmd_dis_maj47[] = {0x07, 0x0a, 0x00, 0x08, 0x2a, 0x2b, 0x1f, 0x1f, 0x1f};
    uint16 rf_updh_dly_dis_maj47[] = {0x02, 0x02, 0x02, 0x01, 0x04, 0x01, 0x01, 0x01, 0x01};

    uint16 fcw_hi, fcw_lo;
    uint16 rx1Spare_val;

    if (ACMAJORREV_47_51_129_130_131(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, td_sfo_be_corr, td_sfo_be_enable,
            (pi->u.pi_acphy->td_sfo_corr_en != 0));
        MOD_PHYREG(pi, td_sfo_corr, td_sfo_corr_en,
            (pi->u.pi_acphy->td_sfo_corr_en != 0));
        MOD_PHYREG(pi, PhaseTrackOffset, tdSfoAdvRetAdj, 1);

        /* Timing for applying CFO */
        if (ACMAJORREV_47(pi->pubpi->phy_rev) || ACMAJORREV_51_131(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, td_sfo_corr, td_sfo_corr_use_fineCFOatHTAGC, 0);
        } else if (ACMAJORREV_129_130(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, td_sfo_corr, td_sfo_corr_use_fineCfo_mode, 2);

            /* Disable TDSFO if lesi is kicked in */
            MOD_PHYREG(pi, td_sfo_corr, td_sfo_corr_lesi_disable, 1);
        }

        fcw_hi = READ_PHYREG(pi, rxFarrowDeltaPhase_hi);
        fcw_lo = READ_PHYREG(pi, rxFarrowDeltaPhase_lo);
        rxfarr_ratio = (1 << N_BITS_RX_FARR) + ((fcw_hi & 0xFF) << 16) +
            ((fcw_lo & 0xFFFF));

        /* Extra 1 bit left shift to help in rounding */
        math_uint64_divide(&td_sfo_be, (uint32)1 << 7, (uint32) 0, (uint32)rxfarr_ratio);
        td_sfo_be = (td_sfo_be + 1) >> 1;
        MOD_PHYREG(pi, td_sfo_be_corr, td_sfo_be, td_sfo_be & 0x7FFF);

        /* MAC will use following values to enable tdsfo */
        if (pi->u.pi_acphy->td_sfo_corr_en != 0) {
            wlapi_bmac_write_shm(pi->sh->physhim, M_PHYREG_TDSFO_VAL(pi),
                READ_PHYREG(pi, td_sfo_be_corr));
        } else {
            wlapi_bmac_write_shm(pi->sh->physhim, M_PHYREG_TDSFO_VAL(pi), 0);
        }

        /* Reduce pktgain set len to compensate extra latency from farrow reset */
        /* Only needed for phybw = 160M */
        if (CHSPEC_IS160(pi->radio_chanspec)) {
            if (pi->u.pi_acphy->td_sfo_corr_en) {
                WRITE_PHYREG(pi, pktgainSettleLen, 0x30);
            } else {
                WRITE_PHYREG(pi, pktgainSettleLen, 0x40);
            }
        }

        /* tdsfo is causing rx badfcs after receing random number of pkts, farrow reset
         * at the end of each pkt is a WAR. UPDATEGAINH is called at the end of each pkt,
         * so inserting farrow reset commands in UPDATEGAINH sequence.
         * td_sfo_corr_en == 1 ==> TDSFO SW war
         * td_sfo_corr_en == 2 ==> TDSFO HW reset (B0 cannot work with this mode)
         */
        if (pi->u.pi_acphy->td_sfo_corr_en == 1) {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8,
                0x30, 16, rf_updh_cmd_en_maj47);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8,
                0xa0, 16, rf_updh_dly_en_maj47);
        } else {
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8,
                0x30, 16, rf_updh_cmd_dis_maj47);
            wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 8,
                0xa0, 16, rf_updh_dly_dis_maj47);
        }
        /* Need TD-SFO perpkt reset if TD-SFO is enabled; for 6715 should be 0 */
        if (!ACMAJORREV_130(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, td_sfo_corr, td_sfo_perpktrst_en,
                pi->u.pi_acphy->td_sfo_corr_en == 2);
        }

        /* Enable tdsfo_reset_all bit from 43684 B1 fixes, B0 needs SW war */
        if (ACMAJORREV_47(pi->pubpi->phy_rev) || ACMAJORREV_51(pi->pubpi->phy_rev)) {
            rx1Spare_val = READ_PHYREG(pi, rx1Spare);
            rx1Spare_val = rx1Spare_val | 0x4;
            MOD_PHYREG(pi, rx1Spare, Spare, rx1Spare_val);
        } else if (ACMAJORREV_129_130(pi->pubpi->phy_rev) ||
                   ACMAJORREV_131(pi->pubpi->phy_rev)) {
            MOD_PHYREG(pi, td_sfo_corr, td_sfo_reset_all, 1);
        }
    } else {
        MOD_PHYREG(pi, td_sfo_corr_en, td_sfo_corr_en, pi->u.pi_acphy->td_sfo_corr_en);
    }
}

int
phy_ac_chanmgr_force_td_sfo(phy_info_t *pi, bool set, uint16 val)
{
    int ret = 0;

    if (set) {
        pi->u.pi_acphy->td_sfo_corr_en = val;
        wlc_phy_td_sfo_eco(pi);
    } else {
        ret = pi->u.pi_acphy->td_sfo_corr_en;
    }
    return ret;
}

int
phy_tdcs_is_enable(phy_info_t *pi)
{
    return pi->u.pi_acphy->chanmgri->tdcs_160_en;
}

/* Called via IOVAR */
int
phy_ac_chanmgr_force_tdcs_160m(phy_info_t *pi, bool set, int8 val)
{
    /* wl tdcs_160m = 1  (default, enable TDCS and set front class.)
       wl tdcs_160m = 0  (disable TDCS and reset front class.)
       wl tdcs_160m = -1 (always disable TDCS for 160M even when 160M STA disassoc.)
     */
    if (set) {
        if (val != 1 && val != 0 && val != -1) {
            return 0;
        }
        pi->u.pi_acphy->chanmgri->tdcs_160_en = val;
        phy_ac_chanmgr_tdcs_160m_rev47(pi);
        return 0;
    } else {
        return pi->u.pi_acphy->chanmgri->tdcs_160_en;
    }
}

/* Called by DRV top level (wlc.c) */
static void
phy_ac_chanmgr_tdcs_enable_160m(phy_info_t *pi, bool set_val)
{
    uint8 val = set_val ? 1 : 0;

    /* Ignore the call if tdcs_160_en = -1 (force off) */
    if (pi->u.pi_acphy->chanmgri->tdcs_160_en != -1) {
        if (pi->u.pi_acphy->chanmgri->tdcs_160_en != val) {
            pi->u.pi_acphy->chanmgri->tdcs_160_en = val;

            if (CHSPEC_IS160(pi->radio_chanspec))
                phy_ac_chanmgr_tdcs_160m_rev47(pi);
        }
    }
}

/* [43684] WAR for 160MHz TDCS+LESI classification bug
 * Cause PER hump with LESI on. JIRA: HW43684-541
 * No 160MHz STA: enable TDCS and set front classification to primary channel.
 * 160MHz STA join: disable TDCS and reset front classification.
 */
static void
phy_ac_chanmgr_tdcs_160m_rev47(phy_info_t *pi)
{
    uint8 mac_suspend;

    if (!pi->sh->clk) return;
    if (!(ACMAJORREV_47(pi->pubpi->phy_rev) && ACMINORREV_LE(pi, 2))) return;
    if (!CHSPEC_IS160(pi->radio_chanspec)) return;

    /* Below code only executed for 160mhz */

    mac_suspend = (R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC);
    if (mac_suspend) wlapi_suspend_mac_and_wait(pi->sh->physhim);

    /* tdcs_160_en = {0, -1} shoudl disable chan_smooth */
    if (pi->u.pi_acphy->chanmgri->tdcs_160_en == 1)
        MOD_PHYREG(pi, chnsmCtrl0, chan_smooth_enable, 1);
    else
        MOD_PHYREG(pi, chnsmCtrl0, chan_smooth_enable, 0);

    phy_ac_chanmgr_class_rev47(pi);

    wlc_phy_resetcca_acphy(pi);
    if (mac_suspend) wlapi_enable_mac(pi->sh->physhim);
}

static void
phy_ac_chanmgr_class_rev47(phy_info_t *pi)
{
    uint8 n, class = 0, ch = wf_chspec_ctlchan(pi->radio_chanspec);

    if (!pi->sh->clk) return;
    if (!(ACMAJORREV_47(pi->pubpi->phy_rev) && ACMINORREV_LE(pi, 2))) return;

    /* force Calssifier to primary for 160mhz when tdcs is ON */
    if (CHSPEC_IS160(pi->radio_chanspec) &&
        (pi->u.pi_acphy->chanmgri->tdcs_160_en == 1)) {
        n = (ch - (CHSPEC_CHANNEL(pi->radio_chanspec) - 14)) / 4;
        class = 1 << n;
    }

    MOD_PHYREG(pi, RxFrontEndDebug2, forceFrontClass, class);
}

static void
phy_ac_chanmgr_pad_online_enable(phy_info_t *pi, bool set_val, bool up_check)
{
    uint8 core = 0;
    bool suspend;

    if (!((ACMAJORREV_128(pi->pubpi->phy_rev) || ACMAJORREV_129(pi->pubpi->phy_rev)) &&
        PHY_PAPDEN(pi)))
        return;
    if (up_check && !pi->sh->up) return;

    suspend = !(R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC);
    if (!suspend)
        wlapi_suspend_mac_and_wait(pi->sh->physhim);

    if (set_val == 0) {
        FOREACH_CORE(pi, core) {
            if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
                RADIO_REG_LIST_START
                    /* Remove 2G and 5G overrides */
                    MOD_RADIO_REG_20709_ENTRY(pi, TX2G_IPA_CFG_OVR,
                        core, ovr_tx2g_ipa_bias_pu, 0)
                    MOD_RADIO_REG_20709_ENTRY(pi, TX2G_IPA_CFG_OVR,
                        core, ovr_tx2g_ipa_biasgen_pu, 0)
                    MOD_RADIO_REG_20709_ENTRY(pi, TX2G_CFG1_OVR, core,
                        ovr_tx2g_bias_pu, 0)
                    MOD_RADIO_REG_20709_ENTRY(pi, TX2G_CFG1_OVR, core,
                        ovr_tx2g_pad_pu, 0)
                    MOD_RADIO_REG_20709_ENTRY(pi, TX5G_IPA_CFG_OVR,
                        core, ovr_tx5g_ipa_bias_pu, 0)
                    MOD_RADIO_REG_20709_ENTRY(pi, TX5G_IPA_CFG_OVR,
                        core, ovr_tx5g_ipa_biasgen_pu, 0)
                    MOD_RADIO_REG_20709_ENTRY(pi, TX5G_CFG1_OVR, core,
                        ovr_tx5g_bias_pu, 0)
                    MOD_RADIO_REG_20709_ENTRY(pi, TX5G_CFG1_OVR, core,
                        ovr_tx5g_pad_pu, 0)
                RADIO_REG_LIST_EXECUTE(pi, core);
            } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
                RADIO_REG_LIST_START
                    /* Remove 2G and 5G overrides */
                    MOD_RADIO_REG_20707_ENTRY(pi, TX2G_IPA_CFG_OVR,
                        core, ovr_tx2g_ipa_bias_pu, 0)
                    MOD_RADIO_REG_20707_ENTRY(pi, TX2G_IPA_CFG_OVR,
                        core, ovr_tx2g_ipa_biasgen_pu, 0)
                    MOD_RADIO_REG_20707_ENTRY(pi, TX2G_CFG1_OVR, core,
                        ovr_tx2g_bias_pu, 0)
                    MOD_RADIO_REG_20707_ENTRY(pi, TX2G_CFG1_OVR, core,
                        ovr_tx2g_pad_pu, 0)
                    MOD_RADIO_REG_20707_ENTRY(pi, TX5G_IPA_CFG_OVR,
                        core, ovr_tx5g_ipa_bias_pu, 0)
                    MOD_RADIO_REG_20707_ENTRY(pi, TX5G_IPA_CFG_OVR,
                        core, ovr_tx5g_ipa_biasgen_pu, 0)
                    MOD_RADIO_REG_20707_ENTRY(pi, TX5G_CFG1_OVR, core,
                        ovr_tx5g_bias_pu, 0)
                    MOD_RADIO_REG_20707_ENTRY(pi, TX5G_CFG1_OVR, core,
                        ovr_tx5g_pad_pu, 0)
                RADIO_REG_LIST_EXECUTE(pi, core);
            }
        }
    } else {
        /* IPA heat transient issue: keep the PAD online all the time
         * to improve TxEVM floor.
         */
        uint16 tmp[2] = {0x0, 0x1f};
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0xfc, 16, &tmp[0]);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x109, 16, &tmp[1]);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x10c, 16, &tmp[0]);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0xfd, 16, &tmp[0]);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x10a, 16, &tmp[1]);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x10d, 16, &tmp[0]);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0xfe, 16, &tmp[0]);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x10b, 16, &tmp[1]);
        wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_RFSEQ, 1, 0x10e, 16, &tmp[0]);

        if (CHSPEC_IS2G(pi->radio_chanspec)) {
            FOREACH_CORE(pi, core) {
                if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
                    RADIO_REG_LIST_START
                        /* Remove 5G overrides */
                        MOD_RADIO_REG_20709_ENTRY(pi, TX5G_IPA_CFG_OVR,
                            core, ovr_tx5g_ipa_bias_pu, 0)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX5G_IPA_CFG_OVR,
                            core, ovr_tx5g_ipa_biasgen_pu, 0)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX5G_CFG1_OVR, core,
                            ovr_tx5g_bias_pu, 0)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX5G_CFG1_OVR, core,
                            ovr_tx5g_pad_pu, 0)

                        MOD_RADIO_REG_20709_ENTRY(pi, TX2G_IPA_REG0, core,
                            tx2g_ipa_bias_pu, 1)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX2G_IPA_CFG_OVR,
                            core, ovr_tx2g_ipa_bias_pu, 1)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX2G_IPA_REG0, core,
                            tx2g_ipa_biasgen_pu, 1)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX2G_IPA_CFG_OVR,
                            core, ovr_tx2g_ipa_biasgen_pu, 1)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX2G_MIX_REG0, core,
                            tx2g_bias_pu, 1)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX2G_CFG1_OVR, core,
                            ovr_tx2g_bias_pu, 1)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX2G_PAD_REG0, core,
                            tx2g_pad_pu, 1)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX2G_CFG1_OVR, core,
                            ovr_tx2g_pad_pu, 1)
                    RADIO_REG_LIST_EXECUTE(pi, core);
                } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
                    RADIO_REG_LIST_START
                        /* Remove 5G overrides */
                        MOD_RADIO_REG_20707_ENTRY(pi, TX5G_IPA_CFG_OVR,
                            core, ovr_tx5g_ipa_bias_pu, 0)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX5G_IPA_CFG_OVR,
                            core, ovr_tx5g_ipa_biasgen_pu, 0)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX5G_CFG1_OVR, core,
                            ovr_tx5g_bias_pu, 0)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX5G_CFG1_OVR, core,
                            ovr_tx5g_pad_pu, 0)

                        MOD_RADIO_REG_20707_ENTRY(pi, TX2G_IPA_REG0, core,
                            tx2g_ipa_bias_pu, 1)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX2G_IPA_CFG_OVR,
                            core, ovr_tx2g_ipa_bias_pu, 1)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX2G_IPA_REG0, core,
                            tx2g_ipa_biasgen_pu, 1)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX2G_IPA_CFG_OVR,
                            core, ovr_tx2g_ipa_biasgen_pu, 1)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX2G_MIX_REG0, core,
                            tx2g_bias_pu, 1)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX2G_CFG1_OVR, core,
                            ovr_tx2g_bias_pu, 1)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX2G_PAD_REG0, core,
                            tx2g_pad_pu, 1)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX2G_CFG1_OVR, core,
                            ovr_tx2g_pad_pu, 1)
                    RADIO_REG_LIST_EXECUTE(pi, core);
                }
            }
        } else {
            FOREACH_CORE(pi, core) {
                if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
                    RADIO_REG_LIST_START
                        /* Remove 2G overrides */
                        MOD_RADIO_REG_20709_ENTRY(pi, TX2G_IPA_CFG_OVR,
                            core, ovr_tx2g_ipa_bias_pu, 0)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX2G_IPA_CFG_OVR,
                            core, ovr_tx2g_ipa_biasgen_pu, 0)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX2G_CFG1_OVR, core,
                            ovr_tx2g_bias_pu, 0)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX2G_CFG1_OVR, core,
                            ovr_tx2g_pad_pu, 0)

                        MOD_RADIO_REG_20709_ENTRY(pi, TX5G_IPA_REG0, core,
                            tx5g_ipa_bias_pu, 1)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX5G_IPA_CFG_OVR,
                            core, ovr_tx5g_ipa_bias_pu, 1)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX5G_IPA_REG0, core,
                            tx5g_ipa_biasgen_pu, 1)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX5G_IPA_CFG_OVR,
                            core, ovr_tx5g_ipa_biasgen_pu, 1)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX5G_MIX_REG0, core,
                            tx5g_bias_pu, 1)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX5G_CFG1_OVR, core,
                            ovr_tx5g_bias_pu, 1)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX5G_PAD_REG0, core,
                            tx5g_pad_pu, 1)
                        MOD_RADIO_REG_20709_ENTRY(pi, TX5G_CFG1_OVR, core,
                            ovr_tx5g_pad_pu, 1)
                    RADIO_REG_LIST_EXECUTE(pi, core);
                } else if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
                    RADIO_REG_LIST_START
                        /* Remove 2G overrides */
                        MOD_RADIO_REG_20707_ENTRY(pi, TX2G_IPA_CFG_OVR,
                            core, ovr_tx2g_ipa_bias_pu, 0)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX2G_IPA_CFG_OVR,
                            core, ovr_tx2g_ipa_biasgen_pu, 0)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX2G_CFG1_OVR, core,
                            ovr_tx2g_bias_pu, 0)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX2G_CFG1_OVR, core,
                            ovr_tx2g_pad_pu, 0)

                        MOD_RADIO_REG_20707_ENTRY(pi, TX5G_IPA_REG0, core,
                            tx5g_ipa_bias_pu, 1)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX5G_IPA_CFG_OVR,
                            core, ovr_tx5g_ipa_bias_pu, 1)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX5G_IPA_REG0, core,
                            tx5g_ipa_biasgen_pu, 1)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX5G_IPA_CFG_OVR,
                            core, ovr_tx5g_ipa_biasgen_pu, 1)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX5G_MIX_REG0, core,
                            tx5g_bias_pu, 1)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX5G_CFG1_OVR, core,
                            ovr_tx5g_bias_pu, 1)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX5G_PAD_REG0, core,
                            tx5g_pad_pu, 1)
                        MOD_RADIO_REG_20707_ENTRY(pi, TX5G_CFG1_OVR, core,
                            ovr_tx5g_pad_pu, 1)
                    RADIO_REG_LIST_EXECUTE(pi, core);
                }
            }
        }
    }
    wlc_phy_resetcca_acphy(pi);

    if (!suspend)
        wlapi_enable_mac(pi->sh->physhim);
}

int
phy_ac_chanmgr_iovar_get_chanup_ovrd(phy_info_t *pi, int32 *ret_val)
{
    *ret_val = (int32)pi->u.pi_acphy->chanmgri->chanup_ovrd;

    return BCME_OK;
}

int
phy_ac_chanmgr_iovar_set_chanup_ovrd(phy_info_t *pi, int32 set_val)
{
    if ((set_val < -2) || (set_val > 1))
        return BCME_ERROR;

    pi->u.pi_acphy->chanmgri->chanup_ovrd = set_val;
    if (set_val == -1) {
    } else if (set_val == 0) {
        WRITE_PHYREG(pi, mluA, 0x0004);
        WRITE_PHYREG(pi, zfuA, 0x0004);
    } else if (set_val == 1) {
        WRITE_PHYREG(pi, mluA, 0x2024);
        WRITE_PHYREG(pi, zfuA, 0x2224);
    }

    return BCME_OK;
}

static void wlc_phy_ul_mac_aided_war(phy_info_t *pi)
{
    /* Note: bit 8 of 'wl ucflags 3 (MHF4 or M_HOST_FLAGS4)'
     * is already used 6878 for MHF4_NOPHYHANGWAR.
     * We use bit 10 (reuse MHF4_NAPPING_ENABLE)
     * for MAC_aided WAR indicator in uCode.
     * Enable MAC_aided WAR (set bit 10 to 1)
     * when mac_aided and timing forcing are enabled.
     * Disable MAC_aided WAR (set bit 10 to 0) otherwise.
     * Note: for 6715, ucode WAR may not be needed
     */

    if (ACMAJORREV_47_51_129_131(pi->pubpi->phy_rev)) {

        if (pi->u.pi_acphy->ul_mac_aided_en &&
            pi->u.pi_acphy->ul_mac_aided_timing_en) {
            wlapi_bmac_mhf(pi->sh->physhim, MHF4, MHF4_MAC_AIDED_WAR,
                MHF4_MAC_AIDED_WAR, WLC_BAND_AUTO);
        } else {
            wlapi_bmac_mhf(pi->sh->physhim, MHF4, MHF4_MAC_AIDED_WAR,
                0, WLC_BAND_AUTO);
        }
    }

}

static void wlc_phy_ul_mac_aided(phy_info_t *pi)
{
    /* enable/disable mac-aided mode with timing forcing */
    /* Note: decoupled bit 8 of 'wl ucflags 3' and
     * 'has_trigger' enable in uCode.
     * 'has_trigger' is always enabled for TBPPDU.
     */
    if (ACMAJORREV_47_51_129_130_131(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, Config0_MAC_aided_trig_frame,
            mac_aided_triggered_rx, pi->u.pi_acphy->ul_mac_aided_en);
    }
}

int
phy_ac_chanmgr_enable_mac_aided(phy_info_t *pi, bool set, uint16 val)
{
    int ret = 0;
    /* wl ul_mac_aided 1 enable mac-aided mode (timing forcing is on by default)
       wl ul_mac_aided 0 disable mac-aided mode
    */

    /* printf("mac_aided_init: ul_mac_aided_en=%x, ul_mac_aided_timint_en=%x\n",
        pi->u.pi_acphy->ul_mac_aided_en, pi->u.pi_acphy->ul_mac_aided_timing_en);
    */

    if (set) {
        pi->u.pi_acphy->ul_mac_aided_en = val;
        wlc_phy_ul_mac_aided(pi);
        wlc_phy_ul_mac_aided_war(pi);
    } else {
        ret = pi->u.pi_acphy->ul_mac_aided_en;
    }
    return ret;
}

static void wlc_phy_ul_mac_aided_timing(phy_info_t *pi)
{
    /* disable/enable timing forcing for mac-aided mode */
    /* Note:
     * Keep 43684 register settings based on current IFS (~12us) here.
     * Can be removed after SIFS is fixed to 16 us.
     */
    if (ACMAJORREV_47(pi->pubpi->phy_rev) || ACMAJORREV_51_131(pi->pubpi->phy_rev) ||
        ACMAJORREV_129_130(pi->pubpi->phy_rev)) {

        if (pi->u.pi_acphy->ul_mac_aided_timing_en > 0) {
            if (!ACMAJORREV_130(pi->pubpi->phy_rev)) {
                MOD_PHYREG(pi, Config0_MAC_aided_trig_frame, en_mac_aided_crs, 1);
                MOD_PHYREG(pi, Config0_MAC_aided_trig_frame, en_mac_aided_cstr, 1);
                MOD_PHYREG(pi, Config0_MAC_aided_trig_frame, en_mac_aided_fstr, 1);
            } else {
                MOD_PHYREG(pi, Config8_MAC_aided_trig_frame, en_mac_aided_det, 1);
                MOD_PHYREG(pi, Config12_MAC_aided_trig_frame,
                    en_mac_aided_cp1p6_det, 1);
                MOD_PHYREG(pi, Config6_MAC_aided_trig_frame,
                    he_mac_aided_LESI_disable, 1);
                MOD_PHYREG(pi, Config7_MAC_aided_trig_frame,
                    he_mac_aided_MrcSigQual_disable, 1);
            }
        } else {
            if (!ACMAJORREV_130(pi->pubpi->phy_rev)) {
                MOD_PHYREG(pi, Config0_MAC_aided_trig_frame, en_mac_aided_crs, 0);
                MOD_PHYREG(pi, Config0_MAC_aided_trig_frame, en_mac_aided_cstr, 0);
                MOD_PHYREG(pi, Config0_MAC_aided_trig_frame, en_mac_aided_fstr, 0);
            } else {
                MOD_PHYREG(pi, Config8_MAC_aided_trig_frame, en_mac_aided_det, 0);
                MOD_PHYREG(pi, Config12_MAC_aided_trig_frame,
                    en_mac_aided_cp1p6_det, 0);
                MOD_PHYREG(pi, Config6_MAC_aided_trig_frame,
                    he_mac_aided_LESI_disable, 0);
                MOD_PHYREG(pi, Config7_MAC_aided_trig_frame,
                    he_mac_aided_MrcSigQual_disable, 0);
            }
        }

        if (pi->u.pi_acphy->ul_mac_aided_timing_en == 2) {
            switch (pi->bw) {
                    // 43684 STA IFS ~12 us
            case WL_CHANSPEC_BW_20:
                MOD_PHYREG(pi, Config2_MAC_ided_trig_frame, wait_deaf_time, 20);
                MOD_PHYREG(pi, Config3_MAC_aided_trig_frame, wait_ofdm_time, 900);
                MOD_PHYREG(pi, Config4_MAC_aided_trig_frame, wait_he_time, 1620);
                MOD_PHYREG(pi, Config5_MAC_aided_trig_frame, wait_trig_time, 2100);
                MOD_PHYREG(pi, Config6_MAC_aided_trig_frame, mac_aided_crs_time, 790);
                MOD_PHYREG(pi, Config7_MAC_aided_trig_frame, mac_aided_cstr_time, 1020);
                MOD_PHYREG(pi, Config8_MAC_aided_trig_frame, mac_aided_fstr_time, 1180);
                break;
            case WL_CHANSPEC_BW_40:
                MOD_PHYREG(pi, Config2_MAC_ided_trig_frame, wait_deaf_time, 20);
                MOD_PHYREG(pi, Config3_MAC_aided_trig_frame, wait_ofdm_time, 820);
                MOD_PHYREG(pi, Config4_MAC_aided_trig_frame, wait_he_time, 1540);
                MOD_PHYREG(pi, Config5_MAC_aided_trig_frame, wait_trig_time, 2020);
                MOD_PHYREG(pi, Config6_MAC_aided_trig_frame, mac_aided_crs_time, 710);
                MOD_PHYREG(pi, Config7_MAC_aided_trig_frame, mac_aided_cstr_time, 940);
                MOD_PHYREG(pi, Config8_MAC_aided_trig_frame, mac_aided_fstr_time, 1100);
                break;
            case WL_CHANSPEC_BW_80:
                MOD_PHYREG(pi, Config2_MAC_ided_trig_frame, wait_deaf_time, 20);
                MOD_PHYREG(pi, Config3_MAC_aided_trig_frame, wait_ofdm_time, 740);
                MOD_PHYREG(pi, Config4_MAC_aided_trig_frame, wait_he_time, 1460);
                MOD_PHYREG(pi, Config5_MAC_aided_trig_frame, wait_trig_time, 1940);
                MOD_PHYREG(pi, Config6_MAC_aided_trig_frame, mac_aided_crs_time, 630);
                MOD_PHYREG(pi, Config7_MAC_aided_trig_frame, mac_aided_cstr_time, 860);
                MOD_PHYREG(pi, Config8_MAC_aided_trig_frame, mac_aided_fstr_time, 1030);
                break;
            case WL_CHANSPEC_BW_160:
                MOD_PHYREG(pi, Config2_MAC_ided_trig_frame, wait_deaf_time, 20);
                MOD_PHYREG(pi, Config3_MAC_aided_trig_frame, wait_ofdm_time, 780);
                MOD_PHYREG(pi, Config4_MAC_aided_trig_frame, wait_he_time, 1500);
                MOD_PHYREG(pi, Config5_MAC_aided_trig_frame, wait_trig_time, 1980);
                MOD_PHYREG(pi, Config6_MAC_aided_trig_frame, mac_aided_crs_time, 670);
                MOD_PHYREG(pi, Config7_MAC_aided_trig_frame, mac_aided_cstr_time, 900);
                MOD_PHYREG(pi, Config8_MAC_aided_trig_frame, mac_aided_fstr_time, 1050);
                break;
            default:
                PHY_ERROR(("%s: Unknow BW=%d\n",
                __FUNCTION__, pi->bw));
                ASSERT(0);
            }
        }

    }
}

int
phy_ac_chanmgr_enable_mac_aided_timing(phy_info_t *pi, bool set, uint16 val)
{
    int ret = 0;
    /* wl ul_mac_aided_timing 0 disable timing forcing which is on by default
       wl ul_mac_aided_timing 1 enable timing forcing (IFS ~16 us by default)
       wl ul_mac_aided_timing 2 enable timing forcing (IFS ~12 us for 43684)
    */
    if (set) {
        pi->u.pi_acphy->ul_mac_aided_timing_en = val;
        wlc_phy_ul_mac_aided_timing(pi);
        wlc_phy_ul_mac_aided_war(pi);
    } else {
        ret = pi->u.pi_acphy->ul_mac_aided_timing_en;
    }
    return ret;
}

void
phy_ac_chanmgr_sr_enable(phy_info_t *pi, uint8 set_val)
{
    uint8 obsspd_en = 0, srp_en = 0;
    PHY_TRACE(("wl%d: %s", pi->sh->unit, __FUNCTION__));

    obsspd_en = set_val & 0x3;
    srp_en = (set_val >> 2) & 0x1;
    if (ACMAJORREV_47_129_130(pi->pubpi->phy_rev)) {
        if (obsspd_en == 0) {
            PHY_TRACE((", Disable OBSS-PD"));
            MOD_PHYREG(pi, OBSS_PD_SR_EN, obss_pd_sr_enable, 0);
            MOD_PHYREG(pi, OBSS_PD_SR_EN, obss_pd_sr_11ac_enable, 0);
            if (ACMAJORREV_129_130(pi->pubpi->phy_rev)) {
                MOD_PHYREG(pi, OBSS_PD_SR_EN, obss_pd_sr_tb_enable, 0);
            }
            MOD_PHYREG(pi, obssSRCtrl, partial_bss_color_enable, 0);
            MOD_PHYREG(pi, HESigSupportCtrlExt, enable_unsrate_for_ulofdma, 0);
            MOD_PHYREG(pi, HESigParseCtrl, he_mybss_color_len, 0);
            MOD_PHYREG(pi, obssSRCtrl, partial_bss_color_enable, 0);
        } else if (obsspd_en == 1) {
            PHY_TRACE((", Enable OBSS-PD for HE"));
            MOD_PHYREG(pi, OBSS_PD_SR_EN, obss_pd_sr_enable, 1);
            MOD_PHYREG(pi, OBSS_PD_SR_EN, obss_pd_sr_11ac_enable, 0);
            if (ACMAJORREV_129_130(pi->pubpi->phy_rev)) {
                MOD_PHYREG(pi, OBSS_PD_SR_EN, obss_pd_sr_tb_enable, 1);
            }
            MOD_PHYREG(pi, obssSRCtrl, partial_bss_color_enable, 0);
            MOD_PHYREG(pi, HESigSupportCtrlExt, enable_unsrate_for_ulofdma, 1);
            MOD_PHYREG(pi, HESigParseCtrl, he_mybss_color_len, 2);
            MOD_PHYREG(pi, obssSRCtrl, partial_bss_color_enable, 0);
        } else if (obsspd_en == 2) {
            PHY_TRACE((", Enable OBSS-PD for HE and VHT"));
            MOD_PHYREG(pi, OBSS_PD_SR_EN, obss_pd_sr_enable, 1);
            MOD_PHYREG(pi, OBSS_PD_SR_EN, obss_pd_sr_11ac_enable, 1);
            if (ACMAJORREV_129_130(pi->pubpi->phy_rev)) {
                MOD_PHYREG(pi, OBSS_PD_SR_EN, obss_pd_sr_tb_enable, 1);
            }
            MOD_PHYREG(pi, obssSRCtrl, partial_bss_color_enable, 1);
            MOD_PHYREG(pi, HESigSupportCtrlExt, enable_unsrate_for_ulofdma, 1);
            MOD_PHYREG(pi, HESigParseCtrl, he_mybss_color_len, 2);
            MOD_PHYREG(pi, obssSRCtrl, partial_bss_color_enable, 1);
        } else {
            return;
        }
        if (ACMAJORREV_47_129(pi->pubpi->phy_rev)) {
            WRITE_PHYREG(pi, heObssPdSrSigA, 0x6000);
            WRITE_PHYREG(pi, heObssPdNonSrgSigA, 0x8000);
        }
        if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
            if (srp_en == 0) {
                PHY_TRACE((", Disable SRP \n"));
                MOD_PHYREG(pi, srpPhyCtrl_0, srpRssiGoodPlcpOnly, 0);
                MOD_PHYREG(pi, srpPhyCtrl_1, srp_sr_enable, 0);
                MOD_PHYREG(pi, srpPhyCtrl_2, noObssPdIfSrpFailed, 0);
            } else if (srp_en == 1) {
                PHY_TRACE((", Enable SRP \n"));
                MOD_PHYREG(pi, srpPhyCtrl_0, srpRssiGoodPlcpOnly, 1);
                MOD_PHYREG(pi, srpPhyCtrl_1, srp_sr_enable, 1);
                MOD_PHYREG(pi, srpPhyCtrl_2, noObssPdIfSrpFailed, 1);
                MOD_PHYREG(pi, HESigSupportCtrlExt, enable_unsrate_for_ulofdma, 1);
            } else {
                return;
            }
        } else {
            PHY_TRACE((" \n"));
        }
    }
}

void
phy_ac_chanmgr_srg_bssColor(phy_info_t *pi, uint8 set_val, uint16 BM0, uint16 BM1,
    uint16 BM2, uint16 BM3)
{
    PHY_TRACE(("wl%d: %s", pi->sh->unit, __FUNCTION__));

    if (set_val == 0 || set_val == 2) {
        WRITE_PHYREG(pi, SRGparBssIdBitmap0, BM0);
        WRITE_PHYREG(pi, SRGparBssIdBitmap1, BM1);
        WRITE_PHYREG(pi, SRGparBssIdBitmap2, BM2);
        WRITE_PHYREG(pi, SRGparBssIdBitmap3, BM3);
    } else if (set_val == 1 || set_val == 2) {
        WRITE_PHYREG(pi, SRGbssColorBitmap0, BM0);
        WRITE_PHYREG(pi, SRGbssColorBitmap1, BM1);
        WRITE_PHYREG(pi, SRGbssColorBitmap2, BM2);
        WRITE_PHYREG(pi, SRGbssColorBitmap3, BM3);
    }
}

int
phy_ac_chanmgr_enable_vco_pll_adj_mode(phy_info_t *pi, bool set, uint16 val)
{

    int ret = 0;
    phy_ac_chanmgr_info_t *chanmgri = pi->u.pi_acphy->chanmgri;

    /* for ch 5510/5590:
       0 (ACPHY_PLL_ADJ_MODE_OFF):
         disable the aggressive vco pll setting
       1 (ACPHY_PLL_ADJ_MODE_ON):
         enable the aggressive vco pll setting
       2 (ACPHY_PLL_ADJ_MODE_AUTO):
         enable automatic switch between aggressive/safe vco pll setting
    */
    if (set) {
        phy_ac_chanmgr_get_data(chanmgri)->vco_pll_adjust_mode = val;
    } else {
        ret = phy_ac_chanmgr_get_data(chanmgri)->vco_pll_adjust_mode;
    }
    return ret;
}

void
wlc_axphy_dump_txctlplcp_on_reset(phy_info_t *pi)
{
    if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        MOD_PHYREG(pi, CapTxControlPlcp0, cc2Offset, 0x40);
        MOD_PHYREG(pi, CapTxControlPlcp1, cc3Offset, 0x60);
        MOD_PHYREG(pi, CapTxControlPlcp1, cc4Offset, 0x70);
    }
}

static void wlc_phy_set_papdlut_dynradioregtbl_majrev_ge40(phy_info_t *pi)
{
    MOD_PHYREG(pi, dyn_radioa0, dyn_radio_ovr0, 0x0);
    MOD_PHYREG(pi, dyn_radioa1, dyn_radio_ovr1, 0x0);
    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPDLUTSELECT0,
    128, 0, 8, papdluttbl);
    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_PAPDLUTSELECT1,
    128, 0, 8, papdluttbl);
    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_DYNRADIOREGTBL0,
    2, 0, 32, dynradioregtbl);
    wlc_phy_table_write_acphy(pi, ACPHY_TBL_ID_DYNRADIOREGTBL1,
    2, 0, 32, dynradioregtbl);
    MOD_PHYREG(pi, fineclockgatecontrol, forcetxgatedClksOn,
    0x1);
    MOD_PHYREG(pi, TxPwrCtrlBaseIndex, loadPwrIndex, 0x0);
    MOD_PHYREG(pi, TxPwrCtrlBaseIndex, loadPwrIndex, 0x1);
    MOD_PHYREG(pi, TxPwrCtrlBaseIndex, loadPwrIndex, 0x0);
    MOD_PHYREG(pi, fineclockgatecontrol, forcetxgatedClksOn,
    0x0);
}

static void
phy_ac_chanmgr_set_bandsel_on_gpio(phy_info_t *pi, bool band2g, int gpio)
{
    uint8 st = 0, en = 0, i;
    uint16 gpio_mask, gpioout = 0;
    acphy_swctrlmap4_t *swctrl = pi->u.pi_acphy->sromi->swctrlmap4;

    if (gpio < 0) {
        st = 0; en = MAX_GPIO_BANDSEL - 1;    /* Check All */
    } else if (gpio < MAX_GPIO_BANDSEL) {
        st = gpio; en = gpio;
    } else {
        ASSERT(0);  /* Invalid Call */
    }

    for (i = st; i <= en; i++) {
        if (swctrl->bandsel_on_gpio[i]) {
            gpio_mask = 1 << i;
            gpioout = band2g ? 0 : gpio_mask;
            si_gpioout(pi->sh->sih, gpio_mask, gpioout, GPIO_DRV_PRIORITY);
            si_gpioouten(pi->sh->sih, gpio_mask, gpio_mask, GPIO_DRV_PRIORITY);
            si_gpiocontrol(pi->sh->sih, gpio_mask, 0, GPIO_DRV_PRIORITY);
        }
    }
}

#ifdef WL_AIR_IQ
static void
wlc_phy_save_regs_3plus1(phy_info_t *pi, int32 set_val)
{
    uint8 stall_val = 0;
    uint8 orig_rxfectrl1 = 0;

    if (!(ACMAJORREV_33(pi->pubpi->phy_rev))) {
        /* only supported for REV_33 */
        return;
    }
    wlapi_suspend_mac_and_wait(pi->sh->physhim);

    /* Disable stalls and hold FIFOs in reset */
    stall_val = READ_PHYREGFLD(pi, RxFeCtrl1, disable_stalls);
    orig_rxfectrl1 = READ_PHYREGFLD(pi, RxFeCtrl1, soft_sdfeFifoReset);
    ACPHY_DISABLE_STALL(pi);
    MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, 1);

    if (set_val == PHYMODE_BGDFS) {
        /* Save the regs that are used in 3+1 mode */
        wlc_phy_radio20693_save_3plus1(pi, 3, 0);
    } else {
        /* Restore the 3plus1 modified regs */
        wlc_phy_radio20693_save_3plus1(pi, 3, 1);
    }
    /* Restore FIFO reset and Stalls */
    ACPHY_ENABLE_STALL(pi, stall_val);
    MOD_PHYREG(pi, RxFeCtrl1, soft_sdfeFifoReset, orig_rxfectrl1);
    OSL_DELAY(1);

    /* Reset PHY */
    wlc_phy_resetcca_acphy(pi);
    wlapi_enable_mac(pi->sh->physhim);
}
#endif /* WL_AIR_IQ */
#endif /* (ACCONF != 0) || (ACCONF2 != 0) || (ACCONF5 != 0) */

int
phy_ac_chanmgr_iovar_get_lowratetssi(phy_ac_chanmgr_info_t *chanmgri, int32 *ret_val)
{
    phy_info_t *pi = chanmgri->pi;

    if (ACMAJORREV_GE47(pi->pubpi->phy_rev)) {
        if (chanmgri->lowratetssi_ovrd == -1) {
            *ret_val = pi->u.pi_acphy->sromi->srom_low_adc_rate_en;
        } else {
            *ret_val = chanmgri->lowratetssi_ovrd;
        }
    }
    return BCME_OK;
}

int
phy_ac_chanmgr_iovar_get_lowratetssi_ovrd(phy_ac_chanmgr_info_t *chanmgri, int32 *ret_val)
{
    *ret_val = (int32) chanmgri->lowratetssi_ovrd;

    return BCME_OK;
}

int
phy_ac_chanmgr_iovar_set_lowratetssi_ovrd(phy_ac_chanmgr_info_t *chanmgri, int32 set_val)
{
    phy_info_t *pi = chanmgri->pi;

    if ((set_val < -2) || (set_val > 1))
        return BCME_ERROR;

    if (!ACMAJORREV_GE47(pi->pubpi->phy_rev) || ACMAJORREV_128(pi->pubpi->phy_rev))
        return BCME_ERROR;

    chanmgri->lowratetssi_ovrd = set_val;

    if (set_val == 0)
        pi->u.pi_acphy->sromi->srom_low_adc_rate_en = 0;
    else if (set_val == 1)
        pi->u.pi_acphy->sromi->srom_low_adc_rate_en = 1;
    else {
        /* set_val == -1: Default */
        phy_ac_chanmgr_register_impl(pi, chanmgri->aci, chanmgri->cmn_info);
    }

    return BCME_OK;
}

int
phy_ac_chanmgr_iovar_get_loftcompshift_ovrd(phy_ac_chanmgr_info_t *chanmgri, int32 *ret_val)
{
    *ret_val = (int8) chanmgri->loftcompshift_ovrd;

    return BCME_OK;
}

int
phy_ac_chanmgr_iovar_set_loftcompshift_ovrd(phy_ac_chanmgr_info_t *chanmgri, int32 set_val)
{
    if ((set_val < -1) || (set_val > 7))
        return BCME_ERROR;

    chanmgri->loftcompshift_ovrd = set_val;

    return BCME_OK;
}

bool
phy_ac_chanmgr_lesi_en(phy_info_t *pi)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;

    /* Although LESI capability might be TRUE, LESI might be off for some bands */
    if (ACMAJORREV_128(pi->pubpi->phy_rev) || ACMAJORREV_131(pi->pubpi->phy_rev)) {
        /* Get settings per band from NVRAM */
        return (bool)(pi_ac->chanmgri->lesi_perband[
            CHSPEC_IS2G(pi->radio_chanspec) ? 0 : 1]);
    }

    return TRUE;
}

int
phy_ac_chanmgr_iovar_get_papr_en(phy_ac_chanmgr_info_t *chanmgri, int32 *ret_val)
{
    *ret_val = (int32) chanmgri->papr_en_iovar;

    return BCME_OK;
}

int
phy_ac_chanmgr_iovar_set_papr_en(phy_ac_chanmgr_info_t *chanmgri, int32 set_val)
{
    phy_info_t *pi = chanmgri->pi;

    if ((set_val < -1) || (set_val > 1))
        return BCME_ERROR;

    chanmgri->papr_en_iovar = set_val;
    chanspec_setup_papr(pi, 0, 0);
    return BCME_OK;
}

int
phy_ac_chanmgr_iovar_get_papr_gamma(phy_ac_chanmgr_info_t *chanmgri, int32 *ret_val)
{
    *ret_val = (int32) chanmgri->papr_gamma_iovar;

    return BCME_OK;
}

int
phy_ac_chanmgr_iovar_set_papr_gamma(phy_ac_chanmgr_info_t *chanmgri, int32 set_val)
{
    phy_info_t *pi = chanmgri->pi;

    if ((set_val < -1) || (set_val > 4095))
        return BCME_ERROR;

    chanmgri->papr_gamma_iovar = set_val;
    chanspec_setup_papr(pi, 0, 0);
    return BCME_OK;
}

static void
wlc_phy_set_regtbl_on_rxchain(phy_info_t *pi, bool nrx_changed)
{
    phy_info_acphy_t *pi_ac = pi->u.pi_acphy;
    uint8 sgi_hw_adj, core, ncores = 0;
    uint8 phyrxchain = phy_stf_get_data(pi->stfi)->phyrxchain;

    FOREACH_ACTV_CORE(pi, phyrxchain, core) ncores++;

    if (ACMAJORREV_129(pi->pubpi->phy_rev)) {
        if (CCT_INIT(pi_ac) || CCT_BW_CHG(pi_ac) || nrx_changed) {
            sgi_hw_adj = (ncores == 1) && CHSPEC_IS20(pi->radio_chanspec) ? 0 : 9;
            MOD_PHYREG(pi, lesiFstrControl5, lesi_sgi_hw_adj, sgi_hw_adj);
        }
    }

    if (ACMAJORREV_130(pi->pubpi->phy_rev)) {
        if (nrx_changed) {
            wlc_phy_farrow_setup_nontiny(pi);
            wlc_phy_resetcca_acphy(pi);
        }
    }
}
