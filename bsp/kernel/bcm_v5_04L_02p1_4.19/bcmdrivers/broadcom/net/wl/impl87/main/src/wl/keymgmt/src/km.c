/*
 * Key Management Module Implementation
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
 * $Id: km.c 802604 2021-09-01 17:08:39Z $
 */

/* This file implements the wlc keymgmt functionality. It provides
 *		attach/detach wlc module interface
 *		reset support
 *		key index, allocation and lifetime management
 *		interface to km_hw
 */

#include "km_pvt.h"
#include "wlc.h"

#if defined(BCMDBG)
#define KM_SCB_DUMP km_scb_dump
#else
#define KM_SCB_DUMP NULL
#endif

#if defined(PKTC) || defined(PKTC_DONGLE)
/* reset key cache. note: must be called when null key is valid */
void km_reset_key_cache(keymgmt_t *km)
{
	if (km->flags & KM_FLAG_DETACHING) {
		return;
	}
	KM_DBG_ASSERT(KM_VALID(km) && km->key_cache != NULL);
	if (km->null_key)
		km_key_update_key_cache(km->null_key, km->key_cache);
}
#endif /* PKTC || PKTC_DONGLE */

int
BCMINITFN(km_wlc_up)(void *ctx)
{
	keymgmt_t *km = (keymgmt_t *)ctx;
	km_hw_init(km->hw);
	km_notify(km, WLC_KEYMGMT_NOTIF_WLC_UP, NULL, NULL, NULL, NULL);
	return BCME_OK;
}

int
BCMUNINITFN(km_wlc_down)(void *ctx)
{
	keymgmt_t *km = (keymgmt_t *)ctx;
	km_notify(km, WLC_KEYMGMT_NOTIF_WLC_DOWN, NULL, NULL, NULL, NULL);
	return BCME_OK;
}

keymgmt_t*
BCMATTACHFN(wlc_keymgmt_attach)(wlc_info_t* wlc)
{
	keymgmt_t *km;
	int err = BCME_OK;
	wlc_key_info_t key_info;
	bsscfg_cubby_params_t cubby_params;
	scb_cubby_params_t km_scb_cubby_params;

	STATIC_ASSERT(WLC_KEYMGMT_NUM_GROUP_KEYS == DOT11_MAX_DEFAULT_KEYS);
	STATIC_ASSERT(WLC_KEYMGMT_NUM_BSS_IGTK == DOT11_MAX_IGTK_KEYS);
	STATIC_ASSERT(WSEC_ALGO_OFF == 0);
	STATIC_ASSERT(CRYPTO_ALGO_NONE == 0);

	KM_DBG_ASSERT(wlc != NULL && wlc->keymgmt == NULL);

	KM_LOG(("wl%d: %s\n",  WLCWLUNIT(wlc), __FUNCTION__));

	km = (keymgmt_t *)MALLOCZ(wlc->osh, sizeof(keymgmt_t));
	if (!km) {
		err = BCME_NOMEM;
		goto done;
	}

	km->magic = KM_MAGIC;
	km->wlc = wlc;

	/* attach to hw */
	km->hw = km_hw_attach(wlc, km);
	if (km->hw == NULL) {
		err = BCME_NORESOURCE;
		goto done;
	}

	err = km_module_register(km);

	if (err != BCME_OK) {
		KM_REGST_ERR(("wl%d: %s: wlc_module_register failed\n",
				WLCWLUNIT(wlc), __FUNCTION__));
		err = BCME_NORESOURCE;
		goto done;
	}

	/* reserve cubby space in the bsscfg container for per-bsscfg private data */
	bzero(&cubby_params, sizeof(cubby_params));

	cubby_params.context = km;
	cubby_params.fn_init = km_bsscfg_init;
	cubby_params.fn_deinit = km_bsscfg_deinit;
#if defined(BCMDBG)
	cubby_params.fn_dump = km_bsscfg_dump;
#endif
	km->h_bsscfg = wlc_bsscfg_cubby_reserve_ext(wlc, sizeof(km_bsscfg_t), &cubby_params);

	if (km->h_bsscfg < 0) {
		KM_REGST_ERR(("wl%d: %s: wlc_bsscfg_cubby_reserve failed\n",
		        WLCWLUNIT(wlc), __FUNCTION__));
		err = BCME_NORESOURCE;
		goto done;
	}

	/* bss up/down */
	err = wlc_bsscfg_updown_register(wlc, km_bsscfg_up_down, (void *)km);
	if (err != BCME_OK) {
		KM_REGST_ERR(("wl%d: %s: wlc_bsscfg_updown_register failed, error %d\n",
		        WLCWLUNIT(wlc), __FUNCTION__, err));
		goto done;
	}

	/* reserve some space in scb container */
	bzero(&km_scb_cubby_params, sizeof(km_scb_cubby_params));

	km_scb_cubby_params.context = km;
	km_scb_cubby_params.fn_init = km_scb_init;
	km_scb_cubby_params.fn_deinit = km_scb_deinit;
	km_scb_cubby_params.fn_dump = KM_SCB_DUMP;

	km->h_scb = wlc_scb_cubby_reserve_ext(wlc, sizeof(km_scb_t), &km_scb_cubby_params);

	if (km->h_scb < 0) {
		KM_REGST_ERR(("wl%d: %s: wlc_scb_cubby_reserve failed\n",
		        WLCWLUNIT(wlc), __FUNCTION__));
		err = BCME_NORESOURCE;
		goto done;
	}

	err = wlc_scb_state_upd_register(wlc, (scb_state_upd_cb_t)km_scb_state_upd, (void*)km);
	if (err != BCME_OK) {
		KM_REGST_ERR(("wl%d: %s: wlc_scb_state_upd_register failed\n",
		        WLCWLUNIT(wlc), __FUNCTION__));
		err = BCME_NORESOURCE;
		goto done;
	}

#if defined(BCMDBG)
	/* register dump support */
	err = km_register_dump(km);
	if (err != BCME_OK) {
		KM_REGST_ERR(("wl%d: %s: km_register_dump failed, error %d\n",
		        WLCWLUNIT(wlc), __FUNCTION__, err));
		goto done;
	}
#endif

	/* register ioctl support */
	err = km_register_ioctl(km);
	if (err != BCME_OK) {
		KM_REGST_ERR(("wl%d: %s: km_register_ioctl failed, error %d\n",
		        WLCWLUNIT(wlc), __FUNCTION__, err));
		goto done;
	}

	/* notification support */
	err = bcm_notif_create_list(wlc->notif, &km->h_notif);
	if (err != BCME_OK) {
		KM_ALLOC_ERR(("wl%d: %s: bcm_notif_create_list failed, error %d\n",
		        WLCWLUNIT(wlc), __FUNCTION__, err));
		goto done;
	}

	/* allocate keys */
	km->max_keys = (uint16)wlc->pub->tunables->max_keys;
	KM_ASSERT(km->max_keys != 0);
	km->keys  = (km_pvt_key_t *)MALLOCZ(wlc->osh, (sizeof(km_pvt_key_t) * km->max_keys));
	if (km->keys ==  NULL) {
		KM_ALLOC_ERR(("wl%d: %s: MALLOCZ failed\n", WLCWLUNIT(wlc), __FUNCTION__));
		km->max_keys = 0;
		err = BCME_NOMEM;
		goto done;
	}

	memset(&key_info, 0, sizeof(key_info));
	key_info.key_idx = WLC_KEY_INDEX_INVALID;
	key_info.key_id = WLC_KEY_ID_INVALID;
	err = km_key_create(km, &key_info, &km->null_key);
	if (err != BCME_OK) {
		KM_ALLOC_ERR(("wl%d: %s: km_key_create failed, error %d\n",
		        WLCWLUNIT(wlc), __FUNCTION__, err));
		goto done;
	}

#ifdef BRCMAPIVTW
	km->ivtw = km_ivtw_attach(wlc, km);
	if (km->ivtw == NULL) {
		KM_ALLOC_ERR(("wl%d: %s: km_ivtw_attach failed\n", WLCWLUNIT(wlc), __FUNCTION__));
		err = BCME_NORESOURCE;
		goto done;
	}
#endif /* BRCMAPIVTW */

#ifdef WOWL
	/* legacy wowl offloads */
	km->wowl_hw = km_wowl_hw_attach(km->wlc, km); /* errors reported by callee */
	if (!km->wowl_hw) {
		KM_ALLOC_ERR(("wl%d: %s: km_wowl_hw_attach failed\n",
				WLCWLUNIT(wlc), __FUNCTION__));
		err = BCME_NORESOURCE;
		goto done;
	}
#endif /* WOWL */

#if defined(PKTC) || defined(PKTC_DONGLE)
	km->key_cache = MALLOCZ(wlc->osh, sizeof(km_key_cache_t));
	if (!km->key_cache) {
		KM_ALLOC_ERR(("wl%d: %s: key cache allocation failed\n",
				WLCWLUNIT(wlc), __FUNCTION__));
		err = BCME_NORESOURCE;
		goto done;
	}
	KM_RESET_KEY_CACHE(km);
#endif /* PKTC || PKTC_DONGLE */

	/* mark the initial state as down */
	km->flags |= KM_FLAG_WLC_DOWN;

#ifdef MFP
	/* Use SW encryption for pre-11ax chips */
	if (D11REV_LT(wlc->pub->corerev, 128)) {
		wlc->pub->_uc_mgmt_sw_encr = TRUE;
	} else {
		wlc->pub->_uc_mgmt_sw_encr = FALSE;
	}
#endif /* MFP */

done:
	if (err != BCME_OK) {
		KM_REGST_ERR(("wl%d: %s: done with error %d\n",
				WLCWLUNIT(wlc),	__FUNCTION__, err));
		MODULE_DETACH(km, wlc_keymgmt_detach);
		km = NULL;
	} else {
		KM_LOG(("wl%d: %s: done\n",  WLCWLUNIT(wlc), __FUNCTION__));
	}

	return km;
}

void
BCMATTACHFN(wlc_keymgmt_detach)(keymgmt_t *km)
{
	wlc_info_t *wlc;
	int i;

	KM_DBG_ASSERT(KM_VALID(km));
	if (km == NULL)
		return;

	KM_LOG(("wl%d: %s: start\n",  KM_UNIT(km), __FUNCTION__));

	wlc = km->wlc;

	/* indicate we are detaching. this is used to control re-entrancy  during
	 * destruction
	 */
	km->flags |= KM_FLAG_DETACHING;

#if defined(PKTC) || defined(PKTC_DONGLE)
	if (km->key_cache) {
		KM_RESET_KEY_CACHE(km);
		MFREE(wlc->osh, km->key_cache, sizeof(*(km->key_cache)));
		km->key_cache = NULL;
	}
#endif /* PKTC || PKTC_DONGLE */
	KM_DBG_ASSERT(km->key_cache == NULL);

#ifdef BRCMAPIVTW
	km_ivtw_detach(&km->ivtw);
#endif

#ifdef WOWL
	if (km->wowl_hw)
		km_wowl_hw_detach(&km->wowl_hw);
#endif
	KM_DBG_ASSERT(km->wowl_hw == NULL);

	/* destroy all  keys */
	if (km->null_key)
		km_key_destroy(&km->null_key);
	KM_DBG_ASSERT(km->null_key == NULL);
	for (i = 0; i < km->max_keys; ++i) {
		if (km->keys[i].key != NULL)
			km_key_destroy(&km->keys[i].key);
		KM_DBG_ASSERT(km->keys[i].key == NULL);
	}
	MFREE(wlc->osh, km->keys, km->max_keys * sizeof(km_pvt_key_t));
	km->keys = NULL;
	km->max_keys = 0;

	if (km->tlv_buffer) {
		MFREE(wlc->osh, km->tlv_buffer, km->tlv_buf_size);
	}

	/* clean up our notify lists */
	if (km->h_notif != NULL)
		bcm_notif_delete_list(&km->h_notif);
	KM_DBG_ASSERT(km->h_notif == NULL);

	/* unregister ioctl support */
	km_unregister_ioctl(km);

	/* nothing to do to unregister dump */

	/* unregister for bss up/down */
	wlc_bsscfg_updown_unregister(wlc, km_bsscfg_up_down, (void *)km);

	/* unregister from scb state changes */
	wlc_scb_state_upd_unregister(wlc, (scb_state_upd_cb_t)km_scb_state_upd, (void*)km);

	/* detach from hw km */
	km_hw_detach(&km->hw);

	/* unregister module  */
	wlc_module_unregister(wlc->pub, KM_MODULE_NAME, km);

	km->magic = KM_BAD_MAGIC;
	MFREE(wlc->osh, km, sizeof(keymgmt_t));
	KM_LOG(("wl%d: %s: done\n",  WLCWLUNIT(wlc), __FUNCTION__));
}

void
wlc_keymgmt_reset(keymgmt_t *km, wlc_bsscfg_t *bsscfg, scb_t *scb)
{
	int bsscfg_idx;
	wlc_info_t *wlc;
	struct scb_iter scbiter;

	KM_DBG_ASSERT(KM_VALID(km));
	KM_LOG(("wl%d: %s: start\n",  KM_UNIT(km), __FUNCTION__));

	wlc = km->wlc;

	if (scb != NULL) {
		km_scb_reset(km, scb);
	}

	if (bsscfg != NULL) {
		FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
			km_scb_reset(km, scb);
		}
		km_bsscfg_reset(km, bsscfg, TRUE);
		km_event_signal(km, WLC_KEYMGMT_EVENT_RESET, bsscfg, NULL, NULL);
		bsscfg->wsec_portopen = FALSE;	/* close wsec port */
	}

	if (bsscfg != NULL || scb != NULL) {
		goto done;
	}

	/* reset all */
	FOREACH_BSS(wlc, bsscfg_idx, bsscfg) {
		FOREACH_BSS_SCB(wlc->scbstate, &scbiter, bsscfg, scb) {
			km_scb_reset(km, scb);
		}
		km_bsscfg_reset(km, bsscfg, TRUE);
		km_event_signal(km, WLC_KEYMGMT_EVENT_RESET, bsscfg, NULL, NULL);
	}

	km_hw_reset(km->hw);

done:
	KM_LOG(("wl%d: %s: done\n",  KM_UNIT(km), __FUNCTION__));
}

void
wlc_keymgmt_get_wsec_info(keymgmt_t *km, wlc_bsscfg_t *bsscfg, wlc_keymgmt_wsec_info_t * wsec_info)
{
	const km_bsscfg_t *bss_km;

	bss_km = KM_BSSCFG(km, bsscfg);
	KM_DBG_ASSERT(bss_km != NULL);
	wsec_info->cur_algos = bss_km ? bss_km->wsec_config_algos : 0;

	return;
}

int
wlc_keymgmt_get_cur_wsec_algo(wlc_keymgmt_t *km, wlc_bsscfg_t *bsscfg,
	uint32 key_size, wlc_key_algo_t *key_algo)
{
	wlc_keymgmt_wsec_info_t wsec_info;
	int err = BCME_OK;

	/* Fetch current wsec algorithms bitmap */
	wlc_keymgmt_get_wsec_info(km, bsscfg, &wsec_info);

	if (key_size == AES_KEY_SIZE) {
		if (wsec_info.cur_algos & (1 << CRYPTO_ALGO_AES_GCM)) {
			*key_algo = CRYPTO_ALGO_AES_GCM;
		}
		else {
			*key_algo = CRYPTO_ALGO_AES_CCM;
		}
	}
	else if (key_size == TKIP_KEY_SIZE) {
		if (wsec_info.cur_algos & (1 << CRYPTO_ALGO_AES_GCM256)) {
			*key_algo = CRYPTO_ALGO_AES_GCM256;
		}
		else {
			*key_algo = CRYPTO_ALGO_TKIP;
		}
	}
	else {
		err = BCME_UNSUPPORTED;
	}

	return err;
}
