/*
 * NOISEmeasure module implementation - iovar handlers & registration
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
 * $Id: phy_noise_iov.c 788933 2020-07-14 19:18:36Z $
 */

#include <phy_noise_iov.h>
#include <phy_noise.h>
#include <wlc_iocv_reg.h>

/* iovar ids */
enum {
	IOV_PHYNOISE_SROM = 1,
#ifdef WL_EAP_NOISE_MEASUREMENTS
	IOV_PHYNOISE_BIAS = 2,
	IOV_PHYNOISE_BIASRXGAINERR = 3,
#endif /* WL_EAP_NOISE_MEASUREMENTS */
	IOV_PHYNOISE_LAST
};

/* iovar table */
static const bcm_iovar_t phy_noise_iovars[] = {
	{"phynoise_srom", IOV_PHYNOISE_SROM, IOVF_GET_UP, 0, IOVT_UINT32, 0},
#ifdef WL_EAP_NOISE_MEASUREMENTS
	/* Noise Bias IOVARs raise or the lower the calculated noise floor */
	{"_phynoisebias", IOV_PHYNOISE_BIAS, 0, 0, IOVT_BUFFER, sizeof(wl_phynoisebias_iov_t)},
	{"phynoisebiasrxgainerr", IOV_PHYNOISE_BIASRXGAINERR, IOVF_GET_UP, 0, IOVT_BOOL, 0},
#endif /* WL_EAP_NOISE_MEASUREMENTS */
	{NULL, 0, 0, 0, 0, 0}
};

#include <wlc_patch.h>

static int
phy_noise_doiovar(void *ctx, uint32 aid,
	void *p, uint plen, void *a, uint alen, uint vsize, struct wlc_if *wlcif)
{
	phy_info_t *pi = (phy_info_t *)ctx;
	int32 int_val = 0;
	int err = BCME_OK;
	int32 *ret_int_ptr = (int32 *)a;

	if (plen >= (uint)sizeof(int_val))
		bcopy(p, &int_val, sizeof(int_val));

	switch (aid) {
	case IOV_GVAL(IOV_PHYNOISE_SROM):
		err = phy_noise_get_srom_level(pi, ret_int_ptr);
		break;

#ifdef WL_EAP_NOISE_MEASUREMENTS
	/* Noise Bias IOVARs raise or the lower the calculated noise floor */
	case IOV_GVAL(IOV_PHYNOISE_BIAS):
	{
		wl_phynoisebias_iov_t *pvp, *pva;
		pvp = (wl_phynoisebias_iov_t *)p;
		pva = (wl_phynoisebias_iov_t *)a;
		/* Error check enums used as array indicies */
		if (((unsigned int)pvp->band < WL_PHY_NB_NUMBANDS) &&
			((unsigned int)pvp->gain < WL_PHY_NB_NUMGAINS)) {
			/* Get band & gain offsets */
			phy_noise_get_gain_bias_range(pi, pvp->gain, pvp->band,
				&pva->dB_offsets);
		} else {
			err = BCME_RANGE;
		}
		break;
	}
	case IOV_SVAL(IOV_PHYNOISE_BIAS):
	{
		wl_phynoisebias_iov_t *pv = (wl_phynoisebias_iov_t *)p;
		/* Error check enums used as array indicies */
		if (((unsigned int)pv->band < WL_PHY_NB_NUMBANDS) &&
			((unsigned int)pv->gain < WL_PHY_NB_NUMGAINS)) {
			/* Set offsets based on band & gain */
			phy_noise_set_gain_bias_range(pi, pv->gain, pv->band,
				&pv->dB_offsets);
		} else {
			err = BCME_RANGE;
		}
		break;
	}

	case IOV_GVAL(IOV_PHYNOISE_BIASRXGAINERR):
		*ret_int_ptr = phy_noise_get_rxgainerr_bias(pi);
		break;
	case IOV_SVAL(IOV_PHYNOISE_BIASRXGAINERR):
		phy_noise_set_rxgainerr_bias(pi, int_val);
		break;
#endif /* WL_EAP_NOISE_MEASUREMENTS */

	default:
		err = BCME_UNSUPPORTED;
		break;
	}

	return err;
}

/* register iovar table to the system */
int
BCMATTACHFN(phy_noise_register_iovt)(phy_info_t *pi, wlc_iocv_info_t *ii)
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

	wlc_iocv_init_iovd(phy_noise_iovars,
	                   NULL, NULL,
	                   phy_noise_doiovar, disp_fn, patch_table, pi,
	                   &iovd);

	return wlc_iocv_register_iovt(ii, &iovd);
}
