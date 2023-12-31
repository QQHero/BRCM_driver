/*
 * Rx Gain Control and Carrier Sense module implementation - iovar table
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
 * $Id: phy_rxgcrs_iov.c 790225 2020-08-19 10:04:21Z $
 */

#include <phy_rxgcrs.h>
#include <phy_rxgcrs_iov.h>
#include <phy_rxgcrs_api.h>
#include <phy_rxgcrs.h>
#include <wlc_iocv_reg.h>
#ifndef ALL_NEW_PHY_MOD
#include <wlc_phy_int.h>
#endif

/* iovar ids */
enum {
	IOV_ED_THRESH = 1,
	IOV_PHY_RXDESENS = 2,
	IOV_PHY_FORCECAL_NOISE = 3,
	IOV_PHY_FORCE_TR2T = 4
};

/* iovar table */
static const bcm_iovar_t phy_rxgcrs_iovars[] = {
	{"phy_ed_thresh", IOV_ED_THRESH, (IOVF_SET_UP | IOVF_GET_UP), 0, IOVT_INT32, 0},
	{"dy_ed_thresh", IOV_DYNAMIC_ED_THRESH_EN, (IOVF_SET_UP | IOVF_GET_UP), 0, IOVT_INT32, 0},
	{"dy_ed_setup", IOV_DED_SETUP,	(IOVF_SET_UP | IOVF_GET_UP), 0, IOVT_BUFFER, 0},
	/* added for debug purposes. Now one can overwrite the initialization */
	{"dy_ed_thresh_acphy", IOV_DYNAMIC_ED_THRESH_ACPHY_NVRAM,
	(IOVF_SET_UP | IOVF_GET_UP), 0, IOVT_INT32, 0},
	{"phy_rxdesens", IOV_PHY_RXDESENS, (IOVF_SET_UP | IOVF_GET_UP), 0, IOVT_INT32, 0},
	{"phy_force_tr2t", IOV_PHY_FORCE_TR2T, (IOVF_SET_UP), 0, IOVT_UINT32, 0},
#ifndef ATE_BUILD
#if defined(WLTEST) || defined(DBG_PHY_IOV) || defined(WFD_PHY_LL_DEBUG)
	{"phy_forcecal_noise", IOV_PHY_FORCECAL_NOISE,
	(IOVF_SET_UP | IOVF_MFG), 0, IOVT_BUFFER, sizeof(uint16)
	},
#endif
#endif /* !ATE_BUILD */
	{NULL, 0, 0, 0, 0, 0}
};

/* This includes the auto generated ROM IOCTL/IOVAR patch handler C source file (if auto patching is
 * enabled). It must be included after the prototypes and declarations above (since the generated
 * source file may reference private constants, types, variables, and functions).
 */
#include <wlc_patch.h>

static int
phy_rxgcrs_doiovar(void *ctx, uint32 aid, void *p, uint plen, void *a, uint alen, uint vsize,
	struct wlc_if *wlcif)
{
	int err = BCME_OK;
	phy_info_t *pi = (phy_info_t *)ctx;
	int int_val = 0;
	int32 *ret_int_ptr = (int32 *)a;

	if (plen >= (uint)sizeof(int_val))
		bcopy(p, &int_val, sizeof(int_val));

	switch (aid) {
	case IOV_SVAL(IOV_ED_THRESH):
		BCM_REFERENCE(int_val);
		if (pi->dynamic_ed_thresh_enable == 0) {
			err = wlc_phy_adjust_ed_thres(pi, &int_val, TRUE);
		}
		break;
	case IOV_GVAL(IOV_ED_THRESH):
		err = wlc_phy_adjust_ed_thres(pi, ret_int_ptr, FALSE);
		break;
	case IOV_SVAL(IOV_DYNAMIC_ED_THRESH_EN):
		if (int_val == 0) {
			wlc_dynamic_ed_thresh_enable(pi, FALSE);
		} else {
			wlc_dynamic_ed_thresh_enable(pi, TRUE);
		}
		break;
	case IOV_GVAL(IOV_DYNAMIC_ED_THRESH_EN):
		*ret_int_ptr = pi->dynamic_ed_thresh_enable;
		break;
	case IOV_SVAL(IOV_DYNAMIC_ED_THRESH_ACPHY_NVRAM):
		wlc_dynamic_ed_th_overwrite_acphy(pi, int_val);
		break;
	case IOV_GVAL(IOV_DYNAMIC_ED_THRESH_ACPHY_NVRAM):
		*ret_int_ptr = pi->acphy_for_dynamic_ed;
		break;
	case IOV_GVAL(IOV_DED_SETUP): {
		dynamic_ed_setup_t *setup = (dynamic_ed_setup_t *)a;
		setup->ed_monitor_window = pi->const_ed_monitor_window;
		setup->sed_dis = pi->const_sed_disable;
		setup->sed_upper_bound = pi->const_sed_upper_bound;
		setup->sed_lower_bound = pi->const_sed_lower_bound;
		setup->ed_th_high = pi->const_ed_th_high;
		setup->ed_th_low = pi->const_ed_th_low;
		setup->ed_inc_step = pi->const_ed_inc_step;
		setup->ed_dec_step = pi->const_ed_dec_step;
		break;
	}
	case IOV_SVAL(IOV_DED_SETUP): {
		dynamic_ed_setup_t *setup = (dynamic_ed_setup_t *)a;
		if (setup->ed_monitor_window > 0) {
			wlc_dynamic_ed_th_overwrite_mon_win(pi, setup->ed_monitor_window);
		}
		if (setup->sed_dis <= DYN_ED_MAX_SED) {
			wlc_dynamic_ed_th_overwrite_sed_dis(pi, setup->sed_dis);
		}
		if (setup->sed_upper_bound >= pi->const_sed_lower_bound &&
			setup->sed_upper_bound <= DYN_ED_MAX_SED) {
			wlc_dynamic_ed_th_overwrite_sed_high(pi, setup->sed_upper_bound);
		}
		if (setup->sed_lower_bound <= pi->const_sed_upper_bound &&
			setup->sed_lower_bound >= DYN_ED_MIN_SED) {
			wlc_dynamic_ed_th_overwrite_sed_low(pi, setup->sed_lower_bound);
		}
		if (setup->ed_th_high >= pi->const_ed_th_low &&
			setup->ed_th_high <= DYN_ED_MAX_ACC_TH) {
			wlc_dynamic_ed_th_overwrite_th_high(pi, setup->ed_th_high);
		}
		if (setup->ed_th_low >= DYN_ED_MIN_ACC_TH &&
			setup->ed_th_low <= pi->const_ed_th_high) {
			wlc_dynamic_ed_th_overwrite_th_low(pi, setup->ed_th_low);
		}
		if (setup->ed_inc_step > 0) {
			wlc_dynamic_ed_th_overwrite_step_inc(pi, setup->ed_inc_step);
		}
		if (setup->ed_dec_step > 0) {
			wlc_dynamic_ed_th_overwrite_step_dec(pi, setup->ed_dec_step);
		}

		break;
	}
	case IOV_GVAL(IOV_PHY_RXDESENS):
		err = phy_rxgcrs_get_rxdesens(pi, ret_int_ptr);
		break;

	case IOV_SVAL(IOV_PHY_RXDESENS):
		if (plen >= (uint)sizeof(int_val))
				bcopy(p, &int_val, sizeof(int_val));
		err = phy_rxgcrs_set_rxdesens(pi, int_val);
		break;
	case IOV_SVAL(IOV_PHY_FORCE_TR2T):
		err = wlc_phy_iovar_force_tr2t(pi, int_val != 0);
		break;
#ifndef ATE_BUILD
#if defined(WLTEST) || defined(DBG_PHY_IOV) || defined(WFD_PHY_LL_DEBUG)
	case IOV_GVAL(IOV_PHY_FORCECAL_NOISE): /* Get crsminpwr for core 0 & core 1 */
		err = wlc_phy_iovar_forcecal_noise(pi, a, FALSE);
		break;

	case IOV_SVAL(IOV_PHY_FORCECAL_NOISE): /* do only Noise Cal */
		err = wlc_phy_iovar_forcecal_noise(pi, a, TRUE);
		break;
#endif
#endif /* !ATE_BUILD */
	default:
		err = BCME_UNSUPPORTED;
		break;
	}

	return err;
}

/* register iovar table to the system */
int
BCMATTACHFN(phy_rxgcrs_register_iovt)(phy_info_t *pi, wlc_iocv_info_t *ii)
{
	wlc_iovt_desc_t iovd;
#if defined(WLC_PATCH_IOCTL)
	wlc_iov_disp_fn_t disp_fn = IOV_PATCH_FN;
	const bcm_iovar_t *patch_table = IOV_PATCH_TBL;
#else
	wlc_iov_disp_fn_t disp_fn = NULL;
	const bcm_iovar_t* patch_table = NULL;
#endif /* WLC_PATCH_IOCTL */

	ASSERT(ii != NULL);

	wlc_iocv_init_iovd(phy_rxgcrs_iovars,
	                   NULL, NULL,
	                   phy_rxgcrs_doiovar, disp_fn, patch_table, pi,
	                   &iovd);

	return wlc_iocv_register_iovt(ii, &iovd);
}
