/*
 * Key Management Module km_hw algo implementation
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
 * $Id: km_hw_algo.c 795548 2021-02-09 10:20:51Z $
 */

#include "km_hw_impl.h"
#include <tkhash.h>

static void
hw_algo_impl_init(const km_hw_t *hw, km_hw_algo_impl_t *impl,
	key_algo_t algo, const km_hw_algo_callbacks_t *cb,
	void *cb_ctx, const km_hw_dt_mask_t dt_mask)
{
	hw_algo_t hw_algo;
	hw_algo = km_hw_algo_to_hw_algo(hw, algo);
	if (hw_algo != WSEC_ALGO_OFF) {
		impl->cb = cb;
		impl->hw_algo = hw_algo;
		impl->cb_ctx = cb_ctx;
		impl->dt_mask = dt_mask;
	}
}

#ifdef WL_HWKTAB
#define HW_ALGO_IMPL_INIT(_hw, _impl, _algo_name, _key_algo) {\
	km_hw_dt_mask_t _dt_mask; \
	_dt_mask = (KM_HW_WOWL_SUPPORTED(_hw) ? \
		_algo_name##_wowl_dt_mask :  _algo_name##_dt_mask); \
	KM_HW_KEYTAB_SUPPORTED(_hw) ? \
	hw_algo_impl_init_hwktab(_hw, _impl, _key_algo, &(_algo_name##_callbacks_hwktab), \
			NULL, _dt_mask) : \
	hw_algo_impl_init(_hw, _impl, _key_algo, &(_algo_name##_callbacks), NULL, _dt_mask); \
}
#else
#define HW_ALGO_IMPL_INIT(_hw, _impl, _algo_name, _key_algo) {\
	km_hw_dt_mask_t _dt_mask; \
	_dt_mask = (KM_HW_WOWL_SUPPORTED(_hw) ? \
		_algo_name##_wowl_dt_mask :  _algo_name##_dt_mask); \
	hw_algo_impl_init(_hw, _impl, _key_algo, &(_algo_name##_callbacks), NULL, _dt_mask); \
}
#endif /* WL_HWKTAB */

/* write the key into shm slot and clear unused */
static void
km_hw_algo_write_key(km_hw_t *hw, shm_addr_t addr, const uint8 *data, size_t data_len)
{
	wlc_info_t *wlc = KM_HW_WLC(hw);

	if ((data_len > 0) && !data[0]) {
	        data = km_hw_fixup_null_hw_key(hw, data, data_len);
	}
	KM_HW_COPYTO_SHM(wlc, addr, data, (int)data_len);
	if (data_len < KM_HW_D11_MIN_KEY_SIZE)
		KM_HW_SET_SHM(wlc, addr + (shm_addr_t)data_len, 0,
			KM_HW_D11_MIN_KEY_SIZE - (uint8)data_len);

	KM_HW_LOG(("wl%d: %s: wrote h/w key data at shm addr %d[0x%04x], length %lu\n",
		KM_HW_UNIT(hw), __FUNCTION__, addr, addr, (unsigned long)data_len));
}

#if defined(BCMDBG) || defined(BCMDBUG_DUMP) || defined(WOWL)
#define HW_READCB(_cb) _cb

static int
wep_read(km_hw_t *hw, void *ctx, hw_idx_t hw_idx,
    wlc_key_data_type_t data_type, int ins, bool tx, uint8 *data,
	size_t data_size, size_t *data_len)
{
	int err = BCME_OK;
	BCM_REFERENCE(ctx);
	BCM_REFERENCE(ins);

	if (!data_len) {
		err = BCME_BADARG;
		goto done;
	}

	switch (data_type) {
	case WLC_KEY_DATA_TYPE_KEY:
		*data_len = WEP128_KEY_SIZE;
		if (data_size < *data_len) {
			err = BCME_BUFTOOSHORT;
			break;
		}
		KM_HW_COPYFROM_SHM(KM_HW_WLC(hw), KM_HW_KEY_ADDR(hw, hw_idx),
			data, (int)(*data_len));
		break;

	case WLC_KEY_DATA_TYPE_SEQ:
		(void)tx;
		/* fall through */
	default:
		err = BCME_UNSUPPORTED;
		break;
	}

done:
	return err;
}

static int
aes_read(km_hw_t *hw, void *ctx, hw_idx_t hw_idx,
    wlc_key_data_type_t data_type, int ins, bool tx, uint8 *data,
	size_t data_size, size_t *data_len)
{
	int err = BCME_OK;
	BCM_REFERENCE(ctx);

	if (!data_len) {
		err = BCME_BADARG;
		goto done;
	}

	switch (data_type) {
	case WLC_KEY_DATA_TYPE_KEY:
		*data_len = hw->max_key_size;
		if (data_size < *data_len) {
			err = BCME_BUFTOOSHORT;
			break;
		}
		KM_HW_COPYFROM_SHM(KM_HW_WLC(hw), KM_HW_KEY_ADDR(hw, hw_idx),
			data, (int)(*data_len));
		break;
	case WLC_KEY_DATA_TYPE_SEQ:
		*data_len = KEY_SEQ_SIZE;
		if (data_size < *data_len) {
			err = BCME_BUFTOOSHORT;
			break;
		}
#ifdef WOWL
		if (tx) {
			shm_addr_t shm_addr;
			if (!hw->shm_info.tx_pn_base) {
				err = BCME_UNSUPPORTED;
				break;
			}
			shm_addr = hw->shm_info.tx_pn_base + OFFSETOF(wowl_templ_ctxt_t, seciv) +
				TKHASH_P1_KEY_SIZE;
			KM_HW_COPYFROM_SHM(KM_HW_WLC(hw), shm_addr, data, KEY_SEQ_SIZE);
			break;
		}
#endif /* WOWL */
		if (!hw->shm_info.rx_pn_base ||
			hw_idx >= hw->shm_info.max_rx_pn ||
			ins >= WLC_KEY_BASE_RX_SEQ) {
			err = BCME_UNSUPPORTED;
			break;
		}
		KM_HW_COPYFROM_SHM(KM_HW_WLC(hw), KM_HW_RX_SEQ_ADDR(hw, hw_idx, ins),
			data, (int)(*data_len));
		break;
	default:
		err = BCME_UNSUPPORTED;
		break;
	}

done:
	return err;
}

#ifndef LINUX_CRYPTO
static int
tkip_read(km_hw_t *hw, void *ctx, hw_idx_t hw_idx,
    wlc_key_data_type_t data_type, int ins, bool tx, uint8 *data,
	size_t data_size, size_t *data_len)
{
	int err = BCME_OK;
	BCM_REFERENCE(ctx);

	if (!data_len) {
		err = BCME_BADARG;
		goto done;
	}

	switch (data_type) {
	case WLC_KEY_DATA_TYPE_TKIP_TK:
		*data_len = TKIP_TK_SIZE;
		if (data_size < *data_len) {
			err = BCME_BUFTOOSHORT;
			break;
		}
		KM_HW_COPYFROM_SHM(KM_HW_WLC(hw), KM_HW_KEY_ADDR(hw, hw_idx),
			data, (int)(*data_len));
		break;
	case WLC_KEY_DATA_TYPE_MIC_KEY_FROM_DS:
		if (!hw->shm_info.tkip_mic_key_base ||
			(hw_idx >= hw->shm_info.max_tkip_mic_keys)) {
			err = BCME_UNSUPPORTED;
			break;
		}

		*data_len = TKIP_MIC_KEY_SIZE;
		if (data_size < *data_len) {
			err = BCME_BUFTOOSHORT;
			break;
		}

		/* no context to determine from/to AP - do something reasonable */
#ifdef WOWL
		KM_HW_COPYFROM_SHM(KM_HW_WLC(hw), KM_HW_TKIP_RX_MIC_KEY_ADDR(hw, hw_idx),
			data, (int)(*data_len));
#else
		KM_HW_COPYFROM_SHM(KM_HW_WLC(hw), KM_HW_TKIP_TX_MIC_KEY_ADDR(hw, hw_idx),
			data, (int)(*data_len));
#endif /* WOWL */
		break;
	case WLC_KEY_DATA_TYPE_MIC_KEY_TO_DS:
		if (!hw->shm_info.tkip_mic_key_base ||
			(hw_idx >= hw->shm_info.max_tkip_mic_keys)) {
			err = BCME_UNSUPPORTED;
			break;
		}

		*data_len = TKIP_MIC_KEY_SIZE;
		if (data_size < *data_len) {
			err = BCME_BUFTOOSHORT;
			break;
		}

		/* no context to determine from/to AP - do something reasonable */
#ifdef WOWL
		KM_HW_COPYFROM_SHM(KM_HW_WLC(hw), KM_HW_TKIP_TX_MIC_KEY_ADDR(hw, hw_idx),
			data, (int)(*data_len));
#else
		KM_HW_COPYFROM_SHM(KM_HW_WLC(hw), KM_HW_TKIP_RX_MIC_KEY_ADDR(hw, hw_idx),
			data, (int)(*data_len));
#endif /* WOWL */
		break;
	case WLC_KEY_DATA_TYPE_TKHASH_P1:
		*data_len = TKHASH_P1_KEY_SIZE + sizeof(uint32);
		if (data_size < *data_len) {
			err = BCME_BUFTOOSHORT;
			break;
		}

		if (!KM_HW_TKIP_TSC_TTAK_SUPPORTED(hw, hw_idx)) {
			err = BCME_UNSUPPORTED;
			break;
		}

		KM_HW_COPYFROM_SHM(KM_HW_WLC(hw),
			KM_HW_TKIP_TSC_TTAK_ADDR(hw, hw_idx), data, (int)(*data_len));
		break;

	case WLC_KEY_DATA_TYPE_SEQ:
		*data_len = KEY_SEQ_SIZE;
		if (data_size < *data_len) {
			err = BCME_BUFTOOSHORT;
			break;
		}
#ifdef WOWL
		if (tx) {
			shm_addr_t shm_addr;
			if (!hw->shm_info.tx_pn_base) {
				err = BCME_UNSUPPORTED;
				break;
			}
			shm_addr = hw->shm_info.tx_pn_base + OFFSETOF(wowl_templ_ctxt_t, seciv) +
				TKHASH_P1_KEY_SIZE;
			KM_HW_COPYFROM_SHM(KM_HW_WLC(hw), shm_addr, data, KEY_SEQ_SIZE);
			KM_SWAP(uint8, data[0], data[1]); /* convert to LE - shm has TSC1.TSC0 */
			break;
		}
#endif /* WOWL */

		if (!hw->shm_info.rx_pn_base ||
			hw_idx >= hw->shm_info.max_rx_pn ||
			ins >= WLC_KEY_BASE_RX_SEQ) {
			err = BCME_UNSUPPORTED;
			break;
		}
		KM_HW_COPYFROM_SHM(KM_HW_WLC(hw), KM_HW_RX_SEQ_ADDR(hw, hw_idx, ins),
			data, (int)(*data_len));
		break;
	default:
		err = BCME_UNSUPPORTED;
		break;
	}

done:
	return err;
}
#endif /* !LINUX_CRYPTO */

#ifdef BCMWAPI_WPI
static int
sms4_read(km_hw_t *hw, void *ctx, hw_idx_t hw_idx,
    wlc_key_data_type_t data_type, int ins, bool tx, uint8 *data,
	size_t data_size, size_t *data_len)
{
	int err = BCME_OK;

	if (!data_len) {
		err = BCME_BADARG;
		goto done;
	}

	switch (data_type) {
	case WLC_KEY_DATA_TYPE_KEY:
		*data_len = SMS4_KEY_LEN;
		if (data_size < *data_len) {
			err = BCME_BUFTOOSHORT;
			break;
		}
		KM_HW_COPYFROM_SHM(KM_HW_WLC(hw), KM_HW_KEY_ADDR(hw, hw_idx),
			data, (int)(*data_len));
		break;
	case WLC_KEY_DATA_TYPE_MIC_KEY:
		if (!hw->shm_info.wapi_mic_key_base ||
			(hw_idx >= hw->shm_info.max_wapi_mic_keys)) {
			err = BCME_UNSUPPORTED;
			break;
		}

		*data_len = KM_HW_SMS4_MIC_KEY_SIZE;
		if (data_size < *data_len) {
			err = BCME_BUFTOOSHORT;
			break;
		}
		KM_HW_COPYFROM_SHM(KM_HW_WLC(hw),
			KM_HW_SMS4_MIC_KEY_ADDR(hw, hw_idx), data, (int)(*data_len));
		break;
	case WLC_KEY_DATA_TYPE_SEQ:
		(void)tx;
		/* fall through */
	default:
		err = BCME_UNSUPPORTED;
		break;
	}

done:
	return err;
}
#endif /* BCMWAPI_WPI */
#else
#define HW_READCB(_cb) NULL
#endif /* BCMDBG || BCMDBUG_DUMP || WOWL */

/* wep support, no support for seq/rxiv */
static const km_hw_dt_mask_t wep_dt_mask = KM_HW_DT_MASK(WLC_KEY_DATA_TYPE_KEY);
static const km_hw_dt_mask_t wep_wowl_dt_mask = KM_HW_DT_MASK(WLC_KEY_DATA_TYPE_KEY);

static int wep_write(km_hw_t *hw, void *ctx, hw_idx_t hw_idx, wlc_key_data_type_t dt,
	int ins, bool tx, const wlc_key_info_t *key_info, const uint8 *data, size_t data_len)
{
	int err = BCME_OK;
	BCM_REFERENCE(ctx);
	BCM_REFERENCE(ins);
	BCM_REFERENCE(key_info);
	switch (dt) {
	case WLC_KEY_DATA_TYPE_KEY:
		km_hw_algo_write_key(hw, KM_HW_KEY_ADDR(hw, hw_idx), data, data_len);
		break;
	default:
		(void)tx;
		err = BCME_UNSUPPORTED;
		break;
	}
	return err;
}

static const km_hw_algo_callbacks_t wep_callbacks = {
	HW_READCB(wep_read),
	wep_write,
	NULL
};

#define WEP_ALGO_IMPL_INIT(hw, key_algo, impl) HW_ALGO_IMPL_INIT(hw, impl, wep, key_algo)

/* aes support */
static const km_hw_dt_mask_t aes_dt_mask = KM_HW_DT_MASK(WLC_KEY_DATA_TYPE_KEY);
static const km_hw_dt_mask_t aes_wowl_dt_mask =
	KM_HW_DT_MASK2(WLC_KEY_DATA_TYPE_KEY, WLC_KEY_DATA_TYPE_SEQ);

static int aes_write(km_hw_t *hw, void *ctx, hw_idx_t hw_idx, wlc_key_data_type_t dt,
	int ins, bool tx, const wlc_key_info_t *key_info, const uint8 *data, size_t data_len)
{
	int err = BCME_OK;
	shm_addr_t shm_addr;
	BCM_REFERENCE(ctx);
	BCM_REFERENCE(key_info);

	switch (dt) {
	case WLC_KEY_DATA_TYPE_KEY:
		km_hw_algo_write_key(hw, KM_HW_KEY_ADDR(hw, hw_idx), data, data_len);
		break;

	case WLC_KEY_DATA_TYPE_SEQ:
		if (data_len != KEY_SEQ_SIZE) {
			err = BCME_BADARG;
			break;
		}

		if (tx) {
#if defined(WOWL)
			if (!hw->shm_info.tx_pn_base) {
				err = BCME_UNSUPPORTED;
				break;
			}
			shm_addr = hw->shm_info.tx_pn_base;
#ifdef WOWL
			shm_addr += OFFSETOF(wowl_templ_ctxt_t, seciv) + TKHASH_P1_KEY_SIZE;
#endif
			KM_HW_COPYTO_SHM(KM_HW_WLC(hw), shm_addr, data, KEY_SEQ_SIZE);
#endif /* WOWL */
			break;
		}

		if (!hw->shm_info.rx_pn_base ||
			hw_idx >= hw->shm_info.max_rx_pn ||
			ins >= WLC_KEY_BASE_RX_SEQ) {
			err = BCME_UNSUPPORTED;
			break;
		}
		shm_addr = KM_HW_RX_SEQ_ADDR(hw, hw_idx, ins);
		KM_HW_COPYTO_SHM(KM_HW_WLC(hw), shm_addr, data, (int)data_len);
		break;
	default:
		err = BCME_UNSUPPORTED;
		break;
	}
	return err;
}

static const km_hw_algo_callbacks_t aes_callbacks = {
	HW_READCB(aes_read),
	aes_write,
	NULL
};
#define AES_ALGO_IMPL_INIT(hw, key_algo, impl) HW_ALGO_IMPL_INIT(hw, impl, aes, key_algo)

/* tkip support */
#ifndef LINUX_CRYPTO
static const km_hw_dt_mask_t tkip_dt_mask =
	KM_HW_DT_MASK4(WLC_KEY_DATA_TYPE_TKIP_TK, WLC_KEY_DATA_TYPE_TKHASH_P1,
	WLC_KEY_DATA_TYPE_MIC_KEY_FROM_DS, WLC_KEY_DATA_TYPE_MIC_KEY_TO_DS);

static const km_hw_dt_mask_t tkip_wowl_dt_mask =
	KM_HW_DT_MASK4(WLC_KEY_DATA_TYPE_TKIP_TK, WLC_KEY_DATA_TYPE_TKHASH_P1,
	WLC_KEY_DATA_TYPE_MIC_KEY_FROM_DS, WLC_KEY_DATA_TYPE_MIC_KEY_TO_DS) |
	KM_HW_DT_MASK(WLC_KEY_DATA_TYPE_SEQ);

static int
tkip_write(km_hw_t *hw, void *ctx, hw_idx_t hw_idx, wlc_key_data_type_t data_type,
	int ins, bool tx, const wlc_key_info_t *key_info, const uint8 *data, size_t data_len)
{
	int err = BCME_OK;
	shm_addr_t shm_addr;
	BCM_REFERENCE(ctx);

	switch (data_type) {
	case WLC_KEY_DATA_TYPE_TKIP_TK:
		shm_addr = KM_HW_KEY_ADDR(hw, hw_idx);
		km_hw_algo_write_key(hw, shm_addr, data, data_len);
		break;
	case WLC_KEY_DATA_TYPE_MIC_KEY_FROM_DS:
		if (!hw->shm_info.tkip_mic_key_base ||
			(hw_idx >= hw->shm_info.max_tkip_mic_keys)) {
			err = BCME_UNSUPPORTED;
			break;
		}

		if (WLC_KEY_IS_AP(key_info) || WLC_KEY_IS_IBSS(key_info))
			shm_addr = KM_HW_TKIP_TX_MIC_KEY_ADDR(hw, hw_idx);
		else
			shm_addr = KM_HW_TKIP_RX_MIC_KEY_ADDR(hw, hw_idx);

		KM_ASSERT(data_len <= TKIP_MIC_KEY_SIZE);
		KM_HW_COPYTO_SHM(KM_HW_WLC(hw), shm_addr, data, (int)data_len);
		break;
	case WLC_KEY_DATA_TYPE_MIC_KEY_TO_DS:
		if (!hw->shm_info.tkip_mic_key_base ||
			(hw_idx >= hw->shm_info.max_tkip_mic_keys)) {
			err = BCME_UNSUPPORTED;
			break;
		}

		if (WLC_KEY_IS_AP(key_info) || WLC_KEY_IS_IBSS(key_info))
			shm_addr = KM_HW_TKIP_RX_MIC_KEY_ADDR(hw, hw_idx);
		else
			shm_addr = KM_HW_TKIP_TX_MIC_KEY_ADDR(hw, hw_idx);

		KM_ASSERT(data_len <= TKIP_MIC_KEY_SIZE);
		KM_HW_COPYTO_SHM(KM_HW_WLC(hw), shm_addr, data, (int)data_len);
		break;
	case WLC_KEY_DATA_TYPE_TKHASH_P1:
		if (!KM_HW_TKIP_TSC_TTAK_SUPPORTED(hw, hw_idx)) {
			err = BCME_UNSUPPORTED;
			break;
		}

		if (tx) {
#if defined(WOWL)
			if (!hw->shm_info.tx_pn_base) {
				err = BCME_UNSUPPORTED;
				break;
			}
			shm_addr = hw->shm_info.tx_pn_base +  OFFSETOF(wowl_templ_ctxt_t, seciv);
			KM_HW_COPYTO_SHM(KM_HW_WLC(hw), shm_addr, data, TKHASH_P1_KEY_SIZE);
#endif /* WOWL  */
			break;
		}

		shm_addr = KM_HW_TKIP_TSC_TTAK_ADDR(hw, hw_idx);

		KM_DBG_ASSERT(data_len == (TKHASH_P1_KEY_SIZE + sizeof(uint32)));

		KM_HW_COPYTO_SHM(KM_HW_WLC(hw), shm_addr, data, (int)data_len - sizeof(uint32));
		shm_addr += TKHASH_P1_KEY_SIZE;
		KM_HW_WRITE_SHM(KM_HW_WLC(hw), shm_addr, ltoh16_ua(data + TKHASH_P1_KEY_SIZE));
		shm_addr += sizeof(uint16);
		KM_HW_WRITE_SHM(KM_HW_WLC(hw), shm_addr,
			ltoh16_ua(data + TKHASH_P1_KEY_SIZE + sizeof(uint16)));
		break;
	case WLC_KEY_DATA_TYPE_SEQ:
		if (data_len != KEY_SEQ_SIZE) {
			err = BCME_BADARG;
			break;
		}
		if (tx) {
#if defined(WOWL)
			uint8 tx_seq[KEY_SEQ_SIZE];
			if (!hw->shm_info.tx_pn_base) {
				err = BCME_UNSUPPORTED;
				break;
			}
			shm_addr = hw->shm_info.tx_pn_base +  OFFSETOF(wowl_templ_ctxt_t, seciv)
				+ TKHASH_P1_KEY_SIZE;
			memcpy(tx_seq, data, KEY_SEQ_SIZE);
			KM_SWAP(uint8, tx_seq[0], tx_seq[1]); /* tsc1.ts0 in shm */
			KM_HW_COPYTO_SHM(KM_HW_WLC(hw), shm_addr, tx_seq, KEY_SEQ_SIZE);
#endif /* WOWL */
			break;
		}
		if (!hw->shm_info.rx_pn_base ||
			hw_idx >= hw->shm_info.max_rx_pn ||
			ins >= WLC_KEY_BASE_RX_SEQ) {
			err = BCME_UNSUPPORTED;
			break;
		}
		KM_HW_COPYTO_SHM(KM_HW_WLC(hw), KM_HW_RX_SEQ_ADDR(hw, hw_idx, ins),
			data, (int)data_len);
		break;
	default:
		err = BCME_UNSUPPORTED;
		break;
	}

	return err;
}

static const km_hw_algo_callbacks_t tkip_callbacks = {
	HW_READCB(tkip_read),
	tkip_write,
	NULL
};
#define TKIP_ALGO_IMPL_INIT(hw, key_algo, impl) HW_ALGO_IMPL_INIT(hw, impl, tkip, key_algo)
#else
#define TKIP_ALGO_IMPL_INIT(hw, key_algo, impl)
#endif /* !LINUX_CRYPTO */

/* sms4 support */
#ifdef BCMWAPI_WPI
static int
sms4_write(km_hw_t *hw, void *ctx, hw_idx_t hw_idx, wlc_key_data_type_t data_type,
	int ins, bool tx, const wlc_key_info_t *key_info, const uint8 *data, size_t data_len)
{
	shm_addr_t shm_addr;
	int err = BCME_OK;

	switch (data_type) {
	case WLC_KEY_DATA_TYPE_KEY:
		shm_addr = KM_HW_KEY_ADDR(hw, hw_idx);
		km_hw_algo_write_key(hw, shm_addr, data, data_len);
		break;
	case WLC_KEY_DATA_TYPE_MIC_KEY:
		if (!hw->shm_info.wapi_mic_key_base ||
			(hw_idx >= hw->shm_info.max_wapi_mic_keys)) {
			err = BCME_UNSUPPORTED;
			break;
		}

		shm_addr = KM_HW_SMS4_MIC_KEY_ADDR(hw, hw_idx);

		KM_ASSERT(data_len == KM_HW_SMS4_MIC_KEY_SIZE);
		KM_HW_COPYTO_SHM(KM_HW_WLC(hw), shm_addr, data, (int)data_len);
		break;

	case WLC_KEY_DATA_TYPE_SEQ:
		(void)tx;
		/* fall through */
	default:
		err = BCME_UNSUPPORTED;
		break;
	}

	return err;
}

static const km_hw_dt_mask_t sms4_dt_mask =
	KM_HW_DT_MASK2(WLC_KEY_DATA_TYPE_KEY, WLC_KEY_DATA_TYPE_MIC_KEY);
static const km_hw_dt_mask_t sms4_wowl_dt_mask =  0; /* not supported */

static const km_hw_algo_callbacks_t sms4_callbacks = {
	HW_READCB(sms4_read),
	sms4_write,
	NULL
};
#define SMS4_ALGO_IMPL_INIT(hw, key_algo, impl) HW_ALGO_IMPL_INIT(hw, impl, sms4, key_algo)
#else
#define SMS4_ALGO_IMPL_INIT(hw, key_algo, impl)
#endif /* BCMWAPI_WPI */

/* public interface */
int
km_hw_algo_init(const km_hw_t *hw, key_algo_t algo, km_hw_algo_impl_t *impl)
{
	KM_DBG_ASSERT(impl != NULL);

	memset(impl, 0, sizeof(*impl));

	switch (algo) {
	case CRYPTO_ALGO_WEP1:
		/* fall through */
	case CRYPTO_ALGO_WEP128:
		/* fall through */
		WEP_ALGO_IMPL_INIT(hw, algo, impl);
		break;
	case CRYPTO_ALGO_TKIP:
		TKIP_ALGO_IMPL_INIT(hw, algo, impl);
		break;
	case CRYPTO_ALGO_AES_CCM:
		/* fall through */
	case CRYPTO_ALGO_AES_GCM:
		/* fall through */
	case CRYPTO_ALGO_AES_GCM256:
		/* fall through */
	case CRYPTO_ALGO_AES_OCB_MSDU:
		/* fall through */
	case CRYPTO_ALGO_AES_OCB_MPDU:
		AES_ALGO_IMPL_INIT(hw, algo, impl);
		break;
#ifdef BCMWAPI_WPI
	case CRYPTO_ALGO_SMS4:
		SMS4_ALGO_IMPL_INIT(hw, algo, impl);
		break;
#endif /* BCMWAPI_WPI */

	default:
		break;
	}

	return (impl->cb == NULL) ?  BCME_UNSUPPORTED : BCME_OK;
}

const km_hw_algo_entry_t*
km_hw_find_algo_entry(const km_hw_t *hw, key_algo_t algo)
{
	int i;

	KM_DBG_ASSERT(KM_HW_VALID(hw));

	for (i = 0; i < hw->impl.num_algo_entries; ++i) {
		const km_hw_algo_entry_t *ae = &hw->impl.algo_entries[i];
		if (ae->algo == algo && ae->impl.cb != NULL &&
			ae->impl.hw_algo != WSEC_ALGO_OFF && ae->impl.dt_mask != 0) {
			return ae;
		}
	}
	return NULL;
}

void
km_hw_algo_destroy_algo_entries(km_hw_t *hw)
{
	int i;
	int err;

	KM_DBG_ASSERT(KM_HW_VALID(hw));

	for (i = 0; i < hw->impl.num_algo_entries; ++i) {
		km_hw_algo_entry_t *ae = & hw->impl.algo_entries[i];
		if (ae->impl.cb != NULL && ae->impl.cb->destroy != NULL) {
			err = (*(ae->impl.cb->destroy))(hw, &ae->impl.cb_ctx);
			if (err != BCME_OK) {
				KM_HW_ERR(("wl%d: %s: error %d destroying algo %d\n",
					WLCWLUNIT(hw->wlc), __FUNCTION__, err, ae->algo));
				/* go on, destroy the rest */
			}
		}
		memset(&ae->impl, 0, sizeof(ae->impl));
	}
}

/* update hw key data - currently rx data only */
int
km_hw_algo_set_hw_key(km_hw_t *hw, hw_idx_t hw_idx, const km_hw_algo_entry_t *ae,
	km_hw_dt_mask_t dt_mask, wlc_key_t *key, const wlc_key_info_t *key_info)
{
	int i;
	int err = BCME_OK;
	uint8 data[KM_HW_MAX_DATA_LEN];
	size_t data_len;
	wlc_key_data_type_t dt;

	KM_HW_LOG(("wl%d: %s: hw idx 0x%04x  key idx 0x%04x mask 0x%08x algo %d[%s]\n",
		KM_HW_UNIT(hw), __FUNCTION__, hw_idx, key_info->key_idx, dt_mask,
		ae->algo, wlc_keymgmt_get_algo_name(KM_HW_KM(hw), ae->algo)));

	/* iterate through data types and update hw */
	for (i = 0; ((uint)i < KM_SIZE_BITS(dt_mask)) && (dt_mask != 0) && (err == BCME_OK);
		++i, dt_mask >>= 1) {
		int tx;

		if (!(dt_mask & 0x1))
			continue;

		dt = (wlc_key_data_type_t)i;

		KM_HW_LOG(("wl%d: %s: updating key data for type %d[%s]\n",
			KM_HW_UNIT(hw), __FUNCTION__, dt, wlc_key_get_data_type_name(dt)));

		/*
		 * P1 & TSC key for rev80 has to be written in shm.
		 * This loop is introduced mainly for
		 * D11REV_80 && dt == WLC_KEY_DATA_TYPE_TKHASH_P1
		 * so that both tx & rx can be written in shm.
		 */
		for (tx = 0; tx < 2; tx++) {
			err = wlc_key_get_data_ex(key, data, sizeof(data), &data_len,
					dt, 0, tx != 0);
			if (err != BCME_OK)
				break;

			KM_ASSERT(ae->impl.cb->write != NULL);

			/* shm writes requires even number of bytes */
			if (data_len & 0x1)
				data[data_len++] = 0;

			err = ae->impl.cb->write(hw, ae->impl.cb_ctx, hw_idx, dt, 0, tx != 0,
					key_info, data, data_len);

			if (err != BCME_UNSUPPORTED && err != BCME_OK)
				break;

			/* tx is supported only for corerev GE80 */
			if (!KM_HW_COREREV_GE80(hw) ||
				(dt != WLC_KEY_DATA_TYPE_TKHASH_P1))
				break;
		}

		if (err == BCME_UNSUPPORTED) {
			err = BCME_OK;
			continue;
		}

		if (err != BCME_OK)
			break;

		/* if this is key seq, update the rest of rx iv/seq counters */
		if (dt == WLC_KEY_DATA_TYPE_SEQ) {
			int j;
			for (j = 1; j < KM_HW_KEY_NUM_RX_SEQ(hw); ++j) {
				err = wlc_key_get_data_ex(key, data, sizeof(data), &data_len,
					dt, j, FALSE);
				if (err == BCME_UNSUPPORTED)
					err = BCME_OK;
				else if (err != BCME_OK)
					break;
				else {
					if (data_len & 0x1)
						data[data_len++] = 0;

					err = ae->impl.cb->write(hw, ae->impl.cb_ctx, hw_idx,
						dt, j, FALSE, key_info, data, data_len);
					if (err != BCME_OK)
						break;
				}
			}
		}

#if defined(WOWL)
		/* handle tx seq and tkip phase1 key update */
		if (KM_HW_WOWL_SUPPORTED(hw) && WLC_KEY_IS_PAIRWISE(key_info)) {
			if (dt == WLC_KEY_DATA_TYPE_SEQ || dt == WLC_KEY_DATA_TYPE_TKHASH_P1) {
				err = wlc_key_get_data_ex(key, data, sizeof(data),
					&data_len, dt, 0, TRUE);
				if (err == BCME_UNSUPPORTED)
					err = BCME_OK;
				else if (err != BCME_OK)
					break;
				else {
					if (data_len & 0x1)
						data[data_len++] = 0;

					err = ae->impl.cb->write(hw, ae->impl.cb_ctx, hw_idx,
						dt, 0, TRUE, key_info, data, data_len);
					if (err != BCME_OK)
						break;
				}
			}
		}
#endif /* WOWL */
	}

	KM_HW_LOG(("wl%d: %s: done with status %d\n",  KM_HW_UNIT(hw), __FUNCTION__, err));
	return err;
}

#ifdef WOWL
int
km_hw_algo_update_sw_key(km_hw_t *hw, hw_idx_t hw_idx, const km_hw_algo_entry_t *ae,
    km_hw_dt_mask_t dt_mask, wlc_key_t *key, const wlc_key_info_t *key_info)
{
	int i;
	uint8 data[KM_HW_MAX_DATA_LEN];
	size_t data_len;
	wlc_key_data_type_t dt;
	int err = BCME_OK;

	KM_HW_LOG(("wl%d: %s: hw idx 0x%04x  key idx 0x%04x mask 0x%08x algo %d[%s]\n",
		KM_HW_UNIT(hw), __FUNCTION__, hw_idx, key_info->key_idx, dt_mask,
		ae->algo, wlc_keymgmt_get_algo_name(KM_HW_KM(hw), ae->algo)));

	/* iterate through data types and update sw */
	for (i = 0; ((uint)i < KM_SIZE_BITS(dt_mask)) && (dt_mask != 0) &&
		(err == BCME_OK); ++i, dt_mask >>= 1) {

		if (!(dt_mask & 0x1))
			continue;

		KM_DBG_ASSERT(ae->impl.cb->read != NULL);

		dt = (wlc_key_data_type_t)i;

		KM_HW_LOG(("wl%d: %s: updating key data for type %d[%s]\n",
			KM_HW_UNIT(hw), __FUNCTION__, dt, wlc_key_get_data_type_name(dt)));

		err  = ae->impl.cb->read(hw, ae->impl.cb_ctx, hw_idx, dt, 0, FALSE, data,
			sizeof(data), &data_len);
		if (err != BCME_OK)
			break;

		if (dt == WLC_KEY_DATA_TYPE_SEQ)
			err = wlc_key_advance_seq(key, data, data_len, 0, FALSE);
		else
			err = wlc_key_set_data_ex(key, data, data_len, dt, 0, FALSE);

		if (err == BCME_UNSUPPORTED) {
			err = BCME_OK;
			continue;
		}
		if (err != BCME_OK)
			break;

		/* if this is rx seq, update the rest of rx iv/seq counters */
		if (dt == WLC_KEY_DATA_TYPE_SEQ) {
			int j;
			for (j = 1; j < KM_HW_KEY_NUM_RX_SEQ(hw); ++j) {
				err = ae->impl.cb->read(hw, ae->impl.cb_ctx, hw_idx, dt, j, FALSE,
					data, sizeof(data), &data_len);
				if (err != BCME_OK)
					break;
				err = wlc_key_advance_seq(key, data, data_len,
					(wlc_key_seq_id_t)j, FALSE);
				if (err != BCME_OK)
					break;
			}

			/* handle tx seq update - phase1 key adjusted by wlc_key impl */
			if (KM_HW_WOWL_SUPPORTED(hw) && WLC_KEY_IS_PAIRWISE(key_info)) {
				err = ae->impl.cb->read(hw, ae->impl.cb_ctx, hw_idx, dt, j, TRUE,
					data, sizeof(data), &data_len);
				if (err != BCME_OK)
					break;
				err = wlc_key_advance_seq(key, data, data_len,
					(wlc_key_seq_id_t)j, TRUE);
				if (err != BCME_OK)
					break;
			}
		}
	}

	KM_HW_LOG(("wl%d: %s: done with status %d\n",  KM_HW_UNIT(hw), __FUNCTION__, err));
	return err;
}
#endif /* WOWL */

/* Note: hw algo to algo mapping is not necessarily 1-1 */
wlc_key_algo_t
km_hw_hw_algo_to_algo(const km_hw_t *hw, wlc_key_hw_algo_t algo)
{
	int i = 0;
	for (i = 0; i < hw->impl.num_algo_entries; ++i) {
		const km_hw_algo_entry_t *ae = &hw->impl.algo_entries[i];
		if (ae->impl.hw_algo == algo)
			return ae->algo;
	}
	return BCME_ERROR;
}

hw_algo_t
km_hw_algo_to_hw_algo(const km_hw_t *hw, wlc_key_algo_t algo)
{
	bool rev40plus, rev61plus;

	KM_DBG_ASSERT(KM_HW_WLC(hw) != NULL);

	rev40plus  = KM_HW_COREREV_GE40(hw);
	rev61plus  = KM_HW_COREREV_GE61(hw);
	switch (algo) {
	case CRYPTO_ALGO_WEP1:
		return WSEC_ALGO_WEP1;

	case CRYPTO_ALGO_WEP128:
		return rev40plus ? WSEC_ALGO_WEP128 : D11_PRE40_WSEC_ALGO_WEP128;

	case CRYPTO_ALGO_TKIP:
		return WSEC_ALGO_TKIP;

	case CRYPTO_ALGO_AES_CCM:
		return rev40plus ? WSEC_ALGO_AES : D11_PRE40_WSEC_ALGO_AES;
	case CRYPTO_ALGO_AES_GCM:
		return rev61plus ? WSEC_ALGO_GCM : WSEC_ALGO_OFF;
	case CRYPTO_ALGO_AES_GCM256:
		return rev61plus ? WSEC_ALGO_GCM256 : WSEC_ALGO_OFF;

	case CRYPTO_ALGO_AES_OCB_MSDU:
		/* fall through */
	case CRYPTO_ALGO_AES_OCB_MPDU:
		return rev40plus ? WSEC_ALGO_WEP128 : D11_PRE40_WSEC_ALGO_WEP128;

	case CRYPTO_ALGO_NALG:
		return rev40plus ? WSEC_ALGO_NALG : D11_PRE40_WSEC_ALGO_NALG;

#ifdef BCMWAPI_WPI
	case CRYPTO_ALGO_SMS4:
		return rev40plus ? WSEC_ALGO_SMS4_DFT_2005_09_07 : D11_PRE40_WSEC_ALGO_SMS4;
#endif /* BCMWAPI_WPI */

	default:
		return WSEC_ALGO_OFF;
	}
}
