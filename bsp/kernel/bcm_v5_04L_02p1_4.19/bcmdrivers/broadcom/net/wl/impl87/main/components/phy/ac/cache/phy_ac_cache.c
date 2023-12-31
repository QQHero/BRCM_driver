/*
 * ACPHY Calibration Cache module implementation
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
 * $Id: phy_ac_cache.c 807826 2022-02-01 11:39:38Z $
 */

#include <phy_ac.h>
#include <phy_cfg.h>
#include <typedefs.h>
#include <bcmdefs.h>
#include <phy_dbg.h>
#include <phy_mem.h>
#include "phy_type_cache.h"
#include <phy_ac_cache.h>
#include <phy_ac_radio.h>
#include <phy_ac_rxgcrs.h>
#include <phy_ac_txiqlocal.h>
#include <phy_papdcal.h>
#include <phy_stf.h>

/* ************************ */
/* Modules used by this module */
/* ************************ */
#include <wlc_radioreg_20698.h>
#include <wlc_radioreg_20704.h>
#include <wlc_radioreg_20707.h>
#include <wlc_radioreg_20708.h>
#include <wlc_radioreg_20709.h>
#include <wlc_radioreg_20710.h>
#include <wlc_phy_radio.h>
#include <wlc_phyreg_ac.h>
#include <wlc_phy_int.h>

#include <phy_utils_reg.h>
#include <phy_ac_info.h>

#ifdef ATE_BUILD
#include <wl_ate.h>
#endif /* ATE_BUILD */

/* module private states */
struct phy_ac_cache_info {
	phy_info_t *pi;
	phy_ac_info_t *aci;
	phy_cache_info_t *cmn_info;
};

/* local functions */
#ifdef PHYCAL_CACHING
static void wlc_phy_cal_cache_acphy(phy_type_cache_ctx_t * cache_ctx);
static int wlc_phy_cal_cache_restore_acphy(phy_type_cache_ctx_t * cache_ctx);
#endif /* PHYCAL_CACHING */
bool BCMATTACHFN(phy_ac_reg_cache_attach)(phy_info_acphy_t *pi_ac);
#if defined(BCMDBG) || defined(WLTEST)
static void
wlc_phydump_cal_cache_acphy(phy_type_cache_ctx_t * cache_ctx, ch_calcache_t *ctx,
	struct bcmstrbuf *b);
#endif /* BCMDBG || WLTEST */
#if defined(BCMDBG) || defined(WLTEST)
/* dump calibration regs/info */
static void wlc_phy_cal_dump_acphy(phy_type_cache_ctx_t * cache_ctx, struct bcmstrbuf *b);
#endif
/* register phy type specific implementation */
phy_ac_cache_info_t *
BCMATTACHFN(phy_ac_cache_register_impl)(phy_info_t *pi, phy_ac_info_t *aci,
	phy_cache_info_t *cmn_info)
{
	phy_ac_cache_info_t *ac_info;
	phy_type_cache_fns_t fns;
	phy_info_acphy_t *pi_ac;

	PHY_TRACE(("%s\n", __FUNCTION__));

	/* allocate all storage together */
	if ((ac_info = phy_malloc(pi, sizeof(phy_ac_cache_info_t))) == NULL) {
		PHY_ERROR(("%s: phy_malloc failed\n", __FUNCTION__));
		goto fail;
	}

	ac_info->pi = pi;
	ac_info->aci = aci;
	ac_info->cmn_info = cmn_info;

	/* register PHY type specific implementation */
	bzero(&fns, sizeof(fns));
#ifdef PHYCAL_CACHING
	fns.cal_cache = wlc_phy_cal_cache_acphy;
	fns.restore = wlc_phy_cal_cache_restore_acphy;
#endif /* PHYCAL_CACHING */
#if defined(BCMDBG) || defined(WLTEST)
	fns.dump_chanctx = wlc_phydump_cal_cache_acphy;
#endif
#if defined(BCMDBG) || defined(WLTEST)
	fns.dump_cal = wlc_phy_cal_dump_acphy;
#endif
	fns.ctx = ac_info;

	if (phy_cache_register_impl(cmn_info, &fns) != BCME_OK) {
		PHY_ERROR(("%s: phy_cache_register_impl failed\n", __FUNCTION__));
		goto fail;
	}

	pi_ac = pi->u.pi_acphy;
	if (ACMAJORREV_GE40(pi->pubpi->phy_rev)) {
		/* Using same method as 43012 */
		if (!phy_ac_reg_cache_attach(pi_ac)) {
			PHY_ERROR(("%s: phy_ac_reg_cache_attach failed\n", __FUNCTION__));
			goto fail;
		}
	}

	return ac_info;

	/* error handling */
fail:
	if (ac_info != NULL)
		phy_mfree(pi, ac_info, sizeof(phy_ac_cache_info_t));
	return NULL;
}

void
BCMATTACHFN(phy_ac_cache_unregister_impl)(phy_ac_cache_info_t *ac_info)
{
	phy_info_t *pi;
	phy_cache_info_t *cmn_info;
	phy_info_acphy_t *pi_ac;

	PHY_TRACE(("%s\n", __FUNCTION__));

	ASSERT(ac_info);
	pi = ac_info->pi;
	cmn_info = ac_info->cmn_info;

	pi_ac = pi->u.pi_acphy;
	phy_ac_reg_cache_detach(pi_ac);

	/* unregister from common */
	phy_cache_unregister_impl(cmn_info);

	phy_mfree(pi, ac_info, sizeof(phy_ac_cache_info_t));
}

#define SAVE	1
#define RESTORE	0
static int
phy_ac_reg_cache_process(phy_info_acphy_t *pi_ac, int id, bool save)
{
	int i = 0, core = 0;
	phy_info_t *pi = pi_ac->pi;
	phy_param_info_t *p = pi_ac->paramsi;
	ad_t *cache = NULL;

	/* check if the id to process is reg cache id in use */
	if (id != p->phy_reg_cache_id && id != p->reg_cache_id) {
		PHY_ERROR(("%s: id mismatch (%d != %d %d != %d) - invalid cache!\n", __FUNCTION__,
				id, p->phy_reg_cache_id, id, p->reg_cache_id));
		return BCME_BADARG;
	}

	switch (id) {
		case RADIOREGS_TXIQCAL:
		case RADIOREGS_RXIQCAL:
		case RADIOREGS_PAPDCAL:
		case RADIOREGS_AFECAL:
		case RADIOREGS_TEMPSENSE_VBAT:
		case RADIOREGS_TSSI:
			FOREACH_CORE(pi, core) {
				ASSERT(p->reg_sz <= p->reg_cache_depth);
				/* setup base */
				cache = p->reg_cache;
				for (i = 0; i < p->reg_sz; i++) {
					if (save) {
						cache->data[core] =
							phy_utils_read_radioreg(pi,
								cache->addr[core]);
					} else {
						phy_utils_write_radioreg(pi,
								cache->addr[core],
								cache->data[core]);
					}
					cache++;
				}
			}
			break;

		case PHYREGS_TXIQCAL:
		case PHYREGS_RXIQCAL:
		case PHYREGS_PAPDCAL:
		case PHYREGS_TEMPSENSE_VBAT:
		case PHYREGS_TSSI:
			FOREACH_CORE(pi, core) {
				ASSERT(p->phy_reg_sz <= p->phy_reg_cache_depth);
				/* setup base */
				cache = p->phy_reg_cache;
				for (i = 0; i < p->phy_reg_sz; i++) {
					if (save) {
						cache->data[core] =
							phy_utils_read_phyreg(pi,
								cache->addr[core]);
					} else {
						phy_utils_write_phyreg(pi,
								cache->addr[core],
								cache->data[core]);
					}
					cache++;
				}
			}
			break;

		default:
			PHY_ERROR(("%s: invalid id\n", __FUNCTION__));
			ASSERT(0);
			break;
	}
	return BCME_OK;
}

static int
phy_ac_reg_cache_process_percore(phy_info_acphy_t *pi_ac, int id, bool save, int core)
{
	int i = 0;
	phy_info_t *pi = pi_ac->pi;
	phy_param_info_t *p = pi_ac->paramsi;
	ad_t *cache = NULL;

	/* check if the id to process is reg cache id in use */
	if (id != p->phy_reg_cache_id && id != p->reg_cache_id) {
		PHY_ERROR(("%s: id mismatch (%d != %d %d != %d) - invalid cache!\n", __FUNCTION__,
				id, p->phy_reg_cache_id, id, p->reg_cache_id));
		return BCME_BADARG;
	}

	switch (id) {
		case RADIOREGS_TXIQCAL:
		case RADIOREGS_RXIQCAL:
		case RADIOREGS_PAPDCAL:
		case RADIOREGS_AFECAL:
		case RADIOREGS_TEMPSENSE_VBAT:
		case RADIOREGS_TSSI:
			ASSERT(p->reg_sz <= p->reg_cache_depth);
			/* setup base */
			cache = p->reg_cache;
			for (i = 0; i < p->reg_sz; i++) {
			if (save) {
				cache->data[core] =
					phy_utils_read_radioreg(pi,
					cache->addr[core]);
			} else {
				phy_utils_write_radioreg(pi,
					cache->addr[core],
					cache->data[core]);
				}
				cache++;
			}
			break;

		case PHYREGS_TXIQCAL:
		case PHYREGS_RXIQCAL:
		case PHYREGS_PAPDCAL:
		case PHYREGS_TEMPSENSE_VBAT:
		case PHYREGS_TSSI:
			ASSERT(p->phy_reg_sz <= p->phy_reg_cache_depth);
			/* setup base */
			cache = p->phy_reg_cache;
			for (i = 0; i < p->phy_reg_sz; i++) {
				if (save) {
					cache->data[core] =
						phy_utils_read_phyreg(pi,
							cache->addr[core]);
				} else {
					phy_utils_write_phyreg(pi,
						cache->addr[core],
						cache->data[core]);
				}
				cache++;
			}
			break;

		default:
			PHY_ERROR(("%s: invalid id\n", __FUNCTION__));
			ASSERT(0);
			break;
	}
	return BCME_OK;
}

static void
phy_ac_reg_cache_parse(phy_info_acphy_t *pi_ac, uint16 id, bool populate)
{
	/* radio 20707 (6710), 20709 (6878), 20710 (6756) will not go to here. */
	phy_info_t *pi = pi_ac->pi;
	phy_param_info_t *p = pi_ac->paramsi;
	ad_t *base = p->reg_cache;
	ad_t *cache = base; /* to baseline for looping over cores */

	uint16 phyregs_tempsense_vbat[] = {
		ACPHY_REG(pi, RfctrlOverrideAuxTssi0),
		ACPHY_REG(pi, RfctrlCoreAuxTssi10),
		ACPHY_REG(pi, RfctrlOverrideRxPus0),
		ACPHY_REG(pi, RfctrlCoreRxPus0),
		ACPHY_REG(pi, RxFeCtrl1),
		ACPHY_REG(pi, RfctrlOverrideTxPus0),
		ACPHY_REG(pi, RfctrlCoreTxPus0),
		ACPHY_REG(pi, RfctrlOverrideGains0),
		ACPHY_REG(pi, RfctrlCoreLpfGain0),
		ACPHY_REG(pi, RxSdFeConfig1),
		ACPHY_REG(pi, RxSdFeConfig6),
		ACPHY_REG(pi, RfctrlOverrideLpfSwtch0),
		ACPHY_REG(pi, RfctrlCoreLpfSwtch0),
		ACPHY_REG(pi, RfctrlOverrideAfeCfg0),
		ACPHY_REG(pi, RfctrlCoreAfeCfg10),
		ACPHY_REG(pi, gpioLoOutEn),
		ACPHY_REG(pi, gpioHiOutEn),
		ACPHY_REG(pi, gpioSel),
		ACPHY_REG(pi, RfctrlCoreAfeCfg20)
	};

	uint16 phyregs_papdcal_majorrev36[] = {
		ACPHY_REG(pi, RfctrlOverrideTxPus0),
		ACPHY_REG(pi, RfctrlOverrideRxPus0),
		ACPHY_REG(pi, RfctrlOverrideGains0),
		ACPHY_REG(pi, RfctrlOverrideLpfCT0),
		ACPHY_REG(pi, RfctrlOverrideLpfSwtch0),
		ACPHY_REG(pi, RfctrlOverrideAfeCfg0),
		ACPHY_REG(pi, RfctrlOverrideLowPwrCfg0),
		ACPHY_REG(pi, RfctrlOverrideAuxTssi0),
		ACPHY_REG(pi, RfctrlCoreTxPus0),
		ACPHY_REG(pi, RfctrlCoreRxPus0),
		ACPHY_REG(pi, RfctrlCoreTXGAIN10),
		ACPHY_REG(pi, RfctrlCoreTXGAIN20),
		ACPHY_REG(pi, RfctrlCoreRXGAIN10),
		ACPHY_REG(pi, RfctrlCoreRXGAIN20),
		ACPHY_REG(pi, RfctrlCoreLpfGain0),
		ACPHY_REG(pi, RfctrlCoreLpfCT0),
		ACPHY_REG(pi, RfctrlCoreLpfGmult0),
		ACPHY_REG(pi, RfctrlCoreRCDACBuf0),
		ACPHY_REG(pi, RfctrlCoreLpfSwtch0),
		ACPHY_REG(pi, RfctrlCoreAfeCfg10),
		ACPHY_REG(pi, RfctrlCoreAfeCfg20),
		ACPHY_REG(pi, RfctrlCoreLowPwr0),
		ACPHY_REG(pi, RfctrlCoreAuxTssi10),
		ACPHY_REG(pi, RfctrlCoreAuxTssi20),
		ACPHY_REG(pi, Dac_gain0),
		ACPHY_REG(pi, RfctrlIntc0),
		ACPHY_REG(pi, RfseqCoreActv2059),
		ACPHY_REG(pi, forceFront0),
		ACPHY_REG(pi, PapdEnable0)
	};

	uint16 phyregs_papdcal_majorrev51[] = {
		ACPHY_REG(pi, RfctrlOverrideTxPus0),
		ACPHY_REG(pi, RfctrlOverrideRxPus0),
		ACPHY_REG(pi, RfctrlOverrideGains0),
		ACPHY_REG(pi, RfctrlOverrideLpfCT0),
		ACPHY_REG(pi, RfctrlOverrideLpfSwtch0),
		ACPHY_REG(pi, RfctrlOverrideAfeCfg0),
		ACPHY_REG(pi, RfctrlOverrideLowPwrCfg0),
		ACPHY_REG(pi, RfctrlOverrideAuxTssi0),
		ACPHY_REG(pi, RfctrlCoreTxPus0),
		ACPHY_REG(pi, RfctrlCoreRxPus0),
		ACPHY_REG(pi, RfctrlCoreTXGAIN10),
		ACPHY_REG(pi, RfctrlCoreTXGAIN20),
		ACPHY_REG(pi, RfctrlCoreRXGAIN10),
		ACPHY_REG(pi, RfctrlCoreRXGAIN20),
		ACPHY_REG(pi, RfctrlCoreLpfGain0),
		ACPHY_REG(pi, RfctrlCoreLpfCT0),
		ACPHY_REG(pi, RfctrlCoreLpfGmult0),
		ACPHY_REG(pi, RfctrlCoreRCDACBuf0),
		ACPHY_REG(pi, RfctrlCoreLpfSwtch0),
		ACPHY_REG(pi, RfctrlCoreAfeCfg10),
		ACPHY_REG(pi, RfctrlCoreAfeCfg20),
		ACPHY_REG(pi, RfctrlCoreLowPwr0),
		ACPHY_REG(pi, RfctrlCoreAuxTssi10),
		ACPHY_REG(pi, RfctrlCoreAuxTssi20),
		ACPHY_REG(pi, Dac_gain0),
		ACPHY_REG(pi, RfctrlIntc0),
		ACPHY_REG(pi, forceFront0),
		ACPHY_REG(pi, PapdEnable0),
	};

	/* 43684 RXIQ cal Radio Reg to be stored */
	uint16 radioregs_tempsense_vbat_20698_majorrev47[] = {
		RADIO_REG_20698(pi, TEMPSENSE_OVR1, 0),
		RADIO_REG_20698(pi, TEMPSENSE_CFG, 0),
		RADIO_REG_20698(pi, TESTBUF_OVR1, 0),
		RADIO_REG_20698(pi, TESTBUF_CFG1, 0),
		RADIO_REG_20698(pi, AUXPGA_OVR1, 0),
		RADIO_REG_20698(pi, AUXPGA_CFG1, 0),
		RADIO_REG_20698(pi, AUXPGA_VMID, 0),
		RADIO_REG_20698(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20698(pi, TIA_REG7, 0),
		RADIO_REG_20698(pi, LPF_OVR1, 0),
		RADIO_REG_20698(pi, LPF_OVR2, 0),
		RADIO_REG_20698(pi, LPF_REG7, 0),
		RADIO_REG_20698(pi, IQCAL_CFG4, 0),
		RADIO_REG_20698(pi, IQCAL_OVR1, 0),
		RADIO_REG_20698(pi, IQCAL_CFG5, 0),
		RADIO_REG_20698(pi, AFEBIAS_REG1, 0)
	};

	/* 43684 RXIQ cal Radio Reg to be stored */
	uint16 radioregs_txiqlocal_20698_majorrev47[] = {
		RADIO_REG_20698(pi, IQCAL_CFG1, 0),
		RADIO_REG_20698(pi, IQCAL_OVR1, 0),
		RADIO_REG_20698(pi, IQCAL_CFG4, 0),
		RADIO_REG_20698(pi, IQCAL_CFG5, 0),
		RADIO_REG_20698(pi, AUXPGA_CFG1, 0),
		RADIO_REG_20698(pi, AUXPGA_OVR1, 0),
		RADIO_REG_20698(pi, TESTBUF_OVR1, 0),
		RADIO_REG_20698(pi, TESTBUF_CFG1, 0),
		RADIO_REG_20698(pi, TX2G_MISC_CFG1, 0),
		RADIO_REG_20698(pi, TX2G_CFG1_OVR, 0),
		RADIO_REG_20698(pi, TX2G_MIX_REG0, 0),
		RADIO_REG_20698(pi, TX5G_MISC_CFG1, 0),
		RADIO_REG_20698(pi, TX5G_CFG2_OVR, 0),
		RADIO_REG_20698(pi, LPF_OVR1, 0),
		RADIO_REG_20698(pi, LPF_OVR2, 0),
		RADIO_REG_20698(pi, LPF_REG7, 0),
		RADIO_REG_20698(pi, IQCAL_GAIN_RIN, 0),
		RADIO_REG_20698(pi, IQCAL_GAIN_RFB, 0),
		RADIO_REG_20698(pi, TXDAC_REG0, 0),
		RADIO_REG_20698(pi, TXDAC_REG1, 0),
		RADIO_REG_20698(pi, TXDAC_REG3, 0),
		RADIO_REG_20698(pi, TX2G_MIX_REG2, 0)
	};

	uint16 radioregs_rxiqcal_20698_majorrev47[] = {
		RADIO_REG_20698(pi, LPF_OVR1, 0),
		RADIO_REG_20698(pi, LPF_OVR2, 0),
		RADIO_REG_20698(pi, LPF_REG7, 0),
		RADIO_REG_20698(pi, TIA_REG7, 0),
		RADIO_REG_20698(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20698(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20698(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20698(pi, TX2G_MIX_REG0, 0),
		RADIO_REG_20698(pi, TX2G_MISC_CFG1, 0),
		RADIO_REG_20698(pi, TXDAC_REG0, 0),
		RADIO_REG_20698(pi, TXDAC_REG1, 0),
		RADIO_REG_20698(pi, TXDAC_REG3, 0),
		RADIO_REG_20698(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20698(pi, RX2G_REG1, 0),
		RADIO_REG_20698(pi, LNA2G_REG1, 0),
		RADIO_REG_20698(pi, LNA2G_REG2, 0),
		RADIO_REG_20698(pi, RX2G_REG3, 0),
		RADIO_REG_20698(pi, TX5G_MISC_CFG1, 0),
		RADIO_REG_20698(pi, TX5G_MIX_REG0, 0),
		RADIO_REG_20698(pi, RX5G_CFG1_OVR, 0),
		RADIO_REG_20698(pi, RX5G_REG1, 0),
		RADIO_REG_20698(pi, RX5G_REG2, 0),
		RADIO_REG_20698(pi, RX5G_REG4, 0),
		RADIO_REG_20698(pi, RX5G_REG5, 0),
		RADIO_REG_20698(pi, LPF_REG6, 0),
		RADIO_REG_20698(pi, TX2G_MIX_REG2, 0)
	};

	uint16 radioregs_afecal_20698_majorrev47[] = {
		RADIO_REG_20698(pi, AFE_CFG1_OVR2, 0),
		RADIO_REG_20698(pi, RXADC_CFG0, 0),
		RADIO_REG_20698(pi, TXDAC_REG0, 0),
		RADIO_REG_20698(pi, TXDAC_REG1, 0),
		RADIO_REG_20698(pi, LPF_REG7, 0),
		RADIO_REG_20698(pi, LPF_OVR1, 0),
		RADIO_REG_20698(pi, LPF_OVR2, 0)
	};

	uint16 radioregs_tempsense_vbat_20704_majorrev51[] = {
		RADIO_REG_20704(pi, TEMPSENSE_OVR1, 0),
		RADIO_REG_20704(pi, TEMPSENSE_CFG, 0),
		RADIO_REG_20704(pi, TESTBUF_OVR1, 0),
		RADIO_REG_20704(pi, TESTBUF_CFG1, 0),
		RADIO_REG_20704(pi, AUXPGA_OVR1, 0),
		RADIO_REG_20704(pi, AUXPGA_CFG1, 0),
		RADIO_REG_20704(pi, AUXPGA_VMID, 0),
		RADIO_REG_20704(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20704(pi, TIA_REG7, 0),
		RADIO_REG_20704(pi, LPF_OVR1, 0),
		RADIO_REG_20704(pi, LPF_OVR2, 0),
		RADIO_REG_20704(pi, LPF_REG7, 0),
		RADIO_REG_20704(pi, IQCAL_CFG4, 0),
		RADIO_REG_20704(pi, IQCAL_OVR1, 0),
		RADIO_REG_20704(pi, IQCAL_CFG5, 0)
	};

	uint16 radioregs_txiqlocal_20704_majorrev51[] = {
		RADIO_REG_20704(pi, IQCAL_OVR1, 0),
		RADIO_REG_20704(pi, IQCAL_CFG1, 0),
		RADIO_REG_20704(pi, IQCAL_CFG4, 0),
		RADIO_REG_20704(pi, IQCAL_CFG5, 0),
		RADIO_REG_20704(pi, IQCAL_GAIN_RIN, 0),
		RADIO_REG_20704(pi, IQCAL_GAIN_RFB, 0),
		RADIO_REG_20704(pi, LPF_OVR1, 0),
		RADIO_REG_20704(pi, LPF_OVR2, 0),
		RADIO_REG_20704(pi, LPF_REG7, 0),
		RADIO_REG_20704(pi, TX5G_MIX_REG0, 0),
		RADIO_REG_20704(pi, TX5G_CFG2_OVR, 0),
		RADIO_REG_20704(pi, TX5G_MISC_CFG1, 0),
		RADIO_REG_20704(pi, AUXPGA_OVR1, 0),
		RADIO_REG_20704(pi, AUXPGA_CFG1, 0),
		RADIO_REG_20704(pi, TESTBUF_OVR1, 0),
		RADIO_REG_20704(pi, TESTBUF_CFG1, 0),
		RADIO_REG_20704(pi, TXDAC_REG0, 0),
		RADIO_REG_20704(pi, TXDAC_REG1, 0),
		RADIO_REG_20704(pi, TXDAC_REG3, 0),
		RADIO_REG_20704(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20704(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20704(pi, TXDB_PAD_REG3, 0)
	};

	uint16 radioregs_rxiqcal_20704_majorrev51[] = {
		RADIO_REG_20704(pi, LPF_OVR1, 0),
		RADIO_REG_20704(pi, LPF_OVR2, 0),
		RADIO_REG_20704(pi, LPF_REG7, 0),
		RADIO_REG_20704(pi, TIA_REG7, 0),
		RADIO_REG_20704(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20704(pi, RXDB_CFG1_OVR, 0),
		RADIO_REG_20704(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20704(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20704(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20704(pi, TX2G_PA_REG0, 0),
		RADIO_REG_20704(pi, RX5G_REG4, 0),
		RADIO_REG_20704(pi, RX5G_REG2, 0),
		RADIO_REG_20704(pi, RX5G_REG6, 0),
		RADIO_REG_20704(pi, RX5G_REG1, 0),
		RADIO_REG_20704(pi, RX2G_REG4, 0),
		RADIO_REG_20704(pi, TXDAC_REG0, 0),
		RADIO_REG_20704(pi, TXDAC_REG1, 0),
		RADIO_REG_20704(pi, TXDAC_REG3, 0),
		RADIO_REG_20704(pi, LPF_REG6, 0),
		RADIO_REG_20704(pi, TXDB_CFG1_OVR, 0),
		RADIO_REG_20704(pi, TX2G_MIX_REG0, 0),
		RADIO_REG_20704(pi, TX2G_CFG1_OVR, 0),
		RADIO_REG_20704(pi, TXDB_REG0, 0)
	};

	uint16 radioregs_afecal_20704_majorrev51[] = {
		RADIO_REG_20704(pi, AFE_CFG1_OVR2, 0),
		RADIO_REG_20704(pi, TXDAC_REG0, 0),
		RADIO_REG_20704(pi, LPF_OVR1, 0),
		RADIO_REG_20704(pi, LPF_OVR2, 0)
	};

	uint16 radioregs_papdcal_20704_majorrev51[] = {
		RADIO_REG_20704(pi, LPF_OVR1, 0),
		RADIO_REG_20704(pi, LPF_OVR2, 0),
		RADIO_REG_20704(pi, LPF_REG7, 0),
		RADIO_REG_20704(pi, TIA_REG7, 0),
		RADIO_REG_20704(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20704(pi, RXDB_CFG1_OVR, 0),
		RADIO_REG_20704(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20704(pi, RX5G_REG1, 0),
		RADIO_REG_20704(pi, RX5G_REG2, 0),
		RADIO_REG_20704(pi, RX2G_REG4, 0),
		RADIO_REG_20704(pi, RX5G_REG4, 0),
		RADIO_REG_20704(pi, RX5G_REG5, 0),
		RADIO_REG_20704(pi, RX5G_REG6, 0),
		RADIO_REG_20704(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20704(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20704(pi, TX2G_PA_REG0, 0),
		RADIO_REG_20704(pi, TXDAC_REG3, 0),
		RADIO_REG_20704(pi, RXADC_CFG0, 0)
	};

	uint16 radioregs_wbpapdcal_20704_majorrev51[] = {
		RADIO_REG_20704(pi, LPF_OVR1, 0),
		RADIO_REG_20704(pi, LPF_OVR2, 0),
		RADIO_REG_20704(pi, LPF_REG1, 0),
		RADIO_REG_20704(pi, LPF_REG2, 0),
		RADIO_REG_20704(pi, LPF_REG3, 0),
		RADIO_REG_20704(pi, LPF_REG4, 0),
		RADIO_REG_20704(pi, LPF_REG6, 0),
		RADIO_REG_20704(pi, LPF_REG7, 0),
		RADIO_REG_20704(pi, LPF_REG11, 0),
		RADIO_REG_20704(pi, TIA_REG1, 0),
		RADIO_REG_20704(pi, TIA_REG2, 0),
		RADIO_REG_20704(pi, TIA_REG3, 0),
		RADIO_REG_20704(pi, TIA_REG4, 0),
		RADIO_REG_20704(pi, TIA_REG5, 0),
		RADIO_REG_20704(pi, TIA_REG6, 0),
		RADIO_REG_20704(pi, TIA_REG7, 0),
		RADIO_REG_20704(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20704(pi, RXDB_CFG1_OVR, 0),
		RADIO_REG_20704(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20704(pi, RX5G_REG1, 0),
		RADIO_REG_20704(pi, RX5G_REG2, 0),
		RADIO_REG_20704(pi, RX2G_REG4, 0),
		RADIO_REG_20704(pi, RX5G_REG4, 0),
		RADIO_REG_20704(pi, RX5G_REG5, 0),
		RADIO_REG_20704(pi, RX5G_REG6, 0),
		RADIO_REG_20704(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20704(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20704(pi, TX2G_PA_REG0, 0),
		RADIO_REG_20704(pi, TXDAC_REG3, 0),
		RADIO_REG_20704(pi, RXADC_CFG0, 0),
		RADIO_REG_20704(pi, LPF_NOTCH_OVR1, 0),
		RADIO_REG_20704(pi, LPF_NOTCH_REG2, 0),
		RADIO_REG_20704(pi, LPF_NOTCH_REG3, 0),
		RADIO_REG_20704(pi, LPF_NOTCH_REG4, 0),
		RADIO_REG_20704(pi, LPF_NOTCH_REG5, 0)
	};

	/* 6878 moves to separate function phy_ac_reg_cache_parse_20709
	to populate lists for save & restore functions to avoid stack size issues
	*/

	if (pi == NULL) return;

	if (!populate) {
		uint16 radioreg_size = 0;
		uint16 phyreg_size = 0;
		p->reg_cache_id = id;
		p->phy_reg_cache_id = id;

		if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
			radioreg_size = MAX(ARRAYSIZE(radioregs_txiqlocal_20698_majorrev47),
				ARRAYSIZE(radioregs_rxiqcal_20698_majorrev47));
			radioreg_size = MAX(radioreg_size,
				ARRAYSIZE(radioregs_afecal_20698_majorrev47));
			radioreg_size = MAX(radioreg_size,
				ARRAYSIZE(radioregs_tempsense_vbat_20698_majorrev47));
		} else if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
			radioreg_size = MAX(ARRAYSIZE(radioregs_txiqlocal_20704_majorrev51),
				ARRAYSIZE(radioregs_rxiqcal_20704_majorrev51));
			radioreg_size = MAX(radioreg_size,
				ARRAYSIZE(radioregs_afecal_20704_majorrev51));
			radioreg_size = MAX(radioreg_size,
				ARRAYSIZE(radioregs_tempsense_vbat_20704_majorrev51));
			if (WBPAPD_ENAB(pi)) {
				radioreg_size = MAX(radioreg_size,
					ARRAYSIZE(radioregs_wbpapdcal_20704_majorrev51));
			} else {
				radioreg_size = MAX(radioreg_size,
					ARRAYSIZE(radioregs_papdcal_20704_majorrev51));
			}
		} else {
			ASSERT(0);
		}

		if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
			phyreg_size = ARRAYSIZE(phyregs_papdcal_majorrev51);
		} else {
			phyreg_size = ARRAYSIZE(phyregs_papdcal_majorrev36);
		}

		phyreg_size = MAX(phyreg_size, ARRAYSIZE(phyregs_tempsense_vbat));
		p->phy_reg_cache_depth =  phyreg_size;
		p->reg_cache_depth = radioreg_size;

	} else {
		uint16 *list = NULL;
		uint16 sz = 0;

		switch (id) {
			case RADIOREGS_TXIQCAL:
				if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
					list = radioregs_txiqlocal_20698_majorrev47;
					sz   = ARRAYSIZE(radioregs_txiqlocal_20698_majorrev47);
				} else if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
					list = radioregs_txiqlocal_20704_majorrev51;
					sz   = ARRAYSIZE(radioregs_txiqlocal_20704_majorrev51);
				} else {
					ASSERT(0);
				}
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_RXIQCAL:
				if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
					list = radioregs_rxiqcal_20698_majorrev47;
					sz   = ARRAYSIZE(radioregs_rxiqcal_20698_majorrev47);
				} else if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
					list = radioregs_rxiqcal_20704_majorrev51;
					sz   = ARRAYSIZE(radioregs_rxiqcal_20704_majorrev51);
				}
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_PAPDCAL:
				if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
					if (WBPAPD_ENAB(pi)) {
						list = radioregs_wbpapdcal_20704_majorrev51;
						sz   = ARRAYSIZE(
							radioregs_wbpapdcal_20704_majorrev51);
					} else {
						list = radioregs_papdcal_20704_majorrev51;
						sz   = ARRAYSIZE(
							radioregs_papdcal_20704_majorrev51);
					}
				} else {
					ASSERT(0);
				}
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case PHYREGS_PAPDCAL:
				if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
					list = phyregs_papdcal_majorrev51;
					sz   = ARRAYSIZE(phyregs_papdcal_majorrev51);
				} else {
					list = phyregs_papdcal_majorrev36;
					sz   = ARRAYSIZE(phyregs_papdcal_majorrev36);
				}
				base = p->phy_reg_cache; /* to baseline for looping over cores */
				p->phy_reg_sz = sz;
				p->phy_reg_cache_id = id;
				break;
			case PHYREGS_TEMPSENSE_VBAT:
				list = phyregs_tempsense_vbat;
				sz   = ARRAYSIZE(phyregs_tempsense_vbat);
				base = p->phy_reg_cache; /* to baseline for looping over cores */
				p->phy_reg_sz = sz;
				p->phy_reg_cache_id = id;
				break;
			case PHYREGS_TSSI:
				ASSERT(0);
				break;
			case RADIOREGS_AFECAL:
				if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
					list = radioregs_afecal_20698_majorrev47;
					sz   = ARRAYSIZE(radioregs_afecal_20698_majorrev47);
				} else if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
					list = radioregs_afecal_20704_majorrev51;
					sz   = ARRAYSIZE(radioregs_afecal_20704_majorrev51);
				} else {
					ASSERT(0);
				}
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_TEMPSENSE_VBAT:
				if (ACMAJORREV_47(pi->pubpi->phy_rev)) {
					list = radioregs_tempsense_vbat_20698_majorrev47;
					sz   = ARRAYSIZE(radioregs_tempsense_vbat_20698_majorrev47);
				} else if (ACMAJORREV_51(pi->pubpi->phy_rev)) {
					list = radioregs_tempsense_vbat_20704_majorrev51;
					sz   = ARRAYSIZE(radioregs_tempsense_vbat_20704_majorrev51);
				} else {
					ASSERT(0);
				}
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_TSSI:
				ASSERT(0);
				break;
			default:
				PHY_ERROR(("%s: Invalid ID %d\n", __FUNCTION__, id));
				ASSERT(0);
				break;
		}
		if (list != NULL) {
			uint8 i = 0, core = 0;
			/* update reg cache id in use */
			/* align to base */
			cache = base;
			PHY_INFORM(("***** core %d ***** : %s\n", core, __FUNCTION__));
			for (i = 0; i < sz; i++) {
				if (RADIOID_IS(pi->pubpi->radioid, BCM20698_ID)) {
					cache->addr[core] = list[i];
					cache->addr[core+1] = (list[i] + JTAG_20698_CR1);
					cache->addr[core+2] = (list[i] + JTAG_20698_CR2);
					cache->addr[core+3] = (list[i] + JTAG_20698_CR3);
					cache->data[core] = 0;
					cache->data[core+1] = 0;
					cache->data[core+2] = 0;
					cache->data[core+3] = 0;
				} else if (RADIOID_IS(pi->pubpi->radioid, BCM20704_ID)) {
					cache->addr[core] = list[i];
					cache->addr[core+1] = (list[i] + JTAG_20704_CR1);
					cache->data[core] = 0;
					cache->data[core+1] = 0;
				} else if (RADIOID_IS(pi->pubpi->radioid, BCM20709_ID)) {
					cache->addr[core] = list[i];
					cache->addr[core+1] = (list[i] + JTAG_20709_CR1);
					cache->data[core] = 0;
					cache->data[core+1] = 0;
				} else {
					cache->addr[core] = list[i];
					cache->data[core] = 0;
				}
				PHY_INFORM(("0x%8p: 0x%04x\n", cache, cache->addr[core]));
				cache++;
			}
		}
	}
}

static void
phy_ac_reg_cache_parse_20707(phy_info_acphy_t *pi_ac, uint16 id, bool populate)
{
	phy_info_t *pi = pi_ac->pi;
	phy_param_info_t *p = pi_ac->paramsi;
	ad_t *base = p->reg_cache;
	ad_t *cache = base; /* to baseline for looping over cores */

	uint16 phyregs_tempsense_vbat[] = {
		ACPHY_REG(pi, RfctrlOverrideAuxTssi0),
		ACPHY_REG(pi, RfctrlCoreAuxTssi10),
		ACPHY_REG(pi, RfctrlOverrideRxPus0),
		ACPHY_REG(pi, RfctrlCoreRxPus0),
		ACPHY_REG(pi, RxFeCtrl1),
		ACPHY_REG(pi, RfctrlOverrideTxPus0),
		ACPHY_REG(pi, RfctrlCoreTxPus0),
		ACPHY_REG(pi, RfctrlOverrideGains0),
		ACPHY_REG(pi, RfctrlCoreLpfGain0),
		ACPHY_REG(pi, RxSdFeConfig1),
		ACPHY_REG(pi, RxSdFeConfig6),
		ACPHY_REG(pi, RfctrlOverrideLpfSwtch0),
		ACPHY_REG(pi, RfctrlCoreLpfSwtch0),
		ACPHY_REG(pi, RfctrlOverrideAfeCfg0),
		ACPHY_REG(pi, RfctrlCoreAfeCfg10),
		ACPHY_REG(pi, gpioLoOutEn),
		ACPHY_REG(pi, gpioHiOutEn),
		ACPHY_REG(pi, gpioSel),
		ACPHY_REG(pi, RfctrlCoreAfeCfg20)
	};

	uint16 phyregs_papdcal_majorrev129[] = {
		ACPHY_REG(pi, RfctrlOverrideTxPus0),
		ACPHY_REG(pi, RfctrlOverrideRxPus0),
		ACPHY_REG(pi, RfctrlOverrideGains0),
		ACPHY_REG(pi, RfctrlOverrideLpfCT0),
		ACPHY_REG(pi, RfctrlOverrideLpfSwtch0),
		ACPHY_REG(pi, RfctrlOverrideAfeCfg0),
		ACPHY_REG(pi, RfctrlOverrideLowPwrCfg0),
		ACPHY_REG(pi, RfctrlOverrideAuxTssi0),
		ACPHY_REG(pi, RfctrlCoreTxPus0),
		ACPHY_REG(pi, RfctrlCoreRxPus0),
		ACPHY_REG(pi, RfctrlCoreTXGAIN10),
		ACPHY_REG(pi, RfctrlCoreTXGAIN20),
		ACPHY_REG(pi, RfctrlCoreRXGAIN10),
		ACPHY_REG(pi, RfctrlCoreRXGAIN20),
		ACPHY_REG(pi, RfctrlCoreLpfGain0),
		ACPHY_REG(pi, RfctrlCoreLpfCT0),
		ACPHY_REG(pi, RfctrlCoreLpfGmult0),
		ACPHY_REG(pi, RfctrlCoreRCDACBuf0),
		ACPHY_REG(pi, RfctrlCoreLpfSwtch0),
		ACPHY_REG(pi, RfctrlCoreAfeCfg10),
		ACPHY_REG(pi, RfctrlCoreAfeCfg20),
		ACPHY_REG(pi, RfctrlCoreLowPwr0),
		ACPHY_REG(pi, RfctrlCoreAuxTssi10),
		ACPHY_REG(pi, RfctrlCoreAuxTssi20),
		ACPHY_REG(pi, Dac_gain0),
		ACPHY_REG(pi, RfctrlIntc0),
		ACPHY_REG(pi, forceFront0),
		ACPHY_REG(pi, PapdEnable0)
	};

	uint16 phyreg_tssi_setup_majorrev129[] = {
		ACPHY_REG(pi, RfctrlOverrideAuxTssi0),
		ACPHY_REG(pi, RfctrlCoreAuxTssi10),
		ACPHY_REG(pi, RfctrlIntc0),
		ACPHY_REG(pi, ClassifierCtrl),
		ACPHY_REG(pi, RxFeCtrl1),
		ACPHY_REG(pi, AfePuCtrl)
	};

	uint16 radioregs_tssi_setup_20707_majorrev129[] = {
		RADIO_REG_20707(pi, TX5G_TSSI_REG0, 0),
		RADIO_REG_20707(pi, TX2G_CFG1_OVR, 0),
		RADIO_REG_20707(pi, TX2G_MIX_REG0, 0),
		RADIO_REG_20707(pi, TX5G_MIX_REG0, 0),
		RADIO_REG_20707(pi, IQCAL_OVR1, 0),
		RADIO_REG_20707(pi, IQCAL_CFG1, 0),
		RADIO_REG_20707(pi, IQCAL_CFG4, 0),
		RADIO_REG_20707(pi, IQCAL_CFG5, 0),
		RADIO_REG_20707(pi, IQCAL_CFG6, 0),
		RADIO_REG_20707(pi, IQCAL_IDAC, 0),
		RADIO_REG_20707(pi, TESTBUF_OVR1, 0),
		RADIO_REG_20707(pi, TESTBUF_CFG1, 0),
		RADIO_REG_20707(pi, AUXPGA_OVR1, 0),
		RADIO_REG_20707(pi, AUXPGA_CFG1, 0),
		RADIO_REG_20707(pi, AUXPGA_VMID, 0),
		RADIO_REG_20707(pi, LPF_OVR1, 0),
		RADIO_REG_20707(pi, LPF_OVR2, 0),
		RADIO_REG_20707(pi, LPF_REG7, 0),
		RADIO_REG_20707(pi, LPF_REG8, 0)
	};

	uint16 radioregs_tempsense_vbat_20707_majorrev129[] = {
		RADIO_REG_20707(pi, TEMPSENSE_OVR1, 0),
		RADIO_REG_20707(pi, TEMPSENSE_CFG, 0),
		RADIO_REG_20707(pi, TESTBUF_OVR1, 0),
		RADIO_REG_20707(pi, TESTBUF_CFG1, 0),
		RADIO_REG_20707(pi, AUXPGA_OVR1, 0),
		RADIO_REG_20707(pi, AUXPGA_CFG1, 0),
		RADIO_REG_20707(pi, AUXPGA_VMID, 0),
		RADIO_REG_20707(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20707(pi, TIA_REG7, 0),
		RADIO_REG_20707(pi, LPF_OVR1, 0),
		RADIO_REG_20707(pi, LPF_OVR2, 0),
		RADIO_REG_20707(pi, LPF_REG7, 0),
		RADIO_REG_20707(pi, IQCAL_CFG4, 0),
		RADIO_REG_20707(pi, IQCAL_OVR1, 0),
		RADIO_REG_20707(pi, IQCAL_CFG5, 0)
	};

	uint16 radioregs_txiqlocal_20707_majorrev129[] = {
		RADIO_REG_20707(pi, IQCAL_OVR1, 0),
		RADIO_REG_20707(pi, IQCAL_CFG1, 0),
		RADIO_REG_20707(pi, IQCAL_CFG4, 0),
		RADIO_REG_20707(pi, IQCAL_CFG5, 0),
		RADIO_REG_20707(pi, IQCAL_GAIN_RIN, 0),
		RADIO_REG_20707(pi, IQCAL_GAIN_RFB, 0),
		RADIO_REG_20707(pi, LPF_OVR1, 0),
		RADIO_REG_20707(pi, LPF_OVR2, 0),
		RADIO_REG_20707(pi, LPF_REG7, 0),
		RADIO_REG_20707(pi, TX2G_CFG_OVR, 0),
		RADIO_REG_20707(pi, TX2G_TSSI_REG0, 0),
		RADIO_REG_20707(pi, TX2G_TSSI_REG1, 0),
		RADIO_REG_20707(pi, TX5G_CFG_OVR, 0),
		RADIO_REG_20707(pi, TX5G_TSSI_REG0, 0),
		RADIO_REG_20707(pi, TX5G_TSSI_REG1, 0),
		RADIO_REG_20707(pi, TX2G_IPA_REG0, 0),
		RADIO_REG_20707(pi, TX2G_IPA_CFG_OVR, 0),
		RADIO_REG_20707(pi, TX5G_IPA_REG0, 0),
		RADIO_REG_20707(pi, TX5G_IPA_CFG_OVR, 0),
		RADIO_REG_20707(pi, TX5G_IPA_GC_REG, 0),
		RADIO_REG_20707(pi, AUXPGA_OVR1, 0),
		RADIO_REG_20707(pi, AUXPGA_CFG1, 0),
		RADIO_REG_20707(pi, TESTBUF_OVR1, 0),
		RADIO_REG_20707(pi, TESTBUF_CFG1, 0),
		RADIO_REG_20707(pi, TXDAC_REG0, 0),
		RADIO_REG_20707(pi, TXDAC_REG1, 0),
		RADIO_REG_20707(pi, TXDAC_REG3, 0),
		RADIO_REG_20707(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20707(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20707(pi, TX5G_CFG1_OVR, 0),
		RADIO_REG_20707(pi, TX5G_MIX_GC_REG, 0),
		RADIO_REG_20707(pi, TX5G_MIX_REG2, 0),
		RADIO_REG_20707(pi, TX5G_PAD_REG1, 0),
		RADIO_REG_20707(pi, TX5G_PAD_REG3, 0),
		RADIO_REG_20707(pi, TX2G_PAD_REG1, 0),
		RADIO_REG_20707(pi, TX2G_PAD_REG3, 0),
		RADIO_REG_20707(pi, TX2G_MIX_REG2, 0),
		RADIO_REG_20707(pi, TX2G_CFG1_OVR, 0),
		RADIO_REG_20707(pi, TX2G_MIX_GC_REG, 0)
	};

	uint16 radioregs_rxiqcal_20707_majorrev129[] = {
		RADIO_REG_20707(pi, LPF_OVR1, 0),
		RADIO_REG_20707(pi, LPF_OVR2, 0),
		RADIO_REG_20707(pi, LPF_REG7, 0),
		RADIO_REG_20707(pi, TIA_REG7, 0),
		RADIO_REG_20707(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20707(pi, RXDB_CFG1_OVR, 0),
		RADIO_REG_20707(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20707(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20707(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20707(pi, RX5G_REG4, 0),
		RADIO_REG_20707(pi, LOGEN_CORE_REG5, 0),
		RADIO_REG_20707(pi, RX5G_REG2, 0),
		RADIO_REG_20707(pi, RX5G_REG6, 0),
		RADIO_REG_20707(pi, RX2G_REG4, 0),
		RADIO_REG_20707(pi, RX5G_REG1, 0),
		RADIO_REG_20707(pi, TXDAC_REG3, 0),
		RADIO_REG_20707(pi, TXDAC_REG0, 0),
		RADIO_REG_20707(pi, TXDAC_REG1, 0),
		RADIO_REG_20707(pi, LPF_REG6, 0),
		RADIO_REG_20707(pi, TX5G_IPA_REG0, 0),
		RADIO_REG_20707(pi, TX5G_IPA_CFG_OVR, 0),
		RADIO_REG_20707(pi, TX5G_IPA_GC_REG, 0),
		RADIO_REG_20707(pi, TX2G_IPA_REG0, 0),
		RADIO_REG_20707(pi, TX2G_IPA_CFG_OVR, 0),
		RADIO_REG_20707(pi, TX2G_IPA_GC_REG, 0),
		RADIO_REG_20707(pi, RX5G_REG5, 0),
		RADIO_REG_20707(pi, TX2G_MIX_REG0, 0),
		RADIO_REG_20707(pi, TX2G_CFG1_OVR, 0),
		RADIO_REG_20707(pi, TX2G_PAD_REG0, 0),
		RADIO_REG_20707(pi, TX5G_MIX_REG0, 0),
		RADIO_REG_20707(pi, TX5G_CFG1_OVR, 0),
		RADIO_REG_20707(pi, TX5G_PAD_REG0, 0),
		RADIO_REG_20707(pi, TX5G_MIX_GC_REG, 0),
		RADIO_REG_20707(pi, TX5G_MIX_REG2, 0),
		RADIO_REG_20707(pi, TX5G_PAD_REG1, 0),
		RADIO_REG_20707(pi, TX5G_PAD_REG3, 0),
		RADIO_REG_20707(pi, TX2G_MIX_GC_REG, 0),
		RADIO_REG_20707(pi, TX2G_PAD_REG1, 0),
		RADIO_REG_20707(pi, TX2G_PAD_REG3, 0),
		RADIO_REG_20707(pi, TX2G_MIX_REG2, 0),
	};

	uint16 radioregs_afecal_20707_majorrev129[] = {
		RADIO_REG_20707(pi, AFE_CFG1_OVR2, 0),
		RADIO_REG_20707(pi, TXDAC_REG0, 0),
		RADIO_REG_20707(pi, LPF_OVR1, 0),
		RADIO_REG_20707(pi, LPF_OVR2, 0)
	};

	uint16 radioregs_papdcal_20707_majorrev129[] = {
		RADIO_REG_20707(pi, LPF_OVR1, 0),
		RADIO_REG_20707(pi, LPF_OVR2, 0),
		RADIO_REG_20707(pi, LPF_REG7, 0),
		RADIO_REG_20707(pi, TIA_REG7, 0),
		RADIO_REG_20707(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20707(pi, RXDB_CFG1_OVR, 0),
		RADIO_REG_20707(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20707(pi, RX5G_REG1, 0),
		RADIO_REG_20707(pi, RX5G_REG2, 0),
		RADIO_REG_20707(pi, RX2G_REG4, 0),
		RADIO_REG_20707(pi, RX5G_REG4, 0),
		RADIO_REG_20707(pi, RX5G_REG5, 0),
		RADIO_REG_20707(pi, RX5G_REG6, 0),
		RADIO_REG_20707(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20707(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20707(pi, TX2G_IPA_REG0, 0),
		RADIO_REG_20707(pi, TX5G_IPA_REG0, 0),
		RADIO_REG_20707(pi, TXDAC_REG3, 0),
		RADIO_REG_20707(pi, RXADC_CFG0, 0),
		RADIO_REG_20707(pi, TX2G_MISC_CFG6, 0)
	};
	uint16 radioregs_wbpapdcal_20707_majorrev129[] = {
		RADIO_REG_20707(pi, LPF_OVR1, 0),
		RADIO_REG_20707(pi, LPF_OVR2, 0),
		RADIO_REG_20707(pi, LPF_REG1, 0),
		RADIO_REG_20707(pi, LPF_REG2, 0),
		RADIO_REG_20707(pi, LPF_REG3, 0),
		RADIO_REG_20707(pi, LPF_REG4, 0),
		RADIO_REG_20707(pi, LPF_REG6, 0),
		RADIO_REG_20707(pi, LPF_REG7, 0),
		RADIO_REG_20707(pi, LPF_REG11, 0),
		RADIO_REG_20707(pi, TIA_REG1, 0),
		RADIO_REG_20707(pi, TIA_REG2, 0),
		RADIO_REG_20707(pi, TIA_REG3, 0),
		RADIO_REG_20707(pi, TIA_REG4, 0),
		RADIO_REG_20707(pi, TIA_REG5, 0),
		RADIO_REG_20707(pi, TIA_REG6, 0),
		RADIO_REG_20707(pi, TIA_REG7, 0),
		RADIO_REG_20707(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20707(pi, RXDB_CFG1_OVR, 0),
		RADIO_REG_20707(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20707(pi, RX5G_REG1, 0),
		RADIO_REG_20707(pi, RX5G_REG2, 0),
		RADIO_REG_20707(pi, RX2G_REG4, 0),
		RADIO_REG_20707(pi, RX5G_REG4, 0),
		RADIO_REG_20707(pi, RX5G_REG5, 0),
		RADIO_REG_20707(pi, RX5G_REG6, 0),
		RADIO_REG_20707(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20707(pi, LOGEN_CORE_REG0, 0),
		//RADIO_REG_20707(pi, TX2G_PA_REG0, 0),
		RADIO_REG_20707(pi, TXDAC_REG3, 0),
		RADIO_REG_20707(pi, RXADC_CFG0, 0),
		RADIO_REG_20707(pi, LPF_NOTCH_OVR1, 0),
		RADIO_REG_20707(pi, LPF_NOTCH_REG2, 0),
		RADIO_REG_20707(pi, LPF_NOTCH_REG3, 0),
		RADIO_REG_20707(pi, LPF_NOTCH_REG4, 0),
		RADIO_REG_20707(pi, LPF_NOTCH_REG5, 0)
	};
	BCM_REFERENCE(pi);

	if (!populate) {
		uint16 radioreg_size = 0;
		uint16 phyreg_size = 0;
		p->reg_cache_id = id;
		p->phy_reg_cache_id = id;

		radioreg_size = MAX(ARRAYSIZE(radioregs_txiqlocal_20707_majorrev129),
			ARRAYSIZE(radioregs_rxiqcal_20707_majorrev129));
		radioreg_size = MAX(radioreg_size,
			ARRAYSIZE(radioregs_afecal_20707_majorrev129));
		radioreg_size = MAX(radioreg_size,
			ARRAYSIZE(radioregs_tempsense_vbat_20707_majorrev129));
		if (WBPAPD_ENAB(pi)) {
			radioreg_size = MAX(radioreg_size,
				ARRAYSIZE(radioregs_wbpapdcal_20707_majorrev129));
		} else {
			radioreg_size = MAX(radioreg_size,
				ARRAYSIZE(radioregs_papdcal_20707_majorrev129));
		}

		phyreg_size = ARRAYSIZE(phyregs_papdcal_majorrev129);
		phyreg_size = MAX(phyreg_size, ARRAYSIZE(phyregs_tempsense_vbat));
		phyreg_size = MAX(phyreg_size, ARRAYSIZE(phyreg_tssi_setup_majorrev129));

		p->phy_reg_cache_depth =  phyreg_size;
		p->reg_cache_depth = radioreg_size;

	} else {
		uint16 *list = NULL;
		uint16 sz = 0;

		switch (id) {
			case RADIOREGS_TXIQCAL:
				list = radioregs_txiqlocal_20707_majorrev129;
				sz   = ARRAYSIZE(radioregs_txiqlocal_20707_majorrev129);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_RXIQCAL:
				list = radioregs_rxiqcal_20707_majorrev129;
				sz   = ARRAYSIZE(radioregs_rxiqcal_20707_majorrev129);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_PAPDCAL:
				if (WBPAPD_ENAB(pi)) {
					list = radioregs_wbpapdcal_20707_majorrev129;
					sz   = ARRAYSIZE(
						radioregs_wbpapdcal_20707_majorrev129);
				} else {
					list = radioregs_papdcal_20707_majorrev129;
					sz   = ARRAYSIZE(
						radioregs_papdcal_20707_majorrev129);
				}
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case PHYREGS_PAPDCAL:
				list = phyregs_papdcal_majorrev129;
				sz   = ARRAYSIZE(phyregs_papdcal_majorrev129);
				base = p->phy_reg_cache; /* to baseline for looping over cores */
				p->phy_reg_sz = sz;
				p->phy_reg_cache_id = id;
				break;
			case PHYREGS_TEMPSENSE_VBAT:
				list = phyregs_tempsense_vbat;
				sz   = ARRAYSIZE(phyregs_tempsense_vbat);
				base = p->phy_reg_cache; /* to baseline for looping over cores */
				p->phy_reg_sz = sz;
				p->phy_reg_cache_id = id;
				break;
			case PHYREGS_TSSI:
				list = phyreg_tssi_setup_majorrev129;
				sz   = ARRAYSIZE(phyreg_tssi_setup_majorrev129);
				base = p->phy_reg_cache; /* to baseline for looping over cores */
				p->phy_reg_sz = sz;
				p->phy_reg_cache_id = id;
				break;
			case RADIOREGS_AFECAL:
				list = radioregs_afecal_20707_majorrev129;
				sz   = ARRAYSIZE(radioregs_afecal_20707_majorrev129);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_TEMPSENSE_VBAT:
				list = radioregs_tempsense_vbat_20707_majorrev129;
				sz = ARRAYSIZE(radioregs_tempsense_vbat_20707_majorrev129);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_TSSI:
				list = radioregs_tssi_setup_20707_majorrev129;
				sz   = ARRAYSIZE(radioregs_tssi_setup_20707_majorrev129);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			default:
				PHY_ERROR(("%s: Invalid ID %d\n", __FUNCTION__, id));
				ASSERT(0);
				break;
		}
		if (list != NULL) {
			uint8 i = 0, core = 0;
			/* update reg cache id in use */
			/* align to base */
			cache = base;
			PHY_INFORM(("***** core %d ***** : %s\n", core, __FUNCTION__));
			for (i = 0; i < sz; i++) {
				cache->addr[core] = list[i];
				cache->addr[core+1] = (list[i] + JTAG_20707_CR1);
				cache->addr[core+2] = (list[i] + JTAG_20707_CR2);
				cache->data[core] = 0;
				cache->data[core+1] = 0;
				cache->data[core+2] = 0;
				PHY_INFORM(("0x%8p: 0x%04x\n", cache, cache->addr[core]));
				cache++;
			}
		}
	}
}

static void
phy_ac_reg_cache_parse_20708(phy_info_acphy_t *pi_ac, uint16 id, bool populate)
{
	phy_info_t *pi = pi_ac->pi;
	phy_param_info_t *p = pi_ac->paramsi;
	ad_t *base = p->reg_cache;
	ad_t *cache = base; /* to baseline for looping over cores */

	uint16 phyregs_tempsense_vbat[] = {
		ACPHY_REG(pi, RfctrlOverrideAuxTssi0),
		ACPHY_REG(pi, RfctrlCoreAuxTssi10),
		ACPHY_REG(pi, RfctrlOverrideRxPus0),
		ACPHY_REG(pi, RfctrlCoreRxPus0),
		ACPHY_REG(pi, RxFeCtrl1),
		ACPHY_REG(pi, RfctrlOverrideTxPus0),
		ACPHY_REG(pi, RfctrlCoreTxPus0),
		ACPHY_REG(pi, RfctrlOverrideGains0),
		ACPHY_REG(pi, RfctrlCoreLpfGain0),
		ACPHY_REG(pi, RxSdFeConfig1),
		ACPHY_REG(pi, RxSdFeConfig6),
		ACPHY_REG(pi, RfctrlOverrideLpfSwtch0),
		ACPHY_REG(pi, RfctrlCoreLpfSwtch0),
		ACPHY_REG(pi, RfctrlOverrideAfeCfg0),
		ACPHY_REG(pi, RfctrlCoreAfeCfg10),
		ACPHY_REG(pi, gpioLoOutEn),
		ACPHY_REG(pi, gpioHiOutEn),
		ACPHY_REG(pi, gpioSel),
		ACPHY_REG(pi, RfctrlCoreAfeCfg20)
	};

	uint16 phyregs_papdcal_majorrev130[] = {
		ACPHY_REG(pi, RfctrlOverrideTxPus0),
		ACPHY_REG(pi, RfctrlOverrideRxPus0),
		ACPHY_REG(pi, RfctrlOverrideGains0),
		ACPHY_REG(pi, RfctrlOverrideLpfCT0),
		ACPHY_REG(pi, RfctrlOverrideLpfSwtch0),
		ACPHY_REG(pi, RfctrlOverrideAfeCfg0),
		ACPHY_REG(pi, RfctrlOverrideLowPwrCfg0),
		ACPHY_REG(pi, RfctrlOverrideAuxTssi0),
		ACPHY_REG(pi, RfctrlCoreTxPus0),
		ACPHY_REG(pi, RfctrlCoreRxPus0),
		ACPHY_REG(pi, RfctrlCoreTXGAIN10),
		ACPHY_REG(pi, RfctrlCoreTXGAIN20),
		ACPHY_REG(pi, RfctrlCoreRXGAIN10),
		ACPHY_REG(pi, RfctrlCoreRXGAIN20),
		ACPHY_REG(pi, RfctrlCoreLpfGain0),
		ACPHY_REG(pi, RfctrlCoreLpfCT0),
		ACPHY_REG(pi, RfctrlCoreLpfGmult0),
		ACPHY_REG(pi, RfctrlCoreRCDACBuf0),
		ACPHY_REG(pi, RfctrlCoreLpfSwtch0),
		ACPHY_REG(pi, RfctrlCoreAfeCfg10),
		ACPHY_REG(pi, RfctrlCoreAfeCfg20),
		ACPHY_REG(pi, RfctrlCoreLowPwr0),
		ACPHY_REG(pi, RfctrlCoreAuxTssi10),
		ACPHY_REG(pi, RfctrlCoreAuxTssi20),
		ACPHY_REG(pi, Dac_gain0),
		ACPHY_REG(pi, RfctrlIntc0),
		ACPHY_REG(pi, forceFront0),
		ACPHY_REG(pi, PapdEnable0)
	};

	uint16 phyreg_tssi_setup_majorrev130[] = {
		ACPHY_REG(pi, RfctrlOverrideAuxTssi0),
		ACPHY_REG(pi, RfctrlCoreAuxTssi10),
		ACPHY_REG(pi, RfctrlIntc0),
		ACPHY_REG(pi, ClassifierCtrl),
		ACPHY_REG(pi, RxFeCtrl1),
		ACPHY_REG(pi, AfePuCtrl)
	};

	uint16 radioregs_tssi_setup_20708_majorrev130[] = {
		//RADIO_REG_20708(pi, TX5G_TSSI_REG0, 0),
		//RADIO_REG_20708(pi, TX2G_CFG1_OVR, 0),
		RADIO_REG_20708(pi, TX2G_MIX_REG0, 0),
		RADIO_REG_20708(pi, TX5G_MIX_REG0, 0),
		RADIO_REG_20708(pi, IQCAL_OVR1, 0),
		RADIO_REG_20708(pi, IQCAL_CFG1, 0),
		RADIO_REG_20708(pi, IQCAL_CFG4, 0),
		RADIO_REG_20708(pi, IQCAL_CFG5, 0),
		RADIO_REG_20708(pi, IQCAL_CFG6, 0),
		RADIO_REG_20708(pi, IQCAL_IDAC, 0),
		RADIO_REG_20708(pi, TESTBUF_OVR1, 0),
		RADIO_REG_20708(pi, TESTBUF_CFG1, 0),
		RADIO_REG_20708(pi, AUXPGA_OVR1, 0),
		RADIO_REG_20708(pi, AUXPGA_CFG1, 0),
		RADIO_REG_20708(pi, AUXPGA_VMID, 0),
		RADIO_REG_20708(pi, LPF_OVR1, 0),
		RADIO_REG_20708(pi, LPF_OVR2, 0),
		RADIO_REG_20708(pi, LPF_REG7, 0),
		RADIO_REG_20708(pi, LPF_REG8, 0)
	};

	uint16 radioregs_tempsense_vbat_20708_majorrev130[] = {
		RADIO_REG_20708(pi, TEMPSENSE_OVR1, 0),
		RADIO_REG_20708(pi, TEMPSENSE_CFG, 0),
		RADIO_REG_20708(pi, TESTBUF_OVR1, 0),
		RADIO_REG_20708(pi, TESTBUF_CFG1, 0),
		RADIO_REG_20708(pi, AUXPGA_OVR1, 0),
		RADIO_REG_20708(pi, AUXPGA_CFG1, 0),
		RADIO_REG_20708(pi, AUXPGA_VMID, 0),
		RADIO_REG_20708(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20708(pi, TIA_REG7, 0),
		RADIO_REG_20708(pi, LPF_OVR1, 0),
		RADIO_REG_20708(pi, LPF_OVR2, 0),
		RADIO_REG_20708(pi, LPF_REG7, 0),
		RADIO_REG_20708(pi, IQCAL_CFG4, 0),
		RADIO_REG_20708(pi, IQCAL_OVR1, 0),
		RADIO_REG_20708(pi, IQCAL_CFG5, 0)
	};

	uint16 radioregs_txiqlocal_20708_majorrev130[] = {
		RADIO_REG_20708(pi, IQCAL_OVR1, 0),
		RADIO_REG_20708(pi, IQCAL_CFG1, 0),
		RADIO_REG_20708(pi, IQCAL_CFG4, 0),
		RADIO_REG_20708(pi, IQCAL_CFG5, 0),
		RADIO_REG_20708(pi, IQCAL_GAIN_RIN, 0),
		RADIO_REG_20708(pi, IQCAL_GAIN_RFB, 0),
		RADIO_REG_20708(pi, LPF_OVR1, 0),
		RADIO_REG_20708(pi, LPF_OVR2, 0),
		RADIO_REG_20708(pi, LPF_REG7, 0),
		RADIO_REG_20708(pi, LPF_NOTCH_CONTROL2, 0),
		RADIO_REG_20708(pi, TXDB_CFG1_OVR, 0),
		RADIO_REG_20708(pi, TXDB_REG0, 0),
		RADIO_REG_20708(pi, TX2G_MIX_REG0, 0),
		RADIO_REG_20708(pi, TX2G_MISC_CFG1, 0),
		RADIO_REG_20708(pi, AUXPGA_OVR1, 0),
		RADIO_REG_20708(pi, AUXPGA_CFG1, 0),
		RADIO_REG_20708(pi, AUXPGA_VMID, 0),
		RADIO_REG_20708(pi, TESTBUF_OVR1, 0),
		RADIO_REG_20708(pi, TESTBUF_CFG1, 0),
		RADIO_REG_20708(pi, TXDAC_REG6, 0),
		RADIO_REG_20708(pi, TX2G_PAD_REG1, 0),
		RADIO_REG_20708(pi, TX2G_MIX_REG4, 0),
		RADIO_REG_20708(pi, TX2G_MIX_REG2, 0),
		RADIO_REG_20708(pi, TXDB_PAD_REG3, 0),
		RADIO_REG_20708(pi, TX2G_CFG1_OVR, 0),
		RADIO_REG_20708(pi, TX2G_MIX_GC_REG, 0),
		RADIO_REG_20708(pi, TX2G_PAD_GC_REG, 0),
	};

	uint16 radioregs_rxiqcal_20708_majorrev130[] = {
		RADIO_REG_20708(pi, LPF_OVR1, 0),
		RADIO_REG_20708(pi, LPF_OVR2, 0),
		RADIO_REG_20708(pi, LPF_NOTCH_CONTROL2, 0),
		RADIO_REG_20708(pi, LPF_REG7, 0),
		RADIO_REG_20708(pi, TIA_REG7, 0),
		RADIO_REG_20708(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20708(pi, RXDB_CFG1_OVR, 0),
		RADIO_REG_20708(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20708(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20708(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20708(pi, RX5G_REG4, 0),
		RADIO_REG_20708(pi, LOGEN_CORE_REG5, 0),
		RADIO_REG_20708(pi, RX5G_REG2, 0),
		RADIO_REG_20708(pi, RX5G_REG6, 0),
		RADIO_REG_20708(pi, RX2G_REG4, 0),
		RADIO_REG_20708(pi, RX5G_REG1, 0),
		RADIO_REG_20708(pi, TXDAC_REG3, 0),
		RADIO_REG_20708(pi, TXDAC_REG0, 0),
		RADIO_REG_20708(pi, TXDAC_REG1, 0),
		RADIO_REG_20708(pi, LPF_REG6, 0),
		RADIO_REG_20708(pi, RX5G_REG5, 0),
		RADIO_REG_20708(pi, TX2G_MIX_REG0, 0),
		RADIO_REG_20708(pi, TX2G_CFG1_OVR, 0),
		RADIO_REG_20708(pi, TX2G_PAD_REG0, 0),
		RADIO_REG_20708(pi, TX5G_MIX_REG0, 0),
		RADIO_REG_20708(pi, TXDB_REG0, 0),
		RADIO_REG_20708(pi, TXDB_PAD_REG3, 0),
		RADIO_REG_20708(pi, TX2G_MIX_GC_REG, 0),
		RADIO_REG_20708(pi, TX2G_PAD_REG1, 0),
		RADIO_REG_20708(pi, TX2G_MIX_REG4, 0),
		RADIO_REG_20708(pi, TX2G_MIX_REG2, 0),
		RADIO_REG_20708(pi, TXDAC_REG6, 0),
		RADIO_REG_20708(pi, RXDB_SPARE, 0),
		RADIO_REG_20708(pi, TXDB_CFG1_OVR, 0),
	};

	uint16 radioregs_afecal_20708_majorrev130[] = {
		RADIO_REG_20708(pi, AFE_CFG1_OVR1, 0),
		RADIO_REG_20708(pi, AFE_CFG1_OVR2, 0),
		RADIO_REG_20708(pi, RXADC_CFG0, 0),
		RADIO_REG_20708(pi, RXADC_CFG10, 0),
		RADIO_REG_20708(pi, TXDAC_REG7, 0),
		RADIO_REG_20708(pi, TXDAC_REG0, 0),
		RADIO_REG_20708(pi, TXDAC_REG1, 0),
		RADIO_REG_20708(pi, TXDAC_REG3, 0),
		RADIO_REG_20708(pi, LPF_REG7, 0),
		RADIO_REG_20708(pi, LPF_OVR1, 0),
		RADIO_REG_20708(pi, LPF_OVR2, 0),
		RADIO_REG_20708(pi, RXADC_CFG1, 0),
		RADIO_REG_20708(pi, RXADC_CFG2, 0),
		RADIO_REG_20708(pi, RXADC_CFG3, 0),
		RADIO_REG_20708(pi, RXADC_CFG5, 0),
		RADIO_REG_20708(pi, RXADC_REG2, 0),
		RADIO_REG_20708(pi, RXADC_REG3, 0)
	};

	uint16 radioregs_papdcal_20708_majorrev130[] = {
		RADIO_REG_20708(pi, LPF_OVR1, 0),
		RADIO_REG_20708(pi, LPF_OVR2, 0),
		RADIO_REG_20708(pi, LPF_REG7, 0),
		RADIO_REG_20708(pi, LPF_REG6, 0),
		RADIO_REG_20708(pi, TIA_REG7, 0),
		RADIO_REG_20708(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20708(pi, TIA_DCDAC_REG1, 0),
		RADIO_REG_20708(pi, TIA_DCDAC_REG2, 0),
		RADIO_REG_20708(pi, RXDB_CFG1_OVR, 0),
		RADIO_REG_20708(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20708(pi, RX5G_REG1, 0),
		RADIO_REG_20708(pi, RX5G_REG2, 0),
		RADIO_REG_20708(pi, RX2G_REG4, 0),
		RADIO_REG_20708(pi, RX5G_REG4, 0),
		RADIO_REG_20708(pi, RX5G_REG5, 0),
		RADIO_REG_20708(pi, RX5G_REG6, 0),
		RADIO_REG_20708(pi, RXDB_SPARE, 0),
		RADIO_REG_20708(pi, RXADC_CFG0, 0),
		RADIO_REG_20708(pi, LPF_NOTCH_REG6, 0)
	};
	uint16 radioregs_wbpapdcal_20708_majorrev130[] = {
		RADIO_REG_20708(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20708(pi, LPF_OVR1, 0),
		RADIO_REG_20708(pi, LPF_OVR2, 0),
		RADIO_REG_20708(pi, LPF_REG1, 0),
		RADIO_REG_20708(pi, LPF_REG2, 0),
		RADIO_REG_20708(pi, LPF_REG3, 0),
		RADIO_REG_20708(pi, LPF_REG4, 0),
		RADIO_REG_20708(pi, LPF_REG6, 0),
		RADIO_REG_20708(pi, LPF_REG7, 0),
		RADIO_REG_20708(pi, LPF_REG11, 0),
		RADIO_REG_20708(pi, LPF_REG12, 0),
		RADIO_REG_20708(pi, TIA_REG1, 0),
		RADIO_REG_20708(pi, TIA_REG2, 0),
		RADIO_REG_20708(pi, TIA_REG3, 0),
		RADIO_REG_20708(pi, TIA_REG4, 0),
		RADIO_REG_20708(pi, TIA_REG5, 0),
		RADIO_REG_20708(pi, TIA_REG6, 0),
		RADIO_REG_20708(pi, TIA_REG7, 0),
		RADIO_REG_20708(pi, RXDB_CFG1_OVR, 0),
		RADIO_REG_20708(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20708(pi, RX5G_REG1, 0),
		RADIO_REG_20708(pi, RX5G_REG2, 0),
		RADIO_REG_20708(pi, RX2G_REG4, 0),
		RADIO_REG_20708(pi, RX5G_REG4, 0),
		RADIO_REG_20708(pi, RX5G_REG5, 0),
		RADIO_REG_20708(pi, RX5G_REG6, 0),
		RADIO_REG_20708(pi, RXDB_SPARE, 0),
		RADIO_REG_20708(pi, RXADC_CFG0, 0),
		RADIO_REG_20708(pi, LPF_NOTCH_OVR1, 0),
		RADIO_REG_20708(pi, LPF_NOTCH_REG1, 0),
		RADIO_REG_20708(pi, LPF_NOTCH_REG2, 0),
		RADIO_REG_20708(pi, LPF_NOTCH_REG3, 0),
		RADIO_REG_20708(pi, LPF_NOTCH_REG4, 0),
		RADIO_REG_20708(pi, LPF_NOTCH_REG5, 0),
		RADIO_REG_20708(pi, LPF_NOTCH_REG6, 0),
		RADIO_REG_20708(pi, SPARE_CFG0, 0)
	};
	BCM_REFERENCE(pi);

	if (!populate) {
		uint16 radioreg_size = 0;
		uint16 phyreg_size = 0;
		p->reg_cache_id = id;
		p->phy_reg_cache_id = id;

		radioreg_size = MAX(ARRAYSIZE(radioregs_txiqlocal_20708_majorrev130),
			ARRAYSIZE(radioregs_rxiqcal_20708_majorrev130));
		radioreg_size = MAX(radioreg_size,
			ARRAYSIZE(radioregs_afecal_20708_majorrev130));
		radioreg_size = MAX(radioreg_size,
			ARRAYSIZE(radioregs_tempsense_vbat_20708_majorrev130));
		if (WBPAPD_ENAB(pi)) {
			radioreg_size = MAX(radioreg_size,
				ARRAYSIZE(radioregs_wbpapdcal_20708_majorrev130));
		} else {
			radioreg_size = MAX(radioreg_size,
				ARRAYSIZE(radioregs_papdcal_20708_majorrev130));
		}

		phyreg_size = ARRAYSIZE(phyregs_papdcal_majorrev130);
		phyreg_size = MAX(phyreg_size, ARRAYSIZE(phyregs_tempsense_vbat));
		phyreg_size = MAX(phyreg_size, ARRAYSIZE(phyreg_tssi_setup_majorrev130));

		p->phy_reg_cache_depth =  phyreg_size;
		p->reg_cache_depth = radioreg_size;

	} else {
		uint16 *list = NULL;
		uint16 sz = 0;

		switch (id) {
			case RADIOREGS_TXIQCAL:
				list = radioregs_txiqlocal_20708_majorrev130;
				sz   = ARRAYSIZE(radioregs_txiqlocal_20708_majorrev130);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_RXIQCAL:
				list = radioregs_rxiqcal_20708_majorrev130;
				sz   = ARRAYSIZE(radioregs_rxiqcal_20708_majorrev130);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_PAPDCAL:
				if (WBPAPD_ENAB(pi)) {
					list = radioregs_wbpapdcal_20708_majorrev130;
					sz   = ARRAYSIZE(
						radioregs_wbpapdcal_20708_majorrev130);
				} else {
					list = radioregs_papdcal_20708_majorrev130;
					sz   = ARRAYSIZE(
						radioregs_papdcal_20708_majorrev130);
				}
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case PHYREGS_PAPDCAL:
				list = phyregs_papdcal_majorrev130;
				sz   = ARRAYSIZE(phyregs_papdcal_majorrev130);
				base = p->phy_reg_cache; /* to baseline for looping over cores */
				p->phy_reg_sz = sz;
				p->phy_reg_cache_id = id;
				break;
			case PHYREGS_TEMPSENSE_VBAT:
				list = phyregs_tempsense_vbat;
				sz   = ARRAYSIZE(phyregs_tempsense_vbat);
				base = p->phy_reg_cache; /* to baseline for looping over cores */
				p->phy_reg_sz = sz;
				p->phy_reg_cache_id = id;
				break;
			case PHYREGS_TSSI:
				list = phyreg_tssi_setup_majorrev130;
				sz   = ARRAYSIZE(phyreg_tssi_setup_majorrev130);
				base = p->phy_reg_cache; /* to baseline for looping over cores */
				p->phy_reg_sz = sz;
				p->phy_reg_cache_id = id;
				break;
			case RADIOREGS_AFECAL:
				list = radioregs_afecal_20708_majorrev130;
				sz   = ARRAYSIZE(radioregs_afecal_20708_majorrev130);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_TEMPSENSE_VBAT:
				list = radioregs_tempsense_vbat_20708_majorrev130;
				sz = ARRAYSIZE(radioregs_tempsense_vbat_20708_majorrev130);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_TSSI:
				list = radioregs_tssi_setup_20708_majorrev130;
				sz   = ARRAYSIZE(radioregs_tssi_setup_20708_majorrev130);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			default:
				PHY_ERROR(("%s: Invalid ID %d\n", __FUNCTION__, id));
				ASSERT(0);
				break;
		}
		if (list != NULL) {
			uint8 i = 0, core = 0;
			/* update reg cache id in use */
			/* align to base */
			cache = base;
			PHY_INFORM(("***** core %d ***** : %s\n", core, __FUNCTION__));
			for (i = 0; i < sz; i++) {
				cache->addr[core] = list[i];
				cache->addr[core+1] = (list[i] + JTAG_20708_CR1);
				cache->addr[core+2] = (list[i] + JTAG_20708_CR2);
				cache->addr[core+3] = (list[i] + JTAG_20708_CR3);
				cache->data[core] = 0;
				cache->data[core+1] = 0;
				cache->data[core+2] = 0;
				cache->data[core+3] = 0;
				PHY_INFORM(("0x%8p: 0x%04x\n", cache, cache->addr[core]));
				cache++;
			}
		}
	}
}

static void
phy_ac_reg_cache_parse_20709(phy_info_acphy_t *pi_ac, uint16 id, bool populate)
{
	phy_info_t *pi = pi_ac->pi;
	phy_param_info_t *p = pi_ac->paramsi;
	ad_t *base = p->reg_cache;
	ad_t *cache = base; /* to baseline for looping over cores */

	uint16 phyregs_tempsense_vbat[] = {
		ACPHY_REG(pi, RfctrlOverrideAuxTssi0),
		ACPHY_REG(pi, RfctrlCoreAuxTssi10),
		ACPHY_REG(pi, RfctrlOverrideRxPus0),
		ACPHY_REG(pi, RfctrlCoreRxPus0),
		ACPHY_REG(pi, RxFeCtrl1),
		ACPHY_REG(pi, RfctrlOverrideTxPus0),
		ACPHY_REG(pi, RfctrlCoreTxPus0),
		ACPHY_REG(pi, RfctrlOverrideGains0),
		ACPHY_REG(pi, RfctrlCoreLpfGain0),
		ACPHY_REG(pi, RxSdFeConfig1),
		ACPHY_REG(pi, RxSdFeConfig6),
		ACPHY_REG(pi, RfctrlOverrideLpfSwtch0),
		ACPHY_REG(pi, RfctrlCoreLpfSwtch0),
		ACPHY_REG(pi, RfctrlOverrideAfeCfg0),
		ACPHY_REG(pi, RfctrlCoreAfeCfg10),
		ACPHY_REG(pi, gpioLoOutEn),
		ACPHY_REG(pi, gpioHiOutEn),
		ACPHY_REG(pi, gpioSel),
		ACPHY_REG(pi, RfctrlCoreAfeCfg20)
	};

	uint16 phyregs_papdcal[] = {
		ACPHY_REG(pi, RfctrlOverrideTxPus0),
		ACPHY_REG(pi, RfctrlOverrideRxPus0),
		ACPHY_REG(pi, RfctrlOverrideGains0),
		ACPHY_REG(pi, RfctrlOverrideLpfCT0),
		ACPHY_REG(pi, RfctrlOverrideLpfSwtch0),
		ACPHY_REG(pi, RfctrlOverrideAfeCfg0),
		ACPHY_REG(pi, RfctrlOverrideLowPwrCfg0),
		ACPHY_REG(pi, RfctrlOverrideAuxTssi0),
		ACPHY_REG(pi, RfctrlCoreTxPus0),
		ACPHY_REG(pi, RfctrlCoreRxPus0),
		ACPHY_REG(pi, RfctrlCoreTXGAIN10),
		ACPHY_REG(pi, RfctrlCoreTXGAIN20),
		ACPHY_REG(pi, RfctrlCoreRXGAIN10),
		ACPHY_REG(pi, RfctrlCoreRXGAIN20),
		ACPHY_REG(pi, RfctrlCoreLpfGain0),
		ACPHY_REG(pi, RfctrlCoreLpfCT0),
		ACPHY_REG(pi, RfctrlCoreLpfGmult0),
		ACPHY_REG(pi, RfctrlCoreRCDACBuf0),
		ACPHY_REG(pi, RfctrlCoreLpfSwtch0),
		ACPHY_REG(pi, RfctrlCoreAfeCfg10),
		ACPHY_REG(pi, RfctrlCoreAfeCfg20),
		ACPHY_REG(pi, RfctrlCoreLowPwr0),
		ACPHY_REG(pi, RfctrlCoreAuxTssi10),
		ACPHY_REG(pi, RfctrlCoreAuxTssi20),
		ACPHY_REG(pi, Dac_gain0),
		ACPHY_REG(pi, RfctrlIntc0),
		ACPHY_REG(pi, forceFront0),
		ACPHY_REG(pi, PapdEnable0)
	};

	uint16 phyreg_tssi_setup[] = {
		ACPHY_REG(pi, RfctrlOverrideAuxTssi0),
		ACPHY_REG(pi, RfctrlCoreAuxTssi10),
		ACPHY_REG(pi, RfctrlIntc0),
		ACPHY_REG(pi, ClassifierCtrl),
		ACPHY_REG(pi, RxFeCtrl1),
		ACPHY_REG(pi, AfePuCtrl)
	};

	/* BCM6878 lists for save & restore functions */
	uint16 radioregs_txiqlocal_20709_majorrev128[] = {
		RADIO_REG_20709(pi, IQCAL_OVR1, 0),
		RADIO_REG_20709(pi, IQCAL_CFG1, 0),
		RADIO_REG_20709(pi, IQCAL_CFG4, 0),
		RADIO_REG_20709(pi, IQCAL_CFG5, 0),
		RADIO_REG_20709(pi, IQCAL_GAIN_RIN, 0),
		RADIO_REG_20709(pi, IQCAL_GAIN_RFB, 0),
		RADIO_REG_20709(pi, LPF_OVR1, 0),
		RADIO_REG_20709(pi, LPF_OVR2, 0),
		RADIO_REG_20709(pi, LPF_REG7, 0),
		RADIO_REG_20709(pi, TX2G_TSSI_REG0, 0),
		RADIO_REG_20709(pi, TX2G_TSSI_REG1, 0),
		RADIO_REG_20709(pi, TX5G_TSSI_REG0, 0),
		RADIO_REG_20709(pi, TX5G_TSSI_REG1, 0),
		RADIO_REG_20709(pi, TX2G_CFG_OVR, 0),
		RADIO_REG_20709(pi, TX5G_CFG_OVR, 0),
		RADIO_REG_20709(pi, AUXPGA_OVR1, 0),
		RADIO_REG_20709(pi, AUXPGA_CFG1, 0),
		RADIO_REG_20709(pi, TESTBUF_OVR1, 0),
		RADIO_REG_20709(pi, TESTBUF_CFG1, 0),
		RADIO_REG_20709(pi, TXDAC_REG0, 0),
		RADIO_REG_20709(pi, TXDAC_REG1, 0),
		RADIO_REG_20709(pi, TXDAC_REG3, 0),
		RADIO_REG_20709(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20709(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20709(pi, TX5G_IPA_GC_REG, 0),
		RADIO_REG_20709(pi, TX5G_IPA_CFG_OVR, 0)
	};
	uint16 radioregs_rxiqcal_20709_majorrev128[] = {
		RADIO_REG_20709(pi, LPF_OVR1, 0),
		RADIO_REG_20709(pi, LPF_OVR2, 0),
		RADIO_REG_20709(pi, LPF_REG6, 0),
		RADIO_REG_20709(pi, LPF_REG7, 0),
		RADIO_REG_20709(pi, TIA_REG7, 0),
		RADIO_REG_20709(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20709(pi, RXDB_CFG1_OVR, 0),
		RADIO_REG_20709(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20709(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20709(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20709(pi, LOGEN_CORE_REG5, 0),
		RADIO_REG_20709(pi, RX5G_REG4, 0),
		RADIO_REG_20709(pi, RX5G_REG5, 0),
		RADIO_REG_20709(pi, RX5G_REG2, 0),
		RADIO_REG_20709(pi, RX5G_REG6, 0),
		RADIO_REG_20709(pi, RX5G_REG1, 0),
		RADIO_REG_20709(pi, RX5G_REG4, 0),
		RADIO_REG_20709(pi, RX2G_REG4, 0),
		RADIO_REG_20709(pi, TXDAC_REG0, 0),
		RADIO_REG_20709(pi, TXDAC_REG1, 0),
		RADIO_REG_20709(pi, TXDAC_REG3, 0),
		RADIO_REG_20709(pi, LPF_REG6, 0),
		RADIO_REG_20709(pi, TX5G_IPA_REG0, 0),
		RADIO_REG_20709(pi, TX5G_IPA_CFG_OVR, 0),
		RADIO_REG_20709(pi, TX5G_IPA_REG0, 0),
		RADIO_REG_20709(pi, TX2G_IPA_REG0, 0),
		RADIO_REG_20709(pi, TX2G_IPA_CFG_OVR, 0),
		RADIO_REG_20709(pi, TX2G_PAD_REG0, 0),
		RADIO_REG_20709(pi, TX5G_PAD_REG0, 0),
		RADIO_REG_20709(pi, RX_TOP_REG1, 0)
	};
	uint16 radioregs_afecal_20709_majorrev128[] = {
		RADIO_REG_20709(pi, AFE_CFG1_OVR2, 0),
		RADIO_REG_20709(pi, TXDAC_REG0, 0),
		RADIO_REG_20709(pi, LPF_OVR1, 0),
		RADIO_REG_20709(pi, LPF_OVR2, 0)
	};
	uint16 radioregs_tempsense_vbat_20709_majorrev128[] = {
		RADIO_REG_20709(pi, TEMPSENSE_OVR1, 0),
		RADIO_REG_20709(pi, TEMPSENSE_CFG, 0),
		RADIO_REG_20709(pi, TESTBUF_OVR1, 0),
		RADIO_REG_20709(pi, TESTBUF_CFG1, 0),
		RADIO_REG_20709(pi, AUXPGA_OVR1, 0),
		RADIO_REG_20709(pi, AUXPGA_CFG1, 0),
		RADIO_REG_20709(pi, AUXPGA_VMID, 0),
		RADIO_REG_20709(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20709(pi, TIA_REG7, 0),
		RADIO_REG_20709(pi, LPF_OVR1, 0),
		RADIO_REG_20709(pi, LPF_OVR2, 0),
		RADIO_REG_20709(pi, LPF_REG7, 0),
		RADIO_REG_20709(pi, IQCAL_CFG4, 0),
		RADIO_REG_20709(pi, IQCAL_OVR1, 0),
		RADIO_REG_20709(pi, IQCAL_CFG5, 0)
	};
	uint16 radioregs_wbpapdcal_20709_majorrev128[] = {
		RADIO_REG_20709(pi, LPF_OVR1, 0),
		RADIO_REG_20709(pi, LPF_OVR2, 0),
		RADIO_REG_20709(pi, LPF_REG1, 0),
		RADIO_REG_20709(pi, LPF_REG2, 0),
		RADIO_REG_20709(pi, LPF_REG3, 0),
		RADIO_REG_20709(pi, LPF_REG4, 0),
		RADIO_REG_20709(pi, LPF_REG6, 0),
		RADIO_REG_20709(pi, LPF_REG7, 0),
		RADIO_REG_20709(pi, LPF_REG11, 0),
		RADIO_REG_20709(pi, TIA_REG1, 0),
		RADIO_REG_20709(pi, TIA_REG2, 0),
		RADIO_REG_20709(pi, TIA_REG3, 0),
		RADIO_REG_20709(pi, TIA_REG4, 0),
		RADIO_REG_20709(pi, TIA_REG5, 0),
		RADIO_REG_20709(pi, TIA_REG6, 0),
		RADIO_REG_20709(pi, TIA_REG7, 0),
		RADIO_REG_20709(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20709(pi, RXDB_CFG1_OVR, 0),
		RADIO_REG_20709(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20709(pi, RX5G_REG1, 0),
		RADIO_REG_20709(pi, RX5G_REG2, 0),
		RADIO_REG_20709(pi, RX2G_REG4, 0),
		RADIO_REG_20709(pi, RX5G_REG4, 0),
		RADIO_REG_20709(pi, RX5G_REG5, 0),
		RADIO_REG_20709(pi, RX5G_REG6, 0),
		RADIO_REG_20709(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20709(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20709(pi, LOGEN_CORE_REG5, 0),
		RADIO_REG_20709(pi, TX2G_IPA_REG0, 0),
		RADIO_REG_20709(pi, TX5G_IPA_REG0, 0),
		RADIO_REG_20709(pi, TXDAC_REG3, 0),
		RADIO_REG_20709(pi, RXADC_CFG0, 0),
		RADIO_REG_20709(pi, LPF_NOTCH_OVR1, 0),
		RADIO_REG_20709(pi, LPF_NOTCH_REG2, 0),
		RADIO_REG_20709(pi, LPF_NOTCH_REG3, 0),
		RADIO_REG_20709(pi, LPF_NOTCH_REG4, 0),
		RADIO_REG_20709(pi, LPF_NOTCH_REG5, 0),
		RADIO_REG_20709(pi, LPF_NOTCH_REG6, 0)
	};
	uint16 radioregs_papdcal_20709_majorrev128[] = {
		RADIO_REG_20709(pi, LPF_OVR1, 0),
		RADIO_REG_20709(pi, LPF_OVR2, 0),
		RADIO_REG_20709(pi, LPF_REG6, 0),
		RADIO_REG_20709(pi, LPF_REG7, 0),
		RADIO_REG_20709(pi, TIA_REG7, 0),
		RADIO_REG_20709(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20709(pi, RXDB_CFG1_OVR, 0),
		RADIO_REG_20709(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20709(pi, RX5G_REG1, 0),
		RADIO_REG_20709(pi, RX5G_REG2, 0),
		RADIO_REG_20709(pi, RX2G_REG4, 0),
		RADIO_REG_20709(pi, RX5G_REG4, 0),
		RADIO_REG_20709(pi, RX5G_REG5, 0),
		RADIO_REG_20709(pi, RX5G_REG6, 0),
		RADIO_REG_20709(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20709(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20709(pi, LOGEN_CORE_REG5, 0),
		RADIO_REG_20709(pi, TXDAC_REG3, 0),
		RADIO_REG_20709(pi, RXADC_CFG0, 0),
		RADIO_REG_20709(pi, LPF_NOTCH_REG6, 0),
		RADIO_REG_20709(pi, TX2G_IPA_REG0, 0),
		RADIO_REG_20709(pi, TX2G_PAD_REG0, 0),
		RADIO_REG_20709(pi, TX5G_IPA_REG0, 0)
	};
	BCM_REFERENCE(pi);

	if (!populate) {
		uint16 radioreg_size = 0;
		uint16 phyreg_size = 0;
		p->reg_cache_id = id;
		p->phy_reg_cache_id = id;

		radioreg_size = MAX(ARRAYSIZE(radioregs_txiqlocal_20709_majorrev128),
				ARRAYSIZE(radioregs_rxiqcal_20709_majorrev128));
		radioreg_size = MAX(radioreg_size,
				ARRAYSIZE(radioregs_afecal_20709_majorrev128));
		radioreg_size = MAX(radioreg_size,
				ARRAYSIZE(radioregs_tempsense_vbat_20709_majorrev128));

		phyreg_size = ARRAYSIZE(phyregs_papdcal);
		phyreg_size = MAX(phyreg_size, ARRAYSIZE(phyregs_tempsense_vbat));
		phyreg_size = MAX(phyreg_size, ARRAYSIZE(phyreg_tssi_setup));

		p->phy_reg_cache_depth =  phyreg_size;
		p->reg_cache_depth = radioreg_size;

	} else {
		uint16 *list = NULL;
		uint16 sz = 0;

		switch (id) {
			case RADIOREGS_TXIQCAL:
				/* 6878 */
				list = radioregs_txiqlocal_20709_majorrev128;
				sz   = ARRAYSIZE(radioregs_txiqlocal_20709_majorrev128);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_RXIQCAL:
				list = radioregs_rxiqcal_20709_majorrev128;
				sz   = ARRAYSIZE(radioregs_rxiqcal_20709_majorrev128);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_PAPDCAL:
				if (WBPAPD_ENAB(pi)) {
					list = radioregs_wbpapdcal_20709_majorrev128;
					sz   = ARRAYSIZE(
						radioregs_wbpapdcal_20709_majorrev128);
				} else {
					list = radioregs_papdcal_20709_majorrev128;
					sz   = ARRAYSIZE(
						radioregs_papdcal_20709_majorrev128);
				}
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case PHYREGS_PAPDCAL:
				list = phyregs_papdcal;
				sz   = ARRAYSIZE(phyregs_papdcal);
				base = p->phy_reg_cache; /* to baseline for looping over cores */
				p->phy_reg_sz = sz;
				p->phy_reg_cache_id = id;
				break;
			case PHYREGS_TEMPSENSE_VBAT:
				list = phyregs_tempsense_vbat;
				sz   = ARRAYSIZE(phyregs_tempsense_vbat);
				base = p->phy_reg_cache; /* to baseline for looping over cores */
				p->phy_reg_sz = sz;
				p->phy_reg_cache_id = id;
				break;
			case PHYREGS_TSSI:
				list = phyreg_tssi_setup;
				sz   = ARRAYSIZE(phyreg_tssi_setup);
				base = p->phy_reg_cache; /* to baseline for looping over cores */
				p->phy_reg_sz = sz;
				p->phy_reg_cache_id = id;
				break;
			case RADIOREGS_AFECAL:
				list = radioregs_afecal_20709_majorrev128;
				sz   = ARRAYSIZE(radioregs_afecal_20709_majorrev128);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_TEMPSENSE_VBAT:
				list = radioregs_tempsense_vbat_20709_majorrev128;
				sz   = ARRAYSIZE(radioregs_tempsense_vbat_20709_majorrev128);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_TSSI:
				/* Not needed to store TSSI registers */
				PHY_ERROR(("%s: RADIOREGS_TSSI not supported for 20709\n",
					__FUNCTION__));
				ASSERT(0);
				break;
			default:
				PHY_ERROR(("%s: Invalid ID %d\n", __FUNCTION__, id));
				ASSERT(0);
				break;
		}
		if (list != NULL) {
			uint8 i = 0, core = 0;
			/* update reg cache id in use */
			/* align to base */
			cache = base;
			PHY_INFORM(("***** core %d ***** : %s\n", core, __FUNCTION__));
			for (i = 0; i < sz; i++) {
				cache->addr[core] = list[i];
				cache->addr[core+1] = (list[i] + JTAG_20709_CR1);
				cache->data[core] = 0;
				cache->data[core+1] = 0;
				PHY_INFORM(("0x%8p: 0x%04x\n", cache, cache->addr[core]));
				cache++;
			}
		}
	}
}

static void
phy_ac_reg_cache_parse_20710(phy_info_acphy_t *pi_ac, uint16 id, bool populate)
{
	phy_info_t *pi = pi_ac->pi;
	phy_param_info_t *p = pi_ac->paramsi;
	ad_t *base = p->reg_cache;
	ad_t *cache = base; /* to baseline for looping over cores */

	uint16 phyregs_tempsense_vbat[] = {
		ACPHY_REG(pi, RfctrlOverrideAuxTssi0),
		ACPHY_REG(pi, RfctrlCoreAuxTssi10),
		ACPHY_REG(pi, RfctrlOverrideRxPus0),
		ACPHY_REG(pi, RfctrlCoreRxPus0),
		ACPHY_REG(pi, RxFeCtrl1),
		ACPHY_REG(pi, RfctrlOverrideTxPus0),
		ACPHY_REG(pi, RfctrlCoreTxPus0),
		ACPHY_REG(pi, RfctrlOverrideGains0),
		ACPHY_REG(pi, RfctrlCoreLpfGain0),
		ACPHY_REG(pi, RxSdFeConfig1),
		ACPHY_REG(pi, RxSdFeConfig6),
		ACPHY_REG(pi, RfctrlOverrideLpfSwtch0),
		ACPHY_REG(pi, RfctrlCoreLpfSwtch0),
		ACPHY_REG(pi, RfctrlOverrideAfeCfg0),
		ACPHY_REG(pi, RfctrlCoreAfeCfg10),
		ACPHY_REG(pi, gpioLoOutEn),
		ACPHY_REG(pi, gpioHiOutEn),
		ACPHY_REG(pi, gpioSel),
		ACPHY_REG(pi, RfctrlCoreAfeCfg20)
	};

	uint16 phyregs_papdcal[] = {
		ACPHY_REG(pi, RfctrlOverrideTxPus0),
		ACPHY_REG(pi, RfctrlOverrideRxPus0),
		ACPHY_REG(pi, RfctrlOverrideGains0),
		ACPHY_REG(pi, RfctrlOverrideLpfCT0),
		ACPHY_REG(pi, RfctrlOverrideLpfSwtch0),
		ACPHY_REG(pi, RfctrlOverrideAfeCfg0),
		ACPHY_REG(pi, RfctrlOverrideLowPwrCfg0),
		ACPHY_REG(pi, RfctrlOverrideAuxTssi0),
		ACPHY_REG(pi, RfctrlCoreTxPus0),
		ACPHY_REG(pi, RfctrlCoreRxPus0),
		ACPHY_REG(pi, RfctrlCoreTXGAIN10),
		ACPHY_REG(pi, RfctrlCoreTXGAIN20),
		ACPHY_REG(pi, RfctrlCoreRXGAIN10),
		ACPHY_REG(pi, RfctrlCoreRXGAIN20),
		ACPHY_REG(pi, RfctrlCoreLpfGain0),
		ACPHY_REG(pi, RfctrlCoreLpfCT0),
		ACPHY_REG(pi, RfctrlCoreLpfGmult0),
		ACPHY_REG(pi, RfctrlCoreRCDACBuf0),
		ACPHY_REG(pi, RfctrlCoreLpfSwtch0),
		ACPHY_REG(pi, RfctrlCoreAfeCfg10),
		ACPHY_REG(pi, RfctrlCoreAfeCfg20),
		ACPHY_REG(pi, RfctrlCoreLowPwr0),
		ACPHY_REG(pi, RfctrlCoreAuxTssi10),
		ACPHY_REG(pi, RfctrlCoreAuxTssi20),
		ACPHY_REG(pi, Dac_gain0),
		ACPHY_REG(pi, RfctrlIntc0),
		ACPHY_REG(pi, forceFront0),
		ACPHY_REG(pi, dccal_control_290),
		ACPHY_REG(pi, dccal_control_500),
		ACPHY_REG(pi, PapdEnable0)
	};

	uint16 phyreg_tssi_setup[] = {
		ACPHY_REG(pi, RfctrlOverrideAuxTssi0),
		ACPHY_REG(pi, RfctrlCoreAuxTssi10),
		ACPHY_REG(pi, RfctrlIntc0),
		ACPHY_REG(pi, ClassifierCtrl),
		ACPHY_REG(pi, RxFeCtrl1),
		ACPHY_REG(pi, AfePuCtrl)
	};

	/* BCM6756 lists for save & restore functions */
	uint16 radioregs_tempsense_vbat_20710_majorrev131[] = {
		RADIO_REG_20710(pi, TEMPSENSE_OVR1, 0),
		RADIO_REG_20710(pi, TEMPSENSE_CFG, 0),
		RADIO_REG_20710(pi, TESTBUF_OVR1, 0),
		RADIO_REG_20710(pi, TESTBUF_CFG1, 0),
		RADIO_REG_20710(pi, AUXPGA_OVR1, 0),
		RADIO_REG_20710(pi, AUXPGA_CFG1, 0),
		RADIO_REG_20710(pi, AUXPGA_VMID, 0),
		RADIO_REG_20710(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20710(pi, TIA_REG7, 0),
		RADIO_REG_20710(pi, LPF_OVR1, 0),
		RADIO_REG_20710(pi, LPF_OVR2, 0),
		RADIO_REG_20710(pi, LPF_REG7, 0),
		RADIO_REG_20710(pi, IQCAL_CFG4, 0),
		RADIO_REG_20710(pi, IQCAL_OVR1, 0),
		RADIO_REG_20710(pi, IQCAL_CFG5, 0)
	};

	uint16 radioregs_txiqlocal_20710_majorrev131[] = {
		RADIO_REG_20710(pi, IQCAL_OVR1, 0),
		RADIO_REG_20710(pi, IQCAL_CFG1, 0),
		RADIO_REG_20710(pi, IQCAL_CFG4, 0),
		RADIO_REG_20710(pi, IQCAL_CFG5, 0),
		RADIO_REG_20710(pi, IQCAL_GAIN_RIN, 0),
		RADIO_REG_20710(pi, IQCAL_GAIN_RFB, 0),
		RADIO_REG_20710(pi, LPF_OVR1, 0),
		RADIO_REG_20710(pi, LPF_OVR2, 0),
		RADIO_REG_20710(pi, LPF_REG7, 0),
		RADIO_REG_20710(pi, TX5G_MIX_REG0, 0),
		RADIO_REG_20710(pi, TX5G_CFG2_OVR, 0),
		RADIO_REG_20710(pi, TX5G_MISC_CFG1, 0),
		RADIO_REG_20710(pi, AUXPGA_OVR1, 0),
		RADIO_REG_20710(pi, AUXPGA_CFG1, 0),
		RADIO_REG_20710(pi, TESTBUF_OVR1, 0),
		RADIO_REG_20710(pi, TESTBUF_CFG1, 0),
		RADIO_REG_20710(pi, TXDAC_REG0, 0),
		RADIO_REG_20710(pi, TXDAC_REG1, 0),
		RADIO_REG_20710(pi, TXDAC_REG3, 0),
		RADIO_REG_20710(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20710(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20710(pi, TXDB_PAD_REG3, 0)
	};

	uint16 radioregs_rxiqcal_20710_majorrev131[] = {
		RADIO_REG_20710(pi, LPF_OVR1, 0),
		RADIO_REG_20710(pi, LPF_OVR2, 0),
		RADIO_REG_20710(pi, LPF_REG7, 0),
		RADIO_REG_20710(pi, TIA_REG7, 0),
		RADIO_REG_20710(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20710(pi, RXDB_CFG1_OVR, 0),
		RADIO_REG_20710(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20710(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20710(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20710(pi, TX2G_PA_REG0, 0),
		RADIO_REG_20710(pi, RX5G_REG4, 0),
		RADIO_REG_20710(pi, RX5G_REG2, 0),
		RADIO_REG_20710(pi, RX5G_REG6, 0),
		RADIO_REG_20710(pi, RX5G_REG1, 0),
		RADIO_REG_20710(pi, RX2G_REG4, 0),
		RADIO_REG_20710(pi, TXDAC_REG0, 0),
		RADIO_REG_20710(pi, TXDAC_REG1, 0),
		RADIO_REG_20710(pi, TXDAC_REG3, 0),
		RADIO_REG_20710(pi, LPF_REG6, 0),
		RADIO_REG_20710(pi, TXDB_CFG1_OVR, 0),
		RADIO_REG_20710(pi, TX2G_MIX_REG0, 0),
		RADIO_REG_20710(pi, TX2G_CFG1_OVR, 0),
		RADIO_REG_20710(pi, TXDB_REG0, 0),
		RADIO_REG_20710(pi, TIA_CFG1_OVR, 0)
	};

	uint16 radioregs_afecal_20710_majorrev131[] = {
		RADIO_REG_20710(pi, AFE_CFG1_OVR2, 0),
		RADIO_REG_20710(pi, TXDAC_REG0, 0),
		RADIO_REG_20710(pi, LPF_OVR1, 0),
		RADIO_REG_20710(pi, LPF_OVR2, 0)
	};

	uint16 radioregs_papdcal_20710_majorrev131[] = {
		RADIO_REG_20710(pi, LPF_OVR1, 0),
		RADIO_REG_20710(pi, LPF_OVR2, 0),
		RADIO_REG_20710(pi, LPF_REG7, 0),
		RADIO_REG_20710(pi, TIA_REG7, 0),
		RADIO_REG_20710(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20710(pi, RXDB_CFG1_OVR, 0),
		RADIO_REG_20710(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20710(pi, RX5G_REG1, 0),
		RADIO_REG_20710(pi, RX5G_REG2, 0),
		RADIO_REG_20710(pi, RX2G_REG4, 0),
		RADIO_REG_20710(pi, RX5G_REG3, 0),
		RADIO_REG_20710(pi, RX5G_REG4, 0),
		RADIO_REG_20710(pi, RX5G_REG5, 0),
		RADIO_REG_20710(pi, RX5G_REG6, 0),
		RADIO_REG_20710(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20710(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20710(pi, TX2G_PA_REG0, 0),
		RADIO_REG_20710(pi, TXDAC_REG3, 0),
		RADIO_REG_20710(pi, RXADC_CFG0, 0)
	};

	uint16 radioregs_wbpapdcal_20710_majorrev131[] = {
		RADIO_REG_20710(pi, LPF_OVR1, 0),
		RADIO_REG_20710(pi, LPF_OVR2, 0),
		RADIO_REG_20710(pi, LPF_REG1, 0),
		RADIO_REG_20710(pi, LPF_REG2, 0),
		RADIO_REG_20710(pi, LPF_REG3, 0),
		RADIO_REG_20710(pi, LPF_REG4, 0),
		RADIO_REG_20710(pi, LPF_REG6, 0),
		RADIO_REG_20710(pi, LPF_REG7, 0),
		RADIO_REG_20710(pi, LPF_REG11, 0),
		RADIO_REG_20710(pi, TIA_REG1, 0),
		RADIO_REG_20710(pi, TIA_REG2, 0),
		RADIO_REG_20710(pi, TIA_REG3, 0),
		RADIO_REG_20710(pi, TIA_REG4, 0),
		RADIO_REG_20710(pi, TIA_REG5, 0),
		RADIO_REG_20710(pi, TIA_REG6, 0),
		RADIO_REG_20710(pi, TIA_REG7, 0),
		RADIO_REG_20710(pi, TIA_CFG1_OVR, 0),
		RADIO_REG_20710(pi, RXDB_CFG1_OVR, 0),
		RADIO_REG_20710(pi, RX2G_CFG1_OVR, 0),
		RADIO_REG_20710(pi, RX5G_REG1, 0),
		RADIO_REG_20710(pi, RX5G_REG2, 0),
		RADIO_REG_20710(pi, RX2G_REG4, 0),
		RADIO_REG_20710(pi, RX5G_REG3, 0),
		RADIO_REG_20710(pi, RX5G_REG4, 0),
		RADIO_REG_20710(pi, RX5G_REG5, 0),
		RADIO_REG_20710(pi, RX5G_REG6, 0),
		RADIO_REG_20710(pi, LOGEN_CORE_OVR0, 0),
		RADIO_REG_20710(pi, LOGEN_CORE_REG0, 0),
		RADIO_REG_20710(pi, TX2G_PA_REG0, 0),
		RADIO_REG_20710(pi, TXDAC_REG3, 0),
		RADIO_REG_20710(pi, RXADC_CFG0, 0),
		RADIO_REG_20710(pi, LPF_NOTCH_OVR1, 0),
		RADIO_REG_20710(pi, LPF_NOTCH_REG2, 0),
		RADIO_REG_20710(pi, LPF_NOTCH_REG3, 0),
		RADIO_REG_20710(pi, LPF_NOTCH_REG4, 0),
		RADIO_REG_20710(pi, LPF_NOTCH_REG5, 0)
	};
	BCM_REFERENCE(pi);

	if (!populate) {
		uint16 radioreg_size = 0;
		uint16 phyreg_size = 0;
		p->reg_cache_id = id;
		p->phy_reg_cache_id = id;

		radioreg_size = MAX(ARRAYSIZE(radioregs_txiqlocal_20710_majorrev131),
				ARRAYSIZE(radioregs_rxiqcal_20710_majorrev131));
		radioreg_size = MAX(radioreg_size,
				ARRAYSIZE(radioregs_afecal_20710_majorrev131));
		radioreg_size = MAX(radioreg_size,
				ARRAYSIZE(radioregs_tempsense_vbat_20710_majorrev131));
		if (WBPAPD_ENAB(pi)) {
			radioreg_size = MAX(radioreg_size,
				ARRAYSIZE(radioregs_wbpapdcal_20710_majorrev131));
		} else {
			radioreg_size = MAX(radioreg_size,
				ARRAYSIZE(radioregs_papdcal_20710_majorrev131));
		}

		phyreg_size = ARRAYSIZE(phyregs_papdcal);
		phyreg_size = MAX(phyreg_size, ARRAYSIZE(phyregs_tempsense_vbat));
		phyreg_size = MAX(phyreg_size, ARRAYSIZE(phyreg_tssi_setup));

		p->phy_reg_cache_depth =  phyreg_size;
		p->reg_cache_depth = radioreg_size;

	} else {
		uint16 *list = NULL;
		uint16 sz = 0;

		switch (id) {
			case RADIOREGS_TXIQCAL:
				/* 6756 */
				list = radioregs_txiqlocal_20710_majorrev131;
				sz   = ARRAYSIZE(radioregs_txiqlocal_20710_majorrev131);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_RXIQCAL:
				list = radioregs_rxiqcal_20710_majorrev131;
				sz   = ARRAYSIZE(radioregs_rxiqcal_20710_majorrev131);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_PAPDCAL:
				if (WBPAPD_ENAB(pi)) {
					list = radioregs_wbpapdcal_20710_majorrev131;
					sz   = ARRAYSIZE(
						radioregs_wbpapdcal_20710_majorrev131);
				} else {
					list = radioregs_papdcal_20710_majorrev131;
					sz   = ARRAYSIZE(
						radioregs_papdcal_20710_majorrev131);
				}
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case PHYREGS_PAPDCAL:
				list = phyregs_papdcal;
				sz   = ARRAYSIZE(phyregs_papdcal);
				base = p->phy_reg_cache; /* to baseline for looping over cores */
				p->phy_reg_sz = sz;
				p->phy_reg_cache_id = id;
				break;
			case PHYREGS_TEMPSENSE_VBAT:
				list = phyregs_tempsense_vbat;
				sz   = ARRAYSIZE(phyregs_tempsense_vbat);
				base = p->phy_reg_cache; /* to baseline for looping over cores */
				p->phy_reg_sz = sz;
				p->phy_reg_cache_id = id;
				break;
			case PHYREGS_TSSI:
				list = phyreg_tssi_setup;
				sz   = ARRAYSIZE(phyreg_tssi_setup);
				base = p->phy_reg_cache; /* to baseline for looping over cores */
				p->phy_reg_sz = sz;
				p->phy_reg_cache_id = id;
				break;
			case RADIOREGS_AFECAL:
				list = radioregs_afecal_20710_majorrev131;
				sz   = ARRAYSIZE(radioregs_afecal_20710_majorrev131);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_TEMPSENSE_VBAT:
				list = radioregs_tempsense_vbat_20710_majorrev131;
				sz   = ARRAYSIZE(radioregs_tempsense_vbat_20710_majorrev131);
				base = p->reg_cache; /* to baseline for looping over cores */
				p->reg_sz = sz;
				p->reg_cache_id = id;
				break;
			case RADIOREGS_TSSI:
				/* Not needed to store TSSI registers */
				PHY_ERROR(("%s: RADIOREGS_TSSI not supported for 20710\n",
					__FUNCTION__));
				ASSERT(0);
				break;
			default:
				PHY_ERROR(("%s: Invalid ID %d\n", __FUNCTION__, id));
				ASSERT(0);
				break;
		}
		if (list != NULL) {
			uint8 i = 0, core = 0;
			/* update reg cache id in use */
			/* align to base */
			cache = base;
			PHY_INFORM(("***** core %d ***** : %s\n", core, __FUNCTION__));
			switch (id) {
				case RADIOREGS_TXIQCAL:
				case RADIOREGS_RXIQCAL:
				case RADIOREGS_PAPDCAL:
				case RADIOREGS_AFECAL:
				case RADIOREGS_TEMPSENSE_VBAT:
				case RADIOREGS_TSSI:
					ASSERT(p->reg_sz <= p->reg_cache_depth);
					break;
				case PHYREGS_PAPDCAL:
				case PHYREGS_TEMPSENSE_VBAT:
				case PHYREGS_TSSI:
					ASSERT(p->phy_reg_sz <= p->phy_reg_cache_depth);
					break;
				default:
					ASSERT(0);
			}
			for (i = 0; i < sz; i++) {
				cache->addr[core] = list[i];
				cache->addr[core+1] = (list[i] + JTAG_20710_CR1);
				cache->data[core] = 0;
				cache->data[core+1] = 0;
				PHY_INFORM(("0x%8p: 0x%04x\n", cache, cache->addr[core]));
				cache++;
			}
		}
	}
}

static void
phy_ac_reg_cache_setup(phy_info_acphy_t *pi_ac, uint16 id)
{
	phy_info_t *pi = pi_ac->pi;
	BCM_REFERENCE(pi);
	if (ACMAJORREV_130(pi->pubpi->phy_rev))
		phy_ac_reg_cache_parse_20708(pi_ac, id, TRUE);
	else if (ACMAJORREV_129(pi->pubpi->phy_rev))
		phy_ac_reg_cache_parse_20707(pi_ac, id, TRUE);
	else if (ACMAJORREV_128(pi->pubpi->phy_rev))
		phy_ac_reg_cache_parse_20709(pi_ac, id, TRUE);
	else if (ACMAJORREV_131(pi->pubpi->phy_rev))
		phy_ac_reg_cache_parse_20710(pi_ac, id, TRUE);
	else
		phy_ac_reg_cache_parse(pi_ac, id, TRUE);
}

static void
phy_ac_upd_reg_cache_depth(phy_info_acphy_t *pi_ac)
{
	phy_info_t *pi = pi_ac->pi;
	BCM_REFERENCE(pi);
	if (ACMAJORREV_130(pi->pubpi->phy_rev))
		phy_ac_reg_cache_parse_20708(pi_ac, 0, FALSE);
	else if (ACMAJORREV_129(pi->pubpi->phy_rev))
		phy_ac_reg_cache_parse_20707(pi_ac, 0, FALSE);
	else if (ACMAJORREV_128(pi->pubpi->phy_rev))
		phy_ac_reg_cache_parse_20709(pi_ac, 0, FALSE);
	else if (ACMAJORREV_131(pi->pubpi->phy_rev))
		phy_ac_reg_cache_parse_20710(pi_ac, 0, FALSE);
	else
		phy_ac_reg_cache_parse(pi_ac, 0, FALSE);
}

void
BCMATTACHFN(phy_ac_reg_cache_detach)(phy_info_acphy_t *pi_ac)
{
	phy_info_t *pi = pi_ac->pi;
	phy_param_info_t *p = pi_ac->paramsi;

	if (p) {
		if (p->phy_reg_cache)
			phy_mfree(pi, p->phy_reg_cache, p->phy_reg_cache_depth * sizeof(ad_t));
		if (p->reg_cache)
			phy_mfree(pi, p->reg_cache, p->reg_cache_depth * sizeof(ad_t));
	}
}

bool
BCMATTACHFN(phy_ac_reg_cache_attach)(phy_info_acphy_t *pi_ac)
{
	phy_info_t *pi = pi_ac->pi;
	phy_param_info_t *p = pi_ac->paramsi;

	/* update reg cache depth */
	phy_ac_upd_reg_cache_depth(pi_ac);

	if ((p->phy_reg_cache = phy_malloc(pi, p->phy_reg_cache_depth * sizeof(ad_t))) == NULL) {
		PHY_ERROR(("%s: phy phy_malloc ad_t failed\n", __FUNCTION__));
		goto fail;
	}

	if ((p->reg_cache = phy_malloc(pi, p->reg_cache_depth * sizeof(ad_t))) == NULL) {
		PHY_ERROR(("%s: radio phy_malloc ad_t failed\n", __FUNCTION__));
		goto fail;
	}

	return TRUE;
fail:
	phy_ac_reg_cache_detach(pi_ac);
	return FALSE;
}

int
phy_ac_reg_cache_save(phy_info_acphy_t *pi_ac, uint16 id)
{
	phy_ac_reg_cache_setup(pi_ac, id);

	return phy_ac_reg_cache_process(pi_ac, id, SAVE);
}

int
phy_ac_reg_cache_restore(phy_info_acphy_t *pi_ac, int id)
{
	return phy_ac_reg_cache_process(pi_ac, id, RESTORE);
}

int
phy_ac_reg_cache_save_percore(phy_info_acphy_t *pi_ac, uint16 id, uint8 core)
{
	phy_ac_reg_cache_setup(pi_ac, id);

	return phy_ac_reg_cache_process_percore(pi_ac, id, SAVE, core);
}

int
phy_ac_reg_cache_restore_percore(phy_info_acphy_t *pi_ac, int id, uint8 core)
{
	return phy_ac_reg_cache_process_percore(pi_ac, id, RESTORE, core);
}

#if defined(PHYCAL_CACHING)
static void
wlc_phy_cal_cache_acphy(phy_type_cache_ctx_t * cache_ctx)
{
	phy_info_t *pi = (phy_info_t *) cache_ctx;
	ch_calcache_t *ctx = NULL;

	ctx = wlc_phy_get_chanctx(pi, pi->radio_chanspec);

#ifndef WLOLPC
	/* A context must have been created before reaching here */
	ASSERT(ctx != NULL);
#endif /* WLOLPC */

	if (ctx == NULL) {
		PHY_ERROR(("wl%d: %s call with null ctx\n",
			pi->sh->unit, __FUNCTION__));
		return;
	}

	ctx->valid = TRUE;

	/* save the callibration to cache */
	phy_ac_txiqlocal_save_cache(pi->u.pi_acphy->txiqlocali, ctx, 0);
	phy_ac_rxiqcal_save_cache(pi->u.pi_acphy->rxiqcali, ctx);
	phy_ac_tssical_idle_save_cache(pi->u.pi_acphy->tssicali, ctx);
	if (PHY_PAPDEN(pi)) {
		phy_ac_papdcal_save_cache(pi->u.pi_acphy->papdcali, ctx);
	}
	phy_ac_tpc_save_cache(pi->u.pi_acphy->tpci, ctx);
}

#if defined(BCMDBG)
void
wlc_phy_cal_cache_dbg_acphy(phy_info_t *pi, ch_calcache_t *ctx)
{
	uint8 i, multilo_cal_cnt;
	uint16 j, eps_table_size;
	uint8 corenum = (uint8) PHYCORENUM(pi->pubpi->phy_corenum);
	uint8 disable_table_extension = 0;
	acphy_calcache_t *cache = &ctx->u.acphy_cache;
	BCM_REFERENCE(pi);

	FOREACH_CORE(pi, i) {
		PHY_CAL(("CORE %d:\n", i));
		if (phy_txiqlocal_num_multilo(pi) != 0) {
			PHY_CAL(("\tofdm_txa:0x%x  ofdm_txb:0x%x  ",
				cache->ofdm_txa[i], cache->ofdm_txb[i]));
			PHY_CAL(("ofdm_txd:"));
			for (multilo_cal_cnt = 1;
				multilo_cal_cnt <= phy_txiqlocal_num_multilo(pi);
				multilo_cal_cnt++) {
				PHY_CAL(("0x%x  ",
					cache->ofdm_txd[i+(multilo_cal_cnt-1)*corenum]));
			}
			PHY_CAL(("\n"));
		} else {
			PHY_CAL(("\tofdm_txa:0x%x  ofdm_txb:0x%x  ofdm_txd:0x%x\n",
				cache->ofdm_txa[i], cache->ofdm_txb[i], cache->ofdm_txd[i]));
		}
		PHY_CAL(("\tbphy_txa:0x%x  bphy_txb:0x%x  bphy_txd:0x%x\n",
			cache->bphy_txa[i], cache->bphy_txb[i], cache->bphy_txd[i]));
		PHY_CAL(("\ttxei:0x%x  txeq:0x%x\n", cache->txei[i], cache->txeq[i]));
		PHY_CAL(("\ttxfi:0x%x  txfq:0x%x\n", cache->txfi[i], cache->txfq[i]));
#ifdef WLC_TXFDIQ
		PHY_CAL(("\ttxs:0x%x\n", cache->txs[i]));
#endif
		PHY_CAL(("\trxa:0x%x  rxb:0x%x\n", cache->rxa[i], cache->rxb[i]));
		PHY_CAL(("\trxs:0x%x  rxe:0x%x\n", cache->rxs[i], cache->rxe));
		PHY_CAL(("\tidletssi:0x%x\n", cache->idle_tssi[i]));
		PHY_CAL(("\tbasedindex:0x%x\n", cache->baseidx[i]));
		eps_table_size = phy_ac_papdcal_eps_table_size(pi, i);
		if (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)) {
			disable_table_extension = READ_PHYREGFLDCE(pi, papdEpsilonTable, i,
				epsilon_tbl_extend_dis);
			if (!disable_table_extension) {
				// Even and odd entries are in duplicate. Only one of them is cached
				eps_table_size >>= 1;
			}
		}
		if (PHY_PAPDEN(pi)) {
			PHY_CAL(("\tPAPD eps table\n"));
			for (j = 0; j < eps_table_size; j += 16) {
				PHY_CAL(("\t%d : 0x%x\n", j,
					cache->papd_eps[(i*eps_table_size)+j]));
			}
			PHY_CAL(("\tPAPD rfpwrlut\n"));
			for (j = 0; j < ACPHY_PAPD_RFPWRLUT_TBL_SIZE; j += 32) {
				PHY_CAL(("\t%d : 0x%x\n",
					j,
					cache->eps_offset_cache[(i*
					ACPHY_PAPD_RFPWRLUT_TBL_SIZE)+j]));
			}
		}
	}
}
#endif /* BCMDBG */
#endif /* PHYCAL_CACHING */

#if defined BCMDBG || defined(WLTEST)
static void
wlc_phydump_cal_cache_acphy(phy_type_cache_ctx_t * cache_ctx, ch_calcache_t *ctx,
	struct bcmstrbuf *b)
{
	phy_info_t *pi = (phy_info_t *)cache_ctx;
	uint8 i, multilo_cal_cnt;
	uint8 corenum = (uint8) PHYCORENUM(pi->pubpi->phy_corenum);
	acphy_calcache_t *cache = &ctx->u.acphy_cache;
	BCM_REFERENCE(pi);
	FOREACH_CORE(pi, i) {
		bcm_bprintf(b, "CORE %d:\n", i);
		if (phy_txiqlocal_num_multilo(pi) != 0) {
			bcm_bprintf(b, "\tofdm_txa:0x%x ofdm_txb:0x%x  ",
				cache->ofdm_txa[i], cache->ofdm_txb[i]);
			bcm_bprintf(b, "ofdm_txd:");
			for (multilo_cal_cnt = 1;
				multilo_cal_cnt <= phy_txiqlocal_num_multilo(pi);
				multilo_cal_cnt++) {
				bcm_bprintf(b, "0x%x  ",
				    cache->ofdm_txd[i+(multilo_cal_cnt-1)*corenum]);
			}
			bcm_bprintf(b, "\n");
		} else {
			bcm_bprintf(b, "\tofdm_txa:0x%x ofdm_txb:0x%x ofdm_txd:0x%x\n",
				cache->ofdm_txa[i], cache->ofdm_txb[i], cache->ofdm_txd[i]);
		}
		bcm_bprintf(b, "\tbphy_txa:0x%x  bphy_txb:0x%x  bphy_txd:0x%x\n",
			cache->bphy_txa[i], cache->bphy_txb[i], cache->bphy_txd[i]);
		bcm_bprintf(b, "\ttxei:0x%x  txeq:0x%x\n", cache->txei[i], cache->txeq[i]);
		bcm_bprintf(b, "\ttxfi:0x%x  txfq:0x%x\n", cache->txfi[i], cache->txfq[i]);
		bcm_bprintf(b, "\trxa:0x%x  rxb:0x%x\n", cache->rxa[i], cache->rxb[i]);
		bcm_bprintf(b, "\tidletssi:0x%x\n", cache->idle_tssi[i]);
		bcm_bprintf(b, "\tbasedindex:0x%x\n", cache->baseidx[i]);
	}
}
#endif /* defined(WLTEST) */

#ifdef PHYCAL_CACHING
static int
wlc_phy_cal_cache_restore_acphy(phy_type_cache_ctx_t * cache_ctx)
{
	phy_info_t *pi = (phy_info_t *)cache_ctx;
	ch_calcache_t *ctx;
	acphy_calcache_t *cache = NULL;
	acphy_ram_calcache_t *ctx_ac;
	bool suspend;
	uint8 core;
	uint16 eps_table_size;
	uint16 *epstbl_offset_cache;
	uint32 *epsilon_cache;
	uint8 num_multi, cnt;
	uint8 corenum = (uint8) PHYCORENUM(pi->pubpi->phy_corenum);
	uint8 disable_table_extension = 0;
	txiqlocal_multilo_t multilo_cal;
	uint32 epsilon_table_ids[] =
		{ACPHY_TBL_ID_EPSILON0, ACPHY_TBL_ID_EPSILON1, ACPHY_TBL_ID_EPSILON2,
			ACPHY_TBL_ID_EPSILON3};
	uint32 rfpwrlut_table_ids[] =
		{ACPHY_TBL_ID_RFPWRLUTS0, ACPHY_TBL_ID_RFPWRLUTS1, ACPHY_TBL_ID_RFPWRLUTS2,
			ACPHY_TBL_ID_RFPWRLUTS3};

	ctx = wlc_phy_get_chanctx(pi, pi->radio_chanspec);

	if (!ctx) {
		PHY_CAL(("wl%d: %s: Chanspec 0x%x not found in calibration cache\n",
			pi->sh->unit, __FUNCTION__, pi->radio_chanspec));
		return BCME_ERROR;
	}

	if (!ctx->valid) {
		PHY_CAL(("wl%d: %s: Chanspec 0x%x found, but not valid in phycal cache\n",
			pi->sh->unit, __FUNCTION__, pi->radio_chanspec));
		return BCME_ERROR;
	}

	PHY_CAL(("wl%d: %s: Restoring all cal coeffs from calibration cache for chanspec 0x%x\n",
		pi->sh->unit, __FUNCTION__, pi->radio_chanspec));

	cache = &ctx->u.acphy_cache;
	ctx_ac = ctx->u_ram_cache.acphy_ram_calcache;

	epsilon_cache = cache->papd_eps;
	epstbl_offset_cache = cache->eps_offset_cache;

	suspend = !(R_REG(pi->sh->osh, D11_MACCONTROL(pi)) & MCTL_EN_MAC);
	if (!suspend) {
		/* suspend mac */
		wlapi_suspend_mac_and_wait(pi->sh->physhim);
	}
	phy_utils_phyreg_enter(pi);

	num_multi = phy_txiqlocal_num_multilo(pi);

	/* restore the txcal from cache */
	FOREACH_CORE(pi, core) {
		uint16 ab_int[2];
		eps_table_size = phy_ac_papdcal_eps_table_size(pi, core);
		/* Restore OFDM Tx IQ Imb Coeffs A,B and Digital Loft Comp Coeffs */
		ab_int[0] = cache->ofdm_txa[core];
		ab_int[1] = cache->ofdm_txb[core];
		wlc_phy_cal_txiqlo_coeffs_acphy(pi, CAL_COEFF_WRITE,
		                                ab_int, TB_OFDM_COEFFS_AB, core);
		wlc_phy_cal_txiqlo_coeffs_acphy(pi, CAL_COEFF_WRITE,
		                                &cache->ofdm_txd[core], TB_OFDM_COEFFS_D, core);
		if (num_multi != 0) {
			if (ACMAJORREV_47_129_130(pi->pubpi->phy_rev)) {
				for (cnt = 1; cnt <= num_multi; cnt++) {
					wlc_phy_txiqlocal_lopwr_gettblidx(pi, &multilo_cal, cnt);
					wlc_phy_populate_tx_loftcoefluts_acphy(pi, core,
						cache->ofdm_txd[core+corenum*(cnt-1)],
						multilo_cal.lofttbl_start_idx,
						multilo_cal.lofttbl_end_idx);
				}
			}
		}

		/* Restore BPHY Tx IQ Imb Coeffs A,B and Digital Loft Comp Coeffs */
		ab_int[0] = cache->bphy_txa[core];
		ab_int[1] = cache->bphy_txb[core];
		wlc_phy_cal_txiqlo_coeffs_acphy(pi, CAL_COEFF_WRITE,
		                                ab_int, TB_BPHY_COEFFS_AB, core);
		wlc_phy_cal_txiqlo_coeffs_acphy(pi, CAL_COEFF_WRITE,
		                                &cache->bphy_txd[core], TB_BPHY_COEFFS_D, core);

		if ((!TINY_RADIO(pi)) && (!IS_28NM_RADIO(pi)) && (!IS_16NM_RADIO(pi))) {
			/* Restore Analog Tx Loft Comp Coeffs */
			phy_utils_write_radioreg(pi, RF_2069_TXGM_LOFT_FINE_I(core),
			                         cache->txei[core]);
			phy_utils_write_radioreg(pi, RF_2069_TXGM_LOFT_FINE_Q(core),
			                         cache->txeq[core]);
			phy_utils_write_radioreg(pi, RF_2069_TXGM_LOFT_COARSE_I(core),
			                         cache->txfi[core]);
			phy_utils_write_radioreg(pi, RF_2069_TXGM_LOFT_COARSE_Q(core),
			                         cache->txfq[core]);
		}

#ifdef WLC_TXFDIQ
		phy_ac_txiqlocal_set_fdiqi_slope(pi->u.pi_acphy->txiqlocali, core,
			cache->txs[core]);
		if (phy_ac_txiqlocal_is_fdiqi_enabled(pi->u.pi_acphy->txiqlocali)) {
		  wlc_phy_tx_fdiqi_comp_acphy(pi, TRUE, 0xFF);
		}
#endif
		/* Restore base index */
		MOD_PHYREGCEE(pi, TxPwrCtrlInit_path, core,
		              pwrIndex_init_path, cache->baseidx[core]);

		/* Restore Idle TSSI & Vmid values */
		wlc_phy_txpwrctrl_set_idle_tssi_acphy(pi, cache->idle_tssi[core], core);
		/* Restore PAPD epsilon offsets */
		if (PHY_PAPDEN(pi)) {
			MOD_PHYREGCEE(pi, EpsilonTableAdjust, core,
				epsilonOffset, ctx_ac->epsilon_offset[core]);
			MOD_PHYREGCEE(pi, PapdEnable, core,
				papd_compEnb, ctx_ac->papd_comp_en[core]);

			if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
				!ACMAJORREV_128(pi->pubpi->phy_rev)) {
				// Devices with duplicate even and odd entries.
				disable_table_extension = READ_PHYREGFLDCE(pi, papdEpsilonTable,
					core, epsilon_tbl_extend_dis);
			}
			if (ACMAJORREV_GE47(pi->pubpi->phy_rev) &&
				!ACMAJORREV_128(pi->pubpi->phy_rev) && !disable_table_extension) {
				// The way to write duplicate value into even and odd entries.
				MOD_PHYREGCE(pi, papdEpsilonTable, core, epsilon_tbl_extend_dis, 1);
				wlc_phy_table_write_acphy_dac_war(pi, epsilon_table_ids[core],
					eps_table_size>>1, 0, 32, epsilon_cache, core);
				epsilon_cache += (eps_table_size>>1);
				MOD_PHYREGCE(pi, papdEpsilonTable, core, epsilon_tbl_extend_dis, 0);
			} else {

				wlc_phy_table_write_acphy_dac_war(pi, epsilon_table_ids[core],
					eps_table_size,	0, 32, epsilon_cache, core);
				epsilon_cache += eps_table_size;
			}
			if (!ACMAJORREV_129_130(pi->pubpi->phy_rev)) {
				wlc_phy_table_write_acphy(pi, rfpwrlut_table_ids[core],
					ACPHY_PAPD_RFPWRLUT_TBL_SIZE, 0, 16, epstbl_offset_cache);
				epstbl_offset_cache += ACPHY_PAPD_RFPWRLUT_TBL_SIZE;
			}
		}
	}

	// Restore Rxiqcal coeffs
	phy_ac_rxiqcal_restore_cache(pi->u.pi_acphy->rxiqcali, ctx);

	phy_utils_phyreg_exit(pi);

	/* unsuspend mac */
	if (!suspend) {
		wlapi_enable_mac(pi->sh->physhim);
	}

	phy_ac_rxcgrs_restore_force_crsmin(pi);

#ifdef BCMDBG
	PHY_CAL(("wl%d: %s: Restored values for chanspec 0x%x are:\n", pi->sh->unit,
	           __FUNCTION__, pi->radio_chanspec));
	wlc_phy_cal_cache_dbg_acphy(pi, ctx);
#endif
	return BCME_OK;
}
#endif /* PHYCAL_CACHING */

#if defined(BCMDBG) || defined(WLTEST)
/* dump calibration regs/info */
static void
wlc_phy_cal_dump_acphy(phy_type_cache_ctx_t * cache_ctx, struct bcmstrbuf *b)
{
	phy_info_t *pi = (phy_info_t *)cache_ctx;
	uint8 core;
	uint8  ac_reg, mf_reg;
	int8 lesiInpSc[8] = {0};
	int16  a_reg, b_reg, a_int, b_int;
	int32 slope;
	uint16 ab_int[2], d_reg;
	uint16 coremask;
	phy_stf_data_t *stf_shdata = phy_stf_get_data(pi->stfi);
	uint8 multilo_cal_cnt;
	txiqlocal_multilo_t multilo_cal = {0, 0};

	if (!pi->sh->up) {
		return;
	}
	BCM_REFERENCE(coremask);
	wlapi_suspend_mac_and_wait(pi->sh->physhim);
	phy_utils_phyreg_enter(pi);

	if (ACMAJORREV_4(pi->pubpi->phy_rev) && (phy_get_phymode(pi) != PHYMODE_RSDB)) {
		coremask = stf_shdata->hw_phyrxchain;
	} else {
		coremask = stf_shdata->phyrxchain;
	}

	bcm_bprintf(b, "Tx-IQ/LOFT-Cal:\n");
	FOREACH_ACTV_CORE(pi, stf_shdata->phytxchain, core) {
		wlc_phy_cal_txiqlo_coeffs_acphy(pi, CAL_COEFF_READ, ab_int,
			TB_OFDM_COEFFS_AB, core);
		wlc_phy_cal_txiqlo_coeffs_acphy(pi, CAL_COEFF_READ, &d_reg,
			TB_OFDM_COEFFS_D, core);
		if (TINY_RADIO(pi)) {
			bcm_bprintf(b, "   core-%d: a/b: (%4d,%4d), d: (%3d,%3d)\n",
				core, (int16) ab_int[0], (int16) ab_int[1],
				(int8)((d_reg & 0xFF00) >> 8), /* di */
				(int8)((d_reg & 0x00FF)));     /* dq */
		} else {
			uint16 eir, eqr, fir, fqr;
			if (ACMAJORREV_GE40(pi->pubpi->phy_rev)) {
				eir = 0;
				eqr = 0;
				fir = 0;
				fqr = 0;
			} else {
				eir = READ_RADIO_REGC(pi, RF, TXGM_LOFT_FINE_I, core);
				eqr = READ_RADIO_REGC(pi, RF, TXGM_LOFT_FINE_Q, core);
				fir = READ_RADIO_REGC(pi, RF, TXGM_LOFT_COARSE_I, core);
				fqr = READ_RADIO_REGC(pi, RF, TXGM_LOFT_COARSE_Q, core);
			}
			if (phy_txiqlocal_num_multilo(pi) == 0) {
				bcm_bprintf(b, "   core-%d: a/b: (%4d,%4d), d: (%3d,%3d),"
					" e: (%3d,%3d), f: (%3d,%3d)\n",
					core, (int16) ab_int[0], (int16) ab_int[1],
					(int8)((d_reg & 0xFF00) >> 8), /* di */
					(int8)((d_reg & 0x00FF)),      /* dq */
					(int8)(-((eir & 0xF0) >> 4) + ((eir & 0xF))), /* ei */
					(int8)(-((eqr & 0xF0) >> 4) + ((eqr & 0xF))), /* eq */
					(int8)(-((fir & 0xF0) >> 4) + ((fir & 0xF))), /* fi */
					(int8)(-((fqr & 0xF0) >> 4) + ((fqr & 0xF))));  /* fq */
			} else {
				bcm_bprintf(b, "   core-%d: a/b: (%4d,%4d), d: (%3d,%3d),"
					" e: (%3d,%3d), f: (%3d,%3d)\n",
					core, (int16) ab_int[0], (int16) ab_int[1], 0, 0,
					(int8)(-((eir & 0xF0) >> 4) + ((eir & 0xF))), /* ei */
					(int8)(-((eqr & 0xF0) >> 4) + ((eqr & 0xF))), /* eq */
					(int8)(-((fir & 0xF0) >> 4) + ((fir & 0xF))), /* fi */
					(int8)(-((fqr & 0xF0) >> 4) + ((fqr & 0xF))));  /* fq */
				bcm_bprintf(b, "   Multi-LOFT enabled: ");

				for (multilo_cal_cnt = 1;
					multilo_cal_cnt <= phy_txiqlocal_num_multilo(pi);
					multilo_cal_cnt++) {
					wlc_phy_txiqlocal_lopwr_gettblidx(pi, &multilo_cal,
						multilo_cal_cnt);
					wlc_phy_table_read_acphy(pi,
						ACPHY_TBL_ID_LOFTCOEFFLUTS(core), 1,
						(int8)multilo_cal.lofttbl_start_idx, 16, &d_reg);
					bcm_bprintf(b, "(%3d,%3d)  ",
						(int8)((d_reg & 0xFF00) >> 8),
						(int8)((d_reg & 0x00FF)));
				}
				bcm_bprintf(b, "\n");
			}
		}
	}

	if (ACPHY_TXCAL_PRERXCAL(pi) && (pi->cal_info)) {
		acphy_cal_result_t *accal = &pi->cal_info->u.accal;
		bcm_bprintf(b, "Tx-IQ/LOFT-Cal for Rx-IQ-Cal:\n");
		FOREACH_ACTV_CORE(pi, stf_shdata->phytxchain, core) {
			bcm_bprintf(b, "   core-%d: a/b: (%4d,%4d), d: (%3d,%3d)\n", core,
				(int16)accal->txiqcal_biq2byp_coeffs[2*core + 0],
				(int16)accal->txiqcal_biq2byp_coeffs[2*core + 1],
				(int8)(accal->txlocal_biq2byp_coeffs[core] >> 8),
				(int8)(accal->txlocal_biq2byp_coeffs[core] & 0xFF));
		}
	}

	bcm_bprintf(b, "Rx-IQ-Cal:\n");
	FOREACH_ACTV_CORE(pi, coremask, core) {
		a_reg = READ_PHYREGCE(pi, Core1RxIQCompA, core);
		b_reg = READ_PHYREGCE(pi, Core1RxIQCompB, core);
		a_int = (a_reg >= 512) ? a_reg - 1024 : a_reg; /* s0.9 format */
		b_int = (b_reg >= 512) ? b_reg - 1024 : b_reg;
		if (phy_ac_rxiqcal_is_fdiqi_enabled(pi->u.pi_acphy->rxiqcali)) {
			slope = phy_ac_rxiqcal_get_fdiqi_slope(pi->u.pi_acphy->rxiqcali, core);
			bcm_bprintf(b, "   core-%d: a/b = (%4d,%4d), S = %2d (%1d)\n",
				core, a_int, b_int, slope,
				READ_PHYREGFLD(pi, rxfdiqImbCompCtrl, rxfdiqImbCompEnable));
		} else {
			bcm_bprintf(b, "   core-%d: a/b = (%4d,%4d), S = OFF (%1d)\n",
				core, a_int, b_int,
				READ_PHYREGFLD(pi, rxfdiqImbCompCtrl, rxfdiqImbCompEnable));
		}
	}

	if (ACMAJORREV_32(pi->pubpi->phy_rev) || ACMAJORREV_33(pi->pubpi->phy_rev)) {
		int16  dci, dcq;
		bcm_bprintf(b, "DC-cal:\n");
		FOREACH_CORE(pi, core) {
			dci = READ_PHYREGCE(pi, DCestimateI, core);
			dcq = READ_PHYREGCE(pi, DCestimateQ, core);
			bcm_bprintf(b, "   core-%d: (%d,", core, ((dci*122)/10000));
			bcm_bprintf(b, "  %d)", ((dcq*122)/10000));
			bcm_bprintf(b, "\n");
		}
	} else if (ACMAJORREV_GE47(pi->pubpi->phy_rev) && !ACMAJORREV_128(pi->pubpi->phy_rev)) {
		int16  dci, dcq;
		uint16 sampnum;
		uint16 num_samps = 183;
		int32 cumdci;
		int32 cumdcq;
		bcm_bprintf(b, "DC-cal:\n");
		FOREACH_ACTV_CORE(pi, coremask, core) {
			cumdci = 0;
			cumdcq = 0;
			for (sampnum = 0; sampnum < num_samps; sampnum++) {
				dci = (int16)READ_PHYREGCE(pi, DCestimateI, core);
				dcq = (int16)READ_PHYREGCE(pi, DCestimateQ, core);
				if ((uint16)dci >=  32768) {
					dci = dci-65536;
				}
				if ((uint16)dcq >=  32768) {
					dcq = dcq-65536;
				}
				cumdci = dci+cumdci;
				cumdcq = dcq+cumdcq;
			}
			/* dcest/(num_samps*2048*16)*600 ; 600/(183*2048*16) = 1/10000 */
			bcm_bprintf(b, "   core-%d: (%d,", core, ((cumdci)/10000));
			bcm_bprintf(b, "  %d)", ((cumdcq)/10000));
			bcm_bprintf(b, "\n");
		}
		FOREACH_ACTV_CORE(pi, coremask, core) {
			uint8 tia_idx;
			uint16 dcoe_size = READ_PHYREGFLD(pi, dccal_control_430, dcoe_num_entries);
			uint16 dcc_tia_num_entries = READ_PHYREGFLDCE(pi, dccal_control_33, core,
					dcc_tia_num_entries);
			uint16 tbl_id = AC2PHY_TBL_ID_DCOE_TABLE0 + 32 * core;
			uint32 read_val;
			int16 ii, rr;

			bcm_bprintf(b, "DCC: dcoe_table core %d:\n   ", core);
			for (tia_idx = 0; tia_idx < dcoe_size; tia_idx++) {
				wlc_phy_table_read_acphy(pi, tbl_id, 1, tia_idx, 32, &read_val);
				// dcoe table is 10bit two's complement format
				rr = (read_val >> 10) & 0x3FF;
				ii = read_val & 0x3FF;
				if (rr >= 512) {
					rr = rr - 1024;
				}
				if (ii >= 512) {
					ii = ii - 1024;
				}
				bcm_bprintf(b, "[%2d] %4d, %4d ", tia_idx, rr, ii);
				if ((tia_idx % dcc_tia_num_entries == dcc_tia_num_entries -1) &&
					(tia_idx != dcoe_size - 1)) {
					bcm_bprintf(b, "\n   ");
				}
			}
			bcm_bprintf(b, "\n");
		}
		FOREACH_ACTV_CORE(pi, coremask, core) {
			uint8 rf_idx;
			uint16 gmap_size = 14;
			uint16 tbl_id = AC2PHY_TBL_ID_IDAC_TABLE0 + 32 * core;
			uint32 read_val;
			int16 ii, rr;

			bcm_bprintf(b, "DCC: idac_table core %d:\n   ", core);
			for (rf_idx = 0; rf_idx < gmap_size; rf_idx++) {
				wlc_phy_table_read_acphy(pi, tbl_id, 1, rf_idx, 32, &read_val);
				// idac table is 10bit signed magnitude format
				rr = (read_val >> 10) & 0x3FF;
				ii = read_val & 0x3FF;
				if (rr >= 512) {
					rr = -(rr & 0x1FF);
				}
				if (ii >= 512) {
					ii = -(ii & 0x1FF);
				}
				bcm_bprintf(b, "[%2d] %4d, %4d ", rf_idx, rr, ii);
				if (((rf_idx % 7) == 6) && (rf_idx != gmap_size - 1)) {
					bcm_bprintf(b, "\n   ");
				}
			}
			bcm_bprintf(b, "\n");

			if (!(ACMAJORREV_47(pi->pubpi->phy_rev) ||
			      ACMAJORREV_51(pi->pubpi->phy_rev) ||
			      ACMAJORREV_GE129(pi->pubpi->phy_rev))) {
				// Skip IDAC_RES for older chips
				continue;
			}

			tbl_id = AXPHY_TBL_ID_IDAC_RES_TABLE0 + 32 * core;

			bcm_bprintf(b, "DCC: idac_res_table core %d:\n   ", core);
			for (rf_idx = 0; rf_idx < gmap_size; rf_idx++) {
				wlc_phy_table_read_acphy(pi, tbl_id, 1, rf_idx, 16, &read_val);
				// dcoe table is 8bit two's complement format
				rr = (int8)((read_val >> 8) & 0xFF);
				ii = (int8)(read_val & 0xFF);
				bcm_bprintf(b, "[%2d] %4d, %4d ", rf_idx, rr, ii);
				if ((rf_idx % 7 == 6) && (rf_idx != gmap_size - 1)) {
					bcm_bprintf(b, "\n   ");
				}
			}
			bcm_bprintf(b, "\n");
		}
	}

	ac_reg =  READ_PHYREGFLD(pi, crsminpoweru0, crsminpower0);
	mf_reg =  READ_PHYREGFLD(pi, crsmfminpoweru0, crsmfminpower0);

	phy_ac_rxgcrs_cal_dump(pi->u.pi_acphy->rxgcrsi, b);

	bcm_bprintf(b, "\n");
	bcm_bprintf(b, "   AC-CRS = %u,", ac_reg);
	bcm_bprintf(b, "   MF-CRS = %u,", mf_reg);
	FOREACH_ACTV_CORE(pi, stf_shdata->phyrxchain, core) {
		int8 offset = READ_PHYREGFLDCE(pi, crsminpoweroffset, core, crsminpowerOffsetu);
		bcm_bprintf(b, "   Offset %d = %d,", core, offset);
	}

	if (ACMAJORREV_128(pi->pubpi->phy_rev)) {
		bcm_bprintf(b, "\n");
		FOREACH_ACTV_CORE(pi, stf_shdata->phyrxchain, core) {
			bcm_bprintf(b, "ADCCAPCAL: core%d: adc: cap0 I: 0x%2x Q: 0x%2x\n", core,
				READ_RADIO_REGFLD_20709(pi, RF, RXADC_CFG6,
				core, rxadc_coeff_cap0_adc_I),
				READ_RADIO_REGFLD_20709(pi, RF, RXADC_CFG9,
				core, rxadc_coeff_cap0_adc_Q));
			bcm_bprintf(b, "ADCCAPCAL: core%d: adc: cap1 I: 0x%2x Q: 0x%2x\n", core,
				READ_RADIO_REGFLD_20709(pi, RF, RXADC_CFG5,
				core, rxadc_coeff_cap1_adc_I),
				READ_RADIO_REGFLD_20709(pi, RF, RXADC_CFG8,
				core, rxadc_coeff_cap1_adc_Q));
			bcm_bprintf(b, "ADCCAPCAL: core%d: adc: cap2 I: 0x%2x Q: 0x%2x\n", core,
				READ_RADIO_REGFLD_20709(pi, RF, RXADC_CFG5,
				core, rxadc_coeff_cap2_adc_I),
				READ_RADIO_REGFLD_20709(pi, RF, RXADC_CFG8,
				core, rxadc_coeff_cap2_adc_Q));
		}
	} else {
		bcm_bprintf(b, "\n");
	}
	if (ACMAJORREV_128(pi->pubpi->phy_rev) || ACMAJORREV_131(pi->pubpi->phy_rev)) {
		bcm_bprintf(b, "\n");
		if ((READ_PHYREG(pi, lesi_control) & 0x1) > 0) {
			bcm_bprintf(b, "LESI Noise Pwr Cal:\n");
			FOREACH_ACTV_CORE(pi, stf_shdata->phyrxchain, core) {
				if (core == 0) {
					lesiInpSc[0] = READ_PHYREGFLD(pi, lesiInputScaling0_0,
						inpScalingFactor_0);
					lesiInpSc[1] = READ_PHYREGFLD(pi, lesiInputScaling1_0,
						inpScalingFactor_1);
					lesiInpSc[2] = READ_PHYREGFLD(pi, lesiInputScaling2_0,
						inpScalingFactor_2);
					lesiInpSc[3] = READ_PHYREGFLD(pi, lesiInputScaling3_0,
						inpScalingFactor_3);
					if (CHSPEC_IS160(pi->radio_chanspec)) {
						lesiInpSc[4] = READ_PHYREGFLD(pi,
							lesiInputScaling4_0, inpScalingFactor_4);
						lesiInpSc[5] = READ_PHYREGFLD(pi,
							lesiInputScaling5_0, inpScalingFactor_5);
						lesiInpSc[6] = READ_PHYREGFLD(pi,
							lesiInputScaling6_0, inpScalingFactor_6);
						lesiInpSc[7] = READ_PHYREGFLD(pi,
							lesiInputScaling7_0, inpScalingFactor_7);
					}
				} else if (core == 1) {
					lesiInpSc[0] = READ_PHYREGFLD(pi, lesiInputScaling0_1,
						inpScalingFactor_0);
					lesiInpSc[1] = READ_PHYREGFLD(pi, lesiInputScaling1_1,
						inpScalingFactor_1);
					lesiInpSc[2] = READ_PHYREGFLD(pi, lesiInputScaling2_1,
						inpScalingFactor_2);
					lesiInpSc[3] = READ_PHYREGFLD(pi, lesiInputScaling3_1,
						inpScalingFactor_3);
					if (CHSPEC_IS160(pi->radio_chanspec)) {
						lesiInpSc[4] = READ_PHYREGFLD(pi,
							lesiInputScaling4_1, inpScalingFactor_4);
						lesiInpSc[5] = READ_PHYREGFLD(pi,
							lesiInputScaling5_1, inpScalingFactor_5);
						lesiInpSc[6] = READ_PHYREGFLD(pi,
							lesiInputScaling6_1, inpScalingFactor_6);
						lesiInpSc[7] = READ_PHYREGFLD(pi,
							lesiInputScaling7_1, inpScalingFactor_7);
					}

				} else {
					ASSERT(0);
				}
				bcm_bprintf(b, "\tcore-%d: lesiInpScalingFactor = %d  %d  %d  %d\n",
				core, lesiInpSc[0], lesiInpSc[1], lesiInpSc[2], lesiInpSc[3]);
				if (CHSPEC_IS160(pi->radio_chanspec)) {
					bcm_bprintf(b, "\tcore-%d: lesiInpScalingFactor80U ="
					" %d  %d  %d  %d\n", core,
					lesiInpSc[4], lesiInpSc[5], lesiInpSc[6], lesiInpSc[7]);
				}
			}
		}
	}

	bcm_bprintf(b, "R-CAL:\n");
	bcm_bprintf(b, "   rc_value = %d\n",
		phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->rcal_value);

	bcm_bprintf(b, "RC-CAL:\n");
	bcm_bprintf(b, "   gmult = %d\n",
		phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->rccal_gmult_rc);
	bcm_bprintf(b, "   cmult = %d\n",
		phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->rccal_cmult_rc);

#ifdef ATE_BUILD
	ate_buffer_regval[0].rccal_gmult_rc =
		phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->rccal_gmult_rc;
	ate_buffer_regval[0].rccal_cmult_rc =
		phy_ac_radio_get_data(pi->u.pi_acphy->radioi)->rccal_cmult_rc;
#endif /* ATE_BUILD */

	if (PHY_PAPDEN(pi)) {
		/* Make dump available for both iPA and ePA */
		wlc_phy_papd_dump_eps_trace_acphy(pi, b);
		FOREACH_ACTV_CORE(pi, stf_shdata->phyrxchain, core) {
			if (ACMAJORREV_2(pi->pubpi->phy_rev) ||
			           ACMAJORREV_5(pi->pubpi->phy_rev)) {
				bcm_bprintf(b, "papdcalidx%d %d\n", core, papd_gainctrl_pga[core]);
			}
		}
	}
	phy_utils_phyreg_exit(pi);
	wlapi_enable_mac(pi->sh->physhim);

	return;
}
#endif
